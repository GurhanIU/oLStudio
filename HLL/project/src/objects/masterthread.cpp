/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "masterthread.h"

#include <QTime>
#include <QDebug>

QT_USE_NAMESPACE

MasterThread::MasterThread(QObject *parent)
    : QThread(parent), m_waitTimeout(0), m_quit(false)
{
    qRegisterMetaType<MasterThread::Error>();
    qRegisterMetaType<MasterThread::State>();
}

MasterThread::~MasterThread()
{
    m_mutex.lock();
    m_quit = true;
    m_cond.wakeOne();
    m_mutex.unlock();
    wait();
}

/**
 * @brief MasterThread::setConfig
 * @param portName
 * @param baudRate
 * @param parity
 * @param dataBits
 * @param stopBits
 * @param waitTimeout
 * @return Return true if thread not running else return false.
 */
bool MasterThread::setConfig(const QString &portName, QSerialPort::BaudRate baudRate, QSerialPort::Parity parity, QSerialPort::DataBits dataBits, QSerialPort::StopBits stopBits, int waitTimeout)
{
    QMutexLocker locker(&m_mutex);

    if (isRunning())
        return false;

    m_quit = false;
    m_portName = portName;
    m_baudRate = baudRate;
    m_parity = parity;
    m_dataBits = dataBits;
    m_stopBits = stopBits;
    m_waitTimeout = waitTimeout;

    return true;
}

void MasterThread::openPort()
{
    if (isRunning())
        return;

    m_quit = false;
    start();
}

void MasterThread::closePort()
{
    QMutexLocker locker(&m_mutex);
    m_quit = true;
    m_cond.wakeOne();
}

void MasterThread::transaction(const QByteArray &data)
{
    QMutexLocker locker(&m_mutex);
    m_transactionData = data;
    if (isRunning())
        m_cond.wakeOne();
}

void MasterThread::run()
{
    m_mutex.lock();

    QString currentPortName = m_portName;
    int currentWaitTimeout = m_waitTimeout;
    QByteArray requestData(m_transactionData);
    m_transactionData.clear();

    // Burada thread in kilidini aciyor.
    m_mutex.unlock();

    emit stateChanged(MasterThread::ConnectingState);
    QSerialPort serial;    
    serial.setPortName(currentPortName);

    if (!serial.open(QIODevice::ReadWrite)) {
        emit errorOccurred(MasterThread::ConnectionError);
        emit stateChanged(MasterThread::UnconnectedState);
        return;
    }

    bool configStatus = false;
    configStatus = serial.setParity(QSerialPort::NoParity);
    configStatus &= serial.setBaudRate(QSerialPort::Baud115200);
    configStatus &= serial.setDataBits(QSerialPort::Data8);
    configStatus &= serial.setStopBits(QSerialPort::OneStop);

    if (!configStatus) {
        emit stateChanged(MasterThread::ClosingState);
        emit errorOccurred(MasterThread::ConfigurationError);
        serial.close();
        emit stateChanged(MasterThread::UnconnectedState);
        return;
    }

    // Thread i kitliyor ve beklemeye basliyor.
    m_mutex.lock();
    emit stateChanged(MasterThread::ConnectedState);
    m_cond.wait(&m_mutex); // Port islemleri yapildi ve transaction bekleniyor!
    m_mutex.unlock();

    while (!m_quit) {
        // write request
        if (!requestData.isEmpty()) {
            emit this->request(requestData);
            serial.write(requestData);
            if (serial.waitForBytesWritten(m_waitTimeout)) {
                // read response
                if (serial.waitForReadyRead(currentWaitTimeout)) {
                    QByteArray responseData = serial.readAll();
                    while (serial.waitForReadyRead(20))
                        responseData += serial.readAll();
                    emit this->response(responseData);
                }
                else {
                    emit errorOccurred(MasterThread::TimeoutError);
                }
            }
            else {
                emit errorOccurred(MasterThread::TimeoutError);
            }

            requestData.clear();
        }
        else {
            if (serial.waitForReadyRead(currentWaitTimeout)) {
                // read request
                QByteArray responseData = serial.readAll();
                while (serial.waitForReadyRead(20))
                    responseData += serial.readAll();
                emit this->response(responseData);
            }
            else {
                emit errorOccurred(MasterThread::TimeoutError);
            }
        }

        // Thread i kitliyor ve varsa yapilan degisiklikleri while dongusunun
        // icine aliyor.
        m_mutex.lock();
        currentWaitTimeout = m_waitTimeout;
        requestData = m_transactionData;
        m_transactionData.clear();
        m_mutex.unlock();
    }

    emit stateChanged(MasterThread::ClosingState);
    serial.close();
    emit stateChanged(MasterThread::UnconnectedState);
}
