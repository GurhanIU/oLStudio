#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMdiArea>
#include <QModbusDataUnit>
#include <QVariant>
#include <QQueue>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QMdiArea;
class QMimeData;
class QModbusClient;
class QModbusReply;

class About;
class ModbusCommSettings;
class SettingsModbusRTU;
class Settings;
class MenuBox;
class EBusData;
class EBusDataEntries;

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
//    static QString mainWindowTitle();

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

private:
    Ui::MainWindow *ui;
    CloseEventPolicy m_policy;

    QModbusClient *m_modbus;

    About *m_dlgAbout;
    SettingsModbusRTU *m_dlgModbusRTU;

    MenuBox *m_menuBox;

    ModbusCommSettings *m_modbusCommSettings;
    void updateStatusBar();
    QLabel *m_statusText;
    QLabel *m_statusInd;
    QLabel *m_baseAddr;

    EBusDataEntries *m_modbusEntries;

    QModbusClient *createModbusClient();

    QString m_dbFile;

    QQueue<QVariantMap> m_queue;

    void disconnectAndDelete();

private slots:
    void showSettingsModbusRTU();
//    void showBusMonitor();
//    void showTools();
//    void changedModbusMode(int currIndex);
//    void changedFunctionCode(int currIndex);
//    void changedBase(int currIndex);
//    void changedDecSign(bool value);
//    void changedStartAddrBase(int currIndex);
    void slSaveScanRate(int value);
    void changedConnect(bool value);
//    void changedStartAddress(int value);
//    void changedNoOfRegs(int value);
//    void changedSlaveID(int value);
//    void addItems();
//    void clearItems();

//    void modbusScanCycle(bool value);
//    void modbusRequest();
//    void refreshView();
//    void changeLanguage();
//    void openModbusManual();
    void loadSession();
    void saveSession();
//    void showHeaders(bool value);

    void onStateChanged(int state);
    void modbusConnect();

    void slWriteAllToDevice();
    void slReadAllFromDevice();

    void slCollectRegisters();
    void slStartStop(bool toggle);

    void callCreateForm();
    void slUpdateStatusBar(const QString &caption, const QString &text, int timeout = 5000);

    void slInsertLog(const QVariant &data);
protected:
    virtual void closeEvent(QCloseEvent *e);

signals:
    void closeEventReceived(QCloseEvent *e);
    void sgProcessRange(int min, int max);
    void sgProcessValue(int step);
    void sgRegistersCollected();
    void sgUpdateTopToolBox(int);
};

#endif // MAINWINDOW_H
