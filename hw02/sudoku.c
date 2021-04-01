#include "sudoku.h"

/* ************************************************************** *
 *               Functions required by assignment                 *
 * ************************************************************** */
bool needs_solving(unsigned int sudoku[9][9]);
bool is_valid(unsigned int sudoku[9][9]);
bool check_number(int row, int collum, unsigned int sudoku[9][9]);
int which_block(int x);
unsigned int possible_number(int row, int collum, unsigned int sudoku[9][9]);
bool is_number(unsigned int number);
bool is_unpredictable(unsigned int number);
bool is_impossible(unsigned int number);
void print(unsigned int sudoku[9][9]);
int converter(unsigned int number);
unsigned int converter_binary(int number);

bool eliminate_row(unsigned int sudoku[9][9], int row_index)
{
    bool changed = false;
    for (int i = 0; i < 9; i++)
    {
        if (is_number(sudoku[row_index][i]))
        {
            continue;
        }
        if (is_impossible(sudoku[row_index][i]))
        {
            printf("ROW: Something went wrong :( [%d][%d]\n", row_index, i);
            return 0;
        }
        for (int j = 0; j < 9; j++)
        {
            if (j == i) {continue;}
            if (!(is_number(sudoku[row_index][j]))) {continue;}
            sudoku[row_index][i] = sudoku[row_index][i] ^ (sudoku[row_index][i] & sudoku[row_index][j]);
            changed = true;
        }
    }
    return changed;
}

bool eliminate_col(unsigned int sudoku[9][9], int col_index)
{
    bool changed = false;
    for (int i = 0; i < 9; i++)
    {
        if (is_number(sudoku[i][col_index])) {continue;}
        if (is_impossible(sudoku[i][col_index]))
        {
            printf("[%d][%d] COL: Something went wrong :(\n", i, col_index);
            return 0;
        }
        for (int j = 0; j < 9; j++)
        {
            if (j == i) {continue;}
            if (!(is_number(sudoku[j][col_index]))) {continue;}
            sudoku[i][col_index] = sudoku[i][col_index] ^ (sudoku[i][col_index] & sudoku[j][col_index]);
            changed = true;
        }
    }
    return changed;
}

bool eliminate_box(unsigned int sudoku[9][9], int row_index, int col_index)
{
    bool changed = false;
    int row_block = which_block(row_index);
    int col_block = which_block(col_index);
    int row_repeat = row_block + 2;
    int col_repeat = col_block + 2;
    for (int x = row_block; x <= row_repeat; x++)
    {
        for (int y = col_block; y <= col_repeat; y++)
        {
            if (is_impossible(sudoku[x][y])){printf("[%d][%d] COL: Something went wrong :(\n", x,y);return 0;}
            for (int i = row_block; i <= row_repeat; i++)
            {
                for (int j = col_block; j <= col_repeat; j++)
                {
                    if (x == i && y == j){continue;}
                    if (!(is_number(sudoku[i][j]))){continue;}
                    sudoku[x][y] = sudoku[x][y] ^ (sudoku[x][y] & sudoku[i][j]);
                }
            }
        }
    }
    return changed;
}

bool needs_solving(unsigned int sudoku[9][9])
{
    for (int row = 0; row < 9; row++)
    {
        for (int collum = 0; collum < 9; collum++)
        {
            if (!(is_number(sudoku[row][collum]))){return 1;}
        }
    }
    return 0;
}

bool is_valid(unsigned int sudoku[9][9])
{
    for (int row = 0; row < 9; row++)
    {
        for (int collum = 0; collum < 9; collum++)
        {
            if (sudoku[row][collum] == 0 && possible_number(row, collum, sudoku) == 0) {return 0;}
            if (is_number(sudoku[row][collum]) && !(check_number(row, collum, sudoku))) {return 0;}
        }
    }
    return 1;
}

bool check_number(int row, int collum, unsigned int sudoku[9][9])
{
    unsigned int number = sudoku[row][collum];
    for (int i = 0; i < 9; i++)
    {
        if (i != collum) {
            if (number == sudoku[row][i]) {return false;}
        }
        if (i != row)
        {
            if (number == sudoku[i][collum]) {return false;}
        }
    }
    int row_block = which_block(row);
    int col_block = which_block(collum);
    int repeat_row = row_block + 2;
    int repeat_col = col_block + 2;
    while (row_block <= repeat_row) {
        while (col_block <= repeat_col) {
            if (row_block != row && col_block != collum && number == sudoku[row_block][col_block]) {return false;}
            col_block++;
        }
        col_block = repeat_col - 2;
        row_block++;
    }
    return true;
}

