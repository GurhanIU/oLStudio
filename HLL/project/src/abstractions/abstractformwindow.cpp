#include "abstractformwindow.h"
#include "widgetfactory_p.h"

#include <QTabBar>
#include <QSizeGrip>
#include <QAbstractButton>
#include <QToolBox>
#include <QMenuBar>
#include <QMainWindow>
#include <QDockWidget>
#include <QToolBar>
#include <QDebug>

/*!
    \class EDesignerFormWindowInterface

    \brief The EDesignerFormWindowInterface class allows you to query
    and manipulate form windows appearing in Qt Designer's workspace.

    \inmodule QtDesigner

    EDesignerFormWindowInterface provides information about
    the associated form window as well as allowing its properties to be
    altered. The interface is not intended to be instantiated
    directly, but to provide access to \QD's current form windows
    controlled by \QD's \l {EDesignerFormWindowManagerInterface}{form
    window manager}.

    If you are looking for the form window containing a specific
    widget, you can use the static
    EDesignerFormWindowInterface::findFormWindow() function:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractformwindow.cpp 0

    But in addition, you can access any of the current form windows
    through \QD's form window manager: Use the
    EDesignerFormEditorInterface::formWindowManager() function to
    retrieve an interface to the manager. Once you have this
    interface, you have access to all of \QD's current form windows
    through the EDesignerFormWindowManagerInterface::formWindow()
    function. For example:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractformwindow.cpp 1

    The pointer to \QD's current EDesignerFormEditorInterface object
    (\c formEditor in the example above) is provided by the
    EDesignerCustomWidgetInterface::initialize() function's
    parameter. When implementing a custom widget plugin, you must
    subclass the EDesignerCustomWidgetInterface class to expose your
    plugin to \QD.

    Once you have the form window, you can query its properties. For
    example, a plain custom widget plugin is managed by \QD only at
    its top level, i.e. none of its child widgets can be resized in
    \QD's workspace. But EDesignerFormWindowInterface provides you
    with functions that enables you to control whether a widget should
    be managed by \QD, or not:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractformwindow.cpp 2

    The complete list of functions concerning widget management is:
    isManaged(), manageWidget() and unmanageWidget(). There is also
    several associated signals: widgetManaged(), widgetRemoved(),
    aboutToUnmanageWidget() and widgetUnmanaged().

    In addition to controlling the management of widgets, you can
    control the current selection in the form window using the
    selectWidget(), clearSelection() and emitSelectionChanged()
    functions, and the selectionChanged() signal.

    You can also retrieve information about where the form is stored
    using absoluteDir(), its include files using includeHints(), and
    its layout and pixmap functions using layoutDefault(),
    layoutFunction() and pixmapFunction(). You can find out whether
    the form window has been modified (but not saved) or not, using
    the isDirty() function. You can retrieve its author(), its
    contents(), its fileName(), associated comment() and
    exportMacro(), its mainContainer(), its features(), its grid() and
    its resourceFiles().

    The interface provides you with functions and slots allowing you
    to alter most of this information as well. The exception is the
    directory storing the form window. Finally, there is several
    signals associated with changes to the information mentioned above
    and to the form window in general.

    \sa EDesignerFormWindowCursorInterface,
    EDesignerFormEditorInterface, EDesignerFormWindowManagerInterface
*/

/*!
    \enum EDesignerFormWindowInterface::FeatureFlag

    This enum describes the features that are available and can be
    controlled by the form window interface. These values are used
    when querying the form window to determine which features it
    supports:

    \value EditFeature      Form editing
    \value GridFeature      Grid display and snap-to-grid facilities for editing
    \value TabOrderFeature  Tab order management
    \value DefaultFeature   Support for default features (form editing and grid)

    \sa hasFeature(), features()
*/

/*!
    Constructs a form window interface with the given \a parent and
    the specified window \a flags.
*/
EDesignerFormWindowInterface::EDesignerFormWindowInterface(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
}

/*!
    Destroys the form window interface.
*/
EDesignerFormWindowInterface::~EDesignerFormWindowInterface()
{
}

/*!
    Returns a pointer to \QD's current EDesignerFormEditorInterface
    object.
*/
EDesignerFormEditorInterface *EDesignerFormWindowInterface::core() const
{
    return 0;
}

/*!
    \fn EDesignerFormWindowInterface *EDesignerFormWindowInterface::findFormWindow(QWidget *widget)

    Returns the form window interface for the given \a widget.
*/

