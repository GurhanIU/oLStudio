#include "abstractmenubox.h"

/*!
    \class EDesignerWidgetBoxInterface

    \brief The EDesignerWidgetBoxInterface class allows you to control
    the contents of Qt Designer's widget box.

    \inmodule QtDesigner

    EDesignerWidgetBoxInterface contains a collection of functions
    that is typically used to manipulate the contents of \QD's widget
    box.

    \QD uses an XML file to populate its widget box. The name of that
    file is one of the widget box's properties, and you can retrieve
    it using the fileName() function.

    EDesignerWidgetBoxInterface also provides the save() function that
    saves the contents of the widget box in the file specified by the
    widget box's file name property. If you have made changes to the
    widget box, for example by dropping a widget into the widget box,
    without calling the save() function, the original content can be
    restored by a simple invocation of the load() function:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractwidgetbox.cpp 0

    The EDesignerWidgetBoxInterface class is not intended to be
    instantiated directly. You can retrieve an interface to Qt
    Designer's widget box using the
    EDesignerFormEditorInterface::widgetBox() function. A pointer to
    \QD's current EDesignerFormEditorInterface object (\c formEditor
    in the example above) is provided by the
    EDesignerCustomWidgetInterface::initialize() function's
    parameter. When implementing a custom widget plugin, you must
    subclass the EDesignerCustomWidgetInterface to expose your plugin
    to \QD.

    If you want to save your changes, and at the same time preserve
    the original contents, you can use the save() function combined
    with the setFileName() function to save your changes into another
    file. Remember to store the name of the original file first:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractwidgetbox.cpp 1

    Then you can restore the original contents of the widget box by
    resetting the file name to the original file and calling load():

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractwidgetbox.cpp 2

    In a similar way, you can later use your customized XML file:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractwidgetbox.cpp 3


    \sa EDesignerFormEditorInterface
*/

/*!
    Constructs a widget box interface with the given \a parent and
    the specified window \a flags.
*/
EMenuBoxInterface::EMenuBoxInterface(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
}

/*!
    Destroys the widget box interface.
*/
EMenuBoxInterface::~EMenuBoxInterface()
{
}

/*!
    \internal
*/
int EMenuBoxInterface::findOrInsertCategory(const QString &categoryName)
{
    int count = categoryCount();
    for (int index=0; index<count; ++index)
    {
        Category c = category(index);
        if (c.name() == categoryName)
            return index;
    }

    addCategory(Category(categoryName));
    return count;
}

/*!
    \internal
    \fn int EDesignerWidgetBoxInterface::categoryCount() const
*/

/*!
    \internal
    \fn Category EDesignerWidgetBoxInterface::category(int cat_idx) const
*/

/*!
    \internal
    \fn void EDesignerWidgetBoxInterface::addCategory(const Category &cat)
*/

/*!
    \internal
    \fn void EDesignerWidgetBoxInterface::removeCategory(int cat_idx)
*/

/*!
    \internal
    \fn int EDesignerWidgetBoxInterface::widgetCount(int cat_idx) const
*/

/*!
    \internal
    \fn Widget EDesignerWidgetBoxInterface::widget(int cat_idx, int wgt_idx) const
*/

/*!
    \internal
    \fn void EDesignerWidgetBoxInterface::addWidget(int cat_idx, const Widget &wgt)
*/

/*!
    \internal
    \fn void EDesignerWidgetBoxInterface::removeWidget(int cat_idx, int wgt_idx)
*/

/*!
    \internal
    \fn void EDesignerWidgetBoxInterface::dropWidgets(const QList<EDesignerDnDItemInterface*> &item_list, const QPoint &global_mouse_pos)

*/

/*!
    \fn void EDesignerWidgetBoxInterface::setFileName(const QString &fileName)

    Sets the XML file that \QD will use to populate its widget box, to
    \a fileName. You must call load() to update the widget box with
    the new XML file.

    \sa fileName(), load()
*/

/*!
    \fn QString EDesignerWidgetBoxInterface::fileName() const

    Returns the name of the XML file \QD is currently using to
    populate its widget box.

    \sa setFileName()
*/

/*!
    \fn bool EDesignerWidgetBoxInterface::load()

    Populates \QD's widget box by loading (or reloading) the currently
    specified XML file. Returns true if the file is successfully
    loaded; otherwise false.

    \sa setFileName()
*/

/*!
    \fn bool EDesignerWidgetBoxInterface::save()

    Saves the contents of \QD's widget box in the file specified by
    the fileName() function. Returns true if the content is
    successfully saved; otherwise false.

    \sa fileName(), setFileName()
*/


/*!
    \internal

    \class EDesignerWidgetBoxInterface::Widget

    \brief The Widget class specified a widget in Qt Designer's widget
    box component.
*/

/*!
    \enum EDesignerWidgetBoxInterface::Widget::Type

    \value Default
    \value Custom
*/

/*!
    \fn EDesignerWidgetBoxInterface::Widget::Widget(const QString &aname, const QString &xml, const QString &icon_name, Type atype)
*/

/*!
    \fn QString EDesignerWidgetBoxInterface::Widget::name() const
*/

/*!
    \fn void EDesignerWidgetBoxInterface::Widget::setName(const QString &aname)
*/

/*!
    \fn QString EDesignerWidgetBoxInterface::Widget::domXml() const
*/

/*!
    \fn void EDesignerWidgetBoxInterface::Widget::setDomXml(const QString &xml)
*/

/*!
    \fn QString EDesignerWidgetBoxInterface::Widget::iconName() const
*/

/*!
    \fn void EDesignerWidgetBoxInterface::Widget::setIconName(const QString &icon_name)
*/

/*!
    \fn Type EDesignerWidgetBoxInterface::Widget::type() const
*/

/*!
    \fn void EDesignerWidgetBoxInterface::Widget::setType(Type atype)
*/

/*!
    \fn bool EDesignerWidgetBoxInterface::Widget::isNull() const
*/


/*!
    \class EDesignerWidgetBoxInterface::Category
    \brief The Category class specifies a category in Qt Designer's widget box component.
    \internal
*/

/*!
    \enum EDesignerWidgetBoxInterface::Category::Type

    \value Default
    \value Scratchpad
*/

/*!
    \fn EDesignerWidgetBoxInterface::Category::Category(const QString &aname, Type atype)
*/

/*!
    \fn QString EDesignerWidgetBoxInterface::Category::name() const
*/

/*!
    \fn void EDesignerWidgetBoxInterface::Category::setName(const QString &aname)
*/

/*!
    \fn int EDesignerWidgetBoxInterface::Category::widgetCount() const
*/

/*!
    \fn Widget EDesignerWidgetBoxInterface::Category::widget(int idx) const
*/

/*!
    \fn void EDesignerWidgetBoxInterface::Category::removeWidget(int idx)
*/

/*!
    \fn void EDesignerWidgetBoxInterface::Category::addWidget(const Widget &awidget)
*/

/*!
    \fn Type EDesignerWidgetBoxInterface::Category::type() const
*/

/*!
    \fn void EDesignerWidgetBoxInterface::Category::setType(Type atype)
*/

/*!
    \fn bool EDesignerWidgetBoxInterface::Category::isNull() const
*/

/*!
    \typedef EDesignerWidgetBoxInterface::CategoryList
    \internal
*/

/*!
    \typedef EDesignerWidgetBoxInterface::WidgetList
    \internal
*/
