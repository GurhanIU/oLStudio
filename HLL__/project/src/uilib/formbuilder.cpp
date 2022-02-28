#include "formbuilder.h"
#include "formbuilderextra_p.h"
#include "ui4_p.h"

#include <QtGui>
#include <QCoreApplication>

#include <QLabel>
#include <QFrame>
#include <QMainWindow>
#include <QToolBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QScrollArea>
#include <QMdiArea>
#include <QDockWidget>
#include <QTableWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDateEdit>
#include <QTimeEdit>
#include <QDial>
#include <QDialog>
#include <QDockWidget>
#include <QFrame>
#include <QGroupBox>
#include <QScrollArea>
#include <QLCDNumber>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QMainWindow>
#include <QMdiArea>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QCommandLinkButton>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTabWidget>
#include <QTableView>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QToolBar>
#include <QToolBox>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeView>
#include <QWidget>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QDialogButtonBox>
#include <QFontComboBox>
#include <QCalendarWidget>
#include <QColumnView>
#include <QWizard>
#include <QWizardPage>
#include <QGraphicsView>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QFormLayout>

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal {
#endif

/*!
    \class QFormBuilder

    \brief The QFormBuilder class is used to dynamically construct
    user interfaces from UI files at run-time.

    \inmodule QtDesigner

    The QFormBuilder class provides a mechanism for dynamically
    creating user interfaces at run-time, based on UI files
    created with \QD. For example:

    \snippet doc/src/snippets/code/tools_designer_src_lib_uilib_formbuilder.cpp 0

    By including the user interface in the example's resources (\c
    myForm.qrc), we ensure that it will be present when the example is
    run:

    \snippet doc/src/snippets/code/tools_designer_src_lib_uilib_formbuilder.cpp 1

    QFormBuilder extends the QAbstractFormBuilder base class with a
    number of functions that are used to support custom widget
    plugins:

    \list
    \o pluginPaths() returns the list of paths that the form builder
       searches when loading custom widget plugins.
    \o addPluginPath() allows additional paths to be registered with
       the form builder.
    \o setPluginPath() is used to replace the existing list of paths
       with a list obtained from some other source.
    \o clearPluginPaths() removes all paths registered with the form
       builder.
    \o customWidgets() returns a list of interfaces to plugins that
       can be used to create new instances of registered custom widgets.
    \endlist

    The QFormBuilder class is typically used by custom components and
    applications that embed \QD. Standalone applications that need to
    dynamically generate user interfaces at run-time use the
    QUiLoader class, found in the QtUiTools module.

    \sa QAbstractFormBuilder, {QtUiTools Module}
*/

/*!
    \fn QFormBuilder::QFormBuilder()

    Constructs a new form builder.
*/

EFormBuilder::EFormBuilder() : EAbstractFormBuilder()
{
}

/*!
    Destroys the form builder.
*/
EFormBuilder::~EFormBuilder()
{
}

/*!
    \internal
*/
QWidget *EFormBuilder::create(DomWidget *ui_widget, QWidget *parentWidget)
{
    EFormBuilderExtra *fb = EFormBuilderExtra::instance(this);
    if (!fb->parentWidgetIsSet())
        fb->setParentWidget(parentWidget);
    // Is this a ELayoutWidget with a margin of 0: Not a known page-based
    // container and no method for adding pages registered.
    fb->setProcessingLayoutWidget(false);
    if (ui_widget->attributeClass() == EFormBuilderStrings::instance().qWidgetClass && !ui_widget->hasAttributeNative() && parentWidget
#ifndef QT_NO_MAINWINDOW
            && !qobject_cast<QMainWindow *>(parentWidget)
#endif
#ifndef QT_NO_TOOLBOX
            && !qobject_cast<QToolBox *>(parentWidget)
#endif
#ifndef QT_NO_STACKEDWIDGET
            && !qobject_cast<QStackedWidget *>(parentWidget)
#endif
#ifndef QT_NO_STACKEDWIDGET
            && !qobject_cast<QTabWidget *>(parentWidget)
#endif
#ifndef QT_NO_SCROLLAREA
            && !qobject_cast<QScrollArea *>(parentWidget)
#endif
#ifndef QT_NO_MDIAREA
            && !qobject_cast<QMdiArea *>(parentWidget)
#endif
#ifndef QT_NO_DOCKWIDGET
            && !qobject_cast<QDockWidget *>(parentWidget)
#endif
        ) {
        const QString parentClassName = QLatin1String(parentWidget->metaObject()->className());
        if (!fb->isCustomWidgetContainer(parentClassName))
            fb->setProcessingLayoutWidget(true);
    }
    return EAbstractFormBuilder::create(ui_widget, parentWidget);
}


/*!
    \internal
*/
QWidget *EFormBuilder::createWidget(const QString &widgetName, QWidget *parentWidget, const QString &name)
{
    if (widgetName.isEmpty())
    {
        //: Empty class name passed to widget factory method
        qWarning() << QCoreApplication::translate("EFormBuilder", "An empty class name was passed on to %1 (object name: '%2').").arg(QString::fromUtf8(Q_FUNC_INFO), name);
        return 0;
    }

    QWidget *w = 0;

#ifndef QT_NO_TABWIDGET
    if (qobject_cast<QTabWidget*>(parentWidget))
        parentWidget = 0;
#endif
#ifndef QT_NO_STACKEDWIDGET
    if (qobject_cast<QStackedWidget*>(parentWidget))
        parentWidget = 0;
#endif
#ifndef QT_NO_TOOLBOX
    if (qobject_cast<QToolBox*>(parentWidget))
        parentWidget = 0;
#endif

    // ### special-casing for Line (QFrame) -- fix for 4.2
    do
    {
        if (widgetName == EFormBuilderStrings::instance().lineClass)
        {
            w = new QFrame(parentWidget);
            static_cast<QFrame*>(w)->setFrameStyle(QFrame::HLine | QFrame::Sunken);
            break;
        }
        const QByteArray widgetNameBA = widgetName.toUtf8();
        const char *widgetNameC = widgetNameBA.constData();
        if (w)
        { // symmetry for macro
        }

#define DECLARE_LAYOUT(L, C)
#define DECLARE_COMPAT_WIDGET(W, C)
#define DECLARE_WIDGET(W, C) else if (!qstrcmp(widgetNameC, #W)) { Q_ASSERT(w == 0); w = new W(parentWidget); }
#define DECLARE_WIDGET_1(W, C) else if (!qstrcmp(widgetNameC, #W)) { Q_ASSERT(w == 0); w = new W(0, parentWidget); }

#include "widgets.table"

#undef DECLARE_COMPAT_WIDGET
#undef DECLARE_LAYOUT
#undef DECLARE_WIDGET
#undef DECLARE_WIDGET_1

        if (w)
            break;

        // try with a registered custom widget
//        EDesignerCustomWidgetInterface *factory = m_customWidgets.value(widgetName);
//        if (factory != 0)
//            w = factory->createWidget(parentWidget);
    } while(false);

    EFormBuilderExtra *fb = EFormBuilderExtra::instance(this);
    if (w == 0)
    { // Attempt to instantiate base class of promoted/custom widgets
        const QString baseClassName = fb->customWidgetBaseClass(widgetName);
        if (!baseClassName.isEmpty())
        {
            qWarning() << QCoreApplication::translate("EFormBuilder", "EFormBuilder was unable to create a custom widget of the class '%1'; defaulting to base class '%2'.").arg(widgetName, baseClassName);
            return createWidget(baseClassName, parentWidget, name);
        }
    }

    if (w == 0)
    { // nothing to do
        qWarning() << QCoreApplication::translate("EFormBuilder", "EFormBuilder was unable to create a widget of the class '%1'.").arg(widgetName);
        return 0;
    }

    w->setObjectName(name);

    if (qobject_cast<QDialog *>(w))
        w->setParent(parentWidget);

    return w;
}

/*!
    \internal
*/
QLayout *EFormBuilder::createLayout(const QString &layoutName, QObject *parent, const QString &name)
{
    QLayout *l = 0;

    QWidget *parentWidget = qobject_cast<QWidget*>(parent);
    QLayout *parentLayout = qobject_cast<QLayout*>(parent);

    Q_ASSERT(parentWidget || parentLayout);

#define DECLARE_WIDGET(W, C)
#define DECLARE_COMPAT_WIDGET(W, C)

#define DECLARE_LAYOUT(L, C) \
    if (layoutName == QLatin1String(#L)) { \
        Q_ASSERT(l == 0); \
        l = parentLayout \
            ? new L() \
            : new L(parentWidget); \
    }

#include "widgets.table"

#undef DECLARE_LAYOUT
#undef DECLARE_COMPAT_WIDGET
#undef DECLARE_WIDGET

    if (l)
    {
        l->setObjectName(name);
        if (parentLayout)
        {
            QWidget *w = qobject_cast<QWidget *>(parentLayout->parent());
            if (w && w->inherits("Q3GroupBox"))
            {
                l->setContentsMargins(w->style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
                                    w->style()->pixelMetric(QStyle::PM_LayoutTopMargin),
                                    w->style()->pixelMetric(QStyle::PM_LayoutRightMargin),
                                    w->style()->pixelMetric(QStyle::PM_LayoutBottomMargin));
                QGridLayout *grid = qobject_cast<QGridLayout *>(l);
                if (grid)
                {
                    grid->setHorizontalSpacing(-1);
                    grid->setVerticalSpacing(-1);
                }
                else
                    l->setSpacing(-1);

                l->setAlignment(Qt::AlignTop);
            }
        }
    }
    else
        qWarning() << QCoreApplication::translate("QFormBuilder", "The layout type `%1' is not supported.").arg(layoutName);

    return l;
}

/*!
    \internal
*/
bool EFormBuilder::addItem(DomLayoutItem *ui_item, QLayoutItem *item, QLayout *layout)
{
    return EAbstractFormBuilder::addItem(ui_item, item, layout);
}

/*!
    \internal
*/
bool EFormBuilder::addItem(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget)
{
    return EAbstractFormBuilder::addItem(ui_widget, widget, parentWidget);
}

/*!
    \internal
*/
QWidget *EFormBuilder::widgetByName(QWidget *topLevel, const QString &name)
{
    Q_ASSERT(topLevel);
    if (topLevel->objectName() == name)
        return topLevel;

    return topLevel->findChild<QWidget*>(name);
}

static QObject *objectByName(QWidget *topLevel, const QString &name)
{
    Q_ASSERT(topLevel);
    if (topLevel->objectName() == name)
        return topLevel;

    return topLevel->findChild<QObject*>(name);
}

/*!
    \internal
*/
void EFormBuilder::createConnections(DomConnections *ui_connections, QWidget *widget)
{
    typedef QList<DomConnection*> DomConnectionList;
    Q_ASSERT(widget != 0);

    if (ui_connections == 0)
        return;

    const DomConnectionList connections = ui_connections->elementConnection();
    if (!connections.empty())
    {
        const DomConnectionList::const_iterator cend = connections.constEnd();
        for (DomConnectionList::const_iterator it = connections.constBegin(); it != cend; ++it)
        {
            QObject *sender = objectByName(widget, (*it)->elementSender());
            QObject *receiver = objectByName(widget, (*it)->elementReceiver());
            if (!sender || !receiver)
                continue;

            QByteArray sig = (*it)->elementSignal().toUtf8();
            sig.prepend("2");
            QByteArray sl = (*it)->elementSlot().toUtf8();
            sl.prepend("1");
            QObject::connect(sender, sig, receiver, sl);
        }
    }
}

/*!
    \internal
*/
QWidget *EFormBuilder::create(DomUI *ui, QWidget *parentWidget)
{
    return EAbstractFormBuilder::create(ui, parentWidget);
}

/*!
    \internal
*/
QLayout *EFormBuilder::create(DomLayout *ui_layout, QLayout *layout, QWidget *parentWidget)
{
    EFormBuilderExtra *fb = EFormBuilderExtra::instance(this);
    // Is this a temporary layout widget used to represent QLayout hierarchies in Designer?
    // Set its margins to 0.
    bool layoutWidget = fb->processingLayoutWidget();
    QLayout *l = EAbstractFormBuilder::create(ui_layout, layout, parentWidget);
    if (layoutWidget)
    {
        const EFormBuilderStrings &strings = EFormBuilderStrings::instance();
        int left, top, right, bottom;
        left = top = right = bottom = 0;
        const DomPropertyHash properties = propertyMap(ui_layout->elementProperty());

        if (DomProperty *prop = properties.value(strings.leftMarginProperty))
            left = prop->elementNumber();

        if (DomProperty *prop = properties.value(strings.topMarginProperty))
            top = prop->elementNumber();

        if (DomProperty *prop = properties.value(strings.rightMarginProperty))
            right = prop->elementNumber();

        if (DomProperty *prop = properties.value(strings.bottomMarginProperty))
            bottom = prop->elementNumber();

        l->setContentsMargins(left, top, right, bottom);
        fb->setProcessingLayoutWidget(false);
    }
    return l;
}

/*!
    \internal
*/
QLayoutItem *EFormBuilder::create(DomLayoutItem *ui_layoutItem, QLayout *layout, QWidget *parentWidget)
{
    return EAbstractFormBuilder::create(ui_layoutItem, layout, parentWidget);
}

/*!
    \internal
*/
QAction *EFormBuilder::create(DomAction *ui_action, QObject *parent)
{
    return EAbstractFormBuilder::create(ui_action, parent);
}

/*!
    \internal
*/
QActionGroup *EFormBuilder::create(DomActionGroup *ui_action_group, QObject *parent)
{
    return EAbstractFormBuilder::create(ui_action_group, parent);
}

/*!
    Returns the list of paths the form builder searches for plugins.

    \sa addPluginPath()
*/
QStringList EFormBuilder::pluginPaths() const
{
    return m_pluginPaths;
}

/*!
    Clears the list of paths that the form builder uses to search for
    custom widget plugins.

    \sa pluginPaths()
*/
void EFormBuilder::clearPluginPaths()
{
    m_pluginPaths.clear();
    updateCustomWidgets();
}

/*!
    Adds a new plugin path specified by \a pluginPath to the list of
    paths that will be searched by the form builder when loading a
    custom widget plugin.

    \sa setPluginPath(), clearPluginPaths()
*/
void EFormBuilder::addPluginPath(const QString &pluginPath)
{
    m_pluginPaths.append(pluginPath);
    updateCustomWidgets();
}

/*!
    Sets the list of plugin paths to the list specified by \a pluginPaths.

    \sa addPluginPath()
*/
void EFormBuilder::setPluginPath(const QStringList &pluginPaths)
{
    m_pluginPaths = pluginPaths;
    updateCustomWidgets();
}

static void insertPlugins(QObject *o, QMap<QString, EDesignerCustomWidgetInterface*> *customWidgets)
{
    // step 1) try with a normal plugin
//    if (EDesignerCustomWidgetInterface *iface = qobject_cast<EDesignerCustomWidgetInterface *>(o))
//    {
//        customWidgets->insert(iface->name(), iface);
//        return;
//    }
    // step 2) try with a collection of plugins
//    if (EDesignerCustomWidgetCollectionInterface *c = qobject_cast<EDesignerCustomWidgetCollectionInterface *>(o))
//    {
//        foreach (EDesignerCustomWidgetInterface *iface, c->customWidgets())
//            customWidgets->insert(iface->name(), iface);
//    }
}

/*!
    \internal
*/
void EFormBuilder::updateCustomWidgets()
{
    m_customWidgets.clear();

    foreach (const QString &path, m_pluginPaths)
    {
        const QDir dir(path);
        const QStringList candidates = dir.entryList(QDir::Files);

        foreach (const QString &plugin, candidates)
        {
            if (!QLibrary::isLibrary(plugin))
                continue;

            QString loaderPath = path;
            loaderPath += QLatin1Char('/');
            loaderPath += plugin;

            QPluginLoader loader(loaderPath);
            if (loader.load())
                insertPlugins(loader.instance(), &m_customWidgets);
        }
    }
    // Check statically linked plugins
    const QObjectList staticPlugins = QPluginLoader::staticInstances();
    if (!staticPlugins.empty())
    {
        foreach (QObject *o, staticPlugins)
            insertPlugins(o, &m_customWidgets);
    }
}

/*!
    \fn QList<EDesignerCustomWidgetInterface*> QFormBuilder::customWidgets() const

    Returns a list of the available plugins.
*/
QList<EDesignerCustomWidgetInterface*> EFormBuilder::customWidgets() const
{
    return m_customWidgets.values();
}

/*!
    \internal
*/

void EFormBuilder::applyProperties(QObject *o, const QList<DomProperty*> &properties)
{
    typedef QList<DomProperty*> DomPropertyList;

    if (properties.empty())
        return;

    EFormBuilderExtra *fb = EFormBuilderExtra::instance(this);
    const EFormBuilderStrings &strings = EFormBuilderStrings::instance();

    const DomPropertyList::const_iterator cend = properties.constEnd();
    for (DomPropertyList::const_iterator it = properties.constBegin(); it != cend; ++it)
    {
        const QVariant v = toVariant(o->metaObject(), *it);
        if (v.isNull())
            continue;

        const QString attributeName = (*it)->attributeName();
        const bool isWidget = o->isWidgetType();
        if (isWidget && o->parent() == fb->parentWidget() && attributeName == strings.geometryProperty)
        {
            // apply only the size part of a geometry for the root widget
            static_cast<QWidget*>(o)->resize(qvariant_cast<QRect>(v).size());
        }
        else if (fb->applyPropertyInternally(o, attributeName, v))
        {
        }
        else if (isWidget && !qstrcmp("QFrame", o->metaObject()->className ()) && attributeName == strings.orientationProperty)
        {
            // ### special-casing for Line (QFrame) -- try to fix me
            o->setProperty("frameShape", v); // v is of QFrame::Shape enum
        }
        else
        {
            o->setProperty(attributeName.toUtf8(), v);
        }
    }
}

#ifdef QFORMINTERNAL_NAMESPACE
} // namespace QFormInternal
#endif
