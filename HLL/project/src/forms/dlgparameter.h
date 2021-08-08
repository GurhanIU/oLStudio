#ifndef DLGPARAMETER_H
#define DLGPARAMETER_H

#include <QDialog>

class QSqlRelationalTableModel;
class QSqlError;

namespace Ui {
class DlgParameter;
}

class EDesignerFormEditorInterface;

class DlgParameter : public QDialog
{
    Q_OBJECT

public:
    explicit DlgParameter(EDesignerFormEditorInterface *core, QWidget *parent = nullptr);
    ~DlgParameter();

private slots:
    void slTableDoubleClicked(const QModelIndex &index);
    void slTableSingleClicked(const QModelIndex &index);

    void on_btnNew_clicked();
    void on_btnAdd_clicked();
    void on_btnSave_clicked();
    void on_btnChange_clicked();
    void on_btnCancel_clicked();

    void on_DlgParameter_finished(int result);

    void slRowInserted(const QModelIndex &parent, int start, int end);
private:
    Ui::DlgParameter *ui;
    EDesignerFormEditorInterface *m_core;
    bool m_dataChanged = false;
    QSqlRelationalTableModel *m_model = nullptr;

    int registerIdx, registerTypeIdx;
    int m_selectedRowIdx = -1;

    void showError(const QSqlError &err);    
    void hideAddedRegister(int index, bool hide);
};

#endif // DLGPARAMETER_H
