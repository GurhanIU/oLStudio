#ifndef ABSTRACTFORMEDITOR_H
#define ABSTRACTFORMEDITOR_H

#include <QObject>
#include <QPointer>
#include <QWidget>

class QWidget;

class EDesignerWidgetBoxInterface;
class EDesignerWidgetDataBaseInterface;
class EDesignerFormEditorInterfacePrivate;

class EDesignerFormEditorInterface : public QObject
{
    Q_OBJECT
public:
    EDesignerFormEditorInterface(QObject *parent = 0);
    virtual ~EDesignerFormEditorInterface();

    QWidget *topLevel() const;
    void setTopLevel(QWidget *topLevel);

    EDesignerWidgetBoxInterface *widgetBox() const;
    void setWidgetBox(EDesignerWidgetBoxInterface *widgetBox);

    EDesignerWidgetDataBaseInterface *widgetDataBase() const;

protected:
    void setWidgetDataBase(EDesignerWidgetDataBaseInterface *widgetDataBase);

private:
    QPointer<QWidget> m_pad1;
    QPointer<EDesignerWidgetBoxInterface> m_pad2;
    QPointer<EDesignerWidgetDataBaseInterface> m_pad3; // m_pad7;
    EDesignerFormEditorInterfacePrivate *d;

private:
    EDesignerFormEditorInterface(const EDesignerFormEditorInterface &other);
    void operator =(const EDesignerFormEditorInterface &other);
};

#endif // ABSTRACTFORMEDITOR_H
