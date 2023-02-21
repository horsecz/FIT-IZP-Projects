/**
 * @author Dominik Horky
 * @date $Date
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#define MAX_ROW_CHARS 100000
#define BORDER_UP_DOWN 4
#define BORDER_LEFT 1
#define BORDER_RIGHT 2
#define BORDER_LEFTRIGHT 6
#define BORDER_LEFTDOWN 7
#define BORDER_RIGHTDOWN 8
#define NO_BORDER 3
#define BORDER_BLOCKED 5
#define TRIANGLE_BASIC 12
#define TRIANGLE_ROTATED 21
#define DIRECTION_LEFT 0
#define DIRECTION_RIGHT 1
#define START_LEFT 69
#define START_RIGHT 96
#define MALLOC_FAIL -1
#define EXIT_FAILURE_NO_ALLOC -1
#define EXIT_TOO_BIG -1

typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;

/**
 * @brief It simply checks, if element at some axis (row, column) has border at left, right or up/down.
 * 
 * @param map
 * @param r
 * @param c
 * @param border
 * @return bool
 * 
 */
bool isborder(Map *map, int r, int c, int border)
{
    unsigned char pos = map->cells[((r-1)*map->cols+c-1)];
    if (pos&border)
    {
        return true;
    }
    else
    {
        return false;
    }
    return true;
}

// returns where is first border of first elements (from where to start)
int start_border(Map *map, int r, int c, int leftright)
{ 
    bool element_borders[3] = {false, false, false}; // array of enter_element borders: {LEFT, RIGHT, UP_DOWN}
    if (isborder(map, r, c, BORDER_LEFT) == true)
    {
        element_borders[0] = true;
    }    
    if (isborder(map, r, c, BORDER_RIGHT) == true)
    {
        element_borders[1] = true;
    }
    if (isborder(map, r, c, BORDER_UP_DOWN) == true)
    {
        element_borders[2] = true;
    }
    
    if (r%2 == 1 && c == 1 && element_borders[1] == true) // start: odd row, from left
    {
        return BORDER_RIGHT;
    }
    else if (r%2 == 0 && c == 1 && element_borders[2] == true) // start: even row, from left
    {
        return BORDER_UP_DOWN;
    }
    else if (r == 1 && element_borders[2] == false) // start: first row, any column (down)
    {
        if (leftright == true && element_borders[1] == true)
        {
            return BORDER_RIGHT;
        }
        else if (leftright == false && element_borders[0] == true)
        {
            return BORDER_LEFT;
        }
        else
        {
            return NO_BORDER;
        }
    }
    else if (r == map->rows && element_borders[2] == false) // start: last row, any column (up)
    {
        if (leftright == true && element_borders[0] == true)
        {
            return BORDER_LEFT;
        }
        else if (leftright == false && element_borders[1] == true)
        {
            return BORDER_RIGHT;
        }
        else
        {
            return NO_BORDER;
        }
    }
    else if (element_borders[2] && ((r%2 == 1 && c%2 == 1) || (r%2 == 0 && c%2 == 0) || (c == map->cols))) // start: from right, element has border at top
    {
        return BORDER_UP_DOWN;
    }
    else if (element_borders[2] && ((r%2 == 0 && c%2 == 1) || (r%2 == 1 && c%2 == 0) || (c == map->cols))) // start: from right, element has border at bottom
    {
        return BORDER_LEFT;
    }
    else if (element_borders[0] && c == map->cols)
    {
        return BORDER_LEFT;
    }
    else
    { 
        return NO_BORDER;
    }
    return EXIT_SUCCESS;
}

