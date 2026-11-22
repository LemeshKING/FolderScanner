#ifndef SCANHISTORYMODEL_H
#define SCANHISTORYMODEL_H

#include "core/scnaInfo.h"

#include <QAbstractTableModel>
#include <QList>

class ScanHistoryModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        PathColumn = 0,
        DateColumn,
        StatusColumn,
        ColumnCount
    };

    enum CustomRole
    {
        ScanIdRole = Qt::UserRole + 1,
        RootPathRole,
        ScannedAtUtcRole,
        StatusRole
    };

    ScanHistoryModel(QObject *parent = nullptr);

    void setScans(const QList<ScanInfo> &scans);
    void clear();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:

    static QString statusToString(const ScanStatus &status);

    QList<ScanInfo> _scans;
};

#endif // SCANHISTORYMODEL_H
