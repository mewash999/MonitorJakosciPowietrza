/**
 * @file stationhandler.h
 * @brief Definicja klasy StationHandler do obsługi danych stacji.
 */

#ifndef STATIONHANDLER_H
#define STATIONHANDLER_H

#include <QJsonArray>
#include <QListWidget>
#include <QLabel>
#include <QJsonObject>
#include <QString>
#include <algorithm>
#include <QJsonObject>

class StationHandler
{
public:
    /**
     * @brief Przetwarza dane stacji z tablicy JSON i aktualizuje listę stacji.
     * 
     * Przetwarza tablicę JSON z danymi stacji, tworząc listę stacji w formacie "<miasto> | <adres lub nazwa stacji>" 
     * i zapisuje je w wektorze `allStations`. Aktualizuje interfejs użytkownika poprzez wywołanie `updateStationList`.
     * 
     * @param array Tablica JSON zawierająca dane stacji.
     * @param stationList Wskaźnik na `QListWidget`, w którym wyświetlane są nazwy stacji.
     * @param lblStationCount Wskaźnik na `QLabel` wyświetlający liczbę stacji.
     * @param allStations Referencja do listy przechowującej pary (tekst wyświetlany, obiekt JSON) dla wszystkich stacji.
     */
    static void handleStationsData(const QJsonArray &array, QListWidget *stationList, QLabel *lblStationCount, QList<QPair<QString, QJsonObject>> &allStations);

    /**
     * @brief Aktualizuje listę stacji w interfejsie użytkownika na podstawie filtra.
     * 
     * Filtruje i sortuje stacje z listy `allStations` według podanego filtra tekstowego, 
     * aktualizuje `stationList` i etykietę `lblStationCount` z liczbą stacji.
     * 
     * @param filter Tekst filtra do wyszukiwania stacji (pusty filtr oznacza brak filtrowania).
     * @param stationList Wskaźnik na `QListWidget`, w którym wyświetlane są nazwy stacji.
     * @param lblStationCount Wskaźnik na `QLabel` wyświetlający liczbę stacji.
     * @param allStations Lista przechowująca pary (tekst wyświetlany, obiekt JSON) dla wszystkich stacji.
     */
    static void updateStationList(const QString &filter, QListWidget *stationList, QLabel *lblStationCount, const QList<QPair<QString, QJsonObject>> &allStations);
};

#endif