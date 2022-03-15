#ifndef TOPTOOLBOX_H
#define TOPTOOLBOX_H

#include <QWidget>

namespace Ui {
class TopToolBox;
}

class TopToolBox : public QWidget
{
    Q_OBJECT
public:
    explicit TopToolBox(QWidget *parent = nullptr);
    ~TopToolBox();

private slots:
    void on_btnStartStop_toggled(bool checked);

private:
    Ui::TopToolBox *ui;

signals:
    void sgStartStopToggled(bool checked);
};

#endif // TOPTOOLBOX_H
