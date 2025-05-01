/**
 * @file apiclient.cpp
 * @brief Implementacja klasy ApiClient do zarządzania żądaniami API.
 */

 #include "apiclient.h"
 #include "apiworker.h"
 
 /**
  * @brief Konstruktor klasy ApiClient.
  * @param parent Wskaźnik na obiekt nadrzędny.
  */
 ApiClient::ApiClient(QObject *parent)
     : QObject(parent), nextRequestId(0)
 {
     worker = new ApiWorker();
     worker->moveToThread(&workerThread);
 
     connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
     connect(this, &ApiClient::requestApiData, worker, &ApiWorker::processRequest);
     connect(worker, &ApiWorker::resultReady, this, &ApiClient::handleResults);
     connect(worker, &ApiWorker::errorOccurred, this, &ApiClient::handleErrors);
 
     QMetaObject::invokeMethod(worker, &ApiWorker::init, Qt::QueuedConnection);
 
     workerThread.start();
 }
 
 /**
  * @brief Destruktor klasy ApiClient.
  */
 ApiClient::~ApiClient()
 {
     workerThread.quit();
     workerThread.wait();
 }
 
 /**
  * @brief Pobiera dane z podanego URL.
  * @param url Adres URL do wysłania żądania.
  */
 void ApiClient::fetchData(const QUrl &url)
 {
     int requestId = nextRequestId++;
     emit requestApiData(url, requestId);
 }
 
 /**
  * @brief Pobiera dane dla konkretnego sensora.
  * @param sensorId Identyfikator sensora.
  */
 void ApiClient::fetchSensorData(int sensorId)
 {
     QUrl url(QString("https://api.gios.gov.pl/pjp-api/rest/data/getData/%1").arg(sensorId));
     int requestId = nextRequestId++;
     emit requestApiData(url, requestId);
 }
 
 /**
  * @brief Obsługuje wyniki żądania.
  * @param result Wynik żądania w formacie QString.
  * @param requestId Identyfikator żądania.
  */
 void ApiClient::handleResults(const QString &result, int requestId)
 {
     emit dataReady(result);
     requestTypes.remove(requestId);
     qDebug() << "handleResults — thread:" << QThread::currentThreadId();
 }
 
 /**
  * @brief Obsługuje błędy żądania.
  * @param error Opis błędu.
  * @param requestId Identyfikator żądania.
  */
 void ApiClient::handleErrors(const QString &error, int requestId)
 {
     emit errorOccurred(error);
     requestTypes.remove(requestId);
 }