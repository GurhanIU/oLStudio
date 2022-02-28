#include "dlgregister.h"
#include "ui_dlgregister.h"

#include "abstractformeditor.h"

#include <QDataWidgetMapper>
#include <QSql>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QSqlError>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>

DlgRegister::DlgRegister(EDesignerFormEditorInterface *core,QWidget *parent) :
    m_core(core),
    QDialog(parent),
    ui(new Ui::DlgRegister)
{
    ui->setupUi(this);

    ui->grp_new->hide();
    ui->grp_edit->hide();
    ui->btnSave->setDisabled(true);
    ui->btnCancel->setDisabled(true);

//    ui->edtAddName->setValidator(new QRegExpValidator( QRegExp("[0-9]{1,20}"), this ));
//    ui->edtAddName->setValidator(new QRegExpValidator(QRegExp("^[A-Za-z0-9]{1,50}"), this ));
//    ui->edtAddName->setValidator(new QRegExpValidator(QRegExp("^[A-Za-z0-9_]{1,50}"), this ));
//    ui->edtEditName->setValidator(new QRegExpValidator(QRegExp("^[A-Za-z0-9_]{1,50}"), this ));

    ui->edtAddAddress->setValidator( new QIntValidator(0, 10000, this) );
    ui->edtEditAddress->setValidator( new QIntValidator(0, 10000, this) );

    ui->edtAddPrecision->setValidator(new QIntValidator(0, 10, this));
    ui->edtEditPrecision->setValidator(new QIntValidator(0, 10, this));

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

//    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode( QHeaderView::Interactive );
//    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->verticalHeader()->setVisible(true);
//    ui->tableView->horizontalHeader()->setDisabled(true);

    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slTableDoubleClicked(QModelIndex)));
    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(slTableSingleClicked(QModelIndex)));

    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());

    m_model = new QSqlRelationalTableModel(ui->tableView, db);
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->setTable("REGISTER");

    deviceIdx       = m_model->fieldIndex("DEVICE_ID");
    modbusFuncIdx   = m_model->fieldIndex("MODBUS_FUNC_ID");
    addressIdx      = m_model->fieldIndex("ADDRESS");
    nameIdx         = m_model->fieldIndex("NAME");
    variantTypeIdx  = m_model->fieldIndex("VARIANT_ID");
    precisionIdx    = m_model->fieldIndex("PRECISION");
    unitIdx         = m_model->fieldIndex("UNIT_ID");
    indicatorObjIdx = m_model->fieldIndex("CONTROL_OBJ_ID");

    m_model->setRelation(deviceIdx, QSqlRelation("DEVICE", "ID", "NAME"));
    m_model->setRelation(modbusFuncIdx, QSqlRelation("MODBUS_FUNCTION", "ID", "NAME"));
    m_model->setRelation(variantTypeIdx, QSqlRelation("VARIANT_TYPE", "ID", "DESCRIPTION"));
    m_model->setRelation(unitIdx, QSqlRelation("UNITS", "ID", "UNIT"));
    m_model->setRelation(indicatorObjIdx, QSqlRelation("CONTROL_OBJECT", "ID", "NAME"));

    // Set the localized header captions
    m_model->setHeaderData(deviceIdx, Qt::Horizontal, trUtf8("CİHAZ"));
    m_model->setHeaderData(modbusFuncIdx, Qt::Horizontal, trUtf8("MODBUS FONK"));
    m_model->setHeaderData(addressIdx, Qt::Horizontal, trUtf8("ADRES"));
    m_model->setHeaderData(nameIdx, Qt::Horizontal, trUtf8("İSİM"));
    m_model->setHeaderData(variantTypeIdx, Qt::Horizontal, trUtf8("TÜR"));
    m_model->setHeaderData(precisionIdx, Qt::Horizontal, trUtf8("HASSASİYET"));
    m_model->setHeaderData(unitIdx, Qt::Horizontal, trUtf8("BİRİM"));
    m_model->setHeaderData(indicatorObjIdx, Qt::Horizontal, trUtf8("GÖSTERGEÇ"));

    // Populate the model
    if (!m_model->select()) {
        showError(m_model->lastError());
        return;
    }

    ui->tableView->setModel(m_model);
    ui->tableView->setColumnHidden(m_model->fieldIndex("ID"), true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->resizeColumnsToContents();

    // -- Add
    ui->cmbAddDevice->setModel(m_model->relationModel(deviceIdx));
    ui->cmbAddDevice->setModelColumn(m_model->relationModel(deviceIdx)->fieldIndex("NAME"));

    ui->cmbAddModbusFunc->setModel(m_model->relationModel(modbusFuncIdx));
    ui->cmbAddModbusFunc->setModelColumn(m_model->relationModel(modbusFuncIdx)->fieldIndex("NAME"));

    QSqlTableModel *variantModel = m_model->relationModel(variantTypeIdx);
    variantModel->setFilter(QString("AVAILABLE == '1'"));
    ui->cmbAddVariantType->setModel(variantModel);
    ui->cmbAddVariantType->setModelColumn(m_model->relationModel(variantTypeIdx)->fieldIndex("DESCRIPTION"));

    ui->cmbAddUnit->setModel(m_model->relationModel(unitIdx));
    ui->cmbAddUnit->setModelColumn(m_model->relationModel(unitIdx)->fieldIndex("UNIT"));

    ui->cmbAddIndicator->setModel(m_model->relationModel(indicatorObjIdx));
    ui->cmbAddIndicator->setModelColumn(m_model->relationModel(indicatorObjIdx)->fieldIndex("NAME"));

    // -- Edit
    ui->cmbEditDevice->setModel(m_model->relationModel(deviceIdx));
    ui->cmbEditDevice->setModelColumn(m_model->relationModel(deviceIdx)->fieldIndex("NAME"));

    ui->cmbEditModbusFunc->setModel(m_model->relationModel(modbusFuncIdx));
    ui->cmbEditModbusFunc->setModelColumn(m_model->relationModel(modbusFuncIdx)->fieldIndex("NAME"));

    ui->cmbEditVariantType->setModel(m_model->relationModel(variantTypeIdx));
    ui->cmbEditVariantType->setModelColumn(m_model->relationModel(variantTypeIdx)->fieldIndex("DESCRIPTION"));

    ui->cmbEditUnit->setModel(m_model->relationModel(unitIdx));
    ui->cmbEditUnit->setModelColumn(m_model->relationModel(unitIdx)->fieldIndex("UNIT"));

    ui->cmbEditIndicator->setModel(m_model->relationModel(indicatorObjIdx));
    ui->cmbEditIndicator->setModelColumn(m_model->relationModel(indicatorObjIdx)->fieldIndex("NAME"));

    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    mapper->setModel(m_model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));

    mapper->addMapping(ui->cmbEditDevice, deviceIdx);
    mapper->addMapping(ui->cmbEditModbusFunc, modbusFuncIdx);
    mapper->addMapping(ui->edtEditAddress, m_model->fieldIndex("ADDRESS"));
    mapper->addMapping(ui->edtEditName, m_model->fieldIndex("NAME"));
    mapper->addMapping(ui->cmbEditVariantType, variantTypeIdx);
    mapper->addMapping(ui->edtEditPrecision, m_model->fieldIndex("PRECISION"));
    mapper->addMapping(ui->cmbEditUnit, unitIdx);
    mapper->addMapping(ui->cmbEditIndicator, indicatorObjIdx);

    connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));

    connect(ui->tableView->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(slRowInserted(QModelIndex,int,int)));

    ui->tableView->setCurrentIndex(m_model->index(0, 0));
    ui->tableView->update();
}

