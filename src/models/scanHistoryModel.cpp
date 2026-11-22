#include "scanHistoryModel.h"

#include <QDir>
#include <algorithm>

ScanHistoryModel::ScanHistoryModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

void ScanHistoryModel::setScans(const QList<ScanInfo> &scans)
{
    beginResetModel();

    _scans = scans;

    std::stable_sort(_scans.begin(), _scans.end(), [](const ScanInfo &a, const ScanInfo &b)
    {
        if(a.scannedAtUtc.isValid() != b.scannedAtUtc.isValid())
            return a.scannedAtUtc.isValid();

        if(!a.scannedAtUtc.isValid())
            return false;

        return a.scannedAtUtc > b.scannedAtUtc;
    });

    endResetModel();
}

void ScanHistoryModel::clear()
{
    beginResetModel();

    _scans.clear();

    endResetModel();
}

int ScanHistoryModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return _scans.size();
}

int ScanHistoryModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return ColumnCount;
}

QVariant ScanHistoryModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return {};

    if(index.row() < 0 || index.row() >= _scans.size())
        return {};

    const ScanInfo &scan = _scans.at(index.row());

    if(role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case PathColumn:
            return QDir::toNativeSeparators(scan.rootPath);

        case DateColumn:
            if(scan.scannedAtUtc.isValid())
                return scan.scannedAtUtc.toLocalTime().toString("dd.MM.yyyy HH:mm");

            return "-";

        case StatusColumn:
            return statusToString(scan.status);

        }
    }

    if(role == Qt::TextAlignmentRole)
        if(index.column() == DateColumn || index.column() == StatusColumn)
            return static_cast<int>(Qt::AlignCenter | Qt::AlignVCenter);

    if(role == Qt::ToolTipRole)
    {
        switch (index.column())
        {
        case PathColumn:
            return scan.rootPath;

        case DateColumn:
            return scan.scannedAtUtc.toString(Qt::ISODate);

        case StatusColumn:
            switch (scan.status)
            {
            case ScanStatus::Complete:
                return "Сканирование завершено полностью.";

            case ScanStatus::Partial:
                return "Сканирование завершено с ошибками.";

            case ScanStatus::Failed:
                return "Не удалось выполнить сканирование.";

            }
        }
    }

    switch (role)
    {
    case ScanIdRole:
        return scan.scanId;

    case RootPathRole:
        return scan.rootPath;

    case ScannedAtUtcRole:
        return scan.scannedAtUtc;

    case StatusRole:
        return static_cast<int>(scan.status);
    }

    return {};
}


QVariant ScanHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch(section)
    {
    case PathColumn:
        return "Путь к папке";

    case DateColumn:
        return "Дата сканирования";

    case StatusColumn:
        return "Статус";
    }

    return {};
}

Qt::ItemFlags ScanHistoryModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QString ScanHistoryModel::statusToString(const ScanStatus &status)
{
    switch(status)
    {
    case ScanStatus::Complete:
        return "COMPLETE";

    case ScanStatus::Partial:
        return "PARTIAL";

    case ScanStatus::Failed:
        return "FAILED";
    }

    return "UNKNOWN";
}
