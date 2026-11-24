#include "comparisonView.h"

#include "models/comparisonModel.h"
#include "ui/styleManager.h"

#include <QAbstractItemView>
#include <QHeaderView>

ComparisonView::ComparisonView(QWidget *parent)
    : QTableView(parent)
{
    setObjectName("comparisonView");

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    setShowGrid(false);
    setAlternatingRowColors(true);
    setWordWrap(false);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    verticalHeader()->hide();
    verticalHeader()->setDefaultSectionSize(42);

    horizontalHeader()->setStretchLastSection(false);
    horizontalHeader()->setHighlightSections(false);

    setCornerButtonEnabled(false);

    setupStyle();
}

void ComparisonView::setupStyle()
{
    QString error;

    if(!StyleManager::applyStyle(this, ":/styles/styles/comparison-view.qss", &error))
        qWarning() << error;
}

void ComparisonView::setComparisonModel(ComparisonModel *model)
{
    if(this->model() == model)
        return;

    QTableView::setModel(model);

    configureHeader();
}

void ComparisonView::configureHeader()
{
    auto *currentModel = model();

    if(!currentModel)
        return;

    auto *header = horizontalHeader();

    for(int column = 0; column < currentModel->columnCount(); column++)
        header->setSectionResizeMode(column, column == 1 ? QHeaderView::Stretch : QHeaderView::ResizeToContents);
}