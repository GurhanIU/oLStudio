#include "menuboxcategorylistview.h"

//#include "edesigner_utils_p.h"
#include "abstractformeditor.h"
//#include "abstractwidgetdatabase.h"
#include "abstractmenubox.h"

#include <QIcon>
#include <QtWidgets/QListView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QItemDelegate>
#include <QSortFilterProxyModel>

#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QList>
#include <QTextStream>
#include <QRegExp>
#include <QDebug>

enum { FilterRole = Qt::UserRole + 11 };

struct MenuBoxCategoryEntry {
    MenuBoxCategoryEntry();
    explicit MenuBoxCategoryEntry(const EMenuBoxInterface::Page &page,
                                    const QString &filter,
                                    const QIcon &icon,
                                    bool editable);

    EMenuBoxInterface::Page page;
    QString toopTip;
    QString whatsThis;
    QString filter;
    QIcon icon;
    bool editable;
};

MenuBoxCategoryEntry::MenuBoxCategoryEntry() : editable(false)
{
}

MenuBoxCategoryEntry::MenuBoxCategoryEntry(const EMenuBoxInterface::Page &p,
                                           const QString &filterIn,
                                           const QIcon &i, bool e) :
    page(p),
    filter(filterIn),
    icon(i),
    editable(e)
{
}

/* WidgetBoxCategoryModel, representing a list of category entries. Uses a
 * QAbstractListModel since the behaviour depends on the view of the list
 * view, it does not return text in the case of IconMode. */

class MenuBoxCategoryModel : public QAbstractListModel {
public:
    explicit MenuBoxCategoryModel(EDesignerFormEditorInterface *core, QObject *parent = 0);

    //QAbstractListModel
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    // The model returns no text in icon mode, so, it also needs to know it
    QListView::ViewMode viewMode() const;
    void setViewMode(QListView::ViewMode vm);

    void addPage(const EMenuBoxInterface::Page &page, const QIcon &icon, bool editable);

    EMenuBoxInterface::Page pageAt(const QModelIndex & index) const;
    EMenuBoxInterface::Page pageAt(int row) const;

    int indexofPage(const QString &name);

    EMenuBoxInterface::Category category() const;
    bool removeCustomPages();

private:
    typedef QList<MenuBoxCategoryEntry> MenuBoxCategoryEntries;

    QRegExp m_classNameRegExp;
    EDesignerFormEditorInterface *m_core;
    MenuBoxCategoryEntries m_items;
    QListView::ViewMode m_viewMode;

};

MenuBoxCategoryModel::MenuBoxCategoryModel(EDesignerFormEditorInterface *core, QObject *parent) :
    QAbstractListModel(parent),
    m_classNameRegExp(QLatin1String("<widget +class *= *\"([^\"]+)\"")),
    m_core(core),
    m_viewMode(QListView::ListMode)
{
    Q_ASSERT(m_classNameRegExp.isValid());
}

QListView::ViewMode MenuBoxCategoryModel::viewMode() const
{
    return m_viewMode;
}

void MenuBoxCategoryModel::setViewMode(QListView::ViewMode vm)
{
//    if (m_viewMode == vm)
//        return;
//    m_viewMode = vm;
//    if (!m_items.empty())
//        reset(); /// TODO: Bakilacak

    if (m_viewMode == vm)
        return;
    const bool empty = m_items.isEmpty();
    if (!empty)
        beginResetModel();
    m_viewMode = vm;
    if (!empty)
        endResetModel();
}

int MenuBoxCategoryModel::indexofPage(const QString &name)
{
    const int count = m_items.size();
    for (int i = 0; i < count; i++)
    {
        if(m_items.at(i).page.name() == name)
            return i;
    }
    return -1;
}

EMenuBoxInterface::Category MenuBoxCategoryModel::category() const
{
    EMenuBoxInterface::Category rc;
    const MenuBoxCategoryEntries::const_iterator cend = m_items.constEnd();
    for (MenuBoxCategoryEntries::const_iterator it = m_items.constBegin(); it != cend; ++it)
        rc.addPage(it->page);
    return rc;
}

bool MenuBoxCategoryModel::removeCustomPages()
{
    // Typecially, we are whole category of custom widgets, so, remove all
    // and do reset.
    bool changed = false;
    for(MenuBoxCategoryEntries::iterator it = m_items.begin(); it != m_items.end();)
    {
        if (it->page.type() == EMenuBoxInterface::Page::Custom )
        {
            it = m_items.erase(it);
            changed = true;
        }
        else
            ++it;
    }

    if (changed)
        endResetModel();
    return changed;
}


