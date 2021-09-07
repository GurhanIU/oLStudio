#ifndef ONBUSREPLY_H
#define ONBUSREPLY_H

#include "onbusdataunit.h"
#include "onbuspdu.h"
#include "onbusdevice.h"

#include <QObject>

class OnBusReplyPrivate;

class OnBusReply : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OnBusReply)

public:
    enum ReplyType {
        Raw,
        Common,
        Broadcast
    };
    Q_ENUM(ReplyType)

    OnBusReply(ReplyType type, QObject *parent = nullptr);

    ReplyType type() const;

    bool isFinished() const;

    OnBusDataUnit result() const;
    OnBusResponse rawResult() const;

    QString errorString() const;
    OnBusDevice::Error error() const;

    void setResult(const OnBusDataUnit &unit);
    void setRawResult(const OnBusResponse &unit);

    void setFinished(bool isFinished);
    void setError(OnBusDevice::Error error, const QString &errorText);

signals:
    void finished();
    void errorOccurred(OnBusDevice::Error error);
};
Q_DECLARE_TYPEINFO(OnBusReply::ReplyType, Q_PRIMITIVE_TYPE);


#endif // ONBUSREPLY_H
