#ifndef COMPARISONVIEW_H
#define COMPARISONVIEW_H

#include <QTableView>
#include <QMetaObject>

class ComparisonModel;

class ComparisonView : public QTableView
{
    Q_OBJECT

public:
    ComparisonView(QWidget *parent = nullptr);

    void setComparisonModel(ComparisonModel *model);

private:

    void setupStyle();
    void configureHeader();
};

#endif // COMPARISONVIEW_H
