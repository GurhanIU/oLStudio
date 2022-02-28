#ifndef FORMBUILDER_H
#define FORMBUILDER_H

//#include <QtDesigner/uilib_global.h>
#include "abstractformbuilder.h"

#include <QStringList>
#include <QMap>

#if 0
// pragma for syncqt, don't remove.

#pragma qt_class(EFormBuilder)
#endif

class EDesignerCustomWidgetInterface;

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal {
#endif

class EFormBuilder: public EAbstractFormBuilder
{
public:
    EFormBuilder();
    virtual ~EFormBuilder();

    QStringList pluginPaths() const;

    void clearPluginPaths();
    void addPluginPath(const QString &pluginPath);
    void setPluginPath(const QStringList &pluginPaths);

    QList<EDesignerCustomWidgetInterface *> customWidgets() const;

protected:
    virtual QWidget *create(DomUI *ui, QWidget *parentWidget);
    virtual QWidget *create(DomWidget *ui_widget, QWidget *parentWidget);
    virtual QLayout *create(DomLayout *ui_layout, QLayout *layout, QWidget *parentWidget);
    virtual QLayoutItem *create(DomLayoutItem *ui_layoutItem, QLayout *layout, QWidget *parentWidget);
    virtual QAction *create(DomAction *ui_action, QObject *parent);
    virtual QActionGroup *create(DomActionGroup *ui_action_group, QObject *parent);

    virtual QWidget *createWidget(const QString &widgetName, QWidget *parentWidget, const QString &name);
    virtual QLayout *createLayout(const QString &layoutName, QObject *parent, const QString &name);

    virtual void createConnections(DomConnections *connections, QWidget *widget);

    virtual bool addItem(DomLayoutItem *ui_item, QLayoutItem *item, QLayout *layout);
    virtual bool addItem(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget);

    virtual void updateCustomWidgets();
    virtual void applyProperties(QObject *o, const QList<DomProperty*> &properties);

    static QWidget *widgetByName(QWidget *topLevel, const QString &name);

private:
    QStringList m_pluginPaths;
    QMap<QString, EDesignerCustomWidgetInterface*> m_customWidgets;
};

#ifdef QFORMINTERNAL_NAMESPACE
}
#endif

#endif // FORMBUILDER_H
