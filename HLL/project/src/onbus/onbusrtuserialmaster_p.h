#ifndef ONBUSRTUSERIALMASTER_P_H
#define ONBUSRTUSERIALMASTER_P_H

#include <QLoggingCategory>
#include <QtMath>
#include <QPointer>
#include <QQueue>
#include <QTimer>
#include <QtSerialPort>
#include <QDebug>

#include "onbusrtuserialmaster.h"
#include "onbusadu_p.h"
#include "onbusmaster_p.h"

Q_DECLARE_LOGGING_CATEGORY(QT_MODBUS)
Q_DECLARE_LOGGING_CATEGORY(QT_MODBUS_LOW)

class Timer : public QObject
{
    Q_OBJECT

public:
    Timer() = default;
    int start(int msec)
    {
        m_timer = QBasicTimer();
        m_timer.start(msec, Qt::PreciseTimer, this);
        return m_timer.timerId();
    }
    void stop() { m_timer.stop(); }
    bool isActive() const { return m_timer.isActive(); }

signals:
    void timeout(int timerId);

private:
    void timerEvent(QTimerEvent *event) override
    {
        const auto id = m_timer.timerId();
        if (event->timerId() == id)
            emit timeout(id);
    }

private:
    QBasicTimer m_timer;
};

class OnBusRtuSerialMasterPrivate : public OnBusMasterPrivate
{
    Q_DECLARE_PUBLIC(OnBusRtuSerialMaster)
        enum State
    {
        Idle,
        WaitingForReplay,
        ProcessReply
    } m_state = Idle;

public:
    void onReadyRead()
    {
        m_responseBuffer += m_serialPort->read(m_serialPort->bytesAvailable());
        qDebug() << "(RTU client) Response buffer:" << m_responseBuffer.toHex(':').toUpper();
//        qCDebug(QT_MODBUS_LOW) << "(RTU client) Response buffer:" << m_responseBuffer.toHex();

        if (m_responseBuffer.size() < 2) {
            qCDebug(QT_MODBUS) << "(RTU client) ADU not complete";
            return;
        }

        const int idxOfStart = m_responseBuffer.indexOf(OnBusPdu::Start);

        if (idxOfStart == -1) { // Pakette AA yoksa
            qCDebug(QT_MODBUS) << "(RTU client) ADU not start";
            m_responseBuffer.clear();
            return;
        }

        if (idxOfStart > 0) // Bulunan ilk AA paketin ilk elemani degil ise AA'ya kadar olan kisim paketten silinir
            m_responseBuffer = m_responseBuffer.mid(idxOfStart);

        if (m_responseBuffer.size() < 2) {
            qCDebug(QT_MODBUS) << "(RTU client) ADU not complete";
            return;
        }

        //----------------------------------------------------------------------
        const OnBusSerialAdu tmpAdu(OnBusSerialAdu::Onbus, m_responseBuffer);
        int pduSizeWithoutFcode = OnBusResponse::calculateDataSize(tmpAdu.pdu());
        if (pduSizeWithoutFcode < 0) {
            // wait for more data
            qCDebug(QT_MODBUS) << "(RTU client) Cannot calculate PDU size for function code:"
                << tmpAdu.pdu().functionCode() << ", delaying pending frame";
            return;
        }

        // server address byte + function code byte + PDU size + 2 bytes CRC
        int aduSize = 2 + pduSizeWithoutFcode + 2;
        if (tmpAdu.rawSize() < aduSize) {
            qCDebug(QT_MODBUS) << "(RTU client) Incomplete ADU received, ignoring";
            return;
        }

        if (m_queue.isEmpty())
            return;
        auto &current = m_queue.first();

        // Special case for Diagnostics:ReturnQueryData. The response has no
        // length indicator and is just a simple echo of what we have send.
        /* Kapattim
          if (tmpAdu.pdu().functionCode() == OnBusPdu::Diagnostics) {
            const OnBusResponse response = tmpAdu.pdu();
            if (canMatchRequestAndResponse(response, tmpAdu.serverAddress())) {
                quint16 subCode = 0xffff;
                response.decodeData(&subCode);
                if (subCode == Diagnostics::ReturnQueryData) {
                    if (response.data() != current.requestPdu.data())
                        return; // echo does not match request yet
                    aduSize = 2 + response.dataSize() + 2;
                    if (tmpAdu.rawSize() < aduSize)
                        return; // echo matches, probably checksum missing
                }
            }
        }*/

        const OnBusSerialAdu adu(OnBusSerialAdu::Rtu, m_responseBuffer.left(aduSize));
        m_responseBuffer.remove(0, aduSize);

        qCDebug(QT_MODBUS) << "(RTU client) Received ADU:" << adu.rawData().toHex();
        if (QT_MODBUS().isDebugEnabled() && !m_responseBuffer.isEmpty())
            qCDebug(QT_MODBUS_LOW) << "(RTU client) Pending buffer:" << m_responseBuffer.toHex();

        // check CRC
        if (!adu.matchingChecksum()) {
            qCWarning(QT_MODBUS) << "(RTU client) Discarding response with wrong CRC, received:"
                << adu.checksum<quint16>() << ", calculated CRC:"
                << OnBusSerialAdu::calculateCRC(adu.data(), adu.size());
            return;
        }

        const OnBusResponse response = adu.pdu();
        if (!canMatchRequestAndResponse(response)) {
            qCWarning(QT_MODBUS) << "(RTU client) Cannot match response with open request, "
                "ignoring";
            return;
        }

        m_state = ProcessReply;
        m_responseTimer.stop();
        current.m_timerId = INT_MIN;

        processQueueElement(response, m_queue.dequeue());

        m_state = Idle;
        scheduleNextRequest(m_interFrameDelayMilliseconds);
    }

