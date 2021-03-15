#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
void coder(unsigned long long int w);
bool encode(void)
{
    int ch;
    unsigned long long int w;
    unsigned int i = 0;
    while ((ch=getchar()) != EOF){
        w = (w << 8) | ch;
        i++;
        if (i != 4)
            continue;
        coder(w);
        i = 0;
        w = 0;
    }
    if (i != 0){
        while (i != 4){
            w = (w << 8) | '\0';
            i++;
        }
        coder(w);
    }
    printf("\n");
    return true;
}

void coder(unsigned long long int x){
    unsigned int a;
    unsigned long long int y;
    unsigned int character;
    int p = 0;
    while (x != 0) {
        a = x % 58;
        switch(a) {
            case 1 ... 9: a = a + 49; break;
            case 10 ... 15: a = a + 56; break;
            case 16 ... 20: a = a + 57; break;
            case 21 ... 32: a = a + 58; break;
            case 33 ... 43: a = a + 64; break;
            default: a = a + 65; break;
        }
        x = x / 58;
        y = (y << 8) | a;
    }
    for (p = 0; p < 6; ++p){
        character = y & 0xFF;
        putchar(character);
        y = y >> 8;
    }
}

bool decode(void)
{
    // TODO: implement simplified Base58 decode
    return false;
}

// ================================
// DO NOT MODIFY THE FOLLOWING CODE
// ================================
int main(int argc, char **argv)
{
    if ((argc == 1) || (argc == 2 && !strcmp(argv[1], "-e"))) {
        encode();
    } else if (argc == 2 && !strcmp(argv[1], "-d")) {
        if (!decode()) {
            fprintf(stderr, "Input isn't encoded via Base58!\n");
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "Invalid switch, use -e or -d\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
