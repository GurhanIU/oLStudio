#ifndef WDGTEST_H
#define WDGTEST_H

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QValidator>

#include "objects/ebusdataentries.h"

class EditableSqlModel;

class QModbusClient;
class QSqlError;
class QSlider;
class QLineEdit;

namespace Ui {
class WdgTest;
}

class EDesignerFormEditorInterface;
class EBusData;
class EBusDataEntries;

class WdgTest : public QWidget
{
    Q_OBJECT

public:
    explicit WdgTest(EDesignerFormEditorInterface *core, EBusDataEntries *dataEntries, QWidget *parent = nullptr);
    ~WdgTest();

public slots:
    void slModbusStateChanged(int state);
    void slUpdateModelByPageId(const QString &name, const int &id);

private slots:
    void actualValueChanged(int value);

private:
    Ui::WdgTest *ui;
    EDesignerFormEditorInterface *m_core;
    EditableSqlModel *m_model = nullptr;

    EBusDataEntries *m_dataEntries;
    EBusDataEntries::EntryList m_entryList;

    int typeIdx, controlObjIdx, deviceIdx, unitIdx;
    int registerIdx, registerTypeIdx;
    int m_selectedRowIdx = -1;
    int m_pageId;

    bool m_dataChanged = false;
};

class QDoubleSlider : public QSlider
{
    Q_OBJECT
public:
    QDoubleSlider(QWidget *parent = 0) : QSlider(parent) {
        connect(this, SIGNAL(valueChanged(int)),
            this, SLOT(notifyValueChanged(int)));
    }

    double singleStep() const { return m_singleStep; }
    void setSingleStep(double singleStep) { m_singleStep = singleStep; }

    int decimals() const { return m_decimals; }
    void setDecimals(int decimals) { m_decimals = decimals; }

signals:
    void doubleValueChanged(double value);
    void valueChangedManually(int value);

public slots:
    void notifyValueChanged(int value) { // deger degismis oluyor.
//        qDebug() << "AA" << value << this->value();
        double dValue = value / 10.0;
        emit doubleValueChanged(dValue);
    }

    void setDoubleValue(double value) {
        auto iValue = static_cast<int>( (value *10) + .5f);
        if (value < 0)
            iValue = static_cast<int>( (value *10) - .5f);
//        qDebug() << "BB" << value << iValue;
        const bool wasBlocked = this->blockSignals(true);
        setValue(iValue);
//        qDebug() << "BC" << this->value() << iValue;
        blockSignals(wasBlocked);
        emit valueChangedManually(this->value());
    }

private:
    double m_singleStep;
    int m_decimals;
};

class CustomSlider : public QSlider
{
    Q_OBJECT
public:
    explicit CustomSlider(QWidget *parent = nullptr) : QSlider(parent) {

    }

signals:
    void valueChangedManually(int value);

};

class CustomLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit CustomLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {

    }

signals:
    void validateFinished(QValidator::State);
    void validatedValue(int);

};

#endif // WDGTEST_H