void MenuBoxCategoryModel::addPage(const EMenuBoxInterface::Page &page, const QIcon &icon, bool editable)
{
    // build item. Filter on name + class name if it is different and not a layout.
    QString filter = page.name();

    MenuBoxCategoryEntry item(page, filter, icon, editable);

    // insert
    const int row = m_items.size();
    beginInsertRows(QModelIndex(), row, row);
    m_items.push_back(item);
    endInsertRows();
}

QVariant MenuBoxCategoryModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if (row < 0 || row >= m_items.size())
        return QVariant();

    const MenuBoxCategoryEntry &item = m_items.at(row);
    switch (role) {
    case Qt::DisplayRole:
        // No text in icon mode
        return QVariant(m_viewMode == QListView::ListMode ? item.page.name() : QString());
        break;
    case Qt::DecorationRole:
        return QVariant(item.icon);
    case Qt::EditRole:
        return QVariant(item.page.name());
    case Qt::ToolTipRole: {
        if (m_viewMode == QListView::ListMode)
            return QVariant(item.toopTip);
        // Icon mode tooltip should contain the class name
        QString tt = item.page.name();
        if (!item.toopTip.isEmpty())
        {
            tt += QLatin1Char('\n');
            tt += item.toopTip;
        }
        return QVariant(tt);
    }
    case Qt::WhatsThisRole:
        return QVariant(item.whatsThis);
    case FilterRole:
        return item.filter;
    }
    return QVariant();
}

bool MenuBoxCategoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const int row = index.row();
    if (role != Qt::EditRole || row < 0 || row >= m_items.size() || value.type() != QVariant::String)
        return false;
    // Set name and adopt Xml
    MenuBoxCategoryEntry &item = m_items[row];
    const QString newName = value.toString();
    item.page.setName(newName);

//    const QDomDocument doc = stringToDom(WidgetBoxCategoryListView::widgetDomXml(item.widget));
//    QDomElement widget_elt = doc.firstChildElement(QLatin1String(widgetElementC));
//    if (!widget_elt.isNull())
//    {
//        widget_elt.setAttribute(QLatin1String(nameAttributeC), newName);
//        item.widget.setDomXml(domToString(widget_elt));
//    }
    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags MenuBoxCategoryModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags rc = Qt::ItemIsEnabled;
    const int row = index.row();
    if (row >= 0 && row < m_items.size())
    {
        rc |= Qt::ItemIsSelectable;
        // Can change name in list mode only
        if (m_viewMode == QListView::ListMode)
            rc |= Qt::ItemIsEditable;
    }
    return rc;
}

int MenuBoxCategoryModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_items.size();
}

bool MenuBoxCategoryModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || count < 1)
        return false;
    const int size = m_items.size();
    const int last = row + count -1;
    if (row >= size || last >= size)
        return false;
    beginRemoveRows(parent, row, last);
    for (int r = last; r >= row; r--)
        m_items.removeAt(r);
    endRemoveRows();
    return true;
}

EMenuBoxInterface::Page MenuBoxCategoryModel::pageAt(const QModelIndex &index) const
{
    return pageAt(index.row());
}

EMenuBoxInterface::Page MenuBoxCategoryModel::pageAt(int row) const
{
    if ( row < 0 || row >+ m_items.size())
        return EMenuBoxInterface::Page();
    return m_items.at(row).page;
}

/* WidgetSubBoxItemDelegate, ensures a valid name using a regexp validator */

class MenuBoxCategoryEntryDelegate : public QItemDelegate
{
public:
    explicit MenuBoxCategoryEntryDelegate(QWidget *parent = 0) : QItemDelegate(parent) {}
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
 };

QWidget * MenuBoxCategoryEntryDelegate::createEditor(QWidget *parent,
                                                       const QStyleOptionViewItem &option,
                                                       const QModelIndex &index) const

{
    QWidget *result = QItemDelegate::createEditor(parent, option, index);
    if (QLineEdit *line_edit = qobject_cast<QLineEdit*>(result))
    {
        const QRegExp re = QRegExp(QLatin1String("[_a-zA-Z][_a-zA-Z0-9]*"));
        Q_ASSERT(re.isValid());
        line_edit->setValidator(new QRegExpValidator(re, line_edit));
    }
    return result;
}

// ------------------------ WidgetBoxCategoryListView

