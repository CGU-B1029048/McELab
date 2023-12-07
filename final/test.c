#include <stdio.h>

int main() {
    for (int i = 0; i < 64; i++) {
        printf("%2d %2d %2d\n", i, i/8, i%8);
    }
}