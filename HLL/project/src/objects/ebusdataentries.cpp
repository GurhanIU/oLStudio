#include "ebusdataentries.h"

#include "ebusdata.h"

#include <QMessageBox>
#include <QModbusClient>

#include <QDebug>

EBusDataEntries::EBusDataEntries(QModbusClient *modbus, int device, QObject *parent) :
    m_modbus(modbus),
    m_device(device),
    m_registerType(QModbusDataUnit::Invalid),
    QObject(parent)
{
    init();
}

void EBusDataEntries::init()
{
    connect(this, &EBusDataEntries::sgMessage, this, &EBusDataEntries::slModbusMessage);
    connect(&m_readTimer, &QTimer::timeout, this, &EBusDataEntries::readAllEntries);
}

EBusData *EBusDataEntries::entry(int address)
{
    EBusData *entry = findChild<EBusData *>(QString::number(address));

    return entry;
}

EBusDataEntries::EntryList EBusDataEntries::allEntries()
{
    // Entry lerin sirali oldugu varsayiliyor.
    // Siralama algoritmasi isletilmesi gerekiyor.
    return findChildren<EBusData *>();
}

/**
 * @brief EBusDataEntries::addEntry
 * @param device
 * @param address
 * @param value
 * @param alias
 * @return Parent object has not address return true, else return false
 */
EBusData * EBusDataEntries::addEntry(int registerId, int address, QVariant data, int precision, const QString &alias, const QString &unit)
{
    EBusData *entry;
    if (!hasEntry(address)) {
        entry = new EBusData(m_registerType, registerId, address, data, precision, alias, unit, this);
        entry->setAlias(alias);

        connect(entry, &EBusData::sgMessage, this, &EBusDataEntries::sgMessage);
    }
    else
        entry = this->entry(address);

    return entry;
}

bool EBusDataEntries::hasEntry(int address)
{
    EBusData *entry = findChild<EBusData *>(QString::number(address));

    return entry ? true : false;
}

bool EBusDataEntries::hasEntry(EBusData *e)
{
    return hasEntry(e->startAddress());
}

void EBusDataEntries::deleteEntry(int address)
{
    EBusData *e = entry(address);

    if (e)
        delete e;
}

void EBusDataEntries::deleteAll()
{
    qDeleteAll(allEntries());
    allEntries().clear();
}

void EBusDataEntries::readByEntry(EBusData *e)
{
    if (e) {
        const EntryList eList = (EntryList() << e);
        readEntries(eList);
    }
}

void EBusDataEntries::readByEntry(int address)
{
    EBusData *e = entry(address);

    readByEntry(e);
}

void EBusDataEntries::readEntries(const EntryList &entryList)
{
    if (!m_modbus || m_device == 0)
        return;

    QList<EntryList *> subEntries = collectSubEntries(entryList);

    m_proccesValue = 0;
    emit sgProcessRange(0, subEntries.count());
    emit sgProcessValue(m_proccesValue);

    foreach (EntryList *subEntry, subEntries) {
        int regCount = calculateRegisterCount(*subEntry); // Kontrol et
        QModbusDataUnit readUnit = QModbusDataUnit(m_registerType, subEntry->first()->startAddress(), regCount);

        if (auto *reply = m_modbus->sendReadRequest(readUnit, m_device)) {
            if (!reply->isFinished())
                connect(reply, &QModbusReply::finished, this, &EBusDataEntries::readReady);
            else
                delete reply; // broadcast replies return immediately
        }
    }
}

void EBusDataEntries::readAllEntries()
{
    EntryList all = allEntries();
    readEntries(all);    
}

