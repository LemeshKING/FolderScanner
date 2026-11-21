#ifndef ERRORREPOSITORY_H
#define ERRORREPOSITORY_H

#include "repositoryBase.h"
#include "core/ScanError.h"

#include <QList>

class ErrorRepository : public RepositoryBase
{
public:
    ErrorRepository(const QSqlDatabase &database);

    bool insertError(ScanError &error);
    bool getErrorsByScanId(const qint64 &scanId, QList<ScanError> &errors);

};

#endif // ERRORREPOSITORY_H