    void onAboutToClose()
    {
        Q_Q(OnBusRtuSerialMaster);
        Q_UNUSED(q) // avoid warning in release mode
        Q_ASSERT(q->state() == OnBusDevice::ClosingState);

        m_responseTimer.stop();
    }

    void onResponseTimeout(int timerId)
    {
        m_responseTimer.stop();
        if (m_state != State::WaitingForReplay || m_queue.isEmpty())
            return;
        const auto current = m_queue.first();

        if (current.m_timerId != timerId)
            return;

        qCDebug(QT_MODBUS) << "(RTU client) Receive timeout:" << current.requestPdu;

        if (current.numberOfRetries <= 0) {
            auto item = m_queue.dequeue();
            if (item.reply) {
                item.reply->setError(OnBusDevice::TimeoutError,
                    OnBusMaster::tr("Request timeout."));
            }
        }

        m_state = Idle;
        scheduleNextRequest(m_interFrameDelayMilliseconds);
    }

    void onBytesWritten(qint64 bytes)
    {
        if (m_queue.isEmpty())
            return;
        auto &current = m_queue.first();

        current.bytesWritten += bytes;
        if (current.bytesWritten != current.adu.size())
            return;

        qCDebug(QT_MODBUS) << "(RTU client) Send successful:" << current.requestPdu;

        if (!current.reply.isNull() && current.reply->type() == OnBusReply::Broadcast) {
            m_state = ProcessReply;
            processQueueElement({}, m_queue.dequeue());
            m_state = Idle;
            scheduleNextRequest(m_turnaroundDelay);
        } else {
            current.m_timerId = m_responseTimer.start(m_responseTimeoutDuration);
        }
    }

    void onError(QSerialPort::SerialPortError error)
    {
        if (error == QSerialPort::NoError)
            return;

        qCDebug(QT_MODBUS) << "(RTU server) QSerialPort error:" << error
            << (m_serialPort ? m_serialPort->errorString() : QString());

        Q_Q(OnBusRtuSerialMaster);

        switch (error) {
        case QSerialPort::DeviceNotFoundError:
            q->setError(OnBusDevice::tr("Referenced serial device does not exist."),
                OnBusDevice::ConnectionError);
            break;
        case QSerialPort::PermissionError:
            q->setError(OnBusDevice::tr("Cannot open serial device due to permissions."),
                OnBusDevice::ConnectionError);
            break;
        case QSerialPort::OpenError:
        case QSerialPort::NotOpenError:
            q->setError(OnBusDevice::tr("Cannot open serial device."),
                OnBusDevice::ConnectionError);
            break;
        case QSerialPort::WriteError:
            q->setError(OnBusDevice::tr("Write error."), OnBusDevice::WriteError);
            break;
        case QSerialPort::ReadError:
            q->setError(OnBusDevice::tr("Read error."), OnBusDevice::ReadError);
            break;
        case QSerialPort::ResourceError:
            q->setError(OnBusDevice::tr("Resource error."), OnBusDevice::ConnectionError);
            break;
        case QSerialPort::UnsupportedOperationError:
            q->setError(OnBusDevice::tr("Device operation is not supported error."),
                OnBusDevice::ConfigurationError);
            break;
        case QSerialPort::TimeoutError:
            q->setError(OnBusDevice::tr("Timeout error."), OnBusDevice::TimeoutError);
            break;
        case QSerialPort::UnknownError:
            q->setError(OnBusDevice::tr("Unknown error."), OnBusDevice::UnknownError);
            break;
        default:
            qCDebug(QT_MODBUS) << "(RTU server) Unhandled QSerialPort error" << error;
            break;
        }
    }

    void setupSerialPort()
    {
        Q_Q(OnBusRtuSerialMaster);
        m_serialPort = new QSerialPort(q);

        QObject::connect(&m_responseTimer, &Timer::timeout, q, [this](int timerId) {
            onResponseTimeout(timerId);
        });

        QObject::connect(m_serialPort, &QSerialPort::readyRead, q, [this]() {
            onReadyRead();
        });

        QObject::connect(m_serialPort, &QSerialPort::aboutToClose, q, [this]() {
            onAboutToClose();
        });

        QObject::connect(m_serialPort, &QSerialPort::bytesWritten, q, [this](qint64 bytes) {
            onBytesWritten(bytes);
        });

        QObject::connect(m_serialPort, &QSerialPort::errorOccurred,
                q, [this](QSerialPort::SerialPortError error) {
            onError(error);
        });
    }

