#ifndef SETTINGSMODBUSRTU_H
#define SETTINGSMODBUSRTU_H

#include <QDialog>
#include <QSettings>

#include "src/modbuscommsettings.h"

namespace Ui {
    class SettingsModbusRTU;
}

class SettingsModbusRTU : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsModbusRTU(QWidget *parent = 0 ,ModbusCommSettings * settings = 0);
    ~SettingsModbusRTU();
    bool modbus_connected;

private:
    Ui::SettingsModbusRTU *ui;
    ModbusCommSettings * m_settings;

private slots:
    void changesAccepted();

protected:
    void showEvent(QShowEvent * event);

signals:
    void sgSettingsChanged();

};

#endif // SETTINGSMODBUSRTU_H
