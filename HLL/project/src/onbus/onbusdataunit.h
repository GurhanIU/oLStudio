#ifndef ONBUSDATAUNIT_H
#define ONBUSDATAUNIT_H

#include "ondata.h"
#include "onbusdata.h"

class OnBusDataUnit
{
public:
    enum RegisterType {
        Invalid,
        InputRegisters,
        HoldingRegisters
    };
    OnBusDataUnit() = default;

    explicit OnBusDataUnit(RegisterType type, const QList<EBusData *> &newValues) :
        m_type(type),
        m_values(newValues),
        m_valueCount(newValues.count())
    {}

    RegisterType registerType() const { return m_type; }
    void setRegisterType(RegisterType type) { m_type = type; }

    inline QList<EBusData *> values() const { return m_values; }
    inline setValues(const QList<EBusData *> &newValues)
    {
        m_values = newValues;
        m_valueCount = newValues.size();
    }

    inline uint valueCount() const { return m_valueCount; }
    inline setValueCount(uint newCount) { m_valueCount = newCount; }

    inline void setValue(int index, EBusData *newValue)
    {
        if (m_values.isEmpty() || index >= m_values.size())
            return;
        m_values[index] = newValue;
    }

    inline EBusData *value(int index) const { return m_values.value(index); }

    bool isValid() const { return m_type != Invalid && m_values.size() > 0; }

private:
    RegisterType m_type = Invalid;
    QList<EBusData *> m_values;
    uint m_valueCount = 0;
};
typedef QMap<OnBusDataUnit::RegisterType, OnBusDataUnit> OnBusDataUnitMap;

Q_DECLARE_TYPEINFO(OnBusDataUnit, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(OnBusDataUnit::RegisterType, Q_PRIMITIVE_TYPE);

Q_DECLARE_METATYPE(OnBusDataUnit::RegisterType)

#endif // ONBUSDATAUNIT_H