void EBusDataEntries::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();

        for (uint i = 0; i < unit.valueCount(); i++) {
            EBusData *busData = entry(unit.startAddress() + i);

            // Her EBusData::dataCount a bakilarak atama yapilacak
            // Eger 4 byte lik bir degisken ise i fazladan artirilacak

            if (busData) {
                QMetaType::Type dType = busData->dataType();

                if (dType == QMetaType::UnknownType)
                    continue;

                switch (dType) {
                case QMetaType::Char: {
                    const qint8 val = static_cast<qint8>(unit.value(i));
                    busData->setData(val);
                } break;
                case QMetaType::UChar: {
                    const quint8 val = static_cast<quint8>(unit.value(i));
                    busData->setData(val);
                } break;

                case QMetaType::Short: {
                    const qint16 val = static_cast<qint16>(unit.value(i));
                    busData->setData(val);
                } break;

                case QMetaType::UShort: {
                    const quint16 val = static_cast<quint16>(unit.value(i));
                    busData->setData(val);
                } break;

                case QMetaType::Int: {
                    const qint32 val = ((int)unit.value(i) << 16) | (unit.value(i+1));
                    busData->setData(val);
                    i++;
                } break;

                case QMetaType::Long: {
                    const qint32 val = ((long)unit.value(i) << 16) | (unit.value(i+1));
                    busData->setData(val);
                    i++;
                } break;

                case QMetaType::ULong: {
                    const quint32 val = ((ulong)unit.value(i) << 16) | (unit.value(i+1));
                    busData->setData(QVariant(val));
                    i++;
                } break;

                case QMetaType::LongLong: {
                    const qlonglong val1 = ((qlonglong)unit.value(i) << 16) | (unit.value(i+1));
                    const qlonglong val2 = ((qlonglong)unit.value(i+2) << 16) | (unit.value(i+3));
                    const qlonglong val  = ((qlonglong)val1 << 32) | val2;
                    busData->setData(val);
                    i += 3;
                } break;

                case QMetaType::ULongLong: {
                    const qulonglong val1 = ((qulonglong)unit.value(i) << 16) | (unit.value(i+1));
                    const qulonglong val2 = ((qulonglong)unit.value(i+2) << 16) | (unit.value(i+3));
                    const qulonglong val  = ((qulonglong)val1 << 32) | val2;
                    busData->setData(val);
                    i += 3;
                } break;

                default:
                    break;
                }
            }
        }

        emit sgProcessValue(++m_proccesValue);
    }
    else if (reply->error() == QModbusDevice::ProtocolError) {
        emit sgMessage(tr("Modbus Error"),
                       tr("Read response error: %1 (Mobus exception: 0x%2)").
                        arg(reply->errorString()).
                        arg(reply->rawResult().exceptionCode(), -1, 16));
    }
    else {
        emit sgMessage(tr("Modbus Error"),
                       tr("Read response error: %1 (code: 0x%2)").
                        arg(reply->errorString()).
                        arg(reply->error(), -1, 16));
    }

    reply->deleteLater();
}

void EBusDataEntries::writeByEntry(EBusData *e)
{
    if (e) {
        const EntryList eList = (EntryList() << e);
        writeEntries(eList);
    }
}

void EBusDataEntries::writeByEntry(int address)
{
    EBusData *e = entry(address);

    writeByEntry(e);
}

void EBusDataEntries::writeByEntry(int address, ushort  value)
{
    EBusData *e = entry(address);
    if (e) {
        QModbusDataUnit writeUnit = QModbusDataUnit(m_registerType, address, 1);
        writeUnit.setValue(0, value);

        writeDataUnit(writeUnit);
    }
}

void EBusDataEntries::writeTempValueByEntry(EBusData *e)
{
    if (!hasEntry(e))
        return;

    if (e)
        writeDataUnit(QModbusDataUnit(m_registerType, e->startAddress(), e->tempVector()));
}

void EBusDataEntries::writeEntries(const EntryList &entryList)
{
    QList<EntryList *> subEntries = collectSubEntries(entryList);
    QList<QModbusDataUnit> writeUnits = prepareDataUnitsForWrite(subEntries);

    m_proccesValue = 0;
    emit sgProcessRange(0, subEntries.count());
    emit sgProcessValue(m_proccesValue);

    foreach (const QModbusDataUnit &unit, writeUnits) {
        writeDataUnit(unit);
    }
}

void EBusDataEntries::writeAllEntries()
{
    if (!m_modbus || m_device == 0)
        return;
    // Adresler ardisil ilerlemiyorsa, birden fazla paket gonderilecek.
    // Bunun icin once paketler olusturulacak sonrasinda her paket icin
    // ayri ayri yazma istegi gonderilecek.
    // {0, 1, 3, 5, 7, 8 } -> {{0, 1}, {3}, {5}, {7, 8}}

    QList<EntryList *> subEntries = collectSubEntries();
    QList<QModbusDataUnit> writeUnits = prepareDataUnitsForWrite(subEntries);

    m_proccesValue = 0;
    emit sgProcessRange(0, subEntries.count());
    emit sgProcessValue(m_proccesValue);

    foreach (const QModbusDataUnit &unit, writeUnits) {
        writeDataUnit(unit);
    }
}

