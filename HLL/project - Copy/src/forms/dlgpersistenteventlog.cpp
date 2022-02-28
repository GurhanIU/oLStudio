#include "dlgpersistenteventlog.h"
#include "ui_dlgpersistenteventlog.h"

#include "abstractformeditor.h"
#include "objects/ebusdata.h"
#include "objects/ebusdataentries.h"

#include <QMessageBox>
#include <QSql>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStatusBar>
#include <QProgressBar>

#include <QDebug>

DlgPersistentEventLog::DlgPersistentEventLog(EDesignerFormEditorInterface *core, QModbusClient *modbus, QWidget *parent) :
    m_core(core),
    m_modbus(modbus),
    QDialog(parent),
    ui(new Ui::DlgPersistentEventLog)
{
    ui->setupUi(this);

    QStatusBar *statusBar = new QStatusBar;
    statusBar->setSizeGripEnabled(true);
    statusBar->setStyleSheet("QStatusBar{ \
                             background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF); \
                             border: 1px solid  lightgrey; \
                             border-radius: 0px; \
                             }");

    QProgressBar *progressBar = new QProgressBar(statusBar);
    progressBar->setFixedWidth(200);
    progressBar->setFixedHeight(20);
    progressBar->setToolTip(tr("Percent of Process"));
    progressBar->setTextVisible(true);
    progressBar->setAlignment(Qt::AlignCenter);
    progressBar->setRange(0,100);
    progressBar->setValue(0);
    progressBar->setStyleSheet("QProgressBar::chunk { background-color: #007de5; }");

    statusBar->addPermanentWidget(progressBar,10);
    ui->mainLayout->addWidget(statusBar);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_dataEntries = new EBusDataEntries(m_modbus, 1, this);
    m_dataEntries->setRegisterType(QModbusDataUnit::HoldingRegisters);

    connect(m_dataEntries, &EBusDataEntries::sgProcessRange, progressBar, &QProgressBar::setRange);
    connect(m_dataEntries, &EBusDataEntries::sgProcessValue, progressBar, &QProgressBar::setValue);
    connect(progressBar, &QProgressBar::valueChanged, progressBar, [=](int value){
        if (value == progressBar->maximum()) {
            qDebug() << "Completed!";
            ui->tableWidget->sortByColumn(2, Qt::DescendingOrder);
//            ui->tableWidget->resizeColumnsToContents();
            ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
        }
    });

    connect(ui->btnUpdate, &QToolButton::clicked, m_dataEntries, &EBusDataEntries::readAllEntries);
    connect(m_dataEntries, &EBusDataEntries::sgMessage, this, &DlgPersistentEventLog::sgMessage);

    QStringList tableHeaders = collectEventMetaData();
    ui->tableWidget->setColumnCount(tableHeaders.count());
    ui->tableWidget->setHorizontalHeaderLabels(tableHeaders);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode( QHeaderView::Interactive );
    ui->tableWidget->horizontalHeader()->setSectionsMovable(true);
    ui->tableWidget->horizontalHeader()->setMinimumHeight(40);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setColumnHidden(1, true);
    ui->tableWidget->setColumnHidden(17, true);
    ui->tableWidget->setColumnHidden(18, true);
    ui->tableWidget->sortByColumn(2, Qt::DescendingOrder);
    ui->tableWidget->setItemDelegateForColumn(2, new DateTimeItemDelegate(ui->tableWidget));
    ui->tableWidget->setItemDelegateForColumn(0, new EventNameItemDelegate(collectEventName(), ui->tableWidget));

    collectEvents();    

    int m = m_dataEntries->allEntries().count() % tableHeaders.count();

    if ( m > 0)
        qDebug() << "hata" << m;
    else {
        int c = m_dataEntries->allEntries().count() / tableHeaders.count();
        for (int i=0; i<c; i++) {
            ui->tableWidget->insertRow(i);
            EBusDataEntries::EntryList ee(m_dataEntries->allEntries().mid(i*tableHeaders.count(), tableHeaders.count()));            

            for(int j=0; j< ee.count(); j++) {
                QTableWidgetItem *item = new QTableWidgetItem(ee.at(j)->toFormattedString());

                connect(ee.at(j), &EBusData::dataChanged, [=](EData *data) {
                    item->setText(ee.at(j)->toFormattedString());
                });

                ui->tableWidget->setItem(i, j, item);
            }
        }
    }
}

DlgPersistentEventLog::~DlgPersistentEventLog()
{
    qDebug() << this << "Siliniyor.";
    delete ui;
}

void DlgPersistentEventLog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    emit sgVisibilityChanged(true);
    m_dataEntries->readAllEntries();
//    m_dataEntries->startReadContinuously(5000);
}

void DlgPersistentEventLog::hideEvent(QHideEvent *e)
{
    QDialog::hideEvent(e);
    emit sgVisibilityChanged(false);
//    m_dataEntries->stopReadContinuously();
}

QStringList DlgPersistentEventLog::collectEventMetaData() const
{
    QStringList list;
    QSqlQuery qMetaData = QSqlQuery(QSqlDatabase::database(m_core->dbFile()));
    qMetaData.exec("SELECT NAME FROM EVENT_METADATA ORDER BY ID;");

    while (qMetaData.next())
        list.append(qMetaData.value("NAME").toString());

    return list;
}

