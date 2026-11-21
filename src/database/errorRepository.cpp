#include "errorRepository.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

ErrorRepository::ErrorRepository(const QSqlDatabase &database)
    : RepositoryBase(database)
{}

bool ErrorRepository::insertError(ScanError &error)
{
    clearError();

    if(!ensureDatabaseReady())
        return false;

    if(error.scanId <= 0)
    {
        _lastError = "Некорректный ID сканирования.";
        return false;
    }

    QSqlQuery query(_database);

    query.prepare(
        "INSERT INTO scan_errors "
        "(scan_id, path, message) "
        "VALUES (:scan_id, :path, :message)"
    );

    query.bindValue(":scan_id", error.scanId);
    query.bindValue(":path", error.path);
    query.bindValue(":message", error.message);

    if(!query.exec())
    {
        _lastError = query.lastError().text();
        return false;
    }

    error.errorId = query.lastInsertId().toLongLong();

    return true;
}


bool ErrorRepository::getErrorsByScanId(const qint64 &scanId, QList<ScanError> &errors)
{
    clearError();
    errors.clear();

    if(!ensureDatabaseReady())
        return false;

    if(scanId <= 0)
    {
        _lastError = "Некорректный Id сканирования.";
        return false;
    }

    QSqlQuery query(_database);

    query.prepare(
        "SELECT error_id, scan_id, path, message "
        "FROM scan_errors "
        "WHERE scan_id = :scan_id "
        "ORDER BY error_id"
    );

    query.bindValue(":scan_id", scanId);

    if(!query.exec())
    {
        _lastError = query.lastError().text();
        return false;
    }

    while(query.next())
    {
        ScanError error;

        error.errorId = query.value("error_id").toLongLong();
        error.scanId = query.value("scan_id").toLongLong();
        error.path = query.value("path").toString();
        error.message = query.value("message").toString();

        errors.append(error);
    }

    if(query.lastError().isValid())
    {
        _lastError = query.lastError().text();
        errors.clear();
        return false;
    }

    return true;
}