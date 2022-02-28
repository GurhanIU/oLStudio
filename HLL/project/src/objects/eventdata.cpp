#include "eventdata.h"

EventData::EventData(QObject *parent) : QObject(parent)
{

}

void EventData::setId(int id)
{
    m_id = id;
}

int EventData::no() const
{
    return m_no;
}

void EventData::setNo(int no)
{
    m_no = no;
}
