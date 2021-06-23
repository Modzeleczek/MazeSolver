/*
Labirynt
Łukasz Modzelewski
PS 12
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned int Type;
typedef struct Element Element;
typedef Element* Address;
struct Element
{
	Address next;
	Type value;
};

typedef struct List List;
struct List
{
	Address first;
	unsigned int length;
};

//Dodaje element przechowujacy okreslona wartosc na koniec listy.
void pushBack(List* list, const Type value)
{
	if(list->first == NULL)
	{
		list->first = (Address) malloc(sizeof(Element));
		list->first->next = NULL;
		list->first->value = value;
		++(list->length);
	}
	else
	{
		Address temp = list->first;
		while(temp->next != NULL)
			temp = temp->next;
		temp->next = (Address) malloc(sizeof(Element));
		temp->next->next = NULL;
		temp->next->value = value;
		++(list->length);
	}
}

//Zwraca wartosc przechowywana przez element listy o podanym indeksie (elementy sa numerowane od 0).
Type getValueAt(const List list, const unsigned int index)
{
	if(list.first == NULL)
		return 0;
	
	Address temp = list.first;
	unsigned int counter = 0;
	while(counter < index && temp->next != NULL)
	{
		temp = temp->next;
		++counter;
	}
	if(counter == index)
		return temp->value;
	else
		return 0;
}

//Usuwa element listy o podanym indeksie (elementy sa numerowane od 0).
void removeAt(List* list, const unsigned int index)
{
	if(list == NULL)
		return;
	if(list->first == NULL)
		return;

	Address temp = list->first;
	Address previous = NULL;
	unsigned int counter = 0;
	while(counter < index && temp->next != NULL)
	{
		previous = temp;
		temp = temp->next;
		++counter;
	}
	if(counter == index)
	{
		if(temp->next != NULL)
		{
			if(previous != NULL)
				previous->next = temp->next;
			else
				list->first = temp->next;
			temp->value = 0;
			temp->next = NULL;
			free(temp);
			temp = NULL;
		}
		else
		{
			temp->value = 0;
			free(temp);
			temp = NULL;
			if(previous != NULL)
				previous->next = NULL;
			else
				list->first = NULL;
		}
		--(list->length);
	}
}

//Wypisuje wszystkie elementy listy.
char printAll(const List list)
{
	if(list.first == NULL)
	{
		printf("Lista jest pusta\n");
		return -1;
	}

	Address temp = list.first;
	printf("%c", temp->value);
	while(temp->next != NULL)
	{
		temp = temp->next;
		printf("%c", temp->value);
	}
	printf("\n");
	return 0;
}

//Usuwa wszystkie elementy z listy.
void deleteAll(List* list)
{
	if(list->first == NULL)
		return;

	Address temp = list->first;
	Address next = temp->next;
	temp->value = 0;
	temp->next = NULL;
	free(temp);
	temp = NULL;
	list->first = NULL;
	while(next != NULL)
	{
		temp = next;
		next = next->next;
		temp->value = 0;
		temp->next = NULL;
		free(temp);
		temp = NULL;
	}
	list->length = 0;
}

/*
wymagania dotyczace pliku z labiryntem:
$ - poczatek
@ - koniec
# - sciana, po ktorej nie mozna chodzic
. - korytarz, po ktorym mozna chodzic

wysokosc szerokosc
8 8
########
......##
$..#..##
##.#####
##.#####
##.#####
##.....@
########
*/
#define MAX_NAME_LENGTH 64
//Wczytuje obraz labiryntu z pliku (wersja na Windowsy - CRLF na koncu wierszy w pliku z labiryntem).
char* loadImageWindows(unsigned int* width, unsigned int* height)
{
	char* name = (char*) malloc(MAX_NAME_LENGTH * sizeof(char));
	printf("Podaj nazwe pliku z obrazem labiryntu (maksymalnie 63 znaki): ");
	//pliki moga miec w nazwach biale znaki (np. spacje), wiec uzywany jest fgets
	fgets(name, MAX_NAME_LENGTH, stdin);
	const unsigned int nameLength = strlen(name);
	//usuniecie z konca nazwy pliku znaku nowej linii pobranego przez fgets
	if(name[nameLength - 1] == '\n')
		name[nameLength - 1] = '\0';

	FILE* input = fopen(name, "r");
	if(input == NULL)
	{
		char* information = (char*) malloc( (nameLength + 29 + 1) * sizeof(char) );
		strcpy(information, "\nNie udalo sie otworzyc pliku ");//kopiuje do napisu information napis "Nie udalo sie wczytac pliku "
		strcat(information, name);//konkatenuje (laczy) 2 napisy: information i name i zapisuje wynik w napisie information
		puts(information);//wypisuje napis information
		free(information);
		free(name);//zwalnia napis przechowujacy nazwe pliku
		return NULL;
	}
	
	unsigned int x, y;
	fscanf(input, "%u %u", height, width);
	fseek(input, 2, SEEK_CUR);//przesuniecie kursora o 2 znaki, zeby ominac CRLF wystepujace po liczbach w 1. wierszu
	char* image = (char*) malloc( (*height) * (*width) * sizeof(char) );
	unsigned int i = 0;
	
	for(y = 0; y < *height; ++y)
	{
		for(x = 0; x < *width; ++x)
		{
			image[i] = fgetc(input);
			if(image[i] == '\n' || image[i] == EOF)
			{
				printf("\nObraz labiryntu jest nieprawidlowy\n");
				fclose(input);
				free(name);
				free(image);
				return NULL;
			}
			++i;
		}
		fseek(input, 2, SEEK_CUR);//przenosi kursor o 2 znaki do przodu w stosunku do obecnej pozycji (omija CRLF)
	}
	fclose(input);
	free(name);
	return image;
}

