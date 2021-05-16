#include <stdio.h>

#include "ini_parser.h"

int main(int argc, char **argv)
{
    const char *help =
            "Help table for this program:\n"
            "\n"
            "This program loads .ini files into one file and takes following options:\n"
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
    if (argc <= 3) {
        fprintf(stderr,"%s", help);
        return EXIT_FAILURE;
    }
    int d = 10;
    bool guard = false;
    bool cycle = false;
    FILE *base_file;
    FILE *result_file;
    int i = 1;
    while (i < argc - 2)
    {
        if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0))
        {
            fprintf(stdout,"%s", help);
            return EXIT_SUCCESS;
        }
        else if ((strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--max-depth") == 0))
        {
            if (((i + 1) < argc) && isdigit(*(argv[i+1]))) {
                d = (int) *argv[i + 1];
                i++;
                i++;
                continue;
            }
            i++;
        }
        else if ((strcmp(argv[i], "-g") == 0) || (strcmp(argv[i], "--include-guard") == 0))
        {
            guard = true;
            i++;
        }
        else if ((strcmp(argv[i], "-r") == 0) || (strcmp(argv[i], "--report-cycles") == 0))
        {
            cycle = true;
            i++;
        }
        else {
            fprintf(stderr,"%s", help);
            return EXIT_FAILURE;
        }
    }

    if (((strstr(".ini", argv[argc-2])) == NULL) ||
        (strstr(".ini", argv[argc-1])) == NULL) {
        fprintf(stderr, "Not two .ini files");
        return EXIT_FAILURE;
    }
    base_file = fopen(argv[argc-2], "r");
    result_file = fopen(argv[argc-1], "w");

    if (readline(base_file, result_file, cycle, guard, d, argv[argc-2]) == EXIT_FAILURE) {
        fclose(base_file);
        fclose(result_file);
        fprintf(stderr, "Something bad in files");
        return EXIT_FAILURE;
    }

    fclose(base_file);
    fclose(result_file);
    return EXIT_SUCCESS;
}
