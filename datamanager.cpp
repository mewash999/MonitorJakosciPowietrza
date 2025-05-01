/**
 * @file datamanager.cpp
 * @brief Implementacja klasy DataManager do zarządzania danymi zapisywanymi na dysku.
 */

 #include "datamanager.h"

 /**
  * @brief Pobiera ścieżkę do pliku danych.
  * @param baseFileName Bazowa nazwa pliku.
  * @param id Identyfikator (opcjonalny, domyślnie -1).
  * @return Ścieżka do pliku.
  */
 QString DataManager::getDataFilePath(const QString &baseFileName, int id) {
     QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
     QDir dir(dataDir);
     if (!dir.exists()) dir.mkpath(".");
 
     QString fileName = (id == -1)
                            ? QString("%1.json").arg(baseFileName)
                            : QString("%1_%2.json").arg(baseFileName).arg(id);
 
     return dir.filePath(fileName);
 }
 
 /**
  * @brief Zapisuje dane do pliku.
  * @param baseFileName Bazowa nazwa pliku.
  * @param data Dane do zapisania.
  * @param id Identyfikator (opcjonalny, domyślnie -1).
  */
 void DataManager::saveDataToFile(const QString &baseFileName, const QByteArray &data, int id) {
     QString path = getDataFilePath(baseFileName, id);
     QFile file(path);
     if (file.open(QIODevice::WriteOnly)) {
         file.write(data);
         file.close();
     }
 }
 
 /**
  * @brief Wczytuje dane z pliku.
  * @param baseFileName Bazowa nazwa pliku.
  * @param id Identyfikator (opcjonalny, domyślnie -1).
  * @return Wczytane dane jako QByteArray.
  */
 QByteArray DataManager::loadDataFromFile(const QString &baseFileName, int id) {
     QString path = getDataFilePath(baseFileName, id);
     QFile file(path);
     if (file.open(QIODevice::ReadOnly)) {
         QByteArray data = file.readAll();
         file.close();
         return data;
     }
     return QByteArray();
 }
 
 /**
  * @brief Zapisuje historyczne dane z oznaczeniem typu.
  * @param type Typ danych (np. "stations", "sensors").
  * @param data Dane do zapisania.
  * @param id Identyfikator (opcjonalny, domyślnie -1).
  */
 void DataManager::saveHistoricalData(const QString &type, const QByteArray &data, int id) {
     QString fileName;
 
     if (type == "stations") {
         fileName = "stations.json";
     } else if (type == "sensors") {
         fileName = QString("%1_%2.json").arg(type).arg(id);
     } else {
         QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
         fileName = QString("%1_%2_%3.json").arg(type).arg(id).arg(timestamp);
     }
 
     QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
     QDir dir(path);
     if (!dir.exists()) dir.mkpath(".");
 
     QFile file(dir.filePath(fileName));
     if (file.open(QIODevice::WriteOnly)) {
         file.write(data);
         file.close();
     }
 }
 
 /**
  * @brief Wczytuje wszystkie historyczne dane dla danego typu i identyfikatora.
  * @param type Typ danych (np. "measurements").
  * @param id Identyfikator.
  * @return Wektor par zawierających znacznik czasowy i dane.
  */
 QVector<QPair<QDateTime, QByteArray>> DataManager::loadAllHistoricalData(const QString &type, int id) {
     QVector<QPair<QDateTime, QByteArray>> result;
     QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
     QDir dir(path);
 
     if (type == "measurements") {
         QString filter = QString("%1_%2_*.json").arg(type).arg(id);
         QStringList files = dir.entryList(QStringList() << filter, QDir::Files, QDir::Time);
 
         foreach (const QString &file, files) {
             QFile f(dir.filePath(file));
             if (f.open(QIODevice::ReadOnly)) {
                 QStringList parts = file.split('_');
                 if (parts.size() >= 4) {
                     QString timestampStr = parts[2] + "_" + parts[3].left(6);
                     QDateTime dt = QDateTime::fromString(timestampStr, "yyyyMMdd_HHmmss");
                     result.append(qMakePair(dt, f.readAll()));
                 }
                 f.close();
             }
         }
     }
     return result;
 }