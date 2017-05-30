// maszyna stanow realizujaca cos w rodzaju protokolu http, oraz prosta implementacja klienta i serwera
// serwer jest tablica, ktorej komorki zawieraja ciagi znakow bez znakow bialych, kazda komorka zawiera odpowiednie wartosci oznaczajace niemozliwosc modyfikacji, lub niemozliwosc usuwania zawartosci tej komorki przez uzytkownika
// uzytkownik dostaje do uzytku trzy komendy - operacje:
// GET adres				- pobiera zawartosc komorki o podanym adresie
// POST adres wartosc		- modyfikuje zawartosc komorki o podanym adresie
// DELETE adres				- usuwa zawartosc komorki o podanym adresie
// protokol uzyskuje stan "nasluchiwanie" dopoki uzytkownik nie poda zadnych komend.
// W przypadku podania nieprawidlowych operacji protokol przyjmuje stan "blad" i pozostaje w nim dopoki uzytkownik nie nacisnie klawisza enter. 
// W przypadku podania w³aœciwej komendy z wymaganyi danymi protokol przechodzi w stan zadania i probuje uzyskac odpowiedz od serwera. 
// Je¿eli uzytkownik podal nieprawidlowy adres serwer nie zwroci nic, protokol zaczeka sekunde po czym przejdzie w stan "time out", a nastepnie w stan "odpowiedz" w ktorym zwroci uzytkownikowi odpowiedz i zaczeka na wcisniecie klawisza enter.
// Je¿eli uzytkownik probowal usunac albo zmodyfikowac komorke, ktora jest niemodyfikowalna lub probowal usuwac nieusuwalna, lub probowal wyswietlic usunieta komorke serwer zwroci 0 po czym protokol przyjmie stan "odmowa", a nastepnie przejdzie w stan "odpowiedz" w ktorym zwroci uzytkownikowi odpowiedz i zaczeka na wcisniecie klawisza enter.
// Je¿eli operacja sie powiedzie serwer zwroci 1, protokol przyjmie stan "potwierdzenie", a nastepnie przejdzie w stan "odpowiedz" w ktorym zwroci uzytkownikowi odpowiedz i zaczeka na wcisniecie klawisza enter.
// Po tym jak uzytkownik zatwierdzi odpowiedz, protokol przejdzie w stan nasluchiwania, az uzytkownik ponownie poda komende
// Do pliku stany s¹ zapisywane w postaci nazwy stanu. Nazwa pliku zawiera date i czas uruchomienia programu. Do pliku zapisywane s¹ równie¿ polecenia wpisane przez u¿ytkownika, podane po '<', po których widaæ jak nastêpuj¹ zmiany stanów.
// Zawartosc servera jest wczytywana z pliku .dat o nazwie zdefiniowanej w makrze SERVER_FILE_FILENAME. Po zamknieciu programu odpowiedni¹ komend¹ (Q) nastepuje zapisywanie zmian. Je¿eli program zostanie zamkniety w inny sposob zmiany nie zostana zapisane. Jest to zrobione celowo.
// Informacja o poprawnym zapisaniu zmian w pliku serwera jest dodawana na koniec pliku z zapisem stanow (trzy wiersze ponizej ostatniej komendy). Jezeli tej informacji nie ma, oraz plik ze zmiamami stanu jest pusty oznacza to, ze zmiany nie zostaly zapisane poniewaz program zosta³ zamkniêty nieprawid³owo.

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

#define MAX_STRING_LENGTH 25
#define SERVER_CAPACITY 20
#define MIN_SERVER_CELL_INDEX 0
#define MAX_SERVER_CELL_INDEX (SERVER_CAPACITY - 1)
#define NUMBER_OF_UNMODIFIABLES_CELLS 5 //licz¹c od pocz¹tku tablicy
#define NUMBER_OF_UNREMOVABLE_CELLS 10 //licz¹c od pocz¹tku tablicy
#define SIZE_OF_CIRCULAR_BUFFER 5
#define MIN_INDEX_OF_CIRCULAR_BUFFER_TAB 0
#define MAX_INDEX_OF_CIRCULAR_BUFFER_TAB (SIZE_OF_CIRCULAR_BUFFER - 1)
#define FILENAME_LENGTH 21
#define EMPTY_VALUE (-999)
#define EMPTY_STRING ""
#define ENTER_KEY 13
#define ESC_KEY 27
#define HELP_MESSAGE "DOSTEPNE OPERACJE:\nGET [adres] - pobiera zawartosc komorki o podanym adresie\nPOST [adres] [lancuch znakow] - modyfikuje zawartosc komorki o podanym adresie\nDELETE [adres] - usuwa zawartosc komorki o podanym adresie\nQ - wychodzi z programu\n[adres] nalezy zastapic liczba nieujemna bedaca adresem. W przypadku podania niepoprawnego adresu uzytkownik zostanie poinformowany.\n[lancuch znakow] nalezy zastapic lancuchem znakow niezawierajacym znakow bialych.\nKazda komende nalezy zatwierdzic wciskajac enter.\nNacisnij enter aby kontynuowac.\n"
#define TRUE 1
#define FALSE 0
#define WAIT_FOR_TIME_OUT_TIME 1000 
#define PRESS_ENTER_TEXT "Nacisnij enter aby kontynuowac.\n"
#define SERVER_FILE_FILENAME "serverFile.dat"

