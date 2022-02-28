#include "deviceevent.h"

DeviceEvent::DeviceEvent(int id) : m_id(id)
{

}

DeviceEvent::~DeviceEvent()
{

}

void DeviceEvent::addEntry(ModbusData *entry)
{
    if (!hasEntry(entry))
        m_entries.append(entry);
}

bool DeviceEvent::hasEntry(ModbusData *entry)
{
    foreach (ModbusData *e, m_entries) {
        if (e == entry)
            return true;
    }

    return false;
}

DeviceEvent::EntryList DeviceEvent::entries()
{
    EntryList list;

    foreach (ModbusData *e, m_entries) {
        list.append(e);
    }

    return list;
}

int DeviceEvent::id() const
{
    return m_id;
}
