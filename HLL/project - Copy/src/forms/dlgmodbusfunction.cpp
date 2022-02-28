#include "DlgModbusFunction.h"
#include "ui_dlgmodbusfunction.h"

#include "abstractformeditor.h"

#include <QSql>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>

DlgModbusFunction::DlgModbusFunction(EDesignerFormEditorInterface *core, QWidget *parent) :
    m_core(core),
    QDialog(parent),
    ui(new Ui::DlgModbusFunction)
{
    ui->setupUi(this);

    ui->grp_new->hide();
    ui->grp_edit->hide();

    ui->edtAddType->setValidator(new QRegExpValidator(QRegExp("^[A-Za-z0-9_]{1,50}"), this ));
    ui->edtAddFunction->setValidator(new QRegExpValidator(QRegExp("^[0-9]{1,2}"), this ));

//    ui->tableView->setFocusPolicy(Qt::NoFocus);
//    ui->tableView->setMouseTracking(true);
//    ui->tableView->viewport()->setMouseTracking(false);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode( QAbstractItemView::SingleSelection );
//    ui->tableView->setTextElideMode(Qt::ElideRight);
//    ui->tableView->setShowGrid(false);
//    ui->tableView->setAutoFillBackground(true);

    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
//    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->verticalHeader()->setVisible(true);
//    ui->tableView->horizontalHeader()->setDisabled(true);

    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slTableDoubleClicked(QModelIndex)));
    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(slTableSingleClicked(QModelIndex)));

    slUpdateTable();
}

DlgModbusFunction::~DlgModbusFunction()
{
    delete ui;
}

void DlgModbusFunction::slUpdateTable()
{
    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());

    if ( !db.isOpen())
        return;

    if( m_model != nullptr)
        delete m_model;

    m_model = new QSqlQueryModel;
    m_model->setQuery("SELECT * FROM MODBUS_FUNCTION; ", db);

    ui->tableView->setModel(m_model);
    ui->tableView->hideColumn(0);
//    ui->tableView->setColumnWidth(0, 30);
    ui->tableView->setColumnWidth(1, 200);
    ui->tableView->setColumnWidth(2, 50);

    ui->tableView->update();
}

void DlgModbusFunction::slTableSingleClicked(const QModelIndex &index)
{
    if( !index.isValid())
        return;

    if( ui->grp_edit->isHidden())
        return;

    m_selectedRowIdx = index.row();

    ui->edtSaveType->setText(m_model->index(m_selectedRowIdx, 1).data().toString());
    ui->edtSaveFunction->setText(m_model->index(m_selectedRowIdx, 2).data().toString());
}

void DlgModbusFunction::slTableDoubleClicked(const QModelIndex &index)
{
    if( !index.isValid())
        return;

    m_selectedRowIdx = index.row();

    if( !ui->grp_new->isHidden())
        ui->grp_new->hide();

    if( ui->grp_edit->isHidden())
        ui->grp_edit->show();
}

void DlgModbusFunction::on_btn_newRecord_clicked()
{
    if(ui->grp_new->isHidden())
    {
        ui->grp_edit->hide();
        ui->grp_new->show();
    }
}

void DlgModbusFunction::on_btnAdd_clicked()
{
    if( ui->edtAddType->text().trimmed().isEmpty() || ui->edtAddFunction->text().trimmed().isEmpty()  )
        return;

    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());

    if ( !db.isOpen())
        return;

    QSqlQuery qry = QSqlQuery(db);
    qry.prepare("INSERT INTO MODBUS_FUNCTION (NAME, FUNCTION) VALUES(:name, :function);");
    qry.bindValue(":name", ui->edtAddType->text());
    qry.bindValue(":function", ui->edtAddFunction->text());

    if(!qry.exec())
        qDebug() << QString("SQL Last Query:%1 \n\r Last Error: %2").arg(qry.lastQuery()).arg(qry.lastError().text());

    slUpdateTable();
}

void DlgModbusFunction::on_btnSave_clicked()
{
    if( ui->edtSaveType->text().trimmed().isEmpty() || ui->edtSaveFunction->text().trimmed().isEmpty()  )
        return;

    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());

    if ( !db.isOpen())
        return;

    QSqlQuery qry = QSqlQuery(db);
    qry.prepare("UPDATE REGISTER_TYPE SET NAME=:name, FUNCTION=:function WHERE ID=:id;");
    qry.bindValue(":id", m_model->index(m_selectedRowIdx, 0).data().toInt());
    qry.bindValue(":name", ui->edtSaveType->text());
    qry.bindValue(":function", ui->edtSaveFunction->text());

    if(!qry.exec())
        qDebug() << QString("SQL Last Query:%1 \n\r Last Error: %2").arg(qry.lastQuery()).arg(qry.lastError().text());

    slUpdateTable();
}

void DlgModbusFunction::on_btnCancel_clicked()
{
    if( !ui->grp_edit->isHidden())
        ui->grp_edit->hide();

    slUpdateTable();
}

void DlgModbusFunction::on_btnCancel_2_clicked()
{
    if( !ui->grp_new->isHidden())
        ui->grp_new->hide();
}
