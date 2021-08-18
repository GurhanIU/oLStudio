#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMdiArea>
#include <QVariant>

#include "masterthread.h"
#include "responsepacket.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QMdiArea;
class QMimeData;

class About;
class CommSettings;
class SettingsRTU;
class Settings;
class MenuBox;
class ModbusData;
class ModbusDataEntries;

class EDesignerFormEditorInterface;

// ----------------- Eski
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum CloseEventPolicy {
        /* Always accept close events */
        AcceptCloseEvents,
        /* Emit a signal with the event, have it handled elsewhere */
        EmitCloseEventSignal };

    CloseEventPolicy closeEventPolicy() const { return m_policy; }
    void setCloseEventPolicy(CloseEventPolicy pol) { m_policy = pol; }

    MainWindow(const QStringList &args, QWidget *parent = nullptr);
    ~MainWindow();

    QString dbFile() const;
    void setDbFile(const QString &dbFile);

signals:
    void closeEventReceived(QCloseEvent *e);
    void sgProcessRange(int min, int max);
    void sgProcessValue(int step);
    void sgResponse(const QByteArray &data);

public slots:
    void threadFinished();

    void slRequest(const QByteArray &data);

private:
    Ui::MainWindow *ui;
    EDesignerFormEditorInterface *m_core;
    CloseEventPolicy m_policy;

    QString m_dbFile;
    QString m_logFile;

    About *m_dlgAbout;
    SettingsRTU *m_dlgModbusRTU;
    Settings *m_dlgSettings;

    MenuBox *m_menuBox;

    CommSettings *m_commSettings;
    void updateStatusBar();
    QLabel *m_statusText;
    QLabel *m_statusInd;
    QLabel *m_lblRequestTraffic;
    QLabel *m_lblResponseTraffic;
    QLabel *m_lblResponseStatus;

    ModbusDataEntries *m_modbusEntries;

    MasterThread *m_thread;
    MasterThread *createMasterThread();

    void collectRegisters();

    void setControlsEnabled(bool enable);


private slots:
    void closeMasterThread();
    void showSettingsModbusRTU();

    void showSettings();

    void changedConnect(bool value);

    void openLogFile();

    void loadSession();
    void saveSession();

    void threadErrorOccured(MasterThread::Error);
    void threadStateChanged(MasterThread::State);

    void slShowAddress();
    void slShowParameter();
    void slShowAddressType();
    void slShowMenu();
    void slShowPageUtil();
    void slShowPairMenuPage();
    void slShowPairRegisterPage();

    void slPageChanged(const QString &name, const int &id);

    void slActualChanged(QVariant value);
    void callCreateForm();
    void openCloseDevice();

    void showRequest(const QByteArray &data);
    void showResponse(const QByteArray &rawData);

protected:
    virtual void closeEvent(QCloseEvent *e);
};
#endif // MAINWINDOW_H
