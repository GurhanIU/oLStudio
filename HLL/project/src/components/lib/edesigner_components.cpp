#include "edesigner_components.h"

//#include "actioneditor_p.h"
//#include "widgetdatabase_p.h"
#include "widgetfactory_p.h"

#include "abstractformeditor.h"
#include "formeditor.h"
#include "menubox.h"
//#include "propertyeditor.h"
//#include "abstractpropertyeditor.h"
//#include "objectinspector.h"
//#include "taskmenu_component.h"
//#include "qtresourceview_p.h"
//#include "edesigner_integration_p.h"
//#include "signalsloteditorwindow.h"

//#include "buddyeditor_plugin.h"
//#include "signalsloteditor_plugin.h"
//#include "tabordereditor_plugin.h"

//#include "abstractlanguage.h"
//#include "eextensionmanager.h"
//#include "abstractresourcebrowser.h"

#include <QtPlugin>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QFile>
#include <QFileInfo>

// ### keep it in sync with Q_IMPORT_PLUGIN in qplugin.h
//#define DECLARE_PLUGIN_INSTANCE(PLUGIN) \
//    extern QT_PREPEND_NAMESPACE(QObject) *qt_plugin_instance_##PLUGIN(); \
//    class Static##PLUGIN##PluginInstance { public: \
//        Static##PLUGIN##PluginInstance() {                      \
//            QT_PREPEND_NAMESPACE(qRegisterStaticPluginInstanceFunction) \
//                (&qt_plugin_instance_##PLUGIN); \
//        } \
//    };

//#define INIT_PLUGIN_INSTANCE(PLUGIN) \
//    do { \
//        Static##PLUGIN##PluginInstance instance; \
//        Q_UNUSED(instance); \
//    } while (0)

//DECLARE_PLUGIN_INSTANCE(SignalSlotEditorPlugin)
//DECLARE_PLUGIN_INSTANCE(BuddyEditorPlugin)
//DECLARE_PLUGIN_INSTANCE(TabOrderEditorPlugin)

//Q_IMPORT_PLUGIN(SignalSlotEditorPlugin) // Qt5
//Q_IMPORT_PLUGIN(BuddyEditorPlugin) // Qt5
//Q_IMPORT_PLUGIN(TabOrderEditorPlugin) // Qt5

static void initResources()
{
    // Q_INIT_RESOURCE only usable in functions in global namespace
//    Q_INIT_RESOURCE(formeditor);
//    Q_INIT_RESOURCE(menubox);
//    Q_INIT_RESOURCE(propertyeditor);
}


static void initInstances()
{
    static bool plugins_initialized = false;

    if (!plugins_initialized)
    {
//        INIT_PLUGIN_INSTANCE(SignalSlotEditorPlugin);
//        INIT_PLUGIN_INSTANCE(BuddyEditorPlugin);
//        INIT_PLUGIN_INSTANCE(TabOrderEditorPlugin);
        plugins_initialized = true;
    }
}

/*!
    \class EDesignerComponents
    \brief The EDesignerComponents class provides a central resource for the various components
    used in the \QD user interface.
    \inmodule QtDesigner
    \internal

    The EDesignerComponents class is a factory for each of the standard components present
    in the \QD user interface. It is mostly useful for developers who want to implement
    a standalone form editing environment using \QD's components, or who need to integrate
    \QD's components into an existing integrated development environment (IDE).

    \sa EDesignerFormEditorInterface, EDesignerObjectInspectorInterface,
        EDesignerPropertyEditorInterface, EDesignerWidgetBoxInterface
*/

/*!
    Initializes the resources used by the components.*/
//void EDesignerComponents::initializeResources()
//{
//    initResources();
//}

/*!
    Initializes the plugins used by the components.*/
//void EDesignerComponents::initializePlugins(EDesignerFormEditorInterface *core)
//{
//    edesigner_internal::EDesignerIntegration::initializePlugins(core);
//}

/*!
    Constructs a form editor interface with the given \a parent.*/
EDesignerFormEditorInterface *EDesignerComponents::createFormEditor(QObject *parent)
{
    initInstances();
    return new FormEditor(parent);
}

/*!
    Returns a new task menu with the given \a parent for the \a core interface.*/
//QObject *EDesignerComponents::createTaskMenu(EDesignerFormEditorInterface *core, QObject *parent)
//{
//    return new edesigner_internal::TaskMenuComponent(core, parent);
//}

//static inline int qtMajorVersion(int qtVersion) { return qtVersion >> 16; }
//static inline int qtMinorVersion(int qtVersion) { return (qtVersion >> 8) & 0xFF; }
//static inline void setMinorVersion(int minorVersion, int *qtVersion)
//{
//    *qtVersion &= ~0xFF00;
//    *qtVersion |= minorVersion << 8;
//}

