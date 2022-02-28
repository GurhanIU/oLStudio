#ifndef EBUSDATA_H
#define EBUSDATA_H

#include "edata.h"

#include <QObject>
#include <QMetaType>
#include <QModbusDataUnit>

class EBusData : public QObject
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

    explicit EBusData(int registerId, int startAddress, EData *data, int precision = 0, const QString &alias = QString(), QObject *parent = nullptr)
        : EBusData(QModbusDataUnit::Invalid, registerId, startAddress, data, precision, alias, parent) { qDebug() << "ModbusData: qint8"; }

    explicit EBusData(QModbusDataUnit::RegisterType type, int registerId, int startAddress, EData *data, int precision, const QString &alias, QObject *parent = nullptr);

    virtual ~EBusData() { /*qDebug() << "Siliniyor:" << this;*/ }

    Mode mode() const { return m_mode; }

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
    inline void changeData(const T &value);
    void changeData(QMetaType::Type type, void *data);

    inline uint dataCount() const { return m_dataCount; }
    inline int sizeOfDataType() const { return QMetaType::sizeOf(m_dataType); } //Returns the size of the type in bytes

    EData* data() const;
    void setData(EData *newEData);
    void setData(const void *data);

    QString toString() const;
    QString toFormattedString() const;

    bool isValid() const { return   m_registerType != QModbusDataUnit::Invalid
                                &&  m_dataType != QMetaType::UnknownType
                                &&  m_startAddress != -1; }


protected:
    virtual bool init();

private:
    Mode m_mode;
    QModbusDataUnit::RegisterType m_registerType;
    uint m_dataType = QMetaType::UnknownType; // QMetaType::Type bilgisi //    QMetaType::Type m_metaType;
    int m_registerId;

    EData *m_data;
    int m_startAddress  = -1; // adres listesinin ilk adresi olsun.
    QList<int> m_addressList;
    uint m_dataCount = 0;
    ushort m_tempValue;
    uint m_precision = 0;
    QString m_alias;

    inline void setMode(const Mode &mode) { m_mode = mode; }
    inline void setRegisterType(const QModbusDataUnit::RegisterType &registerType) { m_registerType = registerType; }
    inline void setRegisterId(int registerId) { m_registerId = registerId; }
    inline void setStartAddress(int newAddress) { m_startAddress = newAddress; }
    inline bool setDataType(uint type);
    inline void setDataCount(uint newCount) { m_dataCount = newCount; }

signals:
    void dataChanged(EData *data);
    void dataTypeChanged(QMetaType::Type type);
    void sgMessage(const QString &caption, const QString &text, int timeout = 0);
};

// Sadece primitive turler kabul edilmeli. Pointer olmamali.
template<typename T>
inline void EBusData::changeData(const T &adata)
{
    const uint t = qMetaTypeId<T>();

    if (t == m_dataType && m_dataType != QMetaType::UnknownType ) {
        m_data->setData((void*)&adata);
        emit this->dataChanged(m_data);
    }
}

Q_DECLARE_OPERATORS_FOR_FLAGS(EBusData::Mode)

#endif // EBUSDATA_H
