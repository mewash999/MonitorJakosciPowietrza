#ifndef MEASUREMENTHANDLER_H
#define MEASUREMENTHANDLER_H

#include <QJsonObject>
#include <QLabel>
#include <QtCharts/QChartView>
#include <QDateTime>
#include <QtCharts>
#include <QtCharts>
#include <QJsonArray>
#include <QtCharts>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QFont>
#include <algorithm>
#include <numeric>

class MeasurementHandler
{
public:
    /**
     * @brief Przetwarza dane pomiarowe i aktualizuje statystyki w interfejsie użytkownika.
     * 
     * Analizuje dane pomiarowe z wektora `customData` lub obiektu JSON (`obj`), obliczając statystyki 
     * (minimum, średnia, maksimum, trend) i aktualizuje etykietę `lblStats`.
     * 
     * @param obj Obiekt JSON zawierający dane pomiarowe (używany, jeśli `customData` jest puste).
     * @param customData Wektor par (czas, wartość) z danymi pomiarowymi.
     * @param lblStats Wskaźnik na `QLabel`, w którym wyświetlane są statystyki.
     */
    static void handleMeasurementsData(const QJsonObject &obj, const QVector<QPair<QDateTime, double>> &customData, QLabel *lblStats);

    /**
     * @brief Aktualizuje wykres danych pomiarowych w interfejsie użytkownika.
     * 
     * Tworzy wykres liniowy na podstawie danych pomiarowych, konfiguruje osie i tytuł, a następnie 
     * wyświetla go w `chartView`.
     * 
     * @param data Wektor par (czas, wartość) z danymi pomiarowymi.
     * @param chartView Wskaźnik na `QChartView`, w którym wyświetlany jest wykres.
     * @param stationCity Nazwa miasta stacji.
     * @param stationAddress Adres stacji.
     * @param paramName Nazwa parametru czujnika.
     */
    static void updateChart(const QVector<QPair<QDateTime, double>> &data, QChartView *chartView, const QString &stationCity, const QString &stationAddress, const QString &paramName);

private:
    /**
     * @brief Analizuje trend danych pomiarowych.
     * 
     * Oblicza nachylenie linii regresji liniowej, aby określić trend danych: "STABILNY", 
     * "WZROSTOWY" lub "SPADKOWY".
     * 
     * @param data Wektor par (czas, wartość) z danymi pomiarowymi.
     * @return QString Tekst opisujący trend ("STABILNY", "WZROSTOWY", "SPADKOWY" lub "Brak danych").
     */
    static QString analyzeTrend(const QVector<QPair<QDateTime, double>> &data);
};

#endif