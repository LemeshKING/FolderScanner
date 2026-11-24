#include "fileTreeView.h"
#include "models/fileTreeModel.h"
#include "ui/styleManager.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QItemSelectionModel>

FileTreeView::FileTreeView(QWidget *parent)
    : QTreeView(parent)
{
    setObjectName("fileTreeView");

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    setAlternatingRowColors(true);
    setUniformRowHeights(true);
    setAnimated(false);

    setIconSize(QSize(18, 18));
    setIndentation(22);
    setRootIsDecorated(true);
    setExpandsOnDoubleClick(true);

    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    setFocusPolicy(Qt::NoFocus);

    setupStyle();
}

void FileTreeView::setupStyle()
{
    QString error;

    if(!StyleManager::applyStyle(this, ":/styles/styles/file-tree-view.qss"))
        qWarning() << error;
}

void FileTreeView::setFileModel(FileTreeModel *model)
{
    if(this->model() == model)
        return;

    if(_selectionConnection)
        disconnect(_selectionConnection);

    QTreeView::setModel(model);

    configureHeader();

    if(!model)
        return;

    _selectionConnection = connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &FileTreeView::onCurrentChanged);
}

void FileTreeView::onCurrentChanged(const QModelIndex &current)
{
    if(!model())
        return;

    emit entrySelected(current);
}

void FileTreeView::configureHeader()
{
    auto *header = this->header();
    auto *currentModel = model();

    header->setStretchLastSection(false);

    if(!currentModel)
        return;

    for(int column = 0; column < currentModel->columnCount(); column++)
        header->setSectionResizeMode(column, column == 0 ? QHeaderView::Stretch : QHeaderView::Interactive);

    header->resizeSection(1, 90);
    header->resizeSection(2, 115);

}

void FileTreeView::selectIndex(const QModelIndex &index)
{
    if(!index.isValid() || index.model() != model())
        return;

    for(QModelIndex parent = index.parent(); parent.isValid(); parent = parent.parent())
        expand(parent);

    selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    scrollTo(index, QAbstractItemView::PositionAtCenter);

}
