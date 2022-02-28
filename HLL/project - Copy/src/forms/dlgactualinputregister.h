#ifndef DLGINPUTREGISTER_H
#define DLGINPUTREGISTER_H

#include <QDialog>

#include "objects/ebusdataentries.h"

namespace Ui {
class DlgActualInputRegister;
}

class QModbusClient;
class QSqlQueryModel;
class EBusDataEntries;
class EDesignerFormEditorInterface;

class DlgActualInputRegister : public QDialog
{
    Q_OBJECT

public:
    explicit DlgActualInputRegister(EDesignerFormEditorInterface *core, QModbusClient *modbus, QWidget *parent = nullptr);
    ~DlgActualInputRegister();

private:
    Ui::DlgActualInputRegister *ui;
    EDesignerFormEditorInterface *m_core;
    QModbusClient *m_modbus;

    QSqlQueryModel *m_model = nullptr;

    EBusDataEntries *m_dataEntries;

    int m_scanRate;

    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
signals:
    void sgVisibilityChanged(bool);
    void sgMessage(const QString &caption, const QString &text, int timeout = 0);

public slots:
    void slChangeScanRate(int scanRate);
};

#endif // DLGINPUTREGISTER_H
