## Opis
Program na początku wyświetla prośbę o wybranie trybu działania. Dostępna jest możliwość automatycznego stworzenia pseudolosowego labiryntu lub wczytania obrazu labiryntu z pliku. Jeśli użytkownik poda nieprawidłową literę trybu, próba otwarcia pliku się nie powiedzie lub labirynt zapisany w pliku jest niezgodny z podanymi wymiarami, program wyświetli błąd i zakończy działanie.

Prawidłowy plik z obrazem labiryntu powinien zawierać w pierwszym wierszu wysokość i szerokość labiryntu, a w kolejnych wierszach – labirynt zbudowany z następujących znaków: $ - początek, @ - koniec, # - ściana, po której nie można chodzić, . - korytarz, po którym można chodzić.

Funkcja wczytująca obraz labiryntu z pliku działa prawidłowo na systemach Windows, które jako znaki końca linii w plikach tekstowych traktują 2 znaki ASCII: CR (13) i LF (10). Pliki z obrazami zapisane przy użyciu testowanego przeze mnie systemu Linux powodują błąd wczytywania pliku.

Po pomyślnym wczytaniu początkowej postaci labiryntu, jest on otaczany zewnętrzną ścianą, co zapobiega wyjściu poza tablicę przechowującą labirynt przez algorytm wyszukujący ścieżkę.

Wyszukiwanie ścieżki odbywa się za pomocą algorytmu A* w funkcji findPath zwracającej listę zawierającą indeksy kolejnych komórek, które należy odwiedzić, aby przejść labirynt od końca do początku lub pustą listę, gdy labirynt jest niemożliwy do przejścia. Następnie, na podstawie listy ze ścieżką odczytywanej w przeciwnym kierunku, odtwarzana jest lista kroków, które należy wykonać, aby przejść labirynt.

Jeśli labirynt udało się przejść, program wyświetla odtworzoną listę kroków i obraz labiryntu ze ścieżką prowadzącą od początku do końca wypełnioną znakami „x”. W przeciwnym wypadku, program wyświetla informację o braku możliwości przejścia labiryntu i jego obraz.

Poza wymaganymi funkcjonalnościami: wczytywaniem obrazu labiryntu z pliku i wypisywaniem ciągu kroków wystarczających do jego przejścia, program oferuje możliwość generowania labiryntów i wyświetla przechodzone labirynty z zaznaczoną ścieżką przejścia.
