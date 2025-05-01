/**
 * @file main.cpp
 * @brief Główny plik programu uruchamiający aplikację Qt.
 */

 #include <QApplication>
 #include "mainwindow.h"
 
 /**
  * @brief Funkcja główna programu.
  * @param argc Liczba argumentów wiersza poleceń.
  * @param argv Tablica argumentów wiersza poleceń.
  * @return Kod wyjścia aplikacji (0 w przypadku sukcesu).
  */
 int main(int argc, char *argv[]) {
     QApplication a(argc, argv);
     MainWindow w;
     w.show();
     return a.exec();
 }