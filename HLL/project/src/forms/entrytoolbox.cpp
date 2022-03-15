#include "entrytoolbox.h"
#include "ui_entrytoolbox.h"

#include <QMessageBox>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlRelation>
#include <QSqlTableModel>

#include <QDebug>

static void showSqlError(QWidget *parent, const QSqlError &err)
{
    QMessageBox::critical(parent, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}

EntryToolBox::EntryToolBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EntryToolBox)
{
    ui->setupUi(this);
}

EntryToolBox::~EntryToolBox()
{
    delete ui;
}

void EntryToolBox::initAddToolBox()
{
    QSqlDatabase db; // = QSqlDatabase::database(m_core->dbFile());

    m_addModel = new QSqlRelationalTableModel(this, db);
    m_addModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_addModel->setTable("REGISTER");

    modbusFuncIdx   = m_addModel->fieldIndex("MODBUS_FUNC_ID");
    variantTypeIdx  = m_addModel->fieldIndex("VARIANT_ID");
    unitIdx         = m_addModel->fieldIndex("UNIT_ID");

    m_addModel->setRelation(modbusFuncIdx, QSqlRelation("MODBUS_FUNCTION", "ID", "NAME"));
    m_addModel->setRelation(variantTypeIdx, QSqlRelation("VARIANT_TYPE", "ID", "DESCRIPTION"));
    m_addModel->setRelation(unitIdx, QSqlRelation("UNITS", "ID", "UNIT"));

    // Populate the model
    if (!m_addModel->select()) {
        showSqlError(this, m_addModel->lastError());
        return;
    }

    // -- Add
    QSqlTableModel *modbusFuncModel = m_addModel->relationModel(modbusFuncIdx);
    modbusFuncModel->setFilter(QString("ACTIVATED == '1'"));
    ui->cmbAddModbusFunc->setModel(modbusFuncModel);
    ui->cmbAddModbusFunc->setModelColumn(m_addModel->relationModel(modbusFuncIdx)->fieldIndex("NAME"));

    QSqlTableModel *variantModel = m_addModel->relationModel(variantTypeIdx);
    variantModel->setFilter(QString("AVAILABLE == '1'"));
    ui->cmbAddVariantType->setModel(variantModel);
    ui->cmbAddVariantType->setModelColumn(m_addModel->relationModel(variantTypeIdx)->fieldIndex("DESCRIPTION"));

    ui->cmbAddUnit->setModel(m_addModel->relationModel(unitIdx));
    ui->cmbAddUnit->setModelColumn(m_addModel->relationModel(unitIdx)->fieldIndex("UNIT"));
}

void EntryToolBox::on_btnAdd_clicked()
{
    int modbusFunc = m_addModel->relationModel(modbusFuncIdx)->index(ui->cmbAddModbusFunc->currentIndex(), m_addModel->relationModel(modbusFuncIdx)->fieldIndex("ID")).data().toInt();
    int address = ui->edtAddAddress->text().trimmed().toInt();
    QString name = ui->edtAddName->text().trimmed();
    int variantTpye = m_addModel->relationModel(variantTypeIdx)->index(ui->cmbAddVariantType->currentIndex(), m_addModel->relationModel(variantTypeIdx)->fieldIndex("ID")).data().toInt();
    int precision = ui->edtAddPrecision->text().trimmed().toInt();
    int unit =  m_addModel->relationModel(unitIdx)->index(ui->cmbAddUnit->currentIndex(), m_addModel->relationModel(unitIdx)->fieldIndex("ID")).data().toInt();

    if (name.isEmpty() ||
        variantTpye == QMetaType::UnknownType)
        return;

    QSqlField fModbusFunc("MODBUS_FUNC_ID", QVariant::Int);
    QSqlField fAddress("ADDRESS", QVariant::Int);
    QSqlField fName("NAME", QVariant::String);
    QSqlField fVariantType("VARIANT_ID", QVariant::Int);
    QSqlField fPrecision("PRECISION", QVariant::Int);
    QSqlField fUnit("UNIT_ID", QVariant::Int);

    fModbusFunc.setValue(QVariant(modbusFunc));
    fAddress.setValue(QVariant(address));
    fName.setValue(QVariant(name));
    fVariantType.setValue(QVariant(variantTpye));
    fPrecision.setValue(QVariant(precision));
    fUnit.setValue(QVariant(unit));

    QSqlRecord record;
    record.append(fModbusFunc);
    record.append(fAddress);
    record.append(fName);
    record.append(fVariantType);
    record.append(fPrecision);
    record.append(fUnit);

    m_addModel->database().transaction();
    if (m_addModel->insertRecord(-1, record)) {
        if (m_addModel->submitAll()) {
            m_addModel->database().commit();
        }
        else {
            m_addModel->database().rollback();
            QMessageBox::warning(this, tr("Cached Table"),
                                       tr("The database reported an error: %1").arg(m_addModel->lastError().text()));
        }

        emit sgCollectRegisters();
    }
    else
        qDebug() << m_addModel->lastError() << "\n" <<m_addModel->lastError().text();
}
