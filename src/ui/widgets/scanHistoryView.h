#ifndef SCANHISTORYVIEW_H
#define SCANHISTORYVIEW_H

#include <QTableView>
#include <QMetaObject>

class ScanHistoryModel;

class ScanHistoryView : public QTableView
{
    Q_OBJECT

public:
    ScanHistoryView(QWidget *parent = nullptr);

    void setScanModel(ScanHistoryModel *model);
    void selectScan(const qint64 &scanId);
    qint64 selectedScanId() const;

signals:
    void scanSelected(qint64 scanId);

private slots:
    void onCurrentChanged(const QModelIndex &current);

private:
    void setupStyle();
    void configureHeader();

    QMetaObject::Connection _selectionConnection;
};

#endif // SCANHISTORYVIEW_H
