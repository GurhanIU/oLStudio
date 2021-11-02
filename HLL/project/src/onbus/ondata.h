#ifndef EDATA_H
#define EDATA_H

#include <qglobal.h>
#include <QDebug>

class OnData
{
public:
    explicit OnData() {}
    virtual ~OnData() {}

    virtual void setData(const void *) = 0;

    virtual QMetaType::Type type() = 0;

    virtual char toChar(bool *ok) { *ok = false; }
    virtual uchar toUChar(bool *ok) { *ok = false; }
    virtual short toShort(bool *ok) { *ok = false; }
    virtual ushort toUShort(bool *ok) { *ok = false; }
    virtual long toLong(bool *ok) { *ok = false; }
    virtual ulong toULong(bool *ok) { *ok = false; }
    virtual qlonglong toLongLong(bool *ok) { *ok = false; }
    virtual qulonglong toULongLong(bool *ok) { *ok = false; }

    virtual QString toString() const = 0;
    virtual void print() = 0;
    bool isValid() { return m_valid; }

protected:
    bool m_valid = false;
};

Q_DECLARE_METATYPE(OnData*)
Q_DECLARE_INTERFACE(OnData, "OnData") // define this out of namespace scope
Q_DECLARE_TYPEINFO(OnData, Q_MOVABLE_TYPE);

class OnDataChar final : public OnData
{
public:
    explicit OnDataChar() {}
    explicit OnDataChar(const void *data) { setData(data); }
    explicit OnDataChar(const char data) { setData(data); }
    ~OnDataChar() { /*delete m_ptr;*/ }

    QMetaType::Type type() override { return QMetaType::Char; }
    char toChar(bool *ok) { *ok = true; return m_data; }
    char data() const { return m_data; }
    void setData(char data) { m_valid = true; m_data = data;}
    void setData(const void *data) override { const char *ptr = reinterpret_cast<const char*>(data); setData(*(char*)ptr); }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    char m_data;
};

class OnDataUChar final : public OnData
{
public:
    explicit OnDataUChar() {}
    explicit OnDataUChar(const void *data) { setData(data); }
    explicit OnDataUChar(const uchar data) { setData(data); }
    ~OnDataUChar() { /*delete m_ptr;*/ }

    QMetaType::Type type() override { return QMetaType::UChar; }
    uchar toUChar(bool *ok) { *ok = true; return m_data; }
    uchar data() const { return m_data; }
    void setData(uchar data) { m_valid = true; m_data = data; }
    void setData(const void *data) override { const uchar *ptr = reinterpret_cast<const uchar*>(data); setData(*(uchar*)ptr); }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    uchar m_data;
};

class OnDataShort final : public OnData
{
public:
    explicit OnDataShort() {}
    explicit OnDataShort(const void *data) { setData(data); }
    explicit OnDataShort(const short data) { setData(data); }
    ~OnDataShort() { /*delete m_ptr;*/ }

    QMetaType::Type type() override { return QMetaType::Short; }
    short toShort(bool *ok) { *ok = true; return m_data; }
    short data() const { return m_data; }
    void setData(short data) { m_valid = true; m_data = data; }
    void setData(const void *data) override { const short *ptr = reinterpret_cast<const short*>(data); setData(*(short*)ptr); }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    short m_data;
};

class OnDataUShort final : public OnData
{
public:
    explicit OnDataUShort() {}
    explicit OnDataUShort(const void *data) { setData(data); }
    explicit OnDataUShort(const ushort data) { setData(data); }
    ~OnDataUShort() { /*delete m_ptr;*/ }

    QMetaType::Type type() override { return QMetaType::UShort; }
    ushort toUShort(bool *ok) { *ok = true; return m_data; }
    ushort data() const { return m_data; }
    void setData(ushort data) { m_valid = true; m_data = data; }
    void setData(const void *data) override { const ushort *ptr = reinterpret_cast<const ushort*>(data); setData(*(ushort*)ptr); }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    ushort m_data;
};

class OnDataLong final : public OnData
{
public:
    explicit OnDataLong() {}
    explicit OnDataLong(const void *data) { setData(data); }
    explicit OnDataLong(const long data) { setData(data); }
    ~OnDataLong() { /*delete m_ptr;*/ }

    QMetaType::Type type() override { return QMetaType::Long; }
    long toLong(bool *ok) { *ok = true; return m_data; }
    long data() const { return m_data; }
    void setData(long data) { m_valid = true; m_data = data; }
    void setData(const void *data) override { const long *ptr = reinterpret_cast<const long*>(data); setData(*(long*)ptr); }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    long m_data;
};

