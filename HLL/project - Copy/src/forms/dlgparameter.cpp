#include "dlgparameter.h"
#include "ui_DlgParameter.h"

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

DlgParameter::DlgParameter(EDesignerFormEditorInterface *core, QWidget *parent) :
    m_core(core),
    QDialog(parent),
    ui(new Ui::DlgParameter)
{
    ui->setupUi(this);

    ui->grp_new->hide();
    ui->grp_edit->hide();
    ui->btnSave->setDisabled(true);
    ui->btnCancel->setDisabled(true);

    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->tableView->horizontalHeader()->setSectionResizeMode( QHeaderView::Interactive );
    ui->tableView->verticalHeader()->setVisible(true);

    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slTableDoubleClicked(QModelIndex)));
    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(slTableSingleClicked(QModelIndex)));

    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());

    m_model = new QSqlRelationalTableModel(ui->tableView, db);
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->setTable("PARAMETER");

    registerIdx = m_model->fieldIndex("REGISTER_ID");

    m_model->setRelation(registerIdx, QSqlRelation("REGISTER", "ID", "NAME"));

    // Set the localized header captions
    m_model->setHeaderData(registerIdx, Qt::Horizontal, trUtf8("İSİM"));
    m_model->setHeaderData(m_model->fieldIndex("FACTOR"), Qt::Horizontal, tr("ÇARPAN"));
    m_model->setHeaderData(m_model->fieldIndex("DEFAULT_VALUE"), Qt::Horizontal, tr("VARSAYILAN"));
    m_model->setHeaderData(m_model->fieldIndex("MINIMUM_VALUE"), Qt::Horizontal, tr("MİMİMUM"));
    m_model->setHeaderData(m_model->fieldIndex("MAXIMUM_VALUE"), Qt::Horizontal, tr("MAKSİMUM"));

    // Populate the model
    if (!m_model->select()) {
        showError(m_model->lastError());
        return;
    }

    ui->tableView->setModel(m_model);
    ui->tableView->setColumnHidden(m_model->fieldIndex("ID"), true);
    ui->tableView->setColumnHidden(m_model->fieldIndex("ACTUAL_VALUE"), true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->resizeColumnsToContents();

    QSqlQueryModel *mdlCmbSelect = new QSqlQueryModel(this);
    mdlCmbSelect->setQuery("SELECT ID, NAME FROM REGISTER WHERE MODBUS_FUNC_ID IN (2,3,5,6) AND ID NOT IN (SELECT REGISTER_ID FROM PARAMETER);", db);
    while (mdlCmbSelect->canFetchMore())
        mdlCmbSelect->fetchMore();

    int nameIdx = mdlCmbSelect->record().indexOf("NAME");
    ui->cmbSelectRegister->setModel(mdlCmbSelect);
    ui->cmbSelectRegister->setModelColumn(nameIdx);

    QSqlQueryModel *mdlComboEdit = new QSqlQueryModel(this);
    mdlComboEdit->setQuery("SELECT ID, NAME FROM REGISTER WHERE ID IN (SELECT REGISTER_ID FROM PARAMETER);", db);
    while (mdlComboEdit->canFetchMore())
        mdlComboEdit->fetchMore();

    nameIdx = mdlComboEdit->record().indexOf("NAME");
    ui->cmbChangeRegister->setModel(mdlComboEdit);
    ui->cmbChangeRegister->setModelColumn(nameIdx);

    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    mapper->setModel(m_model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(ui->cmbChangeRegister, registerIdx);
    mapper->addMapping(ui->edtChangeFactor, m_model->fieldIndex("FACTOR"));
    mapper->addMapping(ui->edtChangeDefaultValue, m_model->fieldIndex("DEFAULT_VALUE"));
    mapper->addMapping(ui->edtChangeMinValue, m_model->fieldIndex("MINIMUM_VALUE"));
    mapper->addMapping(ui->edtChangeMaxValue, m_model->fieldIndex("MAXIMUM_VALUE"));

    connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));

    connect(ui->tableView->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(slRowInserted(QModelIndex,int,int)));

    ui->tableView->setCurrentIndex(m_model->index(0, 0));
    ui->tableView->update();
}

