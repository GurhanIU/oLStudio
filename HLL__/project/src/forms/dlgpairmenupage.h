#ifndef DLGPAIRMENUPAGE_H
#define DLGPAIRMENUPAGE_H

#include <QDialog>
#include <QMessageBox>

class QSqlTableModel;
class QSqlError;
class QAbstractButton;

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
    void slUpdateMenuModel();
    void slUpdatePageModel(int menuId);
    void slTableMenuClicked(const QModelIndex &index);

    void on_buttonBox_clicked(QAbstractButton *button);    
    void on_DlgPairMenuPage_finished(int result);

private:
    Ui::DlgPairMenuPage *ui;
    EDesignerFormEditorInterface *m_core;

    bool m_dataChanged;
    int m_menuId;

    void showEvent(QShowEvent *e);

    QMessageBox::StandardButton comfirm();
    void apply();
    void discard();
    void changeButtonsStatus(bool status);

    void showError(const QSqlError &err);
};

#endif // DLGPAIRMENUPAGE_H
