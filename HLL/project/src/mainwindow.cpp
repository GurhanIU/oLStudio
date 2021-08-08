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

#include "forms/dlgaddress.h"
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
#include "objects/modbusdata.h"
#include "objects/modbusdataentries.h"

static int reqCount = 0;
static int resCount = 0;

MainWindow::MainWindow(const QStringList &args, QWidget *parent) :
    QMainWindow(parent),
    m_policy(EmitCloseEventSignal),
    ui(new Ui::MainWindow),
    m_thread(nullptr),
    m_commSettings(new CommSettings("comm.ini"))
{
    ui->setupUi(this);

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

    m_thread = createMasterThread();

    m_modbusEntries = new ModbusDataEntries(this);

    connect(m_thread, &MasterThread::errorOccurred, this, &MainWindow::threadErrorOccured);
    connect(m_thread, &MasterThread::stateChanged, this, &MainWindow::threadStateChanged);
    connect(m_thread, &MasterThread::finished, this, &MainWindow::threadFinished);
    connect(m_thread, &MasterThread::response, this, &MainWindow::showResponse);
    connect(m_thread, &MasterThread::request, this, &MainWindow::showRequest);

    connect(m_modbusEntries, &ModbusDataEntries::sgProcessRange, this, &MainWindow::sgProcessRange);
    connect(m_modbusEntries, &ModbusDataEntries::sgProcessValue, this, &MainWindow::sgProcessValue);

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

    WdgTest *page = new WdgTest(m_core, m_modbusEntries, ui->splitter);
    ui->splitter->addWidget(page);

    connect(m_thread, &MasterThread::stateChanged, page, &WdgTest::slModbusStateChanged);
    connect(page, &WdgTest::sgRequest,  this, &MainWindow::slRequest);
    connect(this, &MainWindow::sgResponse, page, &WdgTest::slResponse);

//    connect(menuBox, SIGNAL(pageChanged(QString,int)), this, SLOT(slPageChanged(QString,int)));
    connect(menuBox, SIGNAL(pageChanged(QString,int)), page, SLOT(slUpdateModelByPageId(QString,int)));

    collectRegisters();

    ui->actionConnect->setEnabled(true);
    ui->actionOpenFile->setEnabled(false);
    threadStateChanged(MasterThread::UnconnectedState);
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
    if (m_dlgSettings->exec()==QDialog::Accepted) {
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

void MainWindow::changedScanRate(int value)
{
    //Enable-Disable Time Interval
//    QLOG_TRACE()<<  "ScanRate changed. Value = " << value;
    m_commSettings->setScanRate(value);
    m_commSettings->saveSettings();

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
     if (fName != ""){
         m_commSettings->loadSession(fName);
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
         QMessageBox::information(this, "", tr("Load Configuraion File : ") + fName);
     }
     else
         QMessageBox::information(this, "", tr("Cancel Operation or No File Selected"));

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
     if (fName != "")
     {
         m_commSettings->saveSession(fName);
         QMessageBox::information(this, "", tr("Save Configuraion File : ") + fName);
     }
     else
         QMessageBox::information(this, "", tr("Cancel operation or No File Selected"));

}

void MainWindow::slShowAddress()
{
    DlgAddress page(m_core);
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

void MainWindow::slPageChanged(const QString &name, const int &id)
{
    //    qDebug() << id << name;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
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

MasterThread* MainWindow::createMasterThread()
{
    if (m_thread)
        return m_thread;

    MasterThread *mt = new MasterThread();

    return mt;
}

void MainWindow::closeMasterThread()
{
    if (m_thread)
        m_thread->closePort();
}

void MainWindow::collectRegisters()
{
    QSqlQuery qry = QSqlQuery(QSqlDatabase::database(m_dbFile));
    qry.prepare("SELECT r.ID AS rID, r.NAME, vt.ID AS VTYPE, vt.DESCRIPTION, r.ADDRESS, r.PRECISION, '0' AS ACTUAL, u.UNIT \
                 FROM REGISTER AS r \
                 LEFT JOIN VARIANT_TYPE as vt ON r.VARIANT_ID = vt.ID \
                 LEFT JOIN MODBUS_FUNCTION as mf ON r.MODBUS_FUNC_ID = mf.ID \
                 LEFT JOIN UNITS as u ON  r.UNIT_ID = u.ID \
                 WHERE r.ID IN ( SELECT REGISTER_ID FROM PAGE_REGISTER ) GROUP BY r.ID ORDER BY r.ADDRESS;");

    if (!qry.exec()) {
        qDebug() << qry.lastError() << qry.lastQuery();
        return;
    }

    int rIdIdx = qry.record().indexOf("rID");
    int rNameIdx = qry.record().indexOf("NAME");
    int rAddressIdx = qry.record().indexOf("ADDRESS");
    int rActualIdx = qry.record().indexOf("ACTUAL");
    int vTypeIdx = qry.record().indexOf("VTYPE");

    while (qry.next()) {
        const uint actual = qry.value(rActualIdx).toInt();
        bool ok = false;
        ModbusData *entry = m_modbusEntries->addEntry(qry.value(rIdIdx).toInt(),
                                                      qry.value(rAddressIdx).toString().toUInt(&ok, 16),
                                                      QVariant(qry.value(vTypeIdx).toUInt(), (void*)&actual),
                                                      qry.value(rNameIdx).toString());

        if (entry) {
//            connect(entry, &ModbusData::valueChanged, this, &MainWindow::slActualChanged);
            connect(entry, &ModbusData::valueChanged, this, static_cast<void (MainWindow::*)(QVariant)>(&MainWindow::slActualChanged));
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

    while (it.hasPrevious())
    {
        it.previous();
        str.replace(it.key(),it.value().toString());
    }
    return str;
}

void MainWindow::slActualChanged(QVariant value)
{
    ModbusData *modbusData = qobject_cast<ModbusData*>(sender());

    if (!modbusData)
        return;

    QSqlDatabase db = QSqlDatabase::database("log.db");

    QSqlQuery qry = QSqlQuery(db);
    qry.prepare("INSERT INTO LOG (ADDRESS, VALUE, TIMESTAMP) VALUES(:address, :value, :timestamp);");

    qry.bindValue(":address", modbusData->address());
    qry.bindValue(":timestamp", QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss:zzz"));

    if (modbusData->dataType() == QMetaType::SChar || modbusData->dataType() == QMetaType::Char)
        qry.bindValue(":value", (qint8)value.toChar().unicode());// qDebug() << "AA" << value.type() << (qint8)value.toChar().unicode();
    else if (modbusData->dataType() == QMetaType::UChar)
        qry.bindValue(":value", (quint8)value.toChar().unicode()); // qDebug() << "BB" << value.type() << (quint8)value.toChar().unicode();
    else
        qry.bindValue(":value", value.toString());

    qDebug() << getLastExecutedQuery(qry);

    if (!qry.exec())
        qDebug() << qry.lastError() << getLastExecutedQuery(qry);
}

void MainWindow::threadErrorOccured(MasterThread::Error error)
{
    switch (error) {
    case MasterThread::NoError:
//        qDebug() << "MasterThread::NoError";
        break;
    case MasterThread::ReadError:
//        qDebug() << "MasterThread::ReadError";
        break;
    case MasterThread::WriteError:
//        qDebug() << "MasterThread::WriteError";
        break;
    case MasterThread::ConnectionError:
//        qDebug() << "MasterThread::ConnectionError";
        QMessageBox::critical(this, tr("Serial Port"), tr("The Port Could Not Open!"));
        break;
    case MasterThread::ConfigurationError:
//        qDebug() << "MasterThread::ConfigurationError";
        QMessageBox::critical(this, tr("Serial Port"), tr("Port Configuration Is Invalid!"));
        break;
    case MasterThread::TimeoutError:
//        qDebug() << "MasterThread::TimeoutError";
        break;
    case MasterThread::ProtocolError:
//        qDebug() << "MasterThread::ProtocolError";
        break;
    case MasterThread::ReplyAbortedError:
//        qDebug() << "MasterThread::ReplyAbortedError";
        break;
    case MasterThread::UnknownError:
//        qDebug() << "MasterThread::UnknownError";
        break;
    default:
        break;
    }
}

void MainWindow::threadStateChanged(MasterThread::State state)
{
    bool connected = (state != MasterThread::UnconnectedState);

    switch (state) {
    case MasterThread::UnconnectedState:
        qDebug() << "MasterThread::UnconnectedState";
        ui->actionConnect->setText(tr("Bağlan"));
        ui->actionConnect->setChecked(false);
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-red-16.png"));
        break;
    case MasterThread::ConnectingState:
        qDebug() << "MasterThread::ConnectingState";
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-orange-16.png"));
        break;
    case MasterThread::ConnectedState:
        qDebug() << "MasterThread::ConnectedState";
        ui->actionConnect->setText(tr("Kopar "));
        ui->actionConnect->setChecked(true);
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-green-16.png"));
        break;
    case MasterThread::ClosingState:
        qDebug() << "MasterThread::ClosingState";
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-orange-16.png"));
        break;
    }

//    ui->actionScan->setChecked(false);
    setControlsEnabled(connected);
}

void MainWindow::setControlsEnabled(bool enable)
{
    qDebug() << enable;
    ui->actionSerial_RTU->setEnabled(!enable);
    ui->actionLoad_Session->setEnabled(enable);
    ui->actionSave_Session->setEnabled(enable);
    ui->actionOpenLogFile->setEnabled(enable);
}

void MainWindow::changedConnect(bool value) //Connect - Disconnect
{
    if (!m_thread)
        return;

    setControlsEnabled(false);
    m_lblRequestTraffic->clear();
//    m_lblResponseTraffic->setText(tr("Port Status: %1").arg("..."));

    if (m_thread->state() == MasterThread::UnconnectedState) {

        if (m_thread->setConfig(m_commSettings->serialPortName(), QSerialPort::Baud115200,
                                QSerialPort::NoParity, QSerialPort::Data8, QSerialPort::OneStop, 100)) {
            m_thread->openPort();
            QTimer::singleShot(100, this, SLOT(openCloseDevice()));
        }
    }
    else if (m_thread->state() == MasterThread::ConnectedState) {
        openCloseDevice();
        QTimer::singleShot(500, m_thread, SLOT(closePort()));
    }
}

void MainWindow::openCloseDevice()
{
    QByteArray data; // cihaz tarafinda haberlesmeyi acar
    data.append((char) 0xAA);
    data.append((char) 0x03);
    data.append((char) 0x03);
    data.append((char) 0x00);
    data.append((char) 0x00);
    data.append((char) 0x00);
    data.append((char) 0x00);
    data.append((char) 0x55);
    m_thread->transaction(data);
}

void MainWindow::threadFinished()
{
    qDebug() << Q_FUNC_INFO;
}

void MainWindow::showRequest(const QByteArray &data)
{
    m_lblRequestTraffic->setText(tr("Request: %1 #%2#").arg(++reqCount).arg(QString(data.toHex(':').toUpper())));
}

void MainWindow::showResponse(const QByteArray &data)
{
    ResponsePacket *packet = new ResponsePacket;

    connect(packet, &ResponsePacket::responseStatus, [this](const QString &status){
       m_lblResponseStatus->setText(tr("Status: %1").arg(status));
    });

    packet->setPacket(data);
    packet->deleteLater();
    m_lblResponseTraffic->setText(tr("Response: %1 #%2#").arg(++resCount).arg(QString(data.toHex(':').toUpper())));
    emit sgResponse(data);
}

void MainWindow::slRequest(const QByteArray &data)
{
    m_thread->transaction(data);
}