struct Memory_cell {
	char content[MAX_STRING_LENGTH];
	int isModifiable;
	int isRemovable;
};

enum State_names {LISTENING, REQUEST, ERROR_STATE, TIME_OUT, DENIAL, CONFIRMATION, ANSWER}; //ERROR_STATE poniewa¿ ERROR to jakieœ makro z biblioteki <Windows.h>

void set_cells(struct Memory_cell[], FILE *serverFile);
void print_state(enum State_names state, FILE *stateHistoryFile);
void change_state(enum State_names state, enum State_names buffer[], int * currentBufferIndex, FILE * stateHistoryFile);
void get(int tempAdres, struct Memory_cell server[], enum State_names buffer[], int * currentBufferIndex, FILE * stateHistoryFile);
void post(int tempAdres, char tempContent[], struct Memory_cell server[], enum State_names buffer[], int * currentBufferIndex, FILE * stateHistoryFile);
void delete_cell(int tempAdres, struct Memory_cell server[], enum State_names buffer[], int * currentBufferIndex, FILE * stateHistoryFile); //samo delete to s³owo kluczowe
int is_address_proper(int tempAdres, enum State_names buffer[], int * currentBufferIndex, FILE * stateHistoryFile);
void write_command_to_file(char command[],  int tempAdres, char tempContent[], FILE * stateHistoryFile);

