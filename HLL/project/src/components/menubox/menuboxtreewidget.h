#ifndef MENUBOXTREEWIDGET_H
#define MENUBOXTREEWIDGET_H

#include "edesigner_menubox_p.h"
//#include "abstractmenubox.h"

#include <QTreeWidget>

class QSqlDatabase;
class QTimer;

class EDesignerFormEditorInterface;
class MenuBoxCategoryListView;

class MenuBoxTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    typedef EMenuBoxInterface::Page Page;
    typedef EMenuBoxInterface::Category Category;
    typedef EMenuBoxInterface::CategoryList CategoryList;

    explicit MenuBoxTreeWidget(EDesignerFormEditorInterface *core, QWidget *parent = 0);
    ~MenuBoxTreeWidget();

    int categoryCount() const;
    Category category(int cat_idx) const;
    void addCategory(const Category &cat);
    void removeCategory(int cat_idx);

    int pageCount(int cat_idx) const;
    Page page(int cat_idx, int page_idx) const;
    void addPage(int cat_idx, const Page &page);
    void removePage(int cat_idx, int page_idx);

//    void dropWidgets(const QList<EDesignerDnDItemInterface*> &item_list);

    void setFileName(const QString &file_name);
    QString fileName() const;
    bool load(EMenuBox::LoadMode loadMode);
    bool loadContents(const QSqlDatabase &db);
    bool save();
    QIcon iconForPage(QString iconName) const;

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
    MenuBoxCategoryListView *addCategoryView(QTreeWidgetItem *parent, bool iconMode);
    MenuBoxCategoryListView *categoryViewAt(int idx) const;
    void adjustSubListSize(QTreeWidgetItem *cat_item);

    static bool readCategories(const QString &fileName, const QSqlDatabase &db, CategoryList *cats, QString *errorMessage);
//    static bool readPage(Page *w, const QString &xml, QXmlStreamReader &r);

    CategoryList loadCustomCategoryList() const;
//    void writeCategories(QXmlStreamWriter &writer, const CategoryList &cat_list) const;

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

#endif // MENUBOXTREEWIDGET_H
