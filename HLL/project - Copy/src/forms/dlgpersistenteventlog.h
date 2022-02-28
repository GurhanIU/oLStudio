#ifndef DLGPERSISTENTEVENTLOG_H
#define DLGPERSISTENTEVENTLOG_H

#include <QDialog>
#include <QStyledItemDelegate>
#include <QAbstractItemDelegate>
#include <QDateTime>
#include <QDebug>

#include "objects/ebusdataentries.h"

class QModbusClient;
class QSqlQueryModel;
class EBusDataEntries;
//class DeviceEvent;
class EDesignerFormEditorInterface;

class DateTimeItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DateTimeItemDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) { }
    virtual QString displayText(const QVariant &timestamp, const QLocale &locale) const override {
        return QDateTime::fromTime_t(timestamp.toUInt()).toUTC().toString("dd.MM.yyyy hh:mm:ss");
    }
};

class EventNameItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit EventNameItemDelegate(QMap<int, QString> map, QObject *parent = 0) : QStyledItemDelegate(parent), m_map(map) {}
    virtual QString displayText(const QVariant &value, const QLocale &locale) const override {

        return m_map.value(value.toInt());
    }

    QMap<int, QString> m_map;
};

namespace Ui {
class DlgPersistentEventLog;
}

class DlgPersistentEventLog : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPersistentEventLog(EDesignerFormEditorInterface *core, QModbusClient *modbus, QWidget *parent = nullptr);
    ~DlgPersistentEventLog();

private:
    Ui::DlgPersistentEventLog *ui;
    EDesignerFormEditorInterface *m_core;
    QModbusClient *m_modbus;

    QSqlQueryModel *m_model = nullptr;

    EBusDataEntries *m_dataEntries;

    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);

    QStringList collectEventMetaData() const;
    QMap<int, QString> collectEventName() const;
    void collectEvents();
    void initEvents();
signals:
    void sgVisibilityChanged(bool);
    void sgMessage(const QString &caption, const QString &text, int timeout = 0);
};

#endif // DLGPERSISTENTEVENTLOG_H
