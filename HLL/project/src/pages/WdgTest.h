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

class WdgTest : public QWidget
{
    Q_OBJECT

public:

    enum Address
    {
        ADR_SOP = 0,
        ADR_FNC = 1,
        ADR_LEN = 2,
    };
    Q_ENUM(Address)

    enum Function
    {
        FC_WRITE_MEM,
        FC_WATCH_CONF,
        FC_WATCH_VARS,
        FC_CONNECT
    };
    explicit WdgTest(EDesignerFormEditorInterface *core, ModbusDataEntries *dataEntries, QWidget *parent = nullptr);
    ~WdgTest();

public slots:
    void slModbusStateChanged(int state);
    void slUpdateModelByPageId(const QString &name, const int &id);

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
    ModbusDataEntries::EntryList m_entryList;

    int typeIdx, aliasIdx, deviceIdx, unitIdx;
    int registerIdx, registerTypeIdx;
    int m_selectedRowIdx = -1;

    bool m_dataChanged = false;

    static QByteArray prepeareRequest(const ModbusDataEntries::EntryList &entryList);
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
