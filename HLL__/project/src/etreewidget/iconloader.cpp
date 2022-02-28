#include "iconloader_p.h"

#include <QtCore/QFile>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>

namespace edesigner_internal {

QIcon createIconSet(const QString &name)
{
    QStringList candidates = QStringList()
        << (QString::fromUtf8(":/trolltech/formeditor/images/") + name)
#ifdef Q_WS_MAC
        << (QString::fromUtf8(":/trolltech/formeditor/images/mac/") + name)
#else
        << (QString::fromUtf8(":/trolltech/formeditor/images/win/") + name)
#endif
        << (QString::fromUtf8(":/trolltech/formeditor/images/designer_") + name);

    foreach (const QString &f, candidates)
    {
        if (QFile::exists(f))
            return QIcon(f);
    }

    return QIcon();
}

QIcon emptyIcon()
{
    return QIcon(QLatin1String(":/trolltech/fromeditor/images/emptyicon.png"));
}

} // namespace edesigner_internal