int main()
{
	FILE *serverFile;
	if ((serverFile = fopen(SERVER_FILE_FILENAME, "r+b")) == NULL) {
		if ((serverFile = fopen(SERVER_FILE_FILENAME, "w+b")) == NULL) {
			printf("Nie mogê otworzyæ pliku %s!\n", SERVER_FILE_FILENAME);
			exit(1);
		}
	}
	struct Memory_cell server[SERVER_CAPACITY];
	set_cells(server, serverFile);
	enum State_names buffer[SIZE_OF_CIRCULAR_BUFFER]; 
	int currentBufferIndex = MIN_INDEX_OF_CIRCULAR_BUFFER_TAB;
	int tempAdres;
	char tempContent[MAX_STRING_LENGTH];
	char command[50]; // tablica tak duza, aby zmniejszyc szanse na zapisanie komorek poza ni¹ przez u¿ytkownika
	FILE *stateHistoryFile;
	char filename[FILENAME_LENGTH];
	time_t currentTime;
	time(&currentTime);
	struct tm * localCurrentTime = localtime(&currentTime);
	strftime(filename, FILENAME_LENGTH, "%d%b%y_%H;%M;%S.txt", localCurrentTime); //Zamienia date na stringa. %d%b%y - data (03Nov10), %H;%M;%S - godzina (02;40;31).
	if ((stateHistoryFile = fopen(filename, "w+")) == NULL) {
		printf("Nie moge otworzyc pliku %s do zapisu!\n", filename);
		exit(1);
	}
	printf("SYMULACJA UPROSZCZONEGO PROTOKOLU HTTP.\nSerwer jest tablica, ktorej komorki zawieraja ciagi znakow bez znakow bialych, o maksymalnej dlugosci %d znakow.\nZawartosc komorek serwera pobieramy/usuwamy/modyfikujemy uzywajac odpowiednich polecen.\nNiektore komorki maja modyfikatory dostepu uniemozliwiajace modyfikacje/usuniecie ich wartosci. W przypadku gdy uzytkownik sprobuje wykonac na nich te operacje dostanie informacje o niemozliwosci ich wykonania.\nAby uzyskac liste mozliwych operacji wpisz komende HELP.\nNacisnij enter aby kontynuowac lub ESC aby wyjsc.\n", MAX_STRING_LENGTH);
	unsigned char pressedKey = _getch();
	while (pressedKey != ENTER_KEY && pressedKey != ESC_KEY)
		pressedKey = _getch();
	while (pressedKey == ENTER_KEY) {
		change_state(LISTENING, buffer, &currentBufferIndex, stateHistoryFile);
		tempAdres = EMPTY_VALUE;
		strcpy(tempContent, EMPTY_STRING);
		// POBIERANIE POLECEÑ OD U¯YTKOWNIKA
		scanf("%s", command);
		if (!_stricmp(command, "GET")) { // stricmp sprawdza czy wpisany lancuch jest taki sam przy czym duze litery sa uwazane za takie same jak male
			scanf("%d", &tempAdres);
			write_command_to_file(command, tempAdres, tempContent, stateHistoryFile);
			change_state(REQUEST, buffer, &currentBufferIndex, stateHistoryFile);
			if(is_address_proper(tempAdres, buffer, &currentBufferIndex, stateHistoryFile))
				get(tempAdres, server, buffer, &currentBufferIndex, stateHistoryFile);
		}
		else if (!_stricmp(command, "POST")) {
			printf("POST ");
			scanf("%d%s", &tempAdres, &tempContent);
			write_command_to_file(command, tempAdres, tempContent, stateHistoryFile);
			change_state(REQUEST, buffer, &currentBufferIndex, stateHistoryFile);
			if (is_address_proper(tempAdres, buffer, &currentBufferIndex, stateHistoryFile))
				post(tempAdres, tempContent, server, buffer, &currentBufferIndex, stateHistoryFile);
		}
		else if (!_stricmp(command, "DELETE")) {
			printf("DELETE ");
			scanf("%d", &tempAdres);
			write_command_to_file(command, tempAdres, tempContent, stateHistoryFile);
			change_state(REQUEST, buffer, &currentBufferIndex, stateHistoryFile);
			if (is_address_proper(tempAdres, buffer, &currentBufferIndex, stateHistoryFile))
				delete_cell(tempAdres, server, buffer, &currentBufferIndex, stateHistoryFile);
		}
		else if (!_stricmp(command, "HELP")) {
			printf(HELP_MESSAGE);
			write_command_to_file(command, tempAdres, tempContent, stateHistoryFile);
		}
		else if (!_stricmp(command, "Q")) {
			write_command_to_file(command, tempAdres, tempContent, stateHistoryFile);
			fprintf(stateHistoryFile, "\n\nProgram zostal zamkniety pomyslnie i zmiany zostaly zapisane.");
			break;
		}
		else {
			write_command_to_file(command, tempAdres, tempContent, stateHistoryFile);
			change_state(ERROR_STATE, buffer, &currentBufferIndex, stateHistoryFile);
		}
		pressedKey = _getch();
		while (pressedKey != ENTER_KEY)
			pressedKey = _getch();
	}
	fseek(serverFile, 0, SEEK_SET);
	fwrite(server, sizeof(struct Memory_cell), SERVER_CAPACITY, serverFile);
	fclose(serverFile);
	fclose(stateHistoryFile);
	return 0;
}

void set_cells(struct Memory_cell server[], FILE *serverFile)
{
	int ile = fread(server, sizeof(struct Memory_cell), SERVER_CAPACITY, serverFile);

	if (ile == 0) {
		for (int i = 0; i < SERVER_CAPACITY; i++) {
			strcpy(server[i].content, "ALA_MA_KOTA");
			server[i].isModifiable = TRUE;
			server[i].isRemovable = TRUE;
		}
		strcpy(server[0].content, EMPTY_STRING);
		for (int i = 0; i < NUMBER_OF_UNMODIFIABLES_CELLS; i++)
			server[i].isModifiable = FALSE;
		for (int i = 0; i < NUMBER_OF_UNREMOVABLE_CELLS; i++)
			server[i].isRemovable = FALSE;
	}
}

void print_state(enum State_names state, FILE *stateHistoryFile)
{
	char stateName[15];
	switch (state)
	{
	case LISTENING:
		strcpy(stateName, "LISTENING"); break;
	case REQUEST:
		strcpy(stateName, "REQUEST"); break;
	case ERROR_STATE:
		strcpy(stateName, "ERROR"); break;
	case TIME_OUT:
		strcpy(stateName, "TIME_OUT"); break;
	case DENIAL:
		strcpy(stateName, "DENIAL"); break;
	case CONFIRMATION:
		strcpy(stateName, "CONFIRMATION"); break;
	case ANSWER:
		strcpy(stateName, "ANSWER"); break;
	}
	fprintf(stateHistoryFile, "%s\n", stateName);
	printf("%s\n", stateName);
}

