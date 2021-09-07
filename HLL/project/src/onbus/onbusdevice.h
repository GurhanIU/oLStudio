#ifndef ONBUSDEVICE_H
#define ONBUSDEVICE_H


#include <QObject>
#include <QIODevice>
#include <QtSerialBus/qtserialbusglobal.h>

class OnBusDevicePrivate;

class OnBusDevice : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OnBusDevice)

public:
    enum Error {
        NoError,
        ReadError,
        WriteError,
        ConnectionError,
        ConfigurationError,
        TimeoutError,
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

    enum ConnectionParameter {
        SerialPortNameParameter,
        SerialParityParameter,
        SerialBaudRateParameter,
        SerialDataBitsParameter,
        SerialStopBitsParameter,

        NetworkPortParameter,
        NetworkAddressParameter,

        // Reserved
        UserParameter = 0x100 // ### Qt6: remove
    };
    Q_ENUM(ConnectionParameter)

    explicit OnBusDevice(QObject *parent = nullptr);
    ~OnBusDevice();

    QVariant connectionParameter(int parameter) const;
    void setConnectionParameter(int parameter, const QVariant &value);

    bool connectDevice();
    void disconnectDevice();

    State state() const;

    Error error() const;
    QString errorString() const;

    QIODevice *device() const;

Q_SIGNALS:
    void errorOccurred(OnBusDevice::Error error);
    void stateChanged(OnBusDevice::State state);

protected:
    OnBusDevice(OnBusDevicePrivate &dd, QObject *parent = nullptr);

    void setState(OnBusDevice::State newState);
    void setError(const QString &errorText, OnBusDevice::Error error);
    virtual bool open() = 0;
    virtual void close() = 0;
};

//Q_DECLARE_METATYPE(OnBusDevice::Error)
//Q_DECLARE_METATYPE(OnBusDevice::State)
Q_DECLARE_TYPEINFO(OnBusDevice::Error, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(OnBusDevice::State, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(OnBusDevice::ConnectionParameter, Q_PRIMITIVE_TYPE);

#endif // ONBUSDEVICE_H
