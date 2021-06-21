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

    const int idxOfLastEOP = m_packet.lastIndexOf(EOP);
    const int idxOfFirstEOP = m_packet.indexOf(EOP);

    if (idxOfLastEOP > idxOfFirstEOP) {
        // bu durumda paket icerisinde paket olma ihtimalinin 1. adimi gerceklesmistir.
        if (m_packet.at(idxOfFirstEOP +1) == SOP) {
            // bu durumda paket icerisinde paket olma ihtimalinin 2. adimi gerceklesmistir.
            QByteArray newPacket = m_packet.mid(idxOfFirstEOP +1);
            // Mevcut ResponsePacket nesnesine bagli signal-slot yapilari aktarilamiyor.
            ResponsePacket *newResponse = new ResponsePacket(newPacket, parent());
            newResponse->deleteLater();

            m_packet = m_packet.left(idxOfFirstEOP +1);

//            qDebug() << "XXX" << QString(newPacket.toHex(':').toUpper()) << QString(m_packet.toHex(':').toUpper());
        }
        else
            return;
    }

    const int ADR_EOP = m_packet.length() -1;
    const int ADR_CRC = ADR_EOP -1;

    int asama = 1;

    if (m_packet.at(ADR_SOP) != SOP || m_packet.at(ADR_EOP) != EOP) {
        qDebug() << QString("%1. Asama dogrulama gecilemedi. Paket:").arg(asama++).arg(QString(m_packet.toHex(':').toUpper()));
        return;
    }

    if (m_packet.at(ADR_FNC) < FC_WRITE_MEM || m_packet.at(ADR_FNC) > FC_CONNECT) {
        qDebug() << QString("%1. Asama dogrulama gecilemedi. Paket:").arg(asama++).arg(QString(m_packet.toHex(':').toUpper()));
        return;
    }

    uchar len = m_packet.at(ADR_LEN);

    crcCalculation(ADR_LEN +1, ADR_LEN + len, (uchar)m_packet.at(ADR_CRC));

    switch (m_packet.at(ADR_FNC)) {
    case FC_WRITE_MEM:
    {
//        qDebug() << QString("Fonksiyon: %1").arg("FC_WRITE_MEM");
        const uchar pacStatus = (uchar)m_packet.at(ADR_LEN + 1);
        checkPacketStatus(pacStatus);
    }
        break;
    case FC_WATCH_CONF:
    {
//        qDebug() << QString("Fonksiyon: %1").arg("FC_WATCH_CONF");
        const uchar pacStatus = (uchar)m_packet.at(ADR_LEN + 1);
        checkPacketStatus(pacStatus);
    }
        break;
    case FC_WATCH_VARS:
    {
//        qDebug() << QString("Fonksiyon: %1").arg("FC_WATCH_VARS");

        uchar count = m_packet.at(ADR_LEN + 1);

    }
        break;
    case FC_CONNECT:
    {
//        qDebug() << QString("Fonksiyon: %1").arg("FC_CONNECT");
        const uchar pacStatus = (uchar)m_packet.at(ADR_LEN + 1);
        checkPacketStatus(pacStatus);
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

