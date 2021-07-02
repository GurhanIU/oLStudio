#include "menuboxtreewidget.h"
#include "menuboxcategorylistview.h"

#include "sheet_delegate_p.h"

#include <QApplication>
#include <QContextMenuEvent>
#include <QMenu>
#include <QHeaderView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>
#include <QDebug>

enum TopLevelRole  { NORMAL_ITEM, SCRATCHPAD_ITEM, CUSTOM_ITEM };

static void setTopLevelRole(TopLevelRole tlr, QTreeWidgetItem *item)
{
    item->setData(0, Qt::UserRole, QVariant(tlr));
}

static TopLevelRole topLevelRole(const  QTreeWidgetItem *item)
{
    return static_cast<TopLevelRole>(item->data(0, Qt::UserRole).toInt());
}

MenuBoxTreeWidget::MenuBoxTreeWidget(EDesignerFormEditorInterface *core, QWidget *parent) :
    QTreeWidget(parent),
    m_core(core),
    m_iconMode(false),
    m_scratchPadDeleteTimer(0)
{
    setFocusPolicy(Qt::NoFocus);
    setIndentation(10);
//    setRootIsDecorated(false);
    setColumnCount(1);
    header()->hide();
    header()->setSectionResizeMode(QHeaderView::Stretch);
    setTextElideMode(Qt::ElideMiddle);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    setItemDelegate(new SheetDelegate(this, this));

    connect(this, SIGNAL(itemPressed(QTreeWidgetItem*,int)),
            this, SLOT(handleMousePress(QTreeWidgetItem*)));
}

MenuBoxTreeWidget::~MenuBoxTreeWidget()
{
    saveExpandedState();
}

int MenuBoxTreeWidget::categoryCount() const
{
    return topLevelItemCount();
}

MenuBoxTreeWidget::Category MenuBoxTreeWidget::category(int cat_idx) const
{
    if (cat_idx >= topLevelItemCount())
        return Category();

    QTreeWidgetItem *cat_item = topLevelItem(cat_idx);

    QTreeWidgetItem *embedItem = cat_item->child(0);
    MenuBoxCategoryListView *categoryView = static_cast<MenuBoxCategoryListView*>(itemWidget(embedItem, 0));

    Category result = categoryView->category();
    result.setName(cat_item->text(0));

    switch (topLevelRole(cat_item)) {
    case SCRATCHPAD_ITEM:
        result.setType(Category::Scratchpad);
        break;
    default:
        result.setType(Category::Default);
        break;
    }
    return result;
}

void MenuBoxTreeWidget::addCategory(const MenuBoxTreeWidget::Category &cat)
{
    if (cat.pageCount() == 0)
            return;

    const bool isScratchPad = cat.type() == Category::Scratchpad;
    MenuBoxCategoryListView *categoryView;
    QTreeWidgetItem *cat_item;

    if (isScratchPad)
    {
        const int idx = ensureScratchpad();
        categoryView = categoryViewAt(idx);
        cat_item = topLevelItem(idx);
    }
    else
    {
        const int existingIndex = indexOfCategory(cat.name());
        if (existingIndex == -1)
        {
            cat_item = new QTreeWidgetItem();
            cat_item->setText(0, cat.name());
            setTopLevelRole(NORMAL_ITEM, cat_item);
            // insert before scratchpad
            const int scratchPadIndex = indexOfScratchpad();
            if (scratchPadIndex == -1)
                addTopLevelItem(cat_item);
            else
                insertTopLevelItem(scratchPadIndex, cat_item);

            setItemExpanded(cat_item, true);
            categoryView = addCategoryView(cat_item, m_iconMode);
        }
        else
        {
            categoryView = categoryViewAt(existingIndex);
            cat_item = topLevelItem(existingIndex);
        }
    }
    // The same categories are read from the file $HOME, avoid duplicates
    const int pageCount = cat.pageCount();
    for (int i = 0; i < pageCount; ++i)
    {
        const Page p = cat.page(i);
        if (!categoryView->containsPage(p.name()))
            categoryView->addPage(p, iconForPage(p.iconName()), isScratchPad);
    }
    adjustSubListSize(cat_item);
}

