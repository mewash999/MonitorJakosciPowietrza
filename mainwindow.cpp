#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "apiclient.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , apiClient(new ApiClient())
    , currentStationId(-1)
    , currentSensorId(-1)
{
    ui->setupUi(this);
    this->setWindowTitle("Made by Miłosz Kurpisz");
    this->setWindowIcon(QIcon(":/icons/icon.ico"));

    qDebug() << "Main UI - thread:" << QThread::currentThreadId();

    isOffline = true;
    checkConnectionAndReloadStations();

    connect(apiClient, &ApiClient::dataReady, this, &MainWindow::onDataReady);
    connect(apiClient, &ApiClient::errorOccurred, this, &MainWindow::onErrorOccurred);

    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, &MainWindow::updateClock);
    clockTimer->start(100);

    connectionCheckTimer = new QTimer(this);
    connect(connectionCheckTimer, &QTimer::timeout, this, &MainWindow::checkConnectionAndReloadStations);
    connectionCheckTimer->start(5000);

    connect(ui->stationSearch, &QLineEdit::textChanged, this, &MainWindow::updateStationList);
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

void MainWindow::onDataReady(const QString& data) {
    if(isOffline) return;

    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());

    if (doc.isArray() && !doc.array().isEmpty()) {
        if (doc.array().first().toObject().contains("stationName")) {
            saveHistoricalData("stations", data.toUtf8());
            handleStationsData(doc.array());
        } else {
            saveHistoricalData("sensors", data.toUtf8(), currentStationId);
            handleSensorsData(doc.array());
        }
    }
    else if (doc.isObject()) {
        saveHistoricalData("measurements", data.toUtf8(), currentSensorId);
        handleMeasurementsData(doc.object(), QVector<QPair<QDateTime, double>>());
    }
}

void MainWindow::onErrorOccurred(const QString& error) {
    lblStatus->setText("Błąd: " + error);
    lblStatus->setStyleSheet("color: red;");

    if(currentSensorId != -1) {
        on_btnHistory_clicked();
    }
    else if(currentStationId != -1) {
        QByteArray sensorsData = loadDataFromFile("sensors", currentStationId);
        if(!sensorsData.isEmpty()) {
            handleSensorsData(QJsonDocument::fromJson(sensorsData).array());
        }
    }
    else {
        QByteArray stationsData = loadDataFromFile("stations");
        if(!stationsData.isEmpty()) {
            handleStationsData(QJsonDocument::fromJson(stationsData).array());
        }
    }
}

void MainWindow::onConnectionStatusChanged(const QString &status) {
    lblStatus->setText(status);
}

void MainWindow::handleStationsData(const QJsonArray& array) {

    if(array.isEmpty() && !isOffline) {
        apiClient->fetchData(QUrl("https://api.gios.gov.pl/pjp-api/rest/station/findAll"));
        return;
    }

    ui->stationList->clear();
    allStations.clear();

    for (const QJsonValue &value : array) {
        QJsonObject obj = value.toObject();
        QString name = obj["stationName"].toString();
        QString city = obj["city"].toObject()["name"].toString();
        QString address = obj["addressStreet"].toString();
        QString display = city + "  |  " + (!address.isEmpty() ? address : name);
        allStations.append(qMakePair(display, obj));
    }
    updateStationList();
    ui->lblStationCount->setText("Dostępne stacje [ " + QString::number(allStations.size()) + " ]");
}

