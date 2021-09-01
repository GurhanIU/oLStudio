#ifndef MODBUSDATAENTRIES_H
#define MODBUSDATAENTRIES_H

#include <QObject>

class ModbusData;

class ModbusDataEntries : public QObject
{
    Q_OBJECT
public:
    explicit ModbusDataEntries(QObject *parent = nullptr);

    ModbusData *addEntry(int registerId, int address, QVariant data, const QString &alias = QString());

    ModbusData * entry(int address);
    bool hasEntry(int address);
    void deleteEntry(int address);

    typedef QList<ModbusData *> EntryList;
    EntryList allEntries();
    QList<EntryList *> collectSubEntries();
    QList<EntryList *> collectSubEntries(const EntryList &entries);

    int size();
    int count();

public slots:

private:
    void init();

private slots:

signals:
    void sgMessage(const QString &caption, const QString &text);
    void sgProcessRange(int min, int max);
    void sgProcessValue(int value);
};

#endif // MODBUSDATAENTRIES_H
