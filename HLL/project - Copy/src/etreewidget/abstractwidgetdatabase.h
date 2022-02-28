#ifndef ABSTRACTWIDGETDATABASE_H
#define ABSTRACTWIDGETDATABASE_H

#include "abstractformeditor.h"

#include <QObject>
#include <QList>

class QIcon;
class QString;

class QDebug;

class EDesignerFormEditorInterface;
class EDesignerWidgetDataBaseItemInterface
{
public:
    virtual ~EDesignerWidgetDataBaseItemInterface() {}

    virtual QString name() const = 0;
    virtual void setName(const QString &name) = 0;

    virtual QString group() const = 0;
    virtual void setGroup(const QString &group) = 0;

    virtual QString toolTip() const = 0;
    virtual void setToolTip(const QString &toolTip) = 0;

    virtual QString whatsThis() const = 0;
    virtual void setWhatsThis(const QString &whatsThis) = 0;

    virtual QString includeFile() const = 0;
    virtual void setIncludeFile(const QString &includeFile) = 0;

    virtual QIcon icon() const = 0;
    virtual void setIcon(const QIcon &icon) = 0;

    virtual bool isCompat() const = 0;
    virtual void setCompat(bool compat) = 0;

    virtual bool isContainer() const = 0;
    virtual void setContainer(bool container) = 0;

    virtual bool isCustom() const = 0;
    virtual void setCustom(bool custom) = 0;

    virtual QString pluginPath() const = 0;
    virtual void setPluginPath(const QString &path) = 0;

    virtual bool isPromoted() const = 0;
    virtual void setPromoted(bool b) = 0;

    virtual QString extends() const = 0;
    virtual void setExtends(const QString &s) = 0;

    virtual void setDefaultPropertyValues(const QList<QVariant> &list) = 0;
    virtual QList<QVariant> defaultPropertyValues() const = 0;
};

class EDesignerWidgetDataBaseInterface : public QObject
{
    Q_OBJECT
public:
    EDesignerWidgetDataBaseInterface(QObject *parent = 0);
    virtual ~EDesignerWidgetDataBaseInterface();

    virtual int count() const;
    virtual EDesignerWidgetDataBaseItemInterface *item(int index) const;

    virtual int indexOf(EDesignerWidgetDataBaseItemInterface *item) const;
    virtual void insert(int index, EDesignerWidgetDataBaseItemInterface *item);
    virtual void append(EDesignerWidgetDataBaseItemInterface *item);

    virtual indexOfObject(QObject *object, bool resolveName = true) const;
    virtual indexOfClassName(const QString &className, bool resolveName = true) const;

    virtual EDesignerFormEditorInterface *core() const;

    bool isContainer(QObject *object, bool resolveName = true) const;
    bool isCustom(QObject *object, bool resolveName = true) const;

//Q_SIGNALS:
signals:
    void changed();

protected:
    QList<EDesignerWidgetDataBaseItemInterface *> m_items;

};

#endif // ABSTRACTWIDGETDATABASE_H
