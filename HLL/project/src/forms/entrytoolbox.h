#ifndef ENTRYTOOLBOX_H
#define ENTRYTOOLBOX_H

#include <QWidget>

class QSqlRelationalTableModel;

namespace Ui {
class EntryToolBox;
}

class EntryToolBox : public QWidget
{
    Q_OBJECT

public:
    explicit EntryToolBox(QWidget *parent = nullptr);
    ~EntryToolBox();

private slots:
    void on_btnAdd_clicked();

private:
    Ui::EntryToolBox *ui;
    void initAddToolBox();

    QSqlRelationalTableModel *m_addModel;

    int modbusFuncIdx, addressIdx, nameIdx, variantTypeIdx, precisionIdx, unitIdx;

signals:
    void sgCollectRegisters();
    void sgStartStop(bool);
};

#endif // ENTRYTOOLBOX_H
