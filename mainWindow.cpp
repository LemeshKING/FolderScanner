#include "mainWindow.h"
#include "ui/widgets/mainToolBar.h"
#include "models/scanHistoryModel.h"
#include "ui/widgets/scanHistoryPanel.h"
#include "ui/widgets/fileTreePanel.h"
#include "models/fileTreeModel.h"
#include "ui/styleManager.h"
#include "services/scanService.h"
#include "ui/widgets/comparisonPanel.h"
#include "ui/widgets/comparisonView.h"
#include "models/comparisonModel.h"
#include "workers/fileOperationWorker.h"

#include <QThread>
#include <QMetaObject>
#include <QSplitter>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QIcon>

MainWindow::MainWindow(const QSqlDatabase &database, QWidget *parent)
    : QMainWindow(parent),
    _scanService(std::make_unique<ScanService>(database))
{
    setObjectName("mainWindow");
    setWindowTitle("FolderScanner");
    resize(1200, 750);
    setMinimumSize(900, 550);

    setWindowIcon(QIcon(":/icons/icons/file-app"));

    _mainToolBar = new MainToolBar(this);
    addToolBar(Qt::TopToolBarArea, _mainToolBar);

    _scanHistoryModel = new ScanHistoryModel(this);
    _scanHistoryPanel = new ScanHistoryPanel(_scanHistoryModel, this);

    _fileTreeModel = new FileTreeModel(this);
    _fileTreePanel = new FileTreePanel(_fileTreeModel, this);

    _comparisonModel = new ComparisonModel(this);
    _comparisonPanel = new ComparisonPanel(_comparisonModel, this);

    auto *contentTabsContainer = new QWidget(this);
    contentTabsContainer->setObjectName("contentTabsContainer");

    auto *contentTabsLayout = new QVBoxLayout(contentTabsContainer);

    contentTabsLayout->setContentsMargins(8, 8, 0, 8);
    contentTabsLayout->setSpacing(0);

    auto *historyContainer = new QWidget(this);

    auto *historyLayout = new QVBoxLayout(historyContainer);
    historyLayout->setContentsMargins(0, 8, 8, 8);
    historyLayout->setSpacing(0);

    historyLayout->addWidget(_scanHistoryPanel);

    _contentTabs = new QTabWidget(contentTabsContainer);
    _contentTabs->setObjectName("contentTabs");
    _contentTabs->setAttribute(Qt::WA_StyledBackground, true);

    _contentTabs->addTab(_fileTreePanel, "Файлы");
    _contentTabs->addTab(_comparisonPanel, "Сравнение");

    contentTabsLayout->addWidget(_contentTabs);

    _mainToolBar->setCompareEnabled(false);

    auto *splitter = new QSplitter(Qt::Horizontal, this);

    splitter->setObjectName("mainSplitter");

    splitter->setChildrenCollapsible(false);
    splitter->setHandleWidth(5);

    splitter->addWidget(historyContainer);
    splitter->addWidget(contentTabsContainer);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);

    splitter->setSizes({400, 900});

    setCentralWidget(splitter);

    setupStyle();

    connect(_mainToolBar, &MainToolBar::selectFolderRequested, this, &MainWindow::selectFolder);
    connect(_mainToolBar, &MainToolBar::scanRequested, this, &MainWindow::startScan);
    connect(_mainToolBar, &MainToolBar::refreshRequested, this, &MainWindow::refreshHistory);
    connect(_mainToolBar, &MainToolBar::compareRequested, this, &MainWindow::compareSelectedScan);

    connect(_scanHistoryPanel, &ScanHistoryPanel::scanSelected, this, &MainWindow::loadScan);

    setupWorker(database.databaseName());

    refreshHistory();
}

MainWindow::~MainWindow()
{
    if(_workerThread && _workerThread->isRunning())
    {
        _workerThread->quit();
        _workerThread->wait();
    }
}

void MainWindow::selectFolder()
{
    if(_operationInProgress)
        return;

    const QString path = QFileDialog::getExistingDirectory(this, "Выберите папку для сканирования", _selectedRootPath);

    if(path.isEmpty())
        return;

    _selectedRootPath = QDir::cleanPath(path);

    _mainToolBar->setScanEnabled(true);

    statusBar()->showMessage(QString("Выбрана папка: %1").arg(QDir::toNativeSeparators(_selectedRootPath)), 5000);
}

void MainWindow::startScan()
{
    if(_operationInProgress)
        return;

    if(_selectedRootPath.isEmpty())
    {
        selectFolder();

        if(_selectedRootPath.isEmpty())
            return;
    }

    setOperationInProgress(true, "Выполняется сканирование папки...");

    const bool invoked = QMetaObject::invokeMethod(
        _worker,
        "scanFolder",
        Qt::QueuedConnection,
        Q_ARG(QString, _selectedRootPath)
    );

    if(!invoked)
    {
        setOperationInProgress(false);

        QMessageBox::critical(this, "Ошибка", "Не удалось запустить сканирование");
    }
}

void MainWindow::onScanFinished(bool success, const qint64 &scanId, const QString &error)
{
    setOperationInProgress(false);

    if(!success)
    {
        QMessageBox::critical(this, "Ошибка сканирования", error);
        return;
    }

    updateHistory(scanId);

    statusBar()->showMessage("Сканирование успешно завершено", 5000);
}

