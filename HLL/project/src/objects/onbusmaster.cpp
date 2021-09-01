#include "onbusmaster.h"

#include "responsepacket.h"

#include <QTime>
#include <QQueue>
#include <QDebug>

QT_USE_NAMESPACE

OnBusMaster::OnBusMaster(QObject *parent) :
    QThread(parent),
    m_waitTimeout(0),
    m_quit(false),
    m_state(UnconnectedState)
{
    qRegisterMetaType<OnBusMaster::Error>();
    qRegisterMetaType<OnBusMaster::State>();
}

OnBusMaster::~OnBusMaster()
{
    m_mutex.lock();
    m_quit = true;
    m_cond.wakeOne();
    m_mutex.unlock();
    wait();
}

/**
 * @brief MasterThread::setConfig
 * @param portName
 * @param baudRate
 * @param parity
 * @param dataBits
 * @param stopBits
 * @param waitTimeout
 * @return Return true if thread not running else return false.
 */
bool OnBusMaster::setConfig(const QString &portName, QSerialPort::BaudRate baudRate, QSerialPort::Parity parity, QSerialPort::DataBits dataBits, QSerialPort::StopBits stopBits, int waitTimeout)
{
    QMutexLocker locker(&m_mutex);

    if (isRunning())
        return false;

    m_quit = false;
    m_portName = portName;
    m_baudRate = baudRate;
    m_parity = parity;
    m_dataBits = dataBits;
    m_stopBits = stopBits;
    m_waitTimeout = waitTimeout;

    return true;
}

void OnBusMaster::openPort()
{
    if (isRunning())
        return;

    m_quit = false;
    start();
}

void OnBusMaster::closePort()
{
    QMutexLocker locker(&m_mutex);
    m_quit = true;
    m_cond.wakeOne();
}

OnBusMaster::State OnBusMaster::state() const
{
    return m_state;
}

void OnBusMaster::setState(const State &state)
{
    m_state = state;
    emit stateChanged(m_state);
}

void OnBusMaster::transaction(const QByteArray &data)
{
    QMutexLocker locker(&m_mutex);
    if (isRunning() && !data.isEmpty() ) {
        m_transactionData = data;
        m_cond.wakeOne();
    }
}

quint64 OnBusMaster::elapsed(const QElapsedTimer &timer)
{
    quint64 e = timer.elapsed();
    qDebug() << "Elapsed:" <<e;
    return e;
}

