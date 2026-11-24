#ifndef COMPARISONRESULT_H
#define COMPARISONRESULT_H

#include "core/scnaInfo.h"
#include "core/ScanError.h"

#include <QList>
#include <QString>
#include <QMetaType>

#include <optional>

enum class ComparisonChangeType
{
    New,
    Deleted,
    Modified
};

struct ComparisonEntry
{
    ComparisonChangeType type = ComparisonChangeType::Modified;

    QString relativePath;

    std::optional<qint64> previousSizeBytes;
    std::optional<qint64> currentSizeBytes;
};

struct ComparisonResult
{
    qint64 scanId = -1;
    QString rootPath;

    ScanStatus snapshotStatus = ScanStatus::Partial;

    bool currentScanComplete = false;

    QList<ComparisonEntry> changes;
    QList<ScanError> snapshotErrors;
    QList<ScanError> currentErrors;

    bool canDetermineNewFiles() const
    {
        return snapshotStatus == ScanStatus::Complete && snapshotErrors.isEmpty();
    }

    bool canDetermineDeletedFiles() const
    {
        return currentScanComplete;
    }
};

Q_DECLARE_METATYPE(ComparisonResult)

#endif // COMPARISONRESULT_H
