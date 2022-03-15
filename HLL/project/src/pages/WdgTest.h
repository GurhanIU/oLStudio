#ifndef WDGTEST_H
#define WDGTEST_H

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QValidator>

#include "objects/ebusdataentries.h"

class QModbusClient;
class QSqlError;

namespace Ui {
class WdgTest;
}

class EBusData;
class EBusDataEntries;

class WdgTest : public QWidget
{
    Q_OBJECT

public:
    explicit WdgTest(EBusDataEntries *dataEntries, QWidget *parent = nullptr);
    ~WdgTest();

public slots:
    void slModbusStateChanged(int state);
    void slUpdateModel();

private:
    Ui::WdgTest *ui;

    EBusDataEntries *m_dataEntries;
};

#endif // WDGTEST_H
