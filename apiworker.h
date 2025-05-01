/**
 * @file apiworker.h
 * @brief Definicja klasy ApiWorker odpowiedzialnej za przetwarzanie żądań sieciowych.
 */

 #ifndef APIWORKER_H
 #define APIWORKER_H
 
 #include <QObject>
 #include <QNetworkAccessManager>
 #include <QNetworkReply>
 #include <QMap>
 #include <QThread>
 
 /**
  * @class ApiWorker
  * @brief Klasa zarządzająca żądaniami sieciowymi w osobnym wątku.
  */
 class ApiWorker : public QObject
 {
     Q_OBJECT
 
 public:
     /**
      * @brief Konstruktor klasy ApiWorker.
      * @param parent Wskaźnik na obiekt nadrzędny.
      */
     explicit ApiWorker(QObject *parent = nullptr);
 
     /**
      * @brief Destruktor klasy ApiWorker.
      */
     ~ApiWorker();
 
 public slots:
     /**
      * @brief Przetwarza żądanie sieciowe dla podanego URL.
      * @param url Adres URL żądania.
      * @param requestId Identyfikator żądania.
      */
     void processRequest(const QUrl &url, int requestId);
 
     /**
      * @brief Inicjalizuje menedżera sieciowego.
      */
     void init();
 
 signals:
     /**
      * @brief Sygnał emitowany, gdy wynik żądania jest gotowy.
      * @param result Wynik żądania w formacie QString.
      * @param requestId Identyfikator żądania.
      */
     void resultReady(const QString &result, int requestId);
 
     /**
      * @brief Sygnał emitowany w przypadku błędu żądania.
      * @param error Opis błędu.
      * @param requestId Identyfikator żądania.
      */
     void errorOccurred(const QString &error, int requestId);
 
     /**
      * @brief Sygnał emitowany, gdy wszystkie żądania zostały zakończone.
      */
     void finished();
 
 private slots:
     /**
      * @brief Obsługuje zakończenie odpowiedzi sieciowej.
      * @param reply Wskaźnik na obiekt odpowiedzi sieciowej.
      */
     void onReplyFinished(QNetworkReply *reply);
 
 private:
     QNetworkAccessManager *manager; ///< Menedżer sieciowy do wysyłania żądań.
     QMap<QNetworkReply*, int> replyToRequestId; ///< Mapa wiążąca odpowiedzi z identyfikatorami żądań.
 };
 
 #endif