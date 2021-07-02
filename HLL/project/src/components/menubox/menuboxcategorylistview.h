#ifndef MENUBOXCATEGORYLISTVIEW_H
#define MENUBOXCATEGORYLISTVIEW_H

#include "abstractmenubox.h"
//#include "abstractdnditem.h"

#include <QListView>
#include <QList>

class EDesignerFormEditorInterface;

class QSortFilterProxyModel;
class QRegExp;

class MenuBoxCategoryModel;

// List view oif a category, switchable between icon and list mode.
// Provides a filtered view.
class MenuBoxCategoryListView : public QListView
{
    Q_OBJECT
public:
    //Whether to access the filtered or unfiltered view
    enum AccessMode { FilteredAccess, UnfilteredAccess };

    explicit MenuBoxCategoryListView(EDesignerFormEditorInterface *core, QWidget *parent = 0);
    void setViewMode(ViewMode vm);

//    void dropWidgets(const QList<EDesignerDnDItemInterface*> &item_list);

    using QListView:: contentsSize;

    // These methods operate on the filtered/unfiltered model according to accessmode
    int count(AccessMode am) const;
    EMenuBoxInterface::Page pageAt(AccessMode am, const QModelIndex &index) const;
    EMenuBoxInterface::Page pageAt(AccessMode am, int row) const;
    void removeRow(AccessMode am, int row);
    void setCurrentItem(AccessMode am, int row);

    // These methods operate on the unfiltered model and are use for serialization
    void addPage(const EMenuBoxInterface::Page &page, const QIcon &icon, bool editable);
    bool containsPage(const QString &name);
    EMenuBoxInterface::Category category() const;
    bool removeCustomPages();

    // Helper: Ensure a <ui> tag ib the case of empty XML
//    static QString widgetDomXml(const EDesignerWidgetBoxInterface::Widget &widget);

signals:
    void scratchPadChanged();
    void pressed(const QString &name, const QString &id, const QPoint &globalPos);
    void itemRemoved();
    void lastItemRemoved();

public slots:
    void filter(const QRegExp &re);

private slots:
    void slotPressed(const QModelIndex &index);
    void removeCurrentItem();
    void editCurrentItem();

private:
    int mapRowToSource(int filterRow) const;
    QSortFilterProxyModel *m_proxyModel;
    MenuBoxCategoryModel *m_model;
};

#endif // MENUBOXCATEGORYLISTVIEW_H
