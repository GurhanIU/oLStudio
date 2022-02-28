#ifndef COMMSETTINGS_H
#define COMMSETTINGS_H

#include <QSettings>
#include <QString>

class CommSettings : public QSettings
{
    Q_OBJECT
public:
    explicit CommSettings(QObject *parent = 0)  : QSettings(parent) { }
    CommSettings(const QString &fileName, Format format = QSettings::IniFormat, QObject *parent = 0);
    //TCP
    QString TCPPort();
    void setTCPPort(QString tcpPort);
    void setSlaveIP(QString IP);
    QString slaveIP();
    //Serial
    QString serialDev();
    QString serialPort();
    QString serialPortName();
    void setSerialPort(QString serialPort, QString serialDev = "");
    QString baud();
    void setBaud(QString baud);
    QString dataBits();
    void setDataBits(QString dataBits);
    QString stopBits();
    void setStopBits(QString stopBits);
    QString parity();
    void setParity(QString parity);
    QString RTS();
    void setRTS(QString RTS);
    //Var
    QString  maxNoOfLines();
    void setMaxNoOfLines(QString maxNoOfLines);
    QString  baseAddr();
    void setBaseAddr(QString baseAddr);
    QString  timeOut();
    void setTimeOut(QString timeOut);
    void loadSettings();
    void saveSettings();
    //logging
    int loggingLevel();
    //session
    int modbusMode();
    void setModbusMode(int modbusMode);
    int slaveID();
    void setSlaveID(int slaveID);
    int scanRate();
    void setScanRate(int scanRate);
    int functionCode();
    void setFunctionCode(int functionCode);
    int startAddr();
    void setStartAddr(int startAddr);
    int noOfRegs();
    void setNoOfRegs(int noOfRegs);
    int base();
    void setBase(int base);
    void loadSession(QString fName);
    void saveSession(QString fName);

private:
    //TCP
    QString m_TCPPort;
    QString m_slaveIP;
    //Serial
    QString m_serialDev;
    QString m_serialPort;
    QString m_serialPortName;
    QString m_baud;
    QString m_dataBits;
    QString m_stopBits;
    QString m_parity;
    QString m_RTS;
    //Var
    QString m_maxNoOfLines;
    QString m_baseAddr;
    QString m_timeOut;
    void load(QSettings *s);
    void save(QSettings *s);
    //Log
    int m_loggingLevel;
    //Session vars
    int m_modbusMode;
    int m_slaveID;
    int m_scanRate;
    int m_functionCode;
    int m_startAddr;
    int m_noOfRegs;
    int m_base;

signals:

public slots:

};

#endif // COMMSETTINGS_H