void MainWindow::updateStationList() {

    ui->stationList->clear();
    QString filter = ui->stationSearch->text().trimmed().toLower();
    QList<QPair<QString, QJsonObject>> filteredStations;
    for (const auto& pair : allStations) {
        QString display = pair.first;
        QJsonObject obj = pair.second;
        if (filter.isEmpty() || display.toLower().contains(filter)) {
            filteredStations.append(qMakePair(display, obj));
        }
    }
    std::sort(filteredStations.begin(), filteredStations.end(), [](const QPair<QString, QJsonObject>& a, const QPair<QString, QJsonObject>& b) {
        return a.first.toLower() < b.first.toLower();
    });
    for (const auto& pair : filteredStations) {
        QListWidgetItem *item = new QListWidgetItem(pair.first);
        item->setData(Qt::UserRole, pair.second["id"].toInt());
        ui->stationList->addItem(item);
    }

    if(ui->stationList->count() > 0) {
        ui->stationList->setCurrentRow(0);
        ui->stationList->item(0)->setSelected(true);
    }

    QString labelText = filter.isEmpty()
                            ? "Dostępne stacje [ " + QString::number(ui->stationList->count()) + " ]"
                            : "Wyszukane stacje [ " + QString::number(ui->stationList->count()) + " ]";
    ui->lblStationCount->setText(labelText);
}

void MainWindow::handleSensorsData(const QJsonArray& array) {

    ui->sensorList->clear();
    currentSensors.clear();
    QVector<QPair<QString, QJsonObject>> sensors;
    for (const QJsonValue &value : array) {
        QJsonObject obj = value.toObject();
        QString paramName = obj["param"].toObject()["paramName"].toString();
        int sensorId = obj["id"].toInt();
        currentSensors.append(qMakePair(sensorId, paramName));
        sensors.append(qMakePair(paramName, obj));
    }
    std::sort(sensors.begin(), sensors.end(), [](const QPair<QString, QJsonObject>& a, const QPair<QString, QJsonObject>& b) {
        return a.first.toLower() < b.first.toLower();
    });
    for (const auto& pair : sensors) {
        QJsonObject obj = pair.second;
        QString paramName = obj["param"].toObject()["paramName"].toString();
        QListWidgetItem *item = new QListWidgetItem(paramName);
        item->setData(Qt::UserRole, obj["id"].toInt());
        ui->sensorList->addItem(item);
    }

    if(ui->sensorList->count() > 0) {
        ui->sensorList->setCurrentRow(0);
        ui->sensorList->item(0)->setSelected(true);
    }
}

void MainWindow::handleMeasurementsData(const QJsonObject& obj, const QVector<QPair<QDateTime, double>>& customData = {}) {

    QVector<QPair<QDateTime, double>> measurements = customData;

    if (!customData.isEmpty()) {
        measurements = customData;
    }

    if (isOffline) {
        QVector<QPair<QDateTime, QByteArray>> allData = loadAllHistoricalData("measurements", currentSensorId);
        for(const auto &dataPair : allData) {
            QJsonDocument doc = QJsonDocument::fromJson(dataPair.second);
            if(doc.isObject()) {
                QJsonArray values = doc.object()["values"].toArray();
                for(const QJsonValue &val : values) {
                    QJsonObject entry = val.toObject();
                    QDateTime date = QDateTime::fromString(entry["date"].toString(), Qt::ISODate);
                    double value = entry["value"].toDouble(-1.0);
                    measurements.append({date, value});
                }
            }
        }
    } else {
        QJsonArray values = obj["values"].toArray();
        for(const QJsonValue &val : values) {
            QJsonObject entry = val.toObject();
            QDateTime date = QDateTime::fromString(entry["date"].toString(), Qt::ISODate);
            double value = entry["value"].toDouble(-1.0);
            measurements.append({date, value});
        }
    }

    std::sort(measurements.begin(), measurements.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });

    QVector<double> validValues;
    QVector<QPair<QDateTime, double>> chartData;
    for (const auto& m : measurements) {
        if (m.second >= 0) {
            validValues.append(m.second);
            chartData.append(m);
        }
    }

    if (!validValues.isEmpty()) {
        double minVal = *std::min_element(validValues.begin(), validValues.end());
        double maxVal = *std::max_element(validValues.begin(), validValues.end());
        double avgVal = std::accumulate(validValues.begin(), validValues.end(), 0.0) / validValues.size();
        QString trend = analyzeTrend(measurements);

        QString output = QString("Minimum: %1\n"
                                 "Średnia: %2\n"
                                 "Maksimum: %3\n"
                                 "\nTrend: %4")
                             .arg(minVal, 0, 'f', 1)
                             .arg(avgVal, 0, 'f', 1)
                             .arg(maxVal, 0, 'f', 1)
                             .arg(trend);
        ui->lblStats->setText(output);
    }

    updateChart(measurements);
}

