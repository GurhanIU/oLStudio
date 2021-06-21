#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <masterthread.h>

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

public slots:
    void threadFinished();


private slots:
    void showSettingsRTU();

    void transaction(bool checked);
    void threadErrorOccured(MasterThread::Error);
    void threadStateChanged(MasterThread::State);
    void showRequest(const QByteArray &data);
    void showResponse(const QByteArray &data);
    void showResponseStatus(const QString &status);

    void openCloseDevice();

private:
    Ui::MainWindow *ui;
    QLabel *m_statusText;
    QLabel *m_statusInd;
    QLabel *m_lblRequestTraffic;
    QLabel *m_lblResponseTraffic;
    QLabel *m_lblResponseStatus;

    CommSettings *m_commSettings;
    SettingsRTU *m_dlgModbusRTU;

    MasterThread *m_thread;

    void updateStatusBar();

    void changedConnect(bool value);
    void setControlsEnabled(bool enable);
};
#endif // MAINWINDOW_H
