#include "ebusdata.h"

#include <QVector>
#include <QDataStream>

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
}

QVariant EBusData::tempData() const
{
    return m_tempData;
}

void EBusData::setTempData(QVariant data)
{
    m_tempData = data;
    qDebug() << m_tempData;
}

const QString &EBusData::unit() const
{
    return m_unit;
}

void EBusData::setUnit(const QString &unit)
{
    m_unit = unit;
}

QVector<quint16> &EBusData::values()
{
    m_values.clear();
    const quint8 byteCount = sizeOfDataType();
    const QByteArray byteArray = m_data.toByteArray();

    for (int i = 0; i < byteCount; ) {
        quint16 var = byteArray.at(i++);
        quint8 l = byteArray.at(i++);
        var = (var << 8) | l;
        m_values.append(var);
    }

    return m_values;
}

quint8 getUint8Val(const void *data)
{
    const quint8 *ptr = reinterpret_cast<const quint8*>(data);
    quint8 var = *(quint8*)ptr;

    return var;
}

//void values(const QVariant var)
//{
//    QVector<quint16> tempValues;
//    const quint8 byteCount = QMetaType::sizeOf(var);


//    for (int i = 0; i < byteCount; ) {
//        size_t offset_1 = i * 8;
//        size_t offset_2 = ++1 * 8;
//        quint16 var = (quint16)setData(m_tempData.constData() + offset_1);
//        quint8 l = setData(m_tempData.constData() + offset_2);
//        var = (var << 8) | l;
//        tempValues.append(var);
//    }
//}

QVector<quint16> &EBusData::tempValues()
{
    m_tempValues.clear();
    quint8* accessible_data = static_cast<quint8*>(m_tempData.data());
    const int byteCount = sizeOfDataType();

//    QDataStream d();
//    d.setByteOrder(QDataStream::BigEndian);
//    d.setFloatingPointPrecision(QDataStream::SinglePrecision);

    for (int i = 0; i < byteCount; i+=2) {
        quint8 *v1 = static_cast<quint8*>(accessible_data + i);
        quint8 *v2 = static_cast<quint8*>(accessible_data + i + 1);

        quint16 var = (quint16)(*v2);
        var = (var << 8)  | (*v1);
        m_tempValues.append(var);
//        d << *v;
    }

    return m_tempValues;
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

int EBusData::sizeOfDataType() const
{
    return QMetaType::sizeOf(m_dataType);
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
