/**
 * @file measurementhandler.cpp
 * @brief Implementacja klasy MeasurementHandler do obsługi danych pomiarowych i wizualizacji wykresów.
 */

#include "measurementhandler.h"

/**
 * @brief Przetwarza dane pomiarowe i aktualizuje statystyki w interfejsie użytkownika.
 * 
 * Przetwarza dane pomiarowe z wektora `customData` lub z obiektu JSON (`obj`), tworząc posortowaną listę 
 * pomiarów (czas, wartość). Oblicza minimalną, maksymalną i średnią wartość oraz trend danych, 
 * a następnie aktualizuje etykietę `lblStats` z wynikami w formacie tekstowym.
 * 
 * @param obj Obiekt JSON zawierający dane pomiarowe (używany, jeśli `customData` jest puste).
 * @param customData Wektor par (czas, wartość) z danymi pomiarowymi.
 * @param lblStats Wskaźnik na `QLabel`, w którym wyświetlane są statystyki.
 */
void MeasurementHandler::handleMeasurementsData(const QJsonObject &obj, const QVector<QPair<QDateTime, double>> &customData, QLabel *lblStats) {
    QVector<QPair<QDateTime, double>> measurements = customData;

    if (measurements.isEmpty()) {
        QJsonArray values = obj["values"].toArray();
        for (const QJsonValue &val : values) {
            QJsonObject entry = val.toObject();
            QDateTime date = QDateTime::fromString(entry["date"].toString(), Qt::ISODate);
            double value = entry["value"].toDouble(-1.0);
            if (value >= 0) measurements.append({date, value});
        }
    }

    std::sort(measurements.begin(), measurements.end(), [](const auto &a, const auto &b) { return a.first < b.first; });

    QVector<double> validValues;
    for (const auto &m : measurements) {
        if (m.second >= 0) validValues.append(m.second);
    }

    if (!validValues.isEmpty()) {
        double minVal = *std::min_element(validValues.begin(), validValues.end());
        double maxVal = *std::max_element(validValues.begin(), validValues.end());
        double avgVal = std::accumulate(validValues.begin(), validValues.end(), 0.0) / validValues.size();
        QString trend = analyzeTrend(measurements);

        QString output = QString("Minimum: %1\nŚrednia: %2\nMaksimum: %3\n\nTrend: %4")
                             .arg(minVal, 0, 'f', 1)
                             .arg(avgVal, 0, 'f', 1)
                             .arg(maxVal, 0, 'f', 1)
                             .arg(trend);
        lblStats->setText(output);
    }
}

/**
 * @brief Analizuje trend danych pomiarowych.
 * 
 * Oblicza nachylenie linii regresji liniowej dla danych pomiarowych, aby określić trend: 
 * "STABILNY" (nachylenie bliskie 0), "WZROSTOWY" (nachylenie dodatnie) lub "SPADKOWY" (nachylenie ujemne). 
 * Zwraca "Brak danych", jeśli danych jest mniej niż 2.
 * 
 * @param data Wektor par (czas, wartość) z danymi pomiarowymi.
 * @return QString Tekst opisujący trend ("STABILNY", "WZROSTOWY", "SPADKOWY" lub "Brak danych").
 */
QString MeasurementHandler::analyzeTrend(const QVector<QPair<QDateTime, double>> &data) {
    if (data.size() < 2) return "Brak danych";

    QVector<double> validValues;
    for (const auto &point : data) {
        if (point.second >= 0) validValues.append(point.second);
    }

    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    int n = validValues.size();

    for (int i = 0; i < n; ++i) {
        sumX += i;
        sumY += validValues[i];
        sumXY += i * validValues[i];
        sumX2 += i * i;
    }

    double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);

    if (abs(slope) < 0.1) return "STABILNY";
    else if (slope > 0) return "WZROSTOWY";
    else return "SPADKOWY";
}

/**
 * @brief Aktualizuje wykres danych pomiarowych w interfejsie użytkownika.
 * 
 * Tworzy nowy wykres liniowy na podstawie danych pomiarowych, konfiguruje osie czasu (X) i wartości (Y), 
 * ustala zakresy osi z marginesem oraz formatuje tytuł wykresu z informacjami o stacji i parametrze. 
 * Usuwa poprzedni wykres, jeśli istnieje, i ustawia nowy w `chartView`.
 * 
 * @param data Wektor par (czas, wartość) z danymi pomiarowymi.
 * @param chartView Wskaźnik na `QChartView`, w którym wyświetlany jest wykres.
 * @param stationCity Nazwa miasta stacji.
 * @param stationAddress Adres stacji.
 * @param paramName Nazwa parametru czujnika.
 * @note Funkcja kończy działanie, jeśli dane są puste.
 */
void MeasurementHandler::updateChart(const QVector<QPair<QDateTime, double>> &data, QChartView *chartView, const QString &stationCity, const QString &stationAddress, const QString &paramName) {
    if (data.isEmpty()) return;

    QChart *oldChart = chartView->chart();
    if (oldChart) oldChart->deleteLater();

    QChart *chart = new QChart();
    QLineSeries *series = new QLineSeries();
    series->setPointsVisible(false);
    series->setPointLabelsVisible(false);
    QPen pen(Qt::darkYellow);
    pen.setWidth(2);
    series->setPen(pen);

    for (const auto &point : data) {
        if (point.second >= 0) {
            series->append(point.first.toMSecsSinceEpoch(), point.second);
        }
    }

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("dd.MM");
    axisX->setTitleText("<span style=\"font-size: 10pt;font-weight:normal;\">[ Data ]</span>");
    axisX->setTickCount(12);
    axisX->setLabelsAngle(0);

    if (!data.isEmpty()) {
        axisX->setRange(data.first().first, data.last().first);
    }

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("<span style=\"font-size: 10pt;font-weight:normal;\">[ μg/m</span>"
                        "<sup style=\"font-size: 12pt;font-weight:normal;\">3</sup>"
                        "<span style=\"font-size: 10pt;font-weight:normal;\"> ]</span>");
    axisY->setTickCount(12);

    QVector<double> valuesVec;
    for (const auto &point : data) {
        if (point.second >= 0) {
            valuesVec << point.second;
        }
    }

    if (!valuesVec.isEmpty()) {
        double minValue = *std::min_element(valuesVec.begin(), valuesVec.end());
        double maxValue = *std::max_element(valuesVec.begin(), valuesVec.end());
        double margin = (maxValue - minValue) * 0.05;
        minValue -= margin;
        maxValue += margin;
        if (minValue < 0.0) minValue = 0.0;
        minValue = floor(minValue / 2.0) * 2.0;
        maxValue = ceil(maxValue / 2.0) * 2.0;
        axisY->setMin(minValue);
        axisY->setMax(maxValue);
    }

    QFont titleFont;
    titleFont.setFamily("MS UI Gothic");
    titleFont.setPointSize(10);
    titleFont.setBold(false);

    chart->setTitleFont(titleFont);
    axisX->setTitleFont(titleFont);
    axisX->setLabelsFont(titleFont);
    axisY->setTitleFont(titleFont);
    axisY->setLabelsFont(titleFont);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    chart->addSeries(series);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chart->setTitle(QString("Stacja: %1 %2  |  Czujnik: %3")
                        .arg(stationCity)
                        .arg(stationAddress)
                        .arg(paramName));

    chart->legend()->setVisible(false);
    chart->setMargins(QMargins(18, 10, 10, 10));
    chartView->setChart(chart);
}