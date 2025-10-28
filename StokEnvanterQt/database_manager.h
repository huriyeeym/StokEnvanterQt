#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QDebug>
#include <QSqlError>

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager();

    bool connectToDatabase();
    void disconnectFromDatabase();
    QSqlQuery executeQuery(const QString& query);
    QSqlDatabase& getDatabase();

private:
    QSqlDatabase db;
};

#endif // DATABASE_MANAGER_H