    /*!
        According to the Modbus specification, in RTU mode message frames
        are separated by a silent interval of at least 3.5 character times.
        Calculate the timeout if we are less than 19200 baud, use a fixed
        timeout for everything equal or greater than 19200 baud.
        If the user set the timeout to be longer than the calculated one,
        we'll keep the user defined.
    */
    void calculateInterFrameDelay()
    {
        // The spec recommends a timeout value of 1.750 msec. Without such
        // precise single-shot timers use a approximated value of 1.750 msec.
        int delayMilliSeconds = 2;
        if (m_baudRate < 19200) {
            // Example: 9600 baud, 11 bit per packet -> 872 char/sec
            // so: 1000 ms / 872 char = 1.147 ms/char * 3.5 character
            // Always round up because the spec requests at least 3.5 char.
            delayMilliSeconds = qCeil(3500. / (qreal(m_baudRate) / 11.));
        }
        if (m_interFrameDelayMilliseconds < delayMilliSeconds)
            m_interFrameDelayMilliseconds = delayMilliSeconds;
    }

    void setupEnvironment()
    {
        if (m_serialPort) {
            m_serialPort->setPortName(m_comPort);
            m_serialPort->setParity(m_parity);
            m_serialPort->setBaudRate(m_baudRate);
            m_serialPort->setDataBits(m_dataBits);
            m_serialPort->setStopBits(m_stopBits);
        }

        calculateInterFrameDelay();

        m_responseBuffer.clear();
        m_state = OnBusRtuSerialMasterPrivate::Idle;
    }

    OnBusReply *enqueueRequest(const OnBusRequest &request, const OnBusDataUnit &unit, OnBusReply::ReplyType type) override
    {
        Q_Q(OnBusRtuSerialMaster);

        auto reply = new OnBusReply(type, q);
        QueueElement element(reply, request, unit, m_numberOfRetries + 1);

        switch (unit.registerType()) {
        case OnBusDataUnit::Read:
            element.adu = OnBusSerialAdu::createRead(OnBusSerialAdu::Rtu, request);
            break;
        case OnBusDataUnit::Write:
            element.adu = OnBusSerialAdu::createWrite(OnBusSerialAdu::Rtu, request);
            break;
        case OnBusDataUnit::Command:
            element.adu = OnBusSerialAdu::createCommand(OnBusSerialAdu::Rtu, request);
        default:
            break;
        }

        m_queue.enqueue(element);

        scheduleNextRequest(m_interFrameDelayMilliseconds);

        return reply;
    }

    void scheduleNextRequest(int delay)
    {
        Q_Q(OnBusRtuSerialMaster);

        if (m_state == Idle && !m_queue.isEmpty()) {
            m_state = WaitingForReplay;
            QTimer::singleShot(delay, q, [this]() { processQueue(); });
        }
    }

    void processQueue()
    {
        m_responseBuffer.clear();
        m_serialPort->clear(QSerialPort::AllDirections);

        if (m_queue.isEmpty())
            return;
        auto &current = m_queue.first();

        if (current.reply.isNull()) {
            m_queue.dequeue();
            m_state = Idle;
            scheduleNextRequest(m_interFrameDelayMilliseconds);
        } else {
            current.bytesWritten = 0;
            current.numberOfRetries--;
            m_serialPort->write(current.adu);

            qCDebug(QT_MODBUS) << "(RTU client) Sent Serial PDU:" << current.requestPdu;
            qCDebug(QT_MODBUS_LOW).noquote() << "(RTU client) Sent Serial ADU: 0x" + current.adu.toHex();
        }
    }

    bool canMatchRequestAndResponse(const OnBusResponse &response) const
    {
        if (m_queue.isEmpty())
            return false;
        const auto &current = m_queue.first();

        if (current.reply.isNull())
            return false;   // reply deleted
        if (current.requestPdu.functionCode() != response.functionCode())
            return false;   // request for different function code
        return true;
    }

    bool isOpen() const override
    {
        if (m_serialPort)
            return m_serialPort->isOpen();
        return false;
    }

    QIODevice *device() const override { return m_serialPort; }

    Timer m_responseTimer;
    QByteArray m_responseBuffer;

    QQueue<QueueElement> m_queue;
    QSerialPort *m_serialPort = nullptr;

    int m_interFrameDelayMilliseconds = 2; // A approximated value of 1.750 msec.
    int m_turnaroundDelay = 100; // Recommended value is between 100 and 200 msec.
};

#include "onbusrtuserialmaster_p.h"

#endif // ONBUSRTUSERIALMASTER_P_H
