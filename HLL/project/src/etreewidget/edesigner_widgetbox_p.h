#ifndef EDESIGNERWIDGETBOX_H
#define EDESIGNERWIDGETBOX_H

#include "abstractwidgetbox.h"

namespace edesigner_internal {

// A Widget box with a load mode that allows for updating custom widgets.

class EDesignerWidgetBox : public EDesignerWidgetBoxInterface
{
    Q_OBJECT
public:
    enum LoadMode { LoadMerge, LoadReplace, LoadCustomWidgetsOnly};

    explicit EDesignerWidgetBox(QWidget *parent = 0, Qt::WindowFlags flags = 0 /*Qt::Widget*/);

    LoadMode loadMode() const;
    void setLoadMode(LoadMode lm);

    virtual bool loadContents(const QString &contents) = 0;

    // Convience to access the widget box icon of a widget. Empty category
    // matches all
    virtual QIcon iconForWidget(const QString &className,
                                const QString &category = QString()) const = 0;

    // Convience to find a widget by class name. Empty category matches all
    static bool findWidget(const EDesignerWidgetBoxInterface *wbox,
                           const QString &className,
                           const QString &category /* = QString() */,
                           Widget *widgetData);
    // Convience functions to create a DomWidget from widget box xml.
//    static DomUI *xmlToUi(const QString &name, const QString &xml, bool insertFakeTopLevel, QString *errorMessage);
//    static DomUI *xmlToUi(const QString &name, const QString &xml, bool insertFakeTopLevel);

private:
    LoadMode m_loadMode;
};

} // namespace edesigner_internal

#endif // EDESIGNERWIDGETBOX_H
