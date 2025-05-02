#include "connectionmanager.h"
#include "apiclient.h"
#include "datamanager.h"
#include "stationhandler.h"

/**
 * @brief Konstruktor klasy ConnectionManager.
 * 
 * Inicjalizuje obiekt klasy ConnectionManager, ustawiając obiekt nadrzędny (QObject). 
 * Klasa ta odpowiada za zarządzanie połączeniem sieciowym i ładowanie danych stacji.
 * 
 * @param parent Wskaźnik na obiekt nadrzędny (QObject), domyślnie nullptr.
 */
ConnectionManager::ConnectionManager(QObject *parent) : QObject(parent) {}

/**
 * @brief Sprawdza połączenie internetowe i ładuje dane stacji.
 * 
 * Wykonuje żądanie sieciowe do adresu URL (http://www.google.com) w celu sprawdzenia dostępności połączenia internetowego.
 * W zależności od wyniku:
 * - Jeśli połączenie jest dostępne (`NoError`), ustawia status na "Połączono", zmienia kolor etykiety na zielony 
 *   i, jeśli lista stacji jest pusta, wysyła żądanie do API w celu pobrania danych wszystkich stacji.
 * - Jeśli brak połączenia, ustawia status na "Brak połączenia - wczytano dane lokalne", zmienia kolor etykiety na czerwony
 *   i, jeśli lista stacji jest pusta, próbuje wczytać dane stacji z lokalnego pliku JSON.
 * Po zakończeniu operacji zwalnia zasoby (`rep` i `mgr`).
 * 
 * @param apiClient Wskaźnik na obiekt ApiClient do wysyłania żądań API.
 * @param stationList Wskaźnik na QListWidget, w którym wyświetlane są nazwy stacji.
 * @param lblStatus Wskaźnik na QLabel wyświetlający status połączenia.
 * @param lblStationCount Wskaźnik na QLabel wyświetlający liczbę stacji.
 * @param isOffline Referencja do flagi wskazującej, czy aplikacja działa w trybie offline.
 * @param allStations Referencja do wektora przechowującego dane wszystkich stacji (nazwa i obiekt JSON).
 */
void ConnectionManager::checkConnectionAndReloadStations(ApiClient *apiClient, QListWidget *stationList, QLabel *lblStatus, QLabel *lblStationCount, bool &isOffline, QVector<QPair<QString, QJsonObject>> &allStations) {
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest req(QUrl("http://www.google.com"));
    QNetworkReply *rep = mgr->get(req);

    connect(rep, &QNetworkReply::finished, [apiClient, stationList, lblStatus, lblStationCount, &isOffline, &allStations, rep, mgr]() {
        if (rep->error() == QNetworkReply::NoError) {
            isOffline = false;
            lblStatus->setText("Połączono");
            lblStatus->setStyleSheet("color: green;");
            if (stationList->count() == 0) {
                apiClient->fetchData(QUrl("https://api.gios.gov.pl/pjp-api/rest/station/findAll"));
            }
        } else {
            isOffline = true;
            lblStatus->setText("Brak połączenia - wczytano dane lokalne");
            lblStatus->setStyleSheet("color: red;");
            if (stationList->count() == 0) {
                QByteArray stationsData = DataManager::loadDataFromFile("stations");
                if (!stationsData.isEmpty()) {
                    QJsonDocument doc = QJsonDocument::fromJson(stationsData);
                    if (doc.isArray()) {
                        QList<QPair<QString, QJsonObject>> allStations;
                        StationHandler::handleStationsData(doc.array(), stationList, lblStationCount, allStations);
                    }
                }
            }
        }
        rep->deleteLater();
        mgr->deleteLater();
    });
}