#include "toptoolbox.h"
#include "ui_toptoolbox.h"

#include <QIntValidator>

TopToolBox::TopToolBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TopToolBox)
{
//    setAttribute(Qt::WA_StyledBackground, true);
    ui->setupUi(this);

    ui->edtScanRate->setValidator(new QIntValidator(50, 10000, ui->edtScanRate));
}

TopToolBox::~TopToolBox()
{
    delete ui;
}

void TopToolBox::slChangeScanRate(int value)
{
    ui->edtScanRate->setText(QString::number(value));
}

void TopToolBox::on_btnStartStop_toggled(bool checked)
{
    ui->btnStartStop->setText( checked ? trUtf8("DUR") : trUtf8("BAŞLA"));
    ui->edtScanRate->setDisabled(checked);

    emit sgStartStopToggled(checked);
}

void TopToolBox::on_edtScanRate_returnPressed()
{
    int scanRate = ui->edtScanRate->text().toInt();
    emit sgScanRateChanged(scanRate);
}

