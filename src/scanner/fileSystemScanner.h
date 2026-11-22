#ifndef FILESYSTEMSCANNER_H
#define FILESYSTEMSCANNER_H

#include "core/FileEntry.h"
#include "core/ScanError.h"

#include <QList>
#include <QString>

#include <optional>

struct ScannedEntry
{
    FileEntry entry;
    std::optional<QString> parentRelativePathKey;
};

struct FileSystemScanResult
{
    bool rootFound = false;

    QList<ScannedEntry> entries;
    QList<ScanError> errors;
};

class FileSystemScanner
{
public:
    FileSystemScanner();

    FileSystemScanResult scan(const QString &rootPath) const;

private:
    void scanDirectory(const QString &absolutePath, const QString &relativePath, const QString &relativePathKey, FileSystemScanResult &result) const;

    static QString makeRelativePathKey(const QString &relativePath);
    static void addError(FileSystemScanResult &result, const QString &path, const QString &message);
};

#endif // FILESYSTEMSCANNER_H
