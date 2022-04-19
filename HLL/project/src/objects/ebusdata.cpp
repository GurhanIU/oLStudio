#include "ebusdata.h"

#include <QVector>
#include <QDataStream>
#include <QDebug>

EBusData::EBusData(int registerId, int startAddress, QVariant::Type type, int precision, const QString &alias, const QString &unit, QObject *parent)
    : EBusData(QModbusDataUnit::Invalid, registerId, startAddress, type, precision, alias, unit, parent)
{
    qDebug() << "ModbusData: qint8";
}

EBusData::EBusData(QModbusDataUnit::RegisterType type, int registerId, int startAddress, QVariant::Type vType, int precision, const QString &alias, const QString &unit, QObject *parent)
    : QObject(parent),
    m_mode(ModeFlag::None),
    m_registerType(type),
    m_dataType(vType),
    m_registerId(registerId),
    m_startAddress(startAddress),
    m_precision(precision),
    m_alias(alias),
    m_unit(unit),
    m_data(vType)
{
    setObjectName(QString::number(startAddress));
    init();
}

EBusData::~EBusData()
{
    /*qDebug() << "Siliniyor:" << this;*/
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

QVector<quint16> &EBusData::dataVector()
{
    m_values.clear();
    const quint8 byteCount = QMetaType::sizeOf(m_dataType);

    const QByteArray byteArray = m_data.toByteArray();

    for (int i = 0; i < byteCount; ) {
        quint16 var = byteArray.at(i++);
        quint8 l = byteArray.at(i++);
        var = (var << 8) | l;
        m_values.append(var);
    }

    return m_values;
}

QVector<quint16> &EBusData::tempVector()
{
    m_tempValues.clear();

    const quint8 byteCount = QMetaType::sizeOf(m_dataType);

//    QDataStream d();
//    d.setByteOrder(QDataStream::BigEndian);
//    d.setFloatingPointPrecision(QDataStream::SinglePrecision);

    if (byteCount == 1) {
        quint8* accessible_data = static_cast<quint8*>(m_tempData.data());
        quint16 var = (quint16)(*accessible_data);
        m_tempValues.append(var);
    } else {
        quint16* accessible_data = static_cast<quint16*>(m_tempData.data());
        for (int i = byteCount/2; i > 0; i--) {
            quint16 var = (quint16)(accessible_data[i-1]);
            m_tempValues.append(var);
    //        d << *v;
        }
    }

    return m_tempValues;
}

bool EBusData::setDataType(QVariant::Type type)
{
    if (m_dataType == QVariant::Type::Invalid) {
        m_dataType = type;
        emit dataTypeChanged(dataType());
        return true;
    }

    return false;
}

void EBusData::setData(const QVariant &data)
{
    m_data = data;

    emit dataChanged(data);
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

qint64 EBusData::timeStamp() const
{
    return m_timeStamp;
}

void EBusData::setTimeStamp(qint64 newTimeStamp)
{
    m_timeStamp = newTimeStamp;
}
