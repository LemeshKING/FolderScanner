#include "comparisonService.h"
#include "services/scanService.h"

ComparisonService::ComparisonService(ScanService *scanService)
    : _scanService(scanService)
{}

QString ComparisonService::lastError() const
{
    return _lastError;
}

bool ComparisonService::compareWithCurrent(const qint64 &scanId, ComparisonResult &outResult)
{
    _lastError.clear();
    outResult = ComparisonResult{};

    if(scanId <= 0)
    {
        _lastError = "Некорректный ID сканирования.";
        return false;
    }

    ScanInfo savedScan;

    QList<FileEntry> savedEntries;
    QList<ScanError> savedErrors;

    if(!_scanService->loadScan(scanId, savedScan, savedEntries, savedErrors))
    {
        _lastError = _scanService->lastError();
        return false;
    }

    if(savedScan.status == ScanStatus::Failed)
    {
        _lastError = "Невозможно сравнить: снимок завершился с ошибкой.";
        return false;
    }

    FileSystemScanResult currentScan = _scanner.scan(savedScan.rootPath);

    if(!currentScan.rootFound)
    {
        _lastError = "Не удалось просканировать текущую папку.";

        if(!currentScan.errors.isEmpty())
            _lastError += "\n" + currentScan.errors.first().message;

        return false;
    }

    outResult.scanId = savedScan.scanId;
    outResult.rootPath = savedScan.rootPath;
    outResult.snapshotStatus = savedScan.status;

    outResult.snapshotErrors = savedErrors;
    outResult.currentErrors = currentScan.errors;

    outResult.currentScanComplete = currentScan.errors.isEmpty();

    const bool snapshotComplete = savedScan.status == ScanStatus::Complete && savedErrors.isEmpty();

    QHash<QString, FileEntry> savedFiles;

    for(const FileEntry &entry : std::as_const(savedEntries))
    {
        if(entry.type != EntryType::File)
            continue;

        savedFiles.insert(entry.relativePathKey, entry);
    }

    QHash<QString, FileEntry> currentFiles;

    for(const ScannedEntry &scannedEntry : std::as_const(currentScan.entries))
    {
        const FileEntry &entry = scannedEntry.entry;

        if(entry.type != EntryType::File)
            continue;

        currentFiles.insert(entry.relativePathKey, entry);

        const auto savedIt = savedFiles.constFind(entry.relativePathKey);

        if(savedIt == savedFiles.constEnd())
        {
            if(snapshotComplete)
            {
                ComparisonEntry change;

                change.type = ComparisonChangeType::New;
                change.relativePath = entry.relativePath;
                change.currentSizeBytes = entry.sizeBytes;

                outResult.changes.append(change);
            }

            continue;
        }

        const FileEntry &savedEntry = savedIt.value();

        if(savedEntry.sizeBytes != entry.sizeBytes)
        {
            ComparisonEntry change;

            change.type = ComparisonChangeType::Modified;
            change.relativePath = entry.relativePath;
            change.previousSizeBytes = savedEntry.sizeBytes;
            change.currentSizeBytes = entry.sizeBytes;

            outResult.changes.append(change);
        }
    }

    if(outResult.canDetermineDeletedFiles())
    {
        for(const FileEntry &savedEntry : std::as_const(savedEntries))
        {
            if(savedEntry.type != EntryType::File)
                continue;

            if(!currentFiles.contains(savedEntry.relativePathKey))
            {
                ComparisonEntry change;

                change.type = ComparisonChangeType::Deleted;
                change.relativePath = savedEntry.relativePath;
                change.previousSizeBytes = savedEntry.sizeBytes;

                outResult.changes.append(change);
            }
        }
    }

    std::sort(outResult.changes.begin(), outResult.changes.end(), [](const ComparisonEntry &a, const ComparisonEntry &b)
    {
        return QString::compare(a.relativePath, b.relativePath, Qt::CaseInsensitive) < 0;
    });

    return true;
}