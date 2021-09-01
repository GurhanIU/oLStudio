#include "responsepacket.h"
#include <QDebug>

QByteArray ResponsePacket::m_rawData;
bool ResponsePacket::m_valid;

ResponsePacket::ResponsePacket(QObject *parent) :
    QObject(parent)
{

}

ResponsePacket::ResponsePacket(const QByteArray &request, const QByteArray &response, QObject *parent) :
    QObject(parent),
    m_request(request),
    m_response(response)
{
    m_valid = validatePacket(response);

}

ResponsePacket::ResponsePacket(const QByteArray &rawData, QObject *parent) :
    QObject(parent)
{    
    setRawData(rawData);
//    initPacket();
}

ResponsePacket::~ResponsePacket()
{
//    if (parent()->inherits("ResponsePacket"))
//        qDebug() << this << parent();
}

QByteArray ResponsePacket::packet() const
{
    return m_rawData;
}

void ResponsePacket::setRawData(const QByteArray &rawData)
{   
    m_rawData.append(rawData);
    //    initRawData();
}

void ResponsePacket::clearRawData()
{
    m_rawData.clear();
}

void ResponsePacket::init()
{
    m_list = initRawData(m_rawData);
    if (m_list.count() > 1)
        qDebug() << m_list;
    foreach (const QByteArray &packet, m_list) {
        validatePacket(packet);
    }
}

QList<QByteArray> ResponsePacket::initRawData(const QByteArray &raw)
{
    QByteArray rawData = raw;
    QList<QByteArray> list;

    if (rawData.length() < 6) {
        qDebug() << "Paket uzunlugu yetersiz!" << rawData;
        return list;
    }

    bool cikis = false;

    do {
        if (rawData.length() < 6) {
            qDebug() << "Paket uzunlugu hatali-1!" << rawData;
            cikis = true;
        }
        else {
            const int idxOfFirstSOP = rawData.indexOf(SOP); // ilk AA nin indeksi alinir.

            if (idxOfFirstSOP == -1) { // Pakette AA bulunamadiysa cikilir.
                qDebug() << "Pakette [AA] yok!" << rawData;
                cikis = true;
            }
            else {
                if (idxOfFirstSOP > 0) // Bulunan ilk AA paketin ilk elemani degilse, AA nin indeksinden itibaren olan kismi m_packet olarak ata.
                    rawData = rawData.mid(idxOfFirstSOP);

                if (rawData.length() < 6) {
                    qDebug() << "Paket uzunlugu hatali-2!";
                    cikis = true;
                }
                else {
                    if (rawData.at(ADR_FNC) > 0 && rawData.at(ADR_FNC) < 4) {
                        int dataLength = rawData.at(ADR_LEN);
                        int estimatedPacketLen = dataLength + 2 + 2 +1;

                        if (rawData.length() < estimatedPacketLen) {// rawData nin uzunlugu beklenen paket uzunlugundan kisa ise.
                            qDebug() << "Paket uzunlugu hatali-3!";
                            cikis = true;
                        }
                        else {
                            if (rawData.at(estimatedPacketLen - 1) == EOP) // beklenen paket sonu [55] ise [AA] ile baslayip [55] ile biten uygun bir paket bulundu demektir.
                                list.append(rawData.mid(0, estimatedPacketLen));

                            rawData = rawData.mid(estimatedPacketLen); // uygun paketin bulunup bulunmadigina bakilmaksizin, rawData beklenen paket boyutu kadar kaydirilir.

                            if (rawData.isEmpty())
                                cikis = true;
                        }
                    }
                    else { // muhtemel olmasi beklenen paketin fonksiyon adresindeki deger aralik disinda
                        rawData = rawData.mid(1);
                    }
                }
            }
        }

    } while (!cikis);

    return list;
}

bool ResponsePacket::isValid()
{
    return m_valid;
}

const QByteArray &ResponsePacket::request() const
{
    return m_request;
}

const QByteArray &ResponsePacket::response() const
{
    return m_response;
}

const QString &ResponsePacket::status() const
{
    return m_status;
}

static bool checkPacketCrc(const QByteArray &packet)
{
    uchar crc = 0;
    const int start = ResponsePacket::ADR_LEN +1;
    const int stop  = ResponsePacket::ADR_LEN + packet.at(ResponsePacket::ADR_LEN);
    const uchar CRC = (uchar)packet.at(packet.length() -2);

    for(int i = start;  i <= stop; i++) {
        crc += packet.at(i);
    }

    bool ret = crc == CRC ? true : false;

    if (!ret)
        qDebug() << QString("CRC Error: Calculated is %1 and Received is %2. Packet: %3")
                            .arg(crc, 0, 16)
                            .arg(CRC, 0, 16)
                            .arg(QString(packet.toHex(':').toUpper()));
    return ret;
}

bool ResponsePacket::validatePacket(const QByteArray &response)
{
    bool valid = false;

    if (!checkPacketCrc(response))
        return valid;

    switch (response.at(ADR_FNC))
    {
        case FC_WRITE_MEM:
        case FC_WATCH_CONF:
        case FC_CONNECT: {
            const uchar pacStatus = (uchar)response.at(ADR_LEN + 1);
            if (checkPacketStatus(pacStatus) == Status::RESP_OK)
                valid = true;
        } break;

        case FC_WATCH_VARS: {
            valid = true;
//            qDebug() << "FC_WATCH_VARS";
//            uchar dataCount = m_packet.at(ADR_LEN + 1);
//            QList<ushort> listData;

//            for (int i=(ADR_LEN + 2); i < (ADR_LEN + 2 + dataCount*2); i+=2) {
//                uchar lsb = m_packet.at(i);
//                uchar msb = m_packet.at(i+1);

//                ushort data = ((ushort)msb << 8) | (ushort)lsb;
//                listData.append(data);
//            }

//            emit responseData(listData);

        } break;

        default:
            break;
    }

//    if (valid)
//        emit responsePacket(response);

    return valid;
}

ResponsePacket::Status ResponsePacket::checkPacketStatus(const uchar &packetStatus)
{
    switch (packetStatus) {
    case PCK_NOT_RDY:
        m_status = QString("PCK_NOT_RDY");
        emit responseStatus(m_status);
        return PCK_NOT_RDY;
    case PCK_READY:
        m_status = QString("PCK_READY");
        emit responseStatus(m_status);
        return PCK_READY;
    case RESP_OK:
        m_status = QString("RESP_OK");
        emit responseStatus(m_status);
        return RESP_OK;
    case PCK_INV_ID:
        m_status = QString("PCK_INV_ID");
        emit responseStatus(m_status);
        return PCK_INV_ID;
    case PCK_CHK_ERR:
        m_status = QString("PCK_CHK_ERR");
        emit responseStatus(m_status);
        return PCK_CHK_ERR;
    case PCK_OVERFLOW:
        m_status = QString("PCK_OVERFLOW");
        emit responseStatus(m_status);
        return PCK_OVERFLOW;
    case PCK_INV_EOP:
        m_status = QString("PCK_INV_EOP");
        emit responseStatus(m_status);
        return PCK_INV_EOP;
    default:
        m_status = QString("UNKNOWN_STATUS");
        emit responseStatus(m_status);
        return PCK_UNKNOWN;
    }
}