//Funkcja generuje labirynt poprzez przebieganie tablicy wypelnionej znakami sciany ('#') w losowych kierunkach i wstawianie w odwiedzonych komorkach znakow korytarza ('.').
char* generate(unsigned int* width, unsigned int* height)
{
	srand(time(NULL));
    *width = rand() % 10 + 10;
	*height = *width;
    char* image = (char*) malloc( (*width) * (*height) * sizeof(char));
	int x, y, i;

	for(i = 0; i < (*width) * (*height); ++i)
		image[i] = '#';

    const unsigned int numberOfMoves = ( (*width) * (*height) ) / 8;
	x = rand() % (*width);
    y = rand() % (*height);
	int random, position = x + (*width) * y;
    image[position] = '$';
	char turn = 0;
    i = 0;
    while(i < numberOfMoves)
    {
		if(turn == 0)
		{
			if(rand() % 2 == 0)
				random = 0;
			else
				random = 3;
			turn = 1;
		}
		else
		{
			if(rand() % 2 == 0)
				random = 1;
			else
				random = 2;
			turn = 0;
		}
		
		x = position % (*width);
		y = position / (*width);
		switch(random)
		{
			case 0://ruch w gore
				y = y - (rand() % (y + 1));
				y = x + (*width) * y;
				while(position > y)
				{
					if(image[position] != '$')
						image[position] = '.';
					position -= *width;
				}
				break;
				
			case 1://ruch w lewo
				x = x - (rand() % (x + 1));
				x = x + (*width) * y;
				while(position > x)
				{
					if(image[position] != '$')
						image[position] = '.';
					--position;
				}
				break;

			case 2://ruch w prawo
				if(*width != x)
					x = x + (rand() % (*width - x));
				x = x + (*width) * y;
				while(position < x)
				{
					if(image[position] != '$')
						image[position] = '.';
					++position;
				}
				break;

			case 3://ruch w dol
				if(*height != y)
					y = y + (rand() % (*height - y));
				y = x + (*width) * y;
				while(position < y)
				{
					if(image[position] != '$')
						image[position] = '.';
					position += *width;
				}
				break;
		}
		++i;
    }
    image[position] = '@';
	return image;
}

//Wypisuje jednowymiarowa tablice podana jako pierwszy parametr tak, jakby byla dwuwymiarowa, o wymiarach height x width.
void print1DTableAs2D(const char* table, const unsigned int width, const unsigned int height)
{
	unsigned int x, y;
	for(y = 0; y < height; ++y)
	{
		for(x = 0; x < width; ++x)
		{
			printf("%c", *table);
			++table;
		}
		printf("\n");
	}
}

