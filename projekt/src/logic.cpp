//
// Created by nixen on 22.11.2021.
//
/**
 * @file functions.c
 * @author Wojciech Janota
 * @brief  implemenctacje funkcji
 * @version 0.1
 * @date 2020-05-01
 *
 * @copyright Copyright (c) Wojciech Janota 2020
 *
 */

#include "logic.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <vector>
#include <iostream>
#include <dlfcn.h>

// extern "C" void _apply_data_modifications_asm(uint8_t *, uint8_t *, int, int);


/**
 * pomocnicza funkcja NWD
 * @param a pierwsza liczba
 * @param b druga liczba
 * @return NWD
 */
int GCD(int a, int b) {
    if (a == 0) return b;
    return GCD(b % a, a);
}

/**
 * pomocnicza funkcja NWW
 * @param a pierwsza liczba
 * @param b druga liczba
 * @return NWW
 */
int LCM(int a, int b) { return a * b / GCD(a, b); }

int read_headers(FILE *filePointer, BMPHEADER *bmpHeader,
                 DIBHEADER *dibHeader, BITFIELDS * bitFields) {
    char *header = (char *)malloc(3 * sizeof(char));
    int size;
    char *reserved1 = (char *)malloc(3 * sizeof(char));
    uint16_t reserved2;
    int offset;

    int dibsize;
    int width;
    int height;
    int colorplanes;
    int bitsppixel;
    int compression;
    int imagesize;
    int horizontalres;
    int verticalres;
    int colors;
    int importantcolors;
    /* MAIN */
    header[2] = '\0';
    /*
     * odczytanie nagłówka pliku i sprawdzenie czy jest plikiem BMP
     */
    int tmp = fread(header, sizeof(char), 2, filePointer);
    if (tmp != 2) {
        printf("Błąd, nie można odczytać pierwszych dwóch bajtów!\n");
        return 0;
    }
    if (strcmp(header, "BM")) {
        printf("Błędna sygnatura, plik nie jest plikiem BMP!\n");
        return 0;
    }
    /*
     * odczytanie i zapisanie pierwszego nagłówka pliku BMP
     */
    reserved1[2] = '\0';
    fread(&size, sizeof(uint32_t), 1, filePointer);
    fread(reserved1, sizeof(char), 2, filePointer);
    fread(&reserved2, sizeof(uint16_t), 1, filePointer);
    fread(&offset, sizeof(uint32_t), 1, filePointer);

    bmpHeader->HEADER = header;
    bmpHeader->OFFSET = offset;
    bmpHeader->RESERVED1 = reserved1;
    bmpHeader->RESERVED2 = reserved2;
    bmpHeader->SIZE = size;
    /*
     * odczytanie i zapisanie do zmiennych nagłówka DIB
     * */
    fread(&dibsize, sizeof(uint32_t), 1, filePointer);
    fread(&width, sizeof(uint32_t), 1, filePointer);
    fread(&height, sizeof(uint32_t), 1, filePointer);
    fread(&colorplanes, sizeof(uint16_t), 1, filePointer);
    fread(&bitsppixel, sizeof(uint16_t), 1, filePointer);
    fread(&compression, sizeof(uint32_t), 1, filePointer);
    fread(&imagesize, sizeof(uint32_t), 1, filePointer);
    fread(&horizontalres, sizeof(uint32_t), 1, filePointer);
    fread(&verticalres, sizeof(uint32_t), 1, filePointer);
    fread(&colors, sizeof(uint32_t), 1, filePointer);
    fread(&importantcolors, sizeof(uint32_t), 1, filePointer);

    dibHeader->DIBSIZE = dibsize;
    dibHeader->WIDTH = width;
    dibHeader->HEIGHT = height;
    dibHeader->COLORPLANES = colorplanes;
    dibHeader->BITSPERPIXEL = bitsppixel;
    dibHeader->COMPRESSION = compression;
    dibHeader->SIZE = imagesize;
    dibHeader->HORIZONTALRES = horizontalres;
    dibHeader->VERTICALRES = verticalres;
    dibHeader->COLORS = colors;
    dibHeader->IMPORTANTCOLORS = importantcolors;
    /*
     * workaround dla głębi kolorów 32, odczytywanie masek bitowych kolorów
     */
    if (dibHeader->BITSPERPIXEL == 32) {
        uint32_t redMask;
        uint32_t greenMask;
        uint32_t blueMask;
        uint32_t alphaMask;
        fread(&redMask, sizeof(uint32_t), 1, filePointer);
        fread(&greenMask, sizeof(uint32_t), 1, filePointer);
        fread(&blueMask, sizeof(uint32_t), 1, filePointer);
        fread(&alphaMask, sizeof(uint32_t), 1, filePointer);
        bitFields->redMask = redMask;
        bitFields->greenMask = greenMask;
        bitFields->blueMask = blueMask;
        bitFields->alphaMask = alphaMask;
    }
    fseek(filePointer, SEEK_SET, offset);
    return 1;
}

uint8_t *pixelArray_read_16bit(FILE *filePointer, int pixelCount) {
    uint8_t *pixelArray = (uint8_t *)malloc(2 * pixelCount * sizeof(uint8_t));
    if(pixelArray==0){
        printf("Błąd alokacji pamięci tablicy pixeli!\n");
        return 0;
    }
    uint8_t tempARGB;
    int i = 0;
    /*
     * odczyt poszczególnych pixeli
     */
    if (filePointer) {
        for (i = 0; i < 2 * pixelCount; i++) {
            fread(&tempARGB, sizeof(uint8_t), 1, filePointer);
            pixelArray[i] = tempARGB;
        }
    } else {
        return 0;
    }
    return pixelArray;
}

