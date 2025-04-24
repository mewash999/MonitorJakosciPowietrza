#ifndef APIWORKER_H
#define APIWORKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QString>
#include <QDebug>
#include <QThread>

class ApiWorker : public QObject
{
    Q_OBJECT

public:
    explicit ApiWorker(QObject *parent = nullptr);
    ~ApiWorker();
    void init();

public slots:
    void processRequest(const QUrl &url, int requestId = -1);

signals:
    void resultReady(const QString &result, int requestId);
    void errorOccurred(const QString &error, int requestId);
    void finished();

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager = nullptr;
    QMap<QNetworkReply*, int> replyToRequestId;
};

#endif
