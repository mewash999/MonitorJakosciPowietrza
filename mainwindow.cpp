#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "apiclient.h"
#include "connectionmanager.h"
#include "stationhandler.h"
#include "sensorhandler.h"
#include "measurementhandler.h"
#include "datamanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , apiClient(new ApiClient())
    , connectionManager(new ConnectionManager(this))
    , currentStationId(-1)
    , currentSensorId(-1)
{
    ui->setupUi(this);
    this->setWindowTitle("Made by Miłosz Kurpisz");
    this->setWindowIcon(QIcon(":/icons/icon.ico"));

    qDebug() << "Main UI - thread:" << QThread::currentThreadId();

    isOffline = true;
    connectionManager->checkConnectionAndReloadStations(apiClient, ui->stationList, ui->lblStatus, ui->lblStationCount, isOffline, allStations);;

    connect(apiClient, &ApiClient::dataReady, this, &MainWindow::onDataReady);
    connect(apiClient, &ApiClient::errorOccurred, this, &MainWindow::onErrorOccurred);

    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, &MainWindow::updateClock);
    clockTimer->start(100);

    connectionCheckTimer = new QTimer(this);
    connect(connectionCheckTimer, &QTimer::timeout, [=]() {
        connectionManager->checkConnectionAndReloadStations(apiClient, ui->stationList, ui->lblStatus, ui->lblStationCount, isOffline, allStations);
    });
    connectionCheckTimer->start(10000);

    connect(ui->stationSearch, &QLineEdit::textChanged, [=](const QString &text) {
        StationHandler::updateStationList(text, ui->stationList, ui->lblStationCount, allStations);
    });
    connect(ui->btnHistory, &QPushButton::clicked, this, &MainWindow::on_btnHistory_clicked);
    connect(ui->btnLast7Days, &QPushButton::clicked, [this]() { loadHistoricalData(7); });
    connect(ui->btnLast14Days, &QPushButton::clicked, [this]() { loadHistoricalData(14); });

    setTabOrder(ui->stationSearch, ui->stationList);
    setTabOrder(ui->stationList, ui->sensorList);
    setTabOrder(ui->sensorList, ui->btnLast7Days);
    setTabOrder(ui->btnLast7Days, ui->btnLast14Days);
    setTabOrder(ui->btnLast14Days, ui->btnHistory);
    setTabOrder(ui->btnHistory, ui->stationSearch);
    ui->lblStats->setFocusPolicy(Qt::NoFocus);
    ui->chartView->setFocusPolicy(Qt::NoFocus);
    ui->lblTime->setFocusPolicy(Qt::NoFocus);
    ui->stationSearch->setFocus();
    ui->btnLast7Days->installEventFilter(this);
    ui->btnLast14Days->installEventFilter(this);
    ui->btnHistory->installEventFilter(this);
    lblStatus = ui->lblStatus;
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onDataReady(const QString &data) {
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    if (doc.isArray()) {
        QJsonArray array = doc.array();
        if (!array.isEmpty() && array.first().toObject().contains("stationName")) {
            DataManager::saveHistoricalData("stations", data.toUtf8());
            StationHandler::handleStationsData(array, ui->stationList, ui->lblStationCount, allStations);
        } else {
            DataManager::saveHistoricalData("sensors", data.toUtf8(), currentStationId);
            SensorHandler::handleSensorsData(array, ui->sensorList, currentSensors);
        }
    } else if (doc.isObject()) {
        DataManager::saveHistoricalData("measurements", data.toUtf8(), currentSensorId);

        QJsonObject obj = doc.object();
        QVector<QPair<QDateTime, double>> measurements;
        QJsonArray values = obj["values"].toArray();
        for (const QJsonValue &val : values) {
            QJsonObject entry = val.toObject();
            QDateTime date = QDateTime::fromString(entry["date"].toString(), Qt::ISODate);
            double value = entry["value"].toDouble(-1.0);
            if (value >= 0 && date.isValid()) {
                measurements.append({date, value});
            }
        }
        if (!measurements.isEmpty()) {
            MeasurementHandler::handleMeasurementsData(obj, measurements, ui->lblStats);
            MeasurementHandler::updateChart(measurements, ui->chartView, currentStationCity, currentStationAddress, currentParamName);
            ui->lblStatus->setText("Wczytano dane online");
            ui->lblStatus->setStyleSheet("color: green;");
        } else {
            ui->lblStatus->setText("Błąd danych online");
            ui->lblStatus->setStyleSheet("color: red;");
        }
    }
}

