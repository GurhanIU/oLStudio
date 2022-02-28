#ifndef ONBUSRTUSERIALMASTER_H
#define ONBUSRTUSERIALMASTER_H

#include "onbusmaster.h"

class OnBusRtuSerialMasterPrivate;

class OnBusRtuSerialMaster : public OnBusMaster
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OnBusRtuSerialMaster)

public:
    explicit OnBusRtuSerialMaster(QObject *parent = nullptr);
    ~OnBusRtuSerialMaster();

    int interFrameDelay() const;
    void setInterFrameDelay(int microseconds);

    int turnaroundDelay() const;
    void setTurnaroundDelay(int turnaroundDelay);

protected:
    OnBusRtuSerialMaster(OnBusRtuSerialMasterPrivate &dd, QObject *parent = nullptr);

    void close() override;
    bool open() override;
};

#endif // ONBUSRTUSERIALMASTER_H
