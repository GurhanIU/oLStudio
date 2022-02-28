#ifndef ONBUS_P_H
#define ONBUS_P_H

#include "onbusdataunit.h"
#include "onbuspdu.h"
#include "onbusreply.h"

#include <QPointer>
#include <QTimer>

class OnBusPrivate
{
public:
    struct QueueElement {
        QueueElement() = default;
        QueueElement(OnBusReply *r, const OnBusRequest &req, const OnBusDataUnit &u, int num, int timeout = -1)
            : reply(r), requestPdu(req), unit(u), numberOfRetries(num)
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

};

#endif // ONBUS_P_H
