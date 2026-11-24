#include "entryRepository.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QMetaType>

EntryRepository::EntryRepository(const QSqlDatabase &database)
    : RepositoryBase(database)
{}

QString EntryRepository::entryTypeToString(const EntryType &type)
{
    switch (type)
    {
        case EntryType::File:
            return "FILE";

        case EntryType::Directory:
            return "DIR";
        default:
            break;
    }

    return {};
}

bool EntryRepository::entryTypeFromString(const QString &value, EntryType &outType)
{
    if(value == "FILE")
    {
        outType = EntryType::File;
        return true;
    }

    if(value == "DIR")
    {
        outType = EntryType::Directory;
        return true;
    }

    return false;
}

bool EntryRepository::readEntry(const QSqlQuery &query, FileEntry &outEntry)
{
    outEntry.entryId = query.value("entry_id").toLongLong();
    outEntry.scanId = query.value("scan_id").toLongLong();

    QVariant parentId = query.value("parent_entry_id");
    outEntry.parentEntryId.reset();

    if(!parentId.isNull())
        outEntry.parentEntryId = parentId.toLongLong();

    outEntry.name = query.value("name").toString();
    outEntry.relativePath = query.value("relative_path").toString();
    outEntry.relativePathKey = query.value("relative_path_key").toString();

    QString type = query.value("entry_type").toString();
    if(!entryTypeFromString(type, outEntry.type))
    {
        _lastError = "Неизвестный тип файловой записи.";
        return false;
    }

    outEntry.sizeBytes = query.value("size_bytes").toLongLong();

    return true;
}

bool EntryRepository::insertEntry(FileEntry &entry)
{
    clearError();

    if(!ensureDatabaseReady())
        return false;

    if(entry.scanId <= 0)
    {
        _lastError = "Некорректный ID сканирования.";
        return false;
    }

    if(entry.name.isEmpty())
    {
        _lastError = "Имя файла или папки не может быть пустым.";
        return false;
    }

    if(entry.sizeBytes < 0)
    {
        _lastError = "Размер не может быть отрицательным.";
        return false;
    }

    if(entry.parentEntryId.has_value() && entry.parentEntryId.value() <= 0)
    {
        _lastError = "Некорректный ID родительской папки.";
        return false;
    }

    if(entry.type == EntryType::Directory && entry.sizeBytes != 0)
    {
        _lastError = "Размер папки должен быть равен нулю.";
        return false;
    }

    QString type = entryTypeToString(entry.type);

    if(type.isEmpty())
    {
        _lastError = "Неизвестный тип файловой записи.";
        return false;
    }

    QSqlQuery query(_database);

    query.prepare(
        "INSERT INTO scan_entries "
        "(scan_id, parent_entry_id, name, relative_path, relative_path_key, entry_type, size_bytes) "
        "VALUES "
        "(:scan_id, :parent_entry_id, :name, :relative_path, :relative_path_key, :entry_type, :size_bytes)"
    );

    query.bindValue(":scan_id", entry.scanId);

    if(entry.parentEntryId.has_value())
        query.bindValue(":parent_entry_id", *entry.parentEntryId);
    else
        query.bindValue(":parent_entry_id", QVariant(QMetaType::fromType<qint64>()));

    query.bindValue(":name", entry.name);
    query.bindValue(":relative_path", entry.relativePath);
    query.bindValue(":relative_path_key", entry.relativePathKey);
    query.bindValue(":entry_type", type);
    query.bindValue(":size_bytes", entry.sizeBytes);

    if(!query.exec())
    {
        _lastError = query.lastError().text();
        return false;
    }

    entry.entryId = query.lastInsertId().toLongLong();

    if(entry.entryId <= 0)
    {
        _lastError = "Не удалось получить ID добавленной записи.";
        return false;
    }

    return true;
}

bool EntryRepository::getEntryById(const qint64 &scanId, const qint64 &entryId, FileEntry &outEntry)
{
    clearError();

    if(!ensureDatabaseReady())
        return false;

    QSqlQuery query(_database);

    query.prepare("SELECT entry_id, scan_id, parent_entry_id, name, relative_path, relative_path_key, entry_type, size_bytes "
                  "FROM scan_entries "
                  "WHERE scan_id = :scan_id AND entry_id = :entry_id"
    );

    query.bindValue(":scan_id", scanId);
    query.bindValue(":entry_id", entryId);

    if(!query.exec())
    {
        _lastError = query.lastError().text();
        return false;
    }

    if(!query.next())
    {
        _lastError = "Файловая запись не найдена.";
        return false;
    }

    return readEntry(query, outEntry);
}

