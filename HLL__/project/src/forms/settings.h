#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>

#include "commsettings.h"

namespace Ui {
    class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0 , CommSettings * settings = 0);
    ~Settings();
    bool modbus_connected;

private:
    Ui::Settings *ui;
    CommSettings *m_settings;

private slots:
    void changesAccepted();


protected:
    void showEvent(QShowEvent * event);

};

#endif // SETTINGS_H
