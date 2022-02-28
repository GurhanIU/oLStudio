#include "dlgpairtyperegister.h"
#include "ui_dlgpairtyperegister.h"

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

DlgPairTypeRegister::DlgPairTypeRegister(EDesignerFormEditorInterface *core, QWidget *parent) :
    m_core(core),
    m_typeId(-1),
    m_dataChanged(false),
    QDialog(parent),
    ui(new Ui::DlgPairTypeRegister)
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Discard)->setEnabled(false);

    // Page Initilization
    ui->tblType->setAlternatingRowColors(true);
    ui->tblType->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblType->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblType->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->tblType->resizeColumnsToContents();

    ui->tblType->horizontalHeader()->setStretchLastSection(true);
    ui->tblType->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tblType->verticalHeader()->setVisible(false);

    ui->tblType->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblType->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblType->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tblType->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

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

    QSqlTableModel *mdlType = typeTableModel(ui->tblType);
    connect(ui->tblType->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &DlgPairTypeRegister::slPageCurrentRowChanged);

    QSqlQueryModel *mdlRegister = new QSqlQueryModel(ui->tblRegister);
    ui->tblRegister->setModel(mdlRegister);

    connect(this, &DlgPairTypeRegister::sgTypeChanged, this, &DlgPairTypeRegister::slUpdateRegisterModel);
}

DlgPairTypeRegister::~DlgPairTypeRegister()
{
    delete ui;
}

void DlgPairTypeRegister::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);

    ui->tblType->setCurrentIndex(typeTableModel(ui->tblType)->index(0, 0));
}

void DlgPairTypeRegister::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}

void DlgPairTypeRegister::on_DlgPairTypeRegister_finished(int result)
{
    if( m_dataChanged )
        emit ui->buttonBox->button(QDialogButtonBox::Close)->clicked();
}

QSqlTableModel* DlgPairTypeRegister::typeTableModel(QTableView *view)
{
    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(view->model());

    if (!model) {
        QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());
        model = new QSqlTableModel(view, db);
        model->setTable("PAGETYPE");

        if (!model->select()) {
            showError(model->lastError());
            return model;
        }

        model->setHeaderData(model->fieldIndex("NAME"), Qt::Horizontal, trUtf8("PAGE TYPE"));
        view->setModel(model);
        view->hideColumn(model->fieldIndex("ID")); // ui->tblPage->setColumnHidden(model->fieldIndex("ID"), true);
    }

    return model;
}

void DlgPairTypeRegister::slPageCurrentRowChanged(const QModelIndex &current, const QModelIndex &previus)
{
    Q_UNUSED(previus);
    if (!current.isValid())
        return;

    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(ui->tblType->model());
    int typeId = model->data(model->index(current.row(), model->fieldIndex("ID"))).toInt();

    if (m_typeId != typeId) {
        m_typeId = typeId;
        emit sgTypeChanged(typeId);
    }
}

void DlgPairTypeRegister::slUpdateRegisterModel(int pageId)
{
    if (pageId < 1)
        return;

    ui->tblRegister->reset();
    qDeleteAll(findChildren<QCheckBox*>());

    const QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());
    QSqlQueryModel *model = qobject_cast<QSqlQueryModel*>(ui->tblRegister->model());
    model->clear();
//    model->setQuery(QString("SELECT rID AS ID, NAME, SUM(STATUS) AS STATUS \
//                            FROM ( \
//                                SELECT r.ID AS rID, r.NAME, '1' AS STATUS   \
//                                    FROM REGISTER AS r \
//                                    INNER JOIN PAGETYPE_REGISTER AS pr ON pr.REGISTER_ID = r.ID \
//                                    WHERE pr.PAGETYPE_ID = %1 \
//                                UNION \
//                                SELECT ID AS rID, NAME, '0' AS STATUS \
//                                    FROM REGISTER \
//                            ) \
//                            GROUP BY ID ORDER BY STATUS DESC, ID ASC;").arg(pageId), db);
    model->setQuery(QString("SELECT r.ID AS ID, r.NAME AS NAME, '0' AS STATUS  \
                                FROM REGISTER AS r \
                                LEFT JOIN PAGETYPE_REGISTER AS pr ON pr.REGISTER_ID = r.ID \
                                WHERE pr.REGISTER_ID ISNULL \
                            UNION \
                                SELECT r.ID AS ID, r.NAME AS NAME, '1' AS STATUS \
                                FROM REGISTER AS r \
                                INNER JOIN PAGETYPE_REGISTER AS pr ON pr.REGISTER_ID = r.ID \
                                WHERE pr.PAGETYPE_ID = '%1' \
                            GROUP BY r.ID ORDER BY STATUS DESC, r.ID ASC;").arg(pageId), db);


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

    while (model->canFetchMore())
        model->fetchMore();

    for (int row = 0; row < model->rowCount(); row++) {
        QCheckBox *ckb = new QCheckBox(model->record(row).value(rNameIdx).toString(), this);
        ckb->setProperty("pageId", pageId);
        ckb->setProperty("regId", model->record(row).value(rIdIdx).toInt());
        ckb->setChecked(model->record(row).value(stsIdx).toBool());

        connect(ckb, &QCheckBox::stateChanged, [=](int state) {
            QString text;

            if (ckb->checkState() == Qt::Unchecked) {
                text = QString("DELETE FROM PAGETYPE_REGISTER WHERE PAGETYPE_ID = '%1' AND REGISTER_ID = '%2';")
                                .arg(ckb->property("pageId").toInt())
                                .arg(ckb->property("regId").toInt());
            }
            else {
                text = QString("INSERT INTO PAGETYPE_REGISTER (PAGETYPE_ID, REGISTER_ID) VALUES ('%1', '%2');")
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

void DlgPairTypeRegister::on_buttonBox_clicked(QAbstractButton *button)
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

QMessageBox::StandardButton DlgPairTypeRegister::comfirm()
{
    QMessageBox::StandardButton ret = QMessageBox::NoButton;
    if (m_dataChanged)
        ret = QMessageBox::warning(this, tr("The document has been modified."),
                                     tr("Do you want to apply your changes?"), QMessageBox::Apply | QMessageBox::Discard /*| QMessageBox::Cancel*/ );
    return ret;
}

void DlgPairTypeRegister::apply()
{
    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());
    db.commit();

    changeButtonsStatus(false);
    emit sgTypeChanged(m_typeId);
}

void DlgPairTypeRegister::discard()
{
    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());
    db.rollback();

    changeButtonsStatus(false);
    emit sgTypeChanged(m_typeId);
}

void DlgPairTypeRegister::changeButtonsStatus(bool status)
{
    m_dataChanged = status;
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(status);
    ui->buttonBox->button(QDialogButtonBox::Discard)->setEnabled(status);
}
