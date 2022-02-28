#ifndef EVENTDATA_H
#define EVENTDATA_H

#include <QObject>
#include "ebusdata.h"

class EventData : public QObject
{
    Q_OBJECT
public:
    EventData(QObject *parent = nullptr);

    int id() const;
    void setId(int id);

    int no() const;
    void setNo(int no);

private:
    int m_id;
    int m_no;


};

#endif // EVENTDATA_H
