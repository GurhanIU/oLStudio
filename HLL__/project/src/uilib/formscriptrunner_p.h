#ifndef FORMSCRIPTRUNNER_H
#define FORMSCRIPTRUNNER_H

//#include <QtDesigner/uilib_global.h>
#include <QList>
#include <QFlags>
#include <QString>

class QWidget;

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal {
#endif

class DomWidget;

class EFormScriptRunner
{
public:
    EFormScriptRunner();
    ~EFormScriptRunner();

    typedef QList<QWidget*> WidgetList;

    bool run(const DomWidget *domWidget,
             const QString &customWidgetScript,
             QWidget *widget, const WidgetList &children,
             QString *errorMessage);

    struct Error {
        QString objectName;
        QString script;
        QString errorMessage;
    };
    typedef QList<Error> Errors;
    Errors errors() const;
    void clearErrors();

    enum Option {
         NoOptions = 0x0,
         DisableWarnings = 0x1,
         DisableScripts = 02
     };
     Q_DECLARE_FLAGS(Options, Option)

    Options options() const;
    void setOptions(Options options);

private:
    class EFormScriptRunnerPrivate;
    EFormScriptRunnerPrivate *m_impl;

    EFormScriptRunner(const EFormScriptRunner &);
    void operator = (const EFormScriptRunner &);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(EFormScriptRunner::Options)

#ifdef QFORMINTERNAL_NAMESPACE
}
#endif

#endif // FORMSCRIPTRUNNER_H
