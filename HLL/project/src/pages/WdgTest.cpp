#include "WdgTest.h"
#include "ui_WdgTest.h"

#include "abstractformeditor.h"
#include "widgetfactory_p.h"
#include "objects/editablesqlmodel.h"
#include "objects/edata.h"
#include "objects/ebusdata.h"
#include "objects/modbusdata.h"
#include "objects/modbusdataentries.h"

#include "responsepacket.h"

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

WdgTest::WdgTest(EDesignerFormEditorInterface *core, OnBusMaster *onBusMaster, QWidget *parent) :
    QWidget(parent),
    m_core(core),
    ui(new Ui::WdgTest)
{
    ui->setupUi(this);

    ui->grpToolBox->hide();
    ui->lblRequest->hide();
    ui->lblResponse->hide();
    ui->tableView->hide();
    ui->scrollArea->hide();

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
    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->resizeColumnsToContents();

    m_dataEntries = new ModbusDataEntries(onBusMaster, this);
}

WdgTest::~WdgTest()
{
    qDebug() << "Siliniyor:" << this;
    delete ui;
}

void WdgTest::slModbusStateChanged(int state)
{
    bool connected = (state == QModbusDevice::ConnectedState);

    ui->btnStart->setEnabled(connected);
    ui->btnStop->setEnabled(connected);
}

void WdgTest::init(const QString &title, int id)
{
    m_title = title;
    m_pageId = id;

    initModel(id);
    collectEntries();
    updatePage();
}

void WdgTest::initModel(int pageId)
{
    if (m_model)
        delete m_model;

    m_model = new EditableSqlModel(QSqlDatabase::database(m_core->dbFile()), ui->tableView);
    m_model->setQuery(QString("SELECT r.ID AS ID, r.NAME, vt.ID AS VTYPE, vt.DESCRIPTION, r.ADDRESS, r.PRECISION, '---' AS ACTUAL, u.UNIT \
                              FROM REGISTER AS r \
                              LEFT JOIN VARIANT_TYPE as vt ON r.VARIANT_ID = vt.ID \
                              LEFT JOIN MODBUS_FUNCTION as mf ON r.MODBUS_FUNC_ID = mf.ID \
                              LEFT JOIN UNITS as u ON  r.UNIT_ID = u.ID \
                              WHERE  r.ID IN ( SELECT REGISTER_ID FROM PAGE_REGISTER WHERE PAGE_ID = %1) GROUP BY r.ID ORDER BY r.ADDRESS;").arg(pageId),
                             QSqlDatabase::database(m_core->dbFile()));

    if (m_model->lastError().isValid()) {
        QMessageBox::critical(this, tr("Unable to initialize Database"),
                                    tr("Error initializing database: %1").arg(m_model->lastError().text()));
    }
}

void WdgTest::collectEntries()
{
    if (m_dataEntries)
        m_dataEntries->deleteAll();

    int rIdIdx = m_model->record().indexOf("rID");
    int rNameIdx = m_model->record().indexOf("NAME");
    int rAddressIdx = m_model->record().indexOf("ADDRESS");
    int rPrecisionIdx = m_model->record().indexOf("PRECISION");
    int rActualIdx = m_model->record().indexOf("ACTUAL");
    int uUnitIdx = m_model->record().indexOf("UNIT");
    int vTypeIdx = m_model->record().indexOf("VTYPE");
    int vNameIdx = m_model->record().indexOf("DESCRIPTION");

    for(int modelIdx = 0; modelIdx < m_model->rowCount(); modelIdx++) {
        bool ok = false;

        QString name = m_model->record(modelIdx).value(rNameIdx).toString();
        int registerId = m_model->record(modelIdx).value(rIdIdx).toInt();
        uint address = m_model->record(modelIdx).value(rAddressIdx).toString().toUInt(&ok, 16);
        int precision = m_model->record(modelIdx).value(rPrecisionIdx).toInt();

        const QMetaType::Type type = (QMetaType::Type)m_model->record(modelIdx).value(vTypeIdx).toUInt();

        EData *data = EDataUtil::create(type);

        if (data)
            m_dataEntries->addEntry(registerId, address, data, precision, name);
    }
}

