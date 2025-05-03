/**
 * @file datamanager.cpp
 * @brief Implementacja klasy DataManager do zarządzania danymi aplikacji.
 */

#include "datamanager.h"

/**
 * @brief Generuje ścieżkę do pliku danych na podstawie nazwy i identyfikatora.
 * 
 * Tworzy pełną ścieżkę do pliku w katalogu danych aplikacji, używając nazwy bazowej i opcjonalnego 
 * identyfikatora. Jeśli katalog nie istnieje, tworzy go. Nazwa pliku ma format "<baseFileName>.json" 
 * lub "<baseFileName>_<id>.json".
 * 
 * @param baseFileName Bazowa nazwa pliku (np. "stations").
 * @param id Identyfikator, domyślnie -1 (brak identyfikatora w nazwie pliku).
 * @return QString Ścieżka do pliku w formacie JSON.
 */
QString DataManager::getDataFilePath(const QString &baseFileName, int id) {
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataDir);
    if (!dir.exists()) dir.mkpath(".");

    QString fileName = (id == -1)
                           ? QString("%1.json").arg(baseFileName)
                           : QString("%1_%2.json").arg(baseFileName).arg(id);

    return dir.filePath(fileName);
}

/**
 * @brief Zapisuje dane do pliku JSON.
 * 
 * Otwiera plik w trybie zapisu i zapisuje podane dane (QByteArray) w katalogu danych aplikacji. 
 * Ścieżka pliku jest generowana za pomocą getDataFilePath.
 * 
 * @param baseFileName Bazowa nazwa pliku (np. "stations").
 * @param data Dane do zapisania w formacie QByteArray.
 * @param id Identyfikator, domyślnie -1 (brak identyfikatora w nazwie pliku).
 * @note Jeśli plik nie może zostać otwarty, dane nie zostaną zapisane.
 */
void DataManager::saveDataToFile(const QString &baseFileName, const QByteArray &data, int id) {
    QString path = getDataFilePath(baseFileName, id);
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.close();
    }
}

/**
 * @brief Wczytuje dane z pliku JSON.
 * 
 * Otwiera plik w trybie odczytu i zwraca jego zawartość jako QByteArray. Jeśli plik nie istnieje 
 * lub nie można go otworzyć, zwraca pusty QByteArray.
 * 
 * @param baseFileName Bazowa nazwa pliku (np. "stations").
 * @param id Identyfikator, domyślnie -1 (brak identyfikatora w nazwie pliku).
 * @return QByteArray Zawartość pliku lub pusty QByteArray w przypadku błędu.
 */
QByteArray DataManager::loadDataFromFile(const QString &baseFileName, int id) {
    QString path = getDataFilePath(baseFileName, id);
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        return data;
    }
    return QByteArray();
}

/**
 * @brief Zapisuje historyczne dane do pliku JSON.
 * 
 * Zapisuje dane historyczne do pliku w katalogu danych aplikacji. Nazwa pliku zależy od typu danych:
 * - Dla "stations": "stations.json".
 * - Dla "sensors": "<type>_<id>.json".
 * - Dla "measurements": "<type>_<id>_<timestamp>.json" z bieżącym znacznikiem czasu.
 * Jeśli katalog nie istnieje, tworzy go.
 * 
 * @param type Typ danych ("stations", "sensors", "measurements").
 * @param data Dane do zapisania w formacie QByteArray.
 * @param id Identyfikator, domyślnie -1 (używany dla czujników i pomiarów).
 * @note Jeśli plik nie może zostać otwarty, dane nie zostaną zapisane.
 */
void DataManager::saveHistoricalData(const QString &type, const QByteArray &data, int id) {
    QString fileName;

    if (type == "stations") {
        fileName = "stations.json";
    } else if (type == "sensors") {
        fileName = QString("%1_%2.json").arg(type).arg(id);
    } else {
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

/**
 * @brief Wczytuje wszystkie historyczne dane pomiarów dla danego identyfikatora.
 * 
 * Wyszukuje pliki z danymi historycznymi dla typu "measurements" i podanego identyfikatora w formacie 
 * "<type>_<id>_<timestamp>.json". Wczytuje zawartość każdego pliku i parsuje znacznik czasu z nazwy pliku, 
 * zwracając wektor par (czas, dane).
 * 
 * @param type Typ danych ("measurements").
 * @param id Identyfikator czujnika, dla którego wczytywane są dane.
 * @return QVector<QPair<QDateTime, QByteArray>> Wektor par zawierających czas i dane.
 * @note Funkcja przetwarza tylko pliki zgodne z wzorcem nazwy dla typu "measurements".
 */
QVector<QPair<QDateTime, QByteArray>> DataManager::loadAllHistoricalData(const QString &type, int id) {
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