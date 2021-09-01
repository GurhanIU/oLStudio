#include "dlgpairregisterpage.h"
#include "ui_dlgpairregisterpage.h"

#include "abstractformeditor.h"

#include <QSql>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlTableModel>

#include <QCheckBox>
#include <QPushButton>

DlgPairRegisterPage::DlgPairRegisterPage(EDesignerFormEditorInterface *core, QWidget *parent) :
    m_core(core),
    m_pageId(-1),
    m_dataChanged(false),
    QDialog(parent),
    ui(new Ui::DlgPairRegiserPage)
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Discard)->setEnabled(false);

    // Page Initilization
    ui->tblPage->setAlternatingRowColors(true);
    ui->tblPage->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblPage->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblPage->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->tblPage->resizeColumnsToContents();

    ui->tblPage->horizontalHeader()->setStretchLastSection(true);
    ui->tblPage->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tblPage->verticalHeader()->setVisible(false);

    ui->tblPage->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblPage->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblPage->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tblPage->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    // Register Initilization
    ui->tblRegister->setAlternatingRowColors(true);
    ui->tblRegister->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblRegister->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblRegister->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->tblRegister->resizeColumnsToContents();
    ui->tblRegister->horizontalHeader()->setStretchLastSection(true);
    ui->tblRegister->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tblRegister->verticalHeader()->setVisible(false);

    ui->tblRegister->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblRegister->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblRegister->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tblRegister->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(ui->tblPage, &QTableView::clicked, this, &DlgPairRegisterPage::slTablePageClicked);
    connect(this, &DlgPairRegisterPage::sgPageChanged, this, &DlgPairRegisterPage::slUpdateRegisterModel);

    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());
    QSqlTableModel *mdlPage = new QSqlTableModel(ui->tblPage, db);
    mdlPage->setTable("PAGE");
    ui->tblPage->setModel(mdlPage);

    QSqlQueryModel *mdlRegister = new QSqlQueryModel(ui->tblRegister);
    ui->tblRegister->setModel(mdlRegister);
}

DlgPairRegisterPage::~DlgPairRegisterPage()
{
    delete ui;
}

void DlgPairRegisterPage::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}

void DlgPairRegisterPage::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);

    slUpdatePageModel();
}

void DlgPairRegisterPage::on_DlgPairRegiserPage_finished(int result)
{
    if( m_dataChanged )
        emit ui->buttonBox->button(QDialogButtonBox::Close)->clicked();
}

void DlgPairRegisterPage::slUpdatePageModel()
{
    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(ui->tblPage->model());

    // Populate the model
    if (!model->select()) {
        showError(model->lastError());
        return;
    }

    model->setHeaderData(model->fieldIndex("NAME"), Qt::Horizontal, trUtf8("PAGES"));
    ui->tblPage->setColumnHidden(model->fieldIndex("ID"), true);
}

void DlgPairRegisterPage::slTablePageClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QSqlTableModel *mdlPage = qobject_cast<QSqlTableModel*>(ui->tblPage->model());
    int pageId = mdlPage->data(mdlPage->index(index.row(), mdlPage->fieldIndex("ID"))).toInt();

    if (m_pageId != pageId) {
        m_pageId = pageId;
        emit sgPageChanged(pageId);
    }
}

