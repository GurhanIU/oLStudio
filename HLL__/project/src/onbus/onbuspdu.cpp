#include "onbuspdu.h"

#include <QHash>
#include <QDebugStateSaver>

using ReqSizeCalc = QHash<quint8, OnBusRequest::CalcFuncPtr>;
Q_GLOBAL_STATIC(ReqSizeCalc, requestSizeCalculators);

using ResSizeCalc = QHash<quint8, OnBusResponse::CalcFuncPtr>;
Q_GLOBAL_STATIC(ResSizeCalc, responseSizeCalculators);

namespace Private {

enum struct Type {
    Request,
    Response
};

static int minimumDataSize(const OnBusPdu &pdu, Type type)
{
    if (pdu.isException())
        return 1;

    switch (pdu.functionCode()) {
    case OnBusPdu::WriteMemory:
        return type == Type::Request ? 7 : 1;
    case OnBusPdu::ReadConfig:
        return type == Type::Request ? 6 : 1;
    case OnBusPdu::SetCommand:
        return type == Type::Request ? 3 : 1;
//    case OnBusPdu::Invalid:
    case OnBusPdu::UndefinedFunctionCode:
        return -1;
    }
    return -1;
}

static QDataStream &pduFromStream(QDataStream &stream, OnBusPdu &pdu, Type type)
{
    quint8 codeByte = 0;
    if (stream.readRawData(reinterpret_cast<char *>(&codeByte), sizeof(quint8)) != sizeof(quint8))
        return stream;
    OnBusPdu::FunctionCode code = OnBusPdu::FunctionCode(codeByte);
    pdu.setFunctionCode(code);

    auto needsAdditionalRead = [](OnBusPdu &pdu, int size) -> bool {
        if (size < 0)
            pdu.setFunctionCode(OnBusResponse::UndefinedFunctionCode);
        if (size <= 0)
            return false;
        return true;
    };

    const bool isResponse = (type == Type::Response);
    int size = isResponse ? OnBusResponse::minimumDataSize(pdu)
                          : OnBusRequest::minimumDataSize(pdu);
    if (!needsAdditionalRead(pdu, size))
        return stream;

    QByteArray data(size, Qt::Uninitialized);
    if (stream.device()->peek(data.data(), data.size()) != size)
        return stream;

    pdu.setData(data);
    size = isResponse ? OnBusResponse::calculateDataSize(pdu)
                      : OnBusRequest::calculateDataSize(pdu);
    if (!needsAdditionalRead(pdu, size))
        return stream;

    // reset what we have so far, next read might fail as well
    pdu.setData(QByteArray());
    pdu.setFunctionCode(OnBusPdu::UndefinedFunctionCode);

    if (data.size() <= 252) { // The maximum PDU data size is 252 bytes.
        data.resize(size);
        if (stream.readRawData(data.data(), data.size()) == size) {
            pdu.setData(data);
            pdu.setFunctionCode(code);
        }
    }
    return stream;
}

}   // namespace Private

QDebug operator<<(QDebug debug, const OnBusPdu &pdu)
{
    QDebugStateSaver _(debug);
    debug.nospace().noquote() << "0x" << Qt::hex << qSetFieldWidth(2) << qSetPadChar('0')
        << (pdu.isException() ? pdu.functionCode() | OnBusPdu::ExceptionByte : pdu.functionCode())
        << qSetFieldWidth(0) << pdu.data().toHex();
    return debug;
}

QDataStream &operator<<(QDataStream &stream, const OnBusPdu &pdu)
{
    if (pdu.isException())
        stream << static_cast<quint8> (pdu.functionCode() | OnBusPdu::ExceptionByte);
    else {
        stream << static_cast<quint8> (pdu.FirstByte);
        stream << static_cast<quint8> (pdu.functionCode());
        stream << static_cast<quint8> (pdu.dataCount() * 5 +1);
        stream << static_cast<quint8> (pdu.dataCount());
    }
    if (!pdu.data().isEmpty())
        stream.writeRawData(pdu.data().constData(), pdu.data().size());

    return stream;
}

int OnBusRequest::minimumDataSize(const OnBusRequest &request)
{
    return Private::minimumDataSize(request, Private::Type::Request);
}

int OnBusRequest::calculateDataSize(const OnBusRequest &request)
{
    if (requestSizeCalculators.exists()) {
        if (auto ptr = requestSizeCalculators()->value(quint8(request.functionCode()), nullptr))
            return ptr(request);
    }

    if (request.isException())
        return 1;

    int size = -1;
    int minimum = Private::minimumDataSize(request, Private::Type::Request);
    if (minimum < 0)
        return size;

    switch (request.functionCode()) {
    case OnBusPdu::WriteMemory:
        minimum -= 1; // first payload payload byte
        if (request.dataSize() >= minimum)
            size = minimum + quint8(request.data().at(minimum - 1)) /*byte count*/;
        break;
    case OnBusPdu::ReadConfig:
        minimum -= 2; // first 2 payload payload bytes
        if (request.dataSize() >= minimum)
            size = minimum + quint8(request.data().at(minimum - 1)) /*byte count*/;
        break;
    case OnBusPdu::SetCommand:
        if (request.dataSize() >= 1)
            size = 1 /*byte count*/ + quint8(request.data().at(0)) /*actual bytes*/;
        break;
    default:
        size = minimum;
        break;
    }
    return size;
}

void OnBusRequest::registerDataSizeCalculator(FunctionCode fc, CalcFuncPtr calculator)
{
    requestSizeCalculators()->insert(quint8(fc), calculator);
}

QDataStream &operator>>(QDataStream &stream, OnBusRequest &pdu)
{
    return Private::pduFromStream(stream, pdu, Private::Type::Request);
}

int OnBusResponse::minimumDataSize(const OnBusResponse &response)
{
    return Private::minimumDataSize(response, Private::Type::Response);
}

int OnBusResponse::calculateDataSize(const OnBusResponse &response)
{
    if (responseSizeCalculators.exists()) {
        if (auto ptr = responseSizeCalculators()->value(quint8(response.functionCode()), nullptr))
            return ptr(response);
    }

    if (response.isException())
        return 1;

    int size = -1;
    int minimum = Private::minimumDataSize(response, Private::Type::Response);
    if (minimum < 0)
        return size;

    switch (response.functionCode()) {
    case OnBusResponse::ReadConfig:
    case OnBusResponse::WriteMemory:
    case OnBusResponse::SetCommand:
        if (response.dataSize() >= 1)
            size = 1 /*byte count*/ + quint8(response.data().at(0)) /*actual bytes*/;
        break;
    default:
        size = minimum;
        break;
    }
    return size;
}

void OnBusResponse::registerDataSizeCalculator(FunctionCode fc, CalcFuncPtr calculator)
{
    responseSizeCalculators()->insert(quint8(fc), calculator);
}

QDataStream &operator>>(QDataStream &stream, OnBusResponse &pdu)
{
    return Private::pduFromStream(stream, pdu, Private::Type::Response);
}
