#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainToolBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:

    MainToolBar *_mainToolBar = nullptr;
};
#endif // MAINWINDOW_H
