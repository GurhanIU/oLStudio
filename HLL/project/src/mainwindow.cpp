#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
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
#include <QDebug>

#include "initdb.h"
#include "forms/about.h"
#include "forms/settingsrtu.h"
#include "forms/settings.h"
#include "commsettings.h"
#include "infobar.h"

#include "forms/dlgregister.h"
#include "forms/dlgmodbusfunction.h"
#include "forms/dlgparameter.h"
#include "forms/dlgmenu.h"
#include "forms/dlgpageutil.h"
#include "forms/dlgpairmenupage.h"
#include "forms/dlgpairregisterpage.h"

#include "edesigner_components.h"
#include "abstractformeditor.h"
#include "menubox.h"

#include "pages/WdgTest.h"

MainWindow::MainWindow(const QStringList &args, QWidget *parent) :
    QMainWindow(parent),
    m_policy(EmitCloseEventSignal),
    ui(new Ui::MainWindow),
    m_onbusMaster(nullptr),
    m_commSettings(new CommSettings("comm.ini"))
{
    ui->setupUi(this);

    ui->leftTopToolBar->setLayoutDirection(Qt::LeftToRight);
    ui->rightTopToolBar->setLayoutDirection(Qt::RightToLeft);

    //UI - status
    m_statusInd = new QLabel;
    m_statusInd->setFixedSize( 16, 16 );
    m_statusText = new QLabel;

    m_lblRequestTraffic = new QLabel;
    m_lblRequestTraffic->setMaximumWidth(400);

    m_lblResponseTraffic = new QLabel;
    m_lblResponseTraffic->setMaximumWidth(600);

    m_lblResponseStatus = new QLabel;
    m_lblResponseStatus->setMinimumWidth(100);

    ui->statusBar->addWidget(m_statusInd);
    ui->statusBar->addWidget(m_statusText, 2);
    ui->statusBar->addWidget(m_lblRequestTraffic, 100);
    ui->statusBar->addWidget(m_lblResponseTraffic, 100);
    ui->statusBar->addWidget(m_lblResponseStatus);

//    #ifdef QT_DEBUG
    ui->factoryMenu->setEnabled(true);
//    #endif

    m_dlgModbusRTU = new SettingsRTU(this, m_commSettings);
    m_dlgSettings = new Settings(this, m_commSettings);

    m_onbusMaster = createOnBusMaster();

    connect(m_onbusMaster, &OnBusMaster::errorOccurred, [this](OnBusDevice::Error) {
        statusBar()->showMessage(m_onbusMaster->errorString(), 5000);
    });

    if (!m_onbusMaster)
        statusBar()->showMessage(tr("Could not create OnBusMaster."), 5000);
    else
        connect(m_onbusMaster, &OnBusMaster::stateChanged, this, &MainWindow::onStateChanged);

    //UI - Actions
    connect(ui->actionSerial_RTU, SIGNAL(triggered()), this, SLOT(showSettingsModbusRTU()));
    connect(ui->actionOpenFile, &QAction::triggered, this, &MainWindow::callCreateForm);
    connect(ui->actionNew_RegAddress, SIGNAL(triggered()), this, SLOT(slShowAddress()));
    connect(ui->actionNew_ModbusFunction, SIGNAL(triggered()), this, SLOT(slShowAddressType()));
    connect(ui->actionNew_Parameter, SIGNAL(triggered()), this, SLOT(slShowParameter()));
    connect(ui->actionNew_Menu, SIGNAL(triggered()), this, SLOT(slShowMenu()));
    connect(ui->actionNew_Page, SIGNAL(triggered()), this, SLOT(slShowPageUtil()));
    connect(ui->actionPair_Menu_Page, SIGNAL(triggered()), this, SLOT(slShowPairMenuPage()));
    connect(ui->actionPair_RegisterPage, &QAction::triggered, this, &MainWindow::slShowPairRegisterPage);

    //UI - dialogs
    m_dlgAbout = new About();
    connect(ui->actionAbout,SIGNAL(triggered()),m_dlgAbout,SLOT(show()));

    //UI - connections
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::changedConnect);
    connect(ui->actionOpenLogFile, SIGNAL(triggered()), this, SLOT(openLogFile()));
    connect(ui->actionLoad_Session, SIGNAL(triggered(bool)), this, SLOT(loadSession()));
    connect(ui->actionSave_Session, SIGNAL(triggered(bool)), this, SLOT(saveSession()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

    openLogFile();

    QTimer::singleShot(100, this, SLOT(callCreateForm())); // won't show anything if
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::callCreateForm()
{
    ui->actionConnect->setEnabled(false);
    ui->actionSerial_RTU->setEnabled(false);
    ui->actionOpenLogFile->setEnabled(false);
    ui->actionLoad_Session->setEnabled(false);
    ui->actionSave_Session->setEnabled(false);

    QStringList args = QApplication::arguments();

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

        QMessageBox::critical(this, "", tr("Configuration File is Corrupted!"));
        return;
    }

    resetDbActualValues(m_dbFile);

    m_core = new EDesignerFormEditorInterface();
    m_core->setDbFile(m_dbFile);
    EMenuBoxInterface *menuBox = EDesignerComponents::createMenuBox(m_core, ui->splitter);
    m_core->setMenuBox(menuBox);
    ui->splitter->addWidget(menuBox);

    WdgTest *page = new WdgTest(m_core, m_onbusMaster, ui->splitter);
    ui->splitter->addWidget(page);

    connect(m_onbusMaster, &OnBusMaster::stateChanged, page, &WdgTest::slModbusStateChanged);

    connect(menuBox, SIGNAL(pageChanged(QString,int)), page, SLOT(init(QString,int)));

    ui->actionConnect->setEnabled(true);
    ui->actionOpenFile->setEnabled(false);
    onStateChanged(OnBusMaster::UnconnectedState);
    updateStatusBar();
}

void MainWindow::openLogFile()
{
    m_logFile = QDir::currentPath() + "/db" + "/log.db";

    QSqlError dbError = initDb(m_logFile);

    if (dbError.isValid()) {
        qDebug() << m_logFile << dbError.text() ;

        QMessageBox::critical(this, "", tr("Log File is Corrupted!"));
        return;
    }
}

void MainWindow::showSettingsModbusRTU()
{
    if (m_dlgModbusRTU->exec() == QDialog::Accepted) {
//        QLOG_TRACE()<<  "RTU settings changes accepted ";
        updateStatusBar();
        m_commSettings->saveSettings();
    }
//    else
//        QLOG_WARN()<<  "RTU settings changes rejected ";

}

void MainWindow::showSettings()
{
    //Show General Settings Dialog
    m_dlgSettings->modbus_connected = false; // m_modbus->isConnected();
    if (m_dlgSettings->exec() == QDialog::Accepted) {
//        QLOG_TRACE()<<  "Settings changes accepted ";
//        m_modbus->rawModel->setMaxNoOfLines(m_modbusCommSettings->maxNoOfLines().toInt());
//        m_modbus->setTimeOut(m_modbusCommSettings->timeOut().toInt());
//        ui->sbStartAddress->setMinimum(m_modbusCommSettings->baseAddr().toInt());
        m_commSettings->saveSettings();
    }
//    else
//        QLOG_WARN()<<  "Settings changes rejected ";

    updateStatusBar();
}

void MainWindow::loadSession()
{
    QString fName;

    fName = QFileDialog::getOpenFileName(this,
                                          tr("Load Configuraion File"),
                                          "",
                                          tr("Configuraion Files (*.db);;All Files (*.*)"));
    //check
     if (fName != ""){
         m_commSettings->loadSession(fName);
         updateStatusBar();

         QMessageBox::information(this, "", tr("Load Configuraion File : ") + fName);
     }
     else
         QMessageBox::information(this, "", tr("Cancel Operation or No File Selected"));

}

void MainWindow::saveSession()
{
    QString fName = QFileDialog::getSaveFileName(this,
                                         tr("Save Configuraion file"),
                                         "",
                                         tr("Configuraion Files (*.db)"));

    if (fName != "") {
        m_commSettings->saveSession(fName);
        QMessageBox::information(this, "", tr("Save Configuraion File : ") + fName);
    }
    else
        QMessageBox::information(this, "", tr("Cancel operation or No File Selected"));

}

void MainWindow::slShowAddress()
{
    DlgRegister page(m_core);
    page.exec();
}

void MainWindow::slShowParameter()
{
    DlgParameter page(m_core);
    page.exec();
}

void MainWindow::slShowAddressType()
{
    DlgModbusFunction page(m_core);
    page.exec();
}

void MainWindow::slShowMenu()
{
    DlgMenu page(m_core);
    page.exec();
}

void MainWindow::slShowPageUtil()
{
    DlgPageUtil page(m_core);
    page.exec();
}

void MainWindow::slShowPairMenuPage()
{
    DlgPairMenuPage page(m_core);
    page.exec();
}

void MainWindow::slShowPairRegisterPage()
{
    DlgPairRegisterPage page(m_core);
    page.exec();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
//    if (m_onbusMaster->isRunning())
//        m_onbusMaster->closePort();

    switch (m_policy) {
    case AcceptCloseEvents:
        QMainWindow::closeEvent(e);
        break;
    case EmitCloseEventSignal:
        QSqlDatabase::database(m_dbFile).close();
        QSqlDatabase::removeDatabase(m_dbFile);
        emit closeEventReceived(e);
        break;
    }
}

OnBusMaster* MainWindow::createOnBusMaster()
{
    if (m_onbusMaster) {
        m_onbusMaster->disconnectDevice();
        delete m_onbusMaster;
        m_onbusMaster = nullptr;
    }

    return new OnBusRtuSerialMaster(this);
}

void MainWindow::changedConnect(bool value) //Connect - Disconnect
{
    if (value) {
        if (m_onbusMaster)
            QTimer::singleShot(30, this, SLOT(onbusConnect()));
    }
    else {
        if (m_onbusMaster)
            m_onbusMaster->disconnectDevice();

        emit sgProcessValue(0);
    }
//    m_modbus->resetCounters();
//    refreshView();
}

void MainWindow::onbusConnect() //Modbus connect - RTU/TCP
{
    if (!m_onbusMaster)
        return;

    if (m_onbusMaster->state() != OnBusDevice::ConnectedState) {
        m_onbusMaster->setConnectionParameter(OnBusDevice::SerialPortNameParameter,
                                         m_commSettings->serialPortName());
        m_onbusMaster->setConnectionParameter(OnBusDevice::SerialParityParameter,
                                         m_commSettings->parity());
        m_onbusMaster->setConnectionParameter(OnBusDevice::SerialBaudRateParameter,
                                         m_commSettings->baud());
        m_onbusMaster->setConnectionParameter(OnBusDevice::SerialDataBitsParameter,
                                         m_commSettings->dataBits());
        m_onbusMaster->setConnectionParameter(OnBusDevice::SerialStopBitsParameter,
                                         m_commSettings->stopBits());

        m_onbusMaster->setTimeout(m_commSettings->timeOut().toInt());
        m_onbusMaster->setNumberOfRetries(3);

        if (!m_onbusMaster->connectDevice())
            statusBar()->showMessage(tr("Connect failed: ") + m_onbusMaster->errorString(), 5000);
    }
    else if (m_onbusMaster->state() == OnBusMaster::ConnectedState) {
        QTimer::singleShot(500, m_onbusMaster, SLOT(close()));
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

void MainWindow::updateStatusBar()
{
    QString msg = "RTU : ";
    msg += m_commSettings->serialPortName() + " | ";
    msg += m_commSettings->baud() + ",";
    msg += m_commSettings->dataBits() + ",";
    msg += m_commSettings->stopBits() + ",";
    msg += m_commSettings->parity();

    m_statusText->setText(msg);
}

QString getLastExecutedQuery(const QSqlQuery& query)
{
    QString str = query.lastQuery();
    QMapIterator<QString, QVariant> it(query.boundValues());

    it.toBack();

    while (it.hasPrevious()) {
        it.previous();
        str.replace(it.key(),it.value().toString());
    }
    return str;
}

void MainWindow::slActualChanged(QVariant value)
{
//    ModbusData *modbusData = qobject_cast<ModbusData*>(sender());

//    if (!modbusData)
//        return;

//    QSqlDatabase db = QSqlDatabase::database("log.db");

//    QSqlQuery qry = QSqlQuery(db);
//    qry.prepare("INSERT INTO LOG (ADDRESS, VALUE, TIMESTAMP) VALUES(:address, :value, :timestamp);");

//    qry.bindValue(":address", modbusData->address());
//    qry.bindValue(":timestamp", QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss:zzz"));

//    if (modbusData->dataType() == QMetaType::SChar || modbusData->dataType() == QMetaType::Char)
//        qry.bindValue(":value", (qint8)value.toChar().unicode());// qDebug() << "AA" << value.type() << (qint8)value.toChar().unicode();
//    else if (modbusData->dataType() == QMetaType::UChar)
//        qry.bindValue(":value", (quint8)value.toChar().unicode()); // qDebug() << "BB" << value.type() << (quint8)value.toChar().unicode();
//    else
//        qry.bindValue(":value", value.toString());

//    qDebug() << getLastExecutedQuery(qry);

//    if (!qry.exec())
//        qDebug() << qry.lastError() << getLastExecutedQuery(qry);
}

void MainWindow::threadErrorOccured(OnBusMaster::Error error)
{
    switch (error) {
    case OnBusMaster::NoError:
        m_lblResponseTraffic->setText(tr("OnBusMaster::NoError"));
        break;
    case OnBusMaster::ReadError:
        m_lblResponseTraffic->setText(tr("OnBusMaster::ReadError"));
        break;
    case OnBusMaster::WriteError:
        m_lblResponseTraffic->setText(tr("OnBusMaster::WriteError"));
        break;
    case OnBusMaster::ConnectionError:
        m_lblResponseTraffic->setText(tr("OnBusMaster::ConnectionError"));
        QMessageBox::critical(this, tr("Serial Port"), tr("The Port Could Not Open!"));
        break;
    case OnBusMaster::ConfigurationError:
        m_lblResponseTraffic->setText(tr("OnBusMaster::ConfigurationError"));
        QMessageBox::critical(this, tr("Serial Port"), tr("Port Configuration Is Invalid!"));
        break;
    case OnBusMaster::TimeoutError:
        m_lblResponseTraffic->setText(tr("OnBusMaster::TimeoutError"));
        break;
    case OnBusMaster::ProtocolError:
        m_lblResponseTraffic->setText(tr("OnBusMaster::ProtocolError"));
        break;
    case OnBusMaster::ReplyAbortedError:
        m_lblResponseTraffic->setText(tr("OnBusMaster::ReplyAbortedError"));
        break;
    case OnBusMaster::UnknownError:
        m_lblResponseTraffic->setText(tr("OnBusMaster::UnknownError"));
        break;
    default:
        break;
    }
}

void MainWindow::onStateChanged(OnBusMaster::State state)
{
    bool connected = (state != OnBusMaster::UnconnectedState);

    switch (state) {
    case OnBusMaster::UnconnectedState:
        m_lblResponseTraffic->setText(tr("OnBusMaster::UnconnectedState"));
        ui->actionConnect->setText(tr("Bağlan"));
        ui->actionConnect->setChecked(false);
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-red-16.png"));
        break;
    case OnBusMaster::ConnectingState:
        m_lblResponseTraffic->setText(tr("OnBusMaster::ConnectingState"));
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-orange-16.png"));
        break;
    case OnBusMaster::ConnectedState:
        m_lblResponseTraffic->setText(tr("OnBusMaster::ConnectedState"));
        ui->actionConnect->setText(tr("Kopar "));
        ui->actionConnect->setChecked(true);
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-green-16.png"));
        break;
    case OnBusMaster::ClosingState:
        m_lblResponseTraffic->setText(tr("OnBusMaster::ClosingState"));
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-orange-16.png"));
        break;
    }

    setControlsEnabled(connected);
}

void MainWindow::setControlsEnabled(bool enable)
{
    ui->actionSerial_RTU->setEnabled(!enable);
    ui->actionLoad_Session->setEnabled(enable);
    ui->actionSave_Session->setEnabled(enable);
    ui->actionOpenLogFile->setEnabled(enable);
}

void MainWindow::threadFinished()
{
    qDebug() << Q_FUNC_INFO;
}