DlgParameter::~DlgParameter()
{
    delete ui;
}

void DlgParameter::slRowInserted(const QModelIndex & parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(end);

    while (m_model->canFetchMore())
        m_model->fetchMore();

    ui->tableView->scrollTo(ui->tableView->model()->index(start, 0));
    ui->tableView->scrollToBottom();
}

void DlgParameter::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}

void DlgParameter::slTableSingleClicked(const QModelIndex &index)
{
    if( !index.isValid())
        return;

    if( ui->grp_edit->isHidden())
        return;

    m_selectedRowIdx = index.row();
}

void DlgParameter::slTableDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    m_selectedRowIdx = index.row();
    qDebug() << "m_selectedRowIdx" << m_selectedRowIdx;

    if (!ui->grp_new->isHidden())
        ui->grp_new->hide();

    if (ui->grp_edit->isHidden())
        ui->grp_edit->show();   

    if (!ui->btnCancel->isEnabled())
        ui->btnCancel->setEnabled(true);
}

void DlgParameter::on_btnNew_clicked()
{
    if (ui->grp_new->isHidden()) {
        ui->grp_edit->hide();
        ui->grp_new->show();
    }

    if (!ui->btnCancel->isEnabled())
        ui->btnCancel->setEnabled(true);

    while (m_model->canFetchMore())
        m_model->fetchMore();

    QTimer::singleShot(10, ui->tableView, SLOT(scrollToBottom()));
}

void DlgParameter::on_btnAdd_clicked()
{
//    int regA  = m_model->relationModel(registerIdx)->index(ui->cmbSelectRegister->currentIndex(),   m_model->relationModel(registerIdx)->fieldIndex("ID")).data().toInt();
//  int regId = ui->cmbSelectRegister->model()->data(ui->cmbSelectRegister->model()->index(ui->cmbSelectRegister->currentIndex(), registerTypeIdx)).toInt();
    int reg = ui->cmbSelectRegister->model()->index(ui->cmbSelectRegister->currentIndex(), 0).data().toInt();
//    int reg = m_model->relationModel(registerIdx)->index(ui->cmbSelectRegister->currentIndex(), m_model->relationModel(registerIdx)->fieldIndex("ID")).data().toInt();
    QString factor = ui->edtAddFactor->text().trimmed();
    QString defValue = ui->edtAddDefaultValue->text().trimmed();
    QString minValue = ui->edtAddMinValue->text().trimmed();
    QString maxValue = ui->edtAddMaxValue->text().trimmed();

    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());

    QSqlQuery qry = QSqlQuery(db);
    qry.prepare("INSERT INTO PARAMETER (REGISTER_ID, FACTOR, DEFAULT_VALUE, MINIMUM_VALUE, MAXIMUM_VALUE) VALUES(:registerId, :factor, :defValue, :minValue, :maxValue);");
    qry.bindValue(":registerId", reg);
    qry.bindValue(":factor", factor);
    qry.bindValue(":defValue", defValue);
    qry.bindValue(":minValue", minValue);
    qry.bindValue(":maxValue", maxValue);

    m_model->database().transaction();
    if (qry.exec()) {
        if (!ui->btnSave->isEnabled())
            ui->btnSave->setEnabled(true);

        if (!ui->btnCancel->isEnabled())
            ui->btnCancel->setEnabled(true);

        QTimer::singleShot(10, ui->tableView, SLOT(scrollToBottom()));

        int curIdx = ui->cmbSelectRegister->currentIndex();
        hideAddedRegister(curIdx, true);

        if (curIdx >= ui->cmbSelectRegister->count()-1)
            ui->cmbSelectRegister->setCurrentIndex(0);
        else
            ui->cmbSelectRegister->setCurrentIndex(curIdx+1);

        m_model->select();

        while (m_model->canFetchMore())
            m_model->fetchMore();

        m_dataChanged = true;
    }
    else
        qDebug() << QString("SQL Last Query:%1 \n\r Last Error: %2").arg(qry.lastQuery()).arg(qry.lastError().text());

    return;
}

