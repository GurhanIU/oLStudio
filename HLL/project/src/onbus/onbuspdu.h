#ifndef ONBUSPDU_H
#define ONBUSPDU_H

#include <QByteArray>

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
        WriteMemory = 0x00,
        WatchConfig = 0x01,
        WatchVars = 0x02,
        Connect = 0x03,
        UndefinedFunctionCode  = 0x100
    };

    OnBusPdu() = default;
    virtual ~OnBusPdu() = default;

    bool isValid() const {
        return (m_code >= WriteMemory && m_code < UndefinedFunctionCode)
                && (m_data.size() < 255);
    }

    static const quint8 ExceptionByte = 0x80;
    ExceptionCode exception() const {
        if (!m_data.size() || isException())
            return ExtendedException;
        return static_cast<ExceptionCode>(m_data.at(1));
    }

    bool isException() const { return m_code & ExceptionByte; }

    quint16 size() const { return dataSize() + 1; }
    quint16 dataSize() const { return quint16(m_data.size()); }

    FunctionCode funcionCode() const {
        return FunctionCode(quint8(m_code) &~ ExceptionByte);
    }

    virtual void setFunctionCode(FunctionCode code) { m_code = code; }

    QByteArray data() const { return m_data; }
    void setData(const QByteArray &newData) { m_data = newData; }

    template <typename ... Args> void encodeData(Args ... newData) {
        encode(std::forward<Args>(newData)...);
    }

    template <typename ... Args> void decodeData(Args && ... newData) const {
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
    FunctionCode m_code = UndefinedFunctionCode;
    QByteArray m_data;
};

#endif // ONBUSPDU_H
