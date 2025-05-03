/**
 * @file apiworker.cpp
 * @brief Implementacja klasy ApiWorker do wykonywania żądań sieciowych w oddzielnym wątku.
 */

#include "apiworker.h"

/**
 * @brief Konstruktor klasy ApiWorker.
 * 
 * Inicjalizuje obiekt klasy ApiWorker, wypisując informację debugującą z identyfikatorem bieżącego wątku.
 * Klasa odpowiada za asynchroniczne przetwarzanie żądań sieciowych.
 * 
 * @param parent Wskaźnik na obiekt nadrzędny (QObject), domyślnie nullptr.
 */
ApiWorker::ApiWorker(QObject *parent) : QObject(parent)
{
    //qDebug() << "ApiWorker constructor - thread:" << QThread::currentThreadId();
}

/**
 * @brief Destruktor klasy ApiWorker.
 * 
 * Zwalnia zasoby związane z obiektem ApiWorker.
 */
ApiWorker::~ApiWorker() {}

/**
 * @brief Przetwarza żądanie sieciowe dla podanego adresu URL.
 * 
 * Tworzy obiekt QNetworkRequest z podanym adresem URL, ustawia nagłówek User-Agent na "MJP" 
 * i wysyła żądanie GET za pomocą QNetworkAccessManager. Przechowuje mapowanie odpowiedzi 
 * na identyfikator żądania w replyToRequestId.
 * 
 * @param url Adres URL, z którego mają zostać pobrane dane.
 * @param requestId Identyfikator żądania.
 */
void ApiWorker::processRequest(const QUrl &url, int requestId)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "MJP");

    QNetworkReply *reply = manager->get(request);
    replyToRequestId[reply] = requestId;
}

/**
 * @brief Obsługuje zakończenie odpowiedzi sieciowej.
 * 
 * Sprawdza, czy odpowiedź nie zawiera błędu. Jeśli brak błędu, odczytuje dane, konwertuje je na QString 
 * i emituje sygnał resultReady. W przypadku błędu emituje sygnał errorOccurred z opisem błędu. 
 * Następnie usuwa odpowiedź i jej mapowanie oraz emituje sygnał finished, jeśli nie ma więcej oczekujących odpowiedzi.
 * 
 * @param reply Wskaźnik na obiekt QNetworkReply zawierający odpowiedź sieciową.
 */
void ApiWorker::onReplyFinished(QNetworkReply *reply)
{
    int requestId = replyToRequestId.value(reply, -1);

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray bytes = reply->readAll();
        QString response = QString::fromUtf8(bytes);
        emit resultReady(response, requestId);
    } else {
        emit errorOccurred(reply->errorString(), requestId);
    }

    reply->deleteLater();
    replyToRequestId.remove(reply);

    if (replyToRequestId.isEmpty()) {
        emit finished();
    }
    //qDebug() << "onReplyFinished — thread:" << QThread::currentThreadId();
}

/**
 * @brief Inicjalizuje obiekt ApiWorker.
 * 
 * Tworzy nowy obiekt QNetworkAccessManager i łączy sygnał finished z slotem onReplyFinished, 
 * umożliwiając obsługę odpowiedzi sieciowych. Wypisuje informację debugującą z identyfikatorem bieżącego wątku.
 */
void ApiWorker::init() {
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &ApiWorker::onReplyFinished);
    //qDebug() << "ApiWorker::init() — thread:" << QThread::currentThreadId();
}
