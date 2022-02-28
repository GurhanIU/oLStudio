#include "WdgTest.h"
#include "ui_WdgTest.h"

#include "abstractformeditor.h"
#include "widgetfactory_p.h"
#include "objects/editablesqlmodel.h"
#include "objects/ebusdata.h"
#include "objects/ebusdataentries.h"

#include <limits.h>
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
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

#include <QDebug>

WdgTest::WdgTest(EDesignerFormEditorInterface *core, EBusDataEntries *dataEntries, QWidget *parent) :
    QWidget(parent),
    m_core(core),
    m_dataEntries(dataEntries),
    m_pageId(-1),
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
}

WdgTest::~WdgTest()
{
    qDebug() << "Deleted Page:" << m_pageId;
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

void WdgTest::slUpdateModelByPageId(const QString &name, const int &id)
{
    if (m_pageId == id)
        return;

    m_pageId = id;
    m_entryList.clear();
    ui->grpToolBox->setTitle(name);

    if (m_model)
        delete m_model;

    m_model = new EditableSqlModel(QSqlDatabase::database(m_core->dbFile()), ui->tableView);
    m_model->setQuery(QString("SELECT r.ID AS rID, r.NAME, r.ADDRESS, r.PRECISION, u.UNIT \
                                FROM REGISTER AS r \
                                LEFT JOIN MODBUS_FUNCTION AS mf ON r.MODBUS_FUNC_ID = mf.ID \
                                LEFT JOIN UNITS as u ON  r.UNIT_ID = u.ID \
                                LEFT JOIN REGISTER_OPTION  as ro ON r.ID = ro.REGISTER_ID \
                              WHERE r.ID IN ( SELECT REGISTER_ID FROM PAGE_REGISTER WHERE PAGE_ID = %1) GROUP BY r.ID ORDER BY r.ADDRESS;").arg(id), QSqlDatabase::database(m_core->dbFile()));

    if(m_model->lastError().isValid()) {
        QMessageBox::critical(this, tr("Unable to initialize Database"),
                                    tr("Error initializing database: %1").arg(m_model->lastError().text()));
        return;
    }

    int rIdIdx = m_model->record().indexOf("rID");
    int rNameIdx = m_model->record().indexOf("NAME");
    int rAddressIdx = m_model->record().indexOf("ADDRESS");
    int rPrecisionIdx = m_model->record().indexOf("PRECISION");
    int uUnitIdx = m_model->record().indexOf("UNIT");

    ui->tableView->setModel(m_model);
    ui->tableWidget->setRowCount(0);

    for (int modelIdx = 0; modelIdx < m_model->rowCount(); modelIdx++) {
        const QString caption = m_model->record(modelIdx).value(rNameIdx).toString();
        const int modbusAddress = m_model->record(modelIdx).value(rAddressIdx).toInt();
        const int precision = m_model->record(modelIdx).value(rPrecisionIdx).toInt();
        const QString unit = m_model->record(modelIdx).value(uUnitIdx).toString();

        EBusData *busData = m_dataEntries->entry(modbusAddress);

        if (busData)
            m_entryList.append(busData);
        else
            continue;       

        ui->tableWidget->insertRow(modelIdx);
        QTableWidgetItem *itemName = new QTableWidgetItem(caption);
        QTableWidgetItem *itemCurrent = new QTableWidgetItem(QString("%1 %2").arg("-").arg(unit));
        QTableWidgetItem *itemTest = new QTableWidgetItem("");

        QString sFactor = "1";
        int factor = sFactor.leftJustified(precision+1, QChar('0')).toInt();
        double min = std::numeric_limits<double>::min(); //m_model->record(modelIdx).value(pMinimumIdx).toDouble();
        double max = std::numeric_limits<double>::max();//m_model->record(modelIdx).value(pMaximumIdx).toDouble();

        QLineEdit *edt = new QLineEdit;
//        edt->setProperty("dataIndex", QVariant(m_model->index(modelIdx, pActualIdx)));
        edt->setProperty("factor", QVariant(factor));
        edt->setProperty("address", QVariant(modbusAddress));

        QDoubleValidator *dblVal = new QDoubleValidator(min, max, precision, edt);
        dblVal->setNotation(QDoubleValidator::StandardNotation);
        dblVal->setLocale(QLocale::C);

        connect(edt, &QLineEdit::returnPressed, m_dataEntries, [=] {

            int factor = edt->property("factor").toInt();

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
    ui->tableView->setCurrentIndex(m_model->index(0, 0));

    ui->grpToolBox->show();
    #ifdef QT_DEBUG
    ui->tableView->show();
    #else
    ui->tableView->hide();
    #endif
}

void WdgTest::actualValueChanged(int value)
{
    QObject *obj = qobject_cast<QObject*>(sender());
    m_model->setData(obj->property("dataIndex").toModelIndex(), QVariant(value), Qt::EditRole);
}
