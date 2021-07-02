#ifndef WIDGETBOX_H
#define WIDGETBOX_H

#include "edesigner_menubox_p.h"

class EDesignerFormEditorInterface;
class EDesignerFormWindowInterface;

class MenuBoxTreeWidget;

class QSqlDatabase;

class MenuBox : public EMenuBox
{
    Q_OBJECT
public:
    explicit MenuBox(EDesignerFormEditorInterface *core, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~MenuBox();

    EDesignerFormEditorInterface *core() const;

    virtual int categoryCount() const;
    virtual Category category(int cat_idx) const;
    virtual void addCategory(const Category &cat);
    virtual void removeCategory(int cat_idx);

    virtual int pageCount(int cat_idx) const;
    virtual Page page(int cat_idx, int page_idx) const;
    virtual void addPage(int cat_idx, const Page &page);
    virtual void removePage(int cat_idx, int wgt_idx);

//    void dropWidgets(const QList<EDesignerDnDItemInterface *> &item_list, const QPoint &global_mouse_pos);

    virtual void setFileName(const QString &file_name);
    virtual QString fileName() const;
    virtual bool load();
    virtual bool save();

    virtual bool loadContents(const QSqlDatabase &database);
    virtual QIcon iconForPage(const QString &className, const QString &category = QString()) const;

protected:
//    virtual void dragEnterEvent (QDragEnterEvent * event);
//    virtual void dragMoveEvent(QDragMoveEvent * event);
//    virtual void dropEvent (QDropEvent * event);

private slots:
    void handleMousePress(const QString &name, const QString &id, const QPoint &global_mouse_pos);

private:
    EDesignerFormEditorInterface *m_core;
    MenuBoxTreeWidget *m_view;
};

#endif // WIDGETBOX_H
