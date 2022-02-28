#include "about.h"
#include "ui_about.h"
#include "version.h"
//#include "modbus-version.h"

About::About(QWidget *parent) :    QDialog(parent),    ui(new Ui::About)
{
    ui->setupUi(this);
    ui->lblVersion->setText(QString("%1").arg(VER_PRODUCTNAME_STR));
    ui->lblLibVersion->setText(QString("V%1").arg(VER_PRODUCTVERSION_STR));
    ui->lblURL->setText(QString("<a href ='%1'>%2</a>").arg(VER_COMPANYDOMAIN_STR).arg(VER_COMPANYNAME_STR));
}

About::~About()
{
    delete ui;
}
