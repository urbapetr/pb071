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

int main(int argc, char **argv)
{
    int i = 1;
    while (i < argc)
    {
        if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0))
        {
            help_command(argv[0]);
            return EXIT_SUCCESS;
        }
        else if ((strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--max-depth") == 0))
        {
            if (((i + 1) < argc) && isdigit(*(argv[i+1]))) {
                depth_command(argv[0], (int) *(argv[i + 1]));
                i++;
                i++;
                continue;
            }
            depth_command(argv[0], 10);
            i++;
        }
        else if ((strcmp(argv[i], "-g") == 0) || (strcmp(argv[i], "--include-guard") == 0))
        {
            guard_command(argv[0]);
            i++;
        }
        else if ((strcmp(argv[i], "-r") == 0) || (strcmp(argv[i], "--report-cycles") == 0))
        {
            cycles_command(argv[0]);
            i++;
        }
        else {
            help_command(argv[0]);
            return EXIT_FAILURE;
        }
    }
}
