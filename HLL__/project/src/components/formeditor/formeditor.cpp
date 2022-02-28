#include "formeditor.h"
//#include "formeditor_optionspage.h"
//#include "embeddedoptionspage.h"
//#include "templateoptionspage.h"
//#include "metadatabase_p.h"
//#include "widgetdatabase_p.h"
#include "widgetfactory_p.h"
//#include "formwindowmanager.h"
//#include "emainwindow_container.h"
////#include "eworkspace_container.h"
//#include "emdiarea_container.h"
//#include "ewizard_container.h"
//#include "default_container.h"
//#include "default_layoutdecoration.h"
//#include "default_actionprovider.h"
//#include "elayoutwidget_propertysheet.h"
//#include "spacer_propertysheet.h"
//#include "line_propertysheet.h"
//#include "layout_propertysheet.h"
//#include "edesigner_stackedbox_p.h"
//#include "edesigner_toolbox_p.h"
//#include "edesigner_tabwidget_p.h"
//#include "qtbrushmanager.h"
//#include "brushmanagerproxy.h"
//#include "iconcache.h"
//#include "qtresourcemodel_p.h"
//#include "edesigner_integration_p.h"
//#include "itemview_propertysheet.h"

//#include "abstractdialoggui_p.h"


// sdk
//#include "eextensionmanager.h"
//#include "container.h"

// shared
//#include "pluginmanager_p.h"
//#include "edesigner_taskmenu_p.h"
//#include "edesigner_membersheet_p.h"
//#include "edesigner_promotion_p.h"
//#include "dialoggui_p.h"
//#include "edesigner_introspection_p.h"
//#include "edesigner_qsettings_p.h"

#include <QMessageBox>
#include <QDebug>

//namespace edesigner_internal {

