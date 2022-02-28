#include "dlgactualinputregister.h"
#include "ui_dlgactualinputregister.h"

#include "abstractformeditor.h"
#include "objects/ebusdata.h"
#include "objects/ebusdataentries.h"
#include "objects/editablesqlmodel.h"

#include <QMessageBox>
#include <QModbusDevice>
#include <QModbusClient>
#include <QSql>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>

#include <QDebug>

static inline int castUIntToInt(uint16_t value)
{
    int i;

    if (value <= INT16_MAX)
        i = (int16_t)value;
    else if (value >= INT16_MIN)
        i = -(int16_t)~value -1;
    else
        i = INT16_MIN;

    return i;
}

DlgActualInputRegister::DlgActualInputRegister(EDesignerFormEditorInterface *core, QModbusClient *modbus, QWidget *parent) :
    m_core(core),
    m_modbus(modbus),
    m_scanRate(300),
    QDialog(parent),    
    ui(new Ui::DlgActualInputRegister)
{
    ui->setupUi(this);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->tableWidget->horizontalHeader()->setSectionResizeMode( QHeaderView::Interactive );
    ui->tableWidget->verticalHeader()->setVisible(true);
    ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableWidget->horizontalHeader()->setSectionsMovable(true);
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->verticalHeader()->setVisible(false);

    m_model = new QSqlQueryModel(this);
    m_model->setQuery(QString("SELECT r.ID AS ID, r.NAME, vt.ID AS VTYPE, vt.DESCRIPTION, r.ADDRESS, r.PRECISION, '---' AS VALUE, u.UNIT \
                               FROM REGISTER AS r \
                               LEFT JOIN PAGETYPE_REGISTER as pr ON r.ID = pr.REGISTER_ID \
                               LEFT JOIN VARIANT_TYPE as vt ON r.VARIANT_ID = vt.ID \                               
                               LEFT JOIN UNITS as u ON  r.UNIT_ID = u.ID \
                               WHERE  pr.PAGETYPE_ID = 1 GROUP BY r.ID ORDER BY r.ADDRESS;"), QSqlDatabase::database(m_core->dbFile()));

    if (m_model->lastError().isValid()) {
        QMessageBox::critical(this, tr("Unable to initialize Database"),
                                    tr("Error initializing database: %1").arg(m_model->lastError().text()));
        return;
    }

    while (m_model->canFetchMore())
        m_model->fetchMore();

    int rIdIdx = m_model->record().indexOf("ID");
    int rNameIdx = m_model->record().indexOf("NAME");
    int rAddressIdx = m_model->record().indexOf("ADDRESS");
    int rPrecisionIdx = m_model->record().indexOf("PRECISION");
    int rValueIdx = m_model->record().indexOf("VALUE");
    int uUnitIdx = m_model->record().indexOf("UNIT");
    int vTypeIdx = m_model->record().indexOf("VTYPE");
    int vNameIdx = m_model->record().indexOf("DESCRIPTION");

    m_dataEntries = new EBusDataEntries(m_modbus, 1, this);
    m_dataEntries->setRegisterType(QModbusDataUnit::InputRegisters);

    connect(m_dataEntries, &EBusDataEntries::sgMessage, this, &DlgActualInputRegister::sgMessage);

    ui->tableWidget->setColumnCount(m_model->columnCount());

    // m_model icin Delegate yazilabilir. Boylece for ile donmekten kurtulunur.
    for (int cIdx = 0; cIdx < m_model->columnCount(); cIdx++) {
        QTableWidgetItem *header = new QTableWidgetItem();
        header->setText(m_model->record().fieldName(cIdx));
        ui->tableWidget->setHorizontalHeaderItem(cIdx, header);
    }

    for (int rIdx = 0; rIdx < m_model->rowCount(); rIdx++) {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        int registerId = m_model->record(rIdx).value(rIdIdx).toInt();
        QString name = m_model->record(rIdx).value(rAddressIdx).toString();
        int address = m_model->record(rIdx).value(rAddressIdx).toInt();
        int precision = m_model->record(rIdx).value(rPrecisionIdx).toInt();
        uint metaTypeId = m_model->record(rIdx).value(vTypeIdx).toUInt();

        for (int cIdx = 0; cIdx < m_model->columnCount(); cIdx++) {
            QTableWidgetItem *item = new QTableWidgetItem();           
            item->setText(m_model->record(rIdx).value(cIdx).toString());
            item->setData(100, precision);
            ui->tableWidget->setItem(rIdx, cIdx, item);

            if (cIdx == rValueIdx ) {
                item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
//                EBusData *entry = m_dataEntries->addEntry(registerId, address, QVariant(metaType, nullptr), precision, name);
                EBusData *entry = m_dataEntries->addEntry(registerId, address, EDataUtil::create(metaTypeId), precision, name);

                if (entry) {
                    ///NOTE -> Kontrol
//                    entry->setRange(INT16_MIN, INT16_MAX);

                    connect(entry, &EBusData::dataChanged, [=](EData *data) {
                        Q_UNUSED(data)
                        item->setText(entry->toFormattedString());
                    });
                }
            }
        }
    }

    #ifndef QT_DEBUG
    ui->tableWidget->setColumnHidden(rIdIdx, true);
    ui->tableWidget->setColumnHidden(vNameIdx, true);
    ui->tableWidget->setColumnHidden(vTypeIdx, true);
    ui->tableWidget->setColumnHidden(rAddressIdx, true);
    ui->tableWidget->setColumnHidden(rPrecisionIdx, true);
    #endif

    ui->tableWidget->resizeColumnsToContents();
//    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

DlgActualInputRegister::~DlgActualInputRegister()
{
    emit sgVisibilityChanged(false);
    qDebug() << this << "Siliniyor.";
    delete ui;
}

void DlgActualInputRegister::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);

    m_dataEntries->startReadContinuously(m_scanRate);

    emit sgVisibilityChanged(true);
}

void DlgActualInputRegister::hideEvent(QHideEvent *e)
{
    QDialog::hideEvent(e);

    m_dataEntries->stopReadContinuously();

    emit sgVisibilityChanged(false);
}

void DlgActualInputRegister::slChangeScanRate(int scanRate)
{
    m_scanRate = scanRate;
}


