#ifndef DLGREGISTER_H
#define DLGREGISTER_H

#include <QDialog>

class QSqlRelationalTableModel;
class QSqlError;

namespace Ui {
class DlgRegister;
}

class EDesignerFormEditorInterface;

class DlgRegister : public QDialog
{
    Q_OBJECT

public:
    explicit DlgRegister(EDesignerFormEditorInterface *core, QWidget *parent = nullptr);
    ~DlgRegister();

private slots:
    void slTableDoubleClicked(const QModelIndex &index);
    void slTableSingleClicked(const QModelIndex &index);

    void on_btn_newRecord_clicked();
    void on_btnAdd_clicked();
    void on_btnSave_clicked();
    void on_btnChange_clicked();
    void on_btnCancel_clicked();

    void on_DlgRegister_finished(int result);

    void slRowInserted(const QModelIndex &parent, int start, int end);
private:
    Ui::DlgRegister *ui;
    EDesignerFormEditorInterface *m_core;
    QSqlRelationalTableModel *m_model = nullptr;

    int deviceIdx, modbusFuncIdx, addressIdx, nameIdx, variantTypeIdx, precisionIdx, unitIdx, indicatorObjIdx;
    int m_selectedRowIdx = -1;

    bool m_dataChanged = false;

    void showError(const QSqlError &err);
};

#endif // DLGREGISTER_H
