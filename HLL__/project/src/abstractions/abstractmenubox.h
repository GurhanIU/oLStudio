#ifndef EABSTRACTMENUBOX_H
#define EABSTRACTMENUBOX_H

#include <QMetaType>
#include <QWidget>
#include <QIcon>

class EMenuBoxInterface : public QWidget
{
    Q_OBJECT
public:
    class Page {
    public:
        enum Type { Default, Custom };
        Page(const QString &aname = QString(), int id = 0,
                const QString &icon_name = QString(), Type atype = Default)
            : m_name(aname), m_id(id), m_icon_name(icon_name), m_type(atype) {}

        QString name() const { return m_name; }
        void setName(const QString &aname) { m_name = aname; }
        int id() { return m_id; }
        QString idAsString() const { return QString::number(m_id); }
        void setId(int id) { m_id = id; }
        QString iconName() const { return m_icon_name; }
        void setIconName(const QString &icon_name) { m_icon_name = icon_name; }
        Type type() const { return m_type; }
        void setType(Type atype) { m_type = atype; }

        bool isNull() const { return m_name.isEmpty(); }

    private:
        QString m_name;
        int m_id;
        QString m_icon_name;        
        Type m_type;
    };
//    typedef QList<Page> PageList;
//    typedef QList<Widget> WidgetList;

    class Category {
    public:
        enum Type { Default, Scratchpad };

        Category(const QString &aname = QString(), Type atype = Default)
            : m_name(aname), m_type(atype) {}

        QString name() const { return m_name; }
        void setName(const QString &aname) { m_name = aname; }
        int pageCount() const { return m_page_list.size(); }
        Page page(int idx) const { return m_page_list.at(idx); }
        void removePage(int idx) { m_page_list.removeAt(idx); }
        void addPage(const Page &apage) { m_page_list.append(apage); }
        Type type() const { return m_type; }
        void setType(Type atype) { m_type = atype; }

        bool isNull() const { return m_name.isEmpty(); }

    private:
        QString m_name;
        Type m_type;
        QList<Page> m_page_list;
    };
    typedef QList<Category> CategoryList;

    EMenuBoxInterface(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~EMenuBoxInterface();

    virtual int categoryCount() const = 0;
    virtual Category category(int cat_idx) const = 0;
    virtual void addCategory(const Category &cat) = 0;
    virtual void removeCategory(int cat_idx) = 0;

    virtual int pageCount(int cat_idx) const = 0;
    virtual Page page(int cat_idx, int page_idx) const = 0;
    virtual void addPage(int cat_idx, const Page &wgt) = 0;
    virtual void removePage(int cat_idx, int page_idx) = 0;

    int findOrInsertCategory(const QString &categoryName);

    virtual void setFileName(const QString &file_name) = 0;
    virtual QString fileName() const = 0;
    virtual bool load() = 0;
    virtual bool save() = 0;

Q_SIGNALS:
    void pageChanged(const QString &name, const int &id);
};

Q_DECLARE_METATYPE(QT_PREPEND_NAMESPACE(EMenuBoxInterface::Page))
//Q_DECLARE_METATYPE(EDesignerWidgetBoxInterface::Widget)


#endif // EABSTRACTMENUBOX_H
