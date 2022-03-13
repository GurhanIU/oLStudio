#include "WdgTest.h"
#include "ui_WdgTest.h"

#include "abstractformeditor.h"
#include "widgetfactory_p.h"
#include "objects/editablesqlmodel.h"
#include "objects/ebusdata.h"
#include "objects/ebusdataentries.h"

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QModbusDevice>
#include <QSql>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>

#include <QDebug>

WdgTest::WdgTest(EDesignerFormEditorInterface *core, EBusDataEntries *dataEntries, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgTest),
    m_core(core),
    m_dataEntries(dataEntries)
{
    ui->setupUi(this);

    initAddToolBox();
}

WdgTest::~WdgTest()
{
    qDebug() << "WdgTest Deleted!";
    delete ui;
}

void WdgTest::slModbusStateChanged(int state)
{
//    bool connected = (state == QModbusDevice::ConnectedState);

    switch (state) {
    case QModbusDevice::UnconnectedState:

        break;
    case QModbusDevice::ConnectingState:

        break;
    case QModbusDevice::ConnectedState:

        break;
    case QModbusDevice::ClosingState:

        break;
    }
}

void setValidatorByTpye(QLineEdit *lineEdit, int decimals, QMetaType::Type type)
{
    double min = type == QMetaType::Char   ? std::numeric_limits<qint8>::min() :
                 type == QMetaType::UChar  ? std::numeric_limits<quint8>::min() :
                 type == QMetaType::Short  ? std::numeric_limits<qint16>::min() :
                 type == QMetaType::UShort ? std::numeric_limits<quint16>::min() :
                 type == QMetaType::Int    ? std::numeric_limits<qint32>::min() :
                 type == QMetaType::UInt   ? std::numeric_limits<quint32>::min() :
                 type == QMetaType::Long   ? std::numeric_limits<qint32>::min() :
                                              std::numeric_limits<quint32>::min();

    double max = type == QMetaType::Char   ? std::numeric_limits<qint8>::max() :
                 type == QMetaType::UChar  ? std::numeric_limits<quint8>::max() :
                 type == QMetaType::Short  ? std::numeric_limits<qint16>::max() :
                 type == QMetaType::UShort ? std::numeric_limits<quint16>::max() :
                 type == QMetaType::Int    ? std::numeric_limits<qint32>::max() :
                 type == QMetaType::UInt   ? std::numeric_limits<quint32>::max() :
                 type == QMetaType::Long   ? std::numeric_limits<qint32>::max() :
                                              std::numeric_limits<quint32>::max();

    qDebug() << lineEdit->objectName() << min << max;

    QDoubleValidator *dblVal = new QDoubleValidator(min, max, decimals, lineEdit);
    dblVal->setNotation(QDoubleValidator::StandardNotation);
    dblVal->setLocale(QLocale::C);

    lineEdit->setValidator(dblVal);
}

void WdgTest::slUpdateModel()
{
    m_entryList.clear();

    ui->tableWidget->setRowCount(0);

    int row = 0;
    foreach (EBusData *busData, m_dataEntries->allEntries()) {
        if (busData)
            m_entryList.append(busData);
        else
            continue;

        ui->tableWidget->insertRow(row);
        QTableWidgetItem *itemName = new QTableWidgetItem(QString("%1(%2)-%3")
                                                          .arg(busData->alias())
                                                          .arg(busData->startAddress())
                                                          .arg(busData->dataTypeName()));
        QTableWidgetItem *itemCurrent = new QTableWidgetItem(QString("- %1").arg(busData->unit()));
        QTableWidgetItem *itemTest = new QTableWidgetItem("");

        QString sFactor = "1";
        int factor = sFactor.leftJustified(busData->precision() +1, QChar('0')).toInt();

        QLineEdit *edt = new QLineEdit;
        edt->setObjectName(itemName->text());
        edt->setProperty("address", QVariant(busData->startAddress()));

        connect(edt, &QLineEdit::returnPressed, m_dataEntries, [=] {
            qint64 ival= edt->text().toLongLong();

            if (factor > 1) {
                double value = edt->text().toDouble();

                ival = static_cast<int>((value * factor) + (float)(5.0 / factor));
                if (value < 0)
                    ival = static_cast<int>((value * factor) - (float)(5.0 / factor));
            }

            itemTest->setText(QString::number(ival));
            qDebug() << busData->alias()
                     << ival
                     << busData->dataType()
                     << QMetaType::sizeOf(busData->dataType())
                     << busData->dataTypeName();

            busData->setTempData(ival);
            m_dataEntries->writeTempValueByEntry(busData);
        });

//        connect(edt, &CustomLineEdit::validatedValue, this, &WdgTest::actualValueChanged);
//        connect(edt, &CustomLineEdit::validatedValue, busData, &EBusData::setTempValue);
//        connect(edt, &CustomLineEdit::validatedValue, busData, [busData](short value) {
//            busData->changeData(value);
//        });

        edt->setPlaceholderText("");
        edt->setText("");

        setValidatorByTpye(edt, busData->precision(), busData->dataType());

        ui->tableWidget->setItem(row, 0, itemName);
        ui->tableWidget->setItem(row, 1, itemCurrent);
        ui->tableWidget->setCellWidget(row, 2, edt);
        ui->tableWidget->setItem(row, 3, itemTest);
        ui->tableWidget->setRowHeight(row++, 25);
    }
}

void WdgTest::initAddToolBox()
{
    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());

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
        showError(m_addModel->lastError());
        return;
    }

    // -- Add
    ui->cmbAddModbusFunc->setModel(m_addModel->relationModel(modbusFuncIdx));
    ui->cmbAddModbusFunc->setModelColumn(m_addModel->relationModel(modbusFuncIdx)->fieldIndex("NAME"));

    QSqlTableModel *variantModel = m_addModel->relationModel(variantTypeIdx);
    variantModel->setFilter(QString("AVAILABLE == '1'"));
    ui->cmbAddVariantType->setModel(variantModel);
    ui->cmbAddVariantType->setModelColumn(m_addModel->relationModel(variantTypeIdx)->fieldIndex("DESCRIPTION"));

    ui->cmbAddUnit->setModel(m_addModel->relationModel(unitIdx));
    ui->cmbAddUnit->setModelColumn(m_addModel->relationModel(unitIdx)->fieldIndex("UNIT"));
}

void WdgTest::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}

void WdgTest::on_btnAdd_clicked()
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
