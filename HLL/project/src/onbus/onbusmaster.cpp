#include "onbusmaster.h"
#include "onbusmaster_p.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(QT_MODBUS)

OnBusMaster::OnBusMaster(QObject *parent)
    : OnBusDevice(*new OnBusMasterPrivate, parent)
{
}

OnBusMaster::~OnBusMaster()
{
}

OnBusReply *OnBusMaster::sendReadRequest(const OnBusDataUnit &read)
{
    Q_D(OnBusMaster);
    return d->sendRequest(d->createReadRequest(read), &read);
}

OnBusReply *OnBusMaster::sendWriteRequest(const OnBusDataUnit &write)
{
    Q_D(OnBusMaster);
    return d->sendRequest(d->createWriteRequest(write), &write);
}

OnBusReply *OnBusMaster::sendCommandRequest(const OnBusDataUnit &command)
{
    Q_D(OnBusMaster);
    return d->sendRequest(d->createCommandRequest(command), &command);
}

OnBusReply *OnBusMaster::sendRawRequest(const OnBusRequest &request)
{
    return d_func()->sendRequest(request, nullptr);
}

int OnBusMaster::timeout() const
{
    Q_D(const OnBusMaster);
    return d->m_responseTimeoutDuration;
}

void OnBusMaster::setTimeout(int newTimeout)
{
    if (newTimeout < 10)
        return;

    Q_D(OnBusMaster);
    if (d->m_responseTimeoutDuration != newTimeout) {
        d->m_responseTimeoutDuration = newTimeout;
        emit timeoutChanged(newTimeout);
    }
}

int OnBusMaster::numberOfRetries() const
{
    Q_D(const OnBusMaster);
    return d->m_numberOfRetries;
}

void OnBusMaster::setNumberOfRetries(int number)
{
    Q_D(OnBusMaster);
    if (number >= 0)
        d->m_numberOfRetries = number;
}

OnBusMaster::OnBusMaster(OnBusMasterPrivate &dd, QObject *parent) :
    OnBusDevice(dd, parent)
{

}

bool OnBusMaster::processResponse(const OnBusResponse &response, OnBusDataUnit *data)
{
    return d_func()->processResponse(response, data);
}

bool OnBusMaster::processPrivateResponse(const OnBusResponse &response, OnBusDataUnit *data)
{
    Q_UNUSED(response)
    Q_UNUSED(data)
    return false;
}

OnBusReply *OnBusMasterPrivate::sendRequest(const OnBusRequest &request, const OnBusDataUnit *const unit)
{
    Q_Q(OnBusMaster);

    if (!isOpen() || q->state() != OnBusDevice::ConnectedState) {
        qCWarning(QT_MODBUS) << "(Client) Device is not connected";
        q->setError(OnBusMaster::tr("Device not connected."), OnBusDevice::ConnectionError);
        return nullptr;
    }

    if (!request.isValid()) {
        qCWarning(QT_MODBUS) << "(Client) Refuse to send invalid request.";
        q->setError(OnBusMaster::tr("Invalid Modbus request."), OnBusDevice::ProtocolError);
        return nullptr;
    }

    if (unit)
        return enqueueRequest(request, *unit, OnBusReply::Common);
    return enqueueRequest(request, OnBusDataUnit(), OnBusReply::Raw);
}

OnBusRequest OnBusMasterPrivate::createReadRequest(const OnBusDataUnit &data) const
{
    if (!data.isValid())
        return OnBusRequest();

    switch (data.registerType()) {
    case OnBusDataUnit::Read:
        return OnBusRequest(OnBusRequest::ReadConfig, data.values());
    break;

    default:
        break;
    }
    return OnBusRequest();
}

OnBusRequest OnBusMasterPrivate::createWriteRequest(const OnBusDataUnit &data) const
{
    switch (data.registerType()) {
    case OnBusDataUnit::Write: {
        if (data.count() == 1) {
            return OnBusRequest(OnBusRequest::WriteMemory, data.values());
        }
    }   break;

    default:    // fall through on purpose
        break;
    }
    return OnBusRequest();
}

OnBusRequest OnBusMasterPrivate::createCommandRequest(const OnBusDataUnit &data) const
{
    switch (data.registerType()) {
    case OnBusDataUnit::Command: {
        if (data.count() == 1) {
            return OnBusRequest(OnBusRequest::SetCommand, data.values());
        }
    }   break;

    default:    // fall through on purpose
        break;
    }
    return OnBusRequest();
}