void MenuBoxTreeWidget::removeCategory(int cat_idx)
{
    if (cat_idx >= topLevelItemCount())
        return;
    delete takeTopLevelItem(cat_idx);
}

int MenuBoxTreeWidget::pageCount(int cat_idx) const
{
    if (cat_idx >= topLevelItemCount())
        return 0;
    // SDK functions want unfiltered access
    return categoryViewAt(cat_idx)->count(MenuBoxCategoryListView::UnfilteredAccess);
}

MenuBoxTreeWidget::Page MenuBoxTreeWidget::page(int cat_idx, int page_idx) const
{
    if (cat_idx >= topLevelItemCount())
        return Page();
    // SDK functions want unfiltered access
    MenuBoxCategoryListView *categoryView = categoryViewAt(cat_idx);
    return categoryView->pageAt(MenuBoxCategoryListView::UnfilteredAccess, page_idx);
}

void MenuBoxTreeWidget::addPage(int cat_idx, const Page &page)
{
    if (cat_idx >= topLevelItemCount())
        return;

    QTreeWidgetItem *cat_item = topLevelItem(cat_idx);
    MenuBoxCategoryListView *categoryView = categoryViewAt(cat_idx);

    const bool scratch = topLevelRole(cat_item) == SCRATCHPAD_ITEM;
    categoryView->addPage(page, iconForPage(page.iconName()), scratch);
    adjustSubListSize(cat_item);
}

void MenuBoxTreeWidget::removePage(int cat_idx, int page_idx)
{
    if (cat_idx >= topLevelItemCount())
        return;

    MenuBoxCategoryListView *categoryView = categoryViewAt(cat_idx);

    // SDK functions want unfiltered access
    const MenuBoxCategoryListView::AccessMode am = MenuBoxCategoryListView::UnfilteredAccess;
    if (page_idx >= categoryView->count(am))
        return;

    categoryView->removeRow(am, page_idx);
}

/// TODO
/* Kapattim
void MenuBoxTreeWidget::dropWidgets(const QList<EDesignerDnDItemInterface *> &item_list)
{
    QTreeWidgetItem *scratch_item = 0;
    MenuBoxCategoryListView *categoryView = 0;
    bool added = false;

    foreach (EDesignerDnDItemInterface *item, item_list)
    {
        QWidget *w = item->widget();
        if (w == 0)
            continue;

        DomUI *dom_ui = item->domUi();
        if (dom_ui == 0)
            continue;

        const int scratch_idx = ensureScratchpad();
        scratch_item = topLevelItem(scratch_idx);
        categoryView = categoryViewAt(scratch_idx);

        // Temporarily remove the fake toplevel in-between
        DomWidget *fakeTopLevel = dom_ui->takeElementWidget();
        DomWidget *firstWidget = 0;
        if (fakeTopLevel && !fakeTopLevel->elementWidget().isEmpty())
        {
            firstWidget = fakeTopLevel->elementWidget().first();
            dom_ui->setElementWidget(firstWidget);
        }
        else
        {
            dom_ui->setElementWidget(fakeTopLevel);
            continue;
        }

        // Serialize to XML
        QString xml;
        {
            QXmlStreamWriter writer(&xml);
            writer.setAutoFormatting(true);
            writer.setAutoFormattingIndent(1);
            writer.writeStartDocument();
            dom_ui->write(writer);
            writer.writeEndDocument();
        }

        // Insert fake toplevel again
        dom_ui->takeElementWidget();
        dom_ui->setElementWidget(fakeTopLevel);

        const Widget wgt = Widget(w->objectName(), xml);
        categoryView->addWidget(wgt, iconForWidget(wgt.iconName()), true);
        setItemExpanded(scratch_item, true);
        added = true;
    }

    if (added)
    {
        save();
        QApplication::setActiveWindow(this);
        // Is the new item visible in filtered mode?
        const WidgetBoxCategoryListView::AccessMode am = WidgetBoxCategoryListView::FilteredAccess;
        if (const int count = categoryView->count(am))
            categoryView->setCurrentItem(am, count - 1);
        categoryView->adjustSize(); // XXX
        adjustSubListSize(scratch_item);
    }
}*/