uint8_t *pixelArray_read_24bit(FILE *filePointer, int pixelCount) {
    uint8_t *pixelArray = (uint8_t *)malloc(3 * pixelCount * sizeof(uint8_t));
    if(pixelArray==0){
        printf("Błąd alokacji pamięci tablicy pixeli!\n");
        return 0;
    }
    uint8_t temp;
    int i = 0;
    /*
     * odczyt poszczególnych pixeli
     */
    if (filePointer) {
        for (i = 0; i < 3 * pixelCount; i++) {
            fread(&temp, sizeof(uint8_t), 1, filePointer);
            pixelArray[i] = temp;
        }
    } else {
        return 0;
    }
    return pixelArray;
}

uint8_t *pixelArray_read_32bit(FILE *filePointer, int pixelCount) {
    uint8_t *pixelArray = (uint8_t *)malloc(4 * pixelCount * sizeof(uint8_t));
    if(pixelArray==0){
        printf("Błąd alokacji pamięci tablicy pixeli!\n");
        return 0;
    }
    /*
     * odczyt poszczególnych pixeli
     */
    if (filePointer) {
        int i = 0;
        for (i = 0; i < 4 * pixelCount; i++) {
            fread(pixelArray + i, sizeof(uint8_t), 1, filePointer);
        }
    } else {
        return 0;
    }
    return pixelArray;
}

uint8_t *read_file_to_memory(FILE *filePointer, int *filesize, int *arrsize) {
    int byteCount = 0;
    uint8_t *byteTab;
    if (!filePointer) {
        return 0;
    }
    /*
     * obliczenie wielkości pliku
     */
    fseek(filePointer, 0, SEEK_END);
    *filesize = ftell(filePointer);
    fseek(filePointer, 0, SEEK_SET);
    *filesize % 8 ? (byteCount = *filesize / 8)
                  : (byteCount = (*filesize / 8) + 1);
    byteTab = (uint8_t *)malloc(*filesize * sizeof(uint8_t));

    fread(byteTab, sizeof(uint8_t), *filesize, filePointer);
    *arrsize = *filesize;

    return byteTab;
}

SGHEADER header_generate(uint32_t filesize) {
    /*
     * wygenerowanie nagłówka steganograficznego
     */
    SGHEADER header;
    header.HEADER = "SG";
    header.FILESIZE = filesize;
    return header;
}

uint8_t *prepare_data_to_write(uint8_t *dataTab, int bitsPerPixel,
                               int oldArrSize, int *newArrSize) {
    uint8_t *dividedTable =
            (uint8_t *)malloc(sizeof(uint8_t) * oldArrSize * 8 / bitsPerPixel);
    if(dividedTable==0){
        printf("Błąd alokacji pamięci dla tablicy!\n");
        return 0;
    }
    /*
     * podzielenie tablicy bajtów na mniejsze porcje po tyle bitów na bajt, ile podał użytkownik
     */
    uint8_t mask = 0;
    int i = 0;
    for (i = 0; i < (oldArrSize * 8 / bitsPerPixel); i++) {
        if (mask == 0){
            /*
             * resetowanie maski
             */
            mask = (1 << bitsPerPixel) - 1;
        }
        uint8_t help = 0;
        /*
         * dzielenie na "paczki"
         */
        help = dataTab[i / (8 / bitsPerPixel)] & mask;
        help = help >> ((i % (8 / bitsPerPixel)) * bitsPerPixel);

        dividedTable[i] = help;
        mask = mask << bitsPerPixel;
    }
    *newArrSize = oldArrSize * 8 / bitsPerPixel;
    return dividedTable;
}

BMPHEADER prepare_new_header(BMPHEADER bmpHeader,
                             SGHEADER sgHeader,
                             DIBHEADER dibHeader, int newSize) {
    BMPHEADER newBmpHeader;
    newBmpHeader.HEADER = bmpHeader.HEADER;
    newBmpHeader.OFFSET = bmpHeader.OFFSET;
    newBmpHeader.SIZE = newSize;
    newBmpHeader.RESERVED1 = sgHeader.HEADER;
    newBmpHeader.RESERVED2 = sgHeader.FILESIZE;
    return newBmpHeader;
}

