#ifndef APICLIENT_H
#define APICLIENT_H

#include <QThread>
#include <QDebug>
#include "apiworker.h"

class ApiClient : public QObject
{
    Q_OBJECT

public:
    explicit ApiClient(QObject *parent = nullptr);
    ~ApiClient();
    void fetchData(const QUrl& url);
    void fetchSensorData(int sensorId);


signals:
    void dataReady(const QString &data);
    void errorOccurred(const QString &error);
    void requestApiData(const QUrl &url, int requestId);
    void finished();

private slots:
    void handleResults(const QString &result, int requestId);
    void handleErrors(const QString &error, int requestId);

private:
    QThread workerThread;
    ApiWorker *worker;
    int nextRequestId;
    QMap<int, QString> requestTypes;
};

#endif
