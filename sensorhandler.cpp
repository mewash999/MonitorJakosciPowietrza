/**
 * @file sensorhandler.cpp
 * @brief Implementacja klasy SensorHandler do obsługi danych czujników.
 */

 #include "sensorhandler.h"

 /**
  * @brief Przetwarza dane czujników z tablicy JSON.
  * @param array Tablica JSON z danymi czujników.
  * @param sensorList Wskaźnik na listę czujników w interfejsie.
  * @param currentSensors Wektor przechowujący aktualne czujniki.
  */
 void SensorHandler::handleSensorsData(const QJsonArray& array, QListWidget* sensorList, QVector<QPair<int, QString>>& currentSensors) {
     sensorList->clear();
     currentSensors.clear();
     QVector<QPair<QString, QJsonObject>> sensors;
 
     for (const QJsonValue &value : array) {
         QJsonObject obj = value.toObject();
         QString paramName = obj["param"].toObject()["paramName"].toString();
         int sensorId = obj["id"].toInt();
         currentSensors.append(qMakePair(sensorId, paramName));
         sensors.append(qMakePair(paramName, obj));
     }
 
     std::sort(sensors.begin(), sensors.end(), [](const auto& a, const auto& b) {
         return a.first.toLower() < b.first.toLower();
     });
 
     for (const auto& pair : sensors) {
         QListWidgetItem *item = new QListWidgetItem(pair.first);
         item->setData(Qt::UserRole, pair.second["id"].toInt());
         sensorList->addItem(item);
     }
 
     if(sensorList->count() > 0) {
         sensorList->setCurrentRow(0);
         sensorList->item(0)->setSelected(true);
     }
 }