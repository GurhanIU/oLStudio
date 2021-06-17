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

QT_USE_NAMESPACE

MasterThread::MasterThread(QObject *parent)
    : QThread(parent), m_waitTimeout(0), m_quit(false)
{
}

//! [0]
MasterThread::~MasterThread()
{
    m_mutex.lock();
    m_quit = true;
    m_cond.wakeOne();
    m_mutex.unlock();
    wait();
}
//! [0]

void MasterThread::setConfig(const QString &portName, QSerialPort::BaudRate baudRate, QSerialPort::Parity parity, QSerialPort::DataBits dataBits, QSerialPort::StopBits stopBits, int waitTimeout)
{
//    QMutexLocker locker(&m_mutex);
    m_mutex.lock();
    m_portName = portName;
    m_baudRate = baudRate;
    m_parity = parity;
    m_dataBits = dataBits;
    m_stopBits = stopBits;
    m_waitTimeout = waitTimeout;
    m_mutex.unlock();
}

//! [1] //! [2]
void MasterThread::transaction(bool start)
{
    //! [1]
    QMutexLocker locker(&m_mutex);
    m_quit = !start;
//    m_request = request;
    //! [3]
    if (!isRunning() && start)
        this->start();
    else
        m_cond.wakeOne();
}
//! [2] //! [3]

//! [4]
void MasterThread::run()
{
    bool currentPortNameChanged = false;

    m_mutex.lock();
    //! [4] //! [5]
    QString currentPortName;
    if (currentPortName != m_portName) {
        currentPortName = m_portName;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = m_waitTimeout;
    QString currentRequest;// = m_request;
    QString currentRespone;
    m_mutex.unlock();
    //! [5] //! [6]
    QSerialPort serial;

    while (!m_quit) {
        //![6] //! [7]
        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);

            if (!serial.open(QIODevice::ReadWrite)) {
                emit error(tr("Can't open %1, error code %2")
                           .arg(m_portName).arg(serial.error()));
                return;
            }

            serial.setParity(QSerialPort::NoParity);
            serial.setBaudRate(QSerialPort::Baud115200);
            serial.setDataBits(QSerialPort::Data8);
            serial.setStopBits(QSerialPort::OneStop);
        }
        // write request
        if (!currentRequest.isEmpty()) {
            QByteArray requestData = currentRequest.toLocal8Bit();
            serial.write(requestData);
            if (serial.waitForBytesWritten(m_waitTimeout)) {
                // read response
                if (serial.waitForReadyRead(currentWaitTimeout)) {
                    QByteArray responseData = serial.readAll();
                    while (serial.waitForReadyRead(100))
                        responseData += serial.readAll();

                    QString response(responseData);
                    emit this->response(response);
                } else {
                    emit timeout(tr("Wait read response timeout %1")
                                 .arg(QTime::currentTime().toString()));
                }
            } else {
                emit timeout(tr("Wait write request timeout %1")
                             .arg(QTime::currentTime().toString()));
            }
        }
        else {
            if (serial.waitForReadyRead(currentWaitTimeout)) {
                // read request
                QByteArray requestData = serial.readAll();
                while (serial.waitForReadyRead(20))
                    requestData += serial.readAll();

                    QString request(qPrintable(requestData));
//                    emit this->request(request, requestData.length());
                    emit this->request(requestData);

            } else {
                emit timeout(tr("Wait read request timeout %1")
                             .arg(QTime::currentTime().toString()));
            }
        }

        m_mutex.lock();
//        m_cond.wait(&m_mutex);
        if (currentPortName != m_portName) {
            currentPortName = m_portName;
            currentPortNameChanged = true;
        } else {
            currentPortNameChanged = false;
        }
        currentWaitTimeout = m_waitTimeout;
        currentRequest = m_request;
        m_mutex.unlock();

    }

    serial.close();
    //! [13]
}
