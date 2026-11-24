#include "comparisonPanel.h"

#include "comparisonView.h"
#include "models/comparisonModel.h"
#include "ui/styleManager.h"

#include <QAbstractItemModel>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

ComparisonPanel::ComparisonPanel(ComparisonModel *model, QWidget *parent)
    : QWidget(parent)
{
    setObjectName("comparisonPanel");
    setAttribute(Qt::WA_StyledBackground, true);

    setupUi();
    setupStyle();

    setComparisonModel(model);
}

void ComparisonPanel::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(12);

    auto *headerLayout = new QHBoxLayout();

    _titleLabel = new QLabel("Результат сравнения", this);
    _titleLabel->setObjectName("comparisonTitle");

    _newCountLabel = new QLabel(this);
    _newCountLabel->setObjectName("newCount");

    _deletedCountLabel = new QLabel(this);
    _deletedCountLabel->setObjectName("deletedCount");

    _modifiedCountLabel = new QLabel(this);
    _modifiedCountLabel->setObjectName("modifiedCount");

    headerLayout->addWidget(_titleLabel);
    headerLayout->addStretch();

    headerLayout->addWidget(_newCountLabel);
    headerLayout->addWidget(_deletedCountLabel);
    headerLayout->addWidget(_modifiedCountLabel);

    mainLayout->addLayout(headerLayout);

    _statusLabel = new QLabel(this);
    _statusLabel->setObjectName("comparisonStatus");
    _statusLabel->setWordWrap(true);

    mainLayout->addWidget(_statusLabel);

    _tableView = new ComparisonView(this);

    mainLayout->addWidget(_tableView, 1);
}

void ComparisonPanel::setupStyle()
{
    QString error;

    if(!StyleManager::applyStyle(this, ":/styles/styles/comparison-panel.qss", &error))
        qWarning() << error;
}

void ComparisonPanel::setComparisonModel(ComparisonModel *model)
{
    if(_model == model)
        return;

    disconnect(_modelResetConnection);
    disconnect(_rowsInsertedConnetion);
    disconnect(_rowsRemovedConnection);

    _model = model;

    _tableView->setComparisonModel(_model);

    if(_model)
    {
        _modelResetConnection = connect(_model, &QAbstractItemModel::modelReset, this, &ComparisonPanel::updateSummary);
        _rowsInsertedConnetion = connect(_model, &QAbstractItemModel::rowsInserted, this, &ComparisonPanel::updateSummary);
        _rowsRemovedConnection = connect(_model, &QAbstractItemModel::rowsRemoved, this, &ComparisonPanel::updateSummary);
    }

    updateSummary();
}

void ComparisonPanel::updateSummary()
{
    if(!_model)
    {
        _newCountLabel->setText("Новые: 0");
        _deletedCountLabel->setText("Удалённые: 0");
        _modifiedCountLabel->setText("Изменённые: 0");

        _statusLabel->setText("Модель сравнения не назначена.");
        return;
    }

    const int newCount = _model->countByType(ComparisonChangeType::New);
    const int deletedCount = _model->countByType(ComparisonChangeType::Deleted);
    const int modifiedCount = _model->countByType(ComparisonChangeType::Modified);

    _newCountLabel->setText(QString("Новые: %1").arg(newCount));
    _deletedCountLabel->setText(QString("Удалённые: %1").arg(deletedCount));
    _modifiedCountLabel->setText(QString("Изменённые: %1").arg(modifiedCount));

    const ComparisonResult &result = _model->result();

    if(result.scanId <= 0)
    {
        _statusLabel->setText("Результат сравнения пока отсутствуют.");
        return;
    }

    QStringList warnings;

    if(!result.canDetermineNewFiles())
        warnings.append("Исходный снимок неполный. Определение новых файлов может быть неточным.");

    if(!result.canDetermineDeletedFiles())
        warnings.append("Текущее сканирование завершилось с ошибками. Удаление файлов нельзя подтвердить.");

    if(!warnings.isEmpty())
        _statusLabel->setText(warnings.join("\n"));
    else if(_model->rowCount() == 0)
        _statusLabel->setText("Изменений не обнаружено.");
    else
        _statusLabel->clear();

}