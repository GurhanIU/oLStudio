#ifndef EDATA_H
#define EDATA_H

#include <qglobal.h>
#include <QDebug>

//template<typename DataType>
//class EData
//{
//public:
//    explicit EData() {}
//    virtual ~EData() {}

//    virtual DataType data() = 0;
//    virtual void setData(void *) = 0;
//    virtual void setData(DataType&) = 0;
//    virtual void print() = 0;
//};

class EData
{
public:
    explicit EData() {}
    virtual ~EData() {}

    virtual void setData(const void *) = 0;

    virtual QMetaType::Type type() = 0;

    virtual char toChar(bool *ok) { *ok = false; }
    virtual uchar toUChar(bool *ok) { *ok = false; }
    virtual short toShort(bool *ok) { *ok = false; }
    virtual ushort toUshort(bool *ok) { *ok = false; }
    virtual long toLong(bool *ok) { *ok = false; }
    virtual ulong toULong(bool *ok) { *ok = false; }
    virtual qlonglong toLongLong(bool *ok) { *ok = false; }
    virtual qulonglong toULongLong(bool *ok) { *ok = false; }

    virtual QString toString() const = 0;    
    virtual void print() = 0;
};

Q_DECLARE_METATYPE(EData*)
Q_DECLARE_INTERFACE(EData, "EData") // define this out of namespace scope

class EDataChar final : public EData
{
    Q_INTERFACES(EData)
public:
    explicit EDataChar() {}
    explicit EDataChar(const void *data) { setData(data); }
    explicit EDataChar(const char data) { setData(data); }
    ~EDataChar() { delete m_ptr; }

    QMetaType::Type type() override { return QMetaType::Char; }
    char toChar(bool *ok) { *ok = true; return m_data; }
    char data() const { return m_data; }
    void setData(char data) { m_data = data; m_ptr = &m_data; }
    void setData(const void *data) override { m_ptr = reinterpret_cast<const char*>(data); m_data = *(char*)m_ptr; }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    const char *m_ptr;
    char m_data;
};

class EDataUChar final : public EData
{
public:
    explicit EDataUChar() {}
    explicit EDataUChar(const void *data) { setData(data); }
    explicit EDataUChar(const uchar data) { setData(data); }
    ~EDataUChar() { delete m_ptr; }

    QMetaType::Type type() override { return QMetaType::UChar; }
    uchar toUChar(bool *ok) { *ok = true; return m_data; }
    uchar data() const { return m_data; }
    void setData(uchar data) { m_data = data; }
    void setData(const void *data) override { m_ptr = reinterpret_cast<const uchar*>(data); m_data = *(uchar*)m_ptr; }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }
private:
    const uchar *m_ptr;
    uchar m_data;
};

class EDataShort final : public EData
{
public:
    explicit EDataShort() {}
    explicit EDataShort(const void *data) { setData(data); }
    explicit EDataShort(const short data) { setData(data); }
    ~EDataShort() { delete m_ptr; }

    QMetaType::Type type() override { return QMetaType::Short; }
    short toShort(bool *ok) { *ok = true; return m_data; }
    short data() const { return m_data; }
    void setData(short data) { m_data = data; }
    void setData(const void *data) override { m_ptr = reinterpret_cast<const short*>(data); m_data = *(short*)m_ptr; }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    const short *m_ptr;
    short m_data;
};

class EDataUShort final : public EData
{
public:
    explicit EDataUShort() {}
    explicit EDataUShort(const void *data) { setData(data); }
    explicit EDataUShort(const ushort data) { setData(data); }
    ~EDataUShort() { delete m_ptr; }

    QMetaType::Type type() override { return QMetaType::UShort; }
    ushort toUShort(bool *ok) { *ok = true; return m_data; }
    ushort data() const { return m_data; }
    void setData(ushort data) { m_data = data; }
    void setData(const void *data) override { m_ptr = reinterpret_cast<const ushort*>(data); m_data = *(ushort*)m_ptr; }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    const ushort *m_ptr;
    ushort m_data;
};

class EDataLong final : public EData
{
public:
    explicit EDataLong() {}
    explicit EDataLong(const void *data) { setData(data); }
    explicit EDataLong(const long data) { setData(data); }
    ~EDataLong() { delete m_ptr; }

