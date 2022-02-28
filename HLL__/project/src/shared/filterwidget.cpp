#include "filterwidget_p.h"
#include "iconloader_p.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QFocusEvent>
#include <QPalette>
#include <QCursor>
#include <QToolButton>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QStylePainter>

#include <QDebug>
#include <QPropertyAnimation>

enum { debugFilter = 0 };

HintLineEdit::HintLineEdit(QWidget *parent) :
    QLineEdit(parent),
    m_defaultFocusPolicy(focusPolicy()),
    m_refuseFocus(false)
{
}

IconButton::IconButton(QWidget *parent) :
    QToolButton(parent)
{
#ifndef QT_NO_CURSOR
    setCursor(Qt::ArrowCursor);
#endif
}

void IconButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    // Note isDown should really use the active state but in most styles
    // this has no proper feedback
    QIcon::Mode state = QIcon::Disabled;
    if (isEnabled())
        state = isDown() ? QIcon::Selected : QIcon::Normal;
    QPixmap iconPixmap = icon().pixmap(QSize(ICONBUTTON_SIZE, ICONBUTTON_SIZE), state, QIcon::Off);
    QRect pixmapRect = QRect(0, 0, iconPixmap.width(), iconPixmap.height());
    pixmapRect.moveCenter(rect().center());
    painter.setOpacity(m_fader);
    painter.drawPixmap(pixmapRect, iconPixmap);
}

void IconButton::animateShow(bool visible)
{
    if (visible)
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "fader");
        animation->setDuration(160);
        animation->setEndValue(1.0);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    else
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "fader");
        animation->setDuration(160);
        animation->setEndValue(0.0);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

bool HintLineEdit::refuseFocus() const
{
    return m_refuseFocus;
}

void HintLineEdit::setRefuseFocus(bool v)
{
    if (v == m_refuseFocus)
        return;
    m_refuseFocus = v;
    setFocusPolicy(m_refuseFocus ? Qt::NoFocus : m_defaultFocusPolicy);
}

void HintLineEdit::mousePressEvent(QMouseEvent *e)
{
    if (debugFilter)
        qDebug() << Q_FUNC_INFO;
    // Explicitly focus on click
    if (m_refuseFocus && !hasFocus())
        setFocus(Qt::OtherFocusReason);
    QLineEdit::mousePressEvent(e);
}

void HintLineEdit::focusInEvent(QFocusEvent *e)
{
    if (debugFilter)
        qDebug() << Q_FUNC_INFO;
    if (m_refuseFocus)
    {
        // Refuse the focus if the mouse it outside. In addition to the mouse
        // press logic, this prevents a re-focussing which occurs once
        // we actually had focus
        const Qt::FocusReason reason = e->reason();
        if (reason == Qt::ActiveWindowFocusReason || reason == Qt::PopupFocusReason)
        {
            const QPoint mousePos = mapToGlobal(QCursor::pos());
            const bool refuse = !geometry().contains(mousePos);
            if (debugFilter)
                qDebug() << Q_FUNC_INFO << refuse;
            if (refuse)
            {
                e->ignore();
                return;
            }
        }
    }

    QLineEdit::focusInEvent(e);
}

//-------------------------- FilterWidget

FilterWidget::FilterWidget(QWidget *parent, LayoutMode lm) :
    QWidget(parent),
    m_editor(new HintLineEdit(this)),
    m_button(new IconButton(m_editor)),
    m_buttonWidth(0)
{
    m_editor->setPlaceholderText(tr("Filter"));

    //Let the style determine minimum height for our widget
    QSize size(ICONBUTTON_SIZE + 6, ICONBUTTON_SIZE + 2);

    // Note KDE does not reserve space for the highlight color
    if (style()->inherits("OxygenStyle"))
        size = size.expandedTo(QSize(24, 0));

    // Make room for clear icon
    QMargins margins = m_editor->textMargins();
    if (layoutDirection() == Qt::LeftToRight)
        margins.setRight(size.width());
    else
        margins.setLeft(size.width());

    m_editor->setTextMargins(margins);

    QHBoxLayout *l = new QHBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);
    if (lm == LayoutAlignRight)
        l->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    l->addWidget(m_editor);

    // KDE has custom icons for this. Notice that icon namings are counter intuitive
    // If these icons are not avaiable we use the freedesktop standart name before
    // falling back to bundled resource
    QIcon icon = QIcon::fromTheme(layoutDirection() == Qt::LeftToRight ?
                    QLatin1String("edit-clear-locationbar-rtl") :
                    QLatin1String("edit-clear-locationbar-ltr"),
                    QIcon::fromTheme("edit-clear", createIconSet(QLatin1String("cleartext.png"))));

    m_button->setIcon(icon);
    m_button->setToolTip(tr("Clear Text"));
    connect(m_button, SIGNAL(clicked()), this, SLOT(reset()));
    connect(m_editor, SIGNAL(textChanged(QString)), this, SLOT(checkButton(QString)));
    connect(m_editor, SIGNAL(textEdited(QString)), this, SIGNAL(filterChanged(QString)));
}

QString FilterWidget::text() const
{
    return m_editor->text();
}

void FilterWidget::checkButton(const QString &text)
{
    if (m_oldText.isEmpty() || text.isEmpty())
        m_button->animateShow(!m_editor->text().isEmpty());
    m_oldText = text;
}

void FilterWidget::reset()
{
    if (debugFilter)
        qDebug() << Q_FUNC_INFO;

    if (!m_editor->text().isEmpty())
    {
        //Editor has lost focus once this is pressed
        m_editor->clear();
        emit filterChanged(QString());
    }
}

void FilterWidget::resizeEvent(QResizeEvent *)
{
    QRect contentRect = m_editor->rect();
    if (layoutDirection() == Qt::LeftToRight)
    {
        const int iconOffset = m_editor->textMargins().right() + 4;
        m_button->setGeometry(contentRect.adjusted(m_editor->width() - iconOffset, 0, 0, 0));
    }
    else
    {
        const int iconOffset = m_editor->textMargins().left() + 4;
        m_button->setGeometry(contentRect.adjusted(0, 0, -m_editor->width() + iconOffset, 0));
    }
}

bool FilterWidget::refuseFocus() const
{
    return m_editor->refuseFocus();
}

void FilterWidget::setRefuseFocus(bool v)
{
    m_editor->setRefuseFocus(v);
}
