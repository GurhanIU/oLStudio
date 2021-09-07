#ifndef ONBUSMASTER_P_H
#define ONBUSMASTER_P_H

#include <QTimer>
#include "onbusmaster.h"
#include "onbuspdu.h"
#include "onbusdevice_p.h"

class OnBusMasterPrivate : public OnBusDevicePrivate
{
    Q_DECLARE_PUBLIC(OnBusMaster)

public:
    OnBusReply *sendRequest(const OnBusRequest &request, const OnBusDataUnit *const unit);

    OnBusRequest createReadRequest(const OnBusDataUnit &data) const;
    OnBusRequest createWriteRequest(const OnBusDataUnit &data) const;
    OnBusRequest createCommandRequest(const OnBusDataUnit &data) const;

    bool processResponse(const OnBusResponse &response, OnBusDataUnit *data);

    bool processReadResponse(const OnBusResponse &response, OnBusDataUnit *data);
    bool collateBytes(const OnBusPdu &pdu, OnBusDataUnit::RegisterType type, OnBusDataUnit *data);

    bool processWriteResponse(const OnBusResponse &response, OnBusDataUnit *data);
    bool collateSingleValue(const OnBusPdu &pdu, OnBusDataUnit::RegisterType type, OnBusDataUnit *data);
    bool collateMultipleValues(const OnBusPdu &response, OnBusDataUnit::RegisterType type, OnBusDataUnit *data);

    bool processCommandResponse(const OnBusResponse &response, OnBusDataUnit *data);

    virtual OnBusReply *enqueueRequest(const OnBusRequest &, const OnBusDataUnit &, OnBusReply::ReplyType ) {
        return nullptr;
    }

    virtual bool isOpen() const { return false; }

    int m_numberOfRetries = 3;
    int m_responseTimeoutDuration = 1000;

    struct QueueElement {
        QueueElement() = default;
        QueueElement(OnBusReply *r, const OnBusRequest &req, const OnBusDataUnit &u, int num, int timeout = -1)
            : reply(r),
              requestPdu(req),
              unit(u),
              numberOfRetries(num)
        {
            if (timeout >= 0) {
                // always the case for TCP
                timer = QSharedPointer<QTimer>::create();
                timer->setSingleShot(true);
                timer->setInterval(timeout);
            }
        }
        bool operator==(const QueueElement &other) const {
            return reply == other.reply;
        }

        QPointer<OnBusReply> reply;
        OnBusRequest requestPdu;
        OnBusDataUnit unit;
        int numberOfRetries;
        QSharedPointer<QTimer> timer;
        QByteArray adu;
        qint64 bytesWritten = 0;
        qint32 m_timerId = INT_MIN;
    };
    void processQueueElement(const OnBusResponse &pdu, const QueueElement &element);

};

#endif // ONBUSMASTER_P_H
