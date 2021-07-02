#ifndef DLGADDRESS_H
#define DLGADDRESS_H

#include <QDialog>
#include <QStyledItemDelegate>

class QSqlRelationalTableModel;
class QSqlError;

namespace Ui {
class DlgAddress;
}

class EDesignerFormEditorInterface;

class AddressItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit AddressItemDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) { }
    virtual QString displayText(const QVariant &value, const QLocale &locale) const override{
        QString formatted = QString("0x%1").arg(QString("%1").arg(value.toUInt(), 8, 16, QChar('0')).toUpper());
//        QString formatted = locale.toString();
        return formatted;
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

class DlgAddress : public QDialog
{
    Q_OBJECT

public:
    explicit DlgAddress(EDesignerFormEditorInterface *core, QWidget *parent = nullptr);
    ~DlgAddress();

private slots:
    void slTableDoubleClicked(const QModelIndex &index);
    void slTableSingleClicked(const QModelIndex &index);

    void on_btn_newRecord_clicked();
    void on_btnAdd_clicked();
    void on_btnSave_clicked();
    void on_btnChange_clicked();
    void on_btnCancel_clicked();

    void on_DlgAddress_finished(int result);

private:
    Ui::DlgAddress *ui;
    EDesignerFormEditorInterface *m_core;

    QSqlRelationalTableModel *m_model = nullptr;

    int variantTypeIdx, modbusFuncIdx, aliasIdx, deviceIdx, unitIdx;
    int m_selectedRowIdx = -1;
    void showError(const QSqlError &err);

    bool m_dataChanged = false;
};

#endif // DLGADDRESS_H
