#include "scanHistoryView.h"
#include "models/scanHistoryModel.h"
#include "ui/styleManager.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QItemSelectionModel>

ScanHistoryView::ScanHistoryView(QWidget *parent)
    : QTableView(parent)
{
    setObjectName("scanHistoryView");

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    setShowGrid(false);
    setAlternatingRowColors(true);
    setWordWrap(false);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    verticalHeader()->hide();
    verticalHeader()->setDefaultSectionSize(46);

    horizontalHeader()->setStretchLastSection(false);

    setupStyle();
    // header->setSectionResizeMode(ScanHistoryModel::PathColumn, QHeaderView::Stretch);
    // header->setSectionResizeMode(ScanHistoryModel::DateColumn, QHeaderView::ResizeToContents);
    // header->setSectionResizeMode(ScanHistoryModel::StatusColumn, QHeaderView::ResizeToContents);

    // setCornerButtonEnabled(false);
}

void ScanHistoryView::setupStyle()
{
    QString error;

    if(!StyleManager::applyStyle(this, ":/styles/styles/scan-history-view.qss"))
        qWarning() << error;
}

void ScanHistoryView::setScanModel(ScanHistoryModel *model)
{
    if(this->model() == model)
        return;

    if(_selectionConnection)
        disconnect(_selectionConnection);

    QTableView::setModel(model);

    configureHeader();

    if(!model)
        return;

    _selectionConnection = connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &ScanHistoryView::onCurrentChanged);
}

void ScanHistoryView::configureHeader()
{

    auto *currentModel = model();

    if(!currentModel)
        return;

    auto *header = horizontalHeader();

    for(int column = 0; column < currentModel->columnCount(); column++)
        header->setSectionResizeMode(column, column == 0 ? QHeaderView::Stretch : QHeaderView::ResizeToContents);

}

void ScanHistoryView::onCurrentChanged(const QModelIndex &current)
{
    if(!current.isValid() || !model())
        return;

    const QModelIndex idIndex = model()->index(current.row(), 0);

    bool ok = false;

    const qint64 scanId = idIndex.data(ScanHistoryModel::ScanIdRole).toLongLong(&ok);

    if(ok)
        emit scanSelected(scanId);
}

void ScanHistoryView::selectScan(const qint64 &scanId)
{
    auto *currentModel = model();
    if(!currentModel || !selectionModel())
        return;

    for(int row = 0; row < currentModel->rowCount(); row++)
    {
        const QModelIndex index = currentModel->index(row, 0);
        bool ok = false;
        const qint64 currentId = index.data(ScanHistoryModel::ScanIdRole).toLongLong(&ok);

        if(!ok || currentId != scanId)
            continue;

        selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

        scrollTo(index, QAbstractItemView::PositionAtCenter);

        return;
    }
}

qint64 ScanHistoryView::selectedScanId() const
{
    auto *currentModel = model();

    if(!currentModel || !currentIndex().isValid())
        return -1;

    const QModelIndex index = currentModel->index(currentIndex().row(), 0);

    bool ok = false;

    const qint64 scanId = index.data(ScanHistoryModel::ScanIdRole).toLongLong(&ok);

    return ok ? scanId : -1;
}