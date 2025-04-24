#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include "apiclient.h"
#include <QListWidgetItem>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QKeyEvent>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QListWidgetItem>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void showEvent(QShowEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onDataReady(const QString& data);
    void onErrorOccurred(const QString& error);
    void on_stationList_itemClicked(QListWidgetItem *item);
    void on_sensorList_itemClicked(QListWidgetItem *item);
    void updateClock();
    void onConnectionStatusChanged(const QString &status);
    void checkConnectionAndReloadStations();
    void on_btnHistory_clicked();

private:
    Ui::MainWindow *ui;
    ApiClient *apiClient;
    QString currentParamName;

    void handleStationsData(const QJsonArray& array);
    void handleSensorsData(const QJsonArray& array);
    void handleMeasurementsData(const QJsonObject& obj, const QVector<QPair<QDateTime, double>>& customData);
    int currentStationId;
    void updateChart(const QVector<QPair<QDateTime, double>>& data);

    QTimer *clockTimer;
    QVector<QPair<QString, QJsonObject>> allStations;
    void updateStationList();

    QString currentStationCity;
    QString currentStationAddress;
    QVector<QPair<int, QString>> currentSensors;

    QTimer* connectionCheckTimer;
    QLabel *lblStatus;
    QByteArray loadDataFromFile(const QString &fileName, int id = -1);
    void saveDataToFile(const QString &fileName, const QByteArray &data, int id = -1);
    int currentSensorId;

    bool isOffline;
    QString getDataFilePath(const QString &baseFileName, int id = -1);

    void saveHistoricalData(const QString& type, const QByteArray& data, int id = -1);
    QVector<QPair<QDateTime, QByteArray>> loadAllHistoricalData(const QString& type, int id);

    void forceChartUpdate();
    QThread *workerThread;
    QString analyzeTrend(const QVector<QPair<QDateTime, double>>& data);
    void loadHistoricalData(int days);
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif
