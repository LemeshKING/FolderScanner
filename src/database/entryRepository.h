#ifndef ENTRYREPOSITORY_H
#define ENTRYREPOSITORY_H

#include "repositoryBase.h"
#include "core/FileEntry.h"

#include <QList>

class EntryRepository : public RepositoryBase
{
public:
    EntryRepository(const QSqlDatabase &database);

    bool insertEntry(FileEntry &entry);
    bool getEntryById(const qint64 &scanId, const qint64 &entryId, FileEntry &outEntry);
    bool getEntriesByScanId(const qint64 &scanId, QList<FileEntry> &outEntries);
    bool getChildren(const qint64 &scanId, const std::optional<qint64> &parentEntryId, QList<FileEntry> &outEntries);
    bool getDirectoryStatistics(const qint64 &scanId, const qint64 &directoryEntryId, qint64 &outFileCount, qint64 &outTotalSizeBytes);

private:
    static QString entryTypeToString(const EntryType &type);

    static bool entryTypeFromString(const QString &value, EntryType &outType);

    bool readEntry(const class QSqlQuery &query, FileEntry &outEntry);

};

#endif // ENTRYREPOSITORY_H
