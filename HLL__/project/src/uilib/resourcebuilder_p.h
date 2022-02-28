#ifndef RESOURCEBUILDER_H
#define RESOURCEBUILDER_H

//#include <QtDesigner/uilib_global.h>
#include <QList>
#include <QString>

class QDir;
class QVariant;

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal {
#endif

class DomProperty;
class DomResourceIcon;

class EResourceBuilder
{
public:
    enum IconStateFlags {
        NormalOff = 0x1, NormalOn = 0x2, DisabledOff = 0x4, DisabledOn = 0x8,
        ActiveOff = 0x10, ActiveOn = 0x20, SelectedOff = 0x40, SelectedOn = 0x80
    };

    EResourceBuilder();
    virtual ~EResourceBuilder();

    virtual QVariant loadResource(const QDir &workingDirectory, const DomProperty *property) const;

    virtual QVariant toNativeValue(const QVariant &value) const;

    virtual DomProperty *saveResource(const QDir &workingDirectory, const QVariant &value) const;

    virtual bool isResourceProperty(const DomProperty *p) const;

    virtual bool isResourceType(const QVariant &value) const;

    static int iconStateFlags(const DomResourceIcon *resIcon);
};


#ifdef QFORMINTERNAL_NAMESPACE
}
#endif

#endif // RESOURCEBUILDER_H
