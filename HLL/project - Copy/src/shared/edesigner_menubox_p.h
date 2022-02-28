#ifndef EMENUBOX_H
#define EMENUBOX_H

#include "abstractmenubox.h"

class QSqlDatabase;

// A Widget box with a load mode that allows for updating custom widgets.

class EMenuBox : public EMenuBoxInterface
{
    Q_OBJECT
public:
    enum LoadMode { LoadMerge, LoadReplace, LoadCustomWidgetsOnly};

    explicit EMenuBox(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    LoadMode loadMode() const;
    void setLoadMode(LoadMode lm);

    virtual bool loadContents(const QSqlDatabase &database) = 0;

    // Convience to access the widget box icon of a widget. Empty category
    // matches all
    virtual QIcon iconForPage(const QString &className,
                                const QString &category = QString()) const = 0;

    // Convience to find a widget by class name. Empty category matches all
    static bool findPage(const EMenuBoxInterface *wbox,
                           const QString &className,
                           const QString &category /* = QString() */,
                           Page *pageData);
    // Convience functions to create a DomWidget from widget box xml.
//    static DomUI *xmlToUi(const QString &name, const QString &xml, bool insertFakeTopLevel, QString *errorMessage);
//    static DomUI *xmlToUi(const QString &name, const QString &xml, bool insertFakeTopLevel);

Q_SIGNALS:
    void pageChanged(const QString &name, const int &id);

private Q_SLOTS:
    void slotPageChanged(const QString &name, const int &id);


protected:
    void emitPageChanged(const QString &name, const int &id);

private:
    LoadMode m_loadMode;
    bool m_pageChangedForwardingBlocked;
};

#endif // EMENUBOX_H
