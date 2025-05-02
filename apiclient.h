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
 
 class ApiClient : public QObject
 {
     Q_OBJECT
 
 public:
     /**
      * @brief Konstruktor klasy ApiClient.
      *
      * Inicjalizuje obiekt klasy ApiClient, tworząc nowy wątek roboczy (`workerThread`) oraz obiekt `ApiWorker`,
      * który jest przenoszony do tego wątku. Ustawia połączenia sygnałów i slotów między `ApiClient` a `ApiWorker`,
      * aby umożliwić asynchroniczne wysyłanie żądań API, odbieranie wyników oraz obsługę błędów. Wywołuje metodę
      * `init` obiektu `ApiWorker` w sposób opóźniony (QueuedConnection) i uruchamia wątek roboczy.
      *
      * @param parent Wskaźnik na obiekt nadrzędny (QObject), domyślnie nullptr.
      */
     explicit ApiClient(QObject *parent = nullptr);
 
     /**
      * @brief Destruktor klasy ApiClient.
      *
      * Zamyka wątek roboczy (`workerThread`) poprzez wywołanie `quit()` i czeka na jego zakończenie za pomocą
      * `wait()`. Zapewnia poprawne zwolnienie zasobów związanych z wątkiem i obiektem `ApiWorker`.
      */
     ~ApiClient();
 
     /**
      * @brief Wysyła żądanie pobrania danych z podanego adresu URL.
      *
      * Generuje unikalny identyfikator żądania (`requestId`) i emituje sygnał `requestApiData` z podanym adresem
      * URL oraz identyfikatorem. Żądanie jest przekazywane do obiektu `ApiWorker` w celu asynchronicznego
      * przetworzenia w osobnym wątku.
      *
      * @param url Adres URL, z którego mają zostać pobrane dane (np. lista stacji lub czujników).
      * @note Funkcja zwiększa licznik `nextRequestId` dla każdego nowego żądania.
      */
     void fetchData(const QUrl &url);
 
     /**
      * @brief Wysyła żądanie pobrania danych dla konkretnego czujnika na podstawie jego identyfikatora.
      *
      * Tworzy adres URL w formacie specyficznym dla API (https://api.gios.gov.pl/pjp-api/rest/data/getData/<sensorId>),
      * generuje unikalny identyfikator żądania (`requestId`) i emituje sygnał `requestApiData` z utworzonym adresem
      * URL oraz identyfikatorem. Żądanie jest przekazywane do obiektu `ApiWorker` w celu asynchronicznego
      * przetworzenia w osobnym wątku.
      *
      * @param sensorId Identyfikator czujnika, dla którego mają zostać pobrane dane pomiarowe.
      * @note Funkcja zwiększa licznik `nextRequestId` dla każdego nowego żądania.
      */
     void fetchSensorData(int sensorId);
 
 signals:
     /**
      * @brief Sygnał emitowany, gdy dane z żądania API są gotowe.
      *
      * Przekazuje zwrócone dane w formacie QString do slotów podłączonych do tego sygnału (np. `MainWindow::onDataReady`).
      *
      * @param data Dane zwrócone w odpowiedzi na żądanie API.
      */
     void dataReady(const QString &data);
 
     /**
      * @brief Sygnał emitowany, gdy wystąpi błąd podczas przetwarzania żądania API.
      *
      * Przekazuje opis błędu w formacie QString do slotów podłączonych do tego sygnału (np. `MainWindow::onErrorOccurred`).
      *
      * @param error Opis błędu zwrócony przez `ApiWorker`.
      */
     void errorOccurred(const QString &error);
 
     /**
      * @brief Sygnał emitowany w celu przekazania żądania API do ApiWorker.
      *
      * Przekazuje adres URL oraz identyfikator żądania do slotu `ApiWorker::processRequest` w celu asynchronicznego
      * przetworzenia.
      *
      * @param url Adres URL żądania API.
      * @param requestId Identyfikator żądania.
      */
     void requestApiData(const QUrl &url, int requestId);
 
 private slots:
     /**
      * @brief Obsługuje wyniki żądania zwrócone przez ApiWorker.
      *
      * Odbiera dane zwrócone przez `ApiWorker`, emituje sygnał `dataReady` z wynikami oraz usuwa identyfikator
      * żądania z mapy `requestTypes`. Wyświetla informację debugującą z identyfikatorem bieżącego wątku.
      *
      * @param result Dane zwrócone w odpowiedzi na żądanie API, w formacie QString.
      * @param requestId Identyfikator żądania, dla którego zwrócono wyniki.
      */
     void handleResults(const QString &result, int requestId);
 
     /**
      * @brief Obsługuje błędy zgłoszone przez ApiWorker.
      *
      * Odbiera opis błędu zwrócony przez `ApiWorker`, emituje sygnał `errorOccurred` oraz usuwa identyfikator
      * żądania z mapy `requestTypes`.
      *
      * @param error Opis błędu zwrócony w odpowiedzi na żądanie API, w formacie QString.
      * @param requestId Identyfikator żądania, dla którego zgłoszono błąd.
      */
     void handleErrors(const QString &error, int requestId);
 
 private:
     ApiWorker *worker;
     QThread workerThread;
     int nextRequestId;
     QMap<int, QString> requestTypes;
 };
 
 #endif
 