DlgRegister::~DlgRegister()
{
    delete ui;
}

void DlgRegister::slRowInserted(const QModelIndex & parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(end);
    ui->tableView->scrollTo(ui->tableView->model()->index(start, 0));
    ui->tableView->scrollToBottom();
}

void DlgRegister::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}

void DlgRegister::slTableSingleClicked(const QModelIndex &index)
{
    if( !index.isValid())
        return;

    if( ui->grp_edit->isHidden())
        return;

    m_selectedRowIdx = index.row();
}

void DlgRegister::slTableDoubleClicked(const QModelIndex &index)
{
    if( !index.isValid())
        return;

    m_selectedRowIdx = index.row();

    if( !ui->grp_new->isHidden())
        ui->grp_new->hide();

    if( ui->grp_edit->isHidden())
        ui->grp_edit->show();

    if (!ui->btnCancel->isEnabled())
        ui->btnCancel->setEnabled(true);
}

void DlgRegister::on_btn_newRecord_clicked()
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

void DlgRegister::on_btnAdd_clicked()
{
    int device = m_model->relationModel(deviceIdx)->index(ui->cmbAddDevice->currentIndex(), m_model->relationModel(deviceIdx)->fieldIndex("ID")).data().toInt();
    int modbusFunc = m_model->relationModel(modbusFuncIdx)->index(ui->cmbAddModbusFunc->currentIndex(), m_model->relationModel(modbusFuncIdx)->fieldIndex("ID")).data().toInt();
    int address = ui->edtAddAddress->text().trimmed().toInt();
    QString name = ui->edtAddName->text().trimmed();
    int variantTpye = m_model->relationModel(variantTypeIdx)->index(ui->cmbAddVariantType->currentIndex(), m_model->relationModel(variantTypeIdx)->fieldIndex("ID")).data().toInt();
    int precision = ui->edtAddPrecision->text().trimmed().toInt();
    int unit =  m_model->relationModel(unitIdx)->index(ui->cmbAddUnit->currentIndex(), m_model->relationModel(unitIdx)->fieldIndex("ID")).data().toInt();
    int indicatorObj = m_model->relationModel(indicatorObjIdx)->index(ui->cmbAddIndicator->currentIndex(), m_model->relationModel(indicatorObjIdx)->fieldIndex("ID")).data().toInt();

    QSqlField fDevice("DEVICE_ID", QVariant::Int);
    QSqlField fModbusFunc("MODBUS_FUNC_ID", QVariant::Int);
    QSqlField fAddress("ADDRESS", QVariant::Int);
    QSqlField fName("NAME", QVariant::String);
    QSqlField fVariantType("VARIANT_ID", QVariant::Int);
    QSqlField fPrecision("PRECISION", QVariant::Int);
    QSqlField fUnit("UNIT_ID", QVariant::Int);
    QSqlField fIndicatorObj("CONTROL_OBJ_ID", QVariant::Int);

    fDevice.setValue(QVariant(device));
    fModbusFunc.setValue(QVariant(modbusFunc));
    fAddress.setValue(QVariant(address));
    fName.setValue(QVariant(name));
    fVariantType.setValue(QVariant(variantTpye));
    fPrecision.setValue(QVariant(precision));
    fUnit.setValue(QVariant(unit));
    fIndicatorObj.setValue(QVariant(indicatorObj));

    QSqlRecord record;
    record.append(fDevice);
    record.append(fModbusFunc);
    record.append(fAddress);
    record.append(fName);
    record.append(fVariantType);
    record.append(fPrecision);
    record.append(fUnit);
    record.append(fIndicatorObj);

    m_model->database().transaction();
    if (m_model->insertRecord(-1, record)) {
        if (!ui->btnSave->isEnabled())
            ui->btnSave->setEnabled(true);

        if (!ui->btnCancel->isEnabled())
            ui->btnCancel->setEnabled(true);

        while (m_model->canFetchMore())
            m_model->fetchMore();

        QTimer::singleShot(10, ui->tableView, SLOT(scrollToBottom()));

        ui->edtAddAddress->setText(QString::number(address+1));

        m_dataChanged = true;
    }
    else
        qDebug() << m_model->lastError() << "\n" <<m_model->lastError().text();
}

