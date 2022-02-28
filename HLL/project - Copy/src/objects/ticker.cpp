#include "ticker.h"

#include <QPainter>
#include <QEvent>
#include <QDebug>

Ticker::Ticker(QWidget *parent, const char *name) : QWidget(parent)
{
    m_offset = 0;
    m_timerId = 0;
}

void Ticker::setText(const QString &newText)
{
    m_text = newText;
    update();
    updateGeometry();
}

QSize Ticker::sizeHint() const
{
    return fontMetrics().size(0, text());
}

void Ticker::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    int textWidth = fontMetrics().horizontalAdvance(text());
    if (textWidth < 1)
        return;
    int x = -m_offset;
    while (x < width()) {
        painter.drawText(x, 0, textWidth, height(), Qt::AlignLeft | Qt::AlignVCenter, text());
        x += textWidth;
    }
}

void Ticker::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerId) {
        ++m_offset;
        if (m_offset >= fontMetrics().horizontalAdvance(text()))
            m_offset = 0;
        scroll(-1, 0);
    }
    else {
        QWidget::timerEvent(event);
    }
}

void Ticker::showEvent(QShowEvent*)
{
    m_timerId = startTimer(30);
}

void Ticker::hideEvent(QHideEvent*)
{
    killTimer(m_timerId);
}
