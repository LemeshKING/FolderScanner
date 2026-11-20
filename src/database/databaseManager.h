#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <QStringList>

class DatabaseManager
{
public:
    static constexpr int CurrentSchemaVersion = 1;
    DatabaseManager(const QString &databasePath = QString());
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool open();
    bool initializeDatabase();

    void close();

    bool isOpen() const;

    const QSqlDatabase &connection() const;
    QString lastError() const;

    QString databasePath() const;

private:
    bool executeSqlFile(const QString &resourcePath);

    QStringList splitSqlStatements(const QString &script) const;

    bool readSchemaVersion(int &version);
    bool setSchemaVersion(const int &version);

    bool applyMigrationFrom(const int &version);
    void setError(const QString &error);

    QSqlDatabase _database;

    QString _databasePath;
    QString _connectionName;
    QString _lastError;
};

#endif // DATABASEMANAGER_H
