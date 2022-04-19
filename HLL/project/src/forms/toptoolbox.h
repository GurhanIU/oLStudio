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

public slots:
    void slChangeScanRate(int value);

private slots:
    void on_btnStartStop_toggled(bool checked);

    void on_edtScanRate_returnPressed();

private:
    Ui::TopToolBox *ui;

signals:
    void sgStartStopToggled(bool checked);
    void sgScanRateChanged(int scanRate);
};

#endif // TOPTOOLBOX_H
