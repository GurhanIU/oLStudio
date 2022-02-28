#ifndef ONBUSMASTER_H
#define ONBUSMASTER_H

#include "onbusdevice.h"
#include "onbuspdu.h"

#include "onbusreply.h"

class OnBusMasterPrivate;

class OnBusMaster : public OnBusDevice
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OnBusMaster)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)

public:
    explicit OnBusMaster(QObject *parent = nullptr);
    ~OnBusMaster();

    OnBusReply *sendReadRequest(const OnBusDataUnit &read);
    OnBusReply *sendWriteRequest(const OnBusDataUnit &write);
    OnBusReply *sendCommandRequest(const OnBusDataUnit &command);

    OnBusReply *sendRawRequest(const OnBusRequest &request);

    int timeout() const;
    void setTimeout(int newTimeout);

    int numberOfRetries() const;
    void setNumberOfRetries(int number);

Q_SIGNALS:
    void timeoutChanged(int newTimeout);

protected:
    OnBusMaster(OnBusMasterPrivate &dd, QObject *parent = nullptr);

    virtual bool processResponse(const OnBusResponse &response, OnBusDataUnit *data);
    virtual bool processPrivateResponse(const OnBusResponse &response, OnBusDataUnit *data);
};

#endif // ONBUSMASTER_H
