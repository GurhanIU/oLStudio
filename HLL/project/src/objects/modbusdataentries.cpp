#include "modbusdataentries.h"

#include "modbusdata.h"

#include <QMessageBox>
#include <QModbusClient>

#include <QDebug>

ModbusDataEntries::ModbusDataEntries(QObject *parent) : QObject(parent)
{
    init();
}

void ModbusDataEntries::init()
{
//    connect(this, &ModbusDataEntries::sgMessage, this, &ModbusDataEntries::slModbusMessage);
}

ModbusData *ModbusDataEntries::entry(int address)
{
    ModbusData *entry = findChild<ModbusData *>(QString::number(address));

    return entry;
}

ModbusDataEntries::EntryList ModbusDataEntries::allEntries()
{
    // Entry lerin sirali oldugu varsayiliyor.
    // Siralama algoritmasi isletilmesi gerekiyor.
    return findChildren<ModbusData *>();
}

/**
 * @brief ModbusDataEntries::addEntry
 * @param device
 * @param address
 * @param value
 * @param alias
 * @return Parent object has not address return true, else return false
 */
ModbusData * ModbusDataEntries::addEntry(int registerId, int address, QVariant data, const QString &alias)
{
    ModbusData *entry;
    if (!hasEntry(address)) {
        entry = new ModbusData(registerId, address, data, this);
        entry->setAlias(alias);

        connect(entry, &ModbusData::sgMessage, this, &ModbusDataEntries::sgMessage);        
    }
    else
        entry = this->entry(address);

    return entry;
}

bool ModbusDataEntries::hasEntry(int address)
{
    ModbusData *entry = findChild<ModbusData *>(QString::number(address));

    return entry ? true : false;
}

void ModbusDataEntries::deleteEntry(int address)
{
    ModbusData *e = entry(address);

    if (e)
        delete e;
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
            ModbusData *modbusData = entries.at(i);
            ModbusData *lastModbusData = pList->last();
            int difference = modbusData->address() - lastModbusData->address();

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
