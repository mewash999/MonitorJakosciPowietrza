/**
 * @file apiclient.h
 * @brief Definicja klasy ApiClient do zarządzania żądaniami API.
 */

 #ifndef APICLIENT_H
 #define APICLIENT_H
 
 #include <QObject>
 #include <QThread>
 #include "apiworker.h"
 
 /**
  * @class ApiClient
  * @brief Klasa do zarządzania żądaniami API w oddzielnym wątku.
  * @details Klasa koordynuje żądania sieciowe, delegując je do obiektu ApiWorker działającego w oddzielnym wątku. Zapewnia asynchroniczne pobieranie danych z API, emitując sygnały dataReady po otrzymaniu danych lub errorOccurred w przypadku błędu. Klasa zarządza cyklem życia wątku i obiektu ApiWorker, zapewniając bezpieczne zwalnianie zasobów.
  */
 class ApiClient : public QObject {
     Q_OBJECT
 public:
     /**
      * @brief Konstruktor klasy ApiClient.
      * @details Inicjalizuje obiekt ApiClient, tworząc nowy wątek i obiekt ApiWorker. Przenosi ApiWorker do oddzielnego wątku i konfiguruje połączenia sygnałów między ApiClient a ApiWorker, aby umożliwić przekazywanie danych i błędów. Uruchamia wątek, zapewniając asynchroniczną obsługę żądań.
      * @param parent Wskaźnik na obiekt nadrzędny, domyślnie nullptr.
      * @note Wątek i ApiWorker są zwalniane w destruktorze, aby zapobiec wyciekom pamięci.
      */
     explicit ApiClient(QObject *parent = nullptr);
 
     /**
      * @brief Destruktor klasy ApiClient.
      * @details Zatrzymuje wątek, w którym działa ApiWorker, i zwalnia zasoby, w tym obiekt ApiWorker i wątek. Zapewnia bezpieczne zakończenie wszystkich żądań sieciowych przed zniszczeniem obiektu.
      */
     ~ApiClient();
 
     /**
      * @brief Inicjuje żądanie sieciowe dla podanego adresu URL.
      * @details Przekazuje żądanie GET dla podanego adresu URL do obiektu ApiWorker w oddzielnym wątku. Po zakończeniu żądania ApiWorker emituje sygnały, które są przekazywane przez ApiClient jako dataReady (dla danych) lub errorOccurred (dla błędów).
      * @param url Adres URL, z którego pobierane są dane.
      * @note Funkcja nie wykonuje żądania bezpośrednio, lecz deleguje je do ApiWorker, zapewniając asynchroniczność.
      * @see ApiWorker::fetchData
      */
     void fetchData(const QUrl &url);
 
     /**
      * @brief Inicjuje żądanie sieciowe dla danych czujników stacji.
      * @details Przekazuje żądanie GET dla danych czujników stacji o podanym identyfikatorze do obiektu ApiWorker w oddzielnym wątku. Po zakończeniu żądania ApiWorker emituje sygnały, które są przekazywane przez ApiClient jako dataReady (dla danych) lub errorOccurred (dla błędów).
      * @param stationId Identyfikator stacji, dla której pobierane są dane czujników.
      * @note Funkcja deleguje żądanie do ApiWorker, który generuje odpowiedni URL na podstawie stationId.
      * @see ApiWorker::fetchSensorData
      */
     void fetchSensorData(int stationId);
 
 signals:
     /**
      * @brief Sygnał emitowany po pomyślnym pobraniu danych.
      * @details Emitowany, gdy żądanie sieciowe zakończy się sukcesem, przekazując pobrane dane w formacie QByteArray do slotów podłączonych do tego sygnału.
      * @param data Pobrane dane w formacie QByteArray.
      */
     void dataReady(const QByteArray &data);
 
     /**
      * @brief Sygnał emitowany w przypadku błędu sieciowego.
      * @details Emitowany, gdy żądanie sieciowe zakończy się niepowodzeniem, przekazując opis błędu w formacie QString do slotów podłączonych do tego sygnału.
      * @param errorString Opis błędu sieciowego.
      */
     void errorOccurred(const QString &errorString);
 
 private:
     QThread workerThread; ///< Wątek, w którym działa obiekt ApiWorker.
     ApiWorker *worker;    ///< Wskaźnik na obiekt ApiWorker do obsługi żądań sieciowych.
 };
 
 #endif