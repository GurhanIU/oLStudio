#include "abstractwidgetfactory.h"

#include "abstractformeditor.h"
//#include "abstractwidgetdatabase.h"

/*!
    \class EDesignerWidgetFactoryInterface
    \brief The EDesignerWidgetFactoryInterface class provides an interface that is used to control
    the widget factory used by Qt Designer.
    \inmodule QtDesigner
    \internal
*/

/*!
    \fn EDesignerWidgetFactoryInterface::EDesignerWidgetFactoryInterface(QObject *parent)

    Constructs an interface to a widget factory with the given \a parent.
*/
EDesignerWidgetFactoryInterface::EDesignerWidgetFactoryInterface(QObject *parent)
    : QObject(parent)
{
}

/*!
    \fn virtual EDesignerWidgetFactoryInterface::~EDesignerWidgetFactoryInterface()
*/
EDesignerWidgetFactoryInterface::~EDesignerWidgetFactoryInterface()
{
}

/*!
    \fn virtual EDesignerFormEditorInterface *EDesignerWidgetFactoryInterface::core() const = 0

    Returns the core form editor interface associated with this interface.
*/

/*!
    \fn virtual QWidget* EDesignerWidgetFactoryInterface::containerOfWidget(QWidget *child) const = 0

    Returns the widget that contains the specified \a child widget.
*/

/*!
    \fn virtual QWidget* EDesignerWidgetFactoryInterface::widgetOfContainer(QWidget *container) const = 0


*/

/*!
    \fn virtual QWidget *EDesignerWidgetFactoryInterface::createWidget(const QString &name, QWidget *parent) const = 0

    Returns a new widget with the given \a name and \a parent widget. If no parent is specified,
    the widget created will be a top-level widget.
*/

/*!
    \fn virtual QLayout *EDesignerWidgetFactoryInterface::createLayout(QWidget *widget, QLayout *layout, int type) const = 0

    Returns a new layout of the specified \a type for the given \a widget or \a layout.
*/

/*!
    \fn virtual bool EDesignerWidgetFactoryInterface::isPassiveInteractor(QWidget *widget) = 0
*/

/*!
    \fn virtual void EDesignerWidgetFactoryInterface::initialize(QObject *object) const = 0
*/
