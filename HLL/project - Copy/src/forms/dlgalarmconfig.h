#ifndef DLGALARMCONFIG_H
#define DLGALARMCONFIG_H

#include <QDialog>

class QSqlRelationalTableModel;
class QSqlError;

namespace Ui {
class DlgAlarmConfig;
}

class EDesignerFormEditorInterface;

class DlgAlarmConfig : public QDialog
{
    Q_OBJECT

public:
    explicit DlgAlarmConfig(EDesignerFormEditorInterface *core, QWidget *parent = nullptr);
    ~DlgAlarmConfig();

private slots:
    void slTableDoubleClicked(const QModelIndex &index);
    void slTableSingleClicked(const QModelIndex &index);

    void on_btnNew_clicked();
    void on_btnAdd_clicked();
    void on_btnSave_clicked();
    void on_btnChange_clicked();
    void on_btnCancel_clicked();


private:
    Ui::DlgAlarmConfig *ui;
    EDesignerFormEditorInterface *m_core;
    QSqlRelationalTableModel *m_model = nullptr;

    int registerIdx;
    int m_selectedRowIdx = -1;

    bool m_dataChanged = false;

    void showError(const QSqlError &err);
    void hideAddedRegister(int index, bool hide);
};

#endif // DLGALARMCONFIG_H
