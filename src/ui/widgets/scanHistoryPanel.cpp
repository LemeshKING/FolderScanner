#include "scanHistoryPanel.h"

#include "scanHistoryView.h"
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
    setObjectName("scanHistoryPanel");
    setAttribute(Qt::WA_StyledBackground, true);
    setupUi();
    setupStyle();

    if(model)
    {
        connect(_model, &QAbstractItemModel::modelReset, this, &ScanHistoryPanel::updateCount);
        connect(_model, &QAbstractItemModel::rowsInserted, this, &ScanHistoryPanel::updateCount);
        connect(_model, &QAbstractItemModel::rowsRemoved, this, &ScanHistoryPanel::updateCount);
    }
    connect(_tableView, &ScanHistoryView::scanSelected, this, &ScanHistoryPanel::scanSelected);

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

    _tableView = new ScanHistoryView(this);
    _tableView->setScanModel(_model);

    mainLayout->addWidget(_tableView, 1);
}

void ScanHistoryPanel::updateCount()
{
    const int count = _model->rowCount();

    _countLabel->setText(QString("Сканирований: %1").arg(count));
}

qint64 ScanHistoryPanel::selectedScanId() const
{
    return _tableView->selectedScanId();
}

void ScanHistoryPanel::selectScan(const qint64 &scanId)
{
    _tableView->selectScan(scanId);
}