static inline bool stopFindAtTopLevel(const QObject *w, bool stopAtMenu)
{
    // Do we need to go beyond top levels when looking for the form window?
    // 1) A dialog has a window attribute at the moment it is created
    //    before it is properly embedded into a form window. The property
    //    sheet queries the layout attributes precisely at this moment.
    // 2) In the case of floating docks and toolbars, we also need to go beyond the top level window.
    // 3) In the case of menu editing, we don't want to block events from the
    //    Designer menu, so, we say stop.
    // Note that there must be no false positives for dialogs parented on
    // the form (for example, the "change object name" dialog), else, its
    // events will be blocked.

    if (stopAtMenu && w->inherits("EDesignerMenu"))
        return true;
//    return !edesigner_internal::WidgetFactory::isFormEditorObject(w);
    return !WidgetFactory::isFormEditorObject(w);
}

EDesignerFormWindowInterface *EDesignerFormWindowInterface::findFormWindow(QWidget *w)
{
    while (w != 0) {
        if (EDesignerFormWindowInterface *fw = qobject_cast<EDesignerFormWindowInterface*>(w)) {
            return fw;
        } else {
            if (w->isWindow() && stopFindAtTopLevel(w, true))
                break;
        }

        w = w->parentWidget();
    }

    return 0;
}

/*!
    \fn EDesignerFormWindowInterface *EDesignerFormWindowInterface::findFormWindow(QObject *object)

    Returns the form window interface for the given \a object.

    \since 4.4
*/

EDesignerFormWindowInterface *EDesignerFormWindowInterface::findFormWindow(QObject *object)
{
    while (object != 0) {
        if (EDesignerFormWindowInterface *fw = qobject_cast<EDesignerFormWindowInterface*>(object)) {
            return fw;
        } else {
            QWidget *w = qobject_cast<QWidget *>(object);
            // EDesignerMenu is a window, so stopFindAtTopLevel(w) returns 0.
            // However, we want to find the form window for QActions of a menu.
            // If this check is inside stopFindAtTopLevel(w), it will break designer
            // menu editing (e.g. when clicking on items inside an opened menu)
            if (w && w->isWindow() && stopFindAtTopLevel(w, false))
                break;

        }

        object = object->parent();
    }

    return 0;
}

/*!
    \fn virtual QString EDesignerFormWindowInterface::fileName() const

    Returns the file name of the UI file that describes the form
    currently being shown.

    \sa setFileName()
*/

/*!
    \fn virtual QDir EDesignerFormWindowInterface::absoluteDir() const

    Returns the absolute location of the directory containing the form
    shown in the form window.
*/

/*!
    \fn virtual QString EDesignerFormWindowInterface::contents() const

    Returns details of the contents of the form currently being
    displayed in the window.
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setContents(QIODevice *device)

    Sets the form's contents using data obtained from the given \a device.

    Data can be read from QFile objects or any other subclass of QIODevice.
*/

/*!
    \fn virtual Feature EDesignerFormWindowInterface::features() const

    Returns a combination of the features provided by the form window
    associated with the interface. The value returned can be tested
    against the \l Feature enum values to determine which features are
    supported by the window.

    \sa setFeatures(), hasFeature()
*/

/*!
    \fn virtual bool EDesignerFormWindowInterface::hasFeature(Feature feature) const

    Returns true if the form window offers the specified \a feature;
    otherwise returns false.

    \sa features()
*/

/*!
    \fn virtual QString EDesignerFormWindowInterface::author() const

    Returns details of the author or creator of the form currently
    being displayed in the window.
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setAuthor(const QString &author)

    Sets the details for the author or creator of the form to the \a
    author specified.
*/

/*!
    \fn virtual QString EDesignerFormWindowInterface::comment() const

    Returns comments about the form currently being displayed in the window.
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setComment(const QString &comment)

    Sets the information about the form to the \a comment
    specified. This information should be a human-readable comment
    about the form.
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::layoutDefault(int *margin, int *spacing)

    Fills in the default margin and spacing for the form's default
    layout in the \a margin and \a spacing variables specified.
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setLayoutDefault(int margin, int spacing)

    Sets the default \a margin and \a spacing for the form's layout.

    \sa layoutDefault()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::layoutFunction(QString *margin, QString *spacing)

    Fills in the current margin and spacing for the form's layout in
    the \a margin and \a spacing variables specified.
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setLayoutFunction(const QString &margin, const QString &spacing)

    Sets the \a margin and \a spacing for the form's layout.

    The default layout properties will be replaced by the
    corresponding layout functions when \c uic generates code for the
    form, that is, if the functions are specified. This is useful when
    different environments requires different layouts for the same
    form.

    \sa layoutFunction()
*/

/*!
    \fn virtual QString EDesignerFormWindowInterface::pixmapFunction() const

    Returns the name of the function used to load pixmaps into the
    form window.

    \sa setPixmapFunction()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setPixmapFunction(const QString &pixmapFunction)

    Sets the function used to load pixmaps into the form window
    to the given \a pixmapFunction.

    \sa pixmapFunction()
*/

