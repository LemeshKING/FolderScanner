#include "comparisonModel.h"

#include <QBrush>
#include <QColor>
#include <QLocale>
#include <QVariant>

ComparisonModel::ComparisonModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

void ComparisonModel::setResult(const ComparisonResult &result)
{
    beginResetModel();

    _result = result;

    endResetModel();
}

void ComparisonModel::clear()
{
    beginResetModel();

    _result = ComparisonResult{};

    endResetModel();
}

int ComparisonModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return _result.changes.size();
}

int ComparisonModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return ColumnCount;
}

QVariant ComparisonModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return {};

    if(index.row() < 0 || index.row() >= _result.changes.size())
        return {};

    if(index.column() < 0 || index.column() >= ColumnCount)
        return {};

    const auto &entry = _result.changes.at(index.row());

    if(role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case ChangeColumn:
            return changeTypeToString(entry.type);

        case PathColumn:
            return entry.relativePath;

        case PreviousSizeColumn:
            return entry.previousSizeBytes.has_value() ? formatSize(*entry.previousSizeBytes) : QString("-");

        case CurrentSizeColumn:
            return entry.currentSizeBytes.has_value() ? formatSize(*entry.currentSizeBytes) : QString("-");
        }
    }

    if(role == Qt::TextAlignmentRole)
    {
        if(index.column() == ChangeColumn)
            return static_cast<int>(Qt::AlignCenter | Qt::AlignVCenter);

        if(index.column() == PreviousSizeColumn || index.column() == CurrentSizeColumn)
            return static_cast<int>(Qt::AlignRight | Qt::AlignVCenter);
    }

    if(role == Qt::ForegroundRole && index.column() == ChangeColumn)
    {
        switch (entry.type)
        {
        case ComparisonChangeType::New:
            return QBrush(QColor("#63C58B"));

        case ComparisonChangeType::Deleted:
            return QBrush(QColor("#E57373"));

        case ComparisonChangeType::Modified:
            return QBrush(QColor("#E5B96A"));
        }
    }

    if(role == Qt::ToolTipRole)
    {
        switch (entry.type)
        {
        case ComparisonChangeType::New:
            return QString("Новый файл: %1").arg(entry.relativePath);

        case ComparisonChangeType::Deleted:
            return QString("Удаленный файл: %1").arg(entry.relativePath);

        case ComparisonChangeType::Modified:
            return QString("Изменённый файл: %1").arg(entry.relativePath);
        }
    }

    switch (role)
    {
    case ChangeTypeRole:
        return static_cast<int>(entry.type);

    case RelativePathRole:
        return entry.relativePath;

    case PreviousSizeBytesRole:
        return entry.previousSizeBytes.value_or({});

    case CurrentSizeBytesRole:
        return entry.currentSizeBytes.value_or({});

    }

    return {};
}

QVariant ComparisonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section)
    {
    case ChangeColumn:
        return "Изменение";

    case PathColumn:
        return "Файл";

    case PreviousSizeColumn:
        return "Размер до";

    case CurrentSizeColumn:
        return "Размер после";

    }

    return {};
}


Qt::ItemFlags ComparisonModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

const ComparisonResult &ComparisonModel::result() const
{
    return _result;
}

int ComparisonModel::countByType(const ComparisonChangeType &type) const
{
    int count = 0;

    for(const ComparisonEntry &entry : _result.changes)
        if(entry.type == type)
            count++;

    return count;
}

QString ComparisonModel::changeTypeToString(const ComparisonChangeType &type)
{
    switch (type)
    {
    case ComparisonChangeType::New:
        return "NEW";

    case ComparisonChangeType::Deleted:
        return "DELETED";

    case ComparisonChangeType::Modified:
        return "MODIFIED";
    }

    return "UNKNOWN";
}

QString ComparisonModel::formatSize(const qint64 &sizeBytes)
{
    if(sizeBytes < 0)
        return "-";

    static const QStringList units = { "Б", "КБ", "МБ", "ГБ", "ТБ" };

    double size = static_cast<double>(sizeBytes);

    int unitIndex = 0;

    while(size >= 1024.0 && unitIndex < units.size() - 1)
    {
        size /= 1024.0;
        unitIndex++;
    }

    if(unitIndex == 0)
        return QString("%1 %2").arg(sizeBytes).arg(units.at(0));

    return QString("%1 %2").arg(QLocale().toString(size, 'f', 1)).arg(units.at(unitIndex));
}