/**
 * @file sensorhandler.h
 * @brief Definicja klasy SensorHandler do obsługi danych czujników.
 */

 #ifndef SENSORHANDLER_H
 #define SENSORHANDLER_H
 
 #include <QJsonArray>
 #include <QListWidget>
 #include <QJsonObject>
 #include <algorithm>
 
 /**
  * @class SensorHandler
  * @brief Klasa statyczna do zarządzania danymi czujników w interfejsie.
  */
 class SensorHandler
 {
 public:
     /**
      * @brief Przetwarza dane czujników z tablicy JSON.
      * @param array Tablica JSON z danymi czujników.
      * @param sensorList Wskaźnik na listę czujników w interfejsie.
      * @param currentSensors Wektor przechowujący aktualne czujniki.
      */
     static void handleSensorsData(const QJsonArray &array, QListWidget *sensorList, QVector<QPair<int, QString>> &currentSensors);
 };
 
 #endif