// initialize map - load to memory
int mapInit(Map *m, int rows, int cols) 
{
    m->rows = rows;
    m->cols = cols;
    m->cells = malloc(sizeof(unsigned char)*rows*cols);
    if (m->cells != NULL) 
    {
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// free map from memory
void mapDestroy(Map *m)
{
    free(m->cells);
}

// check if maze size in file is correct
unsigned int *fileCheck_size(FILE *f, unsigned int *rowscols, char *buff, char *recyle_bin)
{   
    fgets(buff, MAX_ROW_CHARS, f);
    rowscols[0] = strtod(buff, &recyle_bin); //rows
    rowscols[1] = strtod(recyle_bin, &recyle_bin); //cols
    if ((recyle_bin[0] != '\0' && recyle_bin[0] != '\n' && strlen(recyle_bin) >= 1) || (buff[0] == '\n' && strlen(buff) <= 1))
    {
        return NULL;
    }
    return rowscols;
}

// check if maze elements are defined correctly & save them into map (structure)
int fileCheck_data(FILE *f, unsigned int *data, char *recyle_bin, Map *m)
{
    int number;
    unsigned int loaded_numbers = 0;
    for (unsigned int i = 0; i < data[0]+1; i++) // loads columns+1 (+1 to check if there isn't one column of data more) 
    {
        fgets(recyle_bin, MAX_ROW_CHARS, f); // load one/next column 
        if ((recyle_bin[0] == '\n' && strlen(recyle_bin) <= 1) || recyle_bin[0] == '\0')
        {
            break;
        }
        if (strlen(recyle_bin) < data[1]*2)
        {
            return EXIT_FAILURE;
        }
        if (i >= data[0])
        {
            return EXIT_FAILURE;
        }
        for (unsigned int i = 0; i < data[1]; i++) // checks number by number
        {
            number = strtod(recyle_bin, &recyle_bin);
            if (number > 7 || number < 0 || (i == data[1]-1 && recyle_bin[0] != '\0' && recyle_bin[0] != '\n' && strlen(recyle_bin) >= 1))
            {
                return EXIT_FAILURE;
            }
            m->cells[loaded_numbers] = number;
            loaded_numbers += 1;
        }
    }
    
    if ((data[0]*data[1] != loaded_numbers))
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// check if input file is valid & allocate the map into the memory before everything starts
int fileCheck(char *filepath, Map *maze)
{
    FILE *f = fopen(filepath, "r+");
    static unsigned int data[MAX_ROW_CHARS];
    if (f == NULL)
    {
        return MALLOC_FAIL;
    }
    else if (f != NULL)
    {
        char buff[MAX_ROW_CHARS+1];
        char recyle_bin[MAX_ROW_CHARS+1];
        unsigned int* size_check = fileCheck_size(f, data, buff, recyle_bin);
        if (size_check == NULL)
        {
            fclose(f);
            return MALLOC_FAIL;
        }
        if (data[0] * data[1] > MAX_ROW_CHARS)
        {
            fclose(f);
            return EXIT_TOO_BIG;
        }
        int result = mapInit(maze, data[0], data[1]);
        maze->rows = data[0];
        maze->cols = data[1];
        if (result == EXIT_FAILURE)
        {
            fclose(f);
            return MALLOC_FAIL;
        }
        int data_check = fileCheck_data(f, data, recyle_bin, maze);
        fclose(f);
        if (data_check == EXIT_FAILURE)
        {
            return EXIT_FAILURE;
        }
        else
        {
            return EXIT_SUCCESS;
        }
    }
    else
    {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

// returns which triangle type is on (current) position x and y
int getTriangleType(int x, int y)
{
    if ((x%2 == 0 && y%2 == 1) || (x%2 == 1 && y%2 == 0)) // basic triangle = {LEFT, RIGHT, DOWN}
    {
        return TRIANGLE_BASIC;
    }
    else if ((x%2 == 0 && y%2 == 0) || (x%2 == 1 && y%2 == 1)) // rotated triangle = {LEFT, RIGHT, UP}
    {
        return TRIANGLE_ROTATED;
    }
    return EXIT_SUCCESS;
}

// returns where has triangle-element border(s)
int getBorder(Map *map, int r, int c)
{
    int element_value = map->cells[((r-1)*map->cols+c-1)];
    if (element_value == 0)
    {
        return NO_BORDER;
    }
    else if (element_value == 1)
    {
        return BORDER_LEFT;
    }
    else if (element_value == 2)
    {
        return BORDER_RIGHT;
    }
    else if (element_value == 3)
    {
        return BORDER_LEFTRIGHT;
    }
    else if (element_value == 4)
    {
        return BORDER_UP_DOWN;
    }
    else if (element_value == 5)
    {
        return BORDER_LEFTDOWN;
    }
    else if (element_value == 6)
    {
        return BORDER_RIGHTDOWN;
    }
    else if (element_value == 7)
    {
        return BORDER_BLOCKED;
    }
    return EXIT_SUCCESS;
}

// sets walking direction in 2D maze
int setSearchDirection(int pos_y, Map* maze)
{
    int direction;
    if (pos_y == 1 || pos_y <= maze->cols/2)
    {
        direction = DIRECTION_RIGHT;
    }
    else if (pos_y == maze->cols || pos_y >= maze->cols/2)
    {
        direction = DIRECTION_LEFT;
    }
    else
    {
        direction = false;
    }
    return direction;
}

// sets previous position for findPath
int *setPreviousPositions(int border, int pos_x, int pos_y, Map* maze)
{
    static int prev_positions[2] = {0, 0};
    int prev_x, prev_y;
    
    if (border == BORDER_UP_DOWN)
    {
        prev_x = pos_x;
        if (pos_y == 1)
        {
            prev_y = pos_y-1;
        }
        else if (pos_y == maze->cols)
        {
            prev_y = pos_y+1;
            
        }
    }
    else if ((border == BORDER_LEFT || border == BORDER_RIGHT) && (pos_x == 1 || pos_x == maze->rows))
    {
        prev_y = pos_y;
        if (pos_x == 1)
        {
            prev_x = pos_x-1;
        }
        else if (pos_x == maze->rows)
        {
            prev_x = pos_x+1;
        }
    }
    else if ((border == BORDER_LEFT || border == BORDER_RIGHT) && (pos_y == 1 || pos_y == maze->cols))
    {
        prev_x = pos_x;
        if (pos_y == 1)
        {
            prev_y = pos_y-1;
        }
        else if (pos_y == maze->cols)
        {
            prev_y = pos_y+1;
        }
    }
    
    prev_positions[0] = prev_x;
    prev_positions[1] = prev_y;
    return prev_positions;
}

// value to check if there was element with more than 1 way out -> this is for case I chose the wrong way and got back there again (to try the 2nd way and prevent going back to start)
bool getCrossAgain(int cross_x, int cross_y, int pos_x, int pos_y)
{
    bool cross_again;
    if (cross_x == pos_x && cross_y == pos_y)
    {
        cross_again = true;
    }
    else
    {
        cross_again = false;
    }
    return cross_again;
}

// sets new direction (left / right) depending on move
int setNewDirection(int direction, int pos_y, int prev_y, bool cross_again, int cross_dir)
{
    if ((direction == DIRECTION_RIGHT && (pos_y+1 == prev_y)) || (cross_again == true && cross_dir != direction && direction == DIRECTION_RIGHT))
    {   
        direction = DIRECTION_LEFT;
    }
    else if ((direction == DIRECTION_LEFT && (pos_y-1 == prev_y)) || (cross_again == true && cross_dir != direction && direction == DIRECTION_LEFT))
    {
        direction = DIRECTION_RIGHT;
    }
    
    return direction;
}

// finds a way out of maze by right/left-hand rule
int findPath(bool leftHand, Map *maze, int *enter_position)
{    
    int pos_x = enter_position[0];
    int pos_y = enter_position[1];
    int start_x = pos_x;
    int start_y = pos_y;
    int cross_x = 0;
    int cross_y = 0;
    int cross_dir = 0;
    int change_x = 0;
    int change_y = 0;
    bool cross_again = false;
    int direction = setSearchDirection(pos_y, maze);
    
    fprintf(stdout, "%d,%d\n", pos_x, pos_y); // print first location (starting element)
    
    int border = start_border(maze, pos_x, pos_y, leftHand);
    int *prev_positions = setPreviousPositions(border, pos_x, pos_y, maze); // helps to get my current direction and prevents going back to start
    int prev_x = prev_positions[0];
    int prev_y = prev_positions[1];
    int triangle_type;
    
    while (pos_x >= 1 && pos_y >= 1 && pos_x <= maze->rows && pos_y <= maze->cols) // searching the way out of the maze
    {
        cross_again = getCrossAgain(cross_x, cross_y, pos_x, pos_y);
        direction = setNewDirection(direction, pos_y, prev_y, cross_again, cross_dir);
        triangle_type = getTriangleType(pos_x, pos_y);
        border = getBorder(maze, pos_x, pos_y); // value where has my element border -> so where can I go next
        change_x = 0;
        change_y = 0;
        
        if (border == BORDER_UP_DOWN) // border is at top/bottom -> move left/right
        {
            if (pos_y+1 == prev_y)
            {
                change_y = -1;
            }
            else
            {
                change_y = 1;
            }
        }
        else if (border == BORDER_LEFT) // border is at left side of element -> move down/up (depending on if element has side at top or bottom)
        {
            if (pos_y+1 == prev_y)
            {
                if (triangle_type == TRIANGLE_BASIC)
                {
                    change_x = 1;
                }
                else if (triangle_type == TRIANGLE_ROTATED)
                {
                    change_x = -1;
                }
            }
            else
            {
                change_y = 1;
            }
        }
        else if (border == BORDER_RIGHT) // border is at right side
        {
            if (triangle_type == TRIANGLE_BASIC) // it has side at bottom -> move down/left
            {
                if (pos_y - 1 == prev_y)
                {
                    change_x = 1;
                }
                else
                {
                    change_y = -1;
                }
            }
            else if (triangle_type == TRIANGLE_ROTATED) // side at top -> move left/up
            {
                if (prev_x == pos_x-1)
                {
                   change_y = -1;
                }
                else
                {
                    change_x = -1;
                }
            }
        }
        else if (border == NO_BORDER) // element has no borders => there are always two ways where to go
        {
            cross_x = pos_x;
            cross_y = pos_y;
            cross_dir = direction;
    
            if ((leftHand == true && (pos_x-1 != prev_x) && (triangle_type == TRIANGLE_ROTATED))) // left-hand rule & side at top => move up
            {
                change_x = -1;
            }
            else if ((leftHand == true && (pos_x+1 != prev_x) && (triangle_type == TRIANGLE_BASIC))) // left-hand rule & side at bottom => move down
            {
                change_x = 1;
            }
            else if (((pos_y+1 == prev_y) || (leftHand == true && direction == DIRECTION_LEFT))) // (left direction) || (left hand rule & left direction) => move left
            {
                change_y = -1;
            }
            else if (leftHand == true && direction == DIRECTION_RIGHT && start_y == 1 && start_x != 1) // left-hand rule && right direction => move right
            {
                change_y = 1;
            }
            else if (leftHand == true && direction == DIRECTION_RIGHT && start_x == 1)
            {
                change_y = 1;
            }
            else if ((leftHand == false && (pos_y-1 != prev_y ) && triangle_type == TRIANGLE_ROTATED) || (leftHand == true && pos_x-1 == prev_x && triangle_type == TRIANGLE_ROTATED)) // right-hand rule & left direction & side at top => move left
            {
                change_y = -1;
            }
            else if (leftHand == false || (leftHand == false && triangle_type == TRIANGLE_BASIC)) // (right-hand rule) || (right hand rule & side at bottom) => move right
            {
                change_y = 1;
            }
        }
        else if (border == BORDER_LEFTRIGHT) // element has borders at left and right side => can move only down/up
        {
            if (triangle_type == TRIANGLE_BASIC)
            {
                change_x = 1;
            }
            else if (triangle_type == TRIANGLE_ROTATED)
            {
                change_x = -1;
            }
        }
        else if (border == BORDER_LEFTDOWN) // element has borders at left and down/up => can move only right
        {
            change_y = 1;
        }
        else if (border == BORDER_RIGHTDOWN) // element has borders at right and down/up => can move only left
        {
            change_y = -1;
        }
        else if (border == BORDER_BLOCKED) // element has borders at all its sides
        {
            fprintf(stdout, "error: unexpected event - entered element which can not be left\n");
            break;
        }
        else
        {
            fprintf(stdout, "error: unexpected event\n");
            break;
        }
        
        // set new coordinates (= move)
        prev_x = pos_x;
        prev_y = pos_y;
        pos_x += change_x;
        pos_y += change_y;
        
        if (pos_x >= 1 && pos_y >= 1 && pos_x <= maze->rows && pos_y <= maze->cols) // to prevent printing the last location (out of the maze)
        {
            fprintf(stdout, "%d,%d\n", pos_x, pos_y);
        }
    }
    
    return EXIT_SUCCESS;
}

// returns true if position of element is on the edge of maze (to prevent entering from the middle)
bool enterCheck(int *enter_pos, Map *m)
{
    int row = enter_pos[0];
    int col = enter_pos[1];
    if (row == 0 || col == 0 || row < 0 || col < 0 || (row > (int) m->rows) || (col > (int) m->cols))
    {
        return false;
    }
    if ((row == 1 && col%2 == 1) || (row == (int) m->rows && ((row%2 == 0 && col%2 == 1) || (row%2 == 1 && col%2 == 0))) || (col == 1) || (col == (int) m->cols))
    {
        return true;
    }
    return false;
}

// if there are 5 arguments (=> user wants to find a way out of maze), check if they are correct 
int pathFindArgumentCheck(Map *maze, char **argv)
{
    char *endptr;
    int enter[2];
    for (int i = 0; i < 3; i++)  // check if argument has numbers-only
    {
        if (i < 2)
        {
            enter[i] = strtod(argv[i+2], &endptr);
        }
        if (argv[i+2][0] == '\0' || *endptr != '\0') // its 2nd, 3rd and 4th argument
        {
            fprintf(stderr, "error: bad argument(s) - try ./proj3 --help\n");
            return EXIT_FAILURE_NO_ALLOC;
        }
    }
    
    int data = fileCheck(argv[4], maze);
    if (data == EXIT_FAILURE) // if input is invalid / some error occured => stop
    {
        fprintf(stderr, "error: invalid input\n");
        return EXIT_FAILURE;
    }
    else if (data == MALLOC_FAIL)
    {
        fprintf(stderr, "error: unable to allocate map to memory\n");
        return EXIT_FAILURE_NO_ALLOC;
    }
    else if (data == EXIT_TOO_BIG)
    {
        fprintf(stderr, "error: unable to allocate map to memory - max size limit exceed\n");
        return EXIT_FAILURE_NO_ALLOC;
    }
    
    bool enter_result = enterCheck(enter, maze);
    if (enter_result == false) // prevent to start from the middle of maze or from element with outside border
    {
        fprintf(stderr, "error: invalid start element position\n");
        return EXIT_FAILURE;
    }
    if (strcmp(argv[1], "--rpath") == 0) // follow right-hand rule
    {
        findPath(false, maze, enter);
        return EXIT_SUCCESS;
    }
    else if (strcmp(argv[1], "--lpath") == 0) // follow left-hand rule
    {
        findPath(true, maze, enter);
        return EXIT_SUCCESS;
    }
    else
    {
        fprintf(stderr, "error: bad operation - try ./proj3 --help\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// prints help message 
void printHelpMessage()
{
    char *space_small = "    ";
    char *space_medium = "       ";
    char *space_great = "           ";
    fprintf(stdout, "Usage: ./proj3 <operation> [...]\n");
    fprintf(stdout, "Operations:\n%s", space_small);
    fprintf(stdout, "./proj3 --help\n%s", space_medium);
    fprintf(stdout, "> prints this message\n%s", space_small);
    fprintf(stdout, "./proj3 --test [filepath]\n%s", space_medium);
    fprintf(stdout, "> checks if file located in [filepath] is valid/invalid input for this program\n%s", space_medium);
    fprintf(stdout, "> valid file contains only numbers separated by space: \n%s", space_great);
    fprintf(stdout, "first row: 2 numbers only - rows and cols of maze\n%s", space_great);
    fprintf(stdout, "next row(s): numbers 0 - 7 only\n%s", space_small);
    fprintf(stdout, "./proj3 --rpath [rows] [cols] [filepath]\n%s", space_small);
    fprintf(stdout, "./proj3 --lpath [rows] [cols] [filepath]\n%s", space_medium);
    fprintf(stdout, "> finds a way out through the maze defined in file located in [filepath]\n%s", space_medium);
    fprintf(stdout, "> start point is defined by triangle located at [rows] [cols]\n%s", space_medium);
    fprintf(stdout, "> --lpath finds a way out by left-hand rule, --rpath by right-hand rule\n\n");
    fprintf(stdout, "Examples:\n%s", space_small);
    fprintf(stdout, "./proj3 --lpath 6 1 maze.txt\n%s", space_small);
    fprintf(stdout, "./proj3 --test /home/<username>/proj3/maze.txt\n");
}
        

// help, argument check, etc.
int main(int argc, char **argv)
{
    if (argc > 5 || argc == 1)
    {
        fprintf(stderr, "error: bad usage - try ./proj3 --help\n");
        return EXIT_FAILURE;
    }
    else if (strcmp(argv[1],"--help") == 0 && argc == 2)
    {
        printHelpMessage();
        return EXIT_SUCCESS;
    }
    else if (argc == 3 && strcmp(argv[1],"--test") == 0 && strcmp(argv[2], "\0") != 0) // --test [file] checks if input file has valid/invalid defined maze
    {
        Map maze;
        int result = fileCheck(argv[2], &maze);
        if (result == EXIT_SUCCESS)
        {
            mapDestroy(&maze);
            fprintf(stdout, "Valid\n");
            return EXIT_SUCCESS;
        }
        else if (result == MALLOC_FAIL)
        {
            fprintf(stdout, "Invalid\n");
            return EXIT_FAILURE;
        }
        else
        {
            mapDestroy(&maze);
            fprintf(stdout, "Invalid\n");
            return EXIT_FAILURE;
        }
    }
    else if (argc == 5) // 5 arguments => --lpath R C [file] \/ --rpath R C [file]
    {     
        Map maze;
        int result = pathFindArgumentCheck(&maze, argv);
        if (result == EXIT_SUCCESS)
        {
            mapDestroy(&maze);
            return EXIT_SUCCESS;
        }
        else if (result == EXIT_FAILURE_NO_ALLOC)
        {
            return EXIT_FAILURE;
        }
        else
        {
            mapDestroy(&maze);
            return EXIT_FAILURE;
        }
    }
    else
    {
        fprintf(stderr, "error: bad operation or arguments - try ./proj3 --help\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
