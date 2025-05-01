/**
 * @file connectionmanager.h
 * @brief Definicja klasy ConnectionManager do zarządzania połączeniem sieciowym.
 */

 #ifndef CONNECTIONMANAGER_H
 #define CONNECTIONMANAGER_H
 
 #include <QObject>
 #include <QListWidget>
 #include <QLabel>
 #include <QNetworkAccessManager>
 #include <QNetworkRequest>
 #include <QNetworkReply>
 #include <QJsonDocument>
 
 class ApiClient;
 
 /**
  * @class ConnectionManager
  * @brief Klasa odpowiedzialna za sprawdzanie połączenia sieciowego i ładowanie danych stacji.
  */
 class ConnectionManager : public QObject
 {
     Q_OBJECT
 
 public:
     /**
      * @brief Konstruktor klasy ConnectionManager.
      * @param parent Wskaźnik na obiekt nadrzędny.
      */
     explicit ConnectionManager(QObject *parent = nullptr);
 
     /**
      * @brief Sprawdza połączenie sieciowe i ładuje dane stacji.
      * @param apiClient Wskaźnik na obiekt ApiClient do wysyłania żądań.
      * @param stationList Wskaźnik na listę stacji w interfejsie użytkownika.
      * @param lblStatus Wskaźnik na etykietę statusu połączenia.
      * @param lblStationCount Wskaźnik na etykietę liczby stacji.
      * @param isOffline Flaga wskazująca brak połączenia.
      * @param allStations Wektor przechowujący dane wszystkich stacji.
      */
     void checkConnectionAndReloadStations(ApiClient *apiClient, QListWidget *stationList, QLabel *lblStatus, QLabel *lblStationCount, bool &isOffline, QVector<QPair<QString, QJsonObject>> &allStations);
 };
 
 #endif