#include "fileOperationWorker.h"

#include "database/databaseManager.h"
#include "services/scanService.h"
#include "services/comparisonService.h"
#include "models/fileTreeModel.h"

FileOperationWorker::FileOperationWorker(const QString &databasePath)
    : _databasePath(databasePath)
{}

void FileOperationWorker::scanFolder(const QString &rootPath)
{
    bool success = false;
    qint64 scanId = -1;
    QString error;

    {
        DatabaseManager database(_databasePath);

        if(!database.initializeDatabase())
            error = database.lastError();
        else
        {
            ScanService scanService(database.connection());

            ScanInfo scanInfo;

            success = scanService.scanAndSave(rootPath, scanInfo);

            if(success)
                scanId = scanInfo.scanId;
            else
                error = scanService.lastError();
        }

    }

    emit scanFinished(success, scanId, error);
}

void FileOperationWorker::compareScan(const qint64 &scanId)
{
    bool success = false;
    QString error;

    ComparisonResult result;

    {
        DatabaseManager database(_databasePath);

        if(!database.initializeDatabase())
            error = database.lastError();
        else
        {
            ScanService scanService(database.connection());

            ComparisonService comparisonService(&scanService);

            success = comparisonService.compareWithCurrent(scanId, result);

            if(!success)
                error = comparisonService.lastError();
        }
    }

    emit comparisonFinished(success, result, error);
}

void FileOperationWorker::loadScan(const qint64 &scanId)
{
    ScanLoadResult result;

    {
        DatabaseManager database(_databasePath);

        if(!database.initializeDatabase())
            result.error = database.lastError();
        else
        {
            ScanService scanService(database.connection());

            QList<FileEntry> entries;

            if(!scanService.loadScan(scanId, result.scanInfo, entries, result.errors))
                result.error = scanService.lastError();
            else
            {
                result.treeData = FileTreeModel::prepareEntries(entries);
                result.success = true;
            }
        }
    }

    emit scanLoadFinished(result);
}