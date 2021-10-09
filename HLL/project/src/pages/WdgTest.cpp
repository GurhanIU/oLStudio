#include "WdgTest.h"
#include "ui_WdgTest.h"

#include "abstractformeditor.h"
#include "widgetfactory_p.h"
#include "objects/editablesqlmodel.h"
#include "onbus/ondata.h"
#include "onbus/onbusdata.h"
#include "onbus/onbusmaster.h"
//#include "objects/modbusdata.h"
//#include "objects/modbusdataentries.h"

#include "responsepacket.h"

#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QSql>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QDebug>

namespace Testing {
QByteArray prepareRequest(const QList<OnBusData*> dataset)
{
    if (dataset.isEmpty())
            return QByteArray();
    QByteArray packet;
    packet.append((char)0xAA);
    packet.append((char)0x01); // Paket tipine gore faklilik gosterecek
    packet.append((char)(dataset.count()*5 + 1)); // 5: Her veri icin 4byte adres bilgisi ve 1 adet boyut bilgisi; 1: toplam veri adedi
    packet.append((char)dataset.count()); // toplam veri adedi

    char chkSum = (char)dataset.count();

    foreach (OnBusData *data, dataset ) {
        QMetaType t(data->dataType());
        packet.append((char)data->address());
        packet.append((char)(data->address() >> 8));
        packet.append((char)(data->address() >> 16));
        packet.append((char)(data->address() >> 24));
        packet.append((char)t.sizeOf());

        chkSum += (char)data->address();
        chkSum += (char)(data->address() >> 8);
        chkSum += (char)(data->address() >> 16);
        chkSum += (char)(data->address() >> 24);
        chkSum += (char)t.sizeOf();
    }

    packet.append(chkSum);
    packet.append((char) 0x55);

    return packet;
}
} // namespace Testing

WdgTest::WdgTest(EDesignerFormEditorInterface *core, OnBusMaster *onBusMaster, QWidget *parent) :
    QWidget(parent),
    m_core(core),
    m_busMaster(onBusMaster),
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

//    m_dataEntries = new ModbusDataEntries(onBusMaster, this);
    m_watchEntries.setRegisterType(OnBusDataUnit::Read);
}

WdgTest::~WdgTest()
{
    qDebug() << "Siliniyor:" << this;
    delete ui;
}

void WdgTest::slModbusStateChanged(int state)
{
    bool connected = (state == OnBusDevice::ConnectedState);

    ui->btnStart->setEnabled(connected);
    ui->btnStop->setEnabled(connected);
}

void WdgTest::init(const QString &title, int id)
{
    m_title = title;
    m_pageId = id;

    initModel(id);
    collectEntries();
}