void MainWindow::onErrorOccurred(const QString &error) {
    lblStatus->setText("Błąd: " + error);
    lblStatus->setStyleSheet("color: red;");

    if (currentSensorId != -1) {
        on_btnHistory_clicked();
    } else if (currentStationId != -1) {
        QByteArray sensorsData = DataManager::loadDataFromFile("sensors", currentStationId);
        if (!sensorsData.isEmpty()) {
            onDataReady(QString::fromUtf8(sensorsData));
        }
    } else {
        QByteArray stationsData = DataManager::loadDataFromFile("stations");
        if (!stationsData.isEmpty()) {
            onDataReady(QString::fromUtf8(stationsData));
        }
    }
}

void MainWindow::on_stationList_itemClicked(QListWidgetItem *item) {
    ui->lblStats->clear();
    currentStationId = item->data(Qt::UserRole).toInt();

    auto stationData = std::find_if(allStations.begin(), allStations.end(),
                                    [this](const QPair<QString, QJsonObject>& pair) {
                                        return pair.second["id"].toInt() == currentStationId;
                                    });

    if (stationData != allStations.end()) {
        currentStationCity = stationData->second["city"].toObject()["name"].toString();
        currentStationAddress = stationData->second["addressStreet"].toString();
    }

    if (isOffline) {
        QByteArray sensorsData = DataManager::loadDataFromFile("sensors", currentStationId);
        if (sensorsData.isEmpty()) {
            lblStatus->setText("Brak danych historycznych");
            lblStatus->setStyleSheet("color: red;");
            ui->sensorList->clear();
            currentSensors.clear();
            return;
        }
        QJsonDocument doc = QJsonDocument::fromJson(sensorsData);
        if (doc.isArray()) {
            onDataReady(QString::fromUtf8(sensorsData));
        }
    } else {
        apiClient->fetchData(QUrl(QString("https://api.gios.gov.pl/pjp-api/rest/station/sensors/%1")
                                      .arg(currentStationId)));
    }
}

void MainWindow::on_sensorList_itemClicked(QListWidgetItem *item) {
    ui->lblStats->clear();
    currentSensorId = item->data(Qt::UserRole).toInt();

    auto sensorData = std::find_if(currentSensors.begin(), currentSensors.end(),
                                   [this](const QPair<int, QString>& pair) {
                                       return pair.first == currentSensorId;
                                   });

    if (sensorData != currentSensors.end()) {
        currentParamName = sensorData->second;
    }

    if (isOffline) {
        QVector<QPair<QDateTime, QByteArray>> allData = DataManager::loadAllHistoricalData("measurements", currentSensorId);
        if (allData.isEmpty()) {
            lblStatus->setText("Brak danych historycznych");
            lblStatus->setStyleSheet("color: red;");
            return;
        }

        QMap<QDateTime, double> aggregatedData;
        for (const auto &dataPair : allData) {
            QJsonDocument doc = QJsonDocument::fromJson(dataPair.second);
            if (doc.isObject()) {
                QJsonArray values = doc.object()["values"].toArray();
                for (const QJsonValue &val : values) {
                    QJsonObject entry = val.toObject();
                    QDateTime date = QDateTime::fromString(entry["date"].toString(), Qt::ISODate);
                    double value = entry["value"].toDouble(-1.0);
                    if (value >= 0) {
                        aggregatedData.insert(date, value);
                    }
                }
            }
        }

        if (aggregatedData.isEmpty()) {
            lblStatus->setText("Błędne dane lokalne");
            lblStatus->setStyleSheet("color: red;");
            return;
        }

        QVector<QPair<QDateTime, double>> measurements;
        for (auto it = aggregatedData.begin(); it != aggregatedData.end(); ++it) {
            measurements.append(qMakePair(it.key(), it.value()));
        }

        MeasurementHandler::handleMeasurementsData(QJsonObject(), measurements, ui->lblStats);
        MeasurementHandler::updateChart(measurements, ui->chartView, currentStationCity, currentStationAddress, currentParamName);
    } else {
        apiClient->fetchSensorData(currentSensorId);
    }
}

