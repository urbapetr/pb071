#include <stdio.h>
#include "ini_parser.h"

#include "view.h"

static void help_command(const char *program)
{
    const char *help =
            "Usage: %s OPTIONS\n"
            "\n"
            "This program loads a sudoku and takes following options:\n"
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

static void depth_command(const char *program, int N)
{
    printf("\n in DEPTH \n");
}

static void guard_command(const char *program)
{
    printf("\n in GUARD \n");
}

static void cycles_command(const char *program)
{
    printf("\n in CYCLE \n");
}

int readline(FILE *base_file, FILE *result_file, bool cycle, bool guard, int depth, char *first_file);

int main(int argc, char **argv)
{
    int d = 10;
    bool guard = false;
    bool cycle = false;
    bool have_files = false;
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
            depth_command(argv[0], 10);
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


int readline(FILE *base_file, FILE *result_file, bool cycle, bool guard, int depth, char *first_file) {
    if (depth == 0) {
        return EXIT_SUCCESS;
    }
    char all_lines[255];
    while (fgets(all_lines, 255, base_file)) {
        if ((strstr(".include", all_lines)) != NULL) {
            bool skip = false;
            if (cycle) {
                if (strcmp(&all_lines[9], first_file) == 0) {
                        return EXIT_FAILURE;
                }
            }
            if (guard) {
                if (strcmp(&all_lines[9], first_file) == 0) {
                    skip = true;
                    break;
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