void MainWindow::refreshHistory()
{
    if(_operationInProgress)
        return;

    updateHistory(-1);
}

void MainWindow::updateHistory(qint64 preferredScanId)
{
    QList<ScanInfo> scans;

    if(!_scanService->getScanHistory(scans))
    {
        QMessageBox::critical(this, "Ошибка загрузки истории", _scanService->lastError());
        return;
    }

    qint64 targetScanId = preferredScanId;

    if(targetScanId <= 0)
        targetScanId = _scanHistoryPanel->selectedScanId();

    const auto it = std::find_if(scans.begin(), scans.end(), [targetScanId](const ScanInfo &scan)
    {
        return scan.scanId == targetScanId;
    });

    if(it == scans.end())
        targetScanId = scans.isEmpty() ? -1 : scans.first().scanId;

    _scanHistoryModel->setScans(scans);
    _mainToolBar->setCompareEnabled(targetScanId > 0);
    if(targetScanId > 0)
        _scanHistoryPanel->selectScan(targetScanId);
    else
    {
        _mainToolBar->setCompareEnabled(false);

        _comparisonModel->clear();
        _fileTreeModel->clear();
    }
}

void MainWindow::loadScan(qint64 scanId)
{
    if(_operationInProgress || scanId < 0)
        return;

    setOperationInProgress(true, "Загрузка сканировани...");

    const bool invoked = QMetaObject::invokeMethod(
        _worker,
        "loadScan",
        Qt::QueuedConnection,
        Q_ARG(const qint64 &, scanId)
    );

    if(!invoked)
    {
        setOperationInProgress(false);

        QMessageBox::critical(this, "Ошибка", "Не удалось запустить загрузку сканирования");

    }
}

void MainWindow::onScanLoadFinished(const ScanLoadResult &result)
{
    setOperationInProgress(false);

    if(!result.success)
    {
        QMessageBox::critical(this, "Ошибка загрузки снимка", result.error);
        return;
    }

    _fileTreeModel->setPrepareData(result.treeData);
    _selectedRootPath = result.scanInfo.rootPath;

    _mainToolBar->setScanEnabled(true);
    _mainToolBar->setCompareEnabled(true);

    _comparisonModel->clear();

    _contentTabs->setCurrentWidget(_fileTreePanel);

    if(result.scanInfo.status == ScanStatus::Partial || !result.errors.isEmpty())
        statusBar()->showMessage(QString("Снимок загружен с предупреждениями. Ошибок: %1").arg(result.errors.size()), 10000);
    else if (result.scanInfo.status == ScanStatus::Failed)
        statusBar()->showMessage(QString("Сканирование завершилось с ошибкой"), 10000);
    else
        statusBar()->showMessage("Снимок успешно загружен", 5000);
}

void MainWindow::setupStyle()
{
    QString error;

    if(!StyleManager::applyStyle(this, ":/styles/styles/main-window.qss", &error))
        qWarning() << error;
}

void MainWindow::compareSelectedScan()
{
    if(_operationInProgress)
        return;

    const qint64 scanId = _scanHistoryPanel->selectedScanId();

    if(scanId <= 0)
    {
        QMessageBox::warning(this, "Сравнение", "Сначала выберите сохранённое сканирование.");
        return;
    }

    setOperationInProgress(true, "Сравнение выполняется...");

    const bool invoked = QMetaObject::invokeMethod(
        _worker,
        "compareScan",
        Qt::QueuedConnection,
        Q_ARG(const qint64&, scanId)
    );

    if(!invoked)
    {
        setOperationInProgress(false);
        QMessageBox::critical(this, "Ошибка", "Не удалось запустить сравнение");
    }
}

void MainWindow::onComparisonFinished(bool success, const ComparisonResult &result, const QString &error)
{
    setOperationInProgress(false);

    if(!success)
    {
        QMessageBox::critical(this, "Ошибка сравнения", error);
        return;
    }

    _comparisonModel->setResult(result);
    _contentTabs->setCurrentWidget(_comparisonPanel);

    statusBar()->showMessage("Сравнение завершено", 5000);
}

void MainWindow::setupWorker(const QString &databasePath)
{
    qRegisterMetaType<ComparisonResult>("ComparisonResult");

    _workerThread = new QThread(this);

    _worker = new FileOperationWorker(databasePath);

    _worker->moveToThread(_workerThread);

    connect(_workerThread, &QThread::finished, _worker, &QObject::deleteLater);
    connect(_worker, &FileOperationWorker::scanFinished, this, &MainWindow::onScanFinished, Qt::QueuedConnection);
    connect(_worker, &FileOperationWorker::comparisonFinished, this, &MainWindow::onComparisonFinished, Qt::QueuedConnection);
    connect(_worker, &FileOperationWorker::scanLoadFinished, this, &MainWindow::onScanLoadFinished, Qt::QueuedConnection);

    _workerThread->start();
}

void MainWindow::setOperationInProgress(bool inProgress, const QString &message)
{
    _operationInProgress = inProgress;

    _mainToolBar->setBusy(inProgress);
    _scanHistoryPanel->setEnabled(!inProgress);

    if(!message.isEmpty())
        statusBar()->showMessage(message);
}