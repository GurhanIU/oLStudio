#include "ebusdata.h"

#include <QModbusDataUnit>
#include <QModbusClient>

EBusData::EBusData(QModbusClient *modbus, int device, QModbusDataUnit::RegisterType type, int registerId, int startAddress, QVariant data, QObject *parent) :
    m_mode(ModeFlag::None),
    m_dataType(data.type()),
    m_modbus(modbus),
    m_device(device),
    m_registerType(type),
    m_registerId(registerId),
    m_startAddress(startAddress),
    m_data(data),
    m_alias(QString()),
    QObject(parent)
{
    setObjectName(QString::number(startAddress));
    qDebug() << "ModbusData: QVariant" << data.typeName();
    init();
}

bool EBusData::init()
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

    qDebug() << objectName() << "SizeOfDataType:" << byteCount << "Data:" << data() << m_addressList;

    return true;
}

void EBusData::read()
{
    if (!m_modbus)
        return;

    if (auto *reply = m_modbus->sendReadRequest(QModbusDataUnit(m_registerType, m_startAddress, sizeOfDataType()), m_device)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &EBusData::readReady);
        else
            delete reply; // broadcast replies return immediately
    }
}

void EBusData::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        setData(unit.value(0));
    }
    else if (reply->error() == QModbusDevice::ProtocolError) {
        emit sgMessage(tr("Modbus Error"),
                  tr("Read response error: %1 (Mobus exception: 0x%2)").
                    arg(reply->errorString()).
                    arg(reply->rawResult().exceptionCode(), -1, 16));
    }
    else
    {
        emit sgMessage(tr("Modbus Error"),
                       tr("Read response error: %1 (code: 0x%2)").
                        arg(reply->errorString()).
                        arg(reply->error(), -1, 16));
    }

    reply->deleteLater();
}

void EBusData::write()
{
    if (!m_modbus)
        return;

    writeValue(m_data.toUInt());
}

void EBusData::writeTempValue()
{
    if (!m_modbus)
        return;

    writeValue(m_tempValue);
}

void EBusData::writeValue(quint16 value)
{
    QModbusDataUnit writeUnit = QModbusDataUnit(m_registerType, m_startAddress, 1);
    writeUnit.setValue(0, value);

    if (auto *reply = m_modbus->sendWriteRequest(writeUnit, m_device)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    emit sgMessage(tr("Modbus Error"),
                                   tr("Write response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16));
                }
                else if (reply->error() != QModbusDevice::NoError) {
                    emit sgMessage(tr("Modbus Error"),
                                   tr("Write response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16));
                }
                reply->deleteLater();
            });
        }
        else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    }
    else {
        emit sgMessage(tr("Modbus Error"),
                       tr("Write error: %1").arg(m_modbus->errorString()));
    }
}

bool EBusData::setDataType(uint type)
{
    if (m_dataType == QMetaType::UnknownType || m_dataType == type) {
        m_dataType = type;
        return true;
    }

    return false;
}

void EBusData::setData(uint type, const void *copy)
{
    if (type == (uint)m_dataType || m_dataType == QMetaType::UnknownType ) {
        m_dataType = type;
        m_data = QVariant(type, copy);
        emit valueChanged(m_data);
    }
}

void EBusData::setDatas(const QVector<quint16> &newDatas)
{
    m_datas = newDatas;
    m_dataCount = newDatas.size();
}
