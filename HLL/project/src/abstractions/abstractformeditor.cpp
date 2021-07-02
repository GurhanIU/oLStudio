#include "abstractformeditor.h"

#include "abstractmenubox.h"
#include "abstractwidgetfactory.h"

// Must be done outside of the Qt namespace
static void initResources()
{
//    Q_INIT_RESOURCE(shared);
//    Q_INIT_RESOURCE(ClamshellPhone);
//    Q_INIT_RESOURCE(PortableMedia);
//    Q_INIT_RESOURCE(S60_nHD_Touchscreen);
//    Q_INIT_RESOURCE(S60_QVGA_Candybar);
//    Q_INIT_RESOURCE(SmartPhone2);
//    Q_INIT_RESOURCE(SmartPhone);
//    Q_INIT_RESOURCE(SmartPhoneWithButtons);
//    Q_INIT_RESOURCE(TouchscreenPhone);
}

class EDesignerFormEditorInterfacePrivate
{
public:
    EDesignerFormEditorInterfacePrivate();
    ~EDesignerFormEditorInterfacePrivate();

    QPointer<QWidget> m_topLevel;
    QPointer<EMenuBoxInterface> m_menuBox;
    QPointer<EDesignerWidgetFactoryInterface> m_widgetFactory;
};

EDesignerFormEditorInterfacePrivate::EDesignerFormEditorInterfacePrivate()
{
}

EDesignerFormEditorInterfacePrivate::~EDesignerFormEditorInterfacePrivate()
{
}

/*!
    \class EDesignerFormEditorInterface

    \brief The EDesignerFormEditorInterface class allows you to access
    Qt Designer's various components.

    \inmodule QtDesigner

    \QD's current EDesignerFormEditorInterface object holds
    information about all \QD's components: The action editor, the
    object inspector, the property editor, the widget box, and the
    extension and form window managers. EDesignerFormEditorInterface
    contains a collection of functions that provides interfaces to all
    these components. They are typically used to query (and
    manipulate) the respective component. For example:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractformeditor.cpp 0

    EDesignerFormEditorInterface is not intended to be instantiated
    directly. A pointer to \QD's current EDesignerFormEditorInterface
    object (\c formEditor in the example above) is provided by the
    EDesignerCustomWidgetInterface::initialize() function's
    parameter. When implementing a custom widget plugin, you must
    subclass the EDesignerCustomWidgetInterface to expose your plugin
    to \QD.

    EDesignerFormEditorInterface also provides functions that can set
    the action editor, property editor, object inspector and widget
    box. These are only useful if you want to provide your own custom
    components.

    If designer is embedded in another program, one could to provide its
    own settings manager. The manager is used by the components of \QD
    to store/retrieve persistent configuration settings. The default
    manager uses QSettings as the backend.

    Finally, EDesignerFormEditorInterface provides the topLevel()
    function that returns \QD's top-level widget.

    \sa EDesignerCustomWidgetInterface
*/

/*!
    Constructs a EDesignerFormEditorInterface object with the given \a
    parent.
*/

EDesignerFormEditorInterface::EDesignerFormEditorInterface(QObject *parent)
    : QObject(parent),
      d(new EDesignerFormEditorInterfacePrivate())
{
    initResources();
}

/*!
    Destroys the EDesignerFormEditorInterface object.
*/
EDesignerFormEditorInterface::~EDesignerFormEditorInterface()
{
    delete d;
}

/*!
    Returns an interface to \QD's widget box.

    \sa setWidgetBox()
*/
EMenuBoxInterface *EDesignerFormEditorInterface::menuBox() const
{
    return d->m_menuBox;
}

/*!
    Sets \QD's widget box to be the specified \a widgetBox.

    \sa widgetBox()
*/
void EDesignerFormEditorInterface::setMenuBox(EMenuBoxInterface *menuBox)
{
    d->m_menuBox = menuBox;
}


/*!
    Returns \QD's top-level widget.
*/
QWidget *EDesignerFormEditorInterface::topLevel() const
{
    return d->m_topLevel;
}

/*!
    \internal
*/
void EDesignerFormEditorInterface::setTopLevel(QWidget *topLevel)
{
    d->m_topLevel = topLevel;
}

/*!
    \internal

    Returns an interface to the widget factory used by the form editor
    to create widgets for the form.
*/
EDesignerWidgetFactoryInterface *EDesignerFormEditorInterface::widgetFactory() const
{
    return d->m_widgetFactory;
}

/*!
    \internal
*/
void EDesignerFormEditorInterface::setWidgetFactory(EDesignerWidgetFactoryInterface *widgetFactory)
{
    d->m_widgetFactory = widgetFactory;
}

QString EDesignerFormEditorInterface::dbFile() const
{
    return m_dbFile;
}

void EDesignerFormEditorInterface::setDbFile(const QString &dbFile)
{
    m_dbFile = dbFile;
}
