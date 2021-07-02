#include "DlgAddress.h"
#include "ui_DlgAddress.h"

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
#include <QDebug>

DlgAddress::DlgAddress(EDesignerFormEditorInterface *core,QWidget *parent) :
    m_core(core),
    QDialog(parent),
    ui(new Ui::DlgAddress)
{
    ui->setupUi(this);

    ui->grp_new->hide();
    ui->grp_edit->hide();
    ui->btnSave->setDisabled(true);

//    ui->edtAddName->setValidator(new QRegExpValidator( QRegExp("[0-9]{1,20}"), this ));
//    ui->edtAddName->setValidator(new QRegExpValidator(QRegExp("^[A-Za-z0-9]{1,50}"), this ));
//    ui->edtAddName->setValidator(new QRegExpValidator(QRegExp("^[A-Za-z0-9_]{1,50}"), this ));
//    ui->edtEditName->setValidator(new QRegExpValidator(QRegExp("^[A-Za-z0-9_]{1,50}"), this ));

//    ui->edtAddAddress->setValidator( new QIntValidator(0, 10000, this) );
//    ui->edtEditAddress->setValidator( new QIntValidator(0, 10000, this) );

    ui->edtAddAddress->setValidator(new QRegExpValidator( QRegExp("^0?[x]?[0-9A-F]{8}$", Qt::CaseInsensitive), this));
    ui->edtEditAddress->setValidator(new QRegExpValidator(QRegExp("^0?[x]?[0-9A-F]{8}$", Qt::CaseInsensitive), this));

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

    int nameIdx = m_model->fieldIndex("NAME");
    variantTypeIdx = m_model->fieldIndex("VARIANT_ID");
    modbusFuncIdx = m_model->fieldIndex("MODBUS_FUNC_ID");
    aliasIdx = m_model->fieldIndex("ALIAS_ID");
    deviceIdx = m_model->fieldIndex("DEVICE_ID");
    unitIdx = m_model->fieldIndex("UNIT_ID");
    int addressIdx = m_model->fieldIndex("ADDRESS");
    int precisionIdx = m_model->fieldIndex("PRECISION");

    m_model->setRelation(variantTypeIdx, QSqlRelation("VARIANT_TYPE", "ID", "DESCRIPTION"));
    m_model->setRelation(modbusFuncIdx, QSqlRelation("MODBUS_FUNCTION", "ID", "NAME"));
    m_model->setRelation(aliasIdx, QSqlRelation("REGISTER_ALIAS", "ID", "NAME"));
    m_model->setRelation(deviceIdx, QSqlRelation("DEVICE", "ID", "NAME"));
    m_model->setRelation(unitIdx, QSqlRelation("UNITS", "ID", "UNIT"));

    // Set the localized header captions
    m_model->setHeaderData(nameIdx, Qt::Horizontal, tr("NAME"));
    m_model->setHeaderData(addressIdx, Qt::Horizontal, tr("ADDRESS"));
    m_model->setHeaderData(variantTypeIdx, Qt::Horizontal, tr("VARIANT TYPE"));
    m_model->setHeaderData(precisionIdx, Qt::Horizontal, tr("PRECISION"));
    m_model->setHeaderData(modbusFuncIdx, Qt::Horizontal, trUtf8("MODBUS FUNC"));
    m_model->setHeaderData(aliasIdx, Qt::Horizontal, trUtf8("ALIAS"));
    m_model->setHeaderData(deviceIdx, Qt::Horizontal, trUtf8("DEVICE"));
    m_model->setHeaderData(unitIdx, Qt::Horizontal, trUtf8("UNIT"));

    // Populate the model
    if (!m_model->select())
    {
        showError(m_model->lastError());
        return;
    }

    ui->tableView->setModel(m_model);
    ui->tableView->setColumnHidden(m_model->fieldIndex("ID"), true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->resizeColumnsToContents();
//    ui->tableView->setItemDelegateForColumn(addressIdx, new AddressItemDelegate() );

    ui->cmbAddVariantType->setModel(m_model->relationModel(variantTypeIdx));
    ui->cmbAddVariantType->setModelColumn(m_model->relationModel(variantTypeIdx)->fieldIndex("DESCRIPTION"));

    ui->cmbAddModbusFunc->setModel(m_model->relationModel(modbusFuncIdx));
    ui->cmbAddModbusFunc->setModelColumn(m_model->relationModel(modbusFuncIdx)->fieldIndex("NAME"));

    ui->cmbAddAlias->setModel(m_model->relationModel(aliasIdx));
    ui->cmbAddAlias->setModelColumn(m_model->relationModel(aliasIdx)->fieldIndex("NAME"));

    ui->cmbAddDevice->setModel(m_model->relationModel(deviceIdx));
    ui->cmbAddDevice->setModelColumn(m_model->relationModel(deviceIdx)->fieldIndex("NAME"));

    ui->cmbAddUnit->setModel(m_model->relationModel(unitIdx));
    ui->cmbAddUnit->setModelColumn(m_model->relationModel(unitIdx)->fieldIndex("UNIT"));

    ui->cmbEditVariantType->setModel(m_model->relationModel(variantTypeIdx));
    ui->cmbEditVariantType->setModelColumn(m_model->relationModel(variantTypeIdx)->fieldIndex("DESCRIPTION"));

    ui->cmbEditModbusFunc->setModel(m_model->relationModel(modbusFuncIdx));
    ui->cmbEditModbusFunc->setModelColumn(m_model->relationModel(modbusFuncIdx)->fieldIndex("NAME"));

    ui->cmbEditAlias->setModel(m_model->relationModel(aliasIdx));
    ui->cmbEditAlias->setModelColumn(m_model->relationModel(aliasIdx)->fieldIndex("NAME"));

    ui->cmbEditDevice->setModel(m_model->relationModel(deviceIdx));
    ui->cmbEditDevice->setModelColumn(m_model->relationModel(deviceIdx)->fieldIndex("NAME"));

    ui->cmbEditUnit->setModel(m_model->relationModel(unitIdx));
    ui->cmbEditUnit->setModelColumn(m_model->relationModel(unitIdx)->fieldIndex("UNIT"));

    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    mapper->setModel(m_model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(ui->edtEditName, nameIdx);
    mapper->addMapping(ui->edtEditAddress, addressIdx);
    mapper->addMapping(ui->edtEditPrecision, precisionIdx);
    mapper->addMapping(ui->cmbEditVariantType, variantTypeIdx);
    mapper->addMapping(ui->cmbEditModbusFunc, modbusFuncIdx);
    mapper->addMapping(ui->cmbEditAlias, aliasIdx);
    mapper->addMapping(ui->cmbEditDevice, deviceIdx);
    mapper->addMapping(ui->cmbEditUnit, unitIdx);

    connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));

    ui->tableView->setCurrentIndex(m_model->index(0, 0));
    ui->tableView->update();
}

