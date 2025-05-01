/**
 * @file datamanager.h
 * @brief Definicja klasy DataManager do zarządzania danymi zapisywanymi na dysku.
 */

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
 
 /**
  * @class DataManager
  * @brief Klasa statyczna do zapisywania i wczytywania danych z plików.
  */
 class DataManager
 {
 public:
     /**
      * @brief Pobiera ścieżkę do pliku danych.
      * @param baseFileName Bazowa nazwa pliku.
      * @param id Identyfikator (opcjonalny, domyślnie -1).
      * @return Ścieżka do pliku.
      */
     static QString getDataFilePath(const QString &baseFileName, int id = -1);
 
     /**
      * @brief Zapisuje dane do pliku.
      * @param baseFileName Bazowa nazwa pliku.
      * @param data Dane do zapisania.
      * @param id Identyfikator (opcjonalny, domyślnie -1).
      */
     static void saveDataToFile(const QString &baseFileName, const QByteArray &data, int id = -1);
 
     /**
      * @brief Wczytuje dane z pliku.
      * @param baseFileName Bazowa nazwa pliku.
      * @param id Identyfikator (opcjonalny, domyślnie -1).
      * @return Wczytane dane jako QByteArray.
      */
     static QByteArray loadDataFromFile(const QString &baseFileName, int id = -1);
 
     /**
      * @brief Zapisuje historyczne dane z oznaczeniem typu.
      * @param type Typ danych (np. "stations", "sensors").
      * @param data Dane do zapisania.
      * @param id Identyfikator (opcjonalny, domyślnie -1).
      */
     static void saveHistoricalData(const QString &type, const QByteArray &data, int id = -1);
 
     /**
      * @brief Wczytuje wszystkie historyczne dane dla danego typu i identyfikatora.
      * @param type Typ danych (np. "measurements").
      * @param id Identyfikator.
      * @return Wektor par zawierających znacznik czasowy i dane.
      */
     static QVector<QPair<QDateTime, QByteArray>> loadAllHistoricalData(const QString &type, int id);
 };
 
 #endif