/*!
    \fn virtual QString EDesignerFormWindowInterface::exportMacro() const

    Returns the export macro associated with the form currently being
    displayed in the window.  The export macro is used when the form
    is compiled to create a widget plugin.

    \sa {Creating Custom Widgets for Qt Designer}
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setExportMacro(const QString &exportMacro)

    Sets the form window's export macro to \a exportMacro. The export
    macro is used when building a widget plugin to export the form's
    interface to other components.
*/

/*!
    \fn virtual QStringList EDesignerFormWindowInterface::includeHints() const

    Returns a list of the header files that will be included in the
    form window's associated UI file.

    Header files may be local, i.e. relative to the project's
    directory, \c "mywidget.h", or global, i.e. part of Qt or the
    compilers standard libraries: \c <QWidget>.

    \sa setIncludeHints()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setIncludeHints(const QStringList &includeHints)

    Sets the header files that will be included in the form window's
    associated UI file to the specified \a includeHints.

    Header files may be local, i.e. relative to the project's
    directory, \c "mywidget.h", or global, i.e. part of Qt or the
    compilers standard libraries: \c <QWidget>.

    \sa includeHints()
*/

/*!
    \fn virtual EDesignerFormWindowCursorInterface *EDesignerFormWindowInterface::cursor() const

    Returns the cursor interface used by the form window.
*/

/*!
    \fn virtual int EDesignerFormWindowInterface::toolCount() const

    Returns the number of tools available.

    \internal
*/

/*!
    \fn virtual int EDesignerFormWindowInterface::currentTool() const

    Returns the index of the current tool in use.

    \sa setCurrentTool()

    \internal
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setCurrentTool(int index)

    Sets the current tool to be the one with the given \a index.

    \sa currentTool()

    \internal
*/

/*!
    \fn virtual EDesignerFormWindowToolInterface *EDesignerFormWindowInterface::tool(int index) const

    Returns an interface to the tool with the given \a index.

    \internal
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::registerTool(EDesignerFormWindowToolInterface *tool)

    Registers the given \a tool with the form window.

    \internal
*/

/*!
    \fn virtual QPoint EDesignerFormWindowInterface::grid() const = 0

    Returns the grid spacing used by the form window.

    \sa setGrid()
*/

/*!
    \fn virtual QWidget *EDesignerFormWindowInterface::mainContainer() const

    Returns the main container widget for the form window.

    \sa setMainContainer()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setMainContainer(QWidget *mainContainer)

    Sets the main container widget on the form to the specified \a
    mainContainer.

    \sa mainContainer(), mainContainerChanged()
*/

/*!
    \fn virtual bool EDesignerFormWindowInterface::isManaged(QWidget *widget) const

    Returns true if the specified \a widget is managed by the form
    window; otherwise returns false.

    \sa manageWidget()
*/

/*!
    \fn virtual bool EDesignerFormWindowInterface::isDirty() const

    Returns true if the form window is "dirty" (modified but not
    saved); otherwise returns false.

    \sa setDirty()
*/

/*!
    \fn virtual QUndoStack *EDesignerFormWindowInterface::commandHistory() const

    Returns an object that can be used to obtain the commands used so
    far in the construction of the form.

    \internal
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::beginCommand(const QString &description)

    Begins execution of a command with the given \a
    description. Commands are executed between beginCommand() and
    endCommand() function calls to ensure that they are recorded on
    the undo stack.

    \sa endCommand()

    \internal
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::endCommand()

    Ends execution of the current command.

    \sa beginCommand()

    \internal
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::simplifySelection(QList<QWidget*> *widgets) const

    Simplifies the selection of widgets specified by \a widgets.

    \sa selectionChanged()
    \internal
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::emitSelectionChanged()

    Emits the selectionChanged() signal.

    \sa selectWidget(), clearSelection()
*/

/*!
    \fn virtual QStringList EDesignerFormWindowInterface::resourceFiles() const

    Returns a list of paths to resource files that are currently being
    used by the form window.

    \sa addResourceFile(), removeResourceFile()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::addResourceFile(const QString &path)

    Adds the resource file at the given \a path to those used by the form.

    \sa resourceFiles(), resourceFilesChanged()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::removeResourceFile(const QString &path)

    Removes the resource file at the specified \a path from the list
    of those used by the form.

    \sa resourceFiles(), resourceFilesChanged()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::ensureUniqueObjectName(QObject *object)

    Ensures that the specified \a object has a unique name amongst the
    other objects on the form.

    \internal
*/

// Slots

