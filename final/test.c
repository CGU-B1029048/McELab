#include <stdio.h>

int main() {

    int goldfood[4] = {0b00011000, 0b00111100, 0b00111100, 0b00011000}; 

    for (int i = 0; i < 4; i++) {
        printf("%2x\n", goldfood[i]);
    }
}