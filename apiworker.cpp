/**
 * @file apiworker.cpp
 * @brief Implementacja klasy ApiWorker do obsługi żądań sieciowych.
 */

 #include "apiworker.h"

 /**
  * @brief Konstruktor klasy ApiWorker.
  * @param parent Wskaźnik na obiekt nadrzędny.
  */
 ApiWorker::ApiWorker(QObject *parent) : QObject(parent)
 {
     qDebug() << "ApiWorker constructor - thread:" << QThread::currentThreadId();
 }
 
 /**
  * @brief Destruktor klasy ApiWorker.
  */
 ApiWorker::~ApiWorker() {}
 
 /**
  * @brief Przetwarza żądanie sieciowe dla podanego URL.
  * @param url Adres URL żądania.
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
  * @param reply Wskaźnik na obiekt odpowiedzi sieciowej.
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
     qDebug() << "onReplyFinished — thread:" << QThread::currentThreadId();
 }
 
 /**
  * @brief Inicjalizuje menedżera sieciowego i łączy sygnały.
  */
 void ApiWorker::init() {
     manager = new QNetworkAccessManager(this);
     connect(manager, &QNetworkAccessManager::finished, this, &ApiWorker::onReplyFinished);
     qDebug() << "ApiWorker::init() — thread:" << QThread::currentThreadId();
 }