void OnBusMasterPrivate::processQueueElement(const OnBusResponse &pdu, const QueueElement &element)
{
    if (element.reply.isNull())
        return;

    element.reply->setRawResult(pdu);
    if (pdu.isException()) {
        element.reply->setError(OnBusDevice::ProtocolError,
            OnBusMaster::tr("OnBus Exception Response."));
        return;
    }

    if (element.reply->type() != OnBusReply::Common) {
        element.reply->setFinished(true);
        return;
    }

    OnBusDataUnit unit = element.unit;
    if (!processResponse(pdu, &unit)) {
        element.reply->setError(OnBusDevice::UnknownError,
            OnBusMaster::tr("An invalid response has been received."));
        return;
    }

    element.reply->setResult(unit);
    element.reply->setFinished(true);
}

bool OnBusMasterPrivate::processResponse(const OnBusResponse &response, OnBusDataUnit *data)
{
    switch (response.functionCode()) {
    case OnBusRequest::ReadConfig:
        return processReadResponse(response, data);
    case OnBusRequest::WriteMemory:
        return processWriteResponse(response, data);
    case OnBusRequest::SetCommand:
        return processCommandResponse(response, data);
    default:
        return false;   // Return early, it's not a private response.
    }
    return q_func()->processPrivateResponse(response, data);
}

static bool isValid(const OnBusResponse &response, OnBusResponse::FunctionCode fc)
{
    if (!response.isValid())
        return false;
    if (response.isException())
        return false;
    if (response.functionCode() != fc)
        return false;
    return true;
}

bool OnBusMasterPrivate::processReadResponse(const OnBusResponse &response, OnBusDataUnit *data)
{
    if (!isValid(response, OnBusResponse::ReadConfig))
        return false;
    return collateBytes(response, OnBusDataUnit::Read, data);
}

bool OnBusMasterPrivate::collateBytes(const OnBusPdu &response, OnBusDataUnit::RegisterType type, OnBusDataUnit *data)
{
    if (response.dataSize() < OnBusResponse::minimumDataSize(response))
        return false;

    // byte count needs to match available bytes
    const quint8 byteCount = quint8(response.data().at(0));
    if ((response.dataSize() - 1) != byteCount)
        return false;

    // byte count needs to be odd to match full registers
    if (byteCount % 2 != 0)
        return false;

    if (data) {
        QDataStream stream(response.data().remove(0, 1));

        QVector<quint16> values;
        const quint8 itemCount = byteCount / 2;
        for (int i = 0; i < itemCount; i++) {
            quint16 tmp;
            stream >> tmp;
            values.append(tmp);
        }
        /// TODO Bakilacak
//        data->setValues(values);
        data->setRegisterType(type);
    }
    return true;
}

bool OnBusMasterPrivate::processWriteResponse(const OnBusResponse &response, OnBusDataUnit *data)
{
    if (!isValid(response, OnBusResponse::WriteMemory))
        return false;
    return collateSingleValue(response, OnBusDataUnit::Write, data);
}

bool OnBusMasterPrivate::collateSingleValue(const OnBusPdu &response, OnBusDataUnit::RegisterType type, OnBusDataUnit *data)
{
    if (response.dataSize() != OnBusResponse::minimumDataSize(response))
        return false;

    quint16 address, value;
    response.decodeData(&address, &value);

    /// TODO Bakilacak
//    if (data) {
//        data->setRegisterType(type);
//        data->setStartAddress(address);
//        data->setValues(QVector<quint16>{ value });
//    }
    return true;
}

bool OnBusMasterPrivate::processCommandResponse(const OnBusResponse &response, OnBusDataUnit *data)
{

}

bool OnBusMasterPrivate::collateMultipleValues(const OnBusPdu &response, OnBusDataUnit::RegisterType type, OnBusDataUnit *data)
{
    if (response.dataSize() != OnBusResponse::minimumDataSize(response))
        return false;

    quint16 address, count;
    response.decodeData(&address, &count);

    /// TODO Bakilacak
    // number of registers to write is 1-123 per request
    if ( (type == OnBusDataUnit::Read) && ( count < 1 || count > 123) )
        return false;

    /// TODO Bakilacak
//    if (data) {
//        data->setValueCount(count);
//        data->setRegisterType(type);
//        data->setStartAddress(address);
//    }
    return true;
}
