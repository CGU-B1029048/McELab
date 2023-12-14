#include <stdio.h>

int main() {

    int goldfood[4] = {0b00011000, 0b00111100, 0b00111100, 0b00011000}; 
    int ghost[6] = {0b00111100, 0b01110110, 0b00111110, 0b01111100, 0b01101110, 0b00111100};

    for (int i = 0; i < 6; i++) {
        printf("0x%2x\n", ghost[i]);
    }
}