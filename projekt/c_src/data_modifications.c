#include<stdint.h>

void _apply_data_modifications(uint8_t *pixelArray, uint8_t *dataArray, uint8_t mask, int arrSize) {
    int i=0;
    for (i=0; i< (arrSize); i++){
        pixelArray[i] = (pixelArray[i] & (~mask)) | dataArray[i];
    }
}