#ifndef TICKER_H
#define TICKER_H

#include <QWidget>

class Ticker : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText )
public:
    explicit Ticker(QWidget *parent = nullptr, const char *name = 0);

    void setText(const QString &newText);
    QString text() const {return m_text;}
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *event);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

signals:

private:
    QString m_text;
    int m_offset;
    int m_timerId;

};

#endif // TICKER_H
