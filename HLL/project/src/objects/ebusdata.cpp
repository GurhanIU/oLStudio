#include "ebusdata.h"

EBusData::EBusData(QModbusDataUnit::RegisterType type, int registerId, int startAddress, EData *data, int precision, const QString &alias, const QString &unit, QObject *parent) :
    m_mode(ModeFlag::None),
    m_dataType(data->type()),
    m_registerType(type),
    m_registerId(registerId),
    m_startAddress(startAddress),
    m_data(data),
    m_precision(precision),
    m_alias(alias),
    m_unit(unit),
    QObject(parent)
{
    setObjectName(QString::number(startAddress));
    init();
}


bool EBusData::init()
{
    if (objectName().isEmpty())
        setObjectName(QString::number(m_startAddress));

//    m_data = EDataUtil::create(dataType(), m_data.data());

    const quint8 byteCount = sizeOfDataType();
    const quint8 itemCount = byteCount / 2;

    m_addressList.clear();
    m_addressList.append(m_startAddress);

    for (int i = 1; i < itemCount; i++) {
        m_addressList.append(m_startAddress + i);
    }

    return true;
}

const QString &EBusData::unit() const
{
    return m_unit;
}

void EBusData::setUnit(const QString &unit)
{
    m_unit = unit;
}

bool EBusData::setDataType(uint type)
{
    if (m_dataType == QMetaType::UnknownType) {
        m_dataType = type;
        emit dataTypeChanged(dataType());
        return true;
    }

    return false;
}

void EBusData::setData(EData *newEData)
{
    if (m_data)
        delete m_data;
    m_data = newEData;
    setDataType(newEData->type());
    emit dataChanged(m_data);
}

void EBusData::setData(const void *data)
{
    if (m_dataType != QMetaType::UnknownType )
        setData(EDataUtil::create(dataType(), data));
}

void EBusData::changeData(QMetaType::Type type, void *data)
{
    if (type == dataType() && m_dataType != QMetaType::UnknownType ) {
        m_data->setData(data);
        emit dataChanged(m_data);
    }
}

EData *EBusData::data() const
{
    return m_data;
}

QString EBusData::toString() const
{
    return m_data->toString();
}

QString EBusData::toFormattedString() const
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
