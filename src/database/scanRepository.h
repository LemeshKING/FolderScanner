#ifndef SCANREPOSITORY_H
#define SCANREPOSITORY_H

#include <QSqlDatabase>
#include <QList>
#include <QString>

#include "database/repositoryBase.h"
#include "core/scnaInfo.h"

class ScanRepository : public RepositoryBase
{
public:
    ScanRepository(const QSqlDatabase &database);

    bool insertScan(ScanInfo &scan);
    bool getScanById(const qint64 &scanId, ScanInfo &outScan);
    bool getAllScans(QList<ScanInfo> &outScans);
    bool updateScanStatus(const qint64 &scanId, const ScanStatus &status);
    bool deleteScan(qint64 scnaId);


private:
    bool readScan(const class QSqlQuery &query, ScanInfo &outScan);

    static QString statusToString(const ScanStatus &status);

    static bool statusFromString(const QString &value, ScanStatus &outStatus);


};

#endif // SCANREPOSITORY_H
