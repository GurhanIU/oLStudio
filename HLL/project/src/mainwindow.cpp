#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QDebug>

#include "commsettings.h"
#include "settingsrtu.h"
#include "masterthread.h"

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

    ui->statusBar->addWidget(m_statusInd);
    ui->statusBar->addWidget(m_statusText, 10);
    ui->statusBar->addWidget(m_baseAddr, 10);

    m_dlgModbusRTU = new SettingsRTU(this,m_commSettings);
    connect(ui->actionSerial_RTU, SIGNAL(triggered()), this, SLOT(showSettingsRTU()));

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::changedConnect);
    connect(ui->actionScan, &QAction::triggered, this, &MainWindow::transaction);

    connect(m_thread, &MasterThread::response, this, &MainWindow::showResponse);
    connect(m_thread, &MasterThread::request, this, &MainWindow::showRequest);
    connect(m_thread, &MasterThread::error, this, &MainWindow::processError);
    connect(m_thread, &MasterThread::timeout, this, &MainWindow::processTimeout);

    onStateChanged(MainWindow::UnconnectedState);
    updateStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::onStateChanged(int state)
{
    bool connected = (state != MainWindow::UnconnectedState);

    switch (state) {
    case MainWindow::UnconnectedState:
        qDebug() << "QDevice::UnconnectedState";
        ui->actionConnect->setText(tr("Bağlan"));
        ui->actionConnect->setChecked(false);
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-red-16.png"));
        break;
    case MainWindow::ConnectingState:
        qDebug() << "QDevice::ConnectingState";
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-orange-16.png"));
        break;
    case MainWindow::ConnectedState:
        qDebug() << "QDevice::ConnectedState";
        ui->actionConnect->setText(tr("Kopar "));
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-green-16.png"));
        break;
    case MainWindow::ClosingState:
        qDebug() << "QDevice::ClosingState";
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-orange-16.png"));
        break;
    }

    ui->actionWriteToDevice->setEnabled(connected);
    ui->actionReadFromDevice->setEnabled(connected);
    ui->actionLoad_Session->setEnabled(connected);
    ui->actionSave_Session->setEnabled(connected);
    ui->actionScan->setEnabled(connected);
    ui->actionSerial_RTU->setEnabled(!connected);
}

void MainWindow::transaction()
{
    setControlsEnabled(false);
    m_baseAddr->setText(tr("Status: Running"));

//    m_thread->transaction(serialPortComboBox->currentText(),
//                       waitResponseSpinBox->value(),
//                       requestLineEdit->text());
}

void MainWindow::showResponse(const QString &s)
{
//    setControlsEnabled(true);
    qDebug() << "showResponse" << s;
//    m_baseAddr->setText(tr("Traffic, transaction #%1:"
//                             "\n\r-request: %2"
//                             "\n\r-response: %3")
    //                          .arg(++transactionCount).arg(requestLineEdit->text()).arg(s));
}

void MainWindow::showRequest(QByteArray data)
{
    static int count = 0;
//    setControlsEnabled(true);
//    for (int i=0; i<data.length(); i++) {
//        qDebug() << "showRequest i:" << i  << QString("%1").arg(data.at(i), 16);
//    }
    qDebug() << "showRequest" << data.toHex().toUpper();

    m_baseAddr->setText(tr("Traffic: %1 #%2#").arg(++count).arg(QString(data.toHex().toUpper())));
}

void MainWindow::processError(const QString &s)
{
    setControlsEnabled(true);
//    qDebug() << "processError" << s;
}

void MainWindow::processTimeout(const QString &s)
{
    setControlsEnabled(true);
//    qDebug() << "processTimeout" << s;
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

void MainWindow::changedConnect(bool value) //Connect - Disconnect
{
    qDebug() << value;
    setControlsEnabled(value);
    m_baseAddr->setText(tr("Status: %1").arg(value ? "Connected" : "Disconnected"));

    if (!m_thread->isRunning()) {
        m_thread->setConfig(m_commSettings->serialPortName(),
                            QSerialPort::Baud115200,
                            QSerialPort::NoParity,
                            QSerialPort::Data8,
                            QSerialPort::OneStop,
                            m_commSettings->timeOut().toInt());

    }

    m_thread->transaction(value);
}

void MainWindow::openSerialPort() //Modbus connect - RTU/TCP
{

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
