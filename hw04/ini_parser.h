#ifndef INI_PARSER_H
#define INI_PARSER_H

#include <stdbool.h>
#include "view.h"

static void help_command(const char *program);

int readline(FILE *base_file, FILE *result_file, bool cycle, bool guard, int depth, char *first_file);


#endif
