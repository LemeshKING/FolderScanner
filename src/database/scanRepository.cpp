#include "scanRepository.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QVariant>

ScanRepository::ScanRepository(const QSqlDatabase &database)
    : RepositoryBase(database)
{}

QString ScanRepository::statusToString(const ScanStatus &status)
{
    switch (status)
    {
        case ScanStatus::Complete:
            return "COMPLETE";

        case ScanStatus::Partial:
            return "PARTIAL";

        case ScanStatus::Failed:
            return "FAILED";

        default:
            break;
    }

    return {};
}

bool ScanRepository::statusFromString(const QString &value, ScanStatus &outStatus)
{
    if(value == "COMPLETE")
    {
        outStatus = ScanStatus::Complete;
        return true;
    }

    if(value == "PARTIAL")
    {
        outStatus = ScanStatus::Partial;
        return true;
    }

    if(value == "FAILED")
    {
        outStatus = ScanStatus::Failed;
        return true;
    }

    return false;
}

bool ScanRepository::readScan(const QSqlQuery &query, ScanInfo &outScan)
{
    outScan.scanId = query.value("scan_id").toLongLong();

    outScan.rootPath = query.value("root_path").toString();

    QString dateString = query.value("scanned_at_utc").toString();

    outScan.scannedAtUtc = QDateTime::fromString(dateString, Qt::ISODate);

    if(!outScan.scannedAtUtc.isValid())
    {
        _lastError = "Не удалось прочитать дату сканирования.";
        return false;
    }

    QString statusString = query.value("status").toString();

    if(!statusFromString(statusString, outScan.status))
    {
        _lastError = "В базе данных обнаружен неизвестный статус сканирования.";
        return false;
    }

    return true;
}

bool ScanRepository::insertScan(ScanInfo &scan)
{
    clearError();
    if(!ensureDatabaseReady())
        return false;

    if(scan.rootPath.trimmed().isEmpty())
    {
        _lastError = "Путь к папке не может быть пустым.";
        return false;
    }

    QString status = statusToString(scan.status);

    if(status.isEmpty())
    {
        _lastError = "Неизвестный статус сканирования.";
        return false;
    }

    if(!scan.scannedAtUtc.isValid())
    {
        scan.scannedAtUtc = QDateTime::currentDateTimeUtc();
    }
    else
    {
        scan.scannedAtUtc = scan.scannedAtUtc.toUTC();
    }

    QSqlQuery query(_database);

    query.prepare(
        "INSERT INTO scans "
        "(root_path, scanned_at_utc, status) "
        "VALUES (:root_path, :scanned_at_utc, :status)"
    );


    query.bindValue(":root_path", scan.rootPath);
    query.bindValue(":scanned_at_utc", scan.scannedAtUtc.toString(Qt::ISODateWithMs));
    query.bindValue(":status", status);

    if(!query.exec())
    {
        _lastError = query.lastError().text();
        return false;
    }

    scan.scanId = query.lastInsertId().toLongLong();

    if(scan.scanId <= 0)
    {
        _lastError = "Не удалось получить ID созданного сканирования.";
        return false;
    }

    return true;
}

bool ScanRepository::getScanById(const qint64 &scanId, ScanInfo &outScan)
{
    clearError();

    if(!ensureDatabaseReady())
        return false;

    QSqlQuery query(_database);

    query.prepare("SELECT scan_id, root_path, scanned_at_utc, status "
                  "FROM scans "
                  "WHERE scan_id = :scan_id"
    );

    query.bindValue(":scan_id", scanId);

    if(!query.exec())
    {
        _lastError = query.lastError().text();
        return false;
    }

    if(!query.next())
    {
        _lastError = QString("Сканирование с ID %1 не найдено.").arg(scanId);
        return false;
    }

    return readScan(query, outScan);

}

bool ScanRepository::getAllScans(QList<ScanInfo> &outScans)
{
    clearError();
    outScans.clear();

    if(!ensureDatabaseReady())
        return false;

    QSqlQuery query(_database);
    if(!query.exec(
            "SELECT scan_id, root_path, scanned_at_utc, status "
            "FROM scans "
            "ORDER BY scanned_at_utc DESC, scan_id DESC"))
    {
        _lastError = query.lastError().text();
        return false;
    }

    while(query.next())
    {
        ScanInfo scan;

        if(!readScan(query, scan))
        {
            outScans.clear();
            return false;
        }

        outScans.append(scan);
    }

    return true;
}

bool ScanRepository::updateScanStatus(const qint64 &scanId, const ScanStatus &status)
{
    clearError();
    if(!ensureDatabaseReady())
        return false;

    QString statusString = statusToString(status);
    if(statusString.isEmpty())
    {
        _lastError = "Неизвестный статус сканирования.";
        return false;
    }

    QSqlQuery query(_database);

    query.prepare(
        "UPDATE scans "
        "SET status = :status "
        "WHERE scan_id = :scan_id"
    );

    query.bindValue(":status", statusString);
    query.bindValue(":scan_id", scanId);

    if(!query.exec())
    {
        _lastError = query.lastError().text();
        return false;
    }

    if(query.numRowsAffected() == 0)
    {
        _lastError = QString("Сканирование с ID %1 не найдено").arg(scanId);
        return false;
    }

    return true;

}

bool ScanRepository::deleteScan(const qint64 scanId)
{
    clearError();

    if(!ensureDatabaseReady())
        return false;

    QSqlQuery query(_database);
    query.prepare(
        "DELETE FROM scans "
        "WHERE scan_id = :scan_id"
    );

    query.bindValue(":scan_id", scanId);

    if(!query.exec())
    {
        _lastError = query.lastError().text();
        return false;
    }

    if(query.numRowsAffected() == 0)
    {
        _lastError = QString("Сканирование с ID %1 не найдено").arg(scanId);
        return false;
    }

    return true;
}