int which_block(int x)
{
    switch (x)
    {
    case 0:
    case 1:
    case 2:
        return 0;
    case 3:
    case 4:
    case 5:
        return 3;
    default:
        return 6;
    }
}

unsigned int possible_number(int row, int collum, unsigned int sudoku[9][9])
{
    unsigned int possible_row = 511;
    unsigned int possible_collum = 511;
    for (int i = 0; i < 9; i++)
    {
        if (i != collum){possible_row = possible_row ^ sudoku[row][i];}
        if (i != row) {possible_collum = possible_collum ^ sudoku[i][collum];}
    }
    unsigned int possible_block = 511;
    int row_block = which_block(row);
    int col_block = which_block(collum);
    int repeat_row = row_block + 2;
    int repeat_col = col_block + 2;
    while (row_block <= repeat_row)
    {
        while (col_block <= repeat_col)
        {
            if (collum != col_block && row != row_block)
            {
                possible_block = possible_block ^ (possible_block & sudoku[row_block][col_block]);
            }
            col_block++;
        }
        col_block = repeat_col - 2;
        row_block++;
    }
    int result = possible_block & possible_collum & possible_row;
    return result;
}

bool solve(unsigned int sudoku[9][9])
{
    bool changed = false;
    if (!(needs_solving(sudoku))) {return true;}
    if (!(is_valid(sudoku)))
    {
        printf("SOLVE: Something went wrong :(\n");
        return false;
    }
    for (int i = 0; i < 9; i++)
    {
        if (eliminate_row(sudoku, i)){changed = true;}
        if (eliminate_col(sudoku, i)) {changed = true;};
    }
    int x = 0;
    int y = 0;
    while (x <= 6)
    {
        while (y <= 6)
        {
            if (eliminate_box(sudoku, x, y)) {changed = true;}
            y +=3;
        }
        y = 0;
        x += 3;
    }
    bool solved = false;
    if (!(needs_solving(sudoku))) {return true;}
    if (!changed){return false;}
    solved = solve(sudoku);
    return solved;
}

bool second_load(int i, int j, int row, int collum, char character, unsigned int sudoku[9][9])
{
    int new_char;
    if (row == 0 || row == 4 || row == 8 || row == 12)
    {
        switch (collum)
        {
        case 0:
        case 8:
        case 16:
        case 24:
            return (character == '+');
        case 25:
            return (character == '\n');
        default:
            return (character == '-');
        }
    }
    switch (collum)
    {
    case 0:
    case 8:
    case 16:
    case 24:
        return (character == '|');
    case 2:
    case 4:
    case 6:
    case 10:
    case 12:
    case 14:
    case 18:
    case 20:
    case 22:
        new_char = (character - 48);
        if (new_char == 0) {new_char = 511;}
        sudoku[i][j] = converter_binary(new_char);
        return true;
    case 25:
        return (character == '\n');
    default:
        return (character == ' ');
    }
}

void first_load(int i,int j, int character, unsigned int sudoku[9][9]){
    int new_char = (character - 48);
    if (new_char == 0) {new_char = 511;}
    sudoku[i][j] = converter_binary(new_char);
}

bool load(unsigned int sudoku[9][9])
{
    int i = 0;
    int j = 0;
    int character = getchar();
    int row = 0;
    int collum = 0;
    bool stop = false;
    if (character == '+')
    {
        collum++;
        while (!stop)
        {
            (character = getchar());
            if (!(second_load(i, j, row, collum, character, sudoku))) {return false;}
            if (!(row == 0 || row == 4 || row == 8 || row == 12) &&
                (collum == 2 || collum == 4 || collum == 6 || collum == 10 || collum == 12 || collum == 14 ||
                    collum == 18 || collum == 20 || collum == 22)) {j++;}
            collum++;
            if (collum == 26)
            {
                collum = 0;
                row++;
            }
            if (j == 9)
            {
                j = 0;
                i++;
            }
            if (row == 13)
            {
                stop = true;
            }
        }
    }else {
        first_load(i,j,character,sudoku);
        j++;
        while ((character = getchar()) != '\n')
        {
            first_load(i,j,character,sudoku);
            j++;
            if (j == 9)
            {
                j = 0;
                i++;
            }
            if (i > 9) { return false;}
        }
    }
    if ((getchar()) != '\n'){return false;}
    return true;
}

bool is_number(unsigned int number)
{
    if (number == 1 || number == 2 || number == 4 || number == 8 || number == 16 || number == 32 || number == 64 || number == 128 || number == 256) {
        return true;
    }
    return false;
}

