#ifndef DEVICEEVENT_H
#define DEVICEEVENT_H

#include <QObject>

class ModbusData;

class DeviceEvent
{
public:
    explicit DeviceEvent(int id = 0);
    ~DeviceEvent();

    typedef QList<ModbusData *> EntryList;

    void addEntry(ModbusData *entry);
    bool hasEntry(ModbusData *entry);

    EntryList entries();

    int id() const;

signals:

private:
    int m_id;


    EntryList m_entries;
};

#endif // DEVICEEVENT_H
