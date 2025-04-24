#include "stationhandler.h"

    void StationHandler::handleStationsData(const QJsonArray &array, QListWidget *stationList, QLabel *lblStationCount, QList<QPair<QString, QJsonObject>> &allStations) {
    if (array.isEmpty()) {
        return;
    }

    stationList->clear();
    allStations.clear();

    for (const QJsonValue &value : array) {
        QJsonObject obj = value.toObject();
        QString name = obj["stationName"].toString();
        QString city = obj["city"].toObject()["name"].toString();
        QString address = obj["addressStreet"].toString();
        QString display = city + "  |  " + (!address.isEmpty() ? address : name);
        allStations.append(qMakePair(display, obj));
    }
    updateStationList("", stationList, lblStationCount, allStations);
}

void StationHandler::updateStationList(const QString &filter, QListWidget *stationList, QLabel *lblStationCount, const QList<QPair<QString, QJsonObject>> &allStations) {
    stationList->clear();
    QList<QPair<QString, QJsonObject>> filteredStations;
    for (const auto &pair : allStations) {
        if (filter.isEmpty() || pair.first.toLower().contains(filter.toLower())) {
            filteredStations.append(pair);
        }
    }

    std::sort(filteredStations.begin(), filteredStations.end(), [](const auto &a, const auto &b) {
        return a.first.toLower() < b.first.toLower();
    });

    for (const auto &pair : filteredStations) {
        QListWidgetItem *item = new QListWidgetItem(pair.first);
        item->setData(Qt::UserRole, pair.second["id"].toInt());
        stationList->addItem(item);
    }

    if (stationList->count() > 0) {
        stationList->setCurrentRow(0);
        stationList->item(0)->setSelected(true);
    }

    QString labelText = filter.isEmpty()
                            ? "Dostępne stacje [ " + QString::number(stationList->count()) + " ]"
                            : "Wyszukane stacje [ " + QString::number(stationList->count()) + " ]";
    lblStationCount->setText(labelText);
}
