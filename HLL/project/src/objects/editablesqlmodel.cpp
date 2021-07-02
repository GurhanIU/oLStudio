#include "editablesqlmodel.h"

#include <QSqlQuery>
#include <QDebug>

EditableSqlModel::EditableSqlModel(QSqlDatabase db, QObject *parent)
    :QSqlQueryModel(parent),
      m_db(db)
{

}

bool EditableSqlModel::setActualValue(int id, int value)
{
    QSqlQuery query = QSqlQuery(m_db);
    query.prepare(QString("UPDATE PARAMETER SET ACTUAL_VALUE = %1 WHERE ID = %2")
                  .arg(value)
                  .arg(id));
//    query.addBindValue(value);
//    query.addBindValue(id);
    return query.exec();
}

Qt::ItemFlags EditableSqlModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEditable;

    return QSqlQueryModel::flags(index) | Qt::ItemIsEditable;;
}

bool EditableSqlModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
//    QObject *obj = qobject_cast<QObject*>(sender());
//    qDebug() << obj->objectName();

    if (index.isValid() && role == Qt::EditRole)
    {
        QModelIndex primaryKeyIndex = QSqlQueryModel::index(index.row(), 0);
        int pId = data(primaryKeyIndex).toInt();

        bool ret = setActualValue(pId,value.toInt());

        if (ret)
        {
            emit dataChanged(index, index);
            query().exec();
        }
        return ret;
    }

    return false;
}
