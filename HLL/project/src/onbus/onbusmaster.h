#ifndef ONBUSMASTER_H
#define ONBUSMASTER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QtSerialPort/QSerialPort>

class OnBusMaster : public QThread
{
    Q_OBJECT

public:
    enum Error {
        NoError,
        ReadError,
        WriteError,
        ConnectionError,
        ConfigurationError,
        WriteTimeoutError,
        ReadTimeoutError,
        ProtocolError,
        ReplyAbortedError,
        UnknownError
    };
    Q_ENUM(Error)

    enum State {
        UnconnectedState,
        ConnectingState,
        ConnectedState,
        ClosingState
    };
    Q_ENUM(State)

    OnBusMaster(QObject *parent = 0);
    ~OnBusMaster();

    void run();

    bool setConfig(const QString &portName, QSerialPort::BaudRate baudRate, QSerialPort::Parity parity, QSerialPort::DataBits dataBits, QSerialPort::StopBits stopBits, int waitTimeout);

    OnBusMaster::State state() const;
    void setState(const State &state);

    static quint64 elapsed(const QElapsedTimer &timer);
public slots:
    void transaction(const QByteArray &data);
    void openPort();
    void closePort();
signals:
    void errorOccurred(OnBusMaster::Error error);
    void stateChanged(OnBusMaster::State state);

    void response(const QByteArray &data);
    void request(const QByteArray &data);

private:
    State m_state;
    QString m_portName;
    QSerialPort::Parity m_parity;
    QSerialPort::BaudRate m_baudRate;
    QSerialPort::DataBits m_dataBits;
    QSerialPort::StopBits m_stopBits;

    QByteArray m_transactionData;
    int m_waitTimeout;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_quit;
};

Q_DECLARE_METATYPE(OnBusMaster::Error)
Q_DECLARE_METATYPE(OnBusMaster::State)

#endif // ONBUSMASTER_H
