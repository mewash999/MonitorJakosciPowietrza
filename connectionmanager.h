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
    /**
     * @brief Konstruktor klasy ConnectionManager.
     * 
     * Inicjalizuje obiekt klasy ConnectionManager, ustawiając obiekt nadrzędny (QObject).
     * 
     * @param parent Wskaźnik na obiekt nadrzędny (QObject), domyślnie nullptr.
     */
    explicit ConnectionManager(QObject *parent = nullptr);

    /**
     * @brief Sprawdza połączenie internetowe i ładuje dane stacji.
     * 
     * Sprawdza dostępność połączenia internetowego i w zależności od wyniku ładuje dane stacji 
     * z API (jeśli online) lub z lokalnego pliku (jeśli offline). Aktualizuje status połączenia 
     * i listę stacji w interfejsie użytkownika.
     * 
     * @param apiClient Wskaźnik na obiekt ApiClient do wysyłania żądań API.
     * @param stationList Wskaźnik na QListWidget, w którym wyświetlane są nazwy stacji.
     * @param lblStatus Wskaźnik na QLabel wyświetlający status połączenia.
     * @param lblStationCount Wskaźnik na QLabel wyświetlający liczbę stacji.
     * @param isOffline Referencja do flagi wskazującej, czy aplikacja działa w trybie offline.
     * @param allStations Referencja do wektora przechowującego dane wszystkich stacji (nazwa i obiekt JSON).
     */
    void checkConnectionAndReloadStations(ApiClient *apiClient, QListWidget *stationList, QLabel *lblStatus, QLabel *lblStationCount, bool &isOffline, QVector<QPair<QString, QJsonObject>> &allStations);
};

#endif