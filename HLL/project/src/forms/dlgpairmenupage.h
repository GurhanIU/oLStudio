#ifndef DLGPAIRMENUPAGE_H
#define DLGPAIRMENUPAGE_H

#include <QDialog>

class QSqlTableModel;
class QSqlError;

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

private slots:
    void on_tblMenu_clicked(const QModelIndex &index);
    void slUpdateModelByPageId(const int &id);

private:
    Ui::DlgPairMenuPage *ui;
    EDesignerFormEditorInterface *m_core;

    void showError(const QSqlError &err);
};

#endif // DLGPAIRMENUPAGE_H
