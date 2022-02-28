#ifndef DLGPAIRMENUPAGE_H
#define DLGPAIRMENUPAGE_H

#include <QDialog>
#include <QMessageBox>

class QAbstractButton;
class QSqlTableModel;
class QSqlError;
class QTableView;

namespace Ui {
class DlgPairMenuPage;
}

class EDesignerFormEditorInterface;

class DlgPairMenuPage : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPairMenuPage(EDesignerFormEditorInterface *core, QWidget *parent = nullptr);
    ~DlgPairMenuPage();

signals:
    void sgMenuChanged(int);

private slots:
    void slMenuCurrentRowChanged(const QModelIndex &current, const QModelIndex &previus);
    void slUpdatePageModel(int menuId);    

    void on_buttonBox_clicked(QAbstractButton *button);    
    void on_DlgPairMenuPage_finished(int result);

private:
    Ui::DlgPairMenuPage *ui;
    EDesignerFormEditorInterface *m_core;

    bool m_dataChanged;
    int m_menuId;

    void showEvent(QShowEvent *e);
    QSqlTableModel* menuTableModel(QTableView *view);

    QMessageBox::StandardButton comfirm();
    void apply();
    void discard();
    void changeButtonsStatus(bool status);

    void showError(const QSqlError &err);
};

#endif // DLGPAIRMENUPAGE_H
