#include "responsepacket.h"
#include <QDebug>

ResponsePacket::ResponsePacket(QObject *parent) :
    QObject(parent)
{

}

ResponsePacket::ResponsePacket(const QByteArray &rawData, QObject *parent) :
    QObject(parent),
    m_rawData(rawData)
{
//    initPacket();
}

ResponsePacket::~ResponsePacket()
{
    if ( parent()->inherits("ResponsePacket") )
        qDebug() << this << parent();
}

QByteArray ResponsePacket::packet() const
{
    return m_rawData;
}

void ResponsePacket::setRawData(const QByteArray &rawData)
{
    m_rawData = rawData;
//    initRawData();
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
        qDebug() << "Paket uzunlugu hatali!" << rawData;
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

bool ResponsePacket::validatePacket(const QByteArray &packet)
{
    bool isValid = false;

    if (!checkPacketCrc(packet))
        return isValid;

    switch (packet.at(ADR_FNC))
    {
        case FC_WRITE_MEM:
        case FC_WATCH_CONF:
        case FC_CONNECT:
        {
            qDebug() << "XX";
            const uchar pacStatus = (uchar)packet.at(ADR_LEN + 1);
            checkPacketStatus(pacStatus);
        } break;

        case FC_WATCH_VARS:
        {
            isValid = true;
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

    if (isValid)
        emit responsePacket(packet);

    return isValid;
}

void ResponsePacket::checkPacketStatus(const uchar &packetStatus)
{
    switch (packetStatus) {
    case PCK_NOT_RDY:
        emit responseStatus(QString("PCK_NOT_RDY"));
        break;
    case PCK_READY:
        emit responseStatus(QString("PCK_READY"));
        break;
    case RESP_OK:
        emit responseStatus(QString("RESP_OK"));
        break;
    case PCK_INV_ID:
        emit responseStatus(QString("PCK_INV_ID"));
        break;
    case PCK_CHK_ERR:
        emit responseStatus(QString("PCK_CHK_ERR"));
        break;
    case PCK_OVERFLOW:
        emit responseStatus(QString("PCK_OVERFLOW"));
        break;
    case PCK_INV_EOP:
        emit responseStatus(QString("PCK_INV_EOP"));
        break;
    default:
        emit responseStatus(QString("UNKNOWN_STATUS"));
        break;
    }
}

