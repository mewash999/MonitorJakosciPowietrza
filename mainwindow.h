/**
 * @file mainwindow.h
 * @brief Definicja klasy MainWindow, głównego okna aplikacji.
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
 
 /**
  * @class MainWindow
  * @brief Klasa reprezentująca główne okno aplikacji Qt.
  */
 class MainWindow : public QMainWindow
 {
     Q_OBJECT
 
 public:
     /**
      * @brief Konstruktor klasy MainWindow.
      * @param parent Wskaźnik na obiekt nadrzędny.
      */
     MainWindow(QWidget *parent = nullptr);
 
     /**
      * @brief Destruktor klasy MainWindow.
      */
     ~MainWindow();
 
 protected:
     /**
      * @brief Obsługuje zdarzenia klawiatury.
      * @param event Wskaźnik na obiekt zdarzenia klawiatury.
      */
     void keyPressEvent(QKeyEvent *event) override;
 
     /**
      * @brief Filtruje zdarzenia dla obiektów interfejsu.
      * @param obj Wskaźnik na obiekt, dla którego filtrowane jest zdarzenie.
      * @param event Wskaźnik na obiekt zdarzenia.
      * @return True, jeśli zdarzenie zostało przetworzone, false w przeciwnym razie.
      */
     bool eventFilter(QObject *obj, QEvent *event) override;
 
     /**
      * @brief Obsługuje zdarzenie pokazania okna.
      * @param event Wskaźnik na obiekt zdarzenia pokazania.
      */
     void showEvent(QShowEvent *event) override;
 
 private slots:
     /**
      * @brief Obsługuje gotowe dane z API.
      * @param data Otrzymane dane w formacie QString.
      */
     void onDataReady(const QString &data);
 
     /**
      * @brief Obsługuje błędy API.
      * @param error Opis błędu.
      */
     void onErrorOccurred(const QString &error);
 
     /**
      * @brief Obsługuje kliknięcie elementu listy stacji.
      * @param item Wskaźnik na kliknięty element listy.
      */
     void on_stationList_itemClicked(QListWidgetItem *item);
 
     /**
      * @brief Obsługuje kliknięcie elementu listy czujników.
      * @param item Wskaźnik na kliknięty element listy.
      */
     void on_sensorList_itemClicked(QListWidgetItem *item);
 
     /**
      * @brief Obsługuje kliknięcie przycisku historii.
      */
     void on_btnHistory_clicked();
 
     /**
      * @brief Wczytuje dane historyczne dla określonej liczby dni.
      * @param days Liczba dni do wczytania danych.
      */
     void loadHistoricalData(int days);
 
     /**
      * @brief Aktualizuje zegar w interfejsie.
      */
     void updateClock();
 
 private:
     Ui::MainWindow *ui; ///< Wskaźnik na obiekt interfejsu użytkownika.
     ApiClient *apiClient; ///< Wskaźnik na obiekt klienta API.
     ConnectionManager *connectionManager; ///< Wskaźnik na menedżera połączeń.
     QTimer *clockTimer; ///< Timer do aktualizacji zegara.
     QTimer *connectionCheckTimer; ///< Timer do sprawdzania połączenia.
     QLabel *lblStatus; ///< Etykieta statusu połączenia.
     bool isOffline; ///< Flaga wskazująca brak połączenia.
     int currentStationId; ///< Identyfikator aktualnej stacji.
     int currentSensorId; ///< Identyfikator aktualnego czujnika.
     QString currentStationCity; ///< Nazwa miasta aktualnej stacji.
     QString currentStationAddress; ///< Adres aktualnej stacji.
     QString currentParamName; ///< Nazwa aktualnego parametru.
     QList<QPair<QString, QJsonObject>> allStations; ///< Lista wszystkich stacji.
     QVector<QPair<int, QString>> currentSensors; ///< Lista aktualnych czujników.
 };
 
 #endif