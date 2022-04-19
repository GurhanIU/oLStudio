#ifndef EBUSDATA_H
#define EBUSDATA_H

#include <QObject>
#include <QMetaType>
#include <QModbusDataUnit>
#include <QVariant>

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

    explicit EBusData(int registerId, int startAddress, QVariant::Type type, int precision = 0, const QString &alias = QString(), const QString &unit = QString(), QObject *parent = nullptr);

    explicit EBusData(QModbusDataUnit::RegisterType type, int registerId, int startAddress, QVariant::Type vType, int precision, const QString &alias, const QString &unit, QObject *parent = nullptr);

    virtual ~EBusData();

    void init();

    Mode mode() const { return m_mode; }

    int registerId() const { return m_registerId; }

    QVariant::Type dataType() const { return m_dataType; }
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
    void setData(const QVariant &data);

    inline int startAddress() const { return m_startAddress; }

    bool isValid() const { return   m_registerType != QModbusDataUnit::Invalid
               && m_dataType != QVariant::Type::Invalid /*QMetaType::UnknownType*/
                                    && m_startAddress != -1; }

    const QString &unit() const;
    void setUnit(const QString &unit);

    QVector<quint16> &dataVector();
    QVector<quint16> &tempVector();

    QString toString() const;
    QString toFormattedString() const;

    qint64 timeStamp() const;
    void setTimeStamp(qint64 newTimeStamp);

private:
    Mode m_mode;
    QModbusDataUnit::RegisterType m_registerType;
    QVariant::Type m_dataType = QVariant::Type::Invalid; //QMetaType::UnknownType; // QMetaType::Type bilgisi //    QMetaType::Type m_metaType;
    int m_registerId;

    int m_startAddress  = -1; // adres listesinin ilk adresi olsun.
    int m_precision = 0;
    QString m_alias;
    QString m_unit;

    QVariant m_data;
    QVector<quint16> m_values;

    QVariant m_tempData;
    QVector<quint16> m_tempValues;

    qint64 m_timeStamp;

    inline void setMode(const Mode &mode) { m_mode = mode; }
    inline void setRegisterType(const QModbusDataUnit::RegisterType &registerType) { m_registerType = registerType; }
    inline void setRegisterId(int registerId) { m_registerId = registerId; }
    inline void setStartAddress(int newAddress) { m_startAddress = newAddress; }
    inline bool setDataType(QVariant::Type type);

signals:
    void dataChanged(const QVariant &data);
    void dataTypeChanged(QVariant::Type type);
    void sgMessage(const QString &caption, const QString &text, int timeout = 0);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(EBusData::Mode)

#endif // EBUSDATA_H