void MenuBoxTreeWidget::setFileName(const QString &file_name)
{
    m_file_name = file_name;
}

QString MenuBoxTreeWidget::fileName() const
{
    return m_file_name;
}

bool MenuBoxTreeWidget::load(EMenuBox::LoadMode loadMode)
{
    switch (loadMode) {
    case EMenuBox::LoadReplace:
        clear();
        break;
    case EMenuBox::LoadCustomWidgetsOnly:
        addCustomCategories(true);
        updateGeometry();
        return true;
    default:
        break;
    }

    const QString name = fileName();

    QSqlDatabase db = QSqlDatabase::database(name);

    if (!db.isValid())
        return false;

    if (!db.isOpen())
    {
        if (!db.open())
            return false;
    }

    return loadContents(db);

//    QFile f(name);
//    if (!f.open(QIODevice::ReadOnly)) // Might not exist at first startup
//        return false;

//    const QString contents = QString::fromUtf8(f.readAll());

}

bool MenuBoxTreeWidget::loadContents(const QSqlDatabase &db)
{
    QString errorMessage;
    CategoryList cat_list;

    if (!readCategories(m_file_name, db, &cat_list, &errorMessage))
    {
        qDebug() << Q_FUNC_INFO << errorMessage;
//        qdesigner_internal::designerWarning(errorMessage);
        return false;
    }

    foreach(const Category &cat, cat_list)
        addCategory(cat);

    addCustomCategories(false);
    // Restore which items are expanded
    restoreExpandedState();
    return true;
}

bool MenuBoxTreeWidget::save()
{
    if (fileName().isEmpty())
        return false;

    QFile file(fileName());
    if (!file.open(QIODevice::WriteOnly))
        return false;

    CategoryList cat_list;
    const int count = categoryCount();
    for (int i = 0; i < count; ++i)
        cat_list.append(category(i));

//    QXmlStreamWriter writer(&file);
//    writer.setAutoFormatting(true);
//    writer.setAutoFormattingIndent(1);
//    writer.writeStartDocument();
//    writeCategories(writer, cat_list);
//    writer.writeEndDocument();

    return true;
}

QIcon MenuBoxTreeWidget::iconForPage(QString iconName) const
{
//    if (iconName.isEmpty())
//        iconName = QString();

//    if (iconName.startsWith(QLatin1String(iconPrefixC)))
//    {
//        const IconCache::const_iterator it = m_pluginIcons.constFind(iconName);
//        if (it != m_pluginIcons.constEnd())
//            return it.value();
//    }

//    return createIconSet(iconName);
    return QIcon();
}

void MenuBoxTreeWidget::filter(const QString &f)
{
    const bool empty = f.isEmpty();
    const QRegExp re = empty ? QRegExp() : QRegExp(f, Qt::CaseInsensitive, QRegExp::FixedString);
    const int numTopLevels = topLevelItemCount();
    bool changed = false;
    for (int i = 0; i < numTopLevels; i++)
    {
        QTreeWidgetItem *tl = topLevelItem(i);
        MenuBoxCategoryListView *categoryView = categoryViewAt(i);
        // Anything changed -> Enable the category
        const int oldCount = categoryView->count(MenuBoxCategoryListView::FilteredAccess);
        categoryView->filter(re);
        const int newCount = categoryView->count(MenuBoxCategoryListView::FilteredAccess);
        if (oldCount != newCount)
        {
            changed = true;
            const bool categoryEnabled = newCount > 0 || empty;
            if (categoryEnabled)
            {
                categoryView->adjustSize();
                adjustSubListSize(tl);
            }
            setRowHidden (i, QModelIndex(), !categoryEnabled);
        }
    }
    if (changed)
        updateGeometries();
}

