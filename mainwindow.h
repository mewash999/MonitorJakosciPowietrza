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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void onDataReady(const QString &data);
    void onErrorOccurred(const QString &error);
    void on_stationList_itemClicked(QListWidgetItem *item);
    void on_sensorList_itemClicked(QListWidgetItem *item);
    void on_btnHistory_clicked();
    void loadHistoricalData(int days);
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
