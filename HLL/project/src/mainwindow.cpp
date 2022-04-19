#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QModbusClient>
#include <QModbusRtuSerialMaster>
#include <QWindowList>
#include <QWindow>
//#include <QtConcurrent>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QMdiSubWindow>
#include <QAction>
#include <QDockWidget>
#include <QMimeData>
#include <QCloseEvent>
#include <QProgressBar>
#include <QTimer>
#include <QVariant>
#include <QDateTime>

#include <QDebug>

#include "version.h"
#include "initdb.h"

#include "modbuscommsettings.h"
#include "infobar.h"

#include "pages/WdgTest.h"

#include "forms/about.h"
#include "forms/settingsmodbusrtu.h"
#include "forms/settings.h"
#include "forms/toptoolbox.h"
#include "forms/entrytoolbox.h"

#include "objects/ebusdata.h"
#include "objects/ebusdataentries.h"

namespace TestCode {
// ------- Test Code------------------//
///
/// \brief The A class copy ctor is and copy assignment deleted by compiler.
///
class A {
public:
    A() = default;
    A(A&&) = default;
    A& operator=(A &&) = default;

    int k() const { return m_k; }
    void setK(int k) { m_k = k; }

private:
    int m_k;
};

///
/// \brief The Myclass class have a A m_ax variable that init by an A object with move semantics.
///
class Myclass {
public:
    Myclass(A x) : m_ax(std::move(x)) {}
private:
    A m_ax;
};

void testCode(int y)
{
    A a;
    a.setK(5);
    auto f = [x = std::move(a)](int y) { qDebug() << "Test Code:" << __PRETTY_FUNCTION__ << x.k() * y;};

    f(y);
}

// ------- Test Code------------------//
};

QVariantMap toVMap(const EBusData *data)
{
    QVariantMap v;
    v.insert("registerId", data->registerId());
    v.insert("data", data->data());
    v.insert("timeStamp", data->timeStamp());
    return v;
}

void fromVMap(const QVariantMap &qvm)
{
    /*url_        =*/ qvm.contains("registerId") ? qvm.value("registerId").toInt() : 0;
    /*uuid_       =*/ qvm.contains("data") ? qvm.value("data") : QVariant();
    /*metadata_   =*/ qvm.contains("timeStamp") ? qvm.value("timeStamp").toLongLong() : 0;
}

// ---------------- MainWindowBase

MainWindowBase::MainWindowBase(QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    m_policy(AcceptCloseEvents)
{
#ifndef Q_WS_MAC
    setWindowIcon(windowIcon());
#endif
}

void MainWindowBase::closeEvent(QCloseEvent *e)
{
    switch (m_policy) {
    case AcceptCloseEvents:
        QMainWindow::closeEvent(e);
        break;
      case EmitCloseEventSignal:
        emit closeEventReceived(e);
        break;
    }
}

//QList<QToolBar *>  MainWindowBase::createToolBars(const EDesignerActions *actions, bool singleToolBar)
//{
//    // Note that whenever you want to add a new tool bar here, you also have to update the default
//    // action groups added to the toolbar manager in the mainwindow constructor
//    QList<QToolBar *> rc;
//    if (singleToolBar)
//    {
//        //: Not currently used (main tool bar)
//        QToolBar *main = createToolBar(tr("Main"), QLatin1String("mainToolBar"), actions->fileActions()->actions());
//        addActionsToToolBar(actions->editActions()->actions(), main);
//        addActionsToToolBar(actions->toolActions()->actions(), main);
//        addActionsToToolBar(actions->formActions()->actions(), main);
//        rc.push_back(main);
//    } else {
//        rc.push_back(createToolBar(tr("File"), QLatin1String("fileToolBar"), actions->fileActions()->actions()));
//        rc.push_back(createToolBar(tr("Edit"), QLatin1String("editToolBar"),  actions->editActions()->actions()));
//        rc.push_back(createToolBar(tr("Tools"), QLatin1String("toolsToolBar"), actions->toolActions()->actions()));
//        rc.push_back(createToolBar(tr("Form"), QLatin1String("formToolBar"), actions->formActions()->actions()));
//    }
//    return rc;
//}

//QString MainWindowBase::mainWindowTitle()
//{
//    return trUtf8("Company Configuration Suite");
//}

// Use the minor Qt version as settings versions to avoid conflicts
int MainWindowBase::settingsVersion()
{
    const int version = QT_VERSION;
    return (version & 0x00FF00) >> 8;
}

// ----------------- DockedMdiArea
static const char *uriListMimeFormatC = "text/uri-list";

DockedMdiArea::DockedMdiArea(const QString &extension, QWidget *parent) :
    QMdiArea(parent),
    m_extension(extension)
{
    setAcceptDrops(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

QStringList DockedMdiArea::uiFiles(const QMimeData *d) const
{
    // Extract dropped UI files from Mime data.
    QStringList rc;
    if (!d->hasFormat(QLatin1String(uriListMimeFormatC)))
        return rc;
    const QList<QUrl> urls = d->urls();
    if (urls.empty())
        return rc;
    const QList<QUrl>::const_iterator cend = urls.constEnd();
    for (QList<QUrl>::const_iterator it = urls.constBegin(); it != cend; ++it) {
        const QString fileName = it->toLocalFile();
        if (!fileName.isEmpty() && fileName.endsWith(m_extension))
            rc.push_back(fileName);
    }
    return rc;
}

bool DockedMdiArea::event(QEvent *event)
{
    // Listen for desktop file manager drop and emit a signal once a file is
    // dropped.
    switch (event->type()) {
    case QEvent::DragEnter: {
        QDragEnterEvent *e = static_cast<QDragEnterEvent*>(event);
        if (!uiFiles(e->mimeData()).empty())
        {
            e->acceptProposedAction();
            return true;
        }
    }
        break;
    case QEvent::Drop: {
        QDropEvent *e = static_cast<QDropEvent*>(event);
        const QStringList files = uiFiles(e->mimeData());
        const QStringList::const_iterator cend = files.constEnd();
        for (QStringList::const_iterator it = files.constBegin(); it != cend; ++it) {
            emit fileDropped(*it);
        }
        e->acceptProposedAction();
        return true;
    }
        break;
    default:
        break;
    }
    return QMdiArea::event(event);
}

// ----------------- Eski

MainWindow::MainWindow(const QStringList &args, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_policy(EmitCloseEventSignal),
    m_modbus(nullptr),
    m_modbusCommSettings(new  ModbusCommSettings("qModMaster.ini"))
{
    ui->setupUi(this);

    TestCode::testCode(6);

    setWindowTitle(QString("%1").arg(VER_PRODUCTNAME_STR));

    m_modbus = createModbusClient();

    connect(m_modbus, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        statusBar()->showMessage(m_modbus->errorString(), 5000);
    });

    if (!m_modbus)
        statusBar()->showMessage(tr("Could not create Modbus master."), 5000);
    else
        connect(m_modbus, &QModbusClient::stateChanged, this, &MainWindow::onStateChanged);

    m_modbusEntries = new EBusDataEntries(m_modbus, 1, this);
    m_modbusEntries->setRegisterType(QModbusDataUnit::HoldingRegisters);

    connect(m_modbusEntries, &EBusDataEntries::sgProcessRange, this, &MainWindow::sgProcessRange);
    connect(m_modbusEntries, &EBusDataEntries::sgProcessValue, this, &MainWindow::sgProcessValue);
//    connect(m_modbusEntries, &EBusDataEntries::sgMessage, [=] (QString caption, QString text) {
//            statusBar()->showMessage(QString("%1: %2").arg(caption).arg(text), 1000);
//    });

    connect(m_modbusEntries, &EBusDataEntries::sgMessage, this, &MainWindow::slUpdateStatusBar);

    //UI - Custom
    connect(ui->actionOpenFile, &QAction::triggered, this, &MainWindow::callCreateForm);

    //UI - dialogs
    m_dlgAbout = new About();
    connect(ui->actionAbout,SIGNAL(triggered()),m_dlgAbout,SLOT(show()));

    m_dlgModbusRTU = new SettingsModbusRTU(this,m_modbusCommSettings);
    connect(ui->actionSerial_RTU, SIGNAL(triggered()), this, SLOT(showSettingsModbusRTU()));

//    connect(ui->actionSettings,SIGNAL(triggered()),this,SLOT(showSettings()));
//    m_busMonitor = new BusMonitor(this, m_modbus->rawModel);
//    connect(ui->actionBus_Monitor,SIGNAL(triggered()),this,SLOT(showBusMonitor()));
//    m_tools = new Tools(this, m_modbus, m_modbusCommSettings);
//    connect(ui->actionTools,SIGNAL(triggered()),this,SLOT(showTools()));

    //UI - connections
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::changedConnect);
    connect(ui->actionWriteToDevice, SIGNAL(triggered()), this, SLOT(slWriteAllToDevice()));
    connect(ui->actionReadFromDevice, SIGNAL(triggered()), this,SLOT(slReadAllFromDevice()));

    connect(ui->actionLoad_Session, SIGNAL(triggered(bool)), this, SLOT(loadSession()));
    connect(ui->actionSave_Session, SIGNAL(triggered(bool)), this, SLOT(saveSession()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

    //UI - status
    m_statusInd = new QLabel;
    m_statusInd->setFixedSize( 16, 16 );
    m_statusText = new QLabel;
    m_baseAddr = new QLabel;

    QProgressBar *progressBar = new QProgressBar(ui->statusBar);
    progressBar->setFixedWidth(200);
    progressBar->setToolTip(tr("Percent of Process"));
    progressBar->setTextVisible(true);
    progressBar->setAlignment(Qt::AlignCenter);
    progressBar->setRange(0,100);
    progressBar->setValue(0);
    progressBar->setStyleSheet("QProgressBar::chunk { background-color: #007de5; }");

    connect(this, &MainWindow::sgProcessRange, progressBar, &QProgressBar::setRange);
    connect(this, &MainWindow::sgProcessValue, progressBar, &QProgressBar::setValue);

    ui->statusBar->addWidget(m_statusInd, 1);
    ui->statusBar->addWidget(m_statusText, 10);
    ui->statusBar->addWidget(m_baseAddr, 10);
    ui->statusBar->addPermanentWidget(progressBar,10);

    QTimer::singleShot(100, this, SLOT(callCreateForm())); // won't show anything if suppressed
}

MainWindow::~MainWindow()
{
    m_modbusEntries->deleteAll();
    delete ui;
}

void MainWindow::callCreateForm()
{
    ui->actionConnect->setEnabled(false);
    ui->actionSerial_RTU->setEnabled(false);
    ui->actionPersistentEventLog->setEnabled(false);
    ui->actionFactoryCalibration->setEnabled(false);
    ui->actionWriteToDevice->setEnabled(false);
    ui->actionReadFromDevice->setEnabled(false);
    ui->actionLoad_Session->setEnabled(false);
    ui->actionSave_Session->setEnabled(false);
    ui->actionScan->setEnabled(false);

    QStringList args = QApplication::arguments();

    qDebug() << args;

    QString selectedFileName;
    if (args.count() == 2) {
        const QString dFile = args.at(1);

        QFileInfo fInfo(dFile);
        if (fInfo.isFile()) {
            if (fInfo.suffix() == "db")
                selectedFileName = fInfo.absoluteFilePath();
        }
    }

    if (selectedFileName.isEmpty()) {
        QFileDialog fDialog(this);
        QString filter = QString("%1 (*.db)").arg(tr("Configuration File"));

        selectedFileName = QFileDialog::getOpenFileName(this, tr("Open Configuration File"), QDir::currentPath() + "/db", filter);
    }

    if (selectedFileName.isEmpty())
        return;

    QFileInfo fInfo(selectedFileName);
    m_dbFile = fInfo.fileName();

    QSqlError dbError = initDb(selectedFileName);

    if (dbError.isValid()) {
        qDebug() << dbError.text();

        QMessageBox::critical(this, VER_COMPANYSHORTNAME_STR, tr("Configuration File is Corrupted!"));
        return;
    }

    setWindowTitle(QString("%1 - [%2]").arg(VER_PRODUCTNAME_STR).arg(m_dbFile));

    resetDbActualValues(m_dbFile);

    TopToolBox *topToolBox = new TopToolBox(this);
    ui->centralwidget->layout()->addWidget(topToolBox);

    WdgTest *page = new WdgTest(m_modbusEntries, this);
    ui->centralwidget->layout()->addWidget(page);

    EntryToolBox *entryBox = new EntryToolBox(this);
    ui->centralwidget->layout()->addWidget(entryBox);

    connect(topToolBox, &TopToolBox::sgStartStopToggled, this, &MainWindow::slStartStop);
    connect(topToolBox, &TopToolBox::sgScanRateChanged, this, &MainWindow::slSaveScanRate);
    connect(this, &MainWindow::sgUpdateTopToolBox, topToolBox, &TopToolBox::slChangeScanRate);
    connect(this, &MainWindow::sgRegistersCollected, page, &WdgTest::slUpdateModel);
    connect(entryBox, &EntryToolBox::sgCollectRegisters, this, &MainWindow::slCollectRegisters);

    connect(m_modbus, &QModbusClient::stateChanged, page, &WdgTest::slModbusStateChanged);

    slCollectRegisters();

    ui->actionConnect->setEnabled(true);
    ui->actionOpenFile->setEnabled(false);
    onStateChanged(QModbusDevice::UnconnectedState);
    updateStatusBar();
    emit sgUpdateTopToolBox(m_modbusCommSettings->scanRate());
}

void MainWindow::showSettingsModbusRTU()
{
    if (m_dlgModbusRTU->exec() == QDialog::Accepted) {
//        QLOG_TRACE()<<  "RTU settings changes accepted ";
        updateStatusBar();
        m_modbusCommSettings->saveSettings();
        emit sgUpdateTopToolBox(m_modbusCommSettings->scanRate());
    }
//    else
//        QLOG_WARN()<<  "RTU settings changes rejected ";

}

void MainWindow::slSaveScanRate(int value)
{
    //Enable-Disable Time Interval
//    QLOG_TRACE()<<  "ScanRate changed. Value = " << value;
    m_modbusCommSettings->setScanRate(value);
    m_modbusCommSettings->saveSettings();

//    m_modbus->setScanRate(value);
}

void MainWindow::loadSession()
{
    QString fName;

//     QLOG_TRACE()<<  "load session";
     fName = QFileDialog::getOpenFileName(this,
                                          tr("Load Configuraion File"),
                                          "",
                                          tr("Configuraion Files (*.db);;All Files (*.*)"));
    //check
     if (fName != "") {
         m_modbusCommSettings->loadSession(fName);
         //Update UI
//         ui->sbStartAddress->setMinimum(m_modbusCommSettings->baseAddr().toInt());
//         ui->cmbBase->setCurrentIndex(m_modbusCommSettings->base());
//         ui->cmbFunctionCode->setCurrentIndex(m_modbusCommSettings->functionCode());
//         ui->cmbModbusMode->setCurrentIndex(m_modbusCommSettings->modbusMode());
//         ui->sbSlaveID->setValue(m_modbusCommSettings->slaveID());
//         ui->spInterval->setValue(m_modbusCommSettings->scanRate());
//         ui->sbStartAddress->setValue(m_modbusCommSettings->startAddr());
//         ui->sbNoOfRegs->setValue(m_modbusCommSettings->noOfRegs());
         updateStatusBar();
//         refreshView();
         QMessageBox::information(this, VER_COMPANYSHORTNAME_STR, tr("Load Configuraion File : ") + fName);
     }
     else
         QMessageBox::information(this, VER_COMPANYSHORTNAME_STR, tr("Cancel Operation or No File Selected"));

}

void MainWindow::saveSession()
{
    QString fName;

//     QLOG_TRACE()<<  "save session";
     fName = QFileDialog::getSaveFileName(this,
                                          tr("Save Configuraion file"),
                                          "",
                                          tr("Configuraion Files (*.db)"));

     //check
     if (fName != "") {
         m_modbusCommSettings->saveSession(fName);
         QMessageBox::information(this, VER_COMPANYSHORTNAME_STR, tr("Save Configuraion File : ") + fName);
     }
     else
         QMessageBox::information(this, VER_COMPANYSHORTNAME_STR, tr("Cancel operation or No File Selected"));

}

void MainWindow::closeEvent(QCloseEvent *e)
{
    switch (m_policy) {
    case AcceptCloseEvents:
        QMainWindow::closeEvent(e);
        break;
      case EmitCloseEventSignal:
        QWindowList windows = QGuiApplication::topLevelWindows();

        for (int i = 0; i < windows.size(); ++i) {
            QWindow *window =  windows.at(i);

            if (window->objectName() != "MainWindowWindow") {
                window->close();
//                delete window;
            }
        }

        disconnectAndDelete();
        QSqlDatabase::database(m_dbFile).close();
        QSqlDatabase::removeDatabase(m_dbFile);
        emit closeEventReceived(e);
        break;
    }
}

QModbusClient * MainWindow::createModbusClient()
{
    if (m_modbus) {
        m_modbus->disconnectDevice();
        delete m_modbus;
        m_modbus = nullptr;
    }

    return new QModbusRtuSerialMaster(this);
}

void MainWindow::disconnectAndDelete()
{
    if (!m_modbus)
        return;

    m_modbus->disconnectDevice();
    m_modbus->disconnect();
    delete m_modbus;
    m_modbus = nullptr;
}

void MainWindow::changedConnect(bool value) //Connect - Disconnect
{
    if (value) {
        if (m_modbus)
            QTimer::singleShot(30, this, SLOT(modbusConnect()));
    } else {
        if (m_modbus)
            m_modbus->disconnectDevice();

        emit sgProcessValue(0);
    }
//    m_modbus->resetCounters();
//    refreshView();
}

void MainWindow::modbusConnect() //Modbus connect - RTU/TCP
{
    if (!m_modbus)
        return;

    if (m_modbus->state() != QModbusDevice::ConnectedState) {
        m_modbus->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
                                         m_modbusCommSettings->serialPortName());
        m_modbus->setConnectionParameter(QModbusDevice::SerialParityParameter,
                                         m_modbusCommSettings->parity());
        m_modbus->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
                                         m_modbusCommSettings->baud());
        m_modbus->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
                                         m_modbusCommSettings->dataBits());
        m_modbus->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
                                         m_modbusCommSettings->stopBits());

        m_modbus->setTimeout(m_modbusCommSettings->timeOut().toInt());
        m_modbus->setNumberOfRetries(3);

        if (!m_modbus->connectDevice())
            statusBar()->showMessage(tr("Connect failed: ") + m_modbus->errorString(), 5000);
    } else {
        m_modbus->disconnectDevice();
        emit sgProcessValue(0);
//        ui->actionConnect->setEnabled(true);
//        ui->actionDisconnect->setEnabled(false);
    }
}

void MainWindow::slCollectRegisters()
{
    m_modbusEntries->deleteAll();

    QSqlQuery qry = QSqlQuery(QSqlDatabase::database(m_dbFile));

    qry.prepare("SELECT r.ID AS rID, r.NAME, r.ADDRESS, r.PRECISION, u.UNIT, vt.ID AS VTYPE \
                FROM REGISTER AS r \
                LEFT JOIN VARIANT_TYPE as vt ON r.VARIANT_ID = vt.ID \
                LEFT JOIN MODBUS_FUNCTION AS mf ON r.MODBUS_FUNC_ID = mf.ID \
                LEFT JOIN UNITS as u ON  r.UNIT_ID = u.ID \
               GROUP BY r.ID ORDER BY r.ADDRESS;");

    if (!qry.exec()) {
        QMessageBox::critical(this, tr("Unable to initialize Database"),
                                    tr("Error initializing database: %1").arg(qry.lastError().text()));
        qDebug() << qry.lastError() << qry.lastQuery();
        return;
    }

    int rIdIdx = qry.record().indexOf("rID");
    int rNameIdx = qry.record().indexOf("NAME");
    int rAddressIdx = qry.record().indexOf("ADDRESS");
    int rPrecisonIdx = qry.record().indexOf("PRECISION");
    int uUnitIdx = qry.record().indexOf("UNIT");
    int vTypeIdx = qry.record().indexOf("VTYPE");

//    EDataFactory dataFactory;
//    dataFactory.RegisterAllTypes();

    while (qry.next()) {
        EBusData *entry = m_modbusEntries->addEntry(qry.value(rIdIdx).toInt(),
                                                    qry.value(rAddressIdx).toInt(),
                                                    static_cast<QVariant::Type>(qry.value(vTypeIdx).toUInt()),
                                                    qry.value(rPrecisonIdx).toInt(),
                                                    qry.value(rNameIdx).toString(),
                                                    qry.value(uUnitIdx).toString());

//        if (entry)
//        connect(entry, &EBusData::dataChanged, [entry](const QVariant &data){ // IIFE idiom
        connect(entry, &EBusData::dataChanged, this, &MainWindow::slInsertLog);
//        connect(entry, &EBusData::dataChanged, [entry](const QVariant &data){
//            qDebug() << QDateTime::fromMSecsSinceEpoch(entry->timeStamp()).toString("hh:mm:ss") << data;
//        });
    }

    emit sgRegistersCollected();
}

void MainWindow::slStartStop(bool toggle)
{
    if (toggle) {
        if (!m_queue.isEmpty())
            m_queue.clear();
        m_modbusEntries->startReadContinuously(m_modbusCommSettings->scanRate());
    } else {
        m_modbusEntries->stopReadContinuously();

        QSqlQuery qry = QSqlQuery(QSqlDatabase::database(m_dbFile));

        qry.prepare("INSERT INTO LOG ( REGISTER_ID, VALUE,  TIMESTAMP) "
                             "VALUES (:REGISTERID, :VALUE, :TIMESTAMP);");

        while (!m_queue.isEmpty()) {
            QVariantMap vMap = m_queue.dequeue();

            qry.bindValue(":REGISTERID", vMap.contains("registerId") ? vMap.value("registerId").toInt() : 0);
            qry.bindValue(":VALUE", vMap.contains("data") ? vMap.value("data") : QVariant());
            qry.bindValue(":TIMESTAMP", vMap.contains("timeStamp") ? vMap.value("timeStamp").toLongLong() : 0);

            if(!qry.exec())
                qDebug() << QString("SQL Last Query:%1 \n\r Last Error: %2").arg(qry.lastQuery()).arg(qry.lastError().text());
        }
    }
}

QString MainWindow::dbFile() const
{
    return m_dbFile;
}

