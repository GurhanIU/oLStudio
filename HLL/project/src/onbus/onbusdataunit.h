#ifndef ONBUSDATAUNIT_H
#define ONBUSDATAUNIT_H

#include "ondata.h"

#include <QList>
#include <QDataStream>

class OnBusData;

class OnBusDataset
{
public:
    OnBusDataset() = default;

private:
    QList<OnData*> m_set;
};



class OnBusDataUnit
{
public:
    enum RegisterType {
        Invalid,
        Read, // Read
        Write, // Write
        Command  // Command
    };
    OnBusDataUnit() = default;

    explicit OnBusDataUnit(RegisterType type) : m_type(type) {}
    explicit OnBusDataUnit(RegisterType type, const QList<OnBusData *> &newDataset) :
        m_type(type),
        m_dataset(newDataset)
    {}

    RegisterType registerType() const { return m_type; }
    void setRegisterType(RegisterType type) { m_type = type; }

    inline QList<OnBusData *> dataset() const { return m_dataset; }
    inline void setDataset(const QList<OnBusData *> &newDataset)
    {
        clear();
        m_dataset = newDataset;
    }

    inline quint8 count() const { return m_dataset.size(); }

    inline void setData(int index, OnBusData *newData)
    {
        if (m_dataset.isEmpty() || index >= m_dataset.size())
            return;
        m_dataset[index] = newData;
    }

    inline void addData(OnBusData *newData) { m_dataset.append(newData); }

    inline OnBusData *data(int index) const { return m_dataset.value(index); }


    QByteArray values() const;
//    QVector<quint8> values() const;

    bool isValid() const { return m_type != Invalid && m_dataset.size() > 0; }

    void clear()
    {
        qDeleteAll(m_dataset); // OnBusData nesneleri siliniyor. Dikkatli ol!!
        m_dataset.clear();
    }

private:
    RegisterType m_type = Invalid;
    QVector<quint8> m_values;
    QList<OnBusData*> m_dataset;
};
typedef QMap<OnBusDataUnit::RegisterType, OnBusDataUnit> OnBusDataUnitMap;

Q_DECLARE_TYPEINFO(OnBusDataUnit, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(OnBusDataUnit::RegisterType, Q_PRIMITIVE_TYPE);

Q_DECLARE_METATYPE(OnBusDataUnit::RegisterType)

#endif // ONBUSDATAUNIT_H
