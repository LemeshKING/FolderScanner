#include "repositoryBase.h"

RepositoryBase::RepositoryBase(const QSqlDatabase &database)
    : _database(database)
{}

bool RepositoryBase::ensureDatabaseReady()
{
    if(!_database.isValid())
    {
        _lastError = "Подключение к базе данных недействительно.";
        return false;
    }

    if(!_database.isOpen())
    {
        _lastError = "База данных не открыта.";
        return false;
    }

    return true;
}

QString RepositoryBase::lastError() const
{
    return _lastError;
}

void RepositoryBase::clearError()
{
    _lastError.clear();
}