void MenuBoxTreeWidget::contextMenuEvent(QContextMenuEvent *e)
{
    QTreeWidgetItem *item = itemAt(e->pos());

    const bool scratchpad_menu = item != 0
                            && item->parent() != 0
                            && topLevelRole(item->parent()) == SCRATCHPAD_ITEM;

    QMenu menu;
    menu.addAction(tr("Expand all"), this, SLOT(expandAll()));
    menu.addAction(tr("Collapse all"), this, SLOT(collapseAll()));
    menu.addSeparator();

    QAction *listModeAction = menu.addAction(tr("List View"));
    QAction *iconModeAction = menu.addAction(tr("Icon View"));
    listModeAction->setCheckable(true);
    iconModeAction->setCheckable(true);

    QActionGroup *viewModeGroup = new QActionGroup(&menu);
    viewModeGroup->addAction(listModeAction);
    viewModeGroup->addAction(iconModeAction);

    if (m_iconMode)
        iconModeAction->setChecked(true);
    else
        listModeAction->setChecked(true);

    connect(listModeAction, SIGNAL(triggered()), SLOT(slotListMode()));
    connect(iconModeAction, SIGNAL(triggered()), SLOT(slotIconMode()));

    if (scratchpad_menu)
    {
        menu.addSeparator();
        menu.addAction(tr("Remove"), itemWidget(item, 0), SLOT(removeCurrentItem()));
        if (!m_iconMode)
            menu.addAction(tr("Edit Name"), itemWidget(item, 0), SLOT(editCurremtItem()));
    }

    e->accept();
    menu.exec(mapToGlobal(e->pos()));
}

void MenuBoxTreeWidget::resizeEvent(QResizeEvent *e)
{
    QTreeWidget::resizeEvent(e);
    if (const int numTopLevels = topLevelItemCount())
    {
        for (int i = numTopLevels - 1; i >=0; --i)
            adjustSubListSize(topLevelItem(i));
    }
}

void MenuBoxTreeWidget::slotSave()
{
    save();
}

void MenuBoxTreeWidget::slotScratchPadItemDeleted()
{
    const int scratch_idx = indexOfScratchpad();
    QTreeWidgetItem *scratch_item = topLevelItem(scratch_idx);
    adjustSubListSize(scratch_item);
    save();
}

void MenuBoxTreeWidget::slotLastScratchPadItemDeleted()
{
    // Remove the scratchpad in the next idle loop
    if (!m_scratchPadDeleteTimer)
    {
        m_scratchPadDeleteTimer = new QTimer(this);
        m_scratchPadDeleteTimer->setSingleShot(true);
        m_scratchPadDeleteTimer->setInterval(0);
        connect(m_scratchPadDeleteTimer, SIGNAL(timeout()), this, SLOT(deleteScratchpad()));
    }
    if (!m_scratchPadDeleteTimer->isActive())
        m_scratchPadDeleteTimer->start();
}

void MenuBoxTreeWidget::handleMousePress(QTreeWidgetItem *item)
{
    if (item == 0)
        return;

    if (QApplication::mouseButtons() != Qt::LeftButton)
        return;

    if (item->parent() == 0)
    {
        setItemExpanded(item, !isItemExpanded(item));
        return;
    }
}


void MenuBoxTreeWidget::deleteScratchpad()
{
    const int idx = indexOfScratchpad();
    if (idx == -1)
        return;
    delete takeTopLevelItem(idx);
    save();
}

void MenuBoxTreeWidget::slotListMode()
{
    m_iconMode = false;
    updateViewMode();
}

void MenuBoxTreeWidget::slotIconMode()
{
    m_iconMode = true;
    updateViewMode();
}

void MenuBoxTreeWidget::updateViewMode()
{
    if (const int numTopLevels = topLevelItemCount())
    {
        for (int i = numTopLevels - 1; i >= 0; --i)
        {
            QTreeWidgetItem *topLevel = topLevelItem(i);
            // Scratch pad stays in list mode.
            const QListView::ViewMode viewMode  = m_iconMode && (topLevelRole(topLevel) != SCRATCHPAD_ITEM) ? QListView::IconMode : QListView::ListMode;
            MenuBoxCategoryListView *categoryView = categoryViewAt(i);
            if (viewMode != categoryView->viewMode())
            {
                categoryView->setViewMode(viewMode);
                adjustSubListSize(topLevelItem(i));
            }
        }
    }

    updateGeometries();
}

