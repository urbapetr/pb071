#include <stdio.h>

#include "ini_parser.h"

int main(int argc, char **argv)
{
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
            help_command(argv[0]);
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
            help_command(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (((strstr(".ini", argv[argc-2])) == NULL) &&
        (strstr(".ini", argv[argc-1])) == NULL) {
        return EXIT_FAILURE;
    }
    base_file = fopen(argv[argc-2], "r");
    result_file = fopen(argv[argc-1], "w");

    if (readline(base_file, result_file, cycle, guard, d, argv[argc-2]) == EXIT_FAILURE) {
        fclose(base_file);
        fclose(result_file);
        return EXIT_FAILURE;
    }

    fclose(base_file);
    fclose(result_file);
    return EXIT_SUCCESS;
}
