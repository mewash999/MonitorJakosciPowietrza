/**
 * @file datamanager.h
 * @brief Definicja klasy DataManager do zarządzania danymi aplikacji.
 */

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include <QByteArray>
#include <QVector>
#include <QPair>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonArray>

class DataManager
{
public:
    /**
     * @brief Generuje ścieżkę do pliku danych na podstawie nazwy i identyfikatora.
     * 
     * Tworzy pełną ścieżkę do pliku w katalogu danych aplikacji, uwzględniając nazwę bazową 
     * oraz opcjonalny identyfikator. Jeśli katalog nie istnieje, zostanie utworzony.
     * 
     * @param baseFileName Bazowa nazwa pliku (np. "stations").
     * @param id Identyfikator, domyślnie -1 (brak identyfikatora w nazwie pliku).
     * @return QString Ścieżka do pliku w formacie JSON.
     */
    static QString getDataFilePath(const QString &baseFileName, int id = -1);

    /**
     * @brief Zapisuje dane do pliku JSON.
     * 
     * Zapisuje podane dane (QByteArray) do pliku w katalogu danych aplikacji. Ścieżka pliku 
     * jest generowana na podstawie nazwy bazowej i opcjonalnego identyfikatora.
     * 
     * @param baseFileName Bazowa nazwa pliku (np. "stations").
     * @param data Dane do zapisania w formacie QByteArray.
     * @param id Identyfikator, domyślnie -1 (brak identyfikatora w nazwie pliku).
     */
    static void saveDataToFile(const QString &baseFileName, const QByteArray &data, int id = -1);

    /**
     * @brief Wczytuje dane z pliku JSON.
     * 
     * Wczytuje zawartość pliku z katalogu danych aplikacji na podstawie nazwy bazowej 
     * i opcjonalnego identyfikatora. Zwraca pusty QByteArray, jeśli plik nie istnieje.
     * 
     * @param baseFileName Bazowa nazwa pliku (np. "stations").
     * @param id Identyfikator, domyślnie -1 (brak identyfikatora w nazwie pliku).
     * @return QByteArray Zawartość pliku lub pusty QByteArray w przypadku błędu.
     */
    static QByteArray loadDataFromFile(const QString &baseFileName, int id = -1);

    /**
     * @brief Zapisuje historyczne dane do pliku JSON.
     * 
     * Zapisuje dane historyczne (np. pomiary, stacje, czujniki) do pliku w katalogu danych aplikacji. 
     * Nazwa pliku zależy od typu danych i zawiera znacznik czasu dla pomiarów.
     * 
     * @param type Typ danych ("stations", "sensors", "measurements").
     * @param data Dane do zapisania w formacie QByteArray.
     * @param id Identyfikator, domyślnie -1 (używany dla czujników i pomiarów).
     */
    static void saveHistoricalData(const QString &type, const QByteArray &data, int id = -1);

    /**
     * @brief Wczytuje wszystkie historyczne dane pomiarów dla danego identyfikatora.
     * 
     * Wyszukuje pliki z danymi historycznymi dla podanego typu ("measurements") i identyfikatora, 
     * wczytuje ich zawartość oraz datę utworzenia na podstawie nazwy pliku. Zwraca wektor par 
     * (czas, dane).
     * 
     * @param type Typ danych ("measurements").
     * @param id Identyfikator czujnika, dla którego wczytywane są dane.
     * @return QVector<QPair<QDateTime, QByteArray>> Wektor par zawierających czas i dane.
     */
    static QVector<QPair<QDateTime, QByteArray>> loadAllHistoricalData(const QString &type, int id);
};

#endif