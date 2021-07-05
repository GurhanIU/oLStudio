#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMdiArea>

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

class MainWindowBase: public QMainWindow
{
    Q_DISABLE_COPY(MainWindowBase)
    Q_OBJECT
protected:
    explicit MainWindowBase(QWidget *parent = 0, Qt::WindowFlags flags = Qt::Window);

public:
    enum CloseEventPolicy {
        /* Always accept close events */
        AcceptCloseEvents,
        /* Emit a signal with the event, have it handled elsewhere */
        EmitCloseEventSignal };

    CloseEventPolicy closeEventPolicy() const { return m_policy; }
    void setCloseEventPolicy(CloseEventPolicy pol) { m_policy = pol; }

//    static QList<QToolBar *> createToolBars(const EDesignerActions *actions, bool singleToolBar);
    static QString mainWindowTitle();

    // Use the minor Qt version as settings versions to avoid conflicts
    static int settingsVersion();

signals:
    void closeEventReceived(QCloseEvent *e);

protected:
    virtual void closeEvent(QCloseEvent *e);
private:
    CloseEventPolicy m_policy;
};

class DockedMdiArea : public QMdiArea
{
    Q_DISABLE_COPY(DockedMdiArea)
    Q_OBJECT
public:
    explicit DockedMdiArea(const QString &extension, QWidget *parent = 0);

signals:
    void fileDropped(const QString &);

protected:
    bool event (QEvent *event);

private:
    QStringList uiFiles(const QMimeData *d) const;

    const QString m_extension;
};

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

    void changedScanRate(int value);
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

    void slActualChanged(const QVariant &value);
    void callCreateForm();
    void openCloseDevice();

    void showRequest(const QByteArray &data);
    void showResponse(const QByteArray &data);

protected:
    virtual void closeEvent(QCloseEvent *e);
};
#endif // MAINWINDOW_H
