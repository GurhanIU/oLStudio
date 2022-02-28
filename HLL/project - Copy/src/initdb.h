#ifndef INITDB_H
#define INITDB_H

#include <QFileInfo>
#include <QtSql>
#include <QApplication>

//Some databases don’t support transactions. For those, the transaction(), commit(),
//and rollback() functions do nothing. We can test whether a database
//supports transactions using hasFeature() on the QSqlDriver associated with
//the database:

//QSqlDriver *driver = QSqlDatabase::database()->driver();
//if (driver->hasFeature(QSqlDriver::Transactions))
//    ...


QSqlError initDb(const QString &dbName )
{
    QFileInfo fInfo(dbName);
//    qDebug() << fInfo.fileName();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", fInfo.fileName());
//    db.setConnectOptions("QSQLITE_USE_CIPHER=sqlcipher; SQLCIPHER_LEGACY=1");
//    #ifdef QT_DEBUG
    db.setDatabaseName(dbName);
//    db.setDatabaseName(QString("%1/../../db/%2").arg(QApplication::applicationDirPath()).arg(dbName));
//    #else
//    db.setDatabaseName(QString("%1/db/%2").arg(QApplication::applicationDirPath()).arg(dbName));
//    #endif

//    db.setUserName("");
//    db.setPassword("123456");
    if (!db.open())
        return db.lastError();

    return QSqlError();
}

static inline void resetDbActualValues(const QString &fileName)
{
    QSqlQuery query = QSqlQuery(QSqlDatabase::database(fileName));

    query.prepare(QString("UPDATE PARAMETER SET ACTUAL_VALUE = CAST( (DEFAULT_VALUE * FACTOR) AS REAL);"));
    query.exec();
}

#endif
