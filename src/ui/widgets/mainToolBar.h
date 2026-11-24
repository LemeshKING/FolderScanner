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
    void setCompareEnabled(bool enabled);
    void setBusy(bool busy);

signals:
    void selectFolderRequested();
    void scanRequested();
    void refreshRequested();
    void compareRequested();

private:
    void setupStyle();
    void createActions();

    bool _busy = false;
    bool _scanEnabled = false;
    bool _compareEnabled = false;

    QAction *_selectFolderAction = nullptr;
    QAction *_scanAction = nullptr;
    QAction *_refreshAction = nullptr;
    QAction *_compareAction = nullptr;
};

#endif // MAINTOOLBAR_H