MenuBoxCategoryListView *MenuBoxTreeWidget::categoryViewAt(int idx) const
{
    MenuBoxCategoryListView *rc = 0;
    if (QTreeWidgetItem *cat_item = topLevelItem(idx))
    {
        if (QTreeWidgetItem *embed_item = cat_item->child(0))
            rc = qobject_cast<MenuBoxCategoryListView*>(itemWidget(embed_item, 0));
    }
    Q_ASSERT(rc);
    return rc;
}

void MenuBoxTreeWidget::adjustSubListSize(QTreeWidgetItem *cat_item)
{
    QTreeWidgetItem *embed_item = cat_item->child(0);
    if (embed_item == 0)
        return;

    MenuBoxCategoryListView *list_widget = static_cast<MenuBoxCategoryListView*>(itemWidget(embed_item, 0));
    list_widget->setFixedWidth(header()->width());
    list_widget->doItemsLayout();
    const int height = qMax(list_widget->contentsSize().height(), 1);
    list_widget->setFixedHeight(height);
    embed_item->setSizeHint(0, QSize(-1, height -1));
}

static inline QString msgSqlError(const QString &query, const QSqlError &error)
{
    return EMenuBox::tr("An Sql error has been encountered at query '%1' : %2")
            .arg(query).arg(error.text());
}

bool MenuBoxTreeWidget::readCategories(const QString &fileName, const QSqlDatabase &db,
                                       CategoryList *cats, QString *errorMessage)
{
    // Read widget box SQL:
    // Entries of category with name="invisible" should be ignored
    bool ignoreEntries = false;

    QSqlQuery qMenu = QSqlQuery(db);
    qMenu.prepare("SELECT ID, NAME FROM MENU;");
    qMenu.exec();
    while(qMenu.next())
    {
        const int categoryId = qMenu.value("ID").toInt();
        const QString categoryName = qMenu.value("NAME").toString();

        Category category(categoryName);
        cats->push_back(category);

        QSqlQuery qPage = QSqlQuery(db);
        qPage.prepare(QString("SELECT p.ID, p.NAME FROM PAGE AS p \
                                   INNER JOIN MENU_PAGE AS mp ON mp.PAGE_ID = p.ID \
                               WHERE mp.MENU_ID = %1 ORDER BY mp.PAGE_ID;").arg(categoryId));
        qPage.exec();
        while(qPage.next())
        {
            const int pageId = qPage.value("ID").toInt();
            const QString pageName = qPage.value("NAME").toString();
            MenuBoxTreeWidget::Page::Type pageType =MenuBoxTreeWidget::Page::Default;

            Page p;
            p.setName(pageName);
            p.setId(pageId);
            p.setType(pageType);

            cats->back().addPage(p);
        }

        if (qPage.lastError().isValid())
        {
            *errorMessage = msgSqlError(qPage.lastQuery(), qPage.lastError());
            return false;
        }
    }

    if (qMenu.lastError().isValid())
    {
        *errorMessage = msgSqlError(qMenu.lastQuery(), qMenu.lastError());
        return false;
    }

    return true;
}

static int findCategory(const QString &name, const MenuBoxTreeWidget::CategoryList &list)
{
    int idx = 0;
    foreach (const MenuBoxTreeWidget::Category &cat, list)
    {
        if (cat.name() == name)
            return idx;
        ++idx;
    }
    return -1;
}

static inline bool isValidIcon(const QIcon &icon)
{
    if (!icon.isNull())
    {
        const QList<QSize> availableSizes = icon.availableSizes();
        if (!availableSizes.empty())
            return !availableSizes.front().isEmpty();
    }
    return false;
}

int MenuBoxTreeWidget::indexOfScratchpad() const
{
    if (const int numTopLevels =  topLevelItemCount())
    {
        for (int i = numTopLevels - 1; i >= 0; --i)
        {
            if (topLevelRole(topLevelItem(i)) == SCRATCHPAD_ITEM)
                return i;
        }
    }
    return -1;
}

