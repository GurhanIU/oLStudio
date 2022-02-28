#ifndef EDESIGNER_COMPONENTS_H
#define EDESIGNER_COMPONENTS_H

class QObject;
class QWidget;

class EDesignerFormEditorInterface;
class EMenuBoxInterface;
//class EDesignerPropertyEditorInterface;
//class EDesignerObjectInspectorInterface;
//class EDesignerActionEditorInterface;

class EDesignerComponents
{
public:
//    static void initializeResources();
//    static void initializePlugins(EDesignerFormEditorInterface *core);

    static EDesignerFormEditorInterface *createFormEditor(QObject *parent);
    static EMenuBoxInterface *createMenuBox(EDesignerFormEditorInterface *core, QWidget *parent);
//    static EDesignerPropertyEditorInterface *createPropertyEditor(EDesignerFormEditorInterface *core, QWidget *parent);
//    static EDesignerObjectInspectorInterface *createObjectInspector(EDesignerFormEditorInterface *core, QWidget *parent);
//    static EDesignerActionEditorInterface *createActionEditor(EDesignerFormEditorInterface *core, QWidget *parent);

//    static QObject *createTaskMenu(EDesignerFormEditorInterface *core, QObject *parent);
//    static QWidget *createResourceEditor(EDesignerFormEditorInterface *core, QWidget *parent);
//    static QWidget *createSignalSlotEditor(EDesignerFormEditorInterface *core, QWidget *parent);
};

#endif // EDESIGNER_COMPONENTS_H