void MainWindow::on_btnHistory_clicked() {
    if (currentSensorId == -1) {
        lblStatus->setText("Proszę wybrać stację oraz czujnik");
        lblStatus->setStyleSheet("color: orange;");
        return;
    }

    QVector<QPair<QDateTime, QByteArray>> allData = DataManager::loadAllHistoricalData("measurements", currentSensorId);
    QMap<QDateTime, double> aggregatedData;

    for (const auto &dataPair : allData) {
        QJsonDocument doc = QJsonDocument::fromJson(dataPair.second);
        if (doc.isObject()) {
            QJsonArray values = doc.object()["values"].toArray();
            for (const QJsonValue &val : values) {
                QJsonObject entry = val.toObject();
                QDateTime date = QDateTime::fromString(entry["date"].toString(), Qt::ISODate);
                double value = entry["value"].toDouble(-1.0);
                if (value >= 0 && date.isValid()) {
                    aggregatedData.insert(date, value);
                }
            }
        }
    }

    QVector<QPair<QDateTime, double>> allMeasurements;
    for (auto it = aggregatedData.constBegin(); it != aggregatedData.constEnd(); ++it) {
        allMeasurements.append(qMakePair(it.key(), it.value()));
    }

    std::sort(allMeasurements.begin(), allMeasurements.end(),
              [](const auto &a, const auto &b) { return a.first < b.first; });

    if (!allMeasurements.isEmpty()) {
        MeasurementHandler::handleMeasurementsData(QJsonObject(), allMeasurements, ui->lblStats);
        MeasurementHandler::updateChart(allMeasurements, ui->chartView, currentStationCity, currentStationAddress, currentParamName);
        lblStatus->setText("Wczytano wszystkie dane historyczne");
        lblStatus->setStyleSheet("color: orange;");
    } else {
        lblStatus->setText("Brak danych historycznych");
        lblStatus->setStyleSheet("color: red;");
    }
}

void MainWindow::loadHistoricalData(int days) {
    if (currentSensorId == -1) {
        lblStatus->setText("Proszę wybrać stację oraz czujnik");
        lblStatus->setStyleSheet("color: orange;");
        return;
    }

    QVector<QPair<QDateTime, QByteArray>> allData = DataManager::loadAllHistoricalData("measurements", currentSensorId);
    QMap<QDateTime, double> aggregatedData;

    QDateTime cutoffDate = QDateTime::currentDateTime().addDays(-days);

    for (const auto &dataPair : allData) {
        QJsonDocument doc = QJsonDocument::fromJson(dataPair.second);
        if (doc.isObject()) {
            QJsonArray values = doc.object()["values"].toArray();
            for (const QJsonValue &val : values) {
                QJsonObject entry = val.toObject();
                QDateTime date = QDateTime::fromString(entry["date"].toString(), Qt::ISODate);
                if (date.isValid() && date >= cutoffDate) {
                    double value = entry["value"].toDouble(-1.0);
                    if (value >= 0) {
                        aggregatedData.insert(date, value);
                    }
                }
            }
        }
    }

    QVector<QPair<QDateTime, double>> filteredData;
    for (auto it = aggregatedData.constBegin(); it != aggregatedData.constEnd(); ++it) {
        filteredData.append(qMakePair(it.key(), it.value()));
    }

    std::sort(filteredData.begin(), filteredData.end(),
              [](const auto &a, const auto &b) { return a.first < b.first; });

    if (!filteredData.isEmpty()) {
        MeasurementHandler::handleMeasurementsData(QJsonObject(), filteredData, ui->lblStats);
        MeasurementHandler::updateChart(filteredData, ui->chartView, currentStationCity, currentStationAddress, currentParamName);
        lblStatus->setText(QString("Wczytano dane historyczne z %1 dni").arg(days));
        lblStatus->setStyleSheet("color: orange;");
    } else {
        lblStatus->setText(QString("Brak danych historycznych z %1 dni").arg(days));
        lblStatus->setStyleSheet("color: red;");
    }
}

void MainWindow::updateClock() {
    QString currentTime = QDateTime::currentDateTime().toString("dd.MM.yyyy - hh:mm:ss");
    ui->lblTime->setText(currentTime);
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    ui->stationSearch->setFocus();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (ui->stationList->hasFocus()) {
            QListWidgetItem *currentItem = ui->stationList->currentItem();
            if (currentItem) {
                on_stationList_itemClicked(currentItem);
            }
        } else if (ui->sensorList->hasFocus()) {
            QListWidgetItem *currentItem = ui->sensorList->currentItem();
            if (currentItem) {
                on_sensorList_itemClicked(currentItem);
            }
        } else if (ui->btnHistory->hasFocus()) {
            on_btnHistory_clicked();
        } else if (ui->btnLast7Days->hasFocus()) {
            ui->btnLast7Days->click();
        } else if (ui->btnLast14Days->hasFocus()) {
            ui->btnLast14Days->click();
        } else {
            QMainWindow::keyPressEvent(event);
        }
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right || keyEvent->key() == Qt::Key_Space) {
            if (obj == ui->btnLast7Days || obj == ui->btnLast14Days || obj == ui->btnHistory) {
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