//Otacza labirynt zewnetrznymi scianami, aby uniemozliwic wyjscie poza tablice przez algorytm wyszukiwania sciezki.
void addOuterWalls(const char* source, char* destination, unsigned int width, unsigned int height)
{
	unsigned int i, j, x, y;
	//kopiowanie wnetrza ze zrodla, wiersz po wierszu
	i = 0;//iterator tablicy zrodlowej
	j = 1 + width + 2;//iterator tablicy docelowej
	for(y = 0; y < height; ++y)
	{
		memcpy(destination + j, source + i, width);
		j += width + 2;
		i += width;
	}

	//labirynt po dodaniu zewnetrznych scian ma szerokosc i dlugosc wieksze o 2
	width += 2;
	height += 2;
	//wypelnianie krawedzi scianami ('#')
	//gorna i dolna krawedz
	i = 0;//iterator gornej krawedzi
	j = width * (height - 1);//iterator dolnej krawedzi
	for(x = 0; x <= width - 1; ++x)
	{
		destination[i] = '#';
		destination[j] = '#';
		++i;
		++j;
	}
	//lewa i prawa krawedz
	i = width;//iterator lewej krawedzi
	j = (width - 1) + width;//iterator prawej krawedzi
	for(y = 1; y <= height - 2; ++y)
	{
		destination[i] = '#';
		destination[j] = '#';
		i += width;
		j += width;
	}
}

List findPath(const char* map, const unsigned int mapWidth, const unsigned int mapHeight, unsigned int* begin);
List reproduceStepList(const List path, const unsigned int mapWidth, const unsigned int mapHeight, const unsigned int start);

int main()
{
	char choice;
	unsigned int width, height;
	printf("Dostepne tryby:\n[g] generowany labirynt\n[p] labirynt z pliku\nWybierz tryb: ");
	scanf(" %c", &choice);
	getc(stdin);//wczytanie z bufora znaku nowej linii; fflush(stdin) nie zawsze dziala
	char* image;
	switch(choice)
	{
		case 'g':
			image = generate(&width, &height);
			break;
		case 'p':
			image = loadImageWindows(&width, &height);
			break;
		default:
			printf("Nieprawidlowy tryb\n");
			getc(stdin);
			return -1;
	}
	if(image == NULL)
	{
		printf("Nie udalo sie wczytac labiryntu\n");
		getc(stdin);
		return -2;
	}
	char* preparedImage = (char*) malloc( (height + 2) * (width + 2) );
	addOuterWalls(image, preparedImage, width, height);
	free(image);
	image = NULL;

	//labirynt z zewnetrznymi krawedziami ma o 2 wieksze szerokosc i dlugosc
	width += 2;
	height += 2;
	unsigned int i;
	List path = findPath(preparedImage, width, height, &i);
	List steps = reproduceStepList(path, width, height, i);
	printf("\nLista krokow:\n");
	if(printAll(steps) == 0)
	{
		for(i = 1; i < path.length; ++i)
			preparedImage[getValueAt(path, i)] = 'x';
		
		printf("\nZnaki 'x' wskazuja sciezke\n");
		print1DTableAs2D(preparedImage, width, height);
	}
	else
	{
		printf("Nie mozna przejsc labiryntu\n\n");
		print1DTableAs2D(preparedImage, width, height);
	}

	deleteAll(&steps);
	deleteAll(&path);
	free(preparedImage);
	preparedImage = NULL;
	getc(stdin);
	return 0;
}

//Odtwarza liste kolejnych krokow na podstawie sciezki uzyskanej w funkcji findPath.
List reproduceStepList(List path, const unsigned int mapWidth, const unsigned int mapHeight, const unsigned int start)
{
	if(path.length == 0)
		return path;//zwroc pusta liste
	const int directions[4] = { -mapWidth, -1, 1, mapWidth };
	unsigned int currentCell = start;
	List steps = {NULL, 0};
	pushBack(&steps, 'S');
	unsigned int neighbour;
	int i;
	for(i = path.length - 1; i >= 0; --i)
	{
		neighbour = getValueAt(path, i);
		if(neighbour == currentCell + directions[0])
			pushBack(&steps, 'G');
		else if(neighbour == currentCell + directions[1])
			pushBack(&steps, 'L');
		else if(neighbour == currentCell + directions[2])
			pushBack(&steps, 'P');
		else if(neighbour == currentCell + directions[3])
			pushBack(&steps, 'D');
		else
			pushBack(&steps, '*');
		currentCell = neighbour;
	}
	pushBack(&steps, 'K');
	return steps;
}

