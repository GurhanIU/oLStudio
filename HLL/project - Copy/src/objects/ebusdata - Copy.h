#ifndef EBUSDATA_H
#define EBUSDATA_H

#include <QObject>
#include <QMetaType>
#include <QModbusDataUnit>
#include <QVariant>

#include <QDebug>

class QModbusClient;

class EBusDataPrivate
{
public:
    EBusDataPrivate() { }

};

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
        : EBusData(nullptr, 0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: qint8"; }
    explicit EBusData(int registerId, int startAddress, quint8 data, QObject *parent = nullptr)
        : EBusData(nullptr, 0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: quint8"; }
    explicit EBusData(int registerId, int startAddress, qint16 data, QObject *parent = nullptr)
        : EBusData(nullptr, 0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: qint16"; }
    explicit EBusData(int registerId, int startAddress, quint16 data, QObject *parent = nullptr)
        : EBusData(nullptr, 0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: quint16"; }
    explicit EBusData(int registerId, int startAddress, qint32 data, QObject *parent = nullptr)
        : EBusData(nullptr, 0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: qint32"; }
    explicit EBusData(int registerId, int startAddress, quint32 data, QObject *parent = nullptr)
        : EBusData(nullptr, 0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: quint32"; }
    explicit EBusData(int registerId, int startAddress, qint64 data, QObject *parent = nullptr)
        : EBusData(nullptr, 0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: qint64"; }
    explicit EBusData(int registerId, int startAddress, quint64 data, QObject *parent = nullptr)
        : EBusData(nullptr, 0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: quint64"; }
//    explicit EBusData(int registerId, int startAddress, QVariant data, QObject *parent = nullptr)
//        : EBusData(nullptr, 0, QModbusDataUnit::Invalid, registerId, startAddress, data, parent) { qDebug() << "ModbusData: AAAAAAAAAAAAAAAAAAA"; }
    explicit EBusData(QModbusClient *modbus, int device, QModbusDataUnit::RegisterType type, int registerId, int startAddress, QVariant data, QObject *parent = nullptr);

    virtual ~EBusData() { /* qDebug() << m_alias << m_address << "Siliniyor"; */ }

    Mode mode() const { return m_mode; }
    QModbusClient *modbus() const { return m_modbus; }
    int device() const { return m_device; }

    int registerId() const { return m_registerId; }

    QMetaType::Type dataType() const { return (QMetaType::Type)m_dataType; }
    QString dataTypeName() const { QMetaType mt(m_dataType); return mt.typeName(m_dataType); }

    QString alias() const { return m_alias; }
    void setAlias(const QString &alias) { m_alias = alias; }

    short tempValue() const { return m_tempValue; }
    void setTempValue(quint16 tempValue) { m_tempValue = tempValue; }

    /******************************************************/

    QModbusDataUnit::RegisterType registerType() const { return m_registerType; }

    inline int startAddress() const { return m_startAddress; }

    template<typename T>
    inline void setData(const T &value);    
    void setData(uint type, const void *copy);
    inline void setData(int index, quint16 newData)
    {
        if (m_datas.isEmpty() || index >= m_datas.size())
            return;
        m_datas[index] = newData;
    }

    inline quint16 data(int index) const { return m_datas.value(index); }
    QVariant data() const { return m_data; }

    inline QVector<quint16> datas() const { return m_datas; }
    inline void setDatas(const QVector<quint16> &newDatas);

    inline uint dataCount() const { return m_dataCount; }
    inline int sizeOfDataType() const { return QMetaType::sizeOf(m_dataType); } //Returns the size of the type in bytes

    bool isValid() const { return   m_registerType != QModbusDataUnit::Invalid                                
                                &&  m_dataType != QMetaType::UnknownType
                                &&  m_startAddress != -1; }

protected:
    virtual bool init();

public slots:
    void read();
    void write();
    void writeTempValue();

private:
    Mode m_mode;
    QModbusClient *m_modbus;
    uint m_device = 0;
    QModbusDataUnit::RegisterType m_registerType;
    uint m_dataType = QMetaType::UnknownType; // QMetaType::Type bilgisi //    QMetaType::Type m_metaType;
    int m_registerId;

    QVariant m_data;    

    int m_startAddress  = -1; // adres listesinin ilk adresi olsun.
    QList<int> m_addressList;
    QVector<quint16> m_datas;
    uint m_dataCount = 0;
    short m_tempValue;
    QString m_alias;

    void readReady();
    void writeValue(quint16 value);

    inline void setMode(const Mode &mode) { m_mode = mode; }
    inline void setModbus(QModbusClient *modbus) { m_modbus = modbus; }
    inline void setDevice(int device) { m_device = device; }
    inline void setRegisterType(const QModbusDataUnit::RegisterType &registerType) { m_registerType = registerType; }
    inline void setRegisterId(int registerId) { m_registerId = registerId; }
    inline void setStartAddress(int newAddress) { m_startAddress = newAddress; }
    inline bool setDataType(uint type);
    inline void setDataCount(uint newCount) { m_dataCount = newCount; }

signals:
    void valueChanged(QVariant value);
    void sgMessage(const QString &caption, const QString &text, int timeout = 0);
};

// Sadece primitive turler kabul edilmeli. Pointer olmamali.
template<typename T>
inline void EBusData::setData(const T &adata)
{
    const uint t = qMetaTypeId<T>();
    if (t == m_dataType || m_dataType == QMetaType::UnknownType ) {
        m_dataType = t;
        m_data = QVariant(t, (void*)&adata);
        emit this->valueChanged(m_data);
    }

    else if(t == QMetaType::QVariant) {
        QVariant v(adata);

        setData(v.type(), v.data());
    }
}

Q_DECLARE_OPERATORS_FOR_FLAGS(EBusData::Mode)

//class EBusDataChar : public EBusData
//{
//    Q_OBJECT
//public:
//    explicit EBusDataChar(int registerId, int startAddress, QVariant data, QObject *parent = nullptr)
//        : EBusData(registerId, startAddress, data, parent) { }
//    explicit EBusDataChar(QModbusClient *modbus, int device, QModbusDataUnit::RegisterType type, int registerId, int startAddress, QVariant data, QObject *parent = nullptr)
//        : EBusData(modbus, device, type, registerId, startAddress, data, parent) { }

//    virtual ~EBusDataChar() { }

//private:
//    bool init() { }
//};

#endif // EBUSDATA_H
