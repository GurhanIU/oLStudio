#include "ebusdata.h"

EBusData::EBusData(QModbusDataUnit::RegisterType type, int registerId, int startAddress, QVariant data, int precision, const QString &alias, const QString &unit, QObject *parent) :
    QObject(parent),
    m_mode(ModeFlag::None),
    m_registerType(type),
    m_dataType(data.type()),
    m_registerId(registerId),
    m_data(data),
    m_startAddress(startAddress),
    m_precision(precision),
    m_alias(alias),
    m_unit(unit)
{
    setObjectName(QString::number(startAddress));
    init();
}


void EBusData::init()
{
    if (objectName().isEmpty())
        setObjectName(QString::number(m_startAddress));

    const quint8 byteCount = sizeOfDataType();
    const quint8 itemCount = byteCount / 2;

    m_addressList.clear();
    m_addressList.append(m_startAddress);

    for (int i = 1; i < itemCount; i++) {
        m_addressList.append(m_startAddress + i);
    }

    qDebug() << m_alias << byteCount << m_addressList;
}

QVariant EBusData::tempValue() const
{
    return m_tempValue;
}

void EBusData::setTempValue(QVariant tempValue)
{
    m_tempValue = tempValue;
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

void EBusData::setData(QVariant data)
{
    m_data = data;
    emit dataChanged(m_data);
}

//void EBusData::changeData(QMetaType::Type type, void *data)
//{
//    if (type == dataType() && m_dataType != QMetaType::UnknownType ) {
//        m_data = data;
//        emit dataChanged(m_data);
//    }
//}

QVariant EBusData::data() const
{
    return m_data;
}

QString EBusData::toString() const
{
    return m_data.toString();
}

QString EBusData::toFormattedString() const
{
    if (!m_data.isValid())
        return "NoNe";
    QString formatted = m_data.toString();
    if (m_precision > 0) {
        if (m_precision >= formatted.length())
            formatted.push_front(QChar('0'));
        int pos = formatted.length() - m_precision;
        formatted.insert(pos, QChar(','));
    }

    return formatted;
}