bool is_unpredictable(unsigned int number)
{
    if (!(is_number(number)) && !(is_impossible(number))){return true;}
    return false;
}

bool is_impossible(unsigned int number)
{
    if (number == 0) {return true;}
    return false;
}

void print(unsigned int sudoku[9][9])
{
    char *middle = "+-------+-------+-------+\n";
    printf("%s", middle);
    for (int i = 0; i < 9; i++)
    {
        printf("|");
        for (int y = 0; y < 9; y++)
        {
            if (is_number(sudoku[i][y]))
            {
                printf(" %d", converter(sudoku[i][y]));
            } else if (is_unpredictable(sudoku[i][y]))
            {
                printf(" .");
            } else if (is_impossible(sudoku[i][y]))
            {
                printf(" !");
            }

            if (y % 3 == 2)
            {
                printf(" |");
            }
        }
        printf("\n");
        if (i % 3 == 2)
        {
            printf("%s", middle);
        }
    }
    return;
}

unsigned int converter_binary(int number)
{
    switch (number)
    {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 4;
    case 4:
        return 8;
    case 5:
        return 16;
    case 6:
        return 32;
    case 7:
        return 64;
    case 8:
        return 128;
    case 9:
        return 256;
    case 511:
        return 511;
    default: return false;
    }
}

int converter(unsigned int number)
{
    switch (number)
    {
    case 1:
        return 1;
    case 2:
        return 2;
    case 4:
        return 3;
    case 8:
        return 4;
    case 16:
        return 5;
    case 32:
        return 6;
    case 64:
        return 7;
    case 128:
        return 8;
    case 256:
        return 9;
    default:
        return number;
    }
}

/* ************************************************************** *
 *                              Bonus                             *
 * ************************************************************** */

#ifdef BONUS_GENERATE
void generate(unsigned int sudoku[9][9])
{
    return; // todo
}
#endif

#ifdef BONUS_GENERIC_SOLVE
bool generic_solve(unsigned int sudoku[9][9])
{
    return false; // todo
}
#endif

/* ************************************************************** *
 *                 Adwised auxiliary functionns                   *
 * ************************************************************** */

/* TODO: comment-out #if 0 and correspoding endif to implement */

#if 0
/**
 * @brief Compute the bitset of all done numbers in the box.
 *
 * You might like a similar function for row and for column.
 *
 * @param sudoku 2D array of digit bitsets
 * @param row_index of the top most row in box, one of 0, 3, 6
 * @param col_index of the left most column in box, one of 0, 3, 6
 */
static int box_bitset(unsigned int sudoku[9][9], int row_index, int col_index) {
    return 0;
}

/**
 * @brief Add number into bit set
 *
 * This function encapsulates a bit ands, ors and whatever
 * other bint operations, that would flood the toplevel code
 * with implementation details.
 *
 * @param original  contents of the 2D sudoku cell.
 * @param number    to be added to the set
 * 
 * @return          new value of the cell with the number included
 */
static unsigned int bitset_add(unsigned int original, int number) {
    return 0;
}

/**
 * @brief  Drop number from bit set.
 *
 * For detailed description, see bitset_add.
 *
 * @param original  contents of the 2D sudoku cell.
 * @param number    to be dropped from the set
 * 
 * @return          new value of the cell without the number included
 */
static unsigned int bitset_drop(unsigned int original, int number) {
    return 0;
}

/**
 * @brief  Check whether given number is present in the set.
 *
 * @param original  contents of the 2D sudoku cell.
 * @param query     number which should be checked for presence
 * 
 * @return          true if set, false otherwise
 */
static bool bitset_is_set(unsigned int original, int query) {
	return false;
}

/**
 * @brief  Check whether given cell has a unique value assigned.
 *
 * @param original  bitset to check for single vs. multiple values.
 * 
 * @return          true if set, false otherwise
 */
static bool bitset_is_unique(unsigned int original) {
	return false;
}

/**
 * @brief Return next number present in bit set.
 *
 * This function encapsulates a bit ands, ors and whatever
 * other bint operations, that would flood the toplevel code
 * with implementation details.
 *
 * @param original  contents of the 2D sudoku cell.
 * @param previous  last known number present, 0 for start
 * 
 * @return          * next (higher) number than argument if such
                    such is present.
 *                  * -1 otherwise
 *
 * @note The value previous might not be in the bitset
 */
static int bitset_next(unsigned int bitset, int previous) {
    return 0;
}

#endif // if 0