MenuBoxCategoryListView::MenuBoxCategoryListView(EDesignerFormEditorInterface *core, QWidget *parent) :
    QListView(parent),
    m_proxyModel(new QSortFilterProxyModel(this)),
    m_model(new MenuBoxCategoryModel(core, this))
{
    setFocusPolicy(Qt::NoFocus);
    setFrameShape(QFrame::NoFrame);
    setIconSize(QSize(22, 22));
    setSpacing(1);
    setTextElideMode(Qt::ElideMiddle);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setResizeMode(QListView::Adjust);
    setUniformItemSizes(true);

    setItemDelegate( new MenuBoxCategoryEntryDelegate(this));

    connect(this, SIGNAL(pressed(QModelIndex)), this, SLOT(slotPressed(QModelIndex)));
    setEditTriggers(QAbstractItemView::AnyKeyPressed);

    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterRole(FilterRole);
    setModel(m_proxyModel);
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(scratchPadChanged()));
}

void MenuBoxCategoryListView::setViewMode(ViewMode vm)
{
    QListView::setViewMode(vm);
    m_model->setViewMode(vm);
}

void MenuBoxCategoryListView::setCurrentItem(AccessMode am, int row)
{
    const QModelIndex index = am == FilteredAccess ?
                m_proxyModel->index(row, 0) :
                m_proxyModel->mapFromSource(m_model->index(row, 0));

    if (index.isValid())
        setCurrentIndex(index);
}

void MenuBoxCategoryListView::slotPressed(const QModelIndex &index)
{
    /// TODO Bakilacak : Alltaki 2 satiri ben ekledim.
//    const QModelIndex idx = m_proxyModel->index(index.row(), index.column(), QModelIndex());
//    const EMenuBoxInterface::Page page = m_model->pageAt(idx);

    const EMenuBoxInterface::Page page = m_model->pageAt(m_proxyModel->mapToSource(index)); // Orjinali
    if (page.isNull())
        return;
    emit pressed(page.name(), page.idAsString(), QCursor::pos());
}

void MenuBoxCategoryListView::removeCurrentItem()
{
    const QModelIndex index = currentIndex();
    if (!index.isValid() || !m_proxyModel->removeRow(index.row()))
        return;

    // We check the unfiltered item count here, we dont't want to get removed if he
    // filtered view is empty
    if (m_model->rowCount())
        emit itemRemoved();
    else
        emit lastItemRemoved();
}

void MenuBoxCategoryListView::editCurrentItem()
{
    const QModelIndex index = currentIndex();
    if (index.isValid())
        edit(index);
}

int MenuBoxCategoryListView::count(AccessMode am) const
{
    return am == FilteredAccess ? m_proxyModel->rowCount() : m_model->rowCount();
}

int MenuBoxCategoryListView::mapRowToSource(int filterRow) const
{
    const QModelIndex filterIndex = m_proxyModel->index(filterRow, 0);
    return m_proxyModel->mapToSource(filterIndex).row();
}

EMenuBoxInterface::Page MenuBoxCategoryListView::pageAt(AccessMode am, const QModelIndex &index) const
{
    const QModelIndex unfilteredIndex = am == FilteredAccess ? m_proxyModel->mapToSource(index) : index;
    return m_model->pageAt(unfilteredIndex);
}

EMenuBoxInterface::Page MenuBoxCategoryListView::pageAt(AccessMode am, int row) const
{
    return m_model->pageAt(am == UnfilteredAccess ? row : mapRowToSource(row));
}

void MenuBoxCategoryListView::removeRow(AccessMode am, int row)
{
    m_model->removeRow(am == UnfilteredAccess ? row : mapRowToSource(row));
}

bool MenuBoxCategoryListView::containsPage(const QString &name)
{
    return m_model->indexofPage(name) != -1;
}

void MenuBoxCategoryListView::addPage(const EMenuBoxInterface::Page &page, const QIcon &icon, bool editable)
{
    m_model->addPage(page, icon, editable);
}

//QString MenuBoxCategoryListView::widgetDomXml(const EMenuBoxInterface::Widget &widget)
//{
//    QString domXml = widget.domXml();
//    if (domXml.isEmpty())
//    {
//        domXml = QLatin1String(uiOpeningTagC);
//        domXml += QLatin1String("<widget class=\"");
//        domXml += widget.name();
//        domXml += QLatin1String("\"/>");
//        domXml += QLatin1String(uiClosingTagC);
//    }
//    return domXml;
//}

void MenuBoxCategoryListView::filter(const QRegExp &re)
{
    m_proxyModel->setFilterRegExp(re);
}

EMenuBoxInterface::Category MenuBoxCategoryListView::category() const
{
    return m_model->category();
}

bool MenuBoxCategoryListView::removeCustomPages()
{
    return m_model->removeCustomPages();
}
