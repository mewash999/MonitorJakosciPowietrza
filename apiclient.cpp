/**
 * @file apiclient.cpp
 * @brief Implementacja klasy ApiClient do zarządzania żądaniami API.
 */

#include "apiclient.h"
#include "apiworker.h"

/**
 * @brief Konstruktor klasy ApiClient.
 * 
 * Inicjalizuje obiekt klasy ApiClient, tworząc nowy wątek roboczy (`workerThread`) oraz obiekt `ApiWorker`, 
 * który jest przenoszony do tego wątku. Ustawia połączenia sygnałów i slotów między `ApiClient` a `ApiWorker`, 
 * aby umożliwić asynchroniczne przetwarzanie żądań API oraz obsługę wyników i błędów. Wywołuje metodę `init` 
 * obiektu `ApiWorker` w sposób opóźniony (QueuedConnection) i uruchamia wątek roboczy.
 * 
 * @param parent Wskaźnik na obiekt nadrzędny (QObject), domyślnie nullptr.
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
 * 
 * Zamyka wątek roboczy (`workerThread`) poprzez wywołanie `quit()` i czeka na jego zakończenie za pomocą `wait()`. 
 * Zapewnia poprawne zwolnienie zasobów związanych z wątkiem i obiektem `ApiWorker`.
 */
ApiClient::~ApiClient()
{
    workerThread.quit();
    workerThread.wait();
}

/**
 * @brief Wysyła żądanie pobrania danych z podanego adresu URL.
 * 
 * Generuje unikalny identyfikator żądania (`requestId`) i emituje sygnał `requestApiData` z podanym adresem URL 
 * oraz identyfikatorem. Żądanie jest przekazywane do obiektu `ApiWorker` w celu asynchronicznego przetworzenia.
 * 
 * @param url Adres URL, z którego mają zostać pobrane dane.
 * @note Funkcja zwiększa licznik `nextRequestId` dla każdego nowego żądania.
 */
void ApiClient::fetchData(const QUrl &url)
{
    int requestId = nextRequestId++;
    emit requestApiData(url, requestId);
}

/**
 * @brief Wysyła żądanie pobrania danych dla konkretnego czujnika na podstawie jego identyfikatora.
 * 
 * Tworzy adres URL w formacie specyficznym dla API (https://api.gios.gov.pl/pjp-api/rest/data/getData/<sensorId>), 
 * generuje unikalny identyfikator żądania (`requestId`) i emituje sygnał `requestApiData` z utworzonym adresem URL 
 * oraz identyfikatorem. Żądanie jest przekazywane do obiektu `ApiWorker` w celu asynchronicznego przetworzenia.
 * 
 * @param sensorId Identyfikator czujnika, dla którego mają zostać pobrane dane.
 * @note Funkcja zwiększa licznik `nextRequestId` dla każdego nowego żądania.
 */
void ApiClient::fetchSensorData(int sensorId)
{
    QUrl url(QString("https://api.gios.gov.pl/pjp-api/rest/data/getData/%1").arg(sensorId));
    int requestId = nextRequestId++;
    emit requestApiData(url, requestId);
}

/**
 * @brief Obsługuje wyniki żądania zwrócone przez obiekt ApiWorker.
 * 
 * Emituje sygnał `dataReady` z otrzymanymi danymi w formacie QString oraz usuwa identyfikator żądania 
 * z mapy `requestTypes`. Dodatkowo wyświetla informację debugującą z identyfikatorem bieżącego wątku.
 * 
 * @param result Dane zwrócone w odpowiedzi na żądanie API, w formacie QString.
 * @param requestId Identyfikator żądania, dla którego zwrócono wyniki.
 */
void ApiClient::handleResults(const QString &result, int requestId)
{
    emit dataReady(result);
    requestTypes.remove(requestId);
    //qDebug() << "handleResults — thread:" << QThread::currentThreadId();
}

/**
 * @brief Obsługuje błędy zgłoszone przez obiekt ApiWorker.
 * 
 * Emituje sygnał `errorOccurred` z opisem błędu w formacie QString oraz usuwa identyfikator żądania 
 * z mapy `requestTypes`.
 * 
 * @param error Opis błędu zwrócony w odpowiedzi na żądanie API, w formacie QString.
 * @param requestId Identyfikator żądania, dla którego zgłoszono błąd.
 */
void ApiClient::handleErrors(const QString &error, int requestId)
{
    emit errorOccurred(error);
    requestTypes.remove(requestId);
}
