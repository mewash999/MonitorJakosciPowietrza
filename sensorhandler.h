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

class SensorHandler
{
public:
    /**
     * @brief Przetwarza dane czujników z tablicy JSON i aktualizuje listę czujników.
     * 
     * Przetwarza tablicę JSON z danymi czujników, tworząc listę posortowanych nazw parametrów 
     * i zapisując dane czujników w wektorze `currentSensors`. Aktualizuje `sensorList` w interfejsie użytkownika.
     * 
     * @param array Tablica JSON zawierająca dane czujników.
     * @param sensorList Wskaźnik na `QListWidget`, w którym wyświetlane są nazwy czujników.
     * @param currentSensors Referencja do wektora przechowującego pary (identyfikator, nazwa parametru) dla bieżących czujników.
     */
    static void handleSensorsData(const QJsonArray &array, QListWidget *sensorList, QVector<QPair<int, QString>> &currentSensors);
};

#endif