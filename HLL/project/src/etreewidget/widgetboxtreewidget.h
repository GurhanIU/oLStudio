#ifndef WIDGETBOXTREEWIDGET_H
#define WIDGETBOXTREEWIDGET_H

#include "edesigner_widgetbox_p.h"
//#include "abstractdnditem.h"
//#include "abstractformeditor.h"
//#include "widgetboxcategorylistview.h"

#include <QTreeWidget>
#include <QIcon>
#include <QList>
#include <QHash>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class EDesignerFormEditorInterface;
class EDesignerDnDItemInterface;

class QTimer;

namespace edesigner_internal {

class WidgetBoxCategoryListView;

// WidgetBoxTreeWidget: A tree of categories

class WidgetBoxTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    typedef EDesignerWidgetBoxInterface::Widget Widget;
    typedef EDesignerWidgetBoxInterface::Category Category;
    typedef EDesignerWidgetBoxInterface::CategoryList CategoryList;

    explicit WidgetBoxTreeWidget(EDesignerFormEditorInterface *core, QWidget *parent = 0);
    ~WidgetBoxTreeWidget();

    int categoryCount() const;
    Category category(int cat_idx) const;
    void addCategory(const Category &cat);
    void removeCategory(int cat_idx);

    int widgetCount(int cat_idx) const;
    Widget widget(int cat_idx, int wgt_idx) const;
    void addWidget(int cat_idx, const Widget &wgt);
    void removeWidget(int cat_idx, int wgt_idx);

//    void dropWidgets(const QList<EDesignerDnDItemInterface*> &item_list);

    void setFileName(const QString &file_name);
    QString fileName() const;
    bool load(EDesignerWidgetBox::LoadMode loadMode);
    bool loadContents(const QString &contents);
    bool save();
    QIcon iconForWidget(QString iconName) const;

signals:
    void pressed(const QString name, const QString dom_xml, const QPoint &global_mouse_pos);

public slots:
    void filter(const QString &);

protected:
    void contextMenuEvent(QContextMenuEvent *e);
    void resizeEvent(QResizeEvent *e);

private slots:
    void slotSave();
    void slotScratchPadItemDeleted();
    void slotLastScratchPadItemDeleted();

    void handleMousePress(QTreeWidgetItem *item);
    void deleteScratchpad();
    void slotListMode();
    void slotIconMode();

private:
    WidgetBoxCategoryListView *addCategoryView(QTreeWidgetItem *parent, bool iconMode);
    WidgetBoxCategoryListView *categoryViewAt(int idx) const;
    void adjustSubListSize(QTreeWidgetItem *cat_item);

    static bool readCategories(const QString &fileName, const QString &xml, CategoryList *cats, QString *errorMessage);
    static bool readWidget(Widget *w, const QString &xml, QXmlStreamReader &r);

    CategoryList loadCustomCategoryList() const;
    void writeCategories(QXmlStreamWriter &writer, const CategoryList &cat_list) const;

    int indexOfCategory(const QString &name) const;
    int indexOfScratchpad() const;
    int ensureScratchpad();
    void addCustomCategories(bool replace);

    void saveExpandedState() const;
    void restoreExpandedState();
    void updateViewMode();

    EDesignerFormEditorInterface *m_core;
    QString m_file_name;
    typedef QHash<QString, QIcon> IconCache;
    mutable IconCache m_pluginIcons;
    bool m_iconMode;
    QTimer *m_scratchPadDeleteTimer;
};

} // namespace edesigner_internal

#endif // WIDGETBOXTREEWIDGET_H
