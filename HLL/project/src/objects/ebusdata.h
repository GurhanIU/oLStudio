#ifndef EBUSDATA_H
#define EBUSDATA_H

#include "edata.h"

#include <QObject>
#include <QMetaType>
#include <QModbusDataUnit>
#include <QVariant>
#include <QPointer>

#include <QDebug>

class QModbusClient;

class EBusData : public QObject //, private QVariant
{
    Q_OBJECT
public:
    enum ModeFlag {
        None = 0x0000,
        ReadOnly = 0x0001,
        WriteOnly = 0x0002,
        ReadWrite = ReadOnly | WriteOnly,
        BufferOnly = 0x0004,
    };
    Q_DECLARE_FLAGS(Mode, ModeFlag)

    explicit EBusData(int registerId, int startAddress, qint8 data, QObject *parent = nullptr)
        : EBusData(0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: qint8"; }
    explicit EBusData(int registerId, int startAddress, quint8 data, QObject *parent = nullptr)
        : EBusData(0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: quint8"; }
    explicit EBusData(int registerId, int startAddress, qint16 data, QObject *parent = nullptr)
        : EBusData(0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: qint16"; }
    explicit EBusData(int registerId, int startAddress, quint16 data, QObject *parent = nullptr)
        : EBusData(0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: quint16"; }
    explicit EBusData(int registerId, int startAddress, qint32 data, QObject *parent = nullptr)
        : EBusData(0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: qint32"; }
    explicit EBusData(int registerId, int startAddress, quint32 data, QObject *parent = nullptr)
        : EBusData(0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: quint32"; }
    explicit EBusData(int registerId, int startAddress, qlonglong data, QObject *parent = nullptr)
        : EBusData(0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: qlonglong"; }
    explicit EBusData(int registerId, int startAddress, qulonglong data, QObject *parent = nullptr)
        : EBusData(0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: qulonglong"; }
//    explicit EBusData(int registerId, int startAddress, QVariant data, QObject *parent = nullptr)
//        : EBusData(0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: AAAAAAAAAAAAAAAAAAA"; }
    explicit EBusData(int device, QModbusDataUnit::RegisterType type, int registerId, int startAddress, QVariant data, QObject *parent = nullptr);

    virtual ~EBusData() { /* qDebug() << m_alias << m_address << "Siliniyor"; */ }

    Mode mode() const { return m_mode; }
    int device() const { return m_device; }

    int registerId() const { return m_registerId; }

    QMetaType::Type dataType() const { return (QMetaType::Type)m_dataType; }
    QString dataTypeName() const { return QMetaType::typeName(m_dataType); }

    QString alias() const { return m_alias; }
    void setAlias(const QString &alias) { m_alias = alias; }

    uint precision() const { return m_precision; }
    void setPrecision(uint newPrecision) { m_precision = newPrecision; }

    short tempValue() const { return m_tempValue; }
    void setTempValue(ushort tempValue) { m_tempValue = tempValue; }

    /******************************************************/

    QModbusDataUnit::RegisterType registerType() const { return m_registerType; }

    inline int startAddress() const { return m_startAddress; }

    template<typename T>
    inline void setData(const T &value);    
    void setData(const void *copy);

    QVariant data() const { return m_data; }

    inline uint dataCount() const { return m_dataCount; }
    inline int sizeOfDataType() const { return QMetaType::sizeOf(m_dataType); } //Returns the size of the type in bytes

    EData *eData() const { return m_eData; }
    void setEData(EData *newEData) { m_eData = newEData; }

    QString toString() const;
    QString toFormattedString() const;

    bool isValid() const { return   m_registerType != QModbusDataUnit::Invalid
                                &&  m_dataType != QMetaType::UnknownType
                                &&  m_startAddress != -1; }

protected:
    virtual bool init();

private:
    EData *m_eData;
    Mode m_mode;
    uint m_device = 0;
    QModbusDataUnit::RegisterType m_registerType;
    uint m_dataType = QMetaType::UnknownType; // QMetaType::Type bilgisi //    QMetaType::Type m_metaType;
    int m_registerId;

    QVariant m_data;    

    int m_startAddress  = -1; // adres listesinin ilk adresi olsun.
    QList<int> m_addressList;
    uint m_dataCount = 0;
    ushort m_tempValue;
    uint m_precision = 0;
    QString m_alias;

    inline void setMode(const Mode &mode) { m_mode = mode; }
    inline void setDevice(int device) { m_device = device; }
    inline void setRegisterType(const QModbusDataUnit::RegisterType &registerType) { m_registerType = registerType; }
    inline void setRegisterId(int registerId) { m_registerId = registerId; }
    inline void setStartAddress(int newAddress) { m_startAddress = newAddress; }
    inline bool setDataType(uint type);
    inline void setDataCount(uint newCount) { m_dataCount = newCount; }

signals:
    void valueChanged(QVariant value);
    void dataChanged(EData *data);
    void sgMessage(const QString &caption, const QString &text, int timeout = 0);
};

// Sadece primitive turler kabul edilmeli. Pointer olmamali.
template<typename T>
inline void EBusData::setData(const T &adata)
{
    const uint t = qMetaTypeId<T>();

    if (t == m_dataType && m_dataType != QMetaType::UnknownType ) {
        m_data = QVariant(t, (void*)&adata);

        m_eData->setData((void*)&adata);

//        emit this->valueChanged(m_data);
        emit this->dataChanged(m_eData);

    }

//    else if(t == QMetaType::QVariant) {
//        QVariant v(adata);
//        setData(v.data());
//    }
}

Q_DECLARE_OPERATORS_FOR_FLAGS(EBusData::Mode)

#endif // EBUSDATA_H