int write_headers(BMPHEADER bmpHeader, DIBHEADER dibHeader,
                  FILE *filePointer, BITFIELDS bitFields) {
    if (!filePointer) {
        return 0;
    }
    /*
     * aby poprawnie obliczyć wielkość nagłówków
     */
    BMPHEADERDUMMY bmpHeaderDummy;
    uint32_t offset = 54;
    fseek(filePointer, 0, SEEK_SET);
    /*
     * zapisanie BMPHEADER
     */
    fwrite(bmpHeader.HEADER, sizeof(char), 2, filePointer);
    fwrite(&bmpHeader.SIZE, sizeof(uint32_t), 1, filePointer);
    fwrite(bmpHeader.RESERVED1, sizeof(char), 2, filePointer);
    fwrite(&bmpHeader.RESERVED2, sizeof(uint16_t), 1, filePointer);
    if (dibHeader.BITSPERPIXEL == 32) {
        /*
         * dla 32-bit dodajemy maski kolorów
         */
        uint32_t offset2 =
                54 + 4 * sizeof(uint32_t);
        fwrite(&offset2, sizeof(uint32_t), 1, filePointer);
    } else {
        fwrite(&offset, sizeof(uint32_t), 1, filePointer);
    }
    /*
     * zapisanie DIBHEADER
     */
    fwrite(&dibHeader.DIBSIZE, sizeof(uint32_t), 1, filePointer);
    fwrite(&dibHeader.WIDTH, sizeof(uint32_t), 1, filePointer);
    fwrite(&dibHeader.HEIGHT, sizeof(uint32_t), 1, filePointer);
    fwrite(&dibHeader.COLORPLANES, sizeof(uint16_t), 1, filePointer);
    fwrite(&dibHeader.BITSPERPIXEL, sizeof(uint16_t), 1, filePointer);
    if (dibHeader.BITSPERPIXEL == 16) {
        uint32_t dummy = 0;
        fwrite(&dummy, sizeof(uint32_t), 1, filePointer);
    } else {
        fwrite(&dibHeader.COMPRESSION, sizeof(uint32_t), 1, filePointer);
    }
    fwrite(&dibHeader.SIZE, sizeof(uint32_t), 1, filePointer);
    fwrite(&dibHeader.HORIZONTALRES, sizeof(uint32_t), 1, filePointer);
    fwrite(&dibHeader.VERTICALRES, sizeof(uint32_t), 1, filePointer);
    fwrite(&dibHeader.COLORS, sizeof(uint32_t), 1, filePointer);
    fwrite(&dibHeader.IMPORTANTCOLORS, sizeof(uint32_t), 1, filePointer);
    /*
     * maski kolorów dla 32-bitowych obrazów
     */
    if (dibHeader.BITSPERPIXEL == 32) {
        /*uint32_t greenMask = 65280;
        uint32_t redMask = 16711680;
        uint32_t xMask = 0;
        uint32_t blueMask = 255;
        fwrite(&redMask, sizeof(uint32_t), 1, filePointer);
        fwrite(&greenMask, sizeof(uint32_t), 1, filePointer);
        fwrite(&blueMask, sizeof(uint32_t), 1, filePointer);
        fwrite(&xMask, sizeof(uint32_t), 1, filePointer);*/
        fwrite(&bitFields.redMask, sizeof(uint32_t), 1, filePointer);
        fwrite(&bitFields.greenMask, sizeof(uint32_t), 1, filePointer);
        fwrite(&bitFields.blueMask, sizeof(uint32_t), 1, filePointer);
        fwrite(&bitFields.alphaMask, sizeof(uint32_t), 1, filePointer);
    }
    return 1;
}

void apply_data_modifications_16bit(uint8_t *pixelArray, uint8_t *dataArray, uint8_t mask, int arrSize) {
    int i = 0;
    for (i = 0; i < arrSize; i++) {
        /*
         * zapisanie i maskowanie
         */
        if (i % 2 == 0) {
            pixelArray[i] = (pixelArray[i] & (~1)) | dataArray[i];
        } else {
            uint8_t temp = dataArray[i] << 2;
            pixelArray[i] = (pixelArray[i] & (~4)) | temp;
        }
    }
}

