#ifndef EBUSDATA_H
#define EBUSDATA_H

#include <QObject>
#include <QMetaType>
#include <QModbusDataUnit>
#include <QVariant>
#include <QDebug>

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

    explicit EBusData(int registerId, int startAddress, QVariant data, int precision = 0, const QString &alias = QString(), const QString &unit = QString(), QObject *parent = nullptr)
        : EBusData(QModbusDataUnit::Invalid, registerId, startAddress, data, precision, alias, unit, parent) { qDebug() << "ModbusData: qint8"; }

    explicit EBusData(QModbusDataUnit::RegisterType type, int registerId, int startAddress, QVariant data, int precision, const QString &alias, const QString &unit, QObject *parent = nullptr);

    virtual ~EBusData() { /*qDebug() << "Siliniyor:" << this;*/ }

    void init();

    Mode mode() const { return m_mode; }

    int registerId() const { return m_registerId; }

    QMetaType::Type dataType() const { return (QMetaType::Type)m_dataType; }
    QString dataTypeName() const { return QMetaType::typeName(m_dataType); }

    QString alias() const { return m_alias; }
    void setAlias(const QString &alias) { m_alias = alias; }

    uint precision() const { return m_precision; }
    void setPrecision(uint newPrecision) { m_precision = newPrecision; }

    QVariant tempData() const;
    void setTempData(QVariant data);

    /******************************************************/

    QModbusDataUnit::RegisterType registerType() const { return m_registerType; }

    QVariant data() const;
    void setData(QVariant data);

    inline int startAddress() const { return m_startAddress; }
    int sizeOfDataType() const; //Returns the size of the type in bytes

    bool isValid() const { return   m_registerType != QModbusDataUnit::Invalid
                                    && m_dataType != QMetaType::UnknownType
                                    && m_startAddress != -1; }

    const QString &unit() const;
    void setUnit(const QString &unit);

    QVector<quint16> &values();
    QVector<quint16> &tempValues();

    QString toString() const;
    QString toFormattedString() const;

private:
    Mode m_mode;
    QModbusDataUnit::RegisterType m_registerType;
    uint m_dataType = QMetaType::UnknownType; // QMetaType::Type bilgisi //    QMetaType::Type m_metaType;
    int m_registerId;

    int m_startAddress  = -1; // adres listesinin ilk adresi olsun.
    uint m_precision = 0;
    QString m_alias;
    QString m_unit;

    QVariant m_data;
    QVector<quint16> m_values;

    QVariant m_tempData;
    QVector<quint16> m_tempValues;

    inline void setMode(const Mode &mode) { m_mode = mode; }
    inline void setRegisterType(const QModbusDataUnit::RegisterType &registerType) { m_registerType = registerType; }
    inline void setRegisterId(int registerId) { m_registerId = registerId; }
    inline void setStartAddress(int newAddress) { m_startAddress = newAddress; }
    inline bool setDataType(uint type);

signals:
    void dataChanged(QVariant data);
    void dataTypeChanged(QMetaType::Type type);
    void sgMessage(const QString &caption, const QString &text, int timeout = 0);
};

// Sadece primitive turler kabul edilmeli. Pointer olmamali.
//template<typename T>
//inline void EBusData::changeData(const T &adata)
//{
//    const uint t = qMetaTypeId<T>();

//    if (t == m_dataType && m_dataType != QMetaType::UnknownType ) {
//        m_data->setData((void*)&adata);
//        emit this->dataChanged(m_data);
//    }
//}

Q_DECLARE_OPERATORS_FOR_FLAGS(EBusData::Mode)

#endif // EBUSDATA_H
