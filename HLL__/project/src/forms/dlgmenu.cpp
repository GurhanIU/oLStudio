#include "dlgmenu.h"
#include "ui_dlgmenu.h"

#include "abstractformeditor.h"

#include <QDataWidgetMapper>
#include <QSql>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
//#include <QSqlTableDelegate>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

DlgMenu::DlgMenu(EDesignerFormEditorInterface *core, QWidget *parent) :
    m_core(core),
    QDialog(parent),
    ui(new Ui::DlgMenu)
{
    ui->setupUi(this);

    ui->grp_new->hide();
    ui->grp_edit->hide();

    ui->edtAddName->setValidator(new QRegExpValidator(QRegExp("^[A-Za-z0-9_]{1,50}"), this ));

    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView->verticalHeader()->setVisible(true);

    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slTableDoubleClicked(QModelIndex)));
    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(slTableSingleClicked(QModelIndex)));

    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());

    m_model = new QSqlTableModel(this, db);
    m_model->setTable("MENU");
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->setHeaderData(m_model->fieldIndex("NAME"), Qt::Horizontal, trUtf8("MENU NAMES"));

    // Populate the model
    if (!m_model->select())
    {
        showError(m_model->lastError());
        return;
    }

    ui->tableView->setModel(m_model);
    ui->tableView->setColumnHidden(m_model->fieldIndex("ID"), true);
    ui->tableView->setColumnHidden(m_model->fieldIndex("OPERATION_ID"), true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->resizeColumnsToContents();

    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    mapper->setModel(m_model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
//    mapper->setItemDelegate(new QSqlTableDelegate(this));
    mapper->addMapping(ui->edtEditName, m_model->fieldIndex("NAME"));

    connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));

    ui->tableView->setCurrentIndex(m_model->index(0, 0));
    ui->tableView->update();
}

DlgMenu::~DlgMenu()
{
    delete ui;
}

void DlgMenu::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}

void DlgMenu::slTableSingleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    if (ui->grp_edit->isHidden())
        return;

    m_selectedRowIdx = index.row();

//    ui->edtSaveType->setText(m_model->index(m_selectedRowIdx, 1).data().toString());
}

void DlgMenu::slTableDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    m_selectedRowIdx = index.row();

    if( !ui->grp_new->isHidden())
        ui->grp_new->hide();

    if( ui->grp_edit->isHidden())
        ui->grp_edit->show();

//    m_selectedRowIdx = index.row();
//    ui->edtSaveType->setText(m_model->index(m_selectedRowIdx, 1).data().toString());
}

void DlgMenu::on_btn_newRecord_clicked()
{
    if (ui->grp_new->isHidden())
    {
        ui->grp_edit->hide();
        ui->grp_new->show();
    }
}

void DlgMenu::on_btnAdd_clicked()
{
    if (ui->edtAddName->text().trimmed().isEmpty() )
        return;

    QString name = ui->edtAddName->text().trimmed();

    QSqlField fName("NAME", QVariant::String);
    fName.setValue(QVariant(name));

    QSqlRecord record;
    record.append(fName);

    if (m_model->insertRecord(-1, record) )
    {
        if (!ui->btnSave->isEnabled())
            ui->btnSave->setEnabled(true);

        m_dataChanged = true;
    }
}

void DlgMenu::on_btnSave_clicked()
{
    m_model->database().transaction();
    if (m_model->submitAll())
    {
        m_model->database().commit();
        accept();
    }
    else
    {
        m_model->database().rollback();
        QMessageBox::warning(this, tr("Cached Table"),
                                     tr("The database reported an error: %1")
                                     .arg(m_model->lastError().text()));
    }
}

void DlgMenu::on_btnChange_clicked()
{
    QString name = ui->edtEditName->text().trimmed();

    QSqlField fName("NAME", QVariant::String);
    fName.setValue(QVariant(name));

    QSqlRecord record;
    record.append(fName);

    if(m_model->setRecord(m_selectedRowIdx, record))
    {
        if( !ui->btnSave->isEnabled())
            ui->btnSave->setEnabled(true);

        m_dataChanged = true;
    }
}

void DlgMenu::on_btnCancelEdit_clicked()
{
    if( !ui->grp_edit->isHidden())
        ui->grp_edit->hide();
}

void DlgMenu::on_btnCancelNew_clicked()
{
    if( !ui->grp_new->isHidden())
        ui->grp_new->hide();
}

void DlgMenu::on_DlgMenu_finished(int result)
{
    if( m_dataChanged ) {
        if( result == QDialog::Rejected ) {
            int ret = QMessageBox::warning(this, tr("The document has been modified."),
                                         tr("Do you want to save your changes?"), QMessageBox::Save | QMessageBox::Discard /*| QMessageBox::Cancel*/ );

            if( ret == QMessageBox::Save )
                on_btnSave_clicked();
        }
    }
}
