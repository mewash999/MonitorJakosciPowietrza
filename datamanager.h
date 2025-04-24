#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include <QByteArray>
#include <QVector>
#include <QPair>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonArray>

class DataManager
{
public:
    static QString getDataFilePath(const QString &baseFileName, int id = -1);
    static void saveDataToFile(const QString &baseFileName, const QByteArray &data, int id = -1);
    static QByteArray loadDataFromFile(const QString &baseFileName, int id = -1);
    static void saveHistoricalData(const QString &type, const QByteArray &data, int id = -1);
    static QVector<QPair<QDateTime, QByteArray>> loadAllHistoricalData(const QString &type, int id);
};

#endif
