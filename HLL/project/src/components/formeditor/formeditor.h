#ifndef FORMEDITOR_H
#define FORMEDITOR_H

#include "abstractformeditor.h"

class QObject;

//namespace edesigner_internal {

class FormEditor: public EDesignerFormEditorInterface
{
    Q_OBJECT
public:
    FormEditor(QObject *parent = 0);
    virtual ~FormEditor();
public slots:
    void slotQrcFileChangedExternally(const QString &path);
};

//}  // namespace edesigner_internal

#endif // FORMEDITOR_H