    QMetaType::Type type() override { return QMetaType::Long; }
    long toLong(bool *ok) { *ok = true; return m_data; }
    long data() const { return m_data; }
    void setData(long data) { m_data = data; }
    void setData(const void *data) override { m_ptr = reinterpret_cast<const long*>(data); m_data = *(long*)m_ptr; }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    const long *m_ptr;
    long m_data;
};

class EDataULong final : public EData
{
public:
    explicit EDataULong() {}
    explicit EDataULong(const void *data) { setData(data); }
    explicit EDataULong(const ulong data) { setData(data); }
    ~EDataULong() { delete m_ptr; }

    QMetaType::Type type() override { return QMetaType::ULong; }
    ulong toULong(bool *ok) { *ok = true; return m_data; }
    ulong data() const { return m_data; }
    void setData(ulong data) { m_data = data; }
    void setData(const void *data) override { m_ptr = reinterpret_cast<const ulong*>(data); m_data = *(ulong*)m_ptr; }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    const ulong *m_ptr;
    ulong m_data;
};

class EDataLongLong final : public EData
{
public:
    explicit EDataLongLong() {}
    explicit EDataLongLong(const void *data) { setData(data); }
    explicit EDataLongLong(const qlonglong data) { setData(data); }
    ~EDataLongLong() { delete m_ptr; }

    QMetaType::Type type() override { return QMetaType::LongLong; }
    qlonglong toLongLong(bool *ok) { *ok = true; return m_data; }
    qlonglong data() const { return m_data; }
    void setData(qlonglong data) { m_data = data; }
    void setData(const void *data) override { m_ptr = reinterpret_cast<const qlonglong*>(data); m_data = *(qlonglong*)m_ptr; }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    const qlonglong *m_ptr;
    qlonglong m_data;
};

class EDataULongLong final : public EData
{
public:
    explicit EDataULongLong() {}
    explicit EDataULongLong(const void *data) { setData(data); }
    explicit EDataULongLong(const qulonglong data) { setData(data); }
    ~EDataULongLong() { delete m_ptr; }

    QMetaType::Type type() override { return QMetaType::ULongLong; }
    qulonglong toULongLong(bool *ok) { *ok = true; return m_data; }
    qulonglong data() const { return m_data; }
    void setData(qulonglong data) { m_data = data; }
    void setData(const void *data) override { m_ptr = reinterpret_cast<const qulonglong*>(data); m_data = *(qulonglong*)m_ptr; }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    const qulonglong *m_ptr;
    qulonglong m_data;
};

template <typename T>
class EDataTest : public EData
{
public:
    explicit EDataTest() {}
    explicit EDataTest(const void *data) { setData(data); }
    explicit EDataTest(const T data) { setData(data); }
    ~EDataTest() { delete m_ptr; }

    T data() const { return m_data; }
    void setData(T data) { m_data = data; }
    void setData(const void *data)  { m_ptr = reinterpret_cast<const T*>(data); m_data = *(T*)m_ptr; }
    QString toString() const { return QString::number(m_data); }
    void print()  { qDebug() << m_data; }

private:
    const T *m_ptr;
    T m_data;
};

template class EDataTest<char>;
template class EDataTest<uchar>;
template class EDataTest<short>;
template class EDataTest<ushort>;
template class EDataTest<long>;
template class EDataTest<ulong>;
template class EDataTest<qlonglong>;
template class EDataTest<qulonglong>;

class EDataFactroy
{
public:


private:
    explicit EDataFactroy() { }
};

