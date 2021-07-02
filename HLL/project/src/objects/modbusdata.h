#ifndef MODBUSDATA_H
#define MODBUSDATA_H

#include <QObject>
#include <QVariant>
#include <QMetaType>
#include <QDebug>

class ModbusData : public QObject, private QVariant
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

    explicit ModbusData(QObject *parent = nullptr);
    explicit ModbusData(int registerId, int address, QVariant data, QObject *parent = nullptr);

    ~ModbusData();

    Mode mode() const { return m_mode; }
    void setMode(const Mode &mode) { m_mode = mode; }

    QMetaType::Type dataType() const;
    QString dataTypeName() const;
    bool setDataType(uint type);

    QVariant data() const;
//    void setValue(const QVariant &value);
    void setData(uint type, const void *copy);
    template<typename T>
    inline void setData(const T &value);

    int registerId() const;
    void setRegisterId(int registerId);

    int address() const { return m_address; }
    void setAddress(int address);

    QString alias() const { return m_alias; }
    void setAlias(const QString &alias);   

signals:
    void valueChanged(QVariant);
    void sgMessage(QString caption, QString text);

public slots:

private:
    Mode m_mode;
    uint m_dataType; // QMetaType::Type bilgisi

    QVariant m_data;
    int m_registerId;
    int m_address;
    QString m_alias;
};

// Sadece primitive turler kabul edilmeli. Pointer olmamali.
template<typename T>
inline void ModbusData::setData(const T &adata)
{
    const uint t = qMetaTypeId<T>();
    QMetaType mt;
    qDebug() << t << mt.typeName(t) <<  this->dataTypeName();
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

Q_DECLARE_OPERATORS_FOR_FLAGS(ModbusData::Mode)

#endif // MODBUSDATA_H