class OnDataULong final : public OnData
{
public:
    explicit OnDataULong() {}
    explicit OnDataULong(const void *data) { setData(data); }
    explicit OnDataULong(const ulong data) { setData(data); }
    ~OnDataULong() { /*delete m_ptr;*/ }

    QMetaType::Type type() override { return QMetaType::ULong; }
    ulong toULong(bool *ok) { *ok = true; return m_data; }
    ulong data() const { return m_data; }
    void setData(ulong data) { m_valid = true; m_data = data; }
    void setData(const void *data) override { const ulong *ptr = reinterpret_cast<const ulong*>(data); setData(*(ulong*)ptr); }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    ulong m_data;
};

class OnDataLongLong final : public OnData
{
public:
    explicit OnDataLongLong() {}
    explicit OnDataLongLong(const void *data) { setData(data); }
    explicit OnDataLongLong(const qlonglong data) { setData(data); }
    ~OnDataLongLong() { /*delete m_ptr;*/ }

    QMetaType::Type type() override { return QMetaType::LongLong; }
    qlonglong toLongLong(bool *ok) { *ok = true; return m_data; }
    qlonglong data() const { return m_data; }
    void setData(qlonglong data) { m_valid = true; m_data = data; }
    void setData(const void *data) override { const qlonglong *ptr = reinterpret_cast<const qlonglong*>(data); setData(*(qlonglong*)ptr); }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    qlonglong m_data;
};

class OnDataULongLong final : public OnData
{
public:
    explicit OnDataULongLong() {}
    explicit OnDataULongLong(const void *data) { setData(data); }
    explicit OnDataULongLong(const qulonglong data) { setData(data); }
    ~OnDataULongLong() { /*delete m_ptr;*/ }

    QMetaType::Type type() override { return QMetaType::ULongLong; }
    qulonglong toULongLong(bool *ok) { *ok = true; return m_data; }
    qulonglong data() const { return m_data; }
    void setData(qulonglong data) { m_valid = true; m_data = data; }
    void setData(const void *data) override { const qulonglong *ptr = reinterpret_cast<const qulonglong*>(data); m_data = *(qulonglong*)ptr; }
    QString toString() const override { return QString::number(m_data); }
    void print() override { qDebug() << m_data; }

private:
    qulonglong m_data;
};

/*************************************** OnData Factory Pattern ***************************************/
class OnDataInterface
{
public:
    OnDataInterface() {}
    virtual ~OnDataInterface() {}
    virtual std::unique_ptr<OnData> Create() = 0;
};

template <class T>
class OnDataCreator : public OnDataInterface {
public:
    OnDataCreator() {}
    virtual ~OnDataCreator() {}
    virtual std::unique_ptr<OnData> Create() { return std::make_unique<T>(); }

};

class OnDataFactory
{
public:
    OnDataFactory()
    {
        // If you want, you can do all your Register() calls in here, and even
        // make the Register() function private.
        // Or call RegisterAllTypes().
    }

    template <QMetaType::Type type, typename T>
    void Register()
    {
        Register(type, std::make_unique<OnDataCreator<T> >());
    }

    void RegisterAllTypes()
    {
        Register<QMetaType::Char,       OnDataChar>();
        Register<QMetaType::UChar,      OnDataUChar>();
        Register<QMetaType::Short,      OnDataShort>();
        Register<QMetaType::UShort,     OnDataUShort>();
        Register<QMetaType::Long,       OnDataLong>();
        Register<QMetaType::ULong,      OnDataULong>();
        Register<QMetaType::LongLong,   OnDataLongLong>();
        Register<QMetaType::ULongLong,  OnDataULongLong>();
    }

    std::unique_ptr<OnData> Create(QMetaType::Type type)
    {
        TOnDataToVariant::iterator it = m_OnDataToVariant.find(type);
        if (it == m_OnDataToVariant.end())
            return nullptr;
        return it->second->Create();
    }

private:
    void Register(QMetaType::Type type, std::unique_ptr<OnDataInterface>&& creator )
    {
        m_OnDataToVariant[type] = std::move(creator);
    }

    typedef std::map<QMetaType::Type, std::unique_ptr<OnDataInterface> > TOnDataToVariant;
    TOnDataToVariant m_OnDataToVariant;
};