typedef struct Cell Cell;
struct Cell
{
	unsigned int F, G, parent;
	char available, closed, open;
};

/*
Algorytm wyszukiwania sciezki A*, pseudokod:
https://en.wikipedia.org/wiki/A*_search_algorithm#Pseudocode
https://pl.wikipedia.org/wiki/Algorytm_A*#Algorytm_A*_w_pseudokodzie
*/
List findPath(const char* map, const unsigned int mapWidth, const unsigned int mapHeight, unsigned int* begin)
{
	const unsigned int mapLength = mapWidth * mapHeight;
	
	Cell* cells = (Cell*) malloc( mapLength * sizeof(Cell) );
	
	unsigned int start = 0, destination = 0;
 	unsigned int i;
	for(i = 0; i < mapLength; ++i)
	{
		cells[i].F = cells[i].G = cells[i].closed = cells[i].open = cells[i].parent = 0;
		
		if(map[i] == '#')
			cells[i].available = 0;
		else
		{
			cells[i].available = 1;
			if(map[i] == '$')
				start = i;
			else if(map[i] == '@')
				destination = i;
		}
	}
	//informacja o poczatku labiryntu dla funkcji odtwarzajacej sciezke, aby nie musiala ona drugi raz przeszukiwac calej tablicy z labiryntem
	if(begin != NULL)
		*begin = start;

	List path = {NULL, 0};
	if(cells[start].available == 1)
	{
		const int directions[4] = { -mapWidth, -1, 1, mapWidth };
		unsigned int Htemp, smallestF, Gtemp;
		List openIndexes = {NULL, 0};
		cells[start].G = 0;
		Htemp = abs( (start % mapWidth) - (destination % mapWidth) ) + abs( (start - destination) / mapHeight );
		cells[start].F = cells[start].G + Htemp;
		cells[start].open = 1;
		pushBack(&openIndexes, start);

		unsigned int currentCell = start, smallestFIndex, temp, neighbour;
		while(openIndexes.length > 0)
		{
			//znalezienie komorki o najmniejszym koszcie przejscia F wsrod otwartych komorek
			temp = getValueAt(openIndexes, 0);
			smallestF = cells[temp].F;
			currentCell = temp;
			smallestFIndex = 0;
			for(i = 1; i < openIndexes.length; ++i)
			{
				temp = getValueAt(openIndexes, i);
				if(cells[temp].F < smallestF)
				{
					smallestF = cells[temp].F;
					currentCell = temp;
					smallestFIndex = i;
				}
			}

			//dotarcie do celu
			if(currentCell == destination)
			{
				i = currentCell;
				while(i != start)
				{
					pushBack(&path, i);
					i = cells[i].parent;
				}
				break;
			}
			//usuniecie komorki o najmniejszym F, o indeksie smallestFIndex z listy openIndexes - otwartych komorek
			removeAt(&openIndexes, smallestFIndex);
			//ustawienie statusu tej komorki na nie otwarta i zamknieta
			cells[currentCell].open = 0;
			cells[currentCell].closed = 1;
			for(i = 0; i < 4; ++i)
			{
				neighbour = currentCell + directions[i];
				if(cells[neighbour].available == 1 && cells[neighbour].closed == 0)
				{
					Gtemp = cells[currentCell].G + abs( (neighbour % mapWidth) - (currentCell % mapWidth) ) + abs( directions[i] / mapHeight ) ;
					if(cells[neighbour].open == 0)
					{
						cells[neighbour].open = 1;
						pushBack(&openIndexes, neighbour);
						Htemp = abs( (neighbour % mapWidth) - (destination % mapWidth) ) + abs( (neighbour - destination) / mapHeight );

						cells[neighbour].parent = currentCell;
						cells[neighbour].G = Gtemp;
						cells[neighbour].F = cells[neighbour].G + Htemp;
					}
					else if(Gtemp < cells[neighbour].G)
					{
						cells[neighbour].parent = currentCell;
						cells[neighbour].G = Gtemp;
						cells[neighbour].F = cells[neighbour].G + Htemp;
					}
				}
			}
		}
		deleteAll(&openIndexes);
	}
	else
		printf("\nPoczatek sciezki jest na niedostepnym polu\n");
	free(cells);
	return path;
}
