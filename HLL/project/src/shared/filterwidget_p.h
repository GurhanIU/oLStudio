#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QColor>
#include <QToolButton>

class ToolButton;

/* This widget should never have initial focus
 * (ie, be the first widget of a dialog, else, the hint cannot be displayed.
 * For situations, where it is the only focusable control (widget box),
 * there is a special "refuseFocus()" mode, in which it clears the focus
 * policy and focusses explicitly on click (note that settings Qt::ClickFocus
 * is not sufficient for that as an ActivationsFocus will occur). */

#define ICONBUTTON_SIZE 16

class HintLineEdit  : public QLineEdit
{
    Q_OBJECT
public:
    explicit HintLineEdit(QWidget *parent = 0);

    bool refuseFocus() const;
    void setRefuseFocus(bool v);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void focusInEvent(QFocusEvent *event);

private:
    const Qt::FocusPolicy m_defaultFocusPolicy;
    bool m_refuseFocus;
};

// IconButton: This is a simple helper class that represents clickable icon

class IconButton: public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(float fader READ fader WRITE setFader)
public:
    IconButton(QWidget *parent);
    void paintEvent(QPaintEvent *event);
    float fader() { return m_fader; }
    void setFader(float value) { m_fader = value; }
    void animateShow(bool visible);

private:
    float m_fader;
};

// FilterWidget: For filtering item views, with reset button Uses HintLineEdit.

class FilterWidget : public QWidget
{
    Q_OBJECT
public:
    enum LayoutMode {
        // For use in toolbars: Expand to the right
        LayoutAlignRight,
        // No special alignment
        LayoutAlignNone
    };

    explicit FilterWidget(QWidget *parent = 0, LayoutMode lm = LayoutAlignRight);

    QString text() const;
    void resizeEvent(QResizeEvent *);
    bool refuseFocus() const; // see HintLineEdit
    void setRefuseFocus(bool v);

signals:
    void filterChanged(const QString &);

public slots:
    void reset();

private slots:
    void checkButton(const QString &text);

private:
    HintLineEdit *m_editor;
    IconButton *m_button;
    int m_buttonWidth;
    QString m_oldText;
};

#endif // FILTERWIDGET_H
