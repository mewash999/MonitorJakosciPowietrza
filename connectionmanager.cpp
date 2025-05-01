/**
 * @file connectionmanager.cpp
 * @brief Implementacja klasy ConnectionManager do zarządzania połączeniem sieciowym.
 */

 #include "connectionmanager.h"
 #include "apiclient.h"
 #include "datamanager.h"
 #include "stationhandler.h"
 
 /**
  * @brief Konstruktor klasy ConnectionManager.
  * @param parent Wskaźnik na obiekt nadrzędny.
  */
 ConnectionManager::ConnectionManager(QObject *parent) : QObject(parent) {}
 
 /**
  * @brief Sprawdza połączenie sieciowe i ładuje dane stacji.
  * @param apiClient Wskaźnik na obiekt ApiClient do wysyłania żądań.
  * @param stationList Wskaźnik na listę stacji w interfejsie użytkownika.
  * @param lblStatus Wskaźnik na etykietę statusu połączenia.
  * @param lblStationCount Wskaźnik na etykietę liczby stacji.
  * @param isOffline Flaga wskazująca brak połączenia.
  * @param allStations Wektor przechowujący dane wszystkich stacji.
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