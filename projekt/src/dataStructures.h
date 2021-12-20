//
// Created by nixen on 25.11.2021.
//

#ifndef PROJEKT_DATASTRUCTURES_H
#define PROJEKT_DATASTRUCTURES_H

#include <stdint.h>

/**
 * struktura zawierająca dane ze standardowego nagłówka pliku BMP
 *
 * */
typedef struct _BMPHEADERDUMMY{
    uint16_t HEADER;
    uint32_t SIZE;
    uint16_t RESERVED1;
    uint16_t RESERVED2;
    uint32_t OFFSET;
} BMPHEADERDUMMY;

typedef struct _BMPHEADER {
    char *HEADER;  /**< nagłówek pliku, rozpoznaje typ pliku, jeżeli nie jest
                      "BM",  nie jest plikiem BMP standardu Windows NT */
    uint32_t SIZE; /**< wielkość pliku BMP w bajtach */
    char *RESERVED1; /**< pamięć zarezerwowana, używane do zapisania informacji
                        o ukrytym pliku (SG) */
    uint16_t RESERVED2; /**< pamięć zarezerwowana, używana do zapisywania
                           wielkości ukrytego pliku */
    uint32_t
            OFFSET; /**< offset pliku pod którym można znaleźć tablicę pixeli */
} BMPHEADER;

/**
 * struktura zawierająca podstawowy nagłówek w standardzie DIBHEADER
 * BITMAPINFOHEADER dla Windows
 *
 * */
typedef struct _DIBHEADER {
    uint32_t DIBSIZE;     /**< wielkość nagłówka w bajtach */
    uint32_t WIDTH;       /**< szerokość obrazu w pixelach (ze znakiem) */
    uint32_t HEIGHT;      /**< wysokość obrazu w pixelach (ze znakiem) */
    uint16_t COLORPLANES; /**< tablica koloru */
    uint16_t BITSPERPIXEL; /**< głębia kolorów, ile bitów przypada na pixel */
    uint32_t COMPRESSION; /**< kompresja, rodzaj algorytmu */
    uint32_t
            SIZE; /**< wielkość pliku, dopuszczajna wartość "dummy" wynosząca 0 */
    uint32_t HORIZONTALRES; /**< rozdzielczość na wysokość */
    uint32_t VERTICALRES;   /**< rozdzielczość na szerokość */
    uint32_t COLORS; /**< liczba kolorów w palecie, 0 dla wartości domyślnej
                    \f$2^{n}\f$ */
    uint32_t IMPORTANTCOLORS; /**< "ważne kolory", wartość zwykle pomijana, 0
                            dla wszystkich kolorów jako ważnych */
} DIBHEADER;

/**
 * struktura opisująca nagłówek specyficzny dla pliku z ukrytym plikiem,
 * zapisywana zawsze na pierwszych 72 pojedynczych bitach tablicy pixeli
 *
 */
typedef struct _SGHEADER {
    char *HEADER; /**< nagłówek steganograficzny, jeżeli jest równy "SG\0", plik
                     zawiera ukryte informacje (wielkość: 2 bajty) */
    uint16_t FILESIZE; /**< wielkość pliku (1 bajt) */
} SGHEADER;

/**
 * struktura zawierająca maski BITFIELDS
 */
typedef struct _BITFILEDS {
    uint32_t redMask;   /**< maska koloru czerwonego */
    uint32_t greenMask;   /**< maska koloru zielonego */
    uint32_t blueMask;   /**< maska koloru niebieskiego */
    uint32_t xMask;   /**< maska zastępcza */
    uint32_t alphaMask;   /**< maska przeźroczystości */
} BITFIELDS;

#endif //PROJEKT_DATASTRUCTURES_H
