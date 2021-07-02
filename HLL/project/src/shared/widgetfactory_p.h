#ifndef WIDGETFACTORY_H
#define WIDGETFACTORY_H

//#include "shared_global_p.h"
//#include "pluginmanager_p.h"

#include "abstractwidgetfactory.h"

#include <QMap>
#include <QHash>
#include <QVariant>
#include <QPointer>

class QObject;
class QWidget;
class QLayout;
//class EDesignerCustomWidgetInterface;
class EDesignerFormEditorInterface;
class EDesignerFormWindowInterface;
class QStyle;

//namespace edesigner_internal {

class WidgetFactory: public EDesignerWidgetFactoryInterface
{
    Q_OBJECT
public:
    explicit WidgetFactory(EDesignerFormEditorInterface *core, QObject *parent = 0);
    ~WidgetFactory();

//    virtual QWidget* containerOfWidget(QWidget *widget) const;
    virtual QWidget* widgetOfContainer(QWidget *widget) const;

    QObject* createObject(const QString &className, QObject* parent) const;

    virtual QWidget *createWidget(const QString &className, QWidget *parentWidget) const;
//    virtual QLayout *createLayout(QWidget *widget, QLayout *layout, int type) const;

    virtual bool isPassiveInteractor(QWidget *widget);
    virtual void initialize(QObject *object) const;
    void initializeCommon(QWidget *object) const;
    void initializePreview(QWidget *object) const;


    virtual EDesignerFormEditorInterface *core() const;

    static QString classNameOf(EDesignerFormEditorInterface *core, const QObject* o);

    EDesignerFormWindowInterface *currentFormWindow(EDesignerFormWindowInterface *fw);

    static QLayout *createUnmanagedLayout(QWidget *parentWidget, int type);

    // The widget factory maintains a cache of styles which it owns.
    QString styleName() const;
    void setStyleName(const QString &styleName);

    /* Return a cached style matching the name or QApplication's style if
     * it is the default. */
    QStyle *getStyle(const QString &styleName);
    // Return the current style used by the factory. This either a cached one
    // or QApplication's style */
    QStyle *style() const;

    // Apply one of the cached styles or QApplication's style to a toplevel widget.
    void applyStyleTopLevel(const QString &styleName, QWidget *w);
    static void applyStyleToTopLevel(QStyle *style, QWidget *widget);

    // Return whether object was created by the factory for the form editor.
    static bool isFormEditorObject(const QObject *o);

    // Boolean dynamic property to set on widgets to prevent custom
    // styles from interfering
    static const char *disableStyleCustomPaintingPropertyC;

public slots:
//    void loadPlugins();

private slots:
    void activeFormWindowChanged(EDesignerFormWindowInterface *formWindow);
    void formWindowAdded(EDesignerFormWindowInterface *formWindow);

private:
    struct Strings { // Reduce string allocations by storing predefined strings
        Strings();
        const QString m_alignment;
        const QString m_bottomMargin;
        const QString m_geometry;
        const QString m_leftMargin;
        const QString m_line;
        const QString m_objectName;
        const QString m_spacerName;
        const QString m_orientation;
        const QString m_q3WidgetStack;
        const QString m_qAction;
        const QString m_qButtonGroup;
        const QString m_qAxWidget;
        const QString m_qDialog;
        const QString m_qDockWidget;
        const QString m_eLayoutWidget;
        const QString m_qMenu;
        const QString m_qMenuBar;
        const QString m_qWidget;
        const QString m_rightMargin;
        const QString m_sizeHint;
        const QString m_spacer;
        const QString m_text;
        const QString m_title;
        const QString m_topMargin;
        const QString m_windowIcon;
        const QString m_windowTitle;
    };

//    QWidget* createCustomWidget(const QString &className, QWidget *parentWidget, bool *creationError) const;
    EDesignerFormWindowInterface *findFormWindow(QWidget *parentWidget) const;
    void setFormWindowStyle(EDesignerFormWindowInterface *formWindow);

    const Strings m_strings;
    EDesignerFormEditorInterface *m_core;
//    typedef QMap<QString, EDesignerCustomWidgetInterface*> CustomWidgetFactoryMap;
//    CustomWidgetFactoryMap m_customFactory;
    EDesignerFormWindowInterface *m_formWindow;

    // Points to the cached style or 0 if the default (qApp) is active
    QStyle *m_currentStyle;
    typedef QHash<QString, QStyle *> StyleCache;
    StyleCache m_styleCache;

    static QPointer<QWidget> *m_lastPassiveInteractor;
    static bool m_lastWasAPassiveInteractor;
};

//} // namespace edesigner_internal

#endif // WIDGETFACTORY_H