int MenuBoxTreeWidget::indexOfCategory(const QString &name) const
{
    const int topLevelCount = topLevelItemCount();
    for (int i = 0; i < topLevelCount; ++i)
    {
        if (topLevelItem(i)->text(0) == name)
            return i;
    }
    return -1;
}

int MenuBoxTreeWidget::ensureScratchpad()
{
    const int existingIndex = indexOfScratchpad();
    if (existingIndex != -1)
         return existingIndex;

    QTreeWidgetItem *scratch_item = new QTreeWidgetItem(this);
    scratch_item->setText(0, tr("Scratchpad"));
    setTopLevelRole(SCRATCHPAD_ITEM, scratch_item);
    addCategoryView(scratch_item, false); // Scratchpad in list mode.
    return categoryCount() - 1;
}

void MenuBoxTreeWidget::addCustomCategories(bool replace)
{
//    if (replace)
//    {
//        // clear out all existing custom widgets
//        if (const int numTopLevels =  topLevelItemCount())
//        {
//            for (int t = 0; t < numTopLevels ; ++t)
//                categoryViewAt(t)->removeCustomWidgets();
//        }
//    }
//    // re-add
//    const CategoryList customList = loadCustomCategoryList();
//    const CategoryList::const_iterator cend = customList.constEnd();
//    for (CategoryList::const_iterator it = customList.constBegin(); it != cend; ++it)
//        addCategory(*it);
}

MenuBoxCategoryListView *MenuBoxTreeWidget::addCategoryView(QTreeWidgetItem *parent, bool iconMode)
{
    QTreeWidgetItem *embed_item = new QTreeWidgetItem(parent);
    embed_item->setFlags(Qt::ItemIsEnabled);
    MenuBoxCategoryListView *categoryView = new MenuBoxCategoryListView(m_core, this);
    categoryView->setViewMode(iconMode ? QListView::IconMode : QListView::ListMode);
    connect(categoryView, SIGNAL(scratchPadChanged()), this, SLOT(slotSave()));
    connect(categoryView, SIGNAL(pressed(QString,QString,QPoint)), this, SIGNAL(pressed(QString,QString,QPoint)));
    connect(categoryView, SIGNAL(itemRemoved()), this, SLOT(slotScratchPadItemDeleted()));
    connect(categoryView, SIGNAL(lastItemRemoved()), this, SLOT(slotLastScratchPadItemDeleted()));
    setItemWidget(embed_item, 0, categoryView);
    return categoryView;
}

void MenuBoxTreeWidget::saveExpandedState() const
{
//    QStringList closedCategories;
//    if (const int numCategories = categoryCount())
//    {
//        for (int i = 0; i < numCategories; ++i)
//        {
//            const QTreeWidgetItem *cat_item = topLevelItem(i);
//            if (!isItemExpanded(cat_item))
//                closedCategories.append(cat_item->text(0));
//        }
//    }

//    EDesignerSettingsInterface *settings = m_core->settingsManager();
//    settings->beginGroup(QLatin1String(widgetBoxRootElementC));
//    settings->setValue(QLatin1String("Closed categories"), closedCategories);
//    settings->setValue(QLatin1String("View mode"), m_iconMode);
//    settings->endGroup();
}

void  MenuBoxTreeWidget::restoreExpandedState()
{
//    typedef QSet<QString> StringSet;
//    EDesignerSettingsInterface *settings = m_core->settingsManager();
//    m_iconMode = settings->value(QLatin1String("WidgetBox/View mode")).toBool();
//    updateViewMode();
//    const StringSet closedCategories = settings->value(QLatin1String("WidgetBox/Closed categories"), QStringList()).toStringList().toSet();
//    expandAll();
//    if (closedCategories.empty())
//        return;

//    if (const int numCategories = categoryCount())
//    {
//        for (int i = 0; i < numCategories; ++i)
//        {
//            QTreeWidgetItem *item = topLevelItem(i);
//            if (closedCategories.contains(item->text(0)))
//                item->setExpanded(false);
//        }
//    }
}
