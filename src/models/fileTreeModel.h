#ifndef FILETREEMODEL_H
#define FILETREEMODEL_H

#include "core/FileEntry.h"

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

private:

    struct TreeNode
    {
        FileEntry entry;
        TreeNode *parent = nullptr;
        QList<TreeNode*> childrens;

        qint64 fileCount = 0;
        qint64 totalSizeBytes = 0;
    };

    TreeNode *nodeFromIndex(const QModelIndex &index) const;
    void calculateStatistics(TreeNode *node);
    static QString formatSize(const qint64 &sizeBytes);

    std::unique_ptr<TreeNode> _root;
    std::vector<std::unique_ptr<TreeNode>> _nodes;

};

#endif // FILETREEMODEL_H
