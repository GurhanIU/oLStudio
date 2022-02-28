#include "onbusrtuserialmaster.h"
#include "onbusrtuserialmaster_p.h"

#include <QDebug>

OnBusRtuSerialMaster::OnBusRtuSerialMaster(QObject *parent)
    : OnBusMaster(*new OnBusRtuSerialMasterPrivate, parent)
{
    Q_D(OnBusRtuSerialMaster);
    d->setupSerialPort();
}

OnBusRtuSerialMaster::~OnBusRtuSerialMaster()
{
    close();
}

int OnBusRtuSerialMaster::interFrameDelay() const
{
    Q_D(const OnBusRtuSerialMaster);
    return d->m_interFrameDelayMilliseconds * 1000;
}

void OnBusRtuSerialMaster::setInterFrameDelay(int microseconds)
{
    Q_D(OnBusRtuSerialMaster);
    d->m_interFrameDelayMilliseconds = qCeil(qreal(microseconds) / 1000.);
    d->calculateInterFrameDelay();
}

int OnBusRtuSerialMaster::turnaroundDelay() const
{
    Q_D(const OnBusRtuSerialMaster);
    return d->m_turnaroundDelay;
}

void OnBusRtuSerialMaster::setTurnaroundDelay(int turnaroundDelay)
{
    Q_D(OnBusRtuSerialMaster);
    d->m_turnaroundDelay = turnaroundDelay;
}

OnBusRtuSerialMaster::OnBusRtuSerialMaster(OnBusRtuSerialMasterPrivate &dd, QObject *parent)
    : OnBusMaster(dd, parent)
{
    Q_D(OnBusRtuSerialMaster);
    d->setupSerialPort();
}

bool OnBusRtuSerialMaster::open()
{
    if (state() == OnBusDevice::ConnectedState)
        return true;

    Q_D(OnBusRtuSerialMaster);
    d->setupEnvironment(); // to be done before open
    if (d->m_serialPort->open(QIODevice::ReadWrite)) {
        setState(OnBusDevice::ConnectedState);
        d->m_serialPort->clear(); // only possible after open
    } else {
        setError(d->m_serialPort->errorString(), OnBusDevice::ConnectionError);
    }
    return (state() == OnBusDevice::ConnectedState);
}

void OnBusRtuSerialMaster::close()
{
    if (state() == OnBusDevice::UnconnectedState)
        return;

    setState(OnBusDevice::ClosingState);

    Q_D(OnBusRtuSerialMaster);

    if (d->m_serialPort->isOpen())
        d->m_serialPort->close();

    int numberOfAborts = 0;
    while (!d->m_queue.isEmpty()) {
        // Finish each open reply and forget them
        OnBusRtuSerialMasterPrivate::QueueElement elem = d->m_queue.dequeue();
        if (!elem.reply.isNull()) {
            elem.reply->setError(OnBusDevice::ReplyAbortedError,
                                 OnBusMaster::tr("Reply aborted due to connection closure."));
            numberOfAborts++;
        }
    }

    if (numberOfAborts > 0)
        qCDebug(QT_MODBUS_LOW) << "(RTU client) Aborted replies:" << numberOfAborts;

    setState(OnBusDevice::UnconnectedState);
}
