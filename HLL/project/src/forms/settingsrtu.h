#ifndef SETTINGSRTU_H
#define SETTINGSRTU_H

#include <QDialog>
#include <QSettings>

#include "commsettings.h"

namespace Ui {
    class SettingsRTU;
}

class SettingsRTU : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsRTU(QWidget *parent = 0 , CommSettings *settings = 0);
    ~SettingsRTU();
    bool modbus_connected;

private:
    Ui::SettingsRTU *ui;
    CommSettings * m_settings;

private slots:
    void changesAccepted();

protected:
    void showEvent(QShowEvent * event);

};

#endif // SETTINGSRTU_H