FormEditor::FormEditor(QObject *parent) : EDesignerFormEditorInterface(parent)
{
//    setIntrospection(new EDesignerIntrospection);
//    setDialogGui(new DialogGui);

//    EDesignerPluginManager *pluginManager = new EDesignerPluginManager(this);
//    setPluginManager(pluginManager);

//    WidgetDataBase *widgetDatabase = new WidgetDataBase(this, this);
//    setWidgetDataBase(widgetDatabase);

//    MetaDataBase *metaDataBase = new MetaDataBase(this, this);
//    setMetaDataBase(metaDataBase);

    WidgetFactory *widgetFactory = new WidgetFactory(this, this);
    setWidgetFactory(widgetFactory);

//    FormWindowManager *formWindowManager = new FormWindowManager(this, this);
//    setFormManager(formWindowManager);
//    connect(formWindowManager, SIGNAL(formWindowAdded(EDesignerFormWindowInterface*)), widgetFactory, SLOT(formWindowAdded(EDesignerFormWindowInterface*)));
//    connect(formWindowManager, SIGNAL(activeFormWindowChanged(EDesignerFormWindowInterface*)), widgetFactory, SLOT(activeFormWindowChanged(EDesignerFormWindowInterface*)));

//    EExtensionManager *mgr = new EExtensionManager(this);
//    const QString containerExtensionId = Q_TYPEID(EDesignerContainerExtension);

//    EDesignerStackedWidgetContainerFactory::registerExtension(mgr, containerExtensionId);
//    EDesignerTabWidgetContainerFactory::registerExtension(mgr, containerExtensionId);
//    EDesignerToolBoxContainerFactory::registerExtension(mgr, containerExtensionId);
//    EMainWindowContainerFactory::registerExtension(mgr, containerExtensionId);
//    QDockWidgetContainerFactory::registerExtension(mgr, containerExtensionId);
//    QScrollAreaContainerFactory::registerExtension(mgr, containerExtensionId);
//    EWorkspaceContainerFactory::registerExtension(mgr, containerExtensionId);
//    EMdiAreaContainerFactory::registerExtension(mgr, containerExtensionId);
//    EWizardContainerFactory::registerExtension(mgr, containerExtensionId);

//    mgr->registerExtensions(new EDesignerLayoutDecorationFactory(mgr), Q_TYPEID(EDesignerLayoutDecorationExtension));

//    const QString actionProviderExtensionId = Q_TYPEID(EDesignerActionProviderExtension);
//    EToolBarActionProviderFactory::registerExtension(mgr, actionProviderExtensionId);
//    EMenuBarActionProviderFactory::registerExtension(mgr, actionProviderExtensionId);
//    EMenuActionProviderFactory::registerExtension(mgr, actionProviderExtensionId);

//    EDesignerDefaultPropertySheetFactory::registerExtension(mgr);
//    ELayoutWidgetPropertySheetFactory::registerExtension(mgr);
//    SpacerPropertySheetFactory::registerExtension(mgr);
//    LinePropertySheetFactory::registerExtension(mgr);
//    LayoutPropertySheetFactory::registerExtension(mgr);
//    EStackedWidgetPropertySheetFactory::registerExtension(mgr);
//    EToolBoxWidgetPropertySheetFactory::registerExtension(mgr);
//    ETabWidgetPropertySheetFactory::registerExtension(mgr);
//    EMdiAreaPropertySheetFactory::registerExtension(mgr);
////    EWorkspacePropertySheetFactory::registerExtension(mgr);
//    EWizardPagePropertySheetFactory::registerExtension(mgr);
//    EWizardPropertySheetFactory::registerExtension(mgr);

//    ETreeViewPropertySheetFactory::registerExtension(mgr);
//    ETableViewPropertySheetFactory::registerExtension(mgr);

//    const QString internalTaskMenuId = QLatin1String("EDesignerInternalTaskMenuExtension");
//    EDesignerTaskMenuFactory::registerExtension(mgr, internalTaskMenuId);

//    mgr->registerExtensions(new EDesignerMemberSheetFactory(mgr), Q_TYPEID(EDesignerMemberSheetExtension));

//    setExtensionManager(mgr);

//    setIconCache(new IconCache(this));

//    QtBrushManager *brushManager = new QtBrushManager(this);
//    setBrushManager(brushManager);

//    BrushManagerProxy *brushProxy = new BrushManagerProxy(this, this);
//    brushProxy->setBrushManager(brushManager);
//    setPromotion(new EDesignerPromotion(this));

//    QtResourceModel *resourceModel = new QtResourceModel(this);
//    setResourceModel(resourceModel);
//    connect(resourceModel, SIGNAL(qrcFileModifiedExternally(QString)), this, SLOT(slotQrcFileChangedExternally(QString)));

//    QList<EDesignerOptionsPageInterface*> optionsPages;
//    optionsPages << new TemplateOptionsPage(this) << new FormEditorOptionsPage(this) << new EmbeddedOptionsPage(this);
//    setOptionsPages(optionsPages);

//    setSettingsManager(new EDesignerQSettings());
}

FormEditor::~FormEditor()
{
}

void FormEditor::slotQrcFileChangedExternally(const QString &path)
{
//    EDesignerIntegration *designerIntegration = qobject_cast<EDesignerIntegration *>(integration());
//    if (!designerIntegration)
//        return;

//    EDesignerIntegration::ResourceFileWatcherBehaviour behaviour = designerIntegration->resourceFileWatcherBehaviour();
//    if (behaviour == EDesignerIntegration::NoWatcher)
//        return;
//    else if (behaviour == EDesignerIntegration::PromptAndReload)
//    {
//        QMessageBox::StandardButton button = dialogGui()->message(topLevel(), EDesignerDialogGuiInterface::FileChangedMessage, QMessageBox::Warning,
//                tr("Resource File Changed"),
//                tr("The file \"%1\" has changed outside Designer. Do you want to reload it?").arg(path),
//                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

//        if (button != QMessageBox::Yes)
//            return;
//    }

//    resourceModel()->reload(path);
}

//} //namespace edesigner_internal