int write_data_32bit(uint8_t *dataTab, uint8_t *pixelArray,
                     BMPHEADER header, DIBHEADER dibHeader,
                     FILE *filePointer, int pixels, int arrSize,
                     int bitsPerPixel, BITFIELDS bitFields, int threadsCount, int algoSelection) {
    /*
     * sprawdzenie, czy plik zmieści się w obrazie, jest podzielony na paczki, więc jest to poprawne sprawdzenie
     */
    if (arrSize > (4 * pixels)) {
        printf(
                "Błąd, zbyt dużo danych do zapisania!\n");
        return 0;
    }
    
    typedef void (*shared_apply_data_function)(uint8_t *, uint8_t *, int, int);

    std::string libraryPath;
    std::string sharedFunctionName;

    if (algoSelection) {
        libraryPath = "../c_src/libdata_modifications_c.so";
        sharedFunctionName = "_apply_data_modifications";
    } else {
        libraryPath = "../asm_src/libdata_modifications_asm.so";
        sharedFunctionName = "_apply_data_modifications_asm";
    }

    void *shared_library = dlopen(libraryPath.c_str(), RTLD_NOW);
    shared_apply_data_function _apply_data_modifications = (shared_apply_data_function) dlsym(shared_library, sharedFunctionName.c_str());

    int bytesPerLine = dibHeader.WIDTH * 4; /* (for 32 bit images) */
    int i = 0;
    uint8_t mask = (1 << bitsPerPixel) - 1;
    int blockSize = 0;
    int lastDataBlockSize = 0;
    int pixelArrayBlockCount = 0;
    int lastPixelArrayBlockSize = 0;
    if (arrSize%threadsCount) {
        blockSize = arrSize / (threadsCount - 1);
        lastDataBlockSize = arrSize%(threadsCount - 1);
    } else {
        blockSize = arrSize / threadsCount;
    }
    pixelArrayBlockCount = ( 4 * pixels ) / blockSize;
    if ((4 * pixels)%blockSize) {
        pixelArrayBlockCount++;
        lastPixelArrayBlockSize = (4*pixels) % blockSize;
    }
    uint8_t **tempPixelArray;
    uint8_t **tempDataArray;
    tempPixelArray = (uint8_t **) malloc(pixelArrayBlockCount * sizeof(uint8_t *));
    tempDataArray = (uint8_t **) malloc(threadsCount * sizeof(uint8_t *));
    for (i=0; i<pixelArrayBlockCount - 1; i++) {
        tempPixelArray[i] = (uint8_t *) malloc(blockSize * sizeof(uint8_t));
    }
    if (lastPixelArrayBlockSize) {
        tempPixelArray[pixelArrayBlockCount - 1] = (uint8_t *) malloc(lastPixelArrayBlockSize * sizeof(uint8_t));
    } else {
        tempPixelArray[pixelArrayBlockCount - 1] = (uint8_t *) malloc(blockSize * sizeof(uint8_t));
    }
    for (i=0; i<threadsCount - 1; i++) {
        tempDataArray[i] = (uint8_t *) malloc(blockSize * sizeof(uint8_t));
    }
    if (lastDataBlockSize) {
        tempDataArray[threadsCount-1] = (uint8_t *) malloc(lastDataBlockSize * sizeof(uint8_t));
    } else {
        tempDataArray[threadsCount-1] = (uint8_t *) malloc(blockSize * sizeof(uint8_t));
    }
    for (i=0; i< pixelArrayBlockCount - 1; i++) {
        for (int j = 0; j < blockSize; j++) {
            tempPixelArray[i][j] = pixelArray[i*blockSize + j];
        }
    }
    if (lastPixelArrayBlockSize) {
        for (int j = 0; j < lastDataBlockSize; j++) {
            tempPixelArray[pixelArrayBlockCount-1][j] = pixelArray[(pixelArrayBlockCount-1)*blockSize + j];
        }
    } else {
        for (int j = 0; j < blockSize; j++) {
            tempPixelArray[pixelArrayBlockCount-1][j] = pixelArray[(pixelArrayBlockCount-1)*blockSize + j];
        }
    }
    for (i = 0; i < threadsCount - 1; i++) {
        for (int j = 0; j < blockSize; j++) {
            tempDataArray[i][j] = dataTab[i * blockSize + j];
        }
    }
    if (lastDataBlockSize) {
        for (int j = 0; j < lastDataBlockSize; j++) {
            tempDataArray[threadsCount - 1][j] = dataTab[(threadsCount - 1) * blockSize + j];
        }
    } else {
        for (int j = 0; j < blockSize; j++) {
            tempDataArray[threadsCount - 1][j] = dataTab[(threadsCount - 1) * blockSize + j];
        }
    }
    std::vector <std::thread> threads;
    for (i = 0; i < threadsCount - 1; i++) {
        threads.push_back(std::thread(_apply_data_modifications, tempPixelArray[i], tempDataArray[i], mask, blockSize));
        // apply_data_modifications(tempPixelArray[i], tempDataArray[i], mask, blockSize);
        // _apply_data_modifications(tempPixelArray[i], tempDataArray[i], mask, blockSize);
    } 
    if (lastDataBlockSize){
        threads.push_back(std::thread(_apply_data_modifications, tempPixelArray[threadsCount - 1], tempDataArray[threadsCount - 1], mask, lastDataBlockSize));
        // apply_data_modifications(tempPixelArray[threadsCount - 1], tempDataArray[threadsCount - 1], mask, lastDataBlockSize);
        // _apply_data_modifications(tempPixelArray[threadsCount - 1], tempDataArray[threadsCount - 1], mask, lastDataBlockSize);
    }
    else {
        threads.push_back(std::thread(_apply_data_modifications, tempPixelArray[threadsCount - 1], tempDataArray[threadsCount - 1], mask, blockSize));
        // apply_data_modifications(tempPixelArray[threadsCount - 1], tempDataArray[threadsCount - 1], mask, blockSize);
        // _apply_data_modifications(tempPixelArray[threadsCount - 1], tempDataArray[threadsCount - 1], mask, blockSize);

    }

    for (i = 0; i < threads.size(); i++) {
        threads[i].join();
    }

    uint8_t firstByteMitigation = (pixelArray[0] & (~mask)) | dataTab[0];
    for (i = 0; i < threadsCount - 1; i++) {
        for (int j = 0; j < blockSize; j++) {
            pixelArray[i*blockSize + j] = tempPixelArray[i][j];
        }
    }
    if (lastDataBlockSize) {
        for (int j = 0; j < lastDataBlockSize; j++) {
            pixelArray[(threadsCount - 1) * blockSize + j] = tempPixelArray[threadsCount - 1][j];
        }
    } else {
        for (int j = 0; j < blockSize; j++) {
            pixelArray[(threadsCount - 1) * blockSize + j] = tempPixelArray[threadsCount - 1][j];
        }
    }
    for (i=0; i< pixelArrayBlockCount; i++) {
        free(tempPixelArray[i]);
    }
    free(tempPixelArray);
    for (i=0; i<threadsCount; i++){
        free(tempDataArray[i]);
    }
    free(tempDataArray);
    pixelArray[0] = firstByteMitigation;
    /*
    for (i = 0; i < (arrSize); i++) {
        pixelArray[i] = (pixelArray[i] & (~mask)) | dataTab[i];
    }*/
    if(write_headers(header, dibHeader, filePointer, bitFields)){
        printf("Zapisywanie nagłówków do pliku...\n");
    }
    else{
        printf("Błąd zapisu nagłówków do pliku!\n");
        return 0;
    }
    uint8_t *linebuf;
    linebuf = (uint8_t *)malloc(bytesPerLine * sizeof(uint8_t));
    if (linebuf == NULL) {
        printf("Błąd alokacji pamięci bufora!\n");
        return 0;
    }
    unsigned int line = 0;
    /*
     * zapis linia po linii, aby zachować poprawną liczbę bajtów w linii
     */
    for (line = 0; line < dibHeader.HEIGHT; line++) {
        for (i = 0; i < bytesPerLine; i++) {
            uint8_t temp = pixelArray[line * bytesPerLine + i];
            *(linebuf + i) = temp;
        }
        fwrite(linebuf, sizeof(uint8_t), bytesPerLine, filePointer);
    }
    free(linebuf);
    if (shared_library) {
        dlclose(shared_library);
    }
    return 1;
}
int write_data_24bit(uint8_t *dataTab, uint8_t *pixelArray,
                     BMPHEADER header, DIBHEADER dibHeader,
                     FILE *filePointer, int pixels, int arrSize,
                     int bitsPerPixel, BITFIELDS bitFields, int threadsCount, int algoSelection) {
    /*
     * sprawdzenie, czy plik zmieści się w obrqzie, jest podzielony na paczki, więc jest to poprawne sprawdzenie
     */
    if (arrSize > 3 * pixels) {
        printf(
                "Błąd, zbyt dużo danych do zapisania!\n");
        return 0;
    }
    typedef void (*shared_apply_data_function)(uint8_t *, uint8_t *, int, int);

    std::string libraryPath;
    std::string sharedFunctionName;

    if (algoSelection) {
        libraryPath = "../c_src/libdata_modifications_c.so";
        sharedFunctionName = "_apply_data_modifications";
    } else {
        libraryPath = "../asm_src/libdata_modifications_asm.so";
        sharedFunctionName = "_apply_data_modifications_asm";
    }

    void *shared_library = dlopen(libraryPath.c_str(), RTLD_NOW);
    shared_apply_data_function _apply_data_modifications = (shared_apply_data_function) dlsym(shared_library, sharedFunctionName.c_str());

    uint8_t mask = (1 << bitsPerPixel) - 1;
    int i = 0;
    int blockSize = 0;
    int lastDataBlockSize = 0;
    int pixelArrayBlockCount = 0;
    int lastPixelArrayBlockSize = 0;
    if (arrSize%threadsCount) {
        blockSize = arrSize / (threadsCount - 1);
        lastDataBlockSize = arrSize%(threadsCount - 1);
    } else {
        blockSize = arrSize / threadsCount;
    }
    pixelArrayBlockCount = ( 3 * pixels ) / blockSize;
    if ((3 * pixels)%blockSize) {
        pixelArrayBlockCount++;
        lastPixelArrayBlockSize = (3*pixels) % blockSize;
    }
    uint8_t **tempPixelArray;
    uint8_t **tempDataArray;
    tempPixelArray = (uint8_t **) malloc(pixelArrayBlockCount * sizeof(uint8_t *));
    tempDataArray = (uint8_t **) malloc(threadsCount * sizeof(uint8_t *));
    for (i=0; i<pixelArrayBlockCount - 1; i++) {
        tempPixelArray[i] = (uint8_t *) malloc(blockSize * sizeof(uint8_t));
    }
    if (lastPixelArrayBlockSize) {
        tempPixelArray[pixelArrayBlockCount - 1] = (uint8_t *) malloc(lastPixelArrayBlockSize * sizeof(uint8_t));
    } else {
        tempPixelArray[pixelArrayBlockCount - 1] = (uint8_t *) malloc(blockSize * sizeof(uint8_t));
    }
    for (i=0; i<threadsCount - 1; i++) {
        tempDataArray[i] = (uint8_t *) malloc(blockSize * sizeof(uint8_t));
    }
    if (lastDataBlockSize) {
        tempDataArray[threadsCount-1] = (uint8_t *) malloc(lastDataBlockSize * sizeof(uint8_t));
    } else {
        tempDataArray[threadsCount-1] = (uint8_t *) malloc(blockSize * sizeof(uint8_t));
    }
    for (i=0; i< pixelArrayBlockCount - 1; i++) {
        for (int j = 0; j < blockSize; j++) {
            tempPixelArray[i][j] = pixelArray[i*blockSize + j];
        }
    }
    if (lastPixelArrayBlockSize) {
        for (int j = 0; j < lastDataBlockSize; j++) {
            tempPixelArray[pixelArrayBlockCount-1][j] = pixelArray[(pixelArrayBlockCount-1)*blockSize + j];
        }
    } else {
        for (int j = 0; j < blockSize; j++) {
            tempPixelArray[pixelArrayBlockCount-1][j] = pixelArray[(pixelArrayBlockCount-1)*blockSize + j];
        }
    }
    for (i = 0; i < threadsCount - 1; i++) {
        for (int j = 0; j < blockSize; j++) {
            tempDataArray[i][j] = dataTab[i * blockSize + j];
        }
    }
    if (lastDataBlockSize) {
        for (int j = 0; j < lastDataBlockSize; j++) {
            tempDataArray[threadsCount - 1][j] = dataTab[(threadsCount - 1) * blockSize + j];
        }
    } else {
        for (int j = 0; j < blockSize; j++) {
            tempDataArray[threadsCount - 1][j] = dataTab[(threadsCount - 1) * blockSize + j];
        }
    }
    std::vector <std::thread> threads;
    for (i = 0; i < threadsCount - 1; i++) {
        threads.push_back(std::thread(_apply_data_modifications, tempPixelArray[i], tempDataArray[i], mask, blockSize));
        // apply_data_modifications(tempPixelArray[i], tempDataArray[i], mask, blockSize);
    } 
    if (lastDataBlockSize){
        threads.push_back(std::thread(_apply_data_modifications, tempPixelArray[threadsCount - 1], tempDataArray[threadsCount - 1], mask, lastDataBlockSize));
        // apply_data_modifications(tempPixelArray[threadsCount - 1], tempDataArray[threadsCount - 1], mask, lastDataBlockSize);
    }
    else {
        threads.push_back(std::thread(_apply_data_modifications, tempPixelArray[threadsCount - 1], tempDataArray[threadsCount - 1], mask, blockSize));
        // apply_data_modifications(tempPixelArray[threadsCount - 1], tempDataArray[threadsCount - 1], mask, blockSize);
    }

    for (i = 0; i < threads.size(); i++) {
        threads[i].join();
    }

    uint8_t firstByteMitigation = (pixelArray[0] & (~mask)) | dataTab[0];
    for (i = 0; i < threadsCount - 1; i++) {
        for (int j = 0; j < blockSize; j++) {
            pixelArray[i*blockSize + j] = tempPixelArray[i][j];
        }
    }
    if (lastDataBlockSize) {
        for (int j = 0; j < lastDataBlockSize; j++) {
            pixelArray[(threadsCount - 1) * blockSize + j] = tempPixelArray[threadsCount - 1][j];
        }
    } else {
        for (int j = 0; j < blockSize; j++) {
            pixelArray[(threadsCount - 1) * blockSize + j] = tempPixelArray[threadsCount - 1][j];
        }
    }
    pixelArray[0] = firstByteMitigation;
    /*
    for (i = 0; i < (arrSize); i++) {
        pixelArray[i] = (pixelArray[i] & (~mask)) | dataTab[i];
    }*/
    if(write_headers(header, dibHeader, filePointer, bitFields)){
        printf("Zapisywanie nagłówków do pliku...\n");
    }
    else{
        printf("Błąd zapisu nagłówków do pliku!\n");
        return 0;
    }

    for (i=0; i< pixelArrayBlockCount; i++) {
        free(tempPixelArray[i]);
    }
    free(tempPixelArray);
    for (i=0; i<threadsCount; i++){
        free(tempDataArray[i]);
    }
    free(tempDataArray);
    int bytesPerLine = dibHeader.WIDTH * 3; /* (for 24 bit images) */
    if (bytesPerLine & 0x0003) {
        bytesPerLine |= 0x0003;
        ++bytesPerLine;
    }

    uint8_t *linebuf;

    linebuf = (uint8_t *)malloc(bytesPerLine * sizeof(uint8_t));
    if (linebuf == NULL) {
        printf("Błąd alokacji pamięci bufora!\n");
        return 0;
    }

    /*
     * zapis linia po linii, aby zachować poprawną liczbę bajtów w linii
     */
    unsigned int line = 0;
    for (line = 0; line < dibHeader.HEIGHT; line++) {
        for (i = 0; i < bytesPerLine; i++) {
            if (line*bytesPerLine + i < 3 * dibHeader.HEIGHT * dibHeader.WIDTH){
            uint8_t temp = pixelArray[line * bytesPerLine + i];
            *(linebuf + i) = temp;
            } else {
                *(linebuf + i) = 0;
            }
        }
        fwrite(linebuf, sizeof(uint8_t), bytesPerLine, filePointer);
    }
    free(linebuf);
    if (shared_library) {
        dlclose(shared_library);
    }
    return 1;
}

