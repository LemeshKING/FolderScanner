#ifndef SCANLOADRESULT_H
#define SCANLOADRESULT_H

#include "core/scnaInfo.h"
#include "core/ScanError.h"
#include "models/preparedFileTreeData.h"

#include <QList>
#include <QString>
#include <QMetaType>

struct ScanLoadResult
{
    bool success = false;

    ScanInfo scanInfo;

    QList<ScanError> errors;

    PreparedFileTreeDataPtr treeData;

    QString error;
};

Q_DECLARE_METATYPE(ScanLoadResult)

#endif // SCANLOADRESULT_H
