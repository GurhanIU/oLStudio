#include "abstractformeditor.h"

#include "abstractformeditor.h"
#include "abstractwidgetbox.h"
#include "abstractwidgetdatabase.h"

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
    QPointer<EDesignerWidgetBoxInterface> m_widgetBox;
    QPointer<EDesignerWidgetDataBaseInterface> m_widgetDataBase;
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
  Qt Designer' various components.

  \inmodule EDesigner

  \QD's current EDesignerFormEditorInterface object holds
  information about all \QD's components: The action editor, the
  object inspector, the property editor, the widget box and the
  extension and form window managers. EDesignerFormEditorInterface
  contains a collection of functions that provides interfaces to all
  these components. They are typically used to query (and manipulate)
  the respective component. For example:

  \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractformeditor.cpp 0

  EDesignerFormEditorInterface is not intended to be instantiated
  directly. A pointer to \QD's current EDesignerFormEditorInterface
  object (\c formEditor in the example above) is provided by the
  QDesignerCustomWidgetInterface::initialize() function's
  parameter. When implementing a custom widget plugin, you must
  subclass the QDesignerCustomWidgetInterface to expose your plugin
  to \QD.

  EDesignerFormEditorInterface also provides functions that can set
  the action editor, property editor, object inspector and widget
  box. These are only useful if you want to provide your own custom
  components.

  Id designer is embedded in another program, one could to provide its
  own settings manager. The manager is used by the components of \QD
  to store/retrieve persistent configuration settings. The default
  manager uses QSettings as the backend.

  Finally, EDesignerFromEditorInterface provides the topLevel()
  function that returns \QD's top-level widget;

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
    Destroys the EDesignerFormEditorInterface object
*/
EDesignerFormEditorInterface::~EDesignerFormEditorInterface()
{
    delete d;
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
    Returns an interface to \QD's widget box
*/
EDesignerWidgetBoxInterface *EDesignerFormEditorInterface::widgetBox() const
{
    return d->m_widgetBox;
}

/*!
    Sets \QD's widget box to be the specified \a widgetBox.

    \sa widgetBox()
*/
void EDesignerFormEditorInterface::setWidgetBox(EDesignerWidgetBoxInterface *widgetBox)
{
    d->m_widgetBox = widgetBox;
}

/*!
    \internal

    Returns an interface to the widget database used by the form editor.
*/
EDesignerWidgetDataBaseInterface *EDesignerFormEditorInterface::widgetDataBase() const
{
    return d->m_widgetDataBase;
}

/*!
    \internal
*/
void EDesignerFormEditorInterface::setWidgetDataBase(EDesignerWidgetDataBaseInterface *widgetDataBase)
{
    d->m_widgetDataBase = widgetDataBase;
}

