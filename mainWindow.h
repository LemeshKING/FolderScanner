#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainToolBar;
class ScanHistoryPanel;
class ScanHistoryModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:

    MainToolBar *_mainToolBar = nullptr;
    ScanHistoryModel *_scanHistoryModel = nullptr;
    ScanHistoryPanel *_scanHistoryPanel = nullptr;
};
#endif // MAINWINDOW_H
