#ifndef DLGPAGEUTIL_H
#define DLGPAGEUTIL_H

#include <QDialog>

class QSqlTableModel;
class QSqlError;

namespace Ui {
class DlgPageUtil;
}

class EDesignerFormEditorInterface;

class DlgPageUtil : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPageUtil(EDesignerFormEditorInterface *core, QWidget *parent = nullptr);
    ~DlgPageUtil();

private slots:
    void slTableDoubleClicked(const QModelIndex &index);
    void slTableSingleClicked(const QModelIndex &index);

    void on_btnNew_clicked();
    void on_btnAdd_clicked();
    void on_btnSave_clicked();
    void on_btnChange_clicked();
    void on_btnCancel_clicked();

    void on_DlgPageUtil_finished(int result);

    void slRowInserted(const QModelIndex &parent, int start, int end);
private:
    Ui::DlgPageUtil *ui;
    EDesignerFormEditorInterface *m_core;
    bool m_dataChanged = false;
    QSqlTableModel *m_model;

    int m_selectedRowIdx = -1;    

    void showError(const QSqlError &err);
};

#endif // DLGPAGEUTIL_H
