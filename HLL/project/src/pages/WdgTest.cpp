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
    m_core(core),
    m_dataEntries(dataEntries),
    ui(new Ui::WdgTest)
{
    ui->setupUi(this);

    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView->verticalHeader()->setVisible(true);
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->resizeColumnsToContents();

    initAddToolBox();
}

WdgTest::~WdgTest()
{
    qDebug() << "WdgTest Deleted!";
    delete ui;
}

void WdgTest::slModbusStateChanged(int state)
{
    bool connected = (state == QModbusDevice::ConnectedState);

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

void WdgTest::slUpdateModel()
{
    m_entryList.clear();

    EditableSqlModel *model = new EditableSqlModel(QSqlDatabase::database(m_core->dbFile()), ui->tableView);
    model->setQuery(QString("SELECT r.ID AS rID, r.NAME, r.ADDRESS, r.PRECISION, u.UNIT \
                                FROM REGISTER AS r \
                                LEFT JOIN MODBUS_FUNCTION AS mf ON r.MODBUS_FUNC_ID = mf.ID \
                                LEFT JOIN UNITS as u ON  r.UNIT_ID = u.ID \
                                LEFT JOIN REGISTER_OPTION  as ro ON r.ID = ro.REGISTER_ID \
                               GROUP BY r.ID ORDER BY r.ADDRESS;"),
                    QSqlDatabase::database(m_core->dbFile()));

    if (model->lastError().isValid()) {
        QMessageBox::critical(this, tr("Unable to initialize Database"),
                                    tr("Error initializing database: %1").arg(model->lastError().text()));
        return;
    }

    int rNameIdx = model->record().indexOf("NAME");
    int rAddressIdx = model->record().indexOf("ADDRESS");
    int rPrecisionIdx = model->record().indexOf("PRECISION");
    int uUnitIdx = model->record().indexOf("UNIT");

    ui->tableView->setModel(model);
    ui->tableWidget->setRowCount(0);

    for (int modelIdx = 0; modelIdx < model->rowCount(); modelIdx++) {
        const QString caption = model->record(modelIdx).value(rNameIdx).toString();
        const int modbusAddress = model->record(modelIdx).value(rAddressIdx).toInt();
        const int precision = model->record(modelIdx).value(rPrecisionIdx).toInt();
        const QString unit = model->record(modelIdx).value(uUnitIdx).toString();

        qDebug() << caption;

        EBusData *busData = m_dataEntries->entry(modbusAddress);

        if (busData)
            m_entryList.append(busData);
        else
            continue;

        ui->tableWidget->insertRow(modelIdx);
        QTableWidgetItem *itemName = new QTableWidgetItem(caption);
        QTableWidgetItem *itemCurrent = new QTableWidgetItem(QString("- %1").arg(unit));
        QTableWidgetItem *itemTest = new QTableWidgetItem("");

        QString sFactor = "1";
        int factor = sFactor.leftJustified(precision+1, QChar('0')).toInt();
        double min = std::numeric_limits<double>::min(); //m_model->record(modelIdx).value(pMinimumIdx).toDouble();
        double max = std::numeric_limits<double>::max();//m_model->record(modelIdx).value(pMaximumIdx).toDouble();

        QLineEdit *edt = new QLineEdit;
        edt->setProperty("address", QVariant(modbusAddress));

        QDoubleValidator *dblVal = new QDoubleValidator(min, max, precision, edt);
        dblVal->setNotation(QDoubleValidator::StandardNotation);
        dblVal->setLocale(QLocale::C);

        connect(edt, &QLineEdit::returnPressed, m_dataEntries, [=] {
            int ival = 0;

            if (factor > 1) {
                double value = edt->text().toDouble();

                ival = static_cast<int>((value * factor) + (float)(5.0 / factor));
                if (value < 0)
                    ival = static_cast<int>((value * factor) - (float)(5.0 / factor));
            }
            else
                ival = edt->text().toInt();

            busData->changeData(ival);
            itemTest->setText(QString::number(ival));
            qDebug() << ival;
//            this->m_dataEntries->writeTempValueByEntry(busData);
        });

//        connect(edt, &CustomLineEdit::validatedValue, this, &WdgTest::actualValueChanged);
//        connect(edt, &CustomLineEdit::validatedValue, busData, &EBusData::setTempValue);
//        connect(edt, &CustomLineEdit::validatedValue, busData, [busData](short value) {
//            busData->changeData(value);
//        });

        edt->setPlaceholderText("");
        edt->setText("");
        edt->setValidator(dblVal);

        ui->tableWidget->setItem(modelIdx, 0, itemName);
        ui->tableWidget->setItem(modelIdx, 1, itemCurrent);
        ui->tableWidget->setCellWidget(modelIdx, 2, edt);
        ui->tableWidget->setItem(modelIdx, 3, itemTest);
        ui->tableWidget->setRowHeight(modelIdx, 30);
    }

    ui->tableView->viewport()->update();
    ui->tableView->setVisible(false);
    QRect vporig = ui->tableView->viewport()->geometry();
    QRect vpnew = vporig;
    vpnew.setWidth(std::numeric_limits<int>::max());
    ui->tableView->viewport()->setGeometry(vpnew);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->viewport()->setGeometry(vporig);
    ui->tableView->setVisible(true);
    ui->tableView->setColumnWidth(rNameIdx, 300);
    ui->tableView->setCurrentIndex(model->index(0, 0));

    #ifdef QT_DEBUG
    ui->tableView->show();
    #else
    ui->tableView->hide();
    #endif
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
        !QMetaType::isValid(variantTpye))
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
