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
    static void handleMeasurementsData(const QJsonObject &obj, const QVector<QPair<QDateTime, double>> &customData, QLabel *lblStats);
    static void updateChart(const QVector<QPair<QDateTime, double>> &data, QChartView *chartView, const QString &stationCity, const QString &stationAddress, const QString &paramName);
private:
    static QString analyzeTrend(const QVector<QPair<QDateTime, double>> &data);
};

#endif
