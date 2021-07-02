#ifndef DLGMODBUSFUNCTION_H
#define DLGMODBUSFUNCTION_H

#include <QDialog>

#include <QStyledItemDelegate>
#include <QAbstractItemDelegate>
#include <QIdentityProxyModel>

class QSqlQueryModel;
class EDesignerFormEditorInterface;

class CustomItemDelegateForTemperatureLog : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CustomItemDelegateForTemperatureLog(QObject *parent = 0) : QStyledItemDelegate(parent) { }
    virtual QString displayText(const QVariant &value, const QLocale &locale) const override{
        QString formattedNum = locale.toString(value.toDouble(), 'f', 1);
        return formattedNum;
    }

//    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
//    {
//        QStyleOptionViewItem alignedOption(option);
//        alignedOption.displayAlignment = m_alignment;
//        QStyledItemDelegate::paint(painter, alignedOption, index);
//    }

//    private:
//        Qt::Alignment m_alignment = Qt::AlignCenter;
};

class CustomProxyModel : public QIdentityProxyModel
{
public:
    CustomProxyModel(QObject* parent = 0): QIdentityProxyModel(parent) { }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role == Qt::TextAlignmentRole && index.column() != 0)
            return Qt::AlignCenter;
        return QIdentityProxyModel::data(index, role);
    }
};

namespace Ui {
class DlgModbusFunction;
}

class DlgModbusFunction : public QDialog
{
    Q_OBJECT

public:
    explicit DlgModbusFunction(EDesignerFormEditorInterface *core, QWidget *parent = nullptr);
    ~DlgModbusFunction();

private slots:
    void slUpdateTable();
    void slTableDoubleClicked(const QModelIndex &index);
    void slTableSingleClicked(const QModelIndex &index);

    void on_btn_newRecord_clicked();
    void on_btnAdd_clicked();
    void on_btnSave_clicked();
    void on_btnCancel_clicked();

    void on_btnCancel_2_clicked();

private:
    Ui::DlgModbusFunction *ui;
    EDesignerFormEditorInterface *m_core;

    QSqlQueryModel *m_model = nullptr;
    CustomProxyModel *m_proxyModel;

    int m_selectedRowIdx = -1;
};

#endif // DLGMODBUSFUNCTION_H