DlgAddress::~DlgAddress()
{
    delete ui;
}

void DlgAddress::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}

void DlgAddress::slTableSingleClicked(const QModelIndex &index)
{
    if( !index.isValid())
        return;

    if( ui->grp_edit->isHidden())
        return;

    m_selectedRowIdx = index.row();
}

void DlgAddress::slTableDoubleClicked(const QModelIndex &index)
{
    if( !index.isValid())
        return;

    m_selectedRowIdx = index.row();

    if( !ui->grp_new->isHidden())
        ui->grp_new->hide();

    if( ui->grp_edit->isHidden())
        ui->grp_edit->show();
}

void DlgAddress::on_btn_newRecord_clicked()
{
    if(ui->grp_new->isHidden())
    {
        ui->grp_edit->hide();
        ui->grp_new->show();
    }
}

void DlgAddress::on_btnAdd_clicked()
{
    QString name = ui->edtAddName->text().trimmed();
    QString address = ui->edtAddAddress->text().trimmed();
    QString precision = ui->edtAddPrecision->text().trimmed();
    int variantTpye = m_model->relationModel(variantTypeIdx)->index(ui->cmbAddVariantType->currentIndex(), m_model->relationModel(variantTypeIdx)->fieldIndex("ID")).data().toInt();
    int modbusFunc = m_model->relationModel(modbusFuncIdx)->index(ui->cmbAddModbusFunc->currentIndex(), m_model->relationModel(modbusFuncIdx)->fieldIndex("ID")).data().toInt();
    int alias = m_model->relationModel(aliasIdx)->index(ui->cmbAddAlias->currentIndex(), m_model->relationModel(aliasIdx)->fieldIndex("ID")).data().toInt();
    int device = m_model->relationModel(deviceIdx)->index(ui->cmbAddDevice->currentIndex(), m_model->relationModel(deviceIdx)->fieldIndex("ID")).data().toInt();
    int unit =  m_model->relationModel(unitIdx)->index(ui->cmbAddUnit->currentIndex(), m_model->relationModel(unitIdx)->fieldIndex("ID")).data().toInt();

    QSqlField fName("NAME", QVariant::String);
//    QSqlField fAddress("ADDRESS", QVariant::UInt);
    QSqlField fAddress("ADDRESS", QVariant::String);
    QSqlField fVariantType("VARIANT_ID", QVariant::Int);
    QSqlField fModbusFunc("MODBUS_FUNC_ID", QVariant::Int);
    QSqlField fAlias("ALIAS_ID", QVariant::Int);
    QSqlField fDevice("DEVICE_ID", QVariant::Int);
    QSqlField fPrecision("PRECISION", QVariant::Int);
    QSqlField fUnit("UNIT_ID", QVariant::Int);

    fName.setValue(QVariant(name));
//    fAddress.setValue(QVariant(address.toUInt(&ok, 16)));
    fAddress.setValue(QVariant(address));
    fVariantType.setValue(QVariant(variantTpye));
    fModbusFunc.setValue(QVariant(modbusFunc));
    fAlias.setValue(QVariant(alias));
    fDevice.setValue(QVariant(device));
    fPrecision.setValue(QVariant(precision));
    fUnit.setValue(QVariant(unit));

    QSqlRecord record;
    record.append(fName);
    record.append(fAddress);
    record.append(fPrecision);
    record.append(fVariantType);
    record.append(fModbusFunc);
    record.append(fDevice);
    record.append(fAlias);
    record.append(fUnit);

    if( m_model->insertRecord(-1, record) )
    {
        if( !ui->btnSave->isEnabled())
            ui->btnSave->setEnabled(true);

        m_dataChanged = true;
    }
}

