#ifndef FILEOPERATIONWORKER_H
#define FILEOPERATIONWORKER_H

#include "core/ComparisonResult.h"
#include "core/ScanLoadResult.h"

#include <QObject>
#include <QString>

class FileOperationWorker : public QObject
{
    Q_OBJECT

public:
    FileOperationWorker(const QString &databasePath);

public slots:
    void scanFolder(const QString &rootPath);
    void compareScan(const qint64 &scanId);
    void loadScan(const qint64 &scanId);

signals:
    void scanFinished(bool succes, const qint64 &scanId, const QString &error);
    void comparisonFinished(bool succes, const ComparisonResult &result, const QString &error);
    void scanLoadFinished(const ScanLoadResult &result);

private:
    QString _databasePath;
};

#endif // FILEOPERATIONWORKER_H
