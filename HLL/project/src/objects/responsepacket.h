#ifndef RESPONSEPACKET_H
#define RESPONSEPACKET_H

#include <QObject>
#include <QByteArray>

class ResponsePacket : public QObject
{
    Q_OBJECT
public:    
    const char SOP = 0xAA;
    const char EOP = 0x55;
    const char RX_MAX_LEN = 255;

    enum Address
    {
        ADR_SOP = 0,
        ADR_FNC = 1,
        ADR_LEN = 2,
    };
    Q_ENUM(Address)

    enum Function
    {
        FC_WRITE_MEM,
        FC_WATCH_CONF,
        FC_WATCH_VARS,
        FC_CONNECT
    };
    Q_ENUM(Function)

    enum Values
    {
        PCK_NOT_RDY,
        PCK_READY,
        RESP_OK,
        PCK_INV_ID,
        PCK_CHK_ERR,
        PCK_OVERFLOW,
        PCK_INV_EOP,
    };
    Q_ENUM(Values)

    explicit ResponsePacket(QObject *parent = nullptr);
    ResponsePacket(const QByteArray &packet, QObject *parent = nullptr);

    ~ResponsePacket();

    QByteArray packet() const;
    void setPacket(const QByteArray &packet);

    bool isValid() const;
//    void setValid(bool valid);

signals:
    void responseStatus(const QString &status);
    void responseData(const QList<ushort>&);
    void aaaa();

private:
    QByteArray m_packet;

    bool m_valid;

    void initPacket();

    bool crcCalculation(int start, int stop, const uchar &packetCrc);
    void checkPacketStatus(const uchar &packetStatus);
};

#endif // RESPONSEPACKET_H
