#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QLabel;

class CommSettings;
class SettingsRTU;
class MasterThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum State {
        UnconnectedState,
        ConnectingState,
        ConnectedState,
        ClosingState
    };
    Q_ENUM(State)

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showSettingsRTU();
    void openSerialPort();
    void onStateChanged(int state);


    void transaction();
    void showResponse(const QString &s);
    void showRequest(QByteArray data);
    void processError(const QString &s);
    void processTimeout(const QString &s);


private:
    Ui::MainWindow *ui;
    QLabel *m_statusText;
    QLabel *m_statusInd;
    QLabel *m_baseAddr;

    CommSettings *m_commSettings;
    SettingsRTU *m_dlgModbusRTU;

    MasterThread *m_thread;

    void updateStatusBar();

    void changedConnect(bool value);
    void setControlsEnabled(bool enable);

};
#endif // MAINWINDOW_H