QMap<int, QString> DlgPersistentEventLog::collectEventName() const
{
    QSqlQuery qEventName = QSqlQuery(QSqlDatabase::database(m_core->dbFile()));
    qEventName.exec(QString("SELECT ID, TEXT FROM EVENT_INFO WHERE EVENT_TYPE_ID = 1;"));

    QMap<int, QString> map;

    while (qEventName.next()) {
        QSqlRecord record = qEventName.record();
        map.insert(record.value("ID").toInt(), record.value("TEXT").toString());
    }

    return map;
}

void DlgPersistentEventLog::collectEvents()
{
    QSqlQuery qEventMeta = QSqlQuery(QSqlDatabase::database(m_core->dbFile()));
    qEventMeta.exec(QString("SELECT r.ID AS ID, r.ADDRESS, r.NAME, em.NAME AS METANAME , vt.ID AS VTYPE, r.PRECISION, u.UNIT \
                                FROM REGISTER AS r \
                                LEFT JOIN EVENT_REGISTER  AS er ON r.ID = er.REGISTER_ID \
                                LEFT JOIN VARIANT_TYPE as vt ON r.VARIANT_ID = vt.ID \
                                LEFT JOIN UNITS as u ON  r.UNIT_ID = u.ID \
                                LEFT JOIN EVENT_METADATA AS em ON er.EVENT_METADATA_ID = em.ID \
                                WHERE r.ID IN (SELECT REGISTER_ID FROM PAGETYPE_REGISTER WHERE PAGETYPE_ID = 3);"));

    while (qEventMeta.next()) {
        QSqlRecord record = qEventMeta.record();

        static const int rIdIdx = record.indexOf("ID");
        static const int rNameIdx = record.indexOf("METANAME");
        static const int rAddressIdx = record.indexOf("ADDRESS");
        static const int rPrecisionIdx = record.indexOf("PRECISION");
        static const int uUnitIdx = record.indexOf("UNIT");
        static const int vTypeIdx = record.indexOf("VTYPE");

        const int registerId = record.value(rIdIdx).toInt();
        const QString name = record.value(rNameIdx).toString();
        const int address = record.value(rAddressIdx).toInt();
        const int precision = record.value(rPrecisionIdx).toInt();
        const QMetaType::Type type = (QMetaType::Type)record.value(vTypeIdx).toUInt();

        m_dataEntries->addEntry(registerId, address, EDataUtil::create(type), precision, name);
    }
}

void DlgPersistentEventLog::initEvents()
{
//    if (m_dataEntries->allEntries().isEmpty())
//        return;

//    foreach (EBusData *e, m_dataEntries->allEntries()) {
//        qDebug() << e->alias() << e->startAddress() << e->dataTypeName() << e->toString();
//    }

//    ModbusData *fistData = m_dataEntries->allEntries().first();

//    QListIterator<ModbusData *> i(m_dataEntries->allEntries());
//    while (i.hasNext()) {
//        ModbusData *mData = i.next();
//        qDebug() << mData;
//    }

//    QSqlQuery q = QSqlQuery(QSqlDatabase::database(m_core->dbFile()));
//    for (int i = 51; i<=52; i++) {
//        for (int j = 1; j <=20; j++) {
//            QString qry = QString("INSERT INTO EVENT_INFO_METADATA ('EVENT_INFO_ID', 'EVENT_METADATA_ID') VALUES ('%1', '%2'); ").arg(i).arg(j);

//            if(!q.exec(qry))
//                qDebug() << q.lastError().text();
//        }
//    }

//    int regId = 353;
//    for (int i = 3; i<=50; i++) {
//        for (int j = 1; j <=19; j++) {
//            QString qry = QString("INSERT INTO EVENT_REGISTER ('REGISTER_ID', 'EVENT_METADATA_ID') VALUES ('%1', '%2'); ").arg(regId++).arg(j);

//            if(!q.exec(qry))
//                qDebug() << q.lastError().text();
//        }
//    }

//    static int address = 491;
//    for (int i = 1 ; i <= 50; i++) {
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - Event',                            '36', '0', '27', '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - Yedek Timestamp',                  '36', '0', '27', '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - Tarih Saat',                       '35', '0', '27', '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        address += 1;
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - 28V Alternatör Voltajı',           '36', '1', '1',  '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - 56V Alternatör Voltajı',           '36', '1', '1',  '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - 28V AlternatörAkımı',              '36', '0', '28', '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - 56V AlternatörAkımı',              '36', '0', '28', '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - 28V Bara Yük Akımı',               '36', '0', '28', '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - 56V Bara Yük Akımı',               '36', '0', '28', '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - 28V Akü Voltajı',                  '36', '1', '1',  '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - 56V Akü Voltajı',                  '36', '1', '1',  '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - Yakıt Seviye',                     '36', '0', '7',  '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - Dizel Motor Sıcaklık',             '36', '0', '11', '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - 28V Alternatör Sargı Sıcaklık',    '36', '0', '11', '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - 56V Alternatör Sargı Sıcaklık',    '36', '0', '11', '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - Çalışma Saati',                    '36', '0', '24', '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - Motor Hızı',                       '36', '0', '5',  '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - Yedek1',                           '36', '0', '27', '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//        qDebug().noquote() << QString("INSERT INTO REGISTER ('DEVICE_ID', 'MODBUS_FUNC_ID', 'ADDRESS', 'NAME', 'VARIANT_ID', 'PRECISION', 'UNIT_ID', 'CONTROL_OBJ_ID') VALUES ('1', '4', '%1', 'E%2 - Yedek2',                           '36', '0', '27', '6');").arg(address++).arg(i, 2, 10, QChar('0'));
//    }
}
