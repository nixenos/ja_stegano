//
// Created by nixen on 22.11.2021.
//

#ifndef PROJEKT_LOGIC_H
#define PROJEKT_LOGIC_H
#include <stdio.h>

#include "dataStructures.h"
/**
 * @headerfile
 * */
/**
 * funkcja wczytująca dane z nagłówków
 * @param   filePointer     wskaźnik na plik, z którego odczytane mają zostać
 * nagłówki
 * @param   bmpHeader       wskaźnik na nagłówek główny
 * @param   dibHeader       wskaźnik na nagłówek DIB
 * @return  int             jeżeli wczytywanie się powiedzie, zwracana jest
 * wartość 1, w przeciwnym wypadku zwracana jest wartość 0
 */
int read_headers(FILE *filePointer, BMPHEADER *bmpHeader, DIBHEADER *dibHeader, BITFIELDS * bitFields);

/**
 * funkcja zwracająca tablicę pixeli dla pliku z 16-bitową głębią kolorów
 * @param filePointer wskaźnik na plik obrazu
 * @param pixelCount licznik pixeli w obrazie
 * @return tablica pixeli rozbita na poszczególne składowe
 */
uint8_t *pixelArray_read_16bit(FILE *filePointer, int pixelCount);

/**
 * funkcja zwracająca tablicę pixeli dla pliku z 24-bitową głębią kolorów
 * @param filePointer wskaźnik na plik obrazu
 * @param pixelCount licznik pixeli w obrazie
 * @return tablica pixeli rozbita na poszczególne składowe
 */
uint8_t *pixelArray_read_24bit(FILE *filePointer, int pixelCount);

/**
 * funkcja zwracająca tablicę pixeli dla pliku z 32-bitową głębią kolorów
 * @param filePointer wskaźnik na plik obrazu
 * @param pixelCount licznik pixeli w obrazie
 * @return tablica pixeli rozbita na poszczególne składowe
 */
uint8_t *pixelArray_read_32bit(FILE *filePointer, int pixelCount);

/**
 * funkcja dzieląca tablicę bajtów odczytanych z pliku wejściowego na paczki
 * bitów na osobnych bajtach
 * @param dataTab tablica z odczytanymi bajtami z pliku wejściowego
 * @param bitsPerPixel na ilu bitach ma być zapisana informacja
 * @param oldArrSize wielkość starej tablicy bajtów
 * @param newArrSize nowa wielkość końcowej tablicy z danymi
 * @return końcowa tablica danych
 */
uint8_t *prepare_data_to_write(uint8_t *dataTab, int bitsPerPixel,
                               int oldArrSize, int *newArrSize);

/**
 * funkcja zapisująca nagłówki do pliku
 * @param bmpHeader nagłówek BMPHEADER
 * @param dibHeader nagłówek DIBHEADER
 * @param filePointer wskaźnik na plik wynikowy
 * @return 1 jeżeli wykonano poprawnie, 0 jeżeli wystąpił błąd
 */
int write_headers(BMPHEADER bmpHeader, DIBHEADER dibHeader, FILE *filePointer, BITFIELDS bitFields);

/**
 * funkcja zapisująca tablicę pixeli z podmienionymi bitami do pliku wynikowego
 * dla obrazów 16-bitowych
 * @param dataTab tablica podzielonych danych
 * @param pixelArray tablica pixeli
 * @param header nagłówek BMPHEADER
 * @param dibHeader nagłówek DIBHEADER
 * @param filePointer wskaźnik na plik wynikowy
 * @param pixels liczba pixeli w obrazie
 * @param arrSize wielkość tablicy danych
 * @param threadsCount liczba wątków na których jest uruchamiany algorytm
 * @param algoSelection wybór biblioteki (C lub ASM), 1 to C, 0 to ASM
 * @return 1 jeżeli funkcja wykonała się poprawnie, 0 w przeciwnym wypadku
 */
int write_data_16bit(uint8_t *dataTab, uint8_t *pixelArray, BMPHEADER header,
                     DIBHEADER dibHeader, FILE *filePointer,
                     int pixels, int arrSize, BITFIELDS bitFields, int threadsCount, int algoSelection);

/**
 * funkcja zapisująca tablicę pixeli z podmienionymi bitami do pliku wynikowego
 * dla obrazów 24-bitowych
 * @param dataTab tablica podzielonych danych
 * @param pixelArray tablica pixeli
 * @param header nagłówek BMPHEADER
 * @param dibHeader nagłówek DIBHEADER
 * @param filePointer wskaźnik na plik wynikowy
 * @param pixels liczba pixeli w obrazie
 * @param arrSize wielkość tablicy danych
 * @param threadsCount liczba wątków na których jest uruchamiany algorytm
 * @param algoSelection wybór biblioteki (C lub ASM), 1 to C, 0 to ASM
 * @return 1 jeżeli funkcja wykonała się poprawnie, 0 w przeciwnym wypadku
 */
int write_data_24bit(uint8_t *dataTab, uint8_t *pixelArray, BMPHEADER header,
                     DIBHEADER dibHeader, FILE *filePointer,
                     int pixels, int arrSize, int bitsPerPixel, BITFIELDS bitFields, int threadsCount, int algoSelection);

/**
 * funkcja zapisująca tablicę pixeli z podmienionymi bitami do pliku wynikowego
 * dla obrazów 32-bitowych
 * @param dataTab tablica podzielonych danych
 * @param pixelArray tablica pixeli
 * @param header nagłówek BMPHEADER
 * @param dibHeader nagłówek DIBHEADER
 * @param filePointer wskaźnik na plik wynikowy
 * @param pixels liczba pixeli w obrazie
 * @param arrSize wielkość tablicy danych
 * @param threadsCount liczba wątków na których jest uruchamiany algorytm
 * @param algoSelection wybór biblioteki (C lub ASM), 1 to C, 0 to ASM
 * @return 1 jeżeli funkcja wykonała się poprawnie, 0 w przeciwnym wypadku
 */
int write_data_32bit(uint8_t *dataTab, uint8_t *pixelArray, BMPHEADER header,
                     DIBHEADER dibHeader, FILE *filePointer,
                     int pixels, int arrSize, int bitsPerPixel, BITFIELDS bitFields, int threadsCount, int algoSelection);

/**
 * funkcja sprawdzająca, czy w nagłówku podanego pliku występuje nagłówek steganograficzny
 * @param bmpHeader nagłówek BMPHEADER
 * @return 1 jeżeli prawda, 0 jeżeli fałsz
 */
int check_for_stegano(BMPHEADER bmpHeader);

/**
 * funkcja wczytująca plik bajt po bajcie do pamięci
 * @param filePointer wskaźnik na plik
 * @param filesize wielkość pliku
 * @param arrSize wielkość tablicy z bajtami
 * @return tablica bajtów z plikiem wejściowym
 */
uint8_t *read_file_to_memory(FILE *filePointer, int *filesize, int *arrSize);

/**
 * funkcja wrapper służąca do wpisywania danych z pliku do obrazu
 * @param pixelArray tablica pixeli z obrazu wejściowego
 * @param dataArray tablica bajtów pliku wejściowego
 * @param bmpHeader nagłówek BMPHEADER obrazu wejściowego
 * @param dibHeader nagłówek DIBHEADER obrazu wejściowego
 * @param filePointer wskaźnik na plik obrazu wyjściowego
 * @param dataFileSize wielkość pliku wejściowego
 * @param bitsPerPixelStegano na ilu bitach ma zostać zapisana informacja
 * @param bitsPerPixel głębia kolorów obrazu
 * @param pixels liczba pixeli
 * @param arrSize wielkośc tablicy bajtów pliku wejściowego
 * @param threadsCount liczba wątków na których jest uruchamiany algorytm
 * @param algoSelection wybór biblioteki (C lub ASM), 1 to C, 0 to ASM
 */
int write_data_to_image(uint8_t *pixelArray, uint8_t *dataArray,
                         BMPHEADER bmpHeader, DIBHEADER dibHeader,
                         FILE *filePointer,
                         uint16_t dataFileSize, int bitsPerPixelStegano,
                         int bitsPerPixel, int pixels, int arrSize, BITFIELDS bitFields, int threadsCount, int algoSelection);

/**
 * funkcja odczytująca ukryte dane z obrazów 24 i 32-bitowych
 * @param pixelArray tablica pixeli obrazu wejściowego
 * @param bmpheader nagłówek BMPHEADER obrazu wejściowego
 * @param bitsPerPixel liczba bitów na których została zapisana informacja
 * @return tablica bajtów danych odczytanych z obrazu
 */
uint8_t *read_data_from_steganofile_24_32bit(uint8_t *pixelArray,
                                             BMPHEADER bmpheader,
                                             int bitsPerPixel, int algoSelection, int threadsCount);

/**
 * funkcja odczytująca ukryte dane z obrazów 16-bitowych
 * @param pixelArray tablica pixeli obrazu wejściowego
 * @param bmpheader nagłówek BMPHEADER obrazu wejściowego
 * @param bitsPerPixel liczba bitów na których została zapisana informacja
 * @return tablica bajtów danych odczytanych z obrazu
 */
uint8_t *read_data_from_steganofile_16bit(uint8_t *pixelArray,
                                          BMPHEADER bmpheader,
                                          int bitsPerPixel);

/**
 * funkcja zapisująca dane z tablicy bajtów do pliku wynikowego
 * @param dataArray tablica bajtów odczytanych z obrazu
 * @param dataSize wielkośc tablicy bajtów
 * @param filePointer wskaźnik na plik wynikowy
 * @return 1 jeżeli funkcja wykonała się poprwanie, 0 w przeciwnym wypadku
 */
int write_data_array_to_file(uint8_t *dataArray, int dataSize,
                             FILE *filePointer);

/**
 * funkcja wrapper zapisująca ukryte dane z przekazanej tablicy pixeli do pliku
 * @param bmpHeader nagłówek BMPHEADER
 * @param pixelArray tablica pixeli obrazu wejściowego
 * @param filePointer wskaźnik na plik wynikowy
 * @param dibHeader nagłówek DIBHEADER
 * @param bitsPerPixel liczba bitów na których zostały zapisane dane
 * @param threadsCount liczba wątków na których jest uruchamiany algorytm
 * @param algoSelection wybór biblioteki (C lub ASM), 1 to C, 0 to ASM
 * @return
 */
int write_data_from_image(BMPHEADER bmpHeader, uint8_t *pixelArray,
                          FILE *filePointer, DIBHEADER dibHeader,
                          int bitsPerPixel, int algoSelection, int threadsCount);



#endif //PROJEKT_LOGIC_H
