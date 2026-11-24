#ifndef SCANHISTORYPANEL_H
#define SCANHISTORYPANEL_H

#include <QWidget>
#include <QtGlobal>

class QLabel;
class ScanHistoryView;
class ScanHistoryModel;

class ScanHistoryPanel : public QWidget
{
    Q_OBJECT

public:
    ScanHistoryPanel(ScanHistoryModel *model, QWidget *parent = nullptr);

    void selectScan(const qint64 &scanId);
    qint64 selectedScanId() const;

signals:
    void scanSelected(qint64 scanId);

private slots:

    void updateCount();

private:

    void setupUi();
    void setupStyle();

    ScanHistoryModel *_model = nullptr;

    QLabel *_titleLabel = nullptr;
    QLabel *_countLabel = nullptr;

    ScanHistoryView *_tableView = nullptr;
};

#endif // SCANHISTORYPANEL_H
