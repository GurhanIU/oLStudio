#ifndef ONBUSADU_P_H
#define ONBUSADU_P_H

#include "onbuspdu.h"

class OnBusSerialAdu
{
public:
    enum Type {
        Ascii,
        Rtu
    };

    inline OnBusSerialAdu(Type type, const QByteArray &data)
        : m_type(type), m_data(data), m_rawData(m_data)
    {
        if (m_type == Ascii)
            m_data = QByteArray::fromHex(m_data.mid(1, m_data.size() -3));
    }

    inline int size() const {
        if (m_type == Ascii)
            return m_data.size() -1; // one byte , LRC
        return m_data.size() -2; // two byte, CRC
    }
    inline QByteArray data() const { return m_data.left(size()); }

    inline int rawSize() const { return m_rawData.size(); }
    inline QByteArray rawData() const { return m_rawData; }

    inline OnBusPdu pdu() const {
        Q_ASSERT_X(!m_data.isEmpty(), "OnBusAdu::pdu()", "Empty ADU.");
        return OnBusPdu(OnBusPdu::FunctionCode(m_data.at(1)), m_data.mid(2, size() -2));
    }

    template <typename T>
    auto checksum() const -> decltype(T()) {
        Q_ASSERT_X(!m_data.isEmpty(), "OnBusAdu::checksum()", "Empty ADU.");
        if (m_type == Ascii)
            return quint8(m_data[m_data.size() -1]);
        return quint16(quint8(m_data[m_data.size() -2]) << 8 | quint8(m_data[m_data.size() -1]));
    }

    inline bool matchingChecksum() const {
        Q_ASSERT_X(!m_data.isEmpty(), "OnBusAdu::macthingChecksum()", "Empty ADU.");
        if (m_type == Ascii)
            return OnBusSerialAdu::calculateLRC(data(), size()) == checksum<quint8>();
        return OnBusSerialAdu::calculateCRC(data(), size()) == checksum<quint16>();
    }

    inline static quint8 calculateLRC(const char *data, quint32 len)
    {
        quint32 lrc = 0;
        while (len--)
            lrc += *data++;
        return -(quint8(lrc));
    }

    inline static quint16 calculateCRC(const char *data, qint32 len)
    {
        // Generated by pycrc v0.8.3, https://pycrc.org
        // Width = 16, Poly = 0x8005, XorIn = 0xffff, ReflectIn = True,
        // XorOut = 0x0000, ReflectOut = True, Algorithm = bit-by-bit-fast

        quint16 crc = 0xFFFF;
        while (len--) {
            const quint8 c = *data++;
            for (qint32 i = 0x01; i & 0xFF; i <<= 1) {
                bool bit = crc & 0x8000;
                if (c & i)
                    bit = !bit;
                crc <<= 1;
                if (bit)
                    crc ^= 0x8005;
            }
            crc &= 0xFFFF;
        }
        crc = crc_reflect(crc & 0xFFFF, 16) ^ 0x0000;
        return (crc >> 8) | (crc << 8); // swap bytes
    }

    inline static QByteArray create(Type type, quint8 count, const OnBusPdu &pdu, char delimiter = '\n') {
        QByteArray result;
        QDataStream out(&result, QIODevice::WriteOnly);
        out << (count * 3 +1) << count << pdu;

        if (type == Ascii) {
            out << calculateLRC(result, result.size());
            return ":" + result.toHex() + "\r" + delimiter;
        } else {
            out << calculateCRC(result, result.size());
        }
        return result;
    }


private:
    inline static quint16 crc_reflect(quint16 data, qint32 len)
    {
        // Generated by pycrc v0.8.3, https://pycrc.org
        // Width = 16, Poly = 0x8005, XorIn = 0xffff, ReflectIn = True,
        // XorOut = 0x0000, ReflectOut = True, Algorithm = bit-by-bit-fast

        quint16 ret = data & 0x01;
        for (qint32 i = 1; i < len; i++) {
            data >>= 1;
            ret = (ret << 1) | (data & 0x01);
        }
        return ret;
    }

private:
    Type m_type = Rtu;
    QByteArray m_data;
    QByteArray m_rawData;
};

#endif // ONBUSADU_P_H
