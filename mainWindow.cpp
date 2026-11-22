#include "mainWindow.h"
#include "ui/widgets/mainToolBar.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    _mainToolBar = new MainToolBar(this);
    addToolBar(Qt::TopToolBarArea, _mainToolBar);


}

MainWindow::~MainWindow() = default;
