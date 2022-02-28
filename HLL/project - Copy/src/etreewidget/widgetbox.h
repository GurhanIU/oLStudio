#ifndef WIDGETBOX_H
#define WIDGETBOX_H

#include "edesigner_widgetbox_p.h"
#include "widgetboxtreewidget.h"

class EDesignerFormEditorInterface;
class WidgetBoxTreeWidget;

namespace edesigner_internal {

class WidgetBox : public EDesignerWidgetBox
{
    Q_OBJECT
public:
    explicit WidgetBox(EDesignerFormEditorInterface *core, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~WidgetBox();

    EDesignerFormEditorInterface *core() const;

    virtual int categoryCount() const;
    virtual Category category(int cat_idx) const;
    virtual void addCategory(const Category &cat);
    virtual void removeCategory(int cat_idx);

    virtual int widgetCount(int cat_idx) const;
    virtual Widget widget(int cat_idx, int wgt_idx) const;
    virtual void addWidget(int cat_idx, const Widget &wgt);
    virtual void removeWidget(int cat_idx, int wgt_idx);

//    void dropWidgets(const QList<EDesignerDnDItemInterface *> &item_list, const QPoint &global_mouse_pos);

    virtual void setFileName(const QString &file_name);
    virtual QString fileName() const;
    virtual bool load();
    virtual bool save();

    virtual bool loadContents(const QString &contents);
    virtual QIcon iconForWidget(const QString &className, const QString &category = QString()) const;

protected:
//    virtual void dragEnterEvent (QDragEnterEvent * event);
//    virtual void dragMoveEvent(QDragMoveEvent * event);
//    virtual void dropEvent (QDropEvent * event);

private slots:
    void handleMousePress(const QString &name, const QString &xml, const QPoint &global_mouse_pos);

private:
    EDesignerFormEditorInterface *m_core;
    WidgetBoxTreeWidget *m_view;
};

} // namespace edesigner_internal

#endif // WIDGETBOX_H
