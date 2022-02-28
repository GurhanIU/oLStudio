#ifndef DLGPAIRTYPEREGISTER_H
#define DLGPAIRTYPEREGISTER_H

#include <QDialog>
#include <QMessageBox>

class QAbstractButton;
class QSqlTableModel;
class QSqlError;
class QTableView;

namespace Ui {
class DlgPairTypeRegister;
}

class EDesignerFormEditorInterface;

class DlgPairTypeRegister : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPairTypeRegister(EDesignerFormEditorInterface *core, QWidget *parent = nullptr);
    ~DlgPairTypeRegister();

signals:
    void sgTypeChanged(int);

private slots:
    void slPageCurrentRowChanged(const QModelIndex &current, const QModelIndex &previus);
    void slUpdateRegisterModel(int pageId);

    void on_buttonBox_clicked(QAbstractButton *button);    
    void on_DlgPairTypeRegister_finished(int result);

private:
    Ui::DlgPairTypeRegister *ui;
    EDesignerFormEditorInterface *m_core;

    bool m_dataChanged;
    int m_typeId;

    void showEvent(QShowEvent *e);
    QSqlTableModel* typeTableModel(QTableView *view);

    QMessageBox::StandardButton comfirm();
    void apply();
    void discard();
    void changeButtonsStatus(bool status);

    void showError(const QSqlError &err);
};

#endif // DLGPAIRTYPEREGISTER_H
