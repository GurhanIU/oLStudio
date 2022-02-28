#ifndef DLGPAIRREGISTERPAGE_H
#define DLGPAIRREGISTERPAGE_H

#include <QDialog>
#include <QMessageBox>

class QAbstractButton;
class QSqlTableModel;
class QSqlError;
class QTableView;

namespace Ui {
class DlgPairRegiserPage;
}

class EDesignerFormEditorInterface;

class DlgPairRegisterPage : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPairRegisterPage(EDesignerFormEditorInterface *core, QWidget *parent = nullptr);
    ~DlgPairRegisterPage();

signals:
    void sgPageChanged(int);

private slots:
    void slPageCurrentRowChanged(const QModelIndex &current, const QModelIndex &previus);
    void slUpdateRegisterModel(int pageId);

    void on_buttonBox_clicked(QAbstractButton *button);    
    void on_DlgPairRegiserPage_finished(int result);

private:
    Ui::DlgPairRegiserPage *ui;
    EDesignerFormEditorInterface *m_core;

    bool m_dataChanged;
    int m_pageId;

    void showEvent(QShowEvent *e);
    QSqlTableModel* pageTableModel(QTableView *view);

    QMessageBox::StandardButton comfirm();
    void apply();
    void discard();
    void changeButtonsStatus(bool status);

    void showError(const QSqlError &err);
};

#endif // DLGPAIRREGISTERPAGE_H
