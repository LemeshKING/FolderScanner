#ifndef FILEENTRY_H
#define FILEENTRY_H

#include <QString>
#include <QtGlobal>

#include <optional>

enum class EntryType
{
    File,
    Directory
};

struct FileEntry
{
    qint64 entryId = -1;
    qint64 scanId = -1;
    qint64 sizeBytes = 0;

    std::optional<qint64> parentEntryId;

    QString name;
    QString relativePath;
    QString relativePathKey;

    EntryType type = EntryType::File;
};


#endif // FILEENTRY_H