// Build the version-dependent name of the user widget box file, '$HOME.designer/widgetbox4.4.xml'
//static inline QString widgetBoxFileName(int qtVersion, const EDesignerLanguageExtension *lang = 0)
//{
//    QString rc;
//    {
//        const QChar dot = QLatin1Char('.');
//        QTextStream str(&rc);
//        str << QDir::homePath() << QDir::separator() << QLatin1String(".designer") << QDir::separator()
//            << QLatin1String("widgetbox");
//        // The naming convention using the version was introduced with 4.4
//        const int major = qtMajorVersion(qtVersion);
//        const int minor = qtMinorVersion(qtVersion);
//        if (major >= 4 &&  minor >= 4)
//            str << major << dot << minor;
//        if (lang)
//            str << dot << lang->uiExtension();
//        str << QLatin1String(".xml");
//    }
//    return rc;
//}

/*!
    Returns a new widget box interface with the given \a parent for the \a core interface.*/
EMenuBoxInterface *EDesignerComponents::createMenuBox(EDesignerFormEditorInterface *core, QWidget *parent)
{
    MenuBox *menuBox = new MenuBox(core, parent);

//    const EDesignerLanguageExtension *lang = e_extension<EDesignerLanguageExtension*>(core->extensionManager(), core);

//    do
//    {
//        if (lang)
//        {
//            const QString languageWidgetBox = lang->widgetBoxContents();
//            if (!languageWidgetBox.isEmpty())
//            {
//                widgetBox->loadContents(lang->widgetBoxContents());
//                break;
//            }
//        }

//        widgetBox->setFileName(QLatin1String(":/trolltech/widgetbox/widgetbox.xml"));
//        widgetBox->load();
//    } while (false);

//    const QString userWidgetBoxFile = widgetBoxFileName(QT_VERSION, lang);

    menuBox->setFileName(core->dbFile());
//    if (!QFileInfo(userWidgetBoxFile).exists())
//    {
//        // check previous version, that is, are we running the new version for the first time
//        // If so, try to copy the old widget box file
//        if (const int minv = qtMinorVersion(QT_VERSION))
//        {
//            int oldVersion = QT_VERSION;
//            setMinorVersion(minv - 1, &oldVersion);
//            const QString oldWidgetBoxFile = widgetBoxFileName(oldVersion, lang);
//            if (QFileInfo(oldWidgetBoxFile).exists())
//                QFile::copy(oldWidgetBoxFile, userWidgetBoxFile);
//        }
//    }
    menuBox->load();

    return menuBox;
}

/*!
    Returns a new property editor interface with the given \a parent for the \a core interface.*/
//EDesignerPropertyEditorInterface *EDesignerComponents::createPropertyEditor(EDesignerFormEditorInterface *core, QWidget *parent)
//{
//    return new edesigner_internal::PropertyEditor(core, parent);
//}

/*!
    Returns a new object inspector interface with the given \a parent for the \a core interface.*/
//EDesignerObjectInspectorInterface *EDesignerComponents::createObjectInspector(EDesignerFormEditorInterface *core, QWidget *parent)
//{
//    return new edesigner_internal::ObjectInspector(core, parent);
//}

/*!
    Returns a new action editor interface with the given \a parent for the \a core interface.*/
//EDesignerActionEditorInterface *EDesignerComponents::createActionEditor(EDesignerFormEditorInterface *core, QWidget *parent)
//{
//    return new edesigner_internal::ActionEditor(core, parent);
//}

/*!
    Returns a new resource editor with the given \a parent for the \a core interface.*/
//QWidget *EDesignerComponents::createResourceEditor(EDesignerFormEditorInterface *core, QWidget *parent)
//{
//    if (EDesignerLanguageExtension *lang = e_extension<EDesignerLanguageExtension*>(core->extensionManager(), core))
//    {
//        QWidget *w = lang->createResourceBrowser(parent);
//        if (w)
//            return w;
//    }
//    QtResourceView *resourceView = new QtResourceView(core, parent);
//    resourceView->setResourceModel(core->resourceModel());
//    resourceView->setSettingsKey(QLatin1String("ResourceBrowser"));
//    edesigner_internal::EDesignerIntegration *designerIntegration = qobject_cast<edesigner_internal::EDesignerIntegration *>(core->integration());
//    // Note for integrators: make sure you call createResourceEditor() after you instantiated your subclass of designer integration
//    // (designer doesn't do that since by default editing resources is enabled)
//    if (designerIntegration)
//        resourceView->setResourceEditingEnabled(designerIntegration->isResourceEditingEnabled());
//    return resourceView;
//}

/*!
    Returns a new signal-slot editor with the given \a parent for the \a core interface.*/
//QWidget *EDesignerComponents::createSignalSlotEditor(EDesignerFormEditorInterface *core, QWidget *parent)
//{
//    return new edesigner_internal::SignalSlotEditorWindow(core, parent);
//}