void DlgAddress::on_btnSave_clicked()
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
                                   tr("The database reported an error: %1").arg(m_model->lastError().text()));
    }
}

void DlgAddress::on_btnChange_clicked()
{
    QString name = ui->edtEditName->text().trimmed();
    QString address = ui->edtEditAddress->text().trimmed();
    QString precision = ui->edtEditPrecision->text().trimmed();
    int variantTpye = m_model->relationModel(variantTypeIdx)->index(ui->cmbEditVariantType->currentIndex(), m_model->relationModel(variantTypeIdx)->fieldIndex("ID")).data().toInt();
    int modbusFunc = m_model->relationModel(modbusFuncIdx)->index(ui->cmbEditModbusFunc->currentIndex(), m_model->relationModel(modbusFuncIdx)->fieldIndex("ID")).data().toInt();
    int alias = m_model->relationModel(aliasIdx)->index(ui->cmbEditAlias->currentIndex(), m_model->relationModel(aliasIdx)->fieldIndex("ID")).data().toInt();
    int device = m_model->relationModel(deviceIdx)->index(ui->cmbEditDevice->currentIndex(), m_model->relationModel(deviceIdx)->fieldIndex("ID")).data().toInt();
    int unit =  m_model->relationModel(unitIdx)->index(ui->cmbEditUnit->currentIndex(), m_model->relationModel(unitIdx)->fieldIndex("ID")).data().toInt();


    QSqlField fName("NAME", QVariant::String);
//    QSqlField fAddress("ADDRESS", QVariant::UInt);
    QSqlField fAddress("ADDRESS", QVariant::String);
    QSqlField fVariantType("VARIANT_ID", QVariant::Int);
    QSqlField fModbusFunc("MODBUS_FUNC_ID", QVariant::Int);
    QSqlField fAlias("ALIAS_ID", QVariant::Int);
    QSqlField fDevice("DEVICE_ID", QVariant::Int);
    QSqlField fPrecision("PRECISION", QVariant::Int);
    QSqlField fUnit("UNIT_ID", QVariant::Int);

    fName.setValue(QVariant(name));
//    fAddress.setValue(QVariant(address.toUInt(&ok, 16)));
    fAddress.setValue(QVariant(address));
    fVariantType.setValue(QVariant(variantTpye));
    fModbusFunc.setValue(QVariant(modbusFunc));
    fAlias.setValue(QVariant(alias));
    fDevice.setValue(QVariant(device));
    fPrecision.setValue(QVariant(precision));
    fUnit.setValue(QVariant(unit));

    QSqlRecord record;
    record.append(fName);
    record.append(fAddress);
    record.append(fPrecision);
    record.append(fVariantType);
    record.append(fModbusFunc);
    record.append(fDevice);
    record.append(fAlias);
    record.append(fUnit);

    if (m_model->setRecord(m_selectedRowIdx, record))
    {
        if( !ui->btnSave->isEnabled())
            ui->btnSave->setEnabled(true);

        m_dataChanged = true;
    }
}

void DlgAddress::on_btnCancel_clicked()
{
    if( m_dataChanged )
        m_model->revertRow(m_selectedRowIdx);

    if( !ui->grp_edit->isHidden())
        ui->grp_edit->hide();

    if( ui->btnSave->isEnabled())
        ui->btnSave->setEnabled(false);
}

void DlgAddress::on_DlgAddress_finished(int result)
{
    if( m_dataChanged )
    {
        if( result == QDialog::Rejected )
        {
            int ret = QMessageBox::warning(this, tr("The document has been modified."),
                                         tr("Do you want to save your changes?"), QMessageBox::Save | QMessageBox::Discard /*| QMessageBox::Cancel*/ );

            if( ret == QMessageBox::Save )
                on_btnSave_clicked();
        }
    }
}
