#include "menubox.h"
#include "menuboxtreewidget.h"

#include "abstractformeditor.h"
//#include "abstractformwindowmanager.h"

#include "iconloader_p.h"
//#include "edesigner_utils_p.h"
#include "filterwidget_p.h"

#include <QDropEvent>
#include <QVBoxLayout>
#include <QApplication>
#include <QToolBar>
#include <QIcon>
#include <QSqlDatabase>
#include <QDebug>

MenuBox::MenuBox(EDesignerFormEditorInterface *core, QWidget *parent, Qt::WindowFlags flags)
    : EMenuBox(parent, flags),
      m_core(core),
      m_view(new MenuBoxTreeWidget(m_core))
{

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);

    // Prevent the filter from grabbing focus since Our view has Qt::NoFocus
    FilterWidget *filterWidget = new FilterWidget(0, FilterWidget::LayoutAlignNone);
    filterWidget->setRefuseFocus(true);
    connect(filterWidget, SIGNAL(filterChanged(QString)), m_view, SLOT(filter(QString)));

    QToolBar *toolBar = new QToolBar(this);
    toolBar->addWidget(filterWidget);
    l->addWidget(toolBar);

    // View
    connect(m_view, SIGNAL(pressed(QString,QString,QPoint)), this, SLOT(handleMousePress(QString,QString,QPoint)));
    l->addWidget(m_view);

    setAcceptDrops (true);
}

MenuBox::~MenuBox()
{
}

EDesignerFormEditorInterface *MenuBox::core() const
{
    return m_core;
}

void MenuBox::handleMousePress(const QString &name, const QString &id, const QPoint &global_mouse_pos)
{
    if (QApplication::mouseButtons() != Qt::LeftButton)
        return;

    emitPageChanged(name, id.toInt());

//    qDebug() << id << name;

//    DomUI *ui = xmlToUi(name, xml, true);
//    if (ui == 0)
//        return;
//    QList<EDesignerDnDItemInterface*> item_list;
//    item_list.append(new WidgetBoxDnDItem(core(), ui, global_mouse_pos));
//    m_core->formWindowManager()->dragItems(item_list);
}

int MenuBox::categoryCount() const
{
    return m_view->categoryCount();
}

EMenuBoxInterface::Category MenuBox::category(int cat_idx) const
{
    return m_view->category(cat_idx);
}

void MenuBox::addCategory(const Category &cat)
{
    m_view->addCategory(cat);
}

void MenuBox::removeCategory(int cat_idx)
{
    m_view->removeCategory(cat_idx);
}

int MenuBox::pageCount(int cat_idx) const
{
    return m_view->pageCount(cat_idx);
}

EMenuBoxInterface::Page MenuBox::page(int cat_idx, int page_idx) const
{
    return m_view->page(cat_idx, page_idx);
}

void MenuBox::addPage(int cat_idx, const Page &page)
{
    m_view->addPage(cat_idx, page);
}

void MenuBox::removePage(int cat_idx, int page_idx)
{
    m_view->removePage(cat_idx, page_idx);
}

//void MenuBox::dropWidgets(const QList<EDesignerDnDItemInterface*> &item_list, const QPoint&)
//{
//    m_view->dropWidgets(item_list);
//}

void MenuBox::setFileName(const QString &file_name)
{
    m_view->setFileName(file_name);
}

QString MenuBox::fileName() const
{
    return m_view->fileName();
}

bool MenuBox::load()
{
    return m_view->load(loadMode());
}

bool MenuBox::loadContents(const QSqlDatabase &database)
{
    return m_view->loadContents(database);
}

bool MenuBox::save()
{
    return m_view->save();
}

//static const EDesignerMimeData *checkDragEvent(QDropEvent * event, bool acceptEventsFromWidgetBox)
//{
//    const EDesignerMimeData *mimeData = qobject_cast<const EDesignerMimeData *>(event->mimeData());
//    if (!mimeData)
//    {
//        event->ignore();
//        return 0;
//    }
//    // If desired, ignore a widget box drag and drop, where widget==0.
//    if (!acceptEventsFromWidgetBox)
//    {
//        const bool fromWidgetBox = !mimeData->items().first()->widget();
//        if (fromWidgetBox)
//        {
//            event->ignore();
//            return 0;
//        }
//    }

//    mimeData->acceptEvent(event);
//    return mimeData;
//}

//void WidgetBox::dragEnterEvent (QDragEnterEvent * event)
//{
//    // We accept event originating from the widget box also here,
//    // because otherwise Windows will not show the DnD pixmap.
//    checkDragEvent(event, true);
//}

//void WidgetBox::dragMoveEvent(QDragMoveEvent * event)
//{
//    checkDragEvent(event, true);
//}

//void WidgetBox::dropEvent(QDropEvent * event)
//{
//    const EDesignerMimeData *mimeData = checkDragEvent(event, false);
//    if (!mimeData)
//        return;

//    dropWidgets(mimeData->items(), event->pos());
//    EDesignerMimeData::removeMovedWidgetsFromSourceForm(mimeData->items());
//}

QIcon MenuBox::iconForPage(const QString &className, const QString &category) const
{
    Page pageData;
    if (!findPage(this, className, category, &pageData))
        return QIcon();
    return m_view->iconForPage(pageData.iconName());
}
