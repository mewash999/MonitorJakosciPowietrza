# Monitor Jakości Powietrza
<div style="text-align: justify">
MJP to aplikacja desktopowa stworzona w Qt (C++) do monitorowania jakości powietrza w Polsce. Wykorzystuje publiczne API GIOS (Główny Inspektorat Ochrony Środowiska) do pobierania danych o stacjach pomiarowych, czujnikach i wynikach pomiarów. Aplikacja działa zarówno w trybie online, jak i offline, umożliwiając przeglądanie danych historycznych zapisanych lokalnie.
</div>

## Funkcjonalności

* Wyświetlanie listy stacji pomiarowych z możliwością wyszukiwania po nazwie.<br>
* Wyświetlanie listy czujników dla wybranej stacji.<br>
* Prezentacja aktualnych i historycznych danych pomiarowych zwizualizowanych w formie wykresu.<br>
* Prosta analiza danych oraz wskazanie aktualnego trendu danych.<br>
* Możliwość przeglądania danych zapisanych lokalnie w przypadku (braku) połączenia z internetem.<br>
* Pełna obsługa programu myszką i/lub klawiszami Tab/Enter/strzałkami do wyboru stacji/czujników oraz nawigacji po przyciskach.<br>

## Wymagania

System operacyjny: Windows<br>
Qt: Wersja 5.15 lub nowsza.<br>
Kompilator: C++11 lub nowszy.<br>
Biblioteki: Qt Core, Qt GUI, Qt Network, Qt Widgets, Qt Charts.<br>

Wymagane połączenie internetowe do pobierania danych w trybie online.<br>
Pliki JSON z zapisywanymi danymi w %appdata%/MJP<br>

## Instalacja

Sklonuj repozytorium<br>
Pobierz i zainstaluj Qt z oficjalnej strony wraz z wymaganymi pakietami.<br>
Otwórz plik projektu (MJP.pro) w Qt Creator.<br>
Skonfiguruj ustawienia kompilacji (qmake) i uruchom budowę programu.<br>

## Pliki źródłowe:

* `mainwindow.cpp, mainwindow.h`: Główna klasa okna aplikacji, obsługa interfejsu i logiki.<br>
* `connectionmanager.cpp, connectionmanager.h`: Testowanie połączenia i przełączanie trybu online/offline.<br>
* `apiclient.cpp, apiclient.h`: Komunikacja z API GIOS.<br>
* `apiworker.cpp, apiworker.h`: Obsługa osobnego wątku dla zapytań sieciowych.<br>
* `stationhandler.cpp, stationhandler.h`: Obsługa danych stacji (wypełnianie listy, sortowanie, wyszukiwanie).<br>
* `sensorhandler.cpp, sensorhandler.h`: Obsługa danych czujników.<br>
* `measurementhandler.cpp, measurementhandler.h`: Przetwarzanie i wizualizacja danych pomiarowych.<br>
* `datamanager.cpp, datamanager.h`: Zarządzanie danymi lokalnymi (zapis/odczyt JSON).<br>
* `mainwindow.ui`: Plik interfejsu Qt Designer definiujący układ okna.<br>

## Autor

Miłosz Kurpisz<br>
Wszelkie prawa zastrzeżone!
