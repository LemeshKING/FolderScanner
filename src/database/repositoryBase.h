#ifndef REPOSITORYBASE_H
#define REPOSITORYBASE_H

#include <QSqlDatabase>
#include <QString>

class RepositoryBase
{
public:
    virtual ~RepositoryBase() = default;
    QString lastError() const;

protected:
    RepositoryBase(const QSqlDatabase &database);

    bool ensureDatabaseReady();

    void clearError();
    QSqlDatabase _database;
    QString _lastError;
};

#endif // REPOSITORYBASE_H