void EBusDataEntries::startReadContinuously(int msecs)
{
    m_readTimer.start(msecs);
}

void EBusDataEntries::stopReadContinuously()
{
    m_readTimer.stop();
}

void EBusDataEntries::writeDataUnit(const QModbusDataUnit &unit)
{
    if (auto *reply = m_modbus->sendWriteRequest(unit, m_device)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::NoError) {
                    emit sgProcessValue(++m_proccesValue);
                }
                else if (reply->error() == QModbusDevice::ProtocolError) {
                    emit sgMessage(tr("Modbus Error"),
                                   tr("Write response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16));
                }
                else if (reply->error() != QModbusDevice::NoError) {
                    emit sgMessage(tr("Modbus Error"),
                                   tr("Write response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16));
                }

                reply->deleteLater();                
            });
        }
        else {
            reply->deleteLater(); // broadcast replies return immediately
        }
    }
    else {
        emit sgMessage(tr("Modbus Error"),
                       tr("Write error: %1").
                        arg(m_modbus->errorString()));
    }
}

QList<QModbusDataUnit> EBusDataEntries::prepareDataUnitsForWrite(const QList<EntryList *> &entries)
{
    QList<QModbusDataUnit> units;

    foreach (EntryList *entry, entries) {
        QModbusDataUnit writeUnit = QModbusDataUnit(m_registerType,
                                                    entry->first()->startAddress(),
                                                    entry->count());

        // TODO Her entry sadece bir adres bilgisi tutuyorsa bu mantik gecerlidir.
        // Aksi taktirde bir entry birden fazla adres verisinden meydana geliyorsa
        // her adres icin ayri bir write unit olusturmak gerekir.
        for (int i=0; i < entry->count(); i++) {
            EBusData *bus = entry->at(i);
            bool ok = false;
            quint16 value =  static_cast<quint16>(bus->data().toUInt(&ok));
            writeUnit.setValue(i, value);
        }

        units.append(writeUnit);
    }

    return units;
}

QList<EBusDataEntries::EntryList *> EBusDataEntries::collectSubEntries()
{
    return collectSubEntries(allEntries());
}

QList<EBusDataEntries::EntryList *> EBusDataEntries::collectSubEntries(const EntryList &entries)
{
    QList<EntryList *> subEntries;

    if (!entries.isEmpty()) {
        EntryList *pList = new EntryList;
        pList->append(entries.at(0));
        subEntries.append(pList);

        QList<int> addressList;

        foreach (EBusData *busData, entries) {
            addressList.append(busData->startAddress());

            if (QMetaType::sizeOf(busData->dataType()) > 2 )
                addressList.append(busData->startAddress() +1);
        }

        qDebug() << addressList;

        for (int i=1; i < entries.count(); i++) {
            EBusData *modbusData = entries.at(i);
            EBusData *lastModbusData = pList->last();
            int difference = modbusData->startAddress() - lastModbusData->startAddress();

            if (difference > 1 || pList->count() == 125) {
                pList = new EntryList;
                subEntries.append(pList);
            }
            pList->append(modbusData);
        }
    }

    return subEntries;
}

QModbusClient *EBusDataEntries::modbus() const
{
    return m_modbus;
}

void EBusDataEntries::setModbus(QModbusClient *modbus)
{
    m_modbus = modbus;
}

int EBusDataEntries::device() const
{
    return m_device;
}

void EBusDataEntries::setDevice(int device)
{
    m_device = device;
}

QModbusDataUnit::RegisterType EBusDataEntries::registerType() const
{
    return m_registerType;
}

void EBusDataEntries::setRegisterType(const QModbusDataUnit::RegisterType &registerType)
{
    m_registerType = registerType;
}

void EBusDataEntries::slModbusMessage(const QString &caption, const QString &text)
{
//    if (caption.contains("debug", Qt::CaseInsensitive))
//        qDebug() << caption << text;
//    else
//        QMessageBox::critical(NULL, caption, text);
}

int EBusDataEntries::calculateRegisterCount(const EntryList &entries)
{
    int count = 0;

    foreach (EBusData *entry, entries) {
        count += QMetaType::sizeOf(entry->dataType()) / 2;
    }

    return count;
}
