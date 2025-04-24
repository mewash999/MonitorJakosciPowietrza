#include "apiclient.h"
#include "apiworker.h"

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

ApiClient::~ApiClient()
{
    workerThread.quit();
    workerThread.wait();
}

void ApiClient::fetchData(const QUrl &url)
{
    int requestId = nextRequestId++;
    emit requestApiData(url, requestId);
}

void ApiClient::fetchSensorData(int sensorId)
{
    QUrl url(QString("https://api.gios.gov.pl/pjp-api/rest/data/getData/%1").arg(sensorId));
    int requestId = nextRequestId++;
    emit requestApiData(url, requestId);
}

void ApiClient::handleResults(const QString &result, int requestId)
{
    emit dataReady(result);
    requestTypes.remove(requestId);
    qDebug() << "handleResults — thread:" << QThread::currentThreadId();
}

void ApiClient::handleErrors(const QString &error, int requestId)
{
    emit errorOccurred(error);
    requestTypes.remove(requestId);
}
