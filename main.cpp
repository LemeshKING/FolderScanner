#include "mainWindow.h"

#include <QApplication>
#include <QMessageBox>

#include "database/databaseManager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("FolderScanner");

    DatabaseManager databaseManager;
    if(!databaseManager.initializeDatabase())
    {
        QMessageBox::critical(
            nullptr,
            "Database error",
            databaseManager.lastError()
        );
        return -1;
    }

    MainWindow w;
    w.show();
    return QApplication::exec();
}
