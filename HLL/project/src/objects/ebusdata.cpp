#include "ebusdata.h"

EBusData::EBusData(int device, QModbusDataUnit::RegisterType type, int registerId, int startAddress, QVariant data, QObject *parent) :
    m_mode(ModeFlag::None),
    m_dataType(data.type()),
    m_device(device),
    m_registerType(type),
    m_registerId(registerId),
    m_startAddress(startAddress),
    m_data(data),
    m_alias(QString()),
    QObject(parent)
{
    setObjectName(QString::number(startAddress));
    init();
}

bool EBusData::init()
{
    if (objectName().isEmpty())
        setObjectName(QString::number(m_startAddress));

    m_eData = EDataUtil::create(dataType(), m_data.data());

    const quint8 byteCount = sizeOfDataType();
    const quint8 itemCount = byteCount / 2;

    m_addressList.clear();
    m_addressList.append(m_startAddress);

    for (int i = 1; i < itemCount; i++) {
        m_addressList.append(m_startAddress + i);
    }

    return true;
}

bool EBusData::setDataType(uint type)
{
    if (m_dataType == QMetaType::UnknownType) {
        m_dataType = type;
        return true;
    }

    return false;
}

void EBusData::setData(const void *copy)
{
    if (m_dataType != QMetaType::UnknownType ) {
        m_data = QVariant(m_dataType, copy);
        emit valueChanged(m_data);
    }
}

QString EBusData::toString() const
{
    return m_eData->toString();
}

QString EBusData::toFormattedString() const
{
    QString formatted = m_eData->toString();
    if (m_precision > 0) {
        if (m_precision >= formatted.length())
            formatted.push_front(QChar('0'));
        int pos = formatted.length() - m_precision;
        formatted.insert(pos, QChar(','));
    }

    return formatted;
}
