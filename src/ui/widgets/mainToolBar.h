#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H

#include <QToolBar>

class QAction;

class MainToolBar : public QToolBar
{
    Q_OBJECT

public:
    MainToolBar(QWidget *parent = nullptr);

    void setScanEnabled(bool enabled);

signals:
    void selectFolderRequested();
    void scanRequested();
    void refreshRequested();

private:
    void setupStyle();
    void createActions();

    QAction *_selectFolderAction = nullptr;
    QAction *_scanAction = nullptr;
    QAction *_refreshAction = nullptr;
};

#endif // MAINTOOLBAR_H