void DlgRegister::on_btnSave_clicked()
{
//    m_model->database().transaction();
    if (m_model->submitAll()) {
        m_model->database().commit();
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
                                   tr("The database reported an error: %1").arg(m_model->lastError().text()));
    }
}

void DlgRegister::on_btnChange_clicked()
{
    int device = m_model->relationModel(deviceIdx)->index(ui->cmbEditDevice->currentIndex(), m_model->relationModel(deviceIdx)->fieldIndex("ID")).data().toInt();
    int modbusFunc = m_model->relationModel(modbusFuncIdx)->index(ui->cmbEditModbusFunc->currentIndex(), m_model->relationModel(modbusFuncIdx)->fieldIndex("ID")).data().toInt();
    QString address = ui->edtEditAddress->text().trimmed();
    QString name = ui->edtEditName->text().trimmed();
    int variantTpye = m_model->relationModel(variantTypeIdx)->index(ui->cmbEditVariantType->currentIndex(), m_model->relationModel(variantTypeIdx)->fieldIndex("ID")).data().toInt();
    QString precision = ui->edtEditPrecision->text().trimmed();
    int unit =  m_model->relationModel(unitIdx)->index(ui->cmbEditUnit->currentIndex(), m_model->relationModel(unitIdx)->fieldIndex("ID")).data().toInt();
    int indicatorObj = m_model->relationModel(indicatorObjIdx)->index(ui->cmbEditIndicator->currentIndex(), m_model->relationModel(indicatorObjIdx)->fieldIndex("ID")).data().toInt();

    QSqlField fDevice("DEVICE_ID", QVariant::Int);
    QSqlField fModbusFunc("MODBUS_FUNC_ID", QVariant::Int);
    QSqlField fAddress("ADDRESS", QVariant::Int);
    QSqlField fName("NAME", QVariant::String);
    QSqlField fVariantType("VARIANT_ID", QVariant::Int);    
    QSqlField fPrecision("PRECISION", QVariant::Int);
    QSqlField fUnit("UNIT_ID", QVariant::Int);
    QSqlField fIndicatorObj("CONTROL_OBJ_ID", QVariant::Int);

    fDevice.setValue(QVariant(device));
    fModbusFunc.setValue(QVariant(modbusFunc));
    fAddress.setValue(QVariant(address));
    fName.setValue(QVariant(name));
    fVariantType.setValue(QVariant(variantTpye));       
    fPrecision.setValue(QVariant(precision));
    fUnit.setValue(QVariant(unit));
    fIndicatorObj.setValue(QVariant(indicatorObj));

    QSqlRecord record;
    record.append(fDevice);
    record.append(fModbusFunc);
    record.append(fAddress);
    record.append(fName);
    record.append(fVariantType);
    record.append(fPrecision);
    record.append(fUnit);
    record.append(fIndicatorObj);

    m_model->database().transaction();
    if (m_model->setRecord(m_selectedRowIdx, record)) {
        if (!ui->btnSave->isEnabled())
            ui->btnSave->setEnabled(true);

        if (!ui->btnCancel->isEnabled())
            ui->btnCancel->setEnabled(true);

        m_dataChanged = true;
    }
}

void DlgRegister::on_btnCancel_clicked()
{
    if (m_dataChanged) {
        m_model->revertAll();
        m_model->database().rollback();
//        m_model->revertRow(m_selectedRowIdx);
    }

    if (!ui->grp_edit->isHidden())
        ui->grp_edit->hide();

    if (!ui->grp_new->isHidden())
        ui->grp_new->hide();

    ui->btnSave->setEnabled(false);
    ui->btnCancel->setEnabled(false);

    m_dataChanged = false;
}

void DlgRegister::on_DlgRegister_finished(int result)
{
    if (m_dataChanged) {
        if (result == QDialog::Rejected) {
            int ret = QMessageBox::warning(this, tr("The document has been modified."),
                                         tr("Do you want to save your changes?"), QMessageBox::Save | QMessageBox::Discard /*| QMessageBox::Cancel*/ );

            if (ret == QMessageBox::Save)
                on_btnSave_clicked();
        }
    }
}
