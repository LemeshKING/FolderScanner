#include "scanService.h"

#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QSqlError>
#include <QDebug>

ScanService::ScanService(const QSqlDatabase &database)
    : _database(database),
    _scanRepository(database),
    _entryRepository(database),
    _errorRepository(database)
{}

QString ScanService::lastError() const
{
    return _lastError;
}

bool ScanService::scanAndSave(const QString &rootPath, ScanInfo &outScanInfo)
{
    _lastError.clear();
    outScanInfo = ScanInfo{};

    FileSystemScanResult scanResult = _scanner.scan(rootPath);

    outScanInfo.rootPath = rootPath.trimmed().isEmpty() ? rootPath : QDir::cleanPath(QFileInfo(rootPath).absoluteFilePath());
    outScanInfo.scannedAtUtc = QDateTime::currentDateTimeUtc();

    if(!scanResult.rootFound)
        outScanInfo.status = ScanStatus::Failed;
    else if (!scanResult.errors.isEmpty())
        outScanInfo.status = ScanStatus::Partial;
    else
        outScanInfo.status = ScanStatus::Complete;

    if(!_database.isValid() || !_database.isOpen())
    {
        _lastError = "Подключение к базе данных недействительно";
        return false;
    }

    if(!_database.transaction())
    {
        _lastError = "Не удалось начать транзакцию: " + _database.lastError().text();
        return false;
    }

    auto rollbackWithError = [this, &outScanInfo](const QString &error)
    {
        _database.rollback();
        _lastError = error;
        outScanInfo.scanId = -1;
        return false;
    };

    if(!_scanRepository.insertScan(outScanInfo))
        return rollbackWithError("Не удалосб сохранить информацию о сканировании: " + _scanRepository.lastError());

    QHash<QString, qint64> directoryIds;

    for(const ScannedEntry &scannedEntry : scanResult.entries)
    {
        FileEntry entry = scannedEntry.entry;

        entry.scanId = outScanInfo.scanId;
        if(scannedEntry.parentRelativePathKey.has_value())
        {
            const QString parentKey = scannedEntry.parentRelativePathKey.value();

            const auto parentIt = directoryIds.constFind(parentKey);
            if(parentIt == directoryIds.constEnd())
                return rollbackWithError("Не удалось найти родительскую папку для записи: " + entry.relativePath);

            entry.parentEntryId = parentIt.value();

        }
        else
            entry.parentEntryId = std::nullopt;

        if(!_entryRepository.insertEntry(entry))
            return rollbackWithError("Не удалось сохранить элемент файловой системы: " +  entry.relativePath + ". Причина: " + _entryRepository.lastError());

        if(entry.type == EntryType::Directory)
            directoryIds.insert(entry.relativePathKey, entry.entryId);

    }

    for(ScanError &error : scanResult.errors)
    {
        error.scanId = outScanInfo.scanId;
        if(!_errorRepository.insertError(error))
            return rollbackWithError("Не удалось сохранить ошибку сканирования: " + _errorRepository.lastError());
    }

    if(!_database.commit())
    {
        const QString error = "Не удалось завершить транзакцию: " + _database.lastError().text();
        return rollbackWithError(error);
    }

    return true;

}

bool ScanService::getScanHistory(QList<ScanInfo> &scans)
{
    _lastError.clear();
    scans.clear();

    if(_database.isValid() || !_database.isOpen())
    {
        _lastError = "База данных не открыта";
        return false;
    }

    if(!_scanRepository.getAllScans(scans))
    {
        _lastError = _scanRepository.lastError();
        return false;
    }

    return true;
}

bool ScanService::loadScan(const qint64 &scanId, ScanInfo &scanInfo, QList<FileEntry> &entries, QList<ScanError> &errors)
{
    _lastError.clear();

    scanInfo = ScanInfo{};
    entries.clear();
    errors.clear();

    if(_database.isValid() || !_database.isOpen())
    {
        _lastError = "База данных не открыта.";
        return false;
    }

    if(scanId <= 0)
    {
        _lastError = "Некорректный индетификатор сканирования.";
        return false;
    }

    if(!_scanRepository.getScanById(scanId, scanInfo))
    {
        _lastError = _scanRepository.lastError();
        return false;
    }

    if(!_entryRepository.getEntriesByScanId(scanId, entries))
    {
        _lastError = _entryRepository.lastError();
        return false;
    }

    if(!_errorRepository.getErrorsByScanId(scanId, errors))
    {
        _lastError = _errorRepository.lastError();
        return false;
    }

    return true;
}