#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QListWidget>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>

class ApiClient;

class ConnectionManager : public QObject
{
    Q_OBJECT

public:
    explicit ConnectionManager(QObject *parent = nullptr);
    void checkConnectionAndReloadStations(ApiClient *apiClient, QListWidget *stationList, QLabel *lblStatus, QLabel *lblStationCount, bool &isOffline, QVector<QPair<QString, QJsonObject>> &allStations);
};

#endif
