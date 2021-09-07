#include "onbusdevice.h"
#include "onbusdevice_p.h"
#include "onbusdataunit.h"

#include <QLoggingCategory>

OnBusDevice::OnBusDevice(QObject *parent)
    : QObject(*new OnBusDevicePrivate, parent)
{
//    qRegisterMetaType<OnBusDevice::Error>();
//    qRegisterMetaType<OnBusDevice::State>();
}

OnBusDevice::OnBusDevice(OnBusDevicePrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

OnBusDevice::~OnBusDevice()
{
}

QVariant OnBusDevice::connectionParameter(int parameter) const
{
    Q_D(const OnBusDevice);
    switch (parameter) {
    case SerialPortNameParameter:
        return d->m_comPort;
    case SerialDataBitsParameter:
        return d->m_dataBits;
    case SerialParityParameter:
        return d->m_parity;
    case SerialStopBitsParameter:
        return d->m_stopBits;
    case SerialBaudRateParameter:
        return d->m_baudRate;
    case NetworkPortParameter:
        return d->m_networkPort;
    case NetworkAddressParameter:
        return d->m_networkAddress;
    default:
        break;
    }
    return d->m_userConnectionParams.value(parameter);
}

void OnBusDevice::setConnectionParameter(int parameter, const QVariant &value)
{
    Q_D(OnBusDevice);
    switch (parameter) {
    case SerialPortNameParameter:
        d->m_comPort = value.toString();
        break;
    case SerialDataBitsParameter:
        d->m_dataBits = QSerialPort::DataBits(value.toInt());
        break;
    case SerialParityParameter:
        d->m_parity = QSerialPort::Parity(value.toInt());
        break;
    case SerialStopBitsParameter:
        d->m_stopBits = QSerialPort::StopBits(value.toInt());
        break;
    case SerialBaudRateParameter:
        d->m_baudRate = QSerialPort::BaudRate(value.toInt());
        break;
    case NetworkPortParameter:
        d->m_networkPort = value.toInt();
        break;
    case NetworkAddressParameter:
        d->m_networkAddress = value.toString();
        break;
    default:
        d->m_userConnectionParams.insert(parameter, value); // ### Qt6: remove
        break;
    }
}

bool OnBusDevice::connectDevice()
{
    Q_D(OnBusDevice);

    if (d->state != OnBusDevice::UnconnectedState)
        return false;

    setState(ConnectingState);

    if (!open()) {
        setState(UnconnectedState);
        return false;
    }

    //Connected is set by backend -> might be delayed by event loop
    return true;
}

void OnBusDevice::disconnectDevice()
{
    if (state() == OnBusDevice::UnconnectedState)
        return;

    setState(OnBusDevice::ClosingState);

    //Unconnected is set by backend -> might be delayed by event loop
    close();
}

void OnBusDevice::setState(OnBusDevice::State newState)
{
    Q_D(OnBusDevice);

    if (newState == d->state)
        return;

    d->state = newState;
    emit stateChanged(newState);
}

OnBusDevice::State OnBusDevice::state() const
{
    return d_func()->state;
}

void OnBusDevice::setError(const QString &errorText, OnBusDevice::Error error)
{
    Q_D(OnBusDevice);

    d->error = error;
    d->errorString = errorText;
    emit errorOccurred(error);
}

OnBusDevice::Error OnBusDevice::error() const
{
    return d_func()->error;
}

QString OnBusDevice::errorString() const
{
    return d_func()->errorString;
}

QIODevice *OnBusDevice::device() const
{
    return d_func()->device();
}

Q_LOGGING_CATEGORY(QT_MODBUS, "qt.modbus")
Q_LOGGING_CATEGORY(QT_MODBUS_LOW, "qt.modbus.lowlevel")
