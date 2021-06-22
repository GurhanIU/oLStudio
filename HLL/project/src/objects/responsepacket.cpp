#include "responsepacket.h"
#include <QDebug>

ResponsePacket::ResponsePacket(QObject *parent) : QObject(parent)
{

}

ResponsePacket::ResponsePacket(const QByteArray &packet, QObject *parent)
    : QObject(parent)
    , m_packet(packet)
{
    initPacket();
}

ResponsePacket::~ResponsePacket()
{
//    qDebug() << Q_FUNC_INFO;
}

QByteArray ResponsePacket::packet() const
{
    return m_packet;
}

void ResponsePacket::setPacket(const QByteArray &packet)
{
    m_packet = packet;
    initPacket();
}

void ResponsePacket::initPacket()
{
    if (m_packet.isEmpty() || m_packet.length() < 6 /*|| m_packet.length() > 260*/) {
        qDebug() << "Paket uzunlugu hatali!";
        return;
    }  

    QByteArray dataPacket;
    bool paketBulundu = false;

    do {
        int idxOfFirstSOP = m_packet.indexOf(SOP); // ilk AA nin indeksi alinir.

        if (idxOfFirstSOP == -1 || m_packet.length() <= idxOfFirstSOP +1) // Pakette AA bulunamadiysa cikilir. // Hamsi :)
            return;

        if (m_packet.at(idxOfFirstSOP +1) < 4) {
            int dataLength = m_packet.at(idxOfFirstSOP + 2);
            int estimatedPacketLen = idxOfFirstSOP + dataLength + 2 + 2 +1;

            if (estimatedPacketLen > m_packet.length())
                return;

            if (m_packet.at(estimatedPacketLen - 1) == EOP) {
                dataPacket = m_packet.mid(idxOfFirstSOP, estimatedPacketLen);

                if (m_packet.length() > idxOfFirstSOP + dataPacket.length()) {
                    QByteArray newPacket = m_packet.mid(dataPacket.length());
                    ResponsePacket *newResponse = new ResponsePacket(newPacket, parent());
                    newResponse->deleteLater();
                }
                paketBulundu = true;
            }
            else {
                m_packet = m_packet.mid(1);
            }
        }
        else {
            m_packet = m_packet.mid(1);
        }

    } while (!paketBulundu);

    m_packet = dataPacket;

    const int ADR_EOP = m_packet.length() -1;
    const int ADR_CRC = ADR_EOP -1;

    int asama = 1;

    uchar len = m_packet.at(ADR_LEN);

    if (!crcCalculation(ADR_LEN +1, ADR_LEN + len, (uchar)m_packet.at(ADR_CRC)))
        return;

    switch (m_packet.at(ADR_FNC)) {
        case FC_WRITE_MEM:
        case FC_WATCH_CONF:
        case FC_CONNECT:
        {
            qDebug() << "XX";
            const uchar pacStatus = (uchar)m_packet.at(ADR_LEN + 1);
            checkPacketStatus(pacStatus);
        }
            break;

        case FC_WATCH_VARS:
        {
            qDebug() << "FC_WATCH_VARS";
            uchar dataCount = m_packet.at(ADR_LEN + 1);
            QList<ushort> listData;

            for (int i=(ADR_LEN + 2); i < (ADR_LEN + 2 + dataCount*2); i+=2) {
                uchar lsb = m_packet.at(i);
                uchar msb = m_packet.at(i+1);

                ushort data = ((ushort)msb << 8) | (ushort)lsb;
                listData.append(data);

//                qDebug() << msb << lsb << data;
            }

//            qDebug() << listData;

            emit responseData(listData);

        }
            break;

        default:
            break;
    }
}

bool ResponsePacket::crcCalculation(int start, int stop, const uchar &packetCrc)
{
    uchar crc = 0;
    for(int i = start;  i <= stop; i++) {
        crc += m_packet.at(i);
    }

    if (crc != packetCrc) {
        qDebug() << QString("CRC Error: Calculated is %1 and Received is %2. Packet: %3")
                    .arg(crc, 0, 16)
                    .arg(packetCrc, 0, 16)
                    .arg(QString(m_packet.toHex(':').toUpper()));
        return false;
    }

    return true;
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

