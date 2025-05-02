#ifndef APIWORKER_H
#define APIWORKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>
#include <QThread>

class ApiWorker : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor klasy ApiWorker.
     * 
     * Inicjalizuje obiekt klasy ApiWorker, który obsługuje asynchroniczne żądania sieciowe.
     * 
     * @param parent Wskaźnik na obiekt nadrzędny (QObject), domyślnie nullptr.
     */
    explicit ApiWorker(QObject *parent = nullptr);

    /**
     * @brief Destruktor klasy ApiWorker.
     * 
     * Zwalnia zasoby związane z obiektem ApiWorker.
     */
    ~ApiWorker();

public slots:
    /**
     * @brief Przetwarza żądanie sieciowe dla podanego adresu URL.
     * 
     * Wysyła żądanie GET dla podanego adresu URL i przechowuje identyfikator żądania.
     * 
     * @param url Adres URL, z którego mają zostać pobrane dane.
     * @param requestId Identyfikator żądania.
     */
    void processRequest(const QUrl &url, int requestId);

    /**
     * @brief Inicjalizuje obiekt ApiWorker.
     * 
     * Tworzy QNetworkAccessManager i konfiguruje połączenia sygnałów dla obsługi odpowiedzi sieciowych.
     */
    void init();

signals:
    void resultReady(const QString &result, int requestId);
    void errorOccurred(const QString &error, int requestId);
    void finished();

private slots:
    /**
     * @brief Obsługuje zakończenie odpowiedzi sieciowej.
     * 
     * Przetwarza odpowiedź sieciową, emitując sygnał resultReady lub errorOccurred w zależności od wyniku.
     * 
     * @param reply Wskaźnik na obiekt QNetworkReply zawierający odpowiedź sieciową.
     */
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
    QMap<QNetworkReply*, int> replyToRequestId;
};

#endif