/**
 * @file apiclient.h
 * @brief Definicja klasy ApiClient do zarządzania żądaniami API.
 */

 #ifndef APICLIENT_H
 #define APICLIENT_H
 
 #include <QObject>
 #include <QUrl>
 #include <QThread>
 #include <QMap>
 
 class ApiWorker;
 
 /**
  * @class ApiClient
  * @brief Klasa odpowiedzialna za komunikację z API w osobnym wątku.
  */
 class ApiClient : public QObject
 {
     Q_OBJECT
 
 public:
     /**
      * @brief Konstruktor klasy ApiClient.
      * @param parent Wskaźnik na obiekt nadrzędny.
      */
     explicit ApiClient(QObject *parent = nullptr);
 
     /**
      * @brief Destruktor klasy ApiClient.
      */
     ~ApiClient();
 
     /**
      * @brief Pobiera dane z podanego URL.
      * @param url Adres URL do wysłania żądania.
      */
     void fetchData(const QUrl &url);
 
     /**
      * @brief Pobiera dane dla konkretnego sensora.
      * @param sensorId Identyfikator sensora.
      */
     void fetchSensorData(int sensorId);
 
 signals:
     /**
      * @brief Sygnał emitowany, gdy dane są gotowe.
      * @param data Otrzymane dane w formacie QString.
      */
     void dataReady(const QString &data);
 
     /**
      * @brief Sygnał emitowany w przypadku błędu.
      * @param error Opis błędu.
      */
     void errorOccurred(const QString &error);
 
     /**
      * @brief Sygnał do wysyłania żądania API.
      * @param url Adres URL żądania.
      * @param requestId Identyfikator żądania.
      */
     void requestApiData(const QUrl &url, int requestId);
 
 private slots:
     /**
      * @brief Obsługuje wyniki żądania.
      * @param result Wynik żądania w formacie QString.
      * @param requestId Identyfikator żądania.
      */
     void handleResults(const QString &result, int requestId);
 
     /**
      * @brief Obsługuje błędy żądania.
      * @param error Opis błędu.
      * @param requestId Identyfikator żądania.
      */
     void handleErrors(const QString &error, int requestId);
 
 private:
     ApiWorker *worker; ///< Wskaźnik na obiekt ApiWorker.
     QThread workerThread; ///< Wątek dla ApiWorker.
     int nextRequestId; ///< Kolejny identyfikator żądania.
     QMap<int, QString> requestTypes; ///< Mapa typów żądań.
 };
 
 #endif