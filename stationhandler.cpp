/**
 * @file stationhandler.cpp
 * @brief Implementacja klasy StationHandler do obsługi danych stacji.
 */

#include "stationhandler.h"

/**
 * @brief Przetwarza dane stacji z tablicy JSON i aktualizuje listę stacji.
 * 
 * Czyści listę stacji (`stationList`) oraz wektor `allStations`, a następnie przetwarza tablicę JSON 
 * zawierającą dane stacji. Dla każdej stacji wyciąga nazwę stacji, miasto i adres, tworząc tekst 
 * wyświetlany w formacie "<miasto> | <adres lub nazwa stacji>". Zapisuje dane w wektorze `allStations` 
 * jako pary (tekst wyświetlany, obiekt JSON). Następnie wywołuje `updateStationList` w celu aktualizacji 
 * interfejsu użytkownika bez filtrowania.
 * 
 * @param array Tablica JSON zawierająca dane stacji.
 * @param stationList Wskaźnik na `QListWidget`, w którym wyświetlane są nazwy stacji.
 * @param lblStationCount Wskaźnik na `QLabel` wyświetlający liczbę stacji.
 * @param allStations Referencja do listy przechowującej pary (tekst wyświetlany, obiekt JSON) dla wszystkich stacji.
 * @note Funkcja kończy działanie, jeśli tablica JSON jest pusta.
 */
void StationHandler::handleStationsData(const QJsonArray &array, QListWidget *stationList, QLabel *lblStationCount, QList<QPair<QString, QJsonObject>> &allStations) {
    if (array.isEmpty()) {
        return;
    }

    stationList->clear();
    allStations.clear();

    for (const QJsonValue &value : array) {
        QJsonObject obj = value.toObject();
        QString name = obj["stationName"].toString();
        QString city = obj["city"].toObject()["name"].toString();
        QString address = obj["addressStreet"].toString();
        QString display = city + "  |  " + (!address.isEmpty() ? address : name);
        allStations.append(qMakePair(display, obj));
    }
    updateStationList("", stationList, lblStationCount, allStations);
}

/**
 * @brief Aktualizuje listę stacji w interfejsie użytkownika na podstawie filtra.
 * 
 * Czyści listę stacji (`stationList`) i tworzy nową listę stacji na podstawie wektora `allStations`, 
 * uwzględniając podany filtr tekstowy (ignorując wielkość liter). Sortuje stacje alfabetycznie 
 * według tekstu wyświetlanego i dodaje je do `stationList` jako elementy `QListWidgetItem` 
 * z przypisanym identyfikatorem w danych użytkownika. Jeśli lista nie jest pusta, wybiera 
 * i podświetla pierwszy element. Aktualizuje etykietę `lblStationCount` z liczbą stacji.
 * 
 * @param filter Tekst filtra do wyszukiwania stacji (pusty filtr oznacza brak filtrowania).
 * @param stationList Wskaźnik na `QListWidget`, w którym wyświetlane są nazwy stacji.
 * @param lblStationCount Wskaźnik na `QLabel` wyświetlający liczbę stacji.
 * @param allStations Lista przechowująca pary (tekst wyświetlany, obiekt JSON) dla wszystkich stacji.
 */
void StationHandler::updateStationList(const QString &filter, QListWidget *stationList, QLabel *lblStationCount, const QList<QPair<QString, QJsonObject>> &allStations) {
    stationList->clear();
    QList<QPair<QString, QJsonObject>> filteredStations;
    for (const auto &pair : allStations) {
        if (filter.isEmpty() || pair.first.toLower().contains(filter.toLower())) {
            filteredStations.append(pair);
        }
    }

    std::sort(filteredStations.begin(), filteredStations.end(), [](const auto &a, const auto &b) {
        return a.first.toLower() < b.first.toLower();
    });

    for (const auto &pair : filteredStations) {
        QListWidgetItem *item = new QListWidgetItem(pair.first);
        item->setData(Qt::UserRole, pair.second["id"].toInt());
        stationList->addItem(item);
    }

    if (stationList->count() > 0) {
        stationList->setCurrentRow(0);
        stationList->item(0)->setSelected(true);
    }

    QString labelText = filter.isEmpty()
                            ? "Dostępne stacje [ " + QString::number(stationList->count()) + " ]"
                            : "Wyszukane stacje [ " + QString::number(stationList->count()) + " ]";
    lblStationCount->setText(labelText);
}