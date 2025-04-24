#include "apiworker.h"

ApiWorker::ApiWorker(QObject *parent) : QObject(parent)
{
    qDebug() << "ApiWorker constructor - thread:" << QThread::currentThreadId();
}

ApiWorker::~ApiWorker() {}

void ApiWorker::processRequest(const QUrl &url, int requestId)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "MJP");

    QNetworkReply *reply = manager->get(request);
    replyToRequestId[reply] = requestId;
}

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

void ApiWorker::init() {
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &ApiWorker::onReplyFinished);
    qDebug() << "ApiWorker::init() — thread:" << QThread::currentThreadId();
}
