#ifndef ABSTRACTFORMEDITOR_H
#define ABSTRACTFORMEDITOR_H

#include <QObject>
#include <QPointer>
#include <QWidget>

class EMenuBoxInterface;
class EDesignerWidgetFactoryInterface;

class QWidget;

class EDesignerFormEditorInterfacePrivate;

class EDesignerFormEditorInterface : public QObject
{
    Q_OBJECT
public:
    EDesignerFormEditorInterface(QObject *parent = 0);
    virtual ~EDesignerFormEditorInterface();

    QWidget *topLevel() const;
    EMenuBoxInterface *menuBox() const;
    EDesignerWidgetFactoryInterface *widgetFactory() const;

    void setTopLevel(QWidget *topLevel);
    void setMenuBox(EMenuBoxInterface *menuBox);

    QString dbFile() const;
    void setDbFile(const QString &dbFile);

protected:
    void setWidgetFactory(EDesignerWidgetFactoryInterface *widgetFactory);

private:
    QString m_dbFile;
    QPointer<QWidget> m_pad1;
    QPointer<EMenuBoxInterface> m_pad2;
    QPointer<EDesignerWidgetFactoryInterface> m_pad8;
    EDesignerFormEditorInterfacePrivate *d;

private:
    EDesignerFormEditorInterface(const EDesignerFormEditorInterface &other);
    void operator =(const EDesignerFormEditorInterface &other);
};

#endif // ABSTRACTFORMEDITOR_H
