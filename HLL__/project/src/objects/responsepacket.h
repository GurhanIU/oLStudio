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

    enum Status
    {
        PCK_NOT_RDY,
        PCK_READY,
        RESP_OK,
        PCK_INV_ID,
        PCK_CHK_ERR,
        PCK_OVERFLOW,
        PCK_INV_EOP,
        PCK_UNKNOWN,
    };
    Q_ENUM(Status)

    explicit ResponsePacket(QObject *parent = nullptr);
    explicit ResponsePacket(const QByteArray &request,  const QByteArray &response, QObject *parent = nullptr);
    ResponsePacket(const QByteArray &rawData, QObject *parent = nullptr);

    ~ResponsePacket();

    QByteArray packet() const;
    void setRawData(const QByteArray &rawData);
    void clearRawData();
    void initRawData();

    void init();
    static QList<QByteArray> initRawData(const QByteArray &raw);

    static bool isValid();

    const QByteArray &request() const;
    const QByteArray &response() const;

    const QString &status() const;

signals:
    void responseStatus(const QString &status);
    void responseData(const QList<ushort>&);
    void responsePacket(const QByteArray &packet);

private:
    static QByteArray m_rawData;
    QByteArray m_request;
    QByteArray m_response;
    QList<QByteArray> m_list;
    QString m_status;

    static bool m_valid;

    ResponsePacket::Status checkPacketStatus(const uchar &packetStatus);
    bool validatePacket(const QByteArray &response);
};

#endif // RESPONSEPACKET_H
