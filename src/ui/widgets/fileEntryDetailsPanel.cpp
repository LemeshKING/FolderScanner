#include "fileEntryDetailsPanel.h"
#include "models/fileTreeModel.h"
#include "ui/styleManager.h"

#include <QFormLayout>
#include <QLabel>
#include <QLocale>
#include <QVBoxLayout>


FileEntryDetailsPanel::FileEntryDetailsPanel(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("fileEntryDetailsPanel");
    setAttribute(Qt::WA_StyledBackground, true);
    setupUi();
    setupStyle();

    clear();
}

void FileEntryDetailsPanel::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    auto *titleLabel = new QLabel("Информация об элементе", this);
    titleLabel->setObjectName("detailsTitle");

    _nameLabel = new QLabel(this);
    _nameLabel->setObjectName("entryName");

    _pathLabel = new QLabel(this);
    _pathLabel->setObjectName("entryPath");
    _pathLabel->setWordWrap(true);
    _pathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    _typeLabel = new QLabel(this);

    _sizeTitleLabel = new QLabel("Разрмер:", this);
    _sizeValueLabel = new QLabel(this);

    _fileCountTitleLabel = new QLabel("Файлов:", this);
    _fileCountValueLabel = new QLabel(this);

    auto *formLayout = new QFormLayout();

    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->setHorizontalSpacing(16);
    formLayout->setVerticalSpacing(5);

    formLayout->addRow("Путь:", _pathLabel);
    formLayout->addRow("Тип:", _typeLabel);
    formLayout->addRow(_sizeTitleLabel, _sizeValueLabel);
    formLayout->addRow(_fileCountTitleLabel, _fileCountValueLabel);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(_nameLabel);
    mainLayout->addLayout(formLayout);

    setMinimumHeight(150);
}

void FileEntryDetailsPanel::setupStyle()
{
    QString error;

    if(!StyleManager::applyStyle(this, ":/styles/styles/file-entry-details-panel.qss", &error))
        qWarning() << error;
}

void FileEntryDetailsPanel::setEntry(const QModelIndex &index)
{
    if(!index.isValid())
    {
        clear();
        return;
    }

    const QVariant typeData = index.data(FileTreeModel::EntryTypeRole);

    if(!typeData.isValid())
    {
        clear();
        return;
    }

    const bool isDirectory = typeData.toInt() == static_cast<int>(EntryType::Directory);

    _nameLabel->setText(index.sibling(index.row(), 0).data(Qt::DisplayRole).toString());
    _pathLabel->setText(index.data(FileTreeModel::RelativePathRole).toString());
    _typeLabel->setText(isDirectory ? "Папка" : "Файл");
    _sizeTitleLabel->setText(isDirectory ? "Общий размер:" : "Размер:");

    const qint64 sizeBytes = index.data(isDirectory ? FileTreeModel::TotalSizeBytesRole : FileTreeModel::SizeBytesRole).toLongLong();
    _sizeValueLabel->setText(FileTreeModel::formatSize(sizeBytes));

    if(isDirectory)
    {
        const qint64 fileCount = index.data(FileTreeModel::FileCountRole).toLongLong();
        _fileCountValueLabel->setText(QLocale().toString(fileCount));
    }

    _fileCountTitleLabel->setVisible(isDirectory);
    _fileCountValueLabel->setVisible(isDirectory);
}

void FileEntryDetailsPanel::clear()
{
    _nameLabel->setText("Элемент не выбран");
    _pathLabel->clear();
    _typeLabel->clear();

    _sizeTitleLabel->setText("Размер:");
    _sizeValueLabel->clear();

    _fileCountTitleLabel->hide();
    _fileCountValueLabel->hide();
}
