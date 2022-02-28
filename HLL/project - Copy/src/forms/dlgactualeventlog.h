#ifndef DLGEVENTLOG_H
#define DLGEVENTLOG_H

#include <QDialog>
#include <QTimer>

#include "objects/ebusdataentries.h"

namespace Ui {
class DlgActualEventLog;
}

class QModbusClient;
class QSqlQueryModel;
class EBusDataEntries;
class EDesignerFormEditorInterface;

class DlgEventLog : public QDialog
{
    Q_OBJECT

public:
    explicit DlgEventLog(EDesignerFormEditorInterface *core, QModbusClient *modbus, QWidget *parent = nullptr);
    ~DlgEventLog();

private slots:
    void updateRegisters();

private:
    Ui::DlgActualEventLog *ui;
    EDesignerFormEditorInterface *m_core;
    QModbusClient *m_modbus;

    QSqlQueryModel *m_model = nullptr;

    EBusDataEntries *m_dataEntries;
//    ModbusDataEntries::EntryList m_entryList;

    QTimer m_timer;

    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
signals:
    void sgVisibilityChanged(bool);


};

#endif // DLGEVENTLOG_H
