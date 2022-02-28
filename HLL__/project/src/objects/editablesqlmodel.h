#ifndef EDITABLESQLMODEL_H
#define EDITABLESQLMODEL_H

#include <QSqlQueryModel>
#include <QSqlDatabase>

class EditableSqlModel : public  QSqlQueryModel
{
    Q_OBJECT
public:
    EditableSqlModel(QSqlDatabase db, QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role =  Qt::EditRole);

private:
    bool setActualValue(int id, int value);
    QSqlDatabase m_db;

};

#endif // EDITABLESQLMODEL_H
