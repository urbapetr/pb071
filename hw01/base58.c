#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void coder(uint64_t combinedChar);
bool encode(void)
{
    int16_t character;
    uint64_t combinedChar = 0;
    uint8_t counter = 0;
    while ((character = getchar()) != EOF) {
        combinedChar = (combinedChar << 8) | character;
        counter++;
        if (counter != 4)
            continue;
        coder(combinedChar);
        counter = 0;
        combinedChar = 0;
    }
    if (counter != 0) {
        while (counter != 4) {
            combinedChar = (combinedChar << 8) | '\0';
            counter++;
        }
        coder(combinedChar);
    }
    printf("\n");
    return true;
}

void coder(uint64_t combinedChar)
{
    uint8_t baseChar = 0;
    uint64_t charSaver = 0;
    uint16_t character;
    for (int8_t i = 0; i < 6; i++) {
        baseChar = combinedChar % 58;
        if (baseChar <= 8) {
            baseChar += 49;
        } else if (9 <= baseChar && baseChar <= 16) {
            baseChar += 56;
        } else if (17 <= baseChar && baseChar <= 21) {
            baseChar += 57;
        } else if (22 <= baseChar && baseChar <= 32) {
            baseChar += 58;
        } else if (33 <= baseChar && baseChar <= 43) {
            baseChar += 64;
        } else {
            baseChar += 65;
        }
        combinedChar = combinedChar / 58;
        charSaver = (charSaver << 8) | baseChar;
    }
    for (int8_t counter = 0; counter < 6; ++counter) {
        character = charSaver & 0xFF;
        putchar(character);
        charSaver = charSaver >> 8;
    }
}

bool decode(void)
{
    int16_t inputChar;
    uint64_t combinedChar = 0;
    int8_t counter = 6;
    uint16_t character;
    uint16_t my_list[4];
    int8_t index = 3;
    while ((inputChar = getchar()) != EOF) {
        if (isspace(inputChar))
            continue;
        if (49 <= inputChar && inputChar <= 57) {
            inputChar -= 49;
        } else if (65 <= inputChar && inputChar <= 72) {
            inputChar -= 56;
        } else if (74 <= inputChar && inputChar <= 78) {
            inputChar -= 57;
        } else if (80 <= inputChar && inputChar <= 90) {
            inputChar -= 58;
        } else if (97 <= inputChar && inputChar <= 107) {
            inputChar -= 64;
        } else if (109 <= inputChar && inputChar <= 122) {
            inputChar -= 65;
        } else {
            return false;
        }
        combinedChar = (combinedChar * 58) + inputChar;
        counter--;
        if (counter != 0) {
            continue;
        }
        for (int8_t x = 0; x < 4; x++) {
            character = combinedChar & 0xFF;
            combinedChar = combinedChar >> 8;
            my_list[x] = character;
        }
        while (index >= 0) {
            putchar(my_list[index]);
            index--;
        }
        index = 3;
        combinedChar = 0;
        counter = 6;
    }
    if (counter != 6)
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