namespace EDataUtil {

template <typename T>
static T getData( T t, EData *d)
{
   int dType = qMetaTypeId<T>(); //QMetaType::type(typeid(T).name());

   switch (dType) {
   case QMetaType::Char:
   {
       EDataChar *eData = dynamic_cast<EDataChar *>(d);
       t = eData->data();
   } break;
   case QMetaType::UChar:
   {
       EDataUChar *eData = dynamic_cast<EDataUChar *>(d);
       t = eData->data();
   } break;

   case QMetaType::Int:
   case QMetaType::Short:
   {
       EDataShort *eData = dynamic_cast<EDataShort *>(d);
       t = eData->data();
   } break;

   case QMetaType::UInt:
   case QMetaType::UShort:
   {
       EDataUShort *eData = dynamic_cast<EDataUShort *>(d);
       t = eData->data();
   } break;

   case QMetaType::Long:
   {
       EDataLong *eData = dynamic_cast<EDataLong *>(d);
       t = eData->data();
   } break;

   case QMetaType::ULong:
   {
       EDataULong *eData = dynamic_cast<EDataULong *>(d);
       t = eData->data();
   } break;

   case QMetaType::LongLong:
   {
       EDataLongLong *eData = dynamic_cast<EDataLongLong *>(d);
       t = eData->data();
   } break;

   case QMetaType::ULongLong:
   {
       EDataULongLong *eData = dynamic_cast<EDataULongLong *>(d);
       t = eData->data();
   } break;

   default:
       break;
   }

   return t;
}

static EData* create(QMetaType::Type type, const QVariant &data)
{
    EData *edata = nullptr;
    bool ok = false;

    switch (type) {
    case QMetaType::Char: {
        int v = data.toInt(&ok);
        qDebug() << (int)std::numeric_limits<char>::min() << (int)std::numeric_limits<char>::max();
        if (ok && v > (int)std::numeric_limits<char>::min() &&  v < (int)std::numeric_limits<char>::max())
            edata = new EDataChar(static_cast<char>(v));
    } break;

    case QMetaType::UChar: {
        uint v = data.toUInt(&ok);
        qDebug() << (uint)std::numeric_limits<uchar>::min() << (uint)std::numeric_limits<uchar>::max();
        if (ok && v > (uint)std::numeric_limits<uchar>::min() &&  v < (uint)std::numeric_limits<uchar>::max())
            edata = new EDataUChar(static_cast<uchar>(v));
    } break;

    case QMetaType::Short: {
        int v = data.toInt(&ok);
        if (ok && v > std::numeric_limits<short>::min() &&  v < std::numeric_limits<short>::max())
            edata = new EDataShort(static_cast<short>(v));
    } break;

    case QMetaType::UShort: {
        int v = data.toInt(&ok);
        if (ok && v > std::numeric_limits<ushort>::min() &&  v < std::numeric_limits<ushort>::max())
            edata = new EDataUShort(static_cast<ushort>(v));
    } break;

    case QMetaType::Long: {
        int v = data.toInt(&ok);
        if (ok && v > std::numeric_limits<long>::min() &&  v < std::numeric_limits<long>::max())
            edata = new EDataLong(static_cast<long>(v));
    } break;

    case QMetaType::ULong: {
        uint v = data.toUInt(&ok);
        if (ok && v > std::numeric_limits<ulong>::min() &&  v < std::numeric_limits<ulong>::max())
            edata = new EDataULong(static_cast<ulong>(v));
    } break;

    case QMetaType::LongLong: {
        qlonglong v = data.toLongLong(&ok);
        if (ok)
            edata = new EDataLongLong(v);
    } break;

    case QMetaType::ULongLong: {
        qulonglong v = data.toULongLong(&ok);
        if (ok)
            edata = new EDataULongLong(v);
    } break;

    default:
        break;
    }

    return edata;
}

static EData* create(QMetaType::Type type, const void *data = nullptr)
{
    EData *edata = nullptr;
    switch (type) {
    case QMetaType::Char:
        edata = new EDataChar();
    break;

    case QMetaType::UChar:
        edata = new EDataUChar();
    break;

    case QMetaType::Short:
        edata = new EDataShort();
    break;

    case QMetaType::UShort:
        edata = new EDataUShort();
    break;

    case QMetaType::Long:
        edata = new EDataLong();
    break;

    case QMetaType::ULong:
        edata = new EDataULong();
    break;

    case QMetaType::LongLong:
        edata = new EDataLongLong();
    break;

    case QMetaType::ULongLong:
        edata = new EDataULongLong();
    break;

    default:
        break;
    }

    if (data)
        edata->setData(data);

    return edata;
}

template <typename T>
static EData *create(T data)
{
    int dType = qMetaTypeId<T>();
    return create((QMetaType::Type)dType, (void*)&data);
}

template <typename T>
static T reinterpretCastData(T t, void* data)
{
    T *k = reinterpret_cast<T*>(data);
    T d = *(T*)k;
    return d;
}

} //namespace edesigner_internal

#endif // EDATA_H
