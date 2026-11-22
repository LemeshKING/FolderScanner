#include "styleManager.h"

#include <QFile>
#include <QWidget>

QString StyleManager::loadStyle(const QString &resourcePath, QString *errorMessage)
{
    QFile styleFile(resourcePath);

    if(!styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if(errorMessage)
            *errorMessage = QString("Не удалось открыть файл стилей %1 : %2").arg(resourcePath, styleFile.errorString());

        return {};
    }

    return QString::fromUtf8(styleFile.readAll());
}

bool StyleManager::applyStyle(QWidget *widget, const QString &resourcePath, QString *errorMessage)
{
    if(errorMessage)
        errorMessage->clear();

    if(!widget)
    {
        if(errorMessage)
            *errorMessage = "Не удалось применить стиль: виджет не существует.";

        return false;
    }

    QString style = loadStyle(resourcePath, errorMessage);

    if(style.isNull())
        return false;

    widget->setStyleSheet(style);
    return true;
}
