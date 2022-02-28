#ifndef ONBUSPDU_H
#define ONBUSPDU_H

#include <QDataStream>
#include <QMetaType>
#include <QVector>
#include <QtSerialBus/qtserialbusglobal.h>

class OnBusDataUnit;

class OnBusPdu
{
public:
    enum ExceptionCode {
        NotReady = 0x00,
        Ready = 0x01,
        Ok = 0x02,
        InvalidId = 0x03,
        InvalidChecksum = 0x04,
        Overflow = 0x05,
        InvalidEndOffPacket = 0x06,
        ExtendedException = 0xFF,
    };

    enum FunctionCode {
        Invalid = 0x00,
        WriteMemory = 0x01,
        ReadConfig = 0x02,
        SetCommand = 0x03,
        UndefinedFunctionCode  = 0x100
    };

    enum CommandCode {
        Start = 0xAA,
        Stop = 0x55,
    };

    OnBusPdu() = default;
    virtual ~OnBusPdu() = default;

    bool isValid() const {
        return (m_code >= WriteMemory && m_code < UndefinedFunctionCode)
                && (m_data.size() < 255);
    }

    static const quint8 FirstByte = 0xAA;
    static const quint8 LastByte = 0x55;
    static const quint8 ExceptionByte = 0x80;
    ExceptionCode exception() const {
        if (!m_data.size() || isException())
            return ExtendedException;
        return static_cast<ExceptionCode>(m_data.at(1));
    }

    bool isException() const { return m_code & ExceptionByte; }

    quint16 size() const { return dataSize() + 1; }
    quint16 dataSize() const { return quint16(m_data.size()); }

    void setDataCount(quint8 count) { m_count = count; }
    quint8 dataCount() const { return m_count; }

    FunctionCode functionCode() const {
        return FunctionCode(quint8(m_code) &~ ExceptionByte);
    }

    virtual void setFunctionCode(FunctionCode code) { m_code = code; }

    QByteArray data() const { return m_data; }
    void setData(const QByteArray &newData) { m_data = newData; }

    template <typename ... Args>
    void encodeData(Args ... newData) {
        encode(std::forward<Args>(newData)...);
    }

    template <typename ... Args>
    void decodeData(Args && ... newData) const {
        decode(std::forward<Args>(newData)...);
    }

protected:
    OnBusPdu(FunctionCode code, const QByteArray &newData)
        : m_code(code),
          m_data(newData)
    {}

    OnBusPdu(const OnBusPdu &) = default;
    OnBusPdu &operator=(const OnBusPdu &) = default;

    template <typename ... Args>
    OnBusPdu(FunctionCode code, Args ... newData)
        : m_code(code)
    {
        encode(std::forward<Args>(newData)...);
    }

private:
    template <typename T, typename ... Ts>
    struct IsType {
        enum { value = false };
    };

    template <typename T, typename T1, typename ... Ts>
    struct IsType<T, T1, Ts...> {
        enum { value = std::is_same<T, T1>::value || IsType<T, Ts...>::value };
    };

    template <typename T>
    using is_pod = std::integral_constant<bool, std::is_trivial<T>::value && std::is_standard_layout<T>::value>;

    template <typename T>
    void encode(QDataStream *stream, const T &t) {
        /// TODO Bakilacak
        static_assert(is_pod<T>::value, "Only POD types supported.");
        static_assert(IsType<T, quint8, quint16>::value, "Only quint8 and quint16 supported.");
        (*stream) << t;
    }

    template <typename T>
    void decode(QDataStream *stream, T &t) const {
        static_assert(is_pod<T>::value, "Only POD types supported.");
        static_assert(IsType<T, quint8 *, quint16 *>::value, "Only quint8* and quint16* supported.");
        (*stream) >> *t;
    }

    template <typename T>
    void encode(QDataStream *stream, const QVector<T> &vector) {
        static_assert(is_pod<T>::value, "Only POD types supported.");
        static_assert(IsType<T, quint8, quint16>::value, "Only quint8 and quint16 supported.");
        for (int i = 0; i < vector.count(); ++i)
            (*stream) << vector[i];
    }

