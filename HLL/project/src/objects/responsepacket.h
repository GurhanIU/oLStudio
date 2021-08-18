#ifndef RESPONSEPACKET_H
#define RESPONSEPACKET_H

#include <QObject>
#include <QByteArray>

class ResponsePacket : public QObject
{
    Q_OBJECT
public:    
    static const char SOP = 0xAA;
    static const char EOP = 0x55;
    static const char RX_MAX_LEN = 255;

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
    ResponsePacket(const QByteArray &rawData, QObject *parent = nullptr);

    ~ResponsePacket();

    QByteArray packet() const;
    void setRawData(const QByteArray &rawData);
    void initRawData();

    void init();
    static QList<QByteArray> initRawData(const QByteArray &raw);
signals:
    void responseStatus(const QString &status);
    void responseData(const QList<ushort>&);
    void responsePacket(const QByteArray &packet);
    void aaaa();

private:
    QByteArray m_rawData;
    QList<QByteArray> m_list;

    bool m_valid;

    void checkPacketStatus(const uchar &packetStatus);
    bool validatePacket(const QByteArray &packet);
};

#endif // RESPONSEPACKET_H
