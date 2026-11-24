#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core/ComparisonResult.h"
#include "core/ScanLoadResult.h"

#include <QMainWindow>
#include <QtGlobal>
#include <QString>
#include <memory>

class QSqlDatabase;
class QThread;
class FileOperationWorker;
class ScanService;

class MainToolBar;
class ScanHistoryPanel;
class ScanHistoryModel;
class FileTreeModel;
class FileTreePanel;
class QTabWidget;
class ComparisonPanel;
class ComparisonModel;
class ComparisonService;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QSqlDatabase &database, QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void selectFolder();
    void startScan();
    void refreshHistory();
    void compareSelectedScan();
    void loadScan(qint64 scanId);
    void onScanFinished(bool success, const qint64 &scanId, const QString &error);
    void onComparisonFinished(bool success, const ComparisonResult &result, const QString &error);
    void onScanLoadFinished(const ScanLoadResult &result);

private:
    void updateHistory(qint64 preferredScanId);
    void setupStyle();
    void setupWorker(const QString &databasePath);
    void setOperationInProgress(bool inProgress, const QString &message = QString());



    QThread *_workerThread = nullptr;
    FileOperationWorker *_worker = nullptr;
    MainToolBar *_mainToolBar = nullptr;
    ScanHistoryModel *_scanHistoryModel = nullptr;
    ScanHistoryPanel *_scanHistoryPanel = nullptr;
    FileTreeModel *_fileTreeModel = nullptr;
    FileTreePanel *_fileTreePanel = nullptr;
    ComparisonModel *_comparisonModel = nullptr;
    ComparisonPanel *_comparisonPanel = nullptr;

    std::unique_ptr<ScanService> _scanService;

    QTabWidget *_contentTabs = nullptr;
    QString _selectedRootPath;

    bool _operationInProgress = false;
};
#endif // MAINWINDOW_H
