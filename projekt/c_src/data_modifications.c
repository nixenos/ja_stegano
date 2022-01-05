#include<stdint.h>

void _apply_data_modifications(uint8_t *pixelArray, uint8_t *dataArray, uint8_t mask, int arrSize) {
    int i=0;
    for (i=0; i< (arrSize); i++){
        pixelArray[i] = (pixelArray[i] & (~mask)) | dataArray[i];
    }
}

void _get_data_chunks_from_pixel_array(uint8_t *pixelArray, uint8_t *dataArray, uint8_t mask, int arrSize) {
    int i = 0;
    for (i = 0; i < arrSize; i++) {
        dataArray[i] = pixelArray[i] & mask;
    }
}

void _combine_data_chunks_into_bytes(uint8_t *dataArray, uint8_t *finalDataArray, int chunkSize, int arrSize) {
    int i = 0, j = 0;
    uint8_t temp = 0;
    for (i = 0; i < arrSize; i++) {
        temp = 0;
        for (j = 0; j < chunkSize; j++) {
            temp = temp + (dataArray[i * chunkSize + j] << (j * 2));
        }
        finalDataArray[i] = temp;
    }
}
