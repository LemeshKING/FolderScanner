#include "mainToolBar.h"
#include "ui/styleManager.h"

#include <QAction>
#include <QToolBar>
#include <QKeySequence>
#include <QFile>

MainToolBar::MainToolBar(QWidget *parent)
    : QToolBar(parent)
{
    setObjectName("mainToolBar");

    setMovable(false);
    setFloatable(false);
    setAllowedAreas(Qt::TopToolBarArea);

    setIconSize(QSize(22, 22));
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    setupStyle();
    createActions();
}

void MainToolBar::setupStyle()
{
    QString error;

    if(!StyleManager::applyStyle(this, ":/styles/styles/main-toolbar.qss"))
        qWarning() << error;
}

void MainToolBar::setScanEnabled(bool enabled)
{
    _scanAction->setEnabled(enabled);
}

void MainToolBar::createActions()
{
    _selectFolderAction = addAction(QIcon(":/icons/icons/folder-open.svg"), "Выбрать папку");

    _selectFolderAction->setToolTip("Выбрать папку для сканирования");
    _selectFolderAction->setShortcut(QKeySequence::Open);

    _scanAction = addAction(QIcon(":/icons/icons/play.svg"), "Сканировать");

    _scanAction->setToolTip("Начать сканирование выбранной папки");
    _scanAction->setEnabled(false);

    QWidget *scanBtn = widgetForAction(_scanAction);
    if(scanBtn)
        scanBtn->setObjectName("scanButton");

    addSeparator();

    _refreshAction = addAction(QIcon(":/icons/icons/refresh-cw.svg"), "Обновить");

    _refreshAction->setToolTip("Обновить историю сканирований");
    _refreshAction->setShortcut(Qt::Key_F5);

    connect(_selectFolderAction, &QAction::triggered, this, &MainToolBar::selectFolderRequested);
    connect(_scanAction, &QAction::triggered, this, &MainToolBar::scanRequested);
    connect(_refreshAction, &QAction::triggered, this, &MainToolBar::refreshRequested);
}