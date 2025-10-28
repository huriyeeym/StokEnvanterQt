#include "database_manager.h"

DatabaseManager::DatabaseManager(QObject* parent)
    : QObject(parent)
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QODBC", "qt_sql_default_connection");
    }
}

DatabaseManager::~DatabaseManager()
{
    disconnectFromDatabase();
}

bool DatabaseManager::connectToDatabase()
{
    if (db.isOpen()) {
        qDebug() << "Veritabanı zaten açık!";
        return true;
    }

    db.setDatabaseName("DRIVER={SQL Server};SERVER=HURIYEYILDIRIM\\SQLEXPRESS;DATABASE=StokEnvanterDB;Trusted_Connection=yes;");

    if (db.open()) {
        qDebug() << "Veritabanına bağlantı başarılı!";
        return true;
    } else {
        qDebug() << "Veritabanına bağlanırken hata oluştu:" << db.lastError().text();
        return false;
    }
}

void DatabaseManager::disconnectFromDatabase()
{
    if (db.isOpen()) {
        db.close();
        qDebug() << "Veritabanı bağlantısı kapatıldı.";
    }
}

QSqlQuery DatabaseManager::executeQuery(const QString& query)
{
    if (!db.isOpen()) {
        qDebug() << "Sorgu çalıştırılamadı, veritabanı açık değil.";
        return QSqlQuery();
    }

    QSqlQuery qry(db);
    if (!qry.exec(query)) {
        qDebug() << "SQL Hatası:" << qry.lastError().text();
    }
    return qry;
}

QSqlDatabase& DatabaseManager::getDatabase()
{
    return db;
}
