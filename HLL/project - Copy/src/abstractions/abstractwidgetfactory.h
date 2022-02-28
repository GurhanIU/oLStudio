#ifndef ABSTRACTWIDGETFACTORY_H
#define ABSTRACTWIDGETFACTORY_H

#include <QObject>
#include <QLayout>

class EDesignerFormEditorInterface;
class QWidget;
class QLayout;

class EDesignerWidgetFactoryInterface: public QObject
{
    Q_OBJECT
public:
    EDesignerWidgetFactoryInterface(QObject *parent = 0);
    virtual ~EDesignerWidgetFactoryInterface();

    virtual EDesignerFormEditorInterface *core() const = 0;

//    virtual QWidget* containerOfWidget(QWidget *w) const = 0;
    virtual QWidget* widgetOfContainer(QWidget *w) const = 0;

    virtual QWidget *createWidget(const QString &name, QWidget *parentWidget = 0) const = 0;
//    virtual QLayout *createLayout(QWidget *widget, QLayout *layout, int type) const = 0;

    virtual bool isPassiveInteractor(QWidget *widget) = 0;
    virtual void initialize(QObject *object) const = 0;
};

#endif // ABSTRACTWIDGETFACTORY_H
