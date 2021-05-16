#include <stdio.h>
#include "ini_parser.h"


int readline(FILE *base_file, FILE *result_file, bool cycle, bool guard, int depth, char *first_file) {
    if (depth == 0) {
        return EXIT_SUCCESS;
    }
    char all_lines[255];
    while (fgets(all_lines, 255, base_file)) {
        if ((strstr(".include", all_lines)) != NULL) {
            if (cycle) {
                if (strcmp(&all_lines[9], first_file) == 0) {
                    return EXIT_FAILURE;
                }
            }
            if (guard) {
                if (strcmp(&all_lines[9], first_file) == 0) {
                    continue;
                }
            }
            FILE *next_file;
            next_file = fopen(&all_lines[9], "r");
            readline(next_file, result_file, cycle, guard, depth-1, (char *) &first_file);
            fclose(next_file);
            continue;
        }
        fputs(all_lines, result_file);
    }
    return EXIT_SUCCESS;
}