int write_data_16bit(uint8_t *dataTab, uint8_t *pixelArray,
                     BMPHEADER header, DIBHEADER dibHeader,
                     FILE *filePointer, int pixels,
                     int arrSize, BITFIELDS bitFields, int threadsCount, int algoSelection) {
    /*
     * sprawdzenie, czy plik zmieści się w obrqzie, jest podzielony na paczki, więc jest to poprawne sprawdzenie
     */
    if (arrSize > 2 * pixels) {
        printf(
                "Zbyt dużo danych do zapisania!\n");
        return 0;
    }
    int i = 0;
    if(write_headers(header, dibHeader, filePointer, bitFields)){
        printf("Zapisywanie nagłówków do pliku...\n");
    }
    else{
        printf("Błąd zapisu nagłówków do pliku!\n");
        return 0;
    }
    int bytesPerLine = dibHeader.WIDTH * 2;
    if (bytesPerLine % 4 == 2) {
        bytesPerLine += 2;
    }

    uint8_t *linebuf;

    linebuf = (uint8_t *)malloc(bytesPerLine * sizeof(uint8_t));
    if (linebuf == 0) {
        printf("Błąd alokacji pamięci bufora!\n");
        return 0;
    }

    uint8_t mask = 1;

    int blockSize = 0;
    int lastDataBlockSize = 0;
    int pixelArrayBlockCount = 0;
    int lastPixelArrayBlockSize = 0;
    if (arrSize%threadsCount) {
        blockSize = arrSize / (threadsCount - 1);
        lastDataBlockSize = arrSize%(threadsCount - 1);
    } else {
        blockSize = arrSize / threadsCount;
    }
    pixelArrayBlockCount = ( 2 * pixels ) / blockSize;
    if ((2 * pixels)%blockSize) {
        pixelArrayBlockCount++;
        lastPixelArrayBlockSize = (2*pixels) % blockSize;
    }
    uint8_t **tempPixelArray;
    uint8_t **tempDataArray;
    tempPixelArray = (uint8_t **) malloc(pixelArrayBlockCount * sizeof(uint8_t *));
    tempDataArray = (uint8_t **) malloc(threadsCount * sizeof(uint8_t *));
    for (i=0; i<pixelArrayBlockCount - 1; i++) {
        tempPixelArray[i] = (uint8_t *) malloc(blockSize * sizeof(uint8_t));
    }
    if (lastPixelArrayBlockSize) {
        tempPixelArray[pixelArrayBlockCount - 1] = (uint8_t *) malloc(lastPixelArrayBlockSize * sizeof(uint8_t));
    } else {
        tempPixelArray[pixelArrayBlockCount - 1] = (uint8_t *) malloc(blockSize * sizeof(uint8_t));
    }
    for (i=0; i<threadsCount - 1; i++) {
        tempDataArray[i] = (uint8_t *) malloc(blockSize * sizeof(uint8_t));
    }
    if (lastDataBlockSize) {
        tempDataArray[threadsCount-1] = (uint8_t *) malloc(lastDataBlockSize * sizeof(uint8_t));
    } else {
        tempDataArray[threadsCount-1] = (uint8_t *) malloc(blockSize * sizeof(uint8_t));
    }
    for (i=0; i< pixelArrayBlockCount - 1; i++) {
        for (int j = 0; j < blockSize; j++) {
            tempPixelArray[i][j] = pixelArray[i*blockSize + j];
        }
    }
    if (lastPixelArrayBlockSize) {
        for (int j = 0; j < lastDataBlockSize; j++) {
            tempPixelArray[pixelArrayBlockCount-1][j] = pixelArray[(pixelArrayBlockCount-1)*blockSize + j];
        }
    } else {
        for (int j = 0; j < blockSize; j++) {
            tempPixelArray[pixelArrayBlockCount-1][j] = pixelArray[(pixelArrayBlockCount-1)*blockSize + j];
        }
    }
    for (i = 0; i < threadsCount - 1; i++) {
        for (int j = 0; j < blockSize; j++) {
            tempDataArray[i][j] = dataTab[i * blockSize + j];
        }
    }
    if (lastDataBlockSize) {
        for (int j = 0; j < lastDataBlockSize; j++) {
            tempDataArray[threadsCount - 1][j] = dataTab[(threadsCount - 1) * blockSize + j];
        }
    } else {
        for (int j = 0; j < blockSize; j++) {
            tempDataArray[threadsCount - 1][j] = dataTab[(threadsCount - 1) * blockSize + j];
        }
    }
    std::vector <std::thread> threads;
    for (i = 0; i < threadsCount - 1; i++) {
        threads.push_back(std::thread(apply_data_modifications_16bit, tempPixelArray[i], tempDataArray[i], mask, blockSize));
        // apply_data_modifications(tempPixelArray[i], tempDataArray[i], mask, blockSize);
    } 
    if (lastDataBlockSize){
        threads.push_back(std::thread(apply_data_modifications_16bit, tempPixelArray[threadsCount - 1], tempDataArray[threadsCount - 1], mask, lastDataBlockSize));
        // apply_data_modifications(tempPixelArray[threadsCount - 1], tempDataArray[threadsCount - 1], mask, lastDataBlockSize);
    }
    else {
        threads.push_back(std::thread(apply_data_modifications_16bit, tempPixelArray[threadsCount - 1], tempDataArray[threadsCount - 1], mask, blockSize));
        // apply_data_modifications(tempPixelArray[threadsCount - 1], tempDataArray[threadsCount - 1], mask, blockSize);
    }

    for (i = 0; i < threads.size(); i++) {
        threads[i].join();
    }

    uint8_t firstByteMitigation = (pixelArray[0] & (~mask)) | dataTab[0];

    for (i = 0; i < threadsCount - 1; i++) {
        for (int j = 0; j < blockSize; j++) {
            pixelArray[i*blockSize + j] = tempPixelArray[i][j];
        }
    }
    if (lastDataBlockSize) {
        for (int j = 0; j < lastDataBlockSize; j++) {
            pixelArray[(threadsCount - 1) * blockSize + j] = tempPixelArray[threadsCount - 1][j];
        }
    } else {
        for (int j = 0; j < blockSize; j++) {
            pixelArray[(threadsCount - 1) * blockSize + j] = tempPixelArray[threadsCount - 1][j];
        }
    }

    for (i=0; i< pixelArrayBlockCount; i++) {
        free(tempPixelArray[i]);
    }
    free(tempPixelArray);
    for (i=0; i<threadsCount; i++){
        free(tempDataArray[i]);
    }
    free(tempDataArray);

    // fixes ASM bug where first byte gets corrupted for no reason
    pixelArray[0] = firstByteMitigation;

/*
    for (i = 0; i < arrSize; i++) {
        if (i % 2 == 0) {
            pixelArray[i] = (pixelArray[i] & (~1)) | dataTab[i];
        } else {
            uint8_t temp = dataTab[i] << 2;
            pixelArray[i] = (pixelArray[i] & (~4)) | temp;
        }
    }
    */
    /*
     * zapis linia po linii, aby zachować poprawną liczbę bajtów w linii
     */
    unsigned int line = 0;
    for (line = 0; line < dibHeader.HEIGHT; line++) {
        for (i = 0; i < bytesPerLine; i++) {
            uint8_t temp = pixelArray[line * bytesPerLine + i];
            *(linebuf + i) = temp;
        }
        fwrite(linebuf, sizeof(uint8_t), bytesPerLine, filePointer);
    }
    free(linebuf);
    return 1;
}