QString MainWindow::analyzeTrend(const QVector<QPair<QDateTime, double>>& data) {
    if (data.size() < 2) return "Brak danych";

    QVector<double> validValues;
    for (const auto& point : data) {
        if (point.second >= 0) {
            validValues.append(point.second);
        }
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

QVector<QPair<QDateTime, QByteArray>> MainWindow::loadAllHistoricalData(const QString& type, int id) {

    QVector<QPair<QDateTime, QByteArray>> result;
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(path);

    if (type == "measurements") {
        QString filter = QString("%1_%2_*.json").arg(type).arg(id);
        QStringList files = dir.entryList(QStringList() << filter, QDir::Files, QDir::Time);

        foreach (const QString &file, files) {
            QFile f(dir.filePath(file));
            if (f.open(QIODevice::ReadOnly)) {
                QStringList parts = file.split('_');
                if (parts.size() >= 4) {
                    QString timestampStr = parts[2] + "_" + parts[3].left(6);
                    QDateTime dt = QDateTime::fromString(timestampStr, "yyyyMMdd_HHmmss");
                    result.append(qMakePair(dt, f.readAll()));
                }
                f.close();
            }
        }
    }
    return result;
}

double roundToEven(double value) {
    double intPart;
    double fracPart = modf(value, &intPart);
    fracPart = round(fracPart * 50.0) / 50.0;
    if (fracPart > 0.0 && (int(fracPart * 100) % 2 != 0)) {
        fracPart += 0.01;
    }
    return intPart + fracPart;
}

void MainWindow::updateChart(const QVector<QPair<QDateTime, double>>& data) {

    if(data.isEmpty()) {
        return;
    }

    QChart *oldChart = ui->chartView->chart();
    if (oldChart) oldChart->deleteLater();

    QChart *chart = new QChart();
    QLineSeries *series = new QLineSeries();
    series->setPointsVisible(false);
    QPen pen(Qt::darkYellow);
     pen.setWidth(2);
     series->setPen(pen);

    QVector<QPair<QDateTime, double>> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end(),
              [](const QPair<QDateTime, double>& a, const QPair<QDateTime, double>& b) {
                  return a.first < b.first;
              });

    for (const auto &point : sortedData) {
        if (point.second >= 0) {
            series->append(point.first.toMSecsSinceEpoch(), point.second);
        }
    }

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("dd.MM");
    axisX->setTitleText("<span style=\"font-size: 10pt;font-weight:normal;\">[ Data ]</span>");
    axisX->setTickCount(12);
    axisX->setLabelsAngle(0);

    if(!sortedData.isEmpty()) {
        axisX->setRange(
            sortedData.first().first,
            sortedData.last().first
            );
    }

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("<span style=\"font-size: 10pt;font-weight:normal;\">[ μg/m</span>"
                        "<sup style=\"font-size: 12pt;font-weight:normal;\">3</sup>"
                        "<span style=\"font-size: 10pt;font-weight:normal;\"> ]</span");
    axisY->setTickCount(12);

    QVector<double> valuesVec;
    for(const auto& point : sortedData) {
        if(point.second >= 0) {
            valuesVec << point.second;
        }
    }

    if(!valuesVec.isEmpty()) {
        double minValue = *std::min_element(valuesVec.begin(), valuesVec.end());
        double maxValue = *std::max_element(valuesVec.begin(), valuesVec.end());
        double margin = (maxValue - minValue) * 0.05;
        minValue -= margin;
        maxValue += margin;
        if(minValue < 0.0) minValue = 0.0;
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
                        .arg(currentStationCity)
                        .arg(currentStationAddress)
                        .arg(currentParamName));

    chart->legend()->setVisible(false);
    chart->setMargins(QMargins(18, 10, 10, 10));
    ui->chartView->setChart(chart);
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

        QByteArray sensorsData = loadDataFromFile("sensors", currentStationId);

        if (sensorsData.isEmpty()) {
            lblStatus->setText("Brak danych historycznych");
            lblStatus->setStyleSheet("color: red;");

            ui->sensorList->clear();
            currentSensors.clear();
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(sensorsData);
        if (doc.isArray()) {
            handleSensorsData(doc.array());
        }
    } else {
        apiClient->fetchData(QUrl(QString("https://api.gios.gov.pl/pjp-api/rest/station/sensors/%1")
            .arg(currentStationId)));
    }
}

void MainWindow::on_sensorList_itemClicked(QListWidgetItem *item)
{
    ui->lblStats->clear();
    currentSensorId = item->data(Qt::UserRole).toInt();

    auto sensorData = std::find_if(currentSensors.begin(), currentSensors.end(),
                                   [this](const QPair<int, QString>& pair) {
                                       return pair.first == currentSensorId;
                                   });

    if (sensorData != currentSensors.end()) {
        currentParamName = sensorData->second;
    }

    if (isOffline)
    {
        QVector<QPair<QDateTime, QByteArray>> allData = loadAllHistoricalData("measurements", currentSensorId);

        if(allData.isEmpty()) {
            lblStatus->setText("Brak danych historycznych");
            lblStatus->setStyleSheet("color: red;");
            return;
        }

        QMap<QDateTime, double> aggregatedData;
        for(const auto &dataPair : allData) {
            QJsonDocument doc = QJsonDocument::fromJson(dataPair.second);
            if(doc.isObject()) {
                QJsonArray values = doc.object()["values"].toArray();
                for(const QJsonValue &val : values) {
                    QJsonObject entry = val.toObject();
                    QDateTime date = QDateTime::fromString(entry["date"].toString(), Qt::ISODate);
                    double value = entry["value"].toDouble(-1.0);
                    if(value >= 0) {
                        aggregatedData.insert(date, value);
                    }
                }
            }
        }

        if(aggregatedData.isEmpty()) {
            lblStatus->setText("Błędne lokalne dane");
            lblStatus->setStyleSheet("color: red;");
            return;
        }

        QVector<QPair<QDateTime, double>> measurements;
        for(auto it = aggregatedData.begin(); it != aggregatedData.end(); ++it) {
            measurements.append(qMakePair(it.key(), it.value()));
        }

        handleMeasurementsData(QJsonObject());
        updateChart(measurements);
    }
    else
    {
        apiClient->fetchSensorData(currentSensorId);
    }
}

void MainWindow::checkConnectionAndReloadStations()
{
    QNetworkAccessManager* mgr = new QNetworkAccessManager(this);
    QNetworkRequest req(QUrl("http://www.google.com"));
    QNetworkReply* rep = mgr->get(req);

    connect(rep, &QNetworkReply::finished, [this, rep, mgr]() {
        if (rep->error() == QNetworkReply::NoError) {
            isOffline = false;
            ui->lblStatus->setText("Połączono");
            ui->lblStatus->setStyleSheet("color: green;");

            if(ui->stationList->count() == 0) {
                apiClient->fetchData(QUrl("https://api.gios.gov.pl/pjp-api/rest/station/findAll"));
            }
        } else {
            isOffline = true;
            //qDebug() << "Brak połączenia";
            ui->lblStatus->setText("Brak połączenia - dane lokalne");
            ui->lblStatus->setStyleSheet("color: red;");

            if(ui->stationList->count() == 0) {
                QByteArray stationsData = loadDataFromFile("stations");
                if(!stationsData.isEmpty()) {
                    handleStationsData(QJsonDocument::fromJson(stationsData).array());
                }
            }
        }
        rep->deleteLater();
        mgr->deleteLater();
    });
}


QString MainWindow::getDataFilePath(const QString &baseFileName, int id) {
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataDir);
    if (!dir.exists()) dir.mkpath(".");

    QString fileName = (id == -1)
                           ? QString("%1.json").arg(baseFileName)
                           : QString("%1_%2.json").arg(baseFileName).arg(id);

    return dir.filePath(fileName);
}