void DlgPairRegisterPage::slUpdateRegisterModel(int pageId)
{
    if (pageId < 1)
        return;

    ui->tblRegister->reset();
    qDeleteAll(findChildren<QCheckBox*>());

    const QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());
    QSqlQueryModel *model = qobject_cast<QSqlQueryModel*>(ui->tblRegister->model());
    model->clear();
    model->setQuery(QString("SELECT rID AS ID, NAME, SUM(STATUS) AS STATUS \
                            FROM ( \
                                SELECT r.ID AS rID, r.NAME, '1' AS STATUS   \
                                    FROM REGISTER AS r \
                                    INNER JOIN PAGE_REGISTER AS pr ON pr.REGISTER_ID = r.ID \
                                    WHERE pr.PAGE_ID = %1 \
                                UNION \
                                SELECT ID AS rID, NAME, '0' AS STATUS \
                                    FROM REGISTER \
                            ) \
                            GROUP BY ID ORDER BY STATUS DESC, ID ASC;").arg(pageId), db);

    if (model->lastError().isValid()) {
        showError(model->lastError());
        return;
    }

    model->insertColumn(0); // For Checkbox
    int ckbIdx = 0;
    int rIdIdx = model->record().indexOf("ID");
    int rNameIdx = model->record().indexOf("NAME");
    int stsIdx = model->record().indexOf("STATUS");

    model->setHeaderData(ckbIdx, Qt::Horizontal, trUtf8("REGISTER"));
    ui->tblRegister->setColumnHidden(rNameIdx, true);
    ui->tblRegister->setColumnHidden(rIdIdx, true);
    ui->tblRegister->setColumnHidden(stsIdx, true);

    for (int row = 0; row < model->rowCount(); row++) {
        QCheckBox *ckb = new QCheckBox(model->record(row).value(rNameIdx).toString(), this);
        ckb->setProperty("pageId", pageId);
        ckb->setProperty("regId", model->record(row).value(rIdIdx).toInt());
        ckb->setChecked(model->record(row).value(stsIdx).toBool());

        connect(ckb, &QCheckBox::stateChanged, [=](int state) {
            QString text;

            if (ckb->checkState() == Qt::Unchecked) {
                text = QString("DELETE FROM PAGE_REGISTER WHERE PAGE_ID = '%1' AND REGISTER_ID = '%2';")
                                .arg(ckb->property("pageId").toInt())
                                .arg(ckb->property("regId").toInt());
            }
            else {
                text = QString("INSERT INTO PAGE_REGISTER (PAGE_ID, REGISTER_ID) VALUES ('%1', '%2');")
                                .arg(ckb->property("pageId").toInt())
                                .arg(ckb->property("regId").toInt());
            }

            QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());
            db.transaction();
            QSqlQuery qry = QSqlQuery(db);
            if (!qry.exec(text))
                showError(qry.lastError());

            this->changeButtonsStatus(true);
        });

        ui->tblRegister->setIndexWidget(model->index(row, ckbIdx), ckb);
    }

    ui->tblRegister->viewport()->update();
    ui->tblRegister->setVisible(false);
    QRect vporig = ui->tblRegister->viewport()->geometry();
    QRect vpnew = vporig;
    vpnew.setWidth(std::numeric_limits<int>::max());
    ui->tblRegister->viewport()->setGeometry(vpnew);
    ui->tblRegister->resizeColumnsToContents();
    ui->tblRegister->resizeRowsToContents();
    ui->tblRegister->viewport()->setGeometry(vporig);
    ui->tblRegister->setVisible(true);
    ui->tblRegister->update();
}

void DlgPairRegisterPage::on_buttonBox_clicked(QAbstractButton *button)
{
    if ((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Apply))
        apply();
    else if ((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Discard))
        discard();
    else {
        comfirm() == QMessageBox::Apply ? apply() : discard();
        done(0);
    }
}

QMessageBox::StandardButton DlgPairRegisterPage::comfirm()
{
    QMessageBox::StandardButton ret = QMessageBox::NoButton;
    if (m_dataChanged)
        ret = QMessageBox::warning(this, tr("The document has been modified."),
                                     tr("Do you want to apply your changes?"), QMessageBox::Apply | QMessageBox::Discard /*| QMessageBox::Cancel*/ );
    return ret;
}

void DlgPairRegisterPage::apply()
{
    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());
    db.commit();

    changeButtonsStatus(false);
    emit sgPageChanged(m_pageId);
}

void DlgPairRegisterPage::discard()
{
    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());
    db.rollback();

    changeButtonsStatus(false);
    emit sgPageChanged(m_pageId);
}

void DlgPairRegisterPage::changeButtonsStatus(bool status)
{
    m_dataChanged = status;
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(status);
    ui->buttonBox->button(QDialogButtonBox::Discard)->setEnabled(status);
}