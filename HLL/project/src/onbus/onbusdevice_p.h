#ifndef ONBUSDEVICE_P_H
#define ONBUSDEVICE_P_H

#include "onbusdevice.h"

#include <QVariant>
#include <QSerialPort>
#include <QtCore/private/qobject_p.h>

class OnBusDevicePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(OnBusDevice)

public:
    OnBusDevice::State state = OnBusDevice::UnconnectedState;
    OnBusDevice::Error error = OnBusDevice::NoError;
    QString errorString;

    QString m_comPort;
    QSerialPort::DataBits m_dataBits = QSerialPort::Data8;
    QSerialPort::Parity m_parity = QSerialPort::EvenParity;
    QSerialPort::StopBits m_stopBits = QSerialPort::OneStop;
    QSerialPort::BaudRate m_baudRate = QSerialPort::Baud19200;

    int m_networkPort = 502;
    QString m_networkAddress = QStringLiteral("127.0.0.1");

    QHash<int, QVariant> m_userConnectionParams; // ### Qt6: remove

    virtual QIODevice *device() const { return nullptr; }
};

#endif // ONBUSDEVICE_P_H
