#ifndef PREPAREDFILETREEDATA_H
#define PREPAREDFILETREEDATA_H

#include "core/FileEntry.h"

#include <QList>
#include <QMetaType>

#include <memory>
#include <vector>

class FileTreeModel;

class PreparedFileTreeData
{
public:
    PreparedFileTreeData() = default;
    ~PreparedFileTreeData() = default;

private:
    friend class FileTreeModel;

    struct TreeNode
    {
        FileEntry entry;

        TreeNode *parent = nullptr;
        QList<TreeNode *> childrens;

        qint64 fileCount = 0;
        qint64 totalSizeBytes = 0;
    };

    std::unique_ptr<TreeNode> root = std::make_unique<TreeNode>();

    std::vector<std::unique_ptr<TreeNode>> nodes;
};

using PreparedFileTreeDataPtr = std::shared_ptr<PreparedFileTreeData>;

Q_DECLARE_METATYPE(PreparedFileTreeDataPtr)

#endif // PREPAREDFILETREEDATA_H