int write_data_to_image(uint8_t *pixelArray, uint8_t *dataArray,
                         BMPHEADER bmpHeader, DIBHEADER dibHeader,
                         FILE *filePointer,
                         uint16_t dataFileSize, int bitsPerPixelStegano,
                         int bitsPerPixel, int pixels, int arrSize, BITFIELDS bitFields, int threadsCount, int algoSelection) {
    if(LCM(bitsPerPixelStegano, 8)!=8){
        printf("Błąd: ilość bitów do zapisania w pliku nie może być inna niż dzielnik 8\n");
        return 0;
    }
    SGHEADER sgHeader =
            header_generate(dataFileSize);
    int newArrSize = 0;
    uint8_t *finalDataTable = prepare_data_to_write(
            dataArray, bitsPerPixelStegano, arrSize, &newArrSize);
    if(finalDataTable==0) {
        printf("Błąd alokacji pamięci!\n");
        return 0;
    }
    BMPHEADER newHeader =
            prepare_new_header(bmpHeader, sgHeader, dibHeader, bmpHeader.SIZE);
    if (bitsPerPixel == 24) {
        if(write_data_24bit(finalDataTable, pixelArray, newHeader, dibHeader,
                            filePointer, pixels, newArrSize,
                            bitsPerPixelStegano, bitFields, threadsCount, algoSelection)){
            /*int * ptr;
            *ptr=0;
            fwrite(ptr, sizeof(uint8_t), newSize - bmpHeader.SIZE, filePointer);*/
            printf("Dane zapisane poprawnie!\n");
        }
        else{
            printf("Błąd zapisu danych do pliku!\n");
            return 0;
        }
    } else if (bitsPerPixel == 16) {
        if (bitsPerPixelStegano != 1) {
            printf("Liczba bitów przeznaczonych na zapis informacji nie może być większa od 1!\n");
            return 0;
        }
        if(write_data_16bit(finalDataTable, pixelArray, newHeader, dibHeader,
                            filePointer, pixels, newArrSize, bitFields, threadsCount, algoSelection)){
            printf("Dane zapisane poprawnie!\n");
        }
        else{
            printf("Błąd zapisu danych do pliku!\n");
            return 0;
        }
    } else if (bitsPerPixel == 32) {
        if(write_data_32bit(finalDataTable, pixelArray, newHeader, dibHeader,
                            filePointer, pixels, newArrSize,
                            bitsPerPixelStegano, bitFields, threadsCount, algoSelection)){
            printf("Dane zapisane poprawnie!\n");
        }
        else{
            printf("Błąd zapisu danych do pliku!\n");
            return 0;
        }

    } else {
        printf("Błąd zapisu danych!\n");
        return 0;
    }
    free(finalDataTable);
    return 1;
}