bool EntryRepository::getEntriesByScanId(const qint64 &scanId, QList<FileEntry> &outEntries)
{
    clearError();
    outEntries.clear();

    if(!ensureDatabaseReady())
        return false;

    QSqlQuery query(_database);

    query.prepare(
        "SELECT entry_id, scan_id, parent_entry_id, name, relative_path, relative_path_key, entry_type, size_bytes "
        "FROM scan_entries "
        "WHERE scan_id = :scan_id "
        "ORDER BY entry_id ASC"
    );

    query.bindValue(":scan_id", scanId);
    if(!query.exec())
    {
        _lastError = query.lastError().text();
        return false;
    }

    while(query.next())
    {
        FileEntry entry;

        if(!readEntry(query, entry))
        {
            outEntries.clear();
            return false;
        }

        outEntries.append(entry);
    }

    return true;
}

bool EntryRepository::getChildren(const qint64 &scnaId, const std::optional<qint64> &parentEntryId, QList<FileEntry> &outEntries)
{
    clearError();
    outEntries.clear();

    if(!ensureDatabaseReady())
        return false;

    QString sql =
        "SELECT entry_id, scan_id, parent_entry_id, name, relative_path, relative_path_key, entry_type, size_bytes "
        "FROM scan_entries "
        "WHERE scan_id = :scan_id AND ";

    if(parentEntryId.has_value())
        sql += "parent_entry_id = :parent_entry_id ";
    else
        sql += "parent_entry_id IS NULL ";

    sql += "ORDER BY name COLLATE NOCASE";

    QSqlQuery query(_database);

    query.prepare(sql);

    query.bindValue(":scan_id", scnaId);

    if(parentEntryId.has_value())
        query.bindValue(":parent_entry_id", *parentEntryId);

    if(!query.exec())
    {
        _lastError = query.lastError().text();
        return false;
    }

    while(query.next())
    {
        FileEntry entry;

        if(!readEntry(query, entry))
        {
            outEntries.clear();
            return false;
        }

        outEntries.append(entry);
    }

    return true;
}

bool EntryRepository::getDirectoryStatistics(const qint64 &scanId, const qint64 &directoryEntryId, qint64 &outFileCount, qint64 &outTotalSizeBytes)
{
    clearError();

    outFileCount = 0;
    outTotalSizeBytes = 0;

    if(!ensureDatabaseReady())
        return false;

    QSqlQuery checkQuery(_database);
    checkQuery.prepare(
        "SELECT entry_type "
        "FROM scan_entries "
        "WHERE scan_id = :scan_id "
        "AND entry_id = :entry_id"
    );

    checkQuery.bindValue(":scan_id", scanId);
    checkQuery.bindValue(":entry_id", directoryEntryId);

    if(!checkQuery.exec())
    {
        _lastError = checkQuery.lastError().text();
        return false;
    }

    if(!checkQuery.next() || checkQuery.value(0).toString() != "DIR")
    {
        _lastError = "Указанная папка не найдена";
        return false;
    }

    QSqlQuery query(_database);

    query.prepare(R"SQL(
        WITH RECURSIVE subtree(entry_id, entry_type, size_bytes) AS
        (
            SELECT entry_id, entry_type, size_bytes
            FROM scan_entries
            WHERE scan_id = :scan_id
            AND entry_id = :directory_entry_id

            UNION ALL

            SELECT child.entry_id, child.entry_type, child.size_bytes
            FROM scan_entries AS child
            INNER JOIN subtree AS parent
                ON child.parent_entry_id = parent.entry_id
            WHERE child.scan_id = :scan_id_child
        )

        SELECT SUM(CASE WHEN entry_type = 'FILE' THEN 1 ELSE 0 END) AS file_count,
        COALESCE (SUM (CASE WHEN entry_type = 'FILE' THEN size_bytes ELSE 0 END), 0) AS total_size
        FROM subtree;
    )SQL");

    query.bindValue(":scan_id", scanId);
    query.bindValue(":scan_id_child", scanId);
    query.bindValue(":directory_entry_id", directoryEntryId);

    if(!query.exec())
    {
        _lastError = query.lastError().text();
        return false;
    }

    if(!query.next())
    {
        _lastError = "Не удалось получить статистику папки: " + query.lastError().text();
        return false;
    }

    outFileCount = query.value("file_count").toLongLong();
    outTotalSizeBytes = query.value("total_size").toLongLong();

    return true;
}