void MainWindow::setDbFile(const QString &dbFile)
{
    m_dbFile = dbFile;
}

void MainWindow::slWriteAllToDevice()
{
    m_modbusEntries->writeAllEntries();
}

void MainWindow::slReadAllFromDevice()
{
    m_modbusEntries->readAllEntries();
}

void MainWindow::onStateChanged(int state)
{
    bool connected = (state != QModbusDevice::UnconnectedState);

    switch (state) {
    case QModbusDevice::UnconnectedState:
        qDebug() << "QModbusDevice::UnconnectedState";
        ui->actionConnect->setText(tr("Bağlan"));
        ui->actionConnect->setChecked(false);
        ui->actionAlarms->setChecked(false);
        ui->actionAlarms->setEnabled(false);
        m_baseAddr->setText(tr("RTU: Bağlanmadı"));
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-red-16.png"));
        break;
    case QModbusDevice::ConnectingState:
        qDebug() << "QModbusDevice::ConnectingState";
        m_baseAddr->setText(tr("RTU: Bağlanıyor"));
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-orange-16.png"));
        break;
    case QModbusDevice::ConnectedState:
        qDebug() << "QModbusDevice::ConnectedState";
        ui->actionConnect->setText(tr("Kopar "));
        m_baseAddr->setText(tr("RTU: Bağlandı"));
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-green-16.png"));
        break;
    case QModbusDevice::ClosingState:
        qDebug() << "QModbusDevice::ClosingState";
        m_baseAddr->setText(tr("RTU: Bağlantı Kesiliyor"));
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-orange-16.png"));
        break;
    }

    ui->actionLoad_Session->setEnabled(false);
    ui->actionSave_Session->setEnabled(false);
    ui->actionScan->setEnabled(connected);
//    ui->actionPersistentEventLog->setEnabled(connected);
    ui->actionFactoryCalibration->setEnabled(false); // connected
    ui->actionSerial_RTU->setEnabled(!connected);
}

void MainWindow::updateStatusBar()
{
    QString msg;
    { //RTU
        msg = "RTU : ";
        msg += m_modbusCommSettings->serialPortName() + " | ";
        msg += m_modbusCommSettings->baud() + ",";
        msg += m_modbusCommSettings->dataBits() + ",";
        msg += m_modbusCommSettings->stopBits() + ",";
        msg += m_modbusCommSettings->parity();
    }

    m_statusText->setText(msg);
}

void MainWindow::slUpdateStatusBar(const QString &caption, const QString &text, int timeout)
{
    timeout = timeout == 0 ? 5000 : timeout;
    statusBar()->showMessage(QString("%1: %2").arg(caption).arg(text), timeout);
}

void MainWindow::slInsertLog(const QVariant &data)
{
    Q_UNUSED(data)
    if (EBusData *entry = static_cast<EBusData*>(sender())) {
        QVariantMap vMap = toVMap(entry);
//        qDebug() << vMap;

        m_queue.enqueue(vMap);
    }
}