int check_for_stegano(BMPHEADER bmpHeader) {
    if (strcmp(bmpHeader.RESERVED1, "SG")) {
        return 0;
    }
    return 1;
}

uint8_t *read_data_from_steganofile_24_32bit(uint8_t *pixelArray,
                                             BMPHEADER bmpheader,
                                             int bitsPerPixel) {
    uint8_t *dataArray = (uint8_t *)malloc(bmpheader.RESERVED2 * 8 /
                                           bitsPerPixel * sizeof(uint8_t));
    uint8_t temp;
    if (LCM(8, bitsPerPixel) != 8) {
        printf("Zła liczba bitów do zapisu danych! Liczba ta musi być dzielnikiem liczby 8\n");
        return 0;
    }
    uint8_t *finalDataArray =
            (uint8_t *)malloc(bmpheader.RESERVED2 * sizeof(uint8_t));
    int i, j;
    uint8_t mask = (1 << bitsPerPixel) - 1;
    temp = 0;
    for (i = 0; i < bmpheader.RESERVED2 * (8 / bitsPerPixel); i++) {
        temp = pixelArray[i] & mask;
        dataArray[i] = temp;
    }
    for (i = 0; i < bmpheader.RESERVED2; i++) {
        temp = 0;
        for (j = 0; j < 8 / bitsPerPixel; j++) {
            temp = temp +
                   (dataArray[i * 8 / bitsPerPixel + j] << (j * bitsPerPixel));
        }
        finalDataArray[i] = temp;
    }
    free(dataArray);
    return finalDataArray;
}

