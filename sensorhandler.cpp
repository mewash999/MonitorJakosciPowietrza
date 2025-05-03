/**
 * @file sensorhandler.cpp
 * @brief Implementacja klasy SensorHandler do obsługi danych czujników.
 */

#include "sensorhandler.h"

/**
 * @brief Przetwarza dane czujników z tablicy JSON i aktualizuje listę czujników.
 * 
 * Czyści listę czujników (`sensorList`) oraz wektor `currentSensors`, a następnie przetwarza tablicę JSON 
 * zawierającą dane czujników. Dla każdego czujnika wyciąga nazwę parametru (`paramName`) i identyfikator (`id`), 
 * zapisując je w wektorze `currentSensors`. Tworzy posortowaną listę czujników według nazwy parametru 
 * (ignorując wielkość liter) i dodaje je do `sensorList` jako elementy `QListWidgetItem` z przypisanym 
 * identyfikatorem w danych użytkownika. Jeśli lista nie jest pusta, wybiera i podświetla pierwszy element.
 * 
 * @param array Tablica JSON zawierająca dane czujników.
 * @param sensorList Wskaźnik na `QListWidget`, w którym wyświetlane są nazwy czujników.
 * @param currentSensors Referencja do wektora przechowującego pary (identyfikator, nazwa parametru) dla bieżących czujników.
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