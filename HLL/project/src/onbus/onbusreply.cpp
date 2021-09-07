#include "onbusreply.h"
#include <QtCore/private/qobject_p.h>

class OnBusReplyPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(OnBusReply)

public:
    OnBusReplyPrivate() {}
    OnBusDataUnit m_unit;
    bool m_finished = false;
    OnBusDevice::Error m_error = OnBusDevice::NoError;
    QString m_errorText;
    OnBusResponse m_response;
    OnBusReply::ReplyType m_type;
};


OnBusReply::OnBusReply(ReplyType type, QObject *parent)
    : QObject(*new OnBusReplyPrivate, parent)
{
    Q_D(OnBusReply);
    d->m_type = type;
}

bool OnBusReply::isFinished() const
{
    Q_D(const OnBusReply);
    return d->m_finished;
}

void OnBusReply::setFinished(bool isFinished)
{
    Q_D(OnBusReply);
    d->m_finished = isFinished;
    if (isFinished)
        emit finished();
}

OnBusDataUnit OnBusReply::result() const
{
    Q_D(const OnBusReply);
    if (type() == OnBusReply::Common)
        return d->m_unit;
    return OnBusDataUnit();
}

void OnBusReply::setResult(const OnBusDataUnit &unit)
{
    Q_D(OnBusReply);
    d->m_unit = unit;
}

OnBusDevice::Error OnBusReply::error() const
{
    Q_D(const OnBusReply);
    return d->m_error;
}

void OnBusReply::setError(OnBusDevice::Error error, const QString &errorText)
{
    Q_D(OnBusReply);
    d->m_error = error;
    d->m_errorText = errorText;
    emit errorOccurred(error);
    setFinished(true);
}

QString OnBusReply::errorString() const
{
    Q_D(const OnBusReply);
    return d->m_errorText;
}

OnBusReply::ReplyType OnBusReply::type() const
{
    Q_D(const OnBusReply);
    return d->m_type;
}

OnBusResponse OnBusReply::rawResult() const
{
    Q_D(const OnBusReply);
    return d->m_response;
}

void OnBusReply::setRawResult(const OnBusResponse &response)
{
    Q_D(OnBusReply);
    d->m_response = response;
}

#include "moc_onbusreply.cpp"
