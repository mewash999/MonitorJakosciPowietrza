#include <QApplication>
#include "mainwindow.h"

/**
 * @brief Główna funkcja aplikacji.
 * 
 * Inicjalizuje aplikację Qt, tworzy główne okno aplikacji (`MainWindow`) i uruchamia pętlę zdarzeń. 
 * Zwraca kod wyjścia aplikacji po jej zamknięciu.
 * 
 * @param argc Liczba argumentów wiersza poleceń.
 * @param argv Tablica argumentów wiersza poleceń.
 * @return int Kod wyjścia aplikacji (0 oznacza sukces).
 */
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}