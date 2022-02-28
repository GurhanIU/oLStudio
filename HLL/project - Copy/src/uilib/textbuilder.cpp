#include "textbuilder_p.h"
#include "ui4_p.h"
#include <QVariant>

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal {
#endif

ETextBuilder::ETextBuilder()
{

}

ETextBuilder::~ETextBuilder()
{

}

QVariant ETextBuilder::loadText(const DomProperty *property) const
{
    if (property->kind() == DomProperty::String)
        return property->elementString()->text();
    return QVariant();
}

QVariant ETextBuilder::toNativeValue(const QVariant &value) const
{
    return value;
}

DomProperty *ETextBuilder::saveText(const QVariant &value) const
{
    Q_UNUSED(value)
    return 0;
}

#ifdef QFORMINTERNAL_NAMESPACE
} // namespace QFormInternal
#endif
