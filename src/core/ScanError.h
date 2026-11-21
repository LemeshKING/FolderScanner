#ifndef SCANERROR_H
#define SCANERROR_H

#include <QString>
#include <QtGlobal>

struct ScanError
{
    qint64 errorId = -1;
    qint64 scanId = -1;

    QString path;
    QString message;
};

#endif // SCANERROR_H