/*!
    \fn virtual void EDesignerFormWindowInterface::manageWidget(QWidget *widget)

    Instructs the form window to manage the specified \a widget.

    \sa isManaged(), unmanageWidget(), widgetManaged()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::unmanageWidget(QWidget *widget)

    Instructs the form window not to manage the specified \a widget.

    \sa aboutToUnmanageWidget(), widgetUnmanaged()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setFeatures(Feature features)

    Enables the specified \a features for the form window.

    \sa features(), featureChanged()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setDirty(bool dirty)

    If \a dirty is true, the form window is marked as dirty, meaning
    that it is modified but not saved. If \a dirty is false, the form
    window is considered to be unmodified.

    \sa isDirty()
*/

/*!
\fn virtual void EDesignerFormWindowInterface::clearSelection(bool update)

    Clears the current selection in the form window. If \a update is
    true, the emitSelectionChanged() function is called, emitting the
    selectionChanged() signal.

    \sa selectWidget()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::selectWidget(QWidget *widget, bool select)

    If \a select is true, the given \a widget is selected; otherwise
    the \a widget is deselected.

    \sa clearSelection(), selectionChanged()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setGrid(const QPoint &grid)

    Sets the grid size for the form window to the point specified by
    \a grid. In this function, the coordinates in the QPoint are used
    to specify the dimensions of a rectangle in the grid.

    \sa grid()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setFileName(const QString &fileName)

    Sets the file name for the form to the given \a fileName.

    \sa fileName(), fileNameChanged()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::setContents(const QString &contents)

    Sets the contents of the form using data read from the specified
    \a contents string.

    \sa contents()
*/

/*!
    \fn virtual void EDesignerFormWindowInterface::editWidgets()

    Switches the form window into editing mode.

    \sa \l {Qt Designer's Form Editing Mode}

    \internal
*/

// Signals

/*!
    \fn void EDesignerFormWindowInterface::mainContainerChanged(QWidget *mainContainer)

    This signal is emitted whenever the main container changes.
    The new container is specified by \a mainContainer.

    \sa setMainContainer()
*/

/*!
    \fn void EDesignerFormWindowInterface::toolChanged(int toolIndex)

    This signal is emitted whenever the current tool changes.
    The specified \a toolIndex is the index of the new tool in the list of
    tools in the widget box.

    \internal
*/

/*!
    \fn void EDesignerFormWindowInterface::fileNameChanged(const QString &fileName)

    This signal is emitted whenever the file name of the form changes.
    The new file name is specified by \a fileName.

    \sa setFileName()
*/

/*!
    \fn void EDesignerFormWindowInterface::featureChanged(Feature feature)

    This signal is emitted whenever a feature changes in the form.
    The new feature is specified by \a feature.

    \sa setFeatures()
*/

/*!
    \fn void EDesignerFormWindowInterface::selectionChanged()

    This signal is emitted whenever the selection in the form changes.

    \sa selectWidget(), clearSelection()
*/

/*!
    \fn void EDesignerFormWindowInterface::geometryChanged()

    This signal is emitted whenever the form's geometry changes.
*/

/*!
    \fn void EDesignerFormWindowInterface::resourceFilesChanged()

    This signal is emitted whenever the list of resource files used by the
    form changes.

    \sa resourceFiles()
*/

/*!
    \fn void EDesignerFormWindowInterface::widgetManaged(QWidget *widget)

    This signal is emitted whenever a widget on the form becomes managed.
    The newly managed widget is specified by \a widget.

    \sa manageWidget()
*/

/*!
    \fn void EDesignerFormWindowInterface::widgetUnmanaged(QWidget *widget)

    This signal is emitted whenever a widget on the form becomes unmanaged.
    The newly released widget is specified by \a widget.

    \sa unmanageWidget(), aboutToUnmanageWidget()
*/

/*!
    \fn void EDesignerFormWindowInterface::aboutToUnmanageWidget(QWidget *widget)

    This signal is emitted whenever a widget on the form is about to
    become unmanaged.  When this signal is emitted, the specified \a
    widget is still managed, and a widgetUnmanaged() signal will
    follow, indicating when it is no longer managed.

    \sa unmanageWidget(), isManaged()
*/

/*!
    \fn void EDesignerFormWindowInterface::activated(QWidget *widget)

    This signal is emitted whenever a widget is activated on the form.
    The activated widget is specified by \a widget.
*/

/*!
    \fn void EDesignerFormWindowInterface::changed()

    This signal is emitted whenever a form is changed.
*/

/*!
    \fn void EDesignerFormWindowInterface::widgetRemoved(QWidget *widget)

    This signal is emitted whenever a widget is removed from the form.
    The widget that was removed is specified by \a widget.
*/

/*!
    \fn void EDesignerFormWindowInterface::objectRemoved(QObject *object)

    This signal is emitted whenever an object (such as
    an action or a QButtonGroup) is removed from the form.
    The object that was removed is specified by \a object.

   \since 4.5
*/
