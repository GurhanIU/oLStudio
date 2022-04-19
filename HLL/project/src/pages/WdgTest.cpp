#include "WdgTest.h"
#include "ui_WdgTest.h"

#include "objects/editablesqlmodel.h"
#include "objects/ebusdata.h"
#include "objects/ebusdataentries.h"

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <QModbusDevice>
#include <QDebug>

WdgTest::WdgTest(EBusDataEntries *dataEntries, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgTest),
    m_dataEntries(dataEntries)
{
    ui->setupUi(this);
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

void setValidatorByTpye(QLineEdit *lineEdit, int decimals, QVariant::Type type)
{
    Q_UNUSED(decimals)

    QMetaType::Type mType = static_cast<QMetaType::Type>(type);

    int min = mType == QMetaType::Char   ? std::numeric_limits<qint8>::min() :
              mType == QMetaType::UChar  ? std::numeric_limits<quint8>::min() :
              mType == QMetaType::Short  ? std::numeric_limits<qint16>::min() :
              mType == QMetaType::UShort ? std::numeric_limits<quint16>::min() :
              mType == QMetaType::Int    ? std::numeric_limits<qint32>::min() :
              mType == QMetaType::UInt   ? std::numeric_limits<quint32>::min() :
              mType == QMetaType::Long   ? std::numeric_limits<qint32>::min() :
                                          std::numeric_limits<quint32>::min();

    int max = mType == QMetaType::Char   ? std::numeric_limits<qint8>::max() :
              mType == QMetaType::UChar  ? std::numeric_limits<quint8>::max() :
              mType == QMetaType::Short  ? std::numeric_limits<qint16>::max() :
              mType == QMetaType::UShort ? std::numeric_limits<quint16>::max() :
              mType == QMetaType::Int    ? std::numeric_limits<qint32>::max() :
              mType == QMetaType::UInt   ? std::numeric_limits<quint32>::max() :
              mType == QMetaType::Long   ? std::numeric_limits<qint32>::max() :
                                          std::numeric_limits<quint32>::max();

    qDebug() << lineEdit->objectName() << min << max;

//    QDoubleValidator *validator = new QDoubleValidator(min, max, decimals, lineEdit);
//    validator->setNotation(QDoubleValidator::StandardNotation);
//    validator->setLocale(QLocale::C);
    QIntValidator *validator = new QIntValidator(min, max, lineEdit);

    lineEdit->setValidator(validator);
}

void WdgTest::slUpdateModel()
{
    ui->tableWidget->setRowCount(0);

    foreach (EBusData *busData, m_dataEntries->allEntries()) {
        if (!busData)
            continue;

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        QTableWidgetItem *itemName = new QTableWidgetItem(QString("%1(%2)-%3")
                                                          .arg(busData->alias())
                                                          .arg(busData->startAddress())
                                                          .arg(busData->dataTypeName()));
        QTableWidgetItem *itemCurrent = new QTableWidgetItem(QString("- %1").arg(busData->unit()));
//        QTableWidgetItem *itemTest = new QTableWidgetItem("");

        QString sFactor = "1";
        int factor = sFactor.leftJustified(busData->precision() +1, QChar('0')).toInt();

        QLineEdit *edt = new QLineEdit;
        edt->setObjectName(itemName->text());

        connect(edt, &QLineEdit::returnPressed, m_dataEntries, [=] {
            qint64 ival= edt->text().toLongLong();

            if (factor > 1) {
                double value = edt->text().toDouble();

                ival = static_cast<int>((value * factor) + (float)(5.0 / factor));
                if (value < 0)
                    ival = static_cast<int>((value * factor) - (float)(5.0 / factor));
            }

            qDebug() << QString::number(ival);

            busData->setTempData(ival);
            m_dataEntries->writeTempValueByEntry(busData);
        });

        connect(busData, &EBusData::dataChanged, [itemCurrent](QVariant data){
            itemCurrent->setText(data.toString());
        });

        edt->setPlaceholderText("");
        edt->setText("");

        setValidatorByTpye(edt, busData->precision(), busData->dataType());

        ui->tableWidget->setColumnWidth(0, 350);
        ui->tableWidget->setColumnWidth(1, 150);
        ui->tableWidget->setColumnWidth(2, 150);
        ui->tableWidget->setItem(row, 0, itemName);
        ui->tableWidget->setItem(row, 1, itemCurrent);
        ui->tableWidget->setCellWidget(row, 2, edt);
//        ui->tableWidget->setItem(row, 3, itemTest);
        ui->tableWidget->setRowHeight(row, 25);
    }
}
