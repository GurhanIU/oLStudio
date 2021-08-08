#ifndef INITDB_H
#define INITDB_H

#include <QFileInfo>
#include <QtSql>
#include <QApplication>

QSqlError initDb(const QString &dbName )
{
    QFileInfo fInfo(dbName);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", fInfo.fileName());

    db.setDatabaseName(dbName);

    if (!db.open())
        return db.lastError();

    return QSqlError();
}

static inline void resetDbActualValues(const QString &fileName)
{
    QSqlQuery query = QSqlQuery(QSqlDatabase::database(fileName));

    query.prepare(QString("UPDATE PARAMETER SET ACTUAL_VALUE = CAST( (DEFAULT_VALUE * FACTOR) AS INT);"));
    query.exec();
}

#endif
