#ifndef UNIT_H
#define UNIT_H

#include <QObject>
#include <string>

template <typename T>
class UnitData
{
public:
    T data() const { return m_data; }
    void setData(const T &data) { m_data = data; }

    std::string name() const { return m_name; }
    void setName(const std::string &name) { m_name = name; }

private:
    T m_data;
    std::string m_name;
};

class Unit : public QObject
{
    Q_OBJECT
public:
    explicit Unit(QObject *parent = nullptr);

signals:

};

#endif // UNIT_H
