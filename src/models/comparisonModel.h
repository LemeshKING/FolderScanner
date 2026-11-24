#ifndef COMPARISONMODEL_H
#define COMPARISONMODEL_H

#include "core/ComparisonResult.h"

#include <QAbstractTableModel>

class ComparisonModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    enum Column {
        ChangeColumn = 0,
        PathColumn,
        PreviousSizeColumn,
        CurrentSizeColumn,
        ColumnCount
    };

    enum CustomRole
    {
        ChangeTypeRole = Qt::UserRole + 1,
        RelativePathRole,
        PreviousSizeBytesRole,
        CurrentSizeBytesRole
    };

    ComparisonModel(QObject *parent = nullptr);

    void setResult(const ComparisonResult &result);
    void clear();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    const ComparisonResult &result() const;

    int countByType(const ComparisonChangeType &type) const;

    static QString formatSize(const qint64 &sizeBytes);
private:

    static QString changeTypeToString(const ComparisonChangeType &type);

    ComparisonResult _result;

};

#endif // COMPARISONMODEL_H
