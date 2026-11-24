#include "fileTreePanel.h"
#include "fileTreeView.h"
#include "models/fileTreeModel.h"
#include "ui/styleManager.h"
#include "fileEntryDetailsPanel.h"

#include <QLabel>
#include <QVBoxLayout>

FileTreePanel::FileTreePanel(FileTreeModel *model, QWidget *parent)
    : QWidget(parent)
{
    setObjectName("fileTreePanel");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(10);

    _titleLabel = new QLabel("Файлы и папки", this);
    _titleLabel->setObjectName("fileTreeTitle");

    _treeView = new FileTreeView(this);
    _detailsPanel = new FileEntryDetailsPanel(this);

    mainLayout->addWidget(_titleLabel);
    mainLayout->addWidget(_treeView, 1);
    mainLayout->addWidget(_detailsPanel);

    connect(_treeView, &FileTreeView::entrySelected, this, &FileTreePanel::onEntrySelected);

    setModel(model);

    setupStyle();
}

void FileTreePanel::setupStyle()
{
    QString error;

    if(!StyleManager::applyStyle(this, ":/styles/styles/file-tree-panel.qss"))
        qWarning() << error;
}

void FileTreePanel::setModel(FileTreeModel *model)
{
    if(_modelResetConnection)
        disconnect(_modelResetConnection);

    _treeView->setFileModel(model);
    _detailsPanel->clear();

    if(model)
        _modelResetConnection = connect(model, &QAbstractItemModel::modelReset, _detailsPanel, &FileEntryDetailsPanel::clear);

}

QModelIndex FileTreePanel::currentIndex() const
{
    return _treeView->currentIndex();
}

void FileTreePanel::selectIndex(const QModelIndex &index)
{
    _treeView->selectIndex(index);
}

void FileTreePanel::onEntrySelected(const QModelIndex &index)
{
    _detailsPanel->setEntry(index);
    emit entrySelected(index);
}