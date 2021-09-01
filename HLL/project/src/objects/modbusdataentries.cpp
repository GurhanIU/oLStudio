#include "modbusdataentries.h"

#include "ebusdata.h"
#include "onbusmaster.h"
#include "responsepacket.h"

#include <QMessageBox>

#include <QDebug>

ModbusDataEntries::ModbusDataEntries(OnBusMaster *master, QObject *parent) :
    m_onbusMaster(master),
    QObject(parent)
{
    connect(m_onbusMaster, &OnBusMaster::response, this, &ModbusDataEntries::response);
}

EBusData *ModbusDataEntries::entry(int address)
{
    EBusData *entry = findChild<EBusData *>(QString::number(address));

    return entry;
}

ModbusDataEntries::EntryList ModbusDataEntries::allEntries()
{
    // Entry lerin sirali oldugu varsayiliyor.
    // Siralama algoritmasi isletilmesi gerekiyor.
    return findChildren<EBusData *>();
}

/**
 * @brief ModbusDataEntries::addEntry
 * @param device
 * @param address
 * @param value
 * @param alias
 * @return Parent object has not address return true, else return false
 */
EBusData * ModbusDataEntries::addEntry(int registerId, int address, EData *data, int precision, const QString &alias)
{
    EBusData *entry;
    if (!hasEntry(address)) {
        entry = new EBusData(registerId, address, data, precision, alias, this);
        entry->setAlias(alias);

        connect(entry, &EBusData::sgMessage, this, &ModbusDataEntries::sgMessage);
    }
    else
        entry = this->entry(address);

    return entry;
}

bool ModbusDataEntries::hasEntry(int address)
{
    EBusData *entry = findChild<EBusData *>(QString::number(address));

    return entry ? true : false;
}

void ModbusDataEntries::deleteEntry(int address)
{
    EBusData *e = entry(address);

    if (e)
        delete e;
}

void ModbusDataEntries::deleteAll()
{
    qDeleteAll(findChildren<EBusData *>());
}

QList<ModbusDataEntries::EntryList *> ModbusDataEntries::collectSubEntries()
{
    return collectSubEntries(allEntries());
}

QList<ModbusDataEntries::EntryList *> ModbusDataEntries::collectSubEntries(const EntryList &entries)
{
    QList<EntryList *> subEntries;

    if (!entries.isEmpty()) {
        EntryList *pList = new EntryList;
        pList->append(entries.at(0));
        subEntries.append(pList);

        for(int i=1; i < entries.count(); i++) {
            EBusData *modbusData = entries.at(i);
            EBusData *lastModbusData = pList->last();
            int difference = modbusData->startAddress() - lastModbusData->startAddress();

            if (difference > 1) {
                pList = new EntryList;
                subEntries.append(pList);
            }
            pList->append(modbusData);
        }
    }

    return subEntries;
}

int ModbusDataEntries::size()
{
    return allEntries().size();
}

int ModbusDataEntries::count()
{
    return size();
}

QByteArray ModbusDataEntries::prepareRequest(const EntryList &entryList)
{
    if (entryList.isEmpty())
            return QByteArray();
    QByteArray packet;
    packet.append((char)0xAA);
    packet.append((char)FC_WATCH_CONF);
    packet.append((char)(entryList.count()*5 + 1)); // 5: Her veri icin 4byte adres bilgisi ve 1 adet boyut bilgisi; 1: toplam veri adedi
    packet.append((char)entryList.count()); // toplam veri adedi

    char chkSum = (char)entryList.count();

    foreach (EBusData *entry, entryList ) {
        QMetaType t(entry->dataType());
        packet.append((char)entry->startAddress());
        packet.append((char)(entry->startAddress() >> 8));
        packet.append((char)(entry->startAddress() >> 16));
        packet.append((char)(entry->startAddress() >> 24));
        packet.append((char)t.sizeOf());

        chkSum += (char)entry->startAddress();
        chkSum += (char)(entry->startAddress() >> 8);
        chkSum += (char)(entry->startAddress() >> 16);
        chkSum += (char)(entry->startAddress() >> 24);
        chkSum += (char)t.sizeOf();
    }

    packet.append(chkSum);
    packet.append((char) 0x55);

    return packet;
}

void ModbusDataEntries::prepareRequest()
{
    m_request = prepareRequest(allEntries());
}

QByteArray ModbusDataEntries::requestPacket() const
{
    return m_request;
}

void ModbusDataEntries::openCloseRequest()
{ // cihaz tarafinda haberlesmeyi acar
    static const QByteArray data = (QByteArray().append((char)0xAA).append((char)0x03)
                                                .append((char)0x03).append((char)0x00)
                                                .append((char)0x00).append((char)0x00)
                                                .append((char)0x00).append((char)0x55));

    m_onbusMaster->transaction(data);
}

void ModbusDataEntries::sendRequest(const QByteArray &request)
{
    m_onbusMaster->transaction(request);
}

void ModbusDataEntries::response(const QByteArray &rawData)
{
    ResponsePacket packet(rawData, this);
}

void ModbusDataEntries::sendRequest()
{
    prepareRequest();
    m_onbusMaster->transaction(m_request);
}
