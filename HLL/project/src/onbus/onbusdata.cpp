#include "onbusdata.h"

OnBusData::OnBusData(OnBusDataUnit::RegisterType type, int registerId, int address, OnData *data, int precision, const QString &alias, QObject *parent) :
    m_mode(ModeFlag::None),
    m_dataType(data->type()),
    m_registerType(type),
    m_registerId(registerId),
    m_address(address),
    m_data(data),
    m_precision(precision),
    m_alias(alias),
    QObject(parent)
{
    setObjectName(QString::number(address));
}

bool OnBusData::setDataType(uint type)
{
    if (m_dataType == QMetaType::UnknownType) {
        m_dataType = type;
        emit dataTypeChanged(dataType());
        return true;
    }

    return false;
}

void OnBusData::setData(OnData *newEData)
{
    if (m_data)
        delete m_data;
    m_data = newEData;
    setDataType(newEData->type());
    emit dataChanged(m_data);
}

void OnBusData::setData(const void *data)
{
    if (m_dataType != QMetaType::UnknownType )
        setData(OnDataUtil::create(dataType(), data));
}

void OnBusData::changeData(QMetaType::Type type, void *data)
{
    if (type == dataType() && m_dataType != QMetaType::UnknownType ) {
        m_data->setData(data);
        emit dataChanged(m_data);
    }
}

OnData *OnBusData::data() const
{
    return m_data;
}

QString OnBusData::toString() const
{
    return m_data->toString();
}

QString OnBusData::toFormattedString() const
{
    if (!m_data->isValid())
        return "NoNe";
    QString formatted = m_data->toString();
    if (m_precision > 0) {
        if (m_precision >= formatted.length())
            formatted.push_front(QChar('0'));
        int pos = formatted.length() - m_precision;
        formatted.insert(pos, QChar(','));
    }

    return formatted;
}