    template<typename ... Args> void encode(Args ... newData) {
        m_data.clear();
        Q_CONSTEXPR quint32 argCount = sizeof...(Args);
        if (argCount > 0) {
            QDataStream stream(&m_data, QIODevice::WriteOnly);
            char tmp[argCount] = { (encode(&stream, newData), void(), '0')... };
            Q_UNUSED(tmp)
        }
    }
    template<typename ... Args> void decode(Args ... newData) const {
        Q_CONSTEXPR quint32 argCount = sizeof...(Args);
        if (argCount > 0 && !m_data.isEmpty()) {
            QDataStream stream(m_data);
            char tmp[argCount] = { (decode(&stream, newData), void(), '0')... };
            Q_UNUSED(tmp)
        }
    }

private:
    FunctionCode m_code = UndefinedFunctionCode;
    quint8 m_count = 0;
    QByteArray m_data;
    friend class OnBusSerialAdu;
};
QDebug operator<<(QDebug debug, const OnBusPdu &pdu);
QDataStream &operator<<(QDataStream &stream, const OnBusPdu &pdu);

class OnBusRequest : public OnBusPdu
{
public:
    OnBusRequest() = default;
    OnBusRequest(const OnBusPdu &pdu)
        : OnBusPdu(pdu)
    {}

    explicit OnBusRequest(FunctionCode code, const QByteArray &newData = QByteArray())
        : OnBusPdu(code, newData)
    {}

    static int minimumDataSize(const OnBusRequest &pdu);
    static int calculateDataSize(const OnBusRequest &pdu);

    using CalcFuncPtr = decltype(&calculateDataSize);
    static void registerDataSizeCalculator(FunctionCode fc, CalcFuncPtr func);

    template <typename ... Args>
    OnBusRequest(FunctionCode code, Args ... newData)
        : OnBusPdu(code, newData...)
    {}
};
QDataStream &operator>>(QDataStream &stream, OnBusRequest &pdu);
inline QDataStream &operator<<(QDataStream &stream, const OnBusRequest &pdu)
{ return stream << static_cast<const OnBusPdu &>(pdu); }

class OnBusResponse : public OnBusPdu
{
public:
    OnBusResponse() = default;
    OnBusResponse(const OnBusPdu &pdu)
        : OnBusPdu(pdu)
    {}

    explicit OnBusResponse(FunctionCode code, const QByteArray &newData = QByteArray())
        : OnBusPdu(code, newData)
    {}

    static int minimumDataSize(const OnBusResponse &pdu);
    static int calculateDataSize(const OnBusResponse &pdu);

    using CalcFuncPtr = decltype(&calculateDataSize);
    static void registerDataSizeCalculator(FunctionCode fc, CalcFuncPtr func);

    template <typename ... Args>
    OnBusResponse(FunctionCode code, Args ... newData)
        : OnBusPdu(code, newData...)
    {}

};

class OnBusExceptionResponse : public OnBusResponse
{
public:
    OnBusExceptionResponse() = default;
    OnBusExceptionResponse(const OnBusPdu &pdu)
        : OnBusResponse(pdu)
    {}
    OnBusExceptionResponse(FunctionCode fc, ExceptionCode ec)
        : OnBusResponse(FunctionCode(quint8(fc) | ExceptionByte), static_cast<quint8> (ec))
    {}

    void setFunctionCode(FunctionCode c) {
        OnBusPdu::setFunctionCode(FunctionCode(quint8(c) | ExceptionByte));
    }
    void setExceptionCode(ExceptionCode ec) { OnBusPdu::encodeData(quint8(ec)); }
};
QDataStream &operator>>(QDataStream &stream, OnBusResponse &pdu);
inline QDataStream &operator<<(QDataStream &stream, const OnBusResponse &pdu)
{ return stream << static_cast<const OnBusPdu &>(pdu); }

Q_DECLARE_TYPEINFO(OnBusPdu, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(OnBusPdu::ExceptionCode, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(OnBusPdu::FunctionCode, Q_PRIMITIVE_TYPE);

Q_DECLARE_TYPEINFO(OnBusRequest, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(OnBusResponse, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(OnBusExceptionResponse, Q_MOVABLE_TYPE);

Q_DECLARE_METATYPE(OnBusPdu::ExceptionCode)
Q_DECLARE_METATYPE(OnBusPdu::FunctionCode)

#endif // ONBUSPDU_H
