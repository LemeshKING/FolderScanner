#ifndef SCANSERVICE_H
#define SCANSERVICE_H

#include "core/scnaInfo.h"
#include "scanner/fileSystemScanner.h"
#include "database/scanRepository.h"
#include "database/entryRepository.h"
#include "database/errorRepository.h"

#include <QSqlDatabase>
#include <QString>

class ScanService
{
public:
    ScanService(const QSqlDatabase &database);
    bool scanAndSave(const QString &rootPath, ScanInfo &outScanInfo);
    bool getScanHistory(QList<ScanInfo> &scans);
    bool loadScan(const qint64 &scanId, ScanInfo &scanInfo, QList<FileEntry> &entries, QList<ScanError> &errors);

    QString lastError() const;

private:
    QSqlDatabase _database;

    FileSystemScanner _scanner;

    ScanRepository _scanRepository;
    EntryRepository _entryRepository;
    ErrorRepository _errorRepository;

    QString _lastError;
};

#endif // SCANSERVICE_H
