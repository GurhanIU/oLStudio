#ifndef EBUSDATA_H
#define EBUSDATA_H

#include "ondata.h"

#include <QObject>
#include <QMetaType>

#include "onbus/onbusdataunit.h"



class OnBusData : public QObject
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

    explicit OnBusData(int registerId, int startAddress, OnData *data, int precision = 0, const QString &alias = QString(), QObject *parent = nullptr)
        : OnBusData(OnBusDataUnit::Invalid, registerId, startAddress, data, precision, alias, parent) { }

    explicit OnBusData(OnBusDataUnit::RegisterType type, int registerId, int startAddress, OnData *data, int precision, const QString &alias, QObject *parent = nullptr);

    virtual ~OnBusData() { }

    Mode mode() const { return m_mode; }
    inline void setMode(const Mode &mode) { m_mode = mode; }

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

    OnBusDataUnit::RegisterType registerType() const { return m_registerType; }

    inline int address() const { return m_address; }

    template<typename T>
    inline void changeData(const T &value);
    void changeData(QMetaType::Type type, void *data);

    inline int sizeOfDataType() const { return QMetaType::sizeOf(m_dataType); } //Returns the size of the type in bytes

    OnData* data() const;
    void setData(OnData *newOnData);
    void setData(const void *data);

    QString toString() const;
    QString toFormattedString() const;

    bool isValid() const { return   m_registerType != OnBusDataUnit::Invalid
                                &&  m_dataType != QMetaType::UnknownType
                                &&  m_address != -1; }

private:
    Mode m_mode;
    OnBusDataUnit::RegisterType m_registerType;
    uint m_dataType = QMetaType::UnknownType; // QMetaType::Type bilgisi //    QMetaType::Type m_metaType;
    int m_registerId;

    OnData *m_data;
    int m_address  = -1; // adres listesinin ilk adresi olsun.
    ushort m_tempValue;
    uint m_precision = 0;
    QString m_alias;

    inline void setRegisterType(const OnBusDataUnit::RegisterType &registerType) { m_registerType = registerType; }
    inline void setRegisterId(int registerId) { m_registerId = registerId; }
    inline void setAddress(int address) { m_address = address; }
    inline bool setDataType(uint type);

signals:
    void dataChanged(OnData *data);
    void dataTypeChanged(QMetaType::Type type);
    void sgMessage(const QString &caption, const QString &text, int timeout = 0);
};

// Sadece primitive turler kabul edilmeli. Pointer olmamali.
template<typename T>
inline void OnBusData::changeData(const T &adata)
{
    const uint t = qMetaTypeId<T>();

    if (t == m_dataType && m_dataType != QMetaType::UnknownType ) {
        m_data->setData((void*)&adata);
        emit this->dataChanged(m_data);
    }
}

Q_DECLARE_OPERATORS_FOR_FLAGS(OnBusData::Mode)

#endif // EBUSDATA_H