void WdgTest::initModel(int pageId)
{
    if (m_model)
        delete m_model;

    m_model = new EditableSqlModel(QSqlDatabase::database(m_core->dbFile()), ui->tableView);
    m_model->setQuery(QString("SELECT r.ID AS ID, r.NAME, vt.ID AS VTYPE, vt.DESCRIPTION, r.ADDRESS, r.PRECISION, '---' AS ACTUAL, u.UNIT, mf.FUNCTION \
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
    ui->grpToolBox->setTitle(m_title);
    m_watchEntries.clear();
    qDeleteAll(ui->scrollAreaWidgetContents->children());

    ui->tableView->setModel(m_model);

    int rIdIdx = m_model->record().indexOf("rID");
    int rNameIdx = m_model->record().indexOf("NAME");
    int rAddressIdx = m_model->record().indexOf("ADDRESS");
    int rPrecisionIdx = m_model->record().indexOf("PRECISION");
    int rActualIdx = m_model->record().indexOf("ACTUAL");
    int uUnitIdx = m_model->record().indexOf("UNIT");
    int vTypeIdx = m_model->record().indexOf("VTYPE");
    int vNameIdx = m_model->record().indexOf("DESCRIPTION");
    int mFunctionIdx = m_model->record().indexOf("FUNCTION");

    WidgetFactory *wFactory = new WidgetFactory(m_core, this);

    QVBoxLayout *verticalLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    verticalLayout->setContentsMargins(80, 0, 80, 0);
    verticalLayout->setSpacing(6);

    for(int modelIdx = 0; modelIdx < m_model->rowCount(); modelIdx++) {
        bool ok = false;

        QString name = m_model->record(modelIdx).value(rNameIdx).toString();
        int registerId = m_model->record(modelIdx).value(rIdIdx).toInt();
        uint address = m_model->record(modelIdx).value(rAddressIdx).toString().toUInt(&ok, 16);
        int precision = m_model->record(modelIdx).value(rPrecisionIdx).toInt();
        QStringList functionList = m_model->record(modelIdx).value(mFunctionIdx).toString().split(";");

        const QMetaType::Type type = (QMetaType::Type)m_model->record(modelIdx).value(vTypeIdx).toUInt();

        OnData *data = OnDataUtil::create(type);
        OnBusData *entry = nullptr;
        if (data) {
            entry = new OnBusData(registerId, address, data, precision, name);
            m_watchEntries.addData(entry);
        }
        else {
            continue;
        }

        QGroupBox *groupBox = new QGroupBox(this);
        groupBox->setMinimumSize(QSize(0, 75));
        groupBox->setMaximumSize(QSize(16777215, 75));
        groupBox->setTitle(entry->alias());
        groupBox->setFont(QFont("Tahoma", 8, 70));

        QHBoxLayout *hLytGroup = new QHBoxLayout(groupBox);
        hLytGroup->setContentsMargins(5,5,5,5);
        hLytGroup->setSpacing(3);

        foreach (QString fc, functionList) {
            bool isOk = false;

            uint iFc = fc.toUInt(&isOk, 16);

            if (!isOk)
                continue;

            QWidget *w = nullptr;
            OnBusRequest::FunctionCode code = (OnBusRequest::FunctionCode)fc.toUInt(&isOk, 16);

            switch (code) {
            case OnBusRequest::WriteMemory: {
                hLytGroup->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
                QLineEdit *editBox = new QLineEdit(groupBox);
                QPushButton *btnSend = new QPushButton(tr("Gönder"),groupBox);

                w = qobject_cast<QWidget*>(editBox);
                hLytGroup->addWidget(w, 0, Qt::AlignRight);
                hLytGroup->addWidget(btnSend, 0, Qt::AlignRight);

                entry->setMode((OnBusData::Mode) entry->mode() | OnBusData::WriteOnly);
            }   break;
            case OnBusRequest::ReadConfig: {
                QLabel *lblActual = new QLabel("---", groupBox);

                connect(entry, &OnBusData::dataChanged, lblActual, [lblActual](OnData *data) {
                   lblActual->setText(data->toString());
                });

                w = qobject_cast<QWidget*>(lblActual);
                hLytGroup->addWidget(w, 0, Qt::AlignLeft);

                QLabel *lblUnit = new QLabel(groupBox);
                lblUnit->setMinimumSize(QSize(50, 0));
                lblUnit->setText(m_model->record(modelIdx).value(uUnitIdx).toString());
                lblUnit->setStyleSheet("QLabel{border: 1px solid transparent;}");
                lblUnit->setFont(QFont("Tahoma", 9));

                hLytGroup->addWidget(lblUnit, 0, Qt::AlignLeft);
                hLytGroup->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

                entry->setMode((OnBusData::Mode) entry->mode() | OnBusData::ReadOnly);
            }    break;
            default:
                break;
            }

            if (w == 0) {
                qDebug() << "hata";
                continue;
            }

            w->setMinimumWidth(50);
            w->setMinimumHeight(25);
            w->setFont(QFont("Tahoma", 10));
        }

        verticalLayout->addWidget(groupBox);
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
//    ui->tableView->setColumnWidth(0, 300);
    ui->tableView->setCurrentIndex(m_model->index(0, 0));

    ui->grpToolBox->show();
    ui->lblRequest->show();
    ui->lblResponse->show();
    ui->scrollArea->show();

    ui->lblRequest->setText(tr("Request: #%1#").arg(QString(Testing::prepareRequest(m_watchEntries.dataset()).toHex(':').toUpper())));
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
    m_busMaster->sendReadRequest(m_watchEntries);
}

void WdgTest::on_btnStop_clicked()
{
    OnBusDataUnit unit(OnBusDataUnit::Command);
    unit.addData(new OnBusData(0, 0, new OnDataShort((short)0), 0, QString("1")));
    unit.addData(new OnBusData(0, 0, new OnDataShort((short)0), 0, QString("2")));
    unit.addData(new OnBusData(0, 0, new OnDataShort((short)0), 0, QString("3")));
    m_busMaster->sendCommandRequest(unit);
}

void WdgTest::slResponse(const QByteArray &response)
{
//    ui->lblResponse->setText(tr("Response: #%1#").arg(QString(response.toHex(':').toUpper())));

//    if (response.at(3) == m_dataEntries->allEntries().count()) {

//        int offset = 4;
//        foreach (OnBusData *entry, m_dataEntries->allEntries()) {
//            const int len = QMetaType(entry->dataType()).sizeOf();
//            const QByteArray ba = response.mid(offset, len);
//            entry->changeData(entry->dataType(), (void*)ba.data());
//            offset += len;
////                qDebug() << entry->alias() << entry->data() << QString(ba.toHex(':').toUpper()) << len;
//        }
//    }
//    else
//        ui->lblResponse->setText(tr("Response: Packet Size Incorrect!"));
}
