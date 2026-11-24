#ifndef FILETREEPANEL_H
#define FILETREEPANEL_H

#include <QWidget>
#include <QModelIndex>
#include <QMetaObject>

class FileEntryDetailsPanel;
class FileTreeModel;
class FileTreeView;
class QLabel;

class FileTreePanel : public QWidget
{
    Q_OBJECT

public:
    FileTreePanel(FileTreeModel *model, QWidget *parent = nullptr);

    void setModel(FileTreeModel *model);

    QModelIndex currentIndex() const;
    void selectIndex(const QModelIndex &index);

signals:
    void entrySelected(const QModelIndex &index);

private slots:
    void onEntrySelected(const QModelIndex &index);

private:
    void setupStyle();

    QLabel *_titleLabel = nullptr;
    FileTreeView *_treeView = nullptr;
    FileEntryDetailsPanel *_detailsPanel = nullptr;

    QMetaObject::Connection _modelResetConnection;
};

#endif // FILETREEPANEL_H
