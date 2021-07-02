#ifndef TEXTBUILDER_H
#define TEXTBUILDER_H

//#include <QtDesigner/uilib_global.h>
#include <QList>
#include <QString>

class QDir;
class QVariant;

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal {
#endif

class DomProperty;
class DomString;

class ETextBuilder
{
public:
    ETextBuilder();
    virtual ~ETextBuilder();

    virtual QVariant loadText(const DomProperty *property) const;

    virtual QVariant toNativeValue(const QVariant &value) const;

    virtual DomProperty *saveText(const QVariant &value) const;
};


#ifdef QFORMINTERNAL_NAMESPACE
}
#endif

#endif // TEXTBUILDER_H
