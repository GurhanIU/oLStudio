#include "dlgpairmenupage.h"
#include "ui_dlgpairmenupage.h"

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

DlgPairMenuPage::DlgPairMenuPage(EDesignerFormEditorInterface *core, QWidget *parent) :
    m_core(core),
    m_menuId(-1),
    m_dataChanged(false),
    QDialog(parent),
    ui(new Ui::DlgPairMenuPage)
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Discard)->setEnabled(false);

    // Menu Initilization
    ui->tblMenu->setAlternatingRowColors(true);
    ui->tblMenu->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblMenu->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblMenu->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->tblMenu->resizeColumnsToContents();

    ui->tblMenu->horizontalHeader()->setStretchLastSection(true);
    ui->tblMenu->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tblMenu->verticalHeader()->setVisible(false);

    ui->tblMenu->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblMenu->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblMenu->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tblMenu->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

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

    QSqlTableModel *mdlMenu = menuTableModel(ui->tblMenu);
    connect(ui->tblMenu->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &DlgPairMenuPage::slMenuCurrentRowChanged);

    QSqlQueryModel *mdlPage = new QSqlQueryModel(ui->tblPage);
    ui->tblPage->setModel(mdlPage);

    connect(this, &DlgPairMenuPage::sgMenuChanged, this, &DlgPairMenuPage::slUpdatePageModel);
}

DlgPairMenuPage::~DlgPairMenuPage()
{
    delete ui;
}

void DlgPairMenuPage::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);

    ui->tblMenu->setCurrentIndex(menuTableModel(ui->tblMenu)->index(0, 0));
}

void DlgPairMenuPage::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}

void DlgPairMenuPage::on_DlgPairMenuPage_finished(int result)
{
    Q_UNUSED(result)

    if( m_dataChanged )
        emit ui->buttonBox->button(QDialogButtonBox::Close)->clicked();
}

QSqlTableModel* DlgPairMenuPage::menuTableModel(QTableView *view)
{
    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(view->model());

    if (!model) {
        QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());
        model = new QSqlTableModel(view, db);
        model->setTable("MENU");

        if (!model->select()) {
            showError(model->lastError());
            return model;
        }

        model->setHeaderData(model->fieldIndex("NAME"), Qt::Horizontal, trUtf8("MENU"));
        view->setModel(model);
        view->hideColumn(model->fieldIndex("ID")); // ui->tblPage->setColumnHidden(model->fieldIndex("ID"), true);
        view->hideColumn(model->fieldIndex("OPERATION_ID"));
    }

    return model;
}

void DlgPairMenuPage::slMenuCurrentRowChanged(const QModelIndex &current, const QModelIndex &previus)
{
    Q_UNUSED(previus)

    if (!current.isValid())
        return;

    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(ui->tblMenu->model());
    int menuId = model->data(model->index(current.row(), model->fieldIndex("ID"))).toInt();

    if (m_menuId != menuId) {
        m_menuId = menuId;
        emit sgMenuChanged(menuId);
    }
}

void DlgPairMenuPage::slUpdatePageModel(int menuId)
{
    if (menuId < 1)
        return;

    ui->tblPage->reset();
    qDeleteAll(findChildren<QCheckBox*>());

    const QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());
    QSqlQueryModel *model = qobject_cast<QSqlQueryModel*>(ui->tblPage->model());
    model->clear();
    model->setQuery(QString("SELECT p.ID AS ID, p.NAME AS NAME, '0' AS STATUS  \
                                FROM PAGE AS p \
                                LEFT JOIN MENU_PAGE AS mp ON mp.PAGE_ID = p.ID \
                                WHERE mp.PAGE_ID ISNULL \
                            UNION \
                                SELECT p.ID AS ID, p.NAME AS NAME, '1' AS STATUS \
                                FROM PAGE AS p \
                                INNER JOIN MENU_PAGE AS mp ON mp.PAGE_ID = p.ID \
                                WHERE mp.MENU_ID = '%1' \
                            GROUP BY p.ID ORDER BY STATUS DESC, p.ID ASC;").arg(menuId), db);

    if (model->lastError().isValid()) {
        showError(model->lastError());
        return;
    }

    model->insertColumn(0); // For Checkbox
    int ckbIdx = 0;
    int rIdIdx = model->record().indexOf("ID");
    int rNameIdx = model->record().indexOf("NAME");
    int stsIdx = model->record().indexOf("STATUS");

    model->setHeaderData(ckbIdx, Qt::Horizontal, trUtf8("PAGE"));
    ui->tblPage->setColumnHidden(rNameIdx, true);
    ui->tblPage->setColumnHidden(rIdIdx, true);
    ui->tblPage->setColumnHidden(stsIdx, true);

    for (int row = 0; row < model->rowCount(); row++) {
        QCheckBox *ckb = new QCheckBox(model->record(row).value(rNameIdx).toString(), this);
        ckb->setProperty("menuId", menuId);
        ckb->setProperty("pageId", model->record(row).value(rIdIdx).toInt());
        ckb->setChecked(model->record(row).value(stsIdx).toBool());

        connect(ckb, &QCheckBox::stateChanged, [=](int state) {
            QString text;

            if (ckb->checkState() == Qt::Unchecked) {
                text = QString("DELETE FROM MENU_PAGE WHERE MENU_ID = '%1' AND PAGE_ID = '%2';")
                                .arg(ckb->property("menuId").toInt())
                                .arg(ckb->property("pageId").toInt());
            }
            else {
                text = QString("INSERT INTO MENU_PAGE (MENU_ID, PAGE_ID) VALUES ('%1', '%2');")
                                .arg(ckb->property("menuId").toInt())
                                .arg(ckb->property("pageId").toInt());
            }

            QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());
            db.transaction();
            QSqlQuery qry = QSqlQuery(db);
            if (!qry.exec(text))
                showError(qry.lastError());

            this->changeButtonsStatus(true);
        });

        ui->tblPage->setIndexWidget(model->index(row, ckbIdx), ckb);
    }

    ui->tblPage->viewport()->update();
    ui->tblPage->setVisible(false);
    QRect vporig = ui->tblPage->viewport()->geometry();
    QRect vpnew = vporig;
    vpnew.setWidth(std::numeric_limits<int>::max());
    ui->tblPage->viewport()->setGeometry(vpnew);
    ui->tblPage->resizeColumnsToContents();
    ui->tblPage->resizeRowsToContents();
    ui->tblPage->viewport()->setGeometry(vporig);
    ui->tblPage->setVisible(true);
    ui->tblPage->update();
}

void DlgPairMenuPage::on_buttonBox_clicked(QAbstractButton *button)
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

QMessageBox::StandardButton DlgPairMenuPage::comfirm()
{
    QMessageBox::StandardButton ret = QMessageBox::NoButton;
    if (m_dataChanged)
        ret = QMessageBox::warning(this, tr("The document has been modified."),
                                     tr("Do you want to apply your changes?"), QMessageBox::Apply | QMessageBox::Discard /*| QMessageBox::Cancel*/ );
    return ret;
}

void DlgPairMenuPage::apply()
{
    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());
    db.commit();

    changeButtonsStatus(false);
    emit sgMenuChanged(m_menuId);
}

void DlgPairMenuPage::discard()
{
    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());
    db.rollback();

    changeButtonsStatus(false);
    emit sgMenuChanged(m_menuId);
}

void DlgPairMenuPage::changeButtonsStatus(bool status)
{
    m_dataChanged = status;
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(status);
    ui->buttonBox->button(QDialogButtonBox::Discard)->setEnabled(status);
}