void DlgParameter::hideAddedRegister(int index, bool hide)
{ // Register tablosunda ki ID ye gore combobox da gizleme yapar.
    qobject_cast<QListView *>(ui->cmbSelectRegister->view())->setRowHidden(index, hide);
}

void DlgParameter::on_btnSave_clicked()
{
//    m_model->database().transaction();
//    if (m_model->submitAll()) {
    if (m_model->database().commit()) {
//        accept();
        ui->btnSave->setEnabled(false);
        ui->btnCancel->setEnabled(false);

        if (!ui->grp_edit->isHidden())
            ui->grp_edit->hide();

        if (!ui->grp_new->isHidden())
            ui->grp_new->hide();

        m_dataChanged = false;
    }
    else {
        m_model->database().rollback();
        QMessageBox::warning(this, tr("Cached Table"),
                                     tr("The database reported an error: %1")
                                     .arg(m_model->lastError().text()));

        m_dataChanged = true;

        qDebug() << m_model->lastError() << "\n" <<m_model->lastError().text();
    }

    m_model->select();

    while (m_model->canFetchMore())
        m_model->fetchMore();
}

void DlgParameter::on_btnChange_clicked()
{
    int reg = ui->cmbChangeRegister->model()->index(ui->cmbChangeRegister->currentIndex(), 0).data().toInt();
    qDebug() << reg << ui->cmbChangeRegister->currentIndex() << m_selectedRowIdx;
    reg = ui->cmbChangeRegister->model()->index(m_selectedRowIdx, 0).data().toInt();
    qDebug() << reg << ui->cmbChangeRegister->currentIndex() << m_selectedRowIdx;

    QString factor = ui->edtChangeFactor->text().trimmed();
    QString defValue = ui->edtChangeDefaultValue->text().trimmed();
    QString minValue = ui->edtChangeMinValue->text().trimmed();
    QString maxValue = ui->edtChangeMaxValue->text().trimmed();

    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());

    QSqlQuery qry = QSqlQuery(db);
    qry.prepare("UPDATE PARAMETER SET FACTOR=:factor, DEFAULT_VALUE=:defValue, MINIMUM_VALUE=:minValue, MAXIMUM_VALUE=:maxValue WHERE REGISTER_ID=:registerId;");
    qry.bindValue(":registerId", reg);
    qry.bindValue(":factor", factor);
    qry.bindValue(":defValue", defValue);
    qry.bindValue(":minValue", minValue);
    qry.bindValue(":maxValue", maxValue);

    m_model->database().transaction();
    if (qry.exec()) {
        if (!ui->btnSave->isEnabled())
            ui->btnSave->setEnabled(true);

        if (!ui->btnCancel->isEnabled())
            ui->btnCancel->setEnabled(true);

        m_model->select();

        while (m_model->canFetchMore())
            m_model->fetchMore();

        ui->tableView->selectRow(m_selectedRowIdx);

        m_dataChanged = true;
    }
}

void DlgParameter::on_btnCancel_clicked()
{
    if (m_dataChanged) {
        m_model->revertAll();
        m_model->database().rollback();
//        m_model->revertRow(m_selectedRowIdx);
    }

    m_model->select();

    while (m_model->canFetchMore())
        m_model->fetchMore();

    if (!ui->grp_edit->isHidden())
        ui->grp_edit->hide();

    if (!ui->grp_new->isHidden())
        ui->grp_new->hide();

    ui->btnSave->setEnabled(false);
    ui->btnCancel->setEnabled(false);

    m_dataChanged = false;
}

void DlgParameter::on_DlgParameter_finished(int result)
{
    if (m_dataChanged) {
        if (result == QDialog::Rejected) {
            int ret = QMessageBox::warning(this, tr("The document has been modified."),
                                         tr("Do you want to save your changes?"), QMessageBox::Save | QMessageBox::Discard /*| QMessageBox::Cancel*/ );

            if (ret == QMessageBox::Save)
                m_model->database().commit(); //on_btnSave_clicked();
            else
                m_model->database().rollback();
        }
    }
}