void OnBusMaster::run()
{
    m_mutex.lock();

    QString currentPortName = m_portName;
    int timeout = m_waitTimeout;
    QQueue<QByteArray> reqQueue;
    if (!m_transactionData.isEmpty()) {
        reqQueue.enqueue(m_transactionData);
        m_transactionData.clear();
    }

    // Burada thread in kilidini aciyorum.
    m_mutex.unlock();

    setState(OnBusMaster::ConnectingState);
    QSerialPort serial;    
    serial.setPortName(currentPortName);

    if (!serial.open(QIODevice::ReadWrite)) {
        emit errorOccurred(OnBusMaster::ConnectionError);
        setState(OnBusMaster::UnconnectedState);
        return;
    }

    bool configStatus = false;
    configStatus = serial.setParity(QSerialPort::NoParity);
    configStatus &= serial.setBaudRate(QSerialPort::Baud115200);
    configStatus &= serial.setDataBits(QSerialPort::Data8);
    configStatus &= serial.setStopBits(QSerialPort::OneStop);

    if (!configStatus) {
        setState(OnBusMaster::ClosingState);
        emit errorOccurred(OnBusMaster::ConfigurationError);
        serial.close();
        setState(OnBusMaster::UnconnectedState);
        return;
    }

    serial.flush();
    serial.clear(QSerialPort::AllDirections);

    // Thread i kitliyor ve transaction i beklemeye basliyorum.
    m_mutex.lock();
    setState(OnBusMaster::ConnectedState);
    m_cond.wait(&m_mutex); // Port islemleri yapildi ve transaction bekleniyor!
    m_mutex.unlock();

    QByteArray requestData;
    quint64 counter = 0;

    while (!m_quit) {
        // write request
        if (!reqQueue.isEmpty()) {
            int retry = 0;
            QElapsedTimer writeTimer;

            requestData = reqQueue.dequeue(); // Sorgu kuyrugunda ki ilk elemani aliyorum.
            writeTimer.start();

            do {
                qint64 bytesWritten = 0;
                for (int i=0; i<requestData.size(); i++) {
                    char c = requestData.at(i);
                    if (serial.write(&c, 1) == 1) {
                        if (serial.waitForBytesWritten(10)) {
                            bytesWritten++;
                        } else {
                            qDebug() << "OnBusMaster::WriteTimeoutError";
                            emit this->errorOccurred(OnBusMaster::WriteTimeoutError);
                        }
                    } else {
                        qDebug() << "OnBusMaster::WriteError";
                        emit this->errorOccurred(OnBusMaster::WriteError);
                    }

                    usleep(1);
                }
                qint64 elapsed = writeTimer.elapsed();
//                serial.flush();
                qDebug() << QString("--------Retry %1---------").arg(retry);
                if (bytesWritten == requestData.size()) {
                    qDebug() << elapsed << "ms de yazdim..";
                    break;
                } else {
                    qDebug() << elapsed << "ms de yazamadim..";
                    qDebug() << "OnBusMaster::WriteError";
                    emit this->errorOccurred(OnBusMaster::WriteError);
                }
            } while (++retry < 3 && writeTimer.elapsed() < 30000);
        }

        // read data
        {
            QByteArray rawData;
            do {
                if (serial.waitForReadyRead(timeout)) {
                    QByteArray tmp = serial.readAll();
                    if (!tmp.isEmpty())
                        rawData.append(tmp);
                    else
                        continue;

                    const int idxOfFirstSOP = rawData.indexOf(ResponsePacket::SOP);
                    if (idxOfFirstSOP == -1) { // Okunan veride [AA] yoksa okunan veriyi sil ve yeniden oku.
                        rawData.clear();
                        continue;
                    } else {
                        rawData = rawData.mid(idxOfFirstSOP);
                        if (rawData.length() < 6)
                            continue;
                        else { // Verinin 1. elemani uygun fonksiyon numarasi iceriyor mu
                            if (rawData.at(ResponsePacket::ADR_FNC) >= ResponsePacket::FC_WRITE_MEM && rawData.at(ResponsePacket::ADR_FNC) <= ResponsePacket::FC_CONNECT) {
                                int dataLength = rawData.at(ResponsePacket::ADR_LEN);
                                int estimatedPacketLen = dataLength + 2 + 2 +1;

                                if (rawData.length() < estimatedPacketLen) // rawData nin uzunlugu beklenen paket uzunlugundan kisa ise.
                                    continue;
                                else {
                                    if (rawData.at(estimatedPacketLen - 1) == ResponsePacket::EOP) { // beklenen paket sonu [55] ise [AA] ile baslayip [55] ile biten uygun bir paket bulundu demektir.
                                        QByteArray packet = rawData.mid(0, estimatedPacketLen);

                                        ResponsePacket rP(requestData, packet);
                                        qDebug() << QString(rP.request().toHex(':').toUpper());
                                        qDebug() << QString(rP.response().toHex(':').toUpper());
                                        qDebug() << "Is Valid:" << rP.isValid() << rP.status();
                                        qDebug() << "counter:" << ++counter;
                                    }

                                    rawData = rawData.mid(estimatedPacketLen); // uygun paketin bulunup bulunmadigina bakilmaksizin, rawData beklenen paket boyutu kadar kaydirilir.
                                }
                            } else { // Uygun fonksiyon numarasi yoksa fonksiyon adresinden bir sonraki adrese kadar sil
                                rawData = rawData.mid(ResponsePacket::ADR_FNC);
                                continue;
                            }
                        }
                    }
                } else {
//                    qDebug() << "OnBusMaster::ReadTimeoutError";
                    rawData.clear();
                    emit this->errorOccurred(OnBusMaster::ReadTimeoutError);
                }
            } while (rawData.length() > 0 );

            if (!rawData.trimmed().isEmpty())
                qDebug() << "KALAN DATA:" << rawData.toHex(':').toUpper();
        }

        // Thread i kitliyor ve varsa yapilan degisiklikleri while dongusunun icine aliyorum.
        m_mutex.lock();
        if (timeout != m_waitTimeout)
            timeout = m_waitTimeout;
        if (!m_transactionData.isEmpty()) {
            reqQueue.enqueue(m_transactionData);
            m_transactionData.clear();
        }
        m_mutex.unlock();
    }

    setState(OnBusMaster::ClosingState);
    serial.close();
    setState(OnBusMaster::UnconnectedState);
}
