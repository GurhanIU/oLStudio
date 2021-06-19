#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QDebug>

#include "commsettings.h"
#include "settingsrtu.h"

static int count = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_commSettings(new CommSettings("comm.ini"))
    , m_thread(new MasterThread())
{
    ui->setupUi(this);

    //UI - status
    m_statusInd = new QLabel;
    m_statusInd->setFixedSize( 16, 16 );
    m_statusText = new QLabel;
    m_baseAddr = new QLabel;
    m_baseAddr->setMaximumWidth(300);

    ui->statusBar->addWidget(m_statusInd);
    ui->statusBar->addWidget(m_statusText, 10);
    ui->statusBar->addWidget(m_baseAddr, 10);

    m_dlgModbusRTU = new SettingsRTU(this,m_commSettings);
    connect(ui->actionSerial_RTU, SIGNAL(triggered()), this, SLOT(showSettingsRTU()));

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::changedConnect);
    connect(ui->actionScan, &QAction::triggered, this, &MainWindow::transaction);

    connect(m_thread, &MasterThread::errorOccurred, this, &MainWindow::threadErrorOccured);
    connect(m_thread, &MasterThread::stateChanged, this, &MainWindow::threadStateChanged);
    connect(m_thread, &MasterThread::response, this, &MainWindow::showResponse);
    connect(m_thread, &MasterThread::request, this, &MainWindow::showRequest);
    connect(m_thread, &MasterThread::finished, this, &MainWindow::threadFinished);

    threadStateChanged(MasterThread::UnconnectedState);
    updateStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::threadFinished()
{
    qDebug() << Q_FUNC_INFO;
}

void MainWindow::showSettingsRTU()
{
    if (m_dlgModbusRTU->exec() == QDialog::Accepted) {
//        QLOG_TRACE()<<  "RTU settings changes accepted ";
        updateStatusBar();
        m_commSettings->saveSettings();
    }
//    else
//        QLOG_WARN()<<  "RTU settings changes rejected ";

}

void MainWindow::changedConnect(bool value) //Connect - Disconnect
{
    setControlsEnabled(false);
    m_baseAddr->setText(tr("Port Status: %1").arg("..."));

    if (m_thread->setConfig(m_commSettings->serialPortName(), QSerialPort::Baud115200,
                            QSerialPort::NoParity, QSerialPort::Data8, QSerialPort::OneStop, 100)) {
        m_thread->openPort();
        QTimer::singleShot(100, this, SLOT(openCloseDevice()));
    }
    else {
        openCloseDevice();
        QTimer::singleShot(100, m_thread, SLOT(closePort()));
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

void MainWindow::transaction(bool checked)
{
    if (checked) {
        QByteArray data; // cihaz tarafinda haberlesmeyi acar
        data.append((char) 0xAA);
        data.append((char) 0x01);
        data.append((char) 0x06);
        data.append((char) 0x01);
        data.append((char) 0x22);
        data.append((char) 0xC0);
        data.append((char) 0x00);
        data.append((char) 0x00);
        data.append((char) 0x02);
        data.append((char) 0xE5);
        data.append((char) 0x55);
        m_thread->transaction(data);
    }
    else {
        openCloseDevice();
    }
}

void MainWindow::threadErrorOccured(MasterThread::Error error)
{
    switch (error) {
    case MasterThread::NoError:
        qDebug() << "MasterThread::NoError";
        break;
    case MasterThread::ReadError:
        qDebug() << "MasterThread::ReadError";
        break;
    case MasterThread::WriteError:
        qDebug() << "MasterThread::WriteError";
        break;
    case MasterThread::ConnectionError:
        qDebug() << "MasterThread::ConnectionError";
        QMessageBox::critical(this, tr("Serial Port"), tr("The Port Could Not Open!"));
        break;
    case MasterThread::ConfigurationError:
        qDebug() << "MasterThread::ConfigurationError";
        QMessageBox::critical(this, tr("Serial Port"), tr("Port Configuration Is Invalid!"));

        break;
    case MasterThread::TimeoutError:
        qDebug() << "MasterThread::TimeoutError";
        break;
    case MasterThread::ProtocolError:
        qDebug() << "MasterThread::ProtocolError";
        break;
    case MasterThread::ReplyAbortedError:
        qDebug() << "MasterThread::ReplyAbortedError";
        break;
    case MasterThread::UnknownError:
        qDebug() << "MasterThread::UnknownError";
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

    ui->actionScan->setChecked(false);

    m_baseAddr->setText(tr("Port Status: %1").arg(connected ? "Connected" : "Disconnected"));
    setControlsEnabled(connected);
}

void MainWindow::showResponse(const QByteArray &data)
{
//    qDebug() << "Response" << data.toHex().toUpper() << QTime::currentTime().toString("hh:mm:ss:zzz");
    m_baseAddr->setText(tr("Traffic: %1 #%2#").arg(++count).arg(QString(data.toHex().toUpper())));
}

void MainWindow::showRequest(const QByteArray &data)
{
//    qDebug() << "Request " << data.toHex().toUpper() << QTime::currentTime().toString("hh:mm:ss:zzz");
    m_baseAddr->setText(tr("Traffic: %1 #%2#").arg(++count).arg(QString(data.toHex().toUpper())));
}

void MainWindow::setControlsEnabled(bool enable)
{
    ui->actionSerial_RTU->setEnabled(!enable);
    ui->actionWriteToDevice->setEnabled(enable);
    ui->actionReadFromDevice->setEnabled(enable);
    ui->actionLoad_Session->setEnabled(enable);
    ui->actionSave_Session->setEnabled(enable);
    ui->actionScan->setEnabled(enable);
    ui->actionOpenLogFile->setEnabled(enable);
}

void MainWindow::updateStatusBar()
{
    QString msg = "Port : ";
    msg += m_commSettings->serialPortName() + " | ";
    msg += m_commSettings->baud() + ",";
    msg += m_commSettings->dataBits() + ",";
    msg += m_commSettings->stopBits() + ",";
    msg += m_commSettings->parity();

    m_statusText->setText(msg);
}