/***************************************** OnDataUtil **********************************************/
namespace OnDataUtil {

template <typename T>
static T getData(T t, OnData *d)
{
   int dType = qMetaTypeId<T>(); //QMetaType::type(typeid(T).name());

   switch (dType) {
   case QMetaType::Char: {
       OnDataChar *eData = dynamic_cast<OnDataChar *>(d);
       t = eData->data();
   } break;
   case QMetaType::UChar: {
       OnDataUChar *eData = dynamic_cast<OnDataUChar *>(d);
       t = eData->data();
   } break;

   case QMetaType::Int:
   case QMetaType::Short: {
       OnDataShort *eData = dynamic_cast<OnDataShort *>(d);
       t = eData->data();
   } break;

   case QMetaType::UInt:
   case QMetaType::UShort: {
       OnDataUShort *eData = dynamic_cast<OnDataUShort *>(d);
       t = eData->data();
   } break;

   case QMetaType::Long: {
       OnDataLong *eData = dynamic_cast<OnDataLong *>(d);
       t = eData->data();
   } break;

   case QMetaType::ULong: {
       OnDataULong *eData = dynamic_cast<OnDataULong *>(d);
       t = eData->data();
   } break;

   case QMetaType::LongLong: {
       OnDataLongLong *eData = dynamic_cast<OnDataLongLong *>(d);
       t = eData->data();
   } break;

   case QMetaType::ULongLong: {
       OnDataULongLong *eData = dynamic_cast<OnDataULongLong *>(d);
       t = eData->data();
   } break;

   default:
       break;
   }

   return t;
}

template <typename T>
static T getData(OnData *d)
{
    T t = getData(t, d);
    return t;
}

static OnData* create(const QMetaType::Type type)
{
    switch ((int)type) {
    case QMetaType::Char:
        return new OnDataChar();
    case QMetaType::UChar:
        return new OnDataUChar();
    case QMetaType::Short:
    case QMetaType::Int:
        return new OnDataShort();
    case QMetaType::UShort:
    case QMetaType::UInt:
        return new OnDataUShort();
    case QMetaType::Long:
        return new OnDataLong();
    case QMetaType::ULong:
        return new OnDataULong();
    case QMetaType::LongLong:
        return new OnDataLongLong();
    case QMetaType::ULongLong:
        return new OnDataULongLong();
    }

    Q_ASSERT_X(nullptr, "OnDataUtil::create(QMetaType::Type)", "OnData Not Created!");
    return nullptr;
}

static OnData* create(const QMetaType::Type type, const QVariant &data)
{
    OnData *edata = nullptr;
    bool ok = false;

    switch (type) {
    case QMetaType::Char: {
        int v = data.toInt(&ok);
        if (ok && v > (int)std::numeric_limits<char>::min() &&  v < (int)std::numeric_limits<char>::max())
            edata = new OnDataChar(static_cast<char>(v));
    } break;

    case QMetaType::UChar: {
        uint v = data.toUInt(&ok);
        if (ok && v > (uint)std::numeric_limits<uchar>::min() &&  v < (uint)std::numeric_limits<uchar>::max())
            edata = new OnDataUChar(static_cast<uchar>(v));
    } break;

    case QMetaType::Short:
    case QMetaType::Int: {
        int v = data.toInt(&ok);
        if (ok && v > std::numeric_limits<short>::min() &&  v < std::numeric_limits<short>::max())
            edata = new OnDataShort(static_cast<short>(v));
    } break;

    case QMetaType::UShort:
    case QMetaType::UInt: {
        int v = data.toInt(&ok);
        if (ok && v > std::numeric_limits<ushort>::min() &&  v < std::numeric_limits<ushort>::max())
            edata = new OnDataUShort(static_cast<ushort>(v));
    } break;

    case QMetaType::Long: {
        int v = data.toInt(&ok);
        if (ok && v > std::numeric_limits<long>::min() &&  v < std::numeric_limits<long>::max())
            edata = new OnDataLong(static_cast<long>(v));
    } break;

    case QMetaType::ULong: {
        uint v = data.toUInt(&ok);
        if (ok && v > std::numeric_limits<ulong>::min() &&  v < std::numeric_limits<ulong>::max())
            edata = new OnDataULong(static_cast<ulong>(v));
    } break;

    case QMetaType::LongLong: {
        qlonglong v = data.toLongLong(&ok);
        if (ok)
            edata = new OnDataLongLong(v);
    } break;

    case QMetaType::ULongLong: {
        qulonglong v = data.toULongLong(&ok);
        if (ok)
            edata = new OnDataULongLong(v);
    } break;

    default:
        break;
    }

    Q_ASSERT_X(edata, "OnDataUtil::create(QMetaType::Type, const QVariant &)", "OnData Not Created!");
    return edata;
}

static OnData* create(const QMetaType::Type type, const void *data)
{
    OnData *edata = create(type);
    if (data)
        edata->setData(data);

    return edata;
}

template <typename T>
static OnData *create(T data)
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

} //namespace OnDataUtil

#endif // EDATA_H
