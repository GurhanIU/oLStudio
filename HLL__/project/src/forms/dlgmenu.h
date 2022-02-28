#ifndef DLGMENU_H
#define DLGMENU_H

#include <QDialog>

class QSqlTableModel;
class QSqlError;

namespace Ui {
class DlgMenu;
}

class EDesignerFormEditorInterface;

class DlgMenu : public QDialog
{
    Q_OBJECT

public:
    explicit DlgMenu(EDesignerFormEditorInterface *core, QWidget *parent = nullptr);
    ~DlgMenu();

private slots:
    void slTableDoubleClicked(const QModelIndex &index);
    void slTableSingleClicked(const QModelIndex &index);

    void on_btn_newRecord_clicked();
    void on_btnAdd_clicked();
    void on_btnSave_clicked();
    void on_btnChange_clicked();
    void on_btnCancelEdit_clicked();
    void on_btnCancelNew_clicked();

    void on_DlgMenu_finished(int result);

private:
    Ui::DlgMenu *ui;
    EDesignerFormEditorInterface *m_core;

    QSqlTableModel *m_model;
    int m_selectedRowIdx = -1;
    bool m_dataChanged = false;

    void showError(const QSqlError &err);
};

#endif // DLGMENU_H
