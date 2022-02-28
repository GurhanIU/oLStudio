#ifndef ICONLOADER_H
#define ICONLOADER_H


class QString;
class QIcon;

namespace edesigner_internal {

QIcon createIconSet(const QString &name);
QIcon emptyIcon();

} // namespace edesigner_internal

#endif // ICONLOADER_H
