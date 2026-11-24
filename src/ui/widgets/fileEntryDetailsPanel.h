#ifndef FILEENTRYDETAILSPANEL_H
#define FILEENTRYDETAILSPANEL_H

#include <QWidget>
#include <QModelIndex>

class QLabel;

class FileEntryDetailsPanel : public QWidget
{

    Q_OBJECT

public:
    FileEntryDetailsPanel(QWidget *parent = nullptr);

    void setEntry(const QModelIndex &index);
    void clear();

private:
    void setupUi();
    void setupStyle();

    QLabel *_nameLabel = nullptr;
    QLabel *_pathLabel = nullptr;
    QLabel *_typeLabel = nullptr;

    QLabel *_sizeTitleLabel = nullptr;
    QLabel *_sizeValueLabel = nullptr;

    QLabel *_fileCountTitleLabel = nullptr;
    QLabel *_fileCountValueLabel = nullptr;
};

#endif // FILEENTRYDETAILSPANEL_H
