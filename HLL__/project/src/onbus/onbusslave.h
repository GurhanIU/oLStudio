#ifndef ONBUSSLAVE_H
#define ONBUSSLAVE_H

#include "onbusdevice.h"

class OnBusSlave : public OnBusDevice
{
    Q_OBJECT
public:
    explicit OnBusSlave(QObject *parent = nullptr);
};

#endif // ONBUSSLAVE_H
