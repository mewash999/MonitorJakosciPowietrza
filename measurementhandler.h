/**
 * @file measurementhandler.h
 * @brief Definicja klasy MeasurementHandler do obsługi danych pomiarowych.
 */

 #ifndef MEASUREMENTHANDLER_H
 #define MEASUREMENTHANDLER_H
 
 #include <QJsonObject>
 #include <QLabel>
 #include <QtCharts/QChartView>
 #include <QDateTime>
 #include <QtCharts>
 #include <QJsonArray>
 #include <QChart>
 #include <QLineSeries>
 #include <QDateTimeAxis>
 #include <QValueAxis>
 #include <QFont>
 #include <algorithm>
 #include <numeric>
 
 /**
  * @class MeasurementHandler
  * @brief Klasa statyczna do przetwarzania i wizualizacji danych pomiarowych.
  */
 class MeasurementHandler
 {
 public:
     /**
      * @brief Przetwarza dane pomiarowe i aktualizuje statystyki.
      * @param obj Obiekt JSON z danymi pomiarowymi.
      * @param customData Wektor niestandardowych danych pomiarowych.
      * @param lblStats Wskaźnik na etykietę statystyk.
      */
     static void handleMeasurementsData(const QJsonObject &obj, const QVector<QPair<QDateTime, double>> &customData, QLabel *lblStats);
 
     /**
      * @brief Aktualizuje wykres na podstawie danych pomiarowych.
      * @param data Wektor danych pomiarowych (czas, wartość).
      * @param chartView Wskaźnik na widok wykresu.
      * @param stationCity Nazwa miasta stacji.
      * @param stationAddress Adres stacji.
      * @param paramName Nazwa parametru.
      */
     static void updateChart(const QVector<QPair<QDateTime, double>> &data, QChartView *chartView, const QString &stationCity, const QString &stationAddress, const QString &paramName);
 
 private:
     /**
      * @brief Analizuje trend danych pomiarowych.
      * @param data Wektor danych pomiarowych (czas, wartość).
      * @return Tekstowy opis trendu ("WZROSTOWY", "SPADKOWY", "STABILNY").
      */
     static QString analyzeTrend(const QVector<QPair<QDateTime, double>> &data);
 };
 
 #endif