#include "databaseManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QUuid>
#include <QDebug>

DatabaseManager::DatabaseManager(const QString& databasePath)
{
    _connectionName = QString("FileDatabase_%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
    _databasePath = databasePath;

    if(databasePath.trimmed().isEmpty())
    {
        const QString applicationDirectory = QCoreApplication::applicationDirPath();


        if(applicationDirectory.isEmpty())
        {
            setError("Не удалось определить папку приложения");
            return;
        }

        const QString dataDirectory = QDir(applicationDirectory).filePath("database");

        if(!QDir().mkpath(dataDirectory))
        {
            setError("Не удалось создать папку данных приложения.");
            return;
        }

        _databasePath = QDir(dataDirectory).filePath("database.sqlite");
    }

    const QFileInfo fileInfo(_databasePath);
    if(!QDir().mkpath(fileInfo.absolutePath()))
    {
        setError("Не удалось создать дирректорию базы данных.");
        return;
    }

    _databasePath = QDir::cleanPath(QFileInfo(_databasePath).absoluteFilePath());
    qDebug() << _databasePath;
}

DatabaseManager::~DatabaseManager()
{
    close();
}

bool DatabaseManager::open()
{
    if(_databasePath.isEmpty() && _lastError.isEmpty())
        return false;

    if(_database.isValid() && _database.isOpen())
        return true;

    if(!_database.isValid())
    {
        _database = QSqlDatabase::addDatabase("QSQLITE", _connectionName);
        _database.setDatabaseName(_databasePath);
    }

    if(!_database.open())
    {
        setError("Ошибка открытия базы данных: " + _database.lastError().text());
        return false;
    }

    if(!executeSqlFile(":/sql/sql/00_init.sql"))
    {
        _database.close();
        return false;
    }

    QSqlQuery query(_database);
    if(!query.exec("PRAGMA foreign_keys") || !query.next() || query.value(0).toInt() != 1)
    {
        setError("Не удалось включить проверку внешних ключей SQLite.");
        _database.close();
        return false;
    }

    _lastError.clear();
    return true;
}

bool DatabaseManager::initializeDatabase()
{
    if(!open())
        return false;



    int version = 0;

    if(!readSchemaVersion(version))
        return false;

    if(version > CurrentSchemaVersion)
    {
        setError(QString("Версия базы данных (%1) новее поддерживаемой (%2).")
                    .arg(version)
                    .arg(CurrentSchemaVersion));
        return false;
    }


    if(version == CurrentSchemaVersion)
    {
        _lastError.clear();
        return true;
    }

    if(!_database.transaction())
    {
        setError("Не удалось начать транзакцию: " + _database.lastError().text());
        return false;
    }

    while(version < CurrentSchemaVersion)
    {
        if(!applyMigrationFrom(version))
        {
            qDebug() << "migration fuck off" << version;
            _database.rollback();
            return false;
        }
        qDebug() << "migration?";
        version++;
    }

    if(!_database.commit())
    {
        setError("Ошибка сохранения изменений: " + _database.lastError().text());
        _database.rollback();
        return false;
    }

    _lastError.clear();
    return true;
}

bool DatabaseManager::readSchemaVersion(int &version)
{
    QSqlQuery query(_database);

    if(!query.exec("PRAGMA user_version"))
    {
        setError("Не удалось получить версию БД: " + query.lastError().text());
        return false;
    }

    if(!query.next())
    {
        setError("Не удалось прочитать версию базы данных.");
        return false;
    }

    version = query.value(0).toInt();

    return true;
}

bool DatabaseManager::setSchemaVersion(const int &version)
{
    QSqlQuery query(_database);
    const QString sql = QString("PRAGMA user_version = %1").arg(version);

    if(!query.exec(sql))
    {
        setError("Не удалось установить версию базы данных: " + query.lastError().text());
        return false;
    }

    return true;
}

bool DatabaseManager::applyMigrationFrom(const int &version)
{
    switch (version)
    {
        case 0:
            if(!executeSqlFile(":/sql/sql/01_create_scans.sql"))
                return false;

            if(!executeSqlFile(":/sql/sql/02_create_scan_entries.sql"))
                return false;

            if(!executeSqlFile(":/sql/sql/03_create_scan_errors.sql"))
                return false;

            if(!setSchemaVersion(1))
                return false;


            return true;

        default:
            setError(QString("Не найдекна миграция с версией %1").arg(version));
            return false;
    }
}


bool DatabaseManager::executeSqlFile(const QString &resourcePath)
{
    QFile file(resourcePath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        setError("Не удалось открыть SQL-файл: " + resourcePath);
        return false;
    }

    QString script = QString::fromUtf8(file.readAll());
    file.close();

    if(!script.isEmpty() && script.at(0) == QChar(0xFEFF))
        script.remove(0, 1);

    const QStringList statements = splitSqlStatements(script);

    if(statements.isEmpty())
    {
        setError("SQL-файл не содержит команд или не имеет разделителей: " + resourcePath);
        return false;
    }

    for(const QString &statement : statements)
    {
        QSqlQuery query(_database);
        if(!query.exec(statement))
        {
            setError(QString("Ошибка выполнения SQL-файла %1:\n%2\n\nSQL:\n%3").arg(resourcePath, query.lastError().text(), statement));
            return false;
        }
    }

    return true;
}

QStringList DatabaseManager::splitSqlStatements(const QString &script) const
{
    const QString separator = "-- @statement";
    QStringList statements;
    QString currentStatement;

    bool separatorFound = false;

    const QStringList lines = script.split('\n');

    for(const QString &line : lines)
    {
        if(line.trimmed() == separator)
        {
            if(!currentStatement.trimmed().isEmpty())
                statements.append(currentStatement.trimmed());

            currentStatement.clear();
            separatorFound = true;
        }
        else
            currentStatement += line + '\n';
    }

    if(!currentStatement.trimmed().isEmpty())
        statements.append(currentStatement.trimmed());

    if(!separatorFound)
        return {};

    return statements;
}

void DatabaseManager::close()
{
    if(!_database.isValid())
        return;

    _database.close();
    _database = QSqlDatabase();
    QSqlDatabase::removeDatabase(_connectionName);
}

bool DatabaseManager::isOpen() const
{
    return _database.isValid() && _database.isOpen();
}

const QSqlDatabase &DatabaseManager::connection() const
{
    return _database;
}

QString DatabaseManager::lastError() const
{
    return _lastError;
}

QString DatabaseManager::databasePath() const
{
    return _databasePath;
}

void DatabaseManager::setError(const QString &error)
{
    _lastError = error;
}