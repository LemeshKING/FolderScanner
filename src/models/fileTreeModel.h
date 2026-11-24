#ifndef FILETREEMODEL_H
#define FILETREEMODEL_H

#include "core/FileEntry.h"
#include "preparedFileTreeData.h"

#include <QAbstractItemModel>
#include <QList>

#include <memory>
#include <vector>

class FileTreeModel : public QAbstractItemModel
{
public:
    enum CustomRole
    {
        EntryIdRole = Qt::UserRole + 1,
        RelativePathRole,
        EntryTypeRole,
        SizeBytesRole,
        FileCountRole,
        TotalSizeBytesRole
    };


    FileTreeModel(QObject *parent = nullptr);

    void setEntries(const QList<FileEntry> &entries);
    void clear();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int sectron, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    static QString formatSize(const qint64 &sizeBytes);

    static PreparedFileTreeDataPtr prepareEntries(const QList<FileEntry> &entries);

    void setPrepareData(PreparedFileTreeDataPtr data);

private:

    using TreeNode = PreparedFileTreeData::TreeNode;

    TreeNode *nodeFromIndex(const QModelIndex &index) const;
    static void calculateStatistics(TreeNode *node);

    PreparedFileTreeDataPtr _preparedData;

};

#endif // FILETREEMODEL_H
