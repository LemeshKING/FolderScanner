#include "scanHistoryPanel.h"

#include "ui/styleManager.h"
#include "models/scanHistoryModel.h"

#include <QLabel>
#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include <QScrollBar>

ScanHistoryPanel::ScanHistoryPanel(ScanHistoryModel *model, QWidget *parent)
    : QWidget(parent),
    _model(model)
{
    Q_ASSERT(_model);

    setObjectName("scanHistoryPanel");

    setupUi();
    setupTable();
    setupStyle();

    connect(_model, &QAbstractItemModel::modelReset, this, &ScanHistoryPanel::updateCount);
    connect(_model, &QAbstractItemModel::rowsInserted, this, &ScanHistoryPanel::updateCount);
    connect(_model, &QAbstractItemModel::rowsRemoved, this, &ScanHistoryPanel::updateCount);
    connect(_tableView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &ScanHistoryPanel::onRowChanged);

    updateCount();
}

void ScanHistoryPanel::setupStyle()
{
    QString error;

    if(!StyleManager::applyStyle(this, ":/styles/styles/scan-history-panel.qss"))
        qWarning() << error;
}

void ScanHistoryPanel::setupUi()
{
    auto mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(12);

    auto *headerLayout = new QHBoxLayout();

    _titleLabel = new QLabel("История сканирований", this);
    _titleLabel->setObjectName("scanHistoryTitle");

    _countLabel = new QLabel(this);
    _countLabel->setObjectName("scanHistoryCount");

    headerLayout->addWidget(_titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(_countLabel);

    mainLayout->addLayout(headerLayout);

    _tableView = new QTableView(this);
    _tableView->setObjectName("scanHistoryTable");

    mainLayout->addWidget(_tableView);
}

void ScanHistoryPanel::setupTable()
{
    _tableView->setModel(_model);

    _tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    _tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    _tableView->setShowGrid(false);
    _tableView->setAlternatingRowColors(true);
    _tableView->setWordWrap(false);

    _tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    _tableView->verticalHeader()->hide();
    _tableView->verticalHeader()->setDefaultSectionSize(46);

    QHeaderView *header = _tableView->horizontalHeader();

    header->setHighlightSections(false);
    header->setMinimumSectionSize(90);

    header->setSectionResizeMode(ScanHistoryModel::PathColumn, QHeaderView::Stretch);
    header->setSectionResizeMode(ScanHistoryModel::DateColumn, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(ScanHistoryModel::StatusColumn, QHeaderView::ResizeToContents);

    _tableView->setCornerButtonEnabled(false);
}

void ScanHistoryPanel::updateCount()
{
    const int count = _model->rowCount();

    _countLabel->setText(QString("Сканирований: %1").arg(count));
}

qint64 ScanHistoryPanel::selectedScanId() const
{
    const QModelIndex current = _tableView->currentIndex();

    if(!current.isValid())
        return -1;

    return current.data(ScanHistoryModel::ScanIdRole).toLongLong();
}

bool ScanHistoryPanel::selectScan(const qint64 &scanId)
{
    for(int row = 0; row < _model->rowCount(); row++)
    {
        const QModelIndex index = _model->index(row, ScanHistoryModel::PathColumn);
        const qint64 currentId = index.data(ScanHistoryModel::ScanIdRole).toLongLong();

        if(currentId == scanId)
        {
            _tableView->selectRow(row);
            _tableView->scrollTo(index, QAbstractItemView::PositionAtCenter);
            return true;
        }
    }

    return false;
}

void ScanHistoryPanel::onRowChanged(const QModelIndex &current)
{
    if(!current.isValid())
        return;

    const qint64 scanId = current.data(ScanHistoryModel::ScanIdRole).toLongLong();
    if(scanId > 0)
        emit scanSelected(scanId);
}