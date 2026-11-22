#include "mainWindow.h"
#include "ui/widgets/mainToolBar.h"
#include "models/scanHistoryModel.h"
#include "ui/widgets/scanHistoryPanel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    _mainToolBar = new MainToolBar(this);
    addToolBar(Qt::TopToolBarArea, _mainToolBar);

    _scanHistoryModel = new ScanHistoryModel(this);
    _scanHistoryPanel = new ScanHistoryPanel(_scanHistoryModel, this);

    setCentralWidget(_scanHistoryPanel);


}

MainWindow::~MainWindow() = default;
