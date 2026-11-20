#ifndef SCNAINFO_H
#define SCNAINFO_H

#include <QString>
#include <QDateTime>

enum class ScanStatus
{
    Complete,
    Partial,
    Failed
};

struct ScanInfo
{
    qint64 scanId = -1;
    QString rootPath;
    QDateTime scannedAtUtc;
    ScanStatus status = ScanStatus::Partial;
};

#endif // SCNAINFO_H
