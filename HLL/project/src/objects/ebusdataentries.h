#ifndef EBUSDATAENTRIES_H
#define EBUSDATAENTRIES_H

#include <QObject>
#include <QModbusDataUnit>
#include <QTimer>

class EData;
class EBusData;
class QModbusClient;

class EBusDataEntries : public QObject
{
    Q_OBJECT
public:
    explicit EBusDataEntries(QObject *parent = nullptr) : EBusDataEntries(nullptr, 0, parent) { }
    explicit EBusDataEntries(QModbusClient *modbus, int device, QObject *parent = nullptr);

    QModbusClient *modbus() const;
    void setModbus(QModbusClient *modbus);

    int device() const;
    void setDevice(int device);

    EBusData *entry(int address);
    EBusData *addEntry(int registerId, int address, EData *data, int precision = 0, const QString &alias = QString());

    bool hasEntry(int address);
    bool hasEntry(EBusData *e);
    void deleteEntry(int address);
    void deleteAll();

    QModbusDataUnit::RegisterType registerType() const;
    void setRegisterType(const QModbusDataUnit::RegisterType &registerType);

    typedef QList<EBusData *> EntryList;
    EntryList allEntries();
    QList<EntryList *> collectSubEntries();
    QList<EntryList *> collectSubEntries(const EntryList &entries);

    void readByEntry(EBusData *e);
    void readByEntry(int address);

    void writeByEntry(EBusData *e);
    void writeByEntry(int address);
    void writeByEntry(int address, ushort value);
    void writeTempValueByEntry(EBusData *e);

    void writeEntries(const EntryList &entryList);
    void readEntries(const EntryList &entryList);

    static int calculateRegisterCount(const EntryList &entries);


public slots:
    void readAllEntries();
    void writeAllEntries();

    void startReadContinuously(int msecs);
    void stopReadContinuously();

private:
    QModbusClient *m_modbus;
    QTimer m_readTimer;
    int m_device;
    QModbusDataUnit::RegisterType m_registerType;

    int m_proccesValue;

    void init();

    void readReady();
    QList<QModbusDataUnit> prepareDataUnitsForWrite(const QList<EntryList *> &entries);

    void writeDataUnit(const QModbusDataUnit &unit);

private slots:
    void slModbusMessage(const QString &caption, const QString &text);

signals:
    void sgMessage(const QString &caption, const QString &text, int timeout = 0);
    void sgProcessRange(int min, int max);
    void sgProcessValue(int value);
};

#endif // EBUSDATAENTRIES_H
