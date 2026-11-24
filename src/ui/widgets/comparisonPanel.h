#ifndef COMPARISONPANEL_H
#define COMPARISONPANEL_H

#include <QWidget>
#include <QMetaObject>

class QLabel;
class ComparisonView;
class ComparisonModel;

class ComparisonPanel : public QWidget
{
    Q_OBJECT

public:
    ComparisonPanel(ComparisonModel *model, QWidget *parent = nullptr);

    void setComparisonModel(ComparisonModel *model);

private slots:
    void updateSummary();

private:
    void setupUi();
    void setupStyle();

    ComparisonModel *_model = nullptr;

    QLabel *_titleLabel = nullptr;
    QLabel *_newCountLabel = nullptr;
    QLabel *_deletedCountLabel = nullptr;
    QLabel *_modifiedCountLabel = nullptr;

    QLabel *_statusLabel = nullptr;

    ComparisonView *_tableView = nullptr;

    QMetaObject::Connection _modelResetConnection;
    QMetaObject::Connection _rowsInsertedConnetion;
    QMetaObject::Connection _rowsRemovedConnection;
};

#endif // COMPARISONPANEL_H