void change_state(enum State_names state, enum State_names buffer[], int * currentBufferIndex, FILE *stateHistoryFile)
{
	buffer[*currentBufferIndex] = state;
	print_state(buffer[*currentBufferIndex], stateHistoryFile);
	(*currentBufferIndex)++;
	if (*currentBufferIndex > MAX_INDEX_OF_CIRCULAR_BUFFER_TAB)
		*currentBufferIndex = MIN_INDEX_OF_CIRCULAR_BUFFER_TAB;
}

void get(int tempAdres, struct Memory_cell server[], enum State_names buffer[], int * currentBufferIndex, FILE * stateHistoryFile)
{
	if (strcmp(server[tempAdres].content, EMPTY_STRING) != 0) { // dla roznych zwraca != 0, dla rownych 0
		change_state(CONFIRMATION, buffer, currentBufferIndex, stateHistoryFile);
		change_state(ANSWER, buffer, currentBufferIndex, stateHistoryFile);
		printf("-----------------\n%s\n-----------------\n", server[tempAdres].content);
	}
	else {
		change_state(DENIAL, buffer, currentBufferIndex, stateHistoryFile);
		change_state(ANSWER, buffer, currentBufferIndex, stateHistoryFile);
		printf("Nie moge wyswietlic zawartosci komorki poniewaz jest pusta. ", server[tempAdres].content);
	}
	printf(PRESS_ENTER_TEXT);
}

void post(int tempAdres, char tempContent[], struct Memory_cell server[], enum State_names buffer[], int * currentBufferIndex, FILE * stateHistoryFile)
{
	if (server[tempAdres].isModifiable == TRUE) {
		change_state(CONFIRMATION, buffer, currentBufferIndex, stateHistoryFile);
		strcpy(server[tempAdres].content, tempContent);
		change_state(ANSWER, buffer, currentBufferIndex, stateHistoryFile);
		printf("Zawartosc komorki o adresie %d zaktualizowana pomyslnie.\n", tempAdres);
	}
	else {
		change_state(DENIAL, buffer, currentBufferIndex, stateHistoryFile);
		change_state(ANSWER, buffer, currentBufferIndex, stateHistoryFile);
		printf("Nie moge zaktualizowac zawartosci tej komorki. Jeli chcesz to zmienic skontaktuj sie z administratorem serwera :)\n", server[tempAdres].content);
	}
	printf(PRESS_ENTER_TEXT); 
}

void delete_cell(int tempAdres, struct Memory_cell server[], enum State_names buffer[], int * currentBufferIndex, FILE * stateHistoryFile)
{	
	if (server[tempAdres].isRemovable == TRUE) {
		change_state(CONFIRMATION, buffer, currentBufferIndex, stateHistoryFile);
		strcpy(server[tempAdres].content, EMPTY_STRING);
		change_state(ANSWER, buffer, currentBufferIndex, stateHistoryFile);
		printf("Zawartosc komorki o adresie %d usunieta pomyslnie.\n", tempAdres);
	}
	else {
		change_state(DENIAL, buffer, currentBufferIndex, stateHistoryFile);
		change_state(ANSWER, buffer, currentBufferIndex, stateHistoryFile);
		printf("Nie moge usunac zawartosci tej komorki. Jeli chcesz to zmienic skontaktuj sie z administratorem serwera :)\n", server[tempAdres].content);
	}
	printf(PRESS_ENTER_TEXT);
}

int is_address_proper(int tempAdres, enum State_names buffer[], int * currentBufferIndex, FILE * stateHistoryFile)
{
	if (tempAdres < MIN_SERVER_CELL_INDEX || tempAdres > MAX_SERVER_CELL_INDEX) {
		Sleep(1000); // imitacja czekania na odpowiedz serwera
		change_state(TIME_OUT, buffer, currentBufferIndex, stateHistoryFile);
		change_state(ANSWER, buffer, currentBufferIndex, stateHistoryFile);
		printf("Niepoprawny adres. ");
		printf(PRESS_ENTER_TEXT);
		return FALSE;
	}
	else
		return TRUE;
}

void write_command_to_file(char command[], int tempAdres, char tempContent[], FILE * stateHistoryFile)
{
	fprintf(stateHistoryFile, ">%s", command);
	if (tempAdres != EMPTY_VALUE) {
		fprintf(stateHistoryFile, " %d", tempAdres);
		if (strcmp(tempContent, EMPTY_STRING) != 0) // dla roznych zwraca != 0, dla rownych 0
			fprintf(stateHistoryFile, " %s", tempContent);
	}
	fprintf(stateHistoryFile, "\n");
}
