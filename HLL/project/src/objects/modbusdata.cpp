#include "modbusdata.h"

#include <QModbusDataUnit>
#include <QModbusClient>
//#include <QDebug>

ModbusData::ModbusData(QObject *parent) :
    m_dataType(QMetaType::UnknownType),
    m_address(-1),
    m_alias(QString()),
    QObject(parent)
{

}

ModbusData::ModbusData(int registerId, int address, QVariant data, QObject *parent) :
    m_dataType(data.type()),
    m_registerId(registerId),
    m_address(address),
    m_data(data),
    m_alias(QString()),
    QObject(parent)
{
    setObjectName(QString::number(address));
}

ModbusData::~ModbusData()
{
//    qDebug() << m_alias << m_address << "Siliniyor";
}

void ModbusData::setAlias(const QString &alias)
{
    m_alias = alias;
}

QVariant ModbusData::data() const
{
    return m_data;
}

//void ModbusData::setValue(const QVariant &value)
//{
//    m_value = value;
//}

void ModbusData::setData(uint type, const void *copy)
{
    if (type == (uint)m_dataType || m_dataType == QMetaType::UnknownType ) {
        m_dataType = type;
        m_data = QVariant(type, copy);
        emit valueChanged(m_data);
    }
}

QMetaType::Type ModbusData::dataType() const
{
    return (QMetaType::Type)m_dataType;
}

QString ModbusData::dataTypeName() const
{
    QMetaType mt(m_dataType);
    return mt.typeName(m_dataType);
}

bool ModbusData::setDataType(uint type)
{
    if (m_dataType == QMetaType::UnknownType || m_dataType == type) {
        m_dataType = type;
        return true;
    }

    return false;
}

int ModbusData::registerId() const
{
    return m_registerId;
}

void ModbusData::setRegisterId(int registerId)
{
    m_registerId = registerId;
}

void ModbusData::setAddress(int address)
{
    m_address = address;
}
