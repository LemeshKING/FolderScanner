#include "fileTreeModel.h"

#include <QHash>
#include <QLocale>

FileTreeModel::FileTreeModel(QObject *parent)
    : QAbstractItemModel(parent),
    _root(std::make_unique<TreeNode>())
{}

void FileTreeModel::setEntries(const QList<FileEntry> &entries)
{
    beginResetModel();

    _nodes.clear();
    _root = std::make_unique<TreeNode>();

    QHash<qint64, TreeNode*> nodesById;

    for(const FileEntry &entry : entries)
    {
        auto node = std::make_unique<TreeNode>();
        node->entry = entry;
        TreeNode *nodePtr = node.get();
        _nodes.push_back(std::move(node));
        nodesById.insert(entry.entryId, nodePtr);
    }

    for(const auto &ownedNode : _nodes)
    {
        TreeNode *node = ownedNode.get();
        TreeNode *parentNode = _root.get();

        if(node->entry.parentEntryId.has_value())
        {
            const qint64 parentId = node->entry.parentEntryId.value();
            const auto parentIt = nodesById.constFind(parentId);

            if(parentIt != nodesById.constEnd() && parentIt.value() != node && parentIt.value()->entry.type == EntryType::Directory)
                parentNode = parentIt.value();

        }

        node->parent = parentNode;
        parentNode->childrens.append(node);
    }

    calculateStatistics(_root.get());
    endResetModel();
}

void FileTreeModel::clear()
{
    setEntries({});
}

FileTreeModel::TreeNode *FileTreeModel::nodeFromIndex(const QModelIndex &index) const
{
    if(!index.isValid())
        return _root.get();

    return static_cast<TreeNode*>(index.internalPointer());
}

QModelIndex FileTreeModel::index(int row, int column, const QModelIndex &parentIndex) const
{
    if(row < 0 || column < 0 || column >= columnCount())
        return {};

    if(parentIndex.isValid() && parentIndex.column() != 0)
        return {};

    TreeNode *parentNode = nodeFromIndex(parentIndex);

    if(row >= parentNode->childrens.size())
        return {};

    TreeNode *childNode = parentNode->childrens.at(row);
    return createIndex(row, column, childNode);
}

QModelIndex FileTreeModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return {};

    TreeNode *childNode = nodeFromIndex(child);
    TreeNode *parentNode = childNode->parent;

    if(parentNode == nullptr || parentNode == _root.get())
        return {};

    TreeNode *grandParent = parentNode->parent;

    if(grandParent == nullptr)
        return {};

    const int row = grandParent->childrens.indexOf(parentNode);

    if(row < 0)
        return {};

    return createIndex(row, 0, parentNode);
}

int FileTreeModel::rowCount(const QModelIndex &parentIndex) const
{
    if(parentIndex.isValid() && parentIndex.column() != 0)
        return 0;

    TreeNode *parentNode = nodeFromIndex(parentIndex);
    return parentNode->childrens.size();
}

int FileTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant FileTreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return {};

    TreeNode *node = nodeFromIndex(index);
    const FileEntry &entry = node->entry;

    if(role == Qt::DisplayRole)
    {
        switch (index.column())
        {
            case 0:
                return entry.name;

            case 1:
                return entry.type == EntryType::Directory ? "Папка" : "Файл";

            case 2:
                if(entry.type == EntryType::Directory)
                    return "-";
                return formatSize(entry.sizeBytes);
        }
    }

    if(role == Qt::TextAlignmentRole && index.column() == 2)
        return static_cast<int>(Qt::AlignRight | Qt::AlignVCenter);

    if(role == Qt::ToolTipRole)
    {
        if(entry.type == EntryType::Directory)
            return QString("%1\nФайлов: %2\nОбщий размер: %3").arg(entry.relativePath).arg(node->fileCount).arg(formatSize(node->totalSizeBytes));

        return QString("%1\nРазмер: %2").arg(entry.relativePath).arg(formatSize(entry.sizeBytes));
    }

    switch (role)
    {
        case EntryIdRole:
            return entry.entryId;

        case RelativePathRole:
            return entry.relativePath;

        case SizeBytesRole:
            return entry.sizeBytes;

        case FileCountRole:
            return node->fileCount;

        case TotalSizeBytesRole:
            return node->totalSizeBytes;
    }

    return {};
}

QVariant FileTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section)
    {
        case 0:
            return "Имя";

        case 1:
            return "Тип";

        case 2:
            return "Размер";

    }

    return {};
}

Qt::ItemFlags FileTreeModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void FileTreeModel::calculateStatistics(TreeNode *node)
{
    node->fileCount = 0;
    node->totalSizeBytes = 0;

    for(TreeNode *&child : node->childrens)
    {
        if(child->entry.type == EntryType::File)
        {
            child->fileCount = 1;
            child->totalSizeBytes = child->entry.sizeBytes;
        }
        else
            calculateStatistics(child);

        node->fileCount += child->fileCount;
        node->totalSizeBytes += child->totalSizeBytes;
    }
}

QString FileTreeModel::formatSize(const qint64 &sizeBytes)
{
    if(sizeBytes < 0)
        return "-";

    static const QStringList units = { "Б", "КБ", "МБ", "ГБ", "ТБ" };

    double size = static_cast<double>(sizeBytes);
    int unitIndex = 0;

    while(size >= 1024.0 && unitIndex < units.size() - 1)
    {
        size /= 1024;
        unitIndex++;
    }

    if(unitIndex == 0)
        return QString("%1 %2").arg(sizeBytes).arg(units.at(0));

    return QString("%1 %2").arg(QLocale().toString(size, 'f', 1)).arg(units.at(unitIndex));
}








