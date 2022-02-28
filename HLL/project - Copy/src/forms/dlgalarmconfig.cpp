#include "dlgalarmconfig.h"
#include "ui_dlgalarmconfig.h"

#include "abstractformeditor.h"

#include <QDataWidgetMapper>
#include <QSql>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QSqlError>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>

DlgAlarmConfig::DlgAlarmConfig(EDesignerFormEditorInterface *core, QWidget *parent) :
    m_core(core),
    QDialog(parent),
    ui(new Ui::DlgAlarmConfig)
{
    ui->setupUi(this);
}

DlgAlarmConfig::~DlgAlarmConfig()
{
    delete ui;
}

void DlgAlarmConfig::slTableDoubleClicked(const QModelIndex &index)
{

}

void DlgAlarmConfig::slTableSingleClicked(const QModelIndex &index)
{

}

void DlgAlarmConfig::on_btnNew_clicked()
{

}

void DlgAlarmConfig::on_btnAdd_clicked()
{

}

void DlgAlarmConfig::on_btnSave_clicked()
{

}

void DlgAlarmConfig::on_btnChange_clicked()
{

}

void DlgAlarmConfig::on_btnCancel_clicked()
{

}

void DlgAlarmConfig::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                          "Error initializing database: " + err.text());
}

void DlgAlarmConfig::hideAddedRegister(int index, bool hide)
{ // Register tablosunda ki ID ye gore combobox da gizleme yapar.
    qobject_cast<QListView *>(ui->cmbSelectRegister->view())->setRowHidden(index, hide);
}
