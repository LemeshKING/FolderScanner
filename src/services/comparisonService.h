#ifndef COMPARISONSERVICE_H
#define COMPARISONSERVICE_H

#include "core/ComparisonResult.h"
#include "scanner/fileSystemScanner.h"

class ScanService;

class ComparisonService
{
public:
    ComparisonService(ScanService *scanService);

    bool compareWithCurrent(const qint64 &scanId, ComparisonResult &outResult);

    QString lastError() const;

private:

    ScanService *_scanService = nullptr;

    FileSystemScanner _scanner;

    QString _lastError;
};

#endif // COMPARISONSERVICE_H
