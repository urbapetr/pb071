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
    int ch;
    unsigned long long int w = 0;
    unsigned int i = 6;
    unsigned int character;
    int my_list[4];
    int y = 3;
    int x = 0;
    while ((ch=getchar()) != EOF){
        if (isspace(ch)) continue;
        switch(ch){
            case 49 ... 57: ch = ch - 49; break;
            case 65 ... 72: ch = ch - 56; break;
            case 74 ... 78: ch = ch - 57; break;
            case 80 ... 90: ch = ch - 58; break;
            case 97 ... 107: ch = ch - 64; break;
            default: ch = ch - 65; break;
        }
        w = (w * 58) + ch;
        i--;
        if (i != 0){
            continue;
        }
        for (x = 0; x < 4; x++){
            character = w & 0xFF;
            w = w >> 8;
            my_list[x] = character;
        }
        while (y >= 0){
            putchar(my_list[y]);
            y--;
        }
        y = 3;
        w = 0;
        i = 6;
    }
    printf("\n");
    if (i != 6) 
        return false;
    return true;
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
