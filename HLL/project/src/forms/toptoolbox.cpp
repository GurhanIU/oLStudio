#include "toptoolbox.h"
#include "ui_toptoolbox.h"

#include <QIntValidator>

TopToolBox::TopToolBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TopToolBox)
{
//    setAttribute(Qt::WA_StyledBackground, true);
    ui->setupUi(this);

    ui->edtTimeout->setValidator(new QIntValidator(50, 10000, ui->edtTimeout));
}

TopToolBox::~TopToolBox()
{
    delete ui;
}

void TopToolBox::on_btnStartStop_toggled(bool checked)
{
    ui->btnStartStop->setText( checked ? trUtf8("DUR") : trUtf8("BAŞLA"));
    ui->edtTimeout->setDisabled(checked);

    emit sgStartStopToggled(checked);
}
