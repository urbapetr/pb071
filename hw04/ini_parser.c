#include <stdio.h>
#include "ini_parser.h"


void help_command(const char *program)
{
    const char *help =
            "Usage: %s OPTIONS\n"
            "\n"
            "This proram loads .ini files into one file and takes following options:\n"
            "\n"
            "\t-h, --help\t\t Show OPTIONS of this program (nothing else)\n"
            "\n"
            "\t-d N, --max-depth N\tMaximum depth, where N must be number\n"
            "\t\t\t\t-> 0 = not allowed\n"
            "\t\t\t\t-> minus value -> unlimited\n"
            "\t\t\t\t-> if N is not declared, then N = 10\n"
            "\n"
            "\t-g, --include-guard\tEvery file can be included only for first used time\n"
            "\n"
            "\t-r, --report-cycles\tDetecting cycling program\n"
            "\n";

    printf(help, program);
}

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