void WdgTest::updatePage()
{
    ui->grpToolBox->setTitle(m_title);

    qDeleteAll(ui->scrollAreaWidgetContents->children());

    ui->tableView->setModel(m_model);
//    ui->tableView->setColumnHidden(objectTypeNameIdx, true);
//    ui->tableView->setColumnHidden(keyListIdx, true);
//    ui->tableView->setColumnHidden(valueListIdx, true);

    WidgetFactory *wFactory = new WidgetFactory(m_core, this);

    QVBoxLayout *verticalLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    verticalLayout->setContentsMargins(80, 0, 80, 0);
    verticalLayout->setSpacing(6);

    foreach (EBusData *entry, m_dataEntries->allEntries()) {
        QGroupBox *groupBox = new QGroupBox(this);
        groupBox->setMinimumSize(QSize(0, 75));
        groupBox->setMaximumSize(QSize(16777215, 75));
        groupBox->setTitle(entry->alias());
        groupBox->setFont(QFont("Tahoma", 8, 70));

        QHBoxLayout *hLytGroup = new QHBoxLayout(groupBox);
        hLytGroup->setContentsMargins(5,5,5,5);
        hLytGroup->setSpacing(3);

        QWidget *w;
        w = wFactory->createWidget("QLabel", groupBox);

        if (w == 0) {
            qDebug() << "hata";
            return;
        }

        w->setMinimumHeight(23);
        w->setFont(QFont("Tahoma", 10));        

        QLabel *lblActual = qobject_cast<QLabel*>(w);
        lblActual->setMinimumWidth(50);
        lblActual->setText("---");

        connect(entry, &EBusData::dataChanged, lblActual, [lblActual](EData *data) {
           lblActual->setText(data->toString());
        });

        QLabel *lblUnit = new QLabel(groupBox);
        lblUnit->setMinimumSize(QSize(50, 0));
        lblUnit->setText("-");
        lblUnit->setStyleSheet("QLabel{border: 1px solid transparent;}");
        lblUnit->setFont(QFont("Tahoma", 9));

        hLytGroup->addWidget(w, 0, Qt::AlignLeft);
        hLytGroup->addWidget(lblUnit, 0, Qt::AlignLeft);

        hLytGroup->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
        verticalLayout->addWidget(groupBox);
    }

    m_dataEntries->prepareRequest();
    ui->lblRequest->setText(tr("Request: #%1#").arg(QString(m_dataEntries->requestPacket().toHex(':').toUpper())));

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
//    ui->tableView->setColumnWidth(0, 300);
    ui->tableView->setCurrentIndex(m_model->index(0, 0));

    ui->grpToolBox->show();
    ui->lblRequest->show();
    ui->lblResponse->show();
    ui->scrollArea->show();
}

void WdgTest::actualValueChanged(int value)
{
    QObject *obj = qobject_cast<QObject*>(sender());
    m_model->setData(obj->property("dataIndex").toModelIndex(), QVariant(value), Qt::EditRole);
}

void WdgTest::actualValueChanged(QString value)
{
    QObject *obj = qobject_cast<QObject*>(sender());
    m_model->setData(obj->property("dataIndex").toModelIndex(), QVariant(value), Qt::EditRole);
}

void WdgTest::on_btnStart_clicked()
{
//    m_dataEntries->openCloseRequest();
    m_dataEntries->sendRequest();
}

void WdgTest::on_btnStop_clicked()
{
    m_dataEntries->openCloseRequest();
}

void WdgTest::slResponse(const QByteArray &response)
{
    ui->lblResponse->setText(tr("Response: #%1#").arg(QString(response.toHex(':').toUpper())));

    if (response.at(3) == m_dataEntries->allEntries().count()) {

        int offset = 4;
        foreach (EBusData *entry, m_dataEntries->allEntries()) {
            const int len = QMetaType(entry->dataType()).sizeOf();
            const QByteArray ba = response.mid(offset, len);
            entry->changeData(entry->dataType(), (void*)ba.data());
            offset += len;
//                qDebug() << entry->alias() << entry->data() << QString(ba.toHex(':').toUpper()) << len;
        }
    }
    else
        ui->lblResponse->setText(tr("Response: Packet Size Incorrect!"));
}