void MainWindow::saveDataToFile(const QString &baseFileName, const QByteArray &data, int id) {
    QString path = getDataFilePath(baseFileName, id);
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.close();
    }
}

QByteArray MainWindow::loadDataFromFile(const QString &baseFileName, int id) {
    QString path = getDataFilePath(baseFileName, id);
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        return data;
    }
    return QByteArray();
}

void MainWindow::saveHistoricalData(const QString& type, const QByteArray& data, int id) {
    QString fileName;

    if (type == "stations") {
        fileName = "stations.json";
    }
    else if (type == "sensors") {
        fileName = QString("%1_%2.json").arg(type).arg(id);
    }
    else {
        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
        fileName = QString("%1_%2_%3.json").arg(type).arg(id).arg(timestamp);
    }

    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(path);
    if (!dir.exists()) dir.mkpath(".");

    QFile file(dir.filePath(fileName));
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.close();
    }
}

void MainWindow::on_btnHistory_clicked() {

    if(currentSensorId == -1) {
        lblStatus->setText("Proszę wybrać stację oraz czujnik");
        lblStatus->setStyleSheet("color: orange;");
        return;
    }

    QVector<QPair<QDateTime, QByteArray>> allData = loadAllHistoricalData("measurements", currentSensorId);
    QVector<QPair<QDateTime, double>> allMeasurements;

    foreach (const auto &dataPair, allData) {
        QJsonDocument doc = QJsonDocument::fromJson(dataPair.second);
        if(doc.isObject()) {
            QJsonArray values = doc.object()["values"].toArray();
            for(const QJsonValue &val : values) {
                QJsonObject entry = val.toObject();
                QDateTime date = QDateTime::fromString(entry["date"].toString(), Qt::ISODate);
                double value = entry["value"].toDouble(-1.0);
                if(value >= 0) {
                    allMeasurements.append({date, value});
                }
            }
        }
    }

    if(!allMeasurements.isEmpty()) {
        std::sort(allMeasurements.begin(), allMeasurements.end(),
                  [](const auto& a, const auto& b) { return a.first < b.first; });

        handleMeasurementsData(QJsonObject(), allMeasurements);
        updateChart(allMeasurements);
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

    QVector<QPair<QDateTime, QByteArray>> allData = loadAllHistoricalData("measurements", currentSensorId);
    QVector<QPair<QDateTime, double>> filteredData;

    QDateTime cutoffDate = QDateTime::currentDateTime().addDays(-days);

    for (const auto &dataPair : allData) {
        QJsonDocument doc = QJsonDocument::fromJson(dataPair.second);
        if (doc.isObject()) {
            QJsonArray values = doc.object()["values"].toArray();
            for (const QJsonValue &val : values) {
                QJsonObject entry = val.toObject();
                QDateTime date = QDateTime::fromString(entry["date"].toString(), Qt::ISODate);
                if (date >= cutoffDate) {
                    double value = entry["value"].toDouble(-1.0);
                    if (value >= 0) {
                        filteredData.append({date, value});
                    }
                }
            }
        }
    }

    if (!filteredData.isEmpty()) {
        handleMeasurementsData(QJsonObject(), filteredData);
        lblStatus->setText(QString("Wczytano dane historyczne z %1 dni").arg(days));
        lblStatus->setStyleSheet("color: orange;");
    } else {
        lblStatus->setText("Brak danych historycznych");
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

#include <QKeyEvent>
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
        }
        else if (ui->btnLast7Days->hasFocus()) {
            ui->btnLast7Days->click();
        }
        else if (ui->btnLast14Days->hasFocus()) {
            ui->btnLast14Days->click();
        }
        else {
            QMainWindow::keyPressEvent(event);
        }
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right) {
            if (obj == ui->btnLast7Days || obj == ui->btnLast14Days || obj == ui->btnHistory) {
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
