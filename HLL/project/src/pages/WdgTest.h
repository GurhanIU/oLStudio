#ifndef WDGTEST_H
#define WDGTEST_H

#include <QWidget>
#include <QSlider>
//#include <QDebug>

#include "objects/modbusdataentries.h"

class QSqlError;
class QSlider;

namespace Ui {
class WdgTest;
}

class EditableSqlModel;
class EDesignerFormEditorInterface;
class ModbusData;
class ModbusDataEntries;
class OnBusMaster;

class WdgTest : public QWidget
{
    Q_OBJECT
public:
    explicit WdgTest(EDesignerFormEditorInterface *core, OnBusMaster *onBusMaster, QWidget *parent = nullptr);
    ~WdgTest();

public slots:
    void slModbusStateChanged(int state);
    void init(const QString &title, int id);

    void slResponse(const QByteArray &response);

private slots:
    void actualValueChanged(int value);
    void actualValueChanged(QString value);

    void on_btnStart_clicked();
    void on_btnStop_clicked();    

private:
    Ui::WdgTest *ui;
    EDesignerFormEditorInterface *m_core;
    EditableSqlModel *m_model = nullptr;

    ModbusDataEntries *m_dataEntries;

    int typeIdx, aliasIdx, deviceIdx, unitIdx;
    int registerIdx, registerTypeIdx;
    int m_selectedRowIdx = -1;

    QString m_title;
    int m_pageId;

    bool m_dataChanged = false;

    void updatePage();
    void initModel(int pageId);
    void collectEntries();
signals:
    void sgRequest(const QByteArray &data);
};

class CustomSlider : public QSlider
{
    Q_OBJECT
public:
    explicit CustomSlider(QWidget *parent = nullptr) : QSlider(parent) { }

signals:
    void valueChangedManually(int value);

};

#endif // WDGTEST_H
