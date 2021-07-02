#include "dlgpairmenupage.h"
#include "ui_dlgpairmenupage.h"

#include "abstractformeditor.h"

#include <QCheckBox>
#include <QDataWidgetMapper>
#include <QSql>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
//#include <QSqlTableDelegate>
#include <QModelIndexList>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

DlgPairMenuPage::DlgPairMenuPage(EDesignerFormEditorInterface *core, QWidget *parent) :
    m_core(core),
    QDialog(parent),
    ui(new Ui::DlgPairMenuPage)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());

    // Menu Initilization
    ui->tblMenu->setAlternatingRowColors(true);
    ui->tblMenu->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblMenu->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblMenu->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->tblMenu->horizontalHeader()->setStretchLastSection(true);
    ui->tblMenu->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tblMenu->verticalHeader()->setVisible(true);

    ui->tblMenu->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblMenu->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblMenu->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tblMenu->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    QSqlTableModel *mdlMenu = new QSqlTableModel(this, db);
    mdlMenu->setTable("MENU");
    mdlMenu->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mdlMenu->setHeaderData(mdlMenu->fieldIndex("NAME"), Qt::Horizontal, trUtf8("MENU NAMES"));

    // Populate the model
    if (!mdlMenu->select())
    {
        showError(mdlMenu->lastError());
        return;
    }

    ui->tblMenu->setModel(mdlMenu);
    ui->tblMenu->setColumnHidden(mdlMenu->fieldIndex("ID"), true);
    ui->tblMenu->setColumnHidden(mdlMenu->fieldIndex("OPERATION_ID"), true);
    ui->tblMenu->resizeColumnsToContents();

//    QAbstractItemModel *model = ui->tblMenu->model();
//    QModelIndexList matches = mdlMenu->match( mdlMenu->index(0,0), Qt::UserRole, "ID001" );

//    foreach( const QModelIndex &index, matches )
//    {
//        QTableWidgetItem *item = ui->tblMenu->item( index.row(), index.column() );
//        // Do something with your new-found item ...
//    }

    // Page Initilization
    ui->tblPage->setAlternatingRowColors(true);
    ui->tblPage->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblPage->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblPage->setSelectionMode( QAbstractItemView::MultiSelection );
    ui->tblPage->horizontalHeader()->setStretchLastSection(true);
    ui->tblPage->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tblPage->verticalHeader()->setVisible(true);

    ui->tblPage->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblPage->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblPage->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tblPage->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    QSqlQueryModel *mdlPage = new QSqlQueryModel(ui->tblPage);
    ui->tblPage->setModel(mdlPage);

    slUpdateModelByPageId(1);
}

DlgPairMenuPage::~DlgPairMenuPage()
{
    delete ui;
}


void DlgPairMenuPage::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}

void DlgPairMenuPage::on_tblMenu_clicked(const QModelIndex &index)
{

}

void DlgPairMenuPage::slUpdateModelByPageId(const int &id)
{
    QSqlDatabase db = QSqlDatabase::database(m_core->dbFile());

    QSqlQueryModel *model = qobject_cast<QSqlQueryModel*>(ui->tblPage->model());
    model->setQuery(QString("SELECT m.NAME AS MENU, p.NAME AS PAGE  FROM MENU_PAGE AS mp \
                            INNER JOIN MENU as m ON m.ID = mp.MENU_ID \
                            INNER JOIN PAGE as p ON p.ID = mp.PAGE_ID \
                            WHERE m.ID =%1").arg(id), db);

    model->insertColumn(0); // For Checkbox

    if (model->lastError().isValid())
    {
        showError(model->lastError());
        return;
    }

    for (int p = 0; p < model->rowCount(); p++)
    {
        ui->tblPage ->setIndexWidget(model ->index(p,0),new QCheckBox());
    }

    model->setHeaderData(0, Qt::Horizontal, trUtf8("#"));
    model->setHeaderData(model->record().indexOf("PAGE"), Qt::Horizontal, trUtf8("PAGE NAMES"));
    ui->tblPage->setColumnHidden(model->record().indexOf("MENU"), true);

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

//    ui->tblPage->setCurrentIndex(model->index(0, 0));
}
