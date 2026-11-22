#include "fileSystemScanner.h"

#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QtGlobal>

FileSystemScanner::FileSystemScanner() {}

FileSystemScanResult FileSystemScanner::scan(const QString &rootPath) const
{
    FileSystemScanResult result;

    if(rootPath.trimmed().isEmpty())
    {
        addError(result, rootPath, "Путь к папке не указан.");
        return result;
    }

    QFileInfo rootInfo(rootPath);

    const QString absoluteRootPath = QDir::cleanPath(rootInfo.absoluteFilePath());

    if(!rootInfo.exists() || !rootInfo.isDir())
    {
        addError(result, absoluteRootPath, "Указанный путь не существует или не является папкой.");
        return result;
    }

    if(rootInfo.isSymLink())
    {
        addError(result, absoluteRootPath, "Корневая папка является символической ссылкой.");
        return result;
    }

    result.rootFound = true;
    FileEntry rootEntry;

    rootEntry.name = rootInfo.fileName();
    if(rootEntry.name.isEmpty())
        rootEntry.name = absoluteRootPath;

    rootEntry.relativePath = ".";
    rootEntry.relativePathKey = makeRelativePathKey(".");
    rootEntry.type = EntryType::Directory;
    rootEntry.sizeBytes = 0;

    ScannedEntry scannedRoot;
    scannedRoot.entry = rootEntry;
    scannedRoot.parentRelativePathKey = std::nullopt;

    result.entries.append(scannedRoot);

    scanDirectory(absoluteRootPath, QString(), rootEntry.relativePathKey, result);

    return result;
}

void FileSystemScanner::scanDirectory(const QString &absolutePath, const QString &relativePath, const QString &relativePathKey, FileSystemScanResult &result) const
{
    QFileInfo directoryInfo(absolutePath);

    if(!directoryInfo.exists() || !directoryInfo.isDir())
    {
        addError(result, absolutePath, "Папка больше не существует или недоступна.");
        return;
    }

    if(!directoryInfo.isReadable())
    {
        addError(result, absolutePath, "Нет доступа для чтения папки.");
        return;
    }

    QDir directory(absolutePath);

    const QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System;

    const QDir::SortFlags sorting = QDir::Name | QDir::DirsFirst | QDir::IgnoreCase;

    const QFileInfoList children = directory.entryInfoList(filters, sorting);

    for(const QFileInfo &childInfo : children)
    {
        if(childInfo.isSymLink())
        {
            addError(result, childInfo.absoluteFilePath(), "Символическая ссылка пропущена");
            continue;
        }

        if(!childInfo.exists())
        {
            addError(result, childInfo.absoluteFilePath(), "Элемент больше не существует.");
            continue;
        }

        QString childRelativePath;

        childRelativePath = relativePath + "/" + childInfo.fileName();
        if(relativePath.isEmpty())
            childRelativePath = childInfo.fileName();

        childRelativePath = QDir::fromNativeSeparators(QDir::cleanPath(childRelativePath));

        const QString childRelativePathKey = makeRelativePathKey(childRelativePath);

        if(childInfo.isDir())
        {
            FileEntry entry;

            entry.name = childInfo.fileName();
            entry.relativePath = childRelativePath;
            entry.relativePathKey = childRelativePathKey;
            entry.type = EntryType::Directory;
            entry.sizeBytes = 0;

            ScannedEntry scannedEntry;
            scannedEntry.entry = entry;
            scannedEntry.parentRelativePathKey = relativePathKey;

            result.entries.append(scannedEntry);
            scanDirectory(childInfo.absoluteFilePath(), childRelativePath, childRelativePathKey, result);
        }
        else if(childInfo.isFile())
        {
            const qint64 fileSize = childInfo.size();
            if(fileSize < 0)
            {
                addError(result, childInfo.absoluteFilePath(), "Не удалось определить размер файла.");
                continue;
            }

            FileEntry entry;
            entry.name = childInfo.fileName();
            entry.relativePath = childRelativePath;
            entry.relativePathKey = childRelativePathKey;
            entry.type = EntryType::File;
            entry.sizeBytes = fileSize;

            ScannedEntry scannedEntry;
            scannedEntry.entry = entry;
            scannedEntry.parentRelativePathKey = relativePathKey;
            result.entries.append(scannedEntry);
        }
    }
}

QString FileSystemScanner::makeRelativePathKey(const QString &relativePath)
{
    QString key = QDir::fromNativeSeparators(QDir::cleanPath(relativePath));

#ifdef Q_OS_WIN
    key = key.toCaseFolded();
#endif

    return key;
}

void FileSystemScanner::addError(FileSystemScanResult &result, const QString &path, const QString &message)
{
    ScanError error;

    error.path = QDir::toNativeSeparators(path);
    error.message = message;

    result.errors.append(error);
}