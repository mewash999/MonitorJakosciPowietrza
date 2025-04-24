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
    explicit ApiWorker(QObject *parent = nullptr);
    ~ApiWorker();

public slots:
    void processRequest(const QUrl &url, int requestId);
    void init();

signals:
    void resultReady(const QString &result, int requestId);
    void errorOccurred(const QString &error, int requestId);
    void finished();

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
    QMap<QNetworkReply*, int> replyToRequestId;
};

#endif
