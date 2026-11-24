#ifndef FILETREEVIEW_H
#define FILETREEVIEW_H

#include <QTreeView>
#include <QMetaObject>

class FileTreeModel;

class FileTreeView : public QTreeView
{
    Q_OBJECT
public:

    FileTreeView(QWidget *parent = nullptr);

    void setFileModel(FileTreeModel *model);
    void selectIndex(const QModelIndex &index);

signals:
    void entrySelected(const QModelIndex &index);

private slots:
    void onCurrentChanged(const QModelIndex &current);

private:
    void setupStyle();
    void configureHeader();

    QMetaObject::Connection _selectionConnection;
};

#endif // FILETREEVIEW_H
