#ifndef SCANHISTORYPANEL_H
#define SCANHISTORYPANEL_H

#include <QWidget>
#include <QtGlobal>

class QLabel;
class QTableView;
class ScanHistoryModel;

class ScanHistoryPanel : public QWidget
{
    Q_OBJECT

public:
    ScanHistoryPanel(ScanHistoryModel *model, QWidget *parent = nullptr);

    bool selectScan(const qint64 &scanId);
    qint64 selectedScanId() const;

signals:
    void scanSelected(qint64 scanId);

private slots:

    void onRowChanged(const QModelIndex &current);
    void updateCount();

private:

    void setupUi();
    void setupTable();
    void setupStyle();


    ScanHistoryModel *_model = nullptr;

    QLabel *_titleLabel = nullptr;
    QLabel *_countLabel = nullptr;

    QTableView *_tableView = nullptr;
};

#endif // SCANHISTORYPANEL_H
