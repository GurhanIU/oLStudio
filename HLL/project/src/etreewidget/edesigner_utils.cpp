#include "edesigner_utils_p.h"

namespace edesigner_internal
{
    void designerWarning(const QString &message)
    {
        qWarning("Designer: %s", qPrintable(message));
    }


//edesigner_utils::edesigner_utils()
//{

//}

} // namespace edesigner_internal