uint8_t *read_data_from_steganofile_16bit(uint8_t *pixelArray,
                                          BMPHEADER bmpheader,
                                          int bitsPerPixel) {
    uint8_t *dataArray =
            (uint8_t *)malloc(bmpheader.RESERVED2 * 8 * sizeof(uint8_t));
    uint8_t temp;
    if (bitsPerPixel != 1) {
        printf("Zła liczba bitów do zapisu danych! Liczba ta musi wynosić 1\n");
        return 0;
    }
    uint8_t *finalDataArray =
            (uint8_t *)malloc(bmpheader.RESERVED2 * sizeof(uint8_t));
    int i, j;
    uint8_t mask1 = 1;
    uint8_t mask3 = 1 << 2;
    temp = 0;
    for (i = 0; i < bmpheader.RESERVED2 * 8; i++) {
        if (i % 2 == 0) {
            temp = pixelArray[i] & mask1;
            dataArray[i] = temp;
        } else {
            temp = pixelArray[i] & mask3;
            temp = temp >> 2;
            dataArray[i] = temp;
        }
    }
    for (i = 0; i < bmpheader.RESERVED2; i++) {
        temp = 0;
        for (j = 0; j < 8; j++) {
            temp = temp + (dataArray[i * 8 + j] << j);
        }
        finalDataArray[i] = temp;
    }
    free(dataArray);
    return finalDataArray;
}

int write_data_array_to_file(uint8_t *dataArray, int dataSize,
                             FILE *filePointer) {
    if (filePointer) {
        if (dataArray) {
            fwrite(dataArray, sizeof(uint8_t), dataSize, filePointer);
            return 1;
        }
    }
    return 0;
}

int write_data_from_image(BMPHEADER bmpHeader, uint8_t *pixelArray,
                          FILE *filePointer, DIBHEADER dibHeader,
                          int bitsPerPixel) {
    if (check_for_stegano(bmpHeader)) {
        uint8_t *dataArray;
        switch (dibHeader.BITSPERPIXEL) {
            case 16:
                dataArray = read_data_from_steganofile_16bit(
                        pixelArray, bmpHeader, bitsPerPixel);
                break;
            case 24:
                dataArray = read_data_from_steganofile_24_32bit(
                        pixelArray, bmpHeader, bitsPerPixel);
                break;
            case 32:
                dataArray = read_data_from_steganofile_24_32bit(
                        pixelArray, bmpHeader, bitsPerPixel);
                break;
            default:
                dataArray = 0;
                break;
        }
        if (dataArray) {
            if(write_data_array_to_file(dataArray, bmpHeader.RESERVED2,
                                        filePointer)) {
                printf("Dane zapisane poprawnie!\n");
                free(dataArray);
            }
            else{
                printf("Błąd zapisu do pliku!\n");
            }
        } else {
            printf("Brak danych!\n");
            return 0;
        }
    } else {
        printf("Plik nie zawiera nagłówka steganograficznego!\n");
        return 0;
    }
    return 1;
}