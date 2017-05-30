# Zanim pochopnie ocenisz - przeczytaj

Program powstał na zadanie na jedno z laboratoriów z przedmiotu Programowanie Komputerów (II sem.). Celem tego zadania było wymyślenie i stworzenie programu w języku C, który realizowałby maszynę stanów dla tematu "Protokół HTTP" czyli maszyna miałaby przyjmować stany (nasłuchiwanie, rządanie, błąd, odpowiedź, odmowa, time out, potwierdzenie, metoda). Należało wykorzystać bufor cykliczny do zapisu stanów zadanego automatu, a stanu z bufora zapisywać do pliku tekstowego oraz na ekran. Cała reszta jest moją interpretacją postawionego zadania i próbą zrobienia go w jak najciekawszy sposób, który nie byłby pójściem na łatwiznę.


PONIŻSZY OPIS ZNAJDUJE SIĘ RÓWNIEŻ W PLIKU .c
Maszyna stanow realizujaca cos w rodzaju protokolu http, oraz prosta symulacja klienta i serwera
Serwer jest tablica, ktorej komorki zawieraja ciagi znakow bez znakow bialych, kazda komorka zawiera odpowiednie wartosci oznaczajace niemozliwosc modyfikacji, lub niemozliwosc usuwania zawartosci tej komorki przez uzytkownika
Uzytkownik dostaje do uzytku trzy komendy - operacje:
  GET adres				- pobiera zawartosc komorki o podanym adresie
  POST adres wartosc		- modyfikuje zawartosc komorki o podanym adresie
  DELETE adres				- usuwa zawartosc komorki o podanym adresie
protokol uzyskuje stan "nasluchiwanie" dopoki uzytkownik nie poda zadnych komend.
W przypadku podania nieprawidlowych operacji protokol przyjmuje stan "blad" i pozostaje w nim dopoki uzytkownik nie nacisnie klawisza enter. 
W przypadku podania właściwej komendy z wymaganyi danymi protokol przechodzi w stan zadania i probuje uzyskac odpowiedz od serwera. 
Jeżeli uzytkownik podal nieprawidlowy adres serwer nie zwroci nic, protokol zaczeka sekunde po czym przejdzie w stan "time out", a nastepnie w stan "odpowiedz" w ktorym zwroci uzytkownikowi odpowiedz i zaczeka na wcisniecie klawisza enter.
Jeżeli uzytkownik probowal usunac albo zmodyfikowac komorke, ktora jest niemodyfikowalna lub probowal usuwac nieusuwalna, lub probowal wyswietlic usunieta komorke serwer zwroci 0 po czym protokol przyjmie stan "odmowa", a nastepnie przejdzie w stan "odpowiedz" w ktorym zwroci uzytkownikowi odpowiedz i zaczeka na wcisniecie klawisza enter.
Jeżeli operacja sie powiedzie serwer zwroci 1, protokol przyjmie stan "potwierdzenie", a nastepnie przejdzie w stan "odpowiedz" w ktorym zwroci uzytkownikowi odpowiedz i zaczeka na wcisniecie klawisza enter.
Po tym jak uzytkownik zatwierdzi odpowiedz, protokol przejdzie w stan nasluchiwania, az uzytkownik ponownie poda komende
Do pliku stany są zapisywane w postaci nazwy stanu. Nazwa pliku zawiera date i czas uruchomienia programu. Do pliku zapisywane są również polecenia wpisane przez użytkownika, podane po '<', po których widać jak następują zmiany stanów.
Zawartosc servera jest wczytywana z pliku .dat o nazwie zdefiniowanej w makrze SERVER_FILE_FILENAME. Po zamknieciu programu odpowiednią komendą (Q) nastepuje zapisywanie zmian. Jeżeli program zostanie zamkniety w inny sposob zmiany nie zostana zapisane. Jest to zrobione celowo.
Informacja o poprawnym zapisaniu zmian w pliku serwera jest dodawana na koniec pliku z zapisem stanow (trzy wiersze ponizej ostatniej komendy). Jezeli tej informacji nie ma, oraz plik ze zmiamami stanu jest pusty oznacza to, ze zmiany nie zostaly zapisane poniewaz program został zamknięty nieprawidłowo.
