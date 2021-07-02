#include "about.h"
#include "ui_about.h"
//#include "modbus-version.h"

const QString VER = "V0.0.0b.4";
//const QString LIB_VER = LIBMODBUS_VERSION_STRING;
const QString URL = "<a href = \"http://www.onlab.com.tr/\">Onlab</a>";

About::About(QWidget *parent) :    QDialog(parent),    ui(new Ui::About)
{
    ui->setupUi(this);
//    ui->lblVersion->setText(VER);
    ui->lblLibVersion->setText(VER);
    ui->lblURL->setText(URL);
}

About::~About()
{
    delete ui;
}
