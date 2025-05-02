/**
 * @file mainwindow.h
 * @brief Definicja klasy MainWindow dla głównego okna aplikacji.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QJsonArray>
#include <QJsonObject>
#include <QListWidgetItem>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ApiClient;
class ConnectionManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor klasy MainWindow.
     * 
     * Inicjalizuje główne okno aplikacji, konfiguruje interfejs użytkownika, tworzy obiekty `ApiClient` 
     * i `ConnectionManager`, ustawia timery dla zegara i sprawdzania połączenia oraz konfiguruje filtry zdarzeń.
     * 
     * @param parent Wskaźnik na obiekt nadrzędny (QWidget), domyślnie nullptr.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destruktor klasy MainWindow.
     * 
     * Zwalnia zasoby związane z interfejsem użytkownika i obiektami podrzędnymi.
     */
    ~MainWindow();

protected:
    /**
     * @brief Obsługuje zdarzenia naciśnięcia klawisza.
     * 
     * Przetwarza naciśnięcia klawiszy, takich jak Enter, aby wywołać akcje związane z wybranymi elementami 
     * listy stacji, czujników lub przycisków.
     * 
     * @param event Wskaźnik na obiekt zdarzenia klawisza.
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * @brief Filtruje zdarzenia dla obiektów interfejsu użytkownika.
     * 
     * Blokuje określone klawisze (strzałki, spacja) dla przycisków, aby zapewnić poprawną nawigację w interfejsie.
     * 
     * @param obj Obiekt, dla którego filtrowane jest zdarzenie.
     * @param event Wskaźnik na obiekt zdarzenia.
     * @return bool Wartość true, jeśli zdarzenie zostało przetworzone, false w przeciwnym razie.
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

    /**
     * @brief Obsługuje zdarzenie pokazania okna.
     * 
     * Ustawia fokus na polu wyszukiwania stacji po wyświetleniu okna.
     * 
     * @param event Wskaźnik na obiekt zdarzenia pokazania.
     */
    void showEvent(QShowEvent *event) override;

private slots:
    /**
     * @brief Obsługuje dane zwrócone przez ApiClient.
     * 
     * Przetwarza dane JSON (stacje, czujniki lub pomiary), zapisuje je jako dane historyczne 
     * i aktualizuje interfejs użytkownika (listy, wykres, statystyki).
     * 
     * @param data Dane w formacie QString (JSON).
     */
    void onDataReady(const QString &data);

    /**
     * @brief Obsługuje błędy zgłoszone przez ApiClient.
     * 
     * Wyświetla komunikat o błędzie i próbuje wczytać dane lokalne, jeśli są dostępne.
     * 
     * @param error Opis błędu w formacie QString.
     */
    void onErrorOccurred(const QString &error);

    /**
     * @brief Obsługuje kliknięcie elementu listy stacji.
     * 
     * Aktualizuje identyfikator bieżącej stacji, miasto i adres, a następnie wczytuje dane czujników 
     * (online lub offline).
     * 
     * @param item Wskaźnik na kliknięty element listy `QListWidgetItem`.
     */
    void on_stationList_itemClicked(QListWidgetItem *item);

    /**
     * @brief Obsługuje kliknięcie elementu listy czujników.
     * 
     * Aktualizuje identyfikator bieżącego czujnika i nazwę parametru, a następnie wczytuje dane pomiarów 
     * (online lub offline).
     * 
     * @param item Wskaźnik na kliknięty element listy `QListWidgetItem`.
     */
    void on_sensorList_itemClicked(QListWidgetItem *item);

    /**
     * @brief Obsługuje kliknięcie przycisku historii.
     * 
     * Wczytuje i wyświetla wszystkie dane historyczne dla bieżącego czujnika.
     */
    void on_btnHistory_clicked();

    /**
     * @brief Wczytuje historyczne dane pomiarów dla określonego zakresu dni.
     * 
     * Filtruje dane historyczne dla bieżącego czujnika z ostatnich `days` dni i aktualizuje wykres oraz statystyki.
     * 
     * @param days Liczba dni do wczytania (np. 7 lub 14).
     */
    void loadHistoricalData(int days);

    /**
     * @brief Aktualizuje zegar w interfejsie użytkownika.
     * 
     * Wyświetla bieżącą datę i godzinę w formacie "dd.MM.yyyy - hh:mm:ss".
     */
    void updateClock();

private:
    Ui::MainWindow *ui;
    ApiClient *apiClient;
    ConnectionManager *connectionManager;
    QTimer *clockTimer;
    QTimer *connectionCheckTimer;
    QLabel *lblStatus;
    bool isOffline;
    int currentStationId;
    int currentSensorId;
    QString currentStationCity;
    QString currentStationAddress;
    QString currentParamName;
    QList<QPair<QString, QJsonObject>> allStations;
    QVector<QPair<int, QString>> currentSensors;
};

#endif