#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QString>

class QWidget;

class StyleManager
{
public:
    static bool applyStyle(QWidget *widget, const QString &resourcePath, QString *errorMessage = nullptr);

private:
    static QString loadStyle(const QString &resourcePath, QString *errorMessage);
};

#endif // STYLEMANAGER_H
