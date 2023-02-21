/**
 * @file proj3.h
 * @author Dominik Horky
 * @author xhorky32
 * @date 22. 12. 2019
 */

#ifndef PROJ3_H
#define PROJ3_H

/**
 * @struct Map
 * @brief This struct contains data about the maze - its rows, columns and also array of elements.
 * @see load_map
 * @see free_map
 */
typedef struct {
    int rows; /**< number of rows */
    int cols; /**< number of columns */
    unsigned char *cells; /**< elements and its data - where each element has a border */
} Map;

/**
 * @enum borders
 * @brief There are stored bit weights for each border (side) of element.
 * @note Elements in maze are triangles. Each element can have border at top/bottom, left or right. These bit values (weights) represents where has element border. 1 means there is border, 0 means there is not.
 * @see isborder
 */
enum borders { 
    BLEFT=0x1, /**< Left side of element represents number 1 -> in binary, it's 001 */
    BRIGHT=0x2, /**< Right side of element represents number 2 -> in binary it's 010 */
    BTOP=0x4, /**< Top side is represented by number 4 -> in binary it's 100 */
    BBOTTOM=0x4 /**< Bottom side is represented by number 4 -> in binary it's 100 */
};

/**
 * @brief Deallocates and frees maze map from memory. 
 * @param map structure of the maze to be deallocated
 * @pre Expects existing structure of maze map.
 * @pre Inside of that structure there has to be successfully allocated entire maze map.
 * @post Maze map should be deallocated and empty.
 * @see load_map
 */
void free_map(Map *map);

/** 
 * @brief Saves informations about maze to the Map structure.
 * @details Data about map - its rows, columns or element definitions are in external file. Everything is saved into the Map structure without checking correct data format - checking only if there are numbers.
 * @param filename path to file which contains definition of the maze map
 * @param map structure of the maze to be allocated
 * @pre Expects non-empty input file containing only number characters and the maze map definition. 
 * @pre User should have enough RAM (depending on maze size) for successful allocation.
 * @post Maze map and its data should be stored into the structure.
 * @return ZERO value if data were stored into the structure and memory allocation was successful. Returns NON-ZERO value if allocation was not successful or if the file contains illegal characters (= everything except numbers).
 * @see check_map
 * @see load_and_check_map
 */
int load_map(const char *filename, Map *map);

/**
 * @brief Checks if element at [r, c] position has border at border (specified side).
 * @param map structure of the maze
 * @param r element 'x' position - row
 * @param c element 'y' position - column
 * @param border what border has to be checked (left, right, top, bottom)
 * @pre Expects successfully allocated maze map and existing structure.
 * @pre Element position should be correct and the element has to exist and be correctly defined.
 * @post Alghorithm should know if it can go through or not.
 * @return TRUE if there is border or FALSE if there is not.
 * @see start_border
 */
bool isborder(Map *map, int r, int c, int border);

/**
 * @brief Checks if element at [r, c] position has bottom border or top border.
 * @param r element 'x' position - row
 * @param c element 'y' position - column
 * @pre Expects correct element position and element should exist.
 * @post Alghorithm should know if is possible to go up/down and if there is border or not.
 * @return TRUE if element has border at bottom, otherwise returns FALSE.
 * @see start_border
 */
bool hasbottom(int r, int c);

/**
 * @brief Function checks the first element and returns starting border of the first element when entering the maze.
 * @details When entering the maze, there could be more possible ways where to go and what path to choose. This function is defining how to behave when entering the maze and what border should it follow when searching the way out.
 * @param map structure of the maze map
 * @param r element 'x' position - row
 * @param c element 'y' position - column
 * @param leftright which rule is program following
 * @note The program can follow left-hand or right-hand rule. Depending on used rule, program finds a way out differently - with right-hand rule, searching alghorithm follows right borders and tries to go to the right.
 * @pre Expects successfully allocated maze map and existing Map structure.
 * @pre Element at 'r' row and 'c' column should exist
 * @post Alghorithm should know how to continue and find exit.
 * @return ZERO or POSITIVE value what border to follow (aka starting border) or NEGATIVE value if some error occured.
 * @see isborder
 */
int start_border(Map *map, int r, int c, int leftright);

/**
 * @brief Checks validity of the maze.
 * @details Data should be in correct format - should not contain illegal characters (= everything except numbers and spaces). But the main reason of this function is check valid definition of the maze map elements. For example element with value 0 next to the element with value 7 is not correct definition, because first element has no border at right side and second element has border at left side, so now can't be told if path-finding alghorithm can go through or not.
 * @param map structure of the maze map
 * @pre Expects successfully allocated maze map and existing structure.
 * @post Program can start searching the path out or not - depending on result of this function.
 * @return ZERO value if maze is defined correctly or NON-ZERO value if maze is defined incorrectly. If returned value is not zero, it returns a number (position) of the first element, which is defined incorrectly.
 * @see load_map
 * @see load_and_check_map
 */
int check_map(Map *map);

/**
 * @brief Saves data of the maze map into the Map structure.
 * @param filename path to the file which contains definition of the maze
 * @param map structure of the maze map, which is going to be allocated
 * @pre Expects correct path to the file and existing structure of the map.
 * @post Program should know validity of the maze and can print 'Valid.' or 'Invalid.' for '--test' argument.
 * @return ZERO value if maze was allocated successfully and it's defined correctly. Returns NON-ZERO value if an error occured - NEGATIVE value when allocation has failed or POSITIVE value when map elements are defined incorrectly.
 * @note This function does the same job as functions 'load_map' and 'check_map'. These functions only do the job separately. Look into their documentation for more information.
 * @see load_map
 * @see check_map
 */
int load_and_check_map(const char *filename, Map *map);

/**
 * @brief Function checks, if program is or is not out of the maze => found the exit.
 * @param map structure of the maze map
 * @param r current 'x' position - row
 * @param c current 'y' position - column
 * @pre Expects successfully allocated maze.
 * @post Alghorithm should know if it has to be stopped (= stop searching way out because now I am) or if it has to continue.
 * @return TRUE if position [r, c] is out of the maze (and exit has been found) or FALSE if that position is still inside the maze.
 * @see print_path
 */
bool is_out(Map *map, int r, int c);

/**
 * @brief Prints path (row and column) of the alghorithm followed by left or right-hand rule.
 * @param map structure of the maze map
 * @param r 'x' position - row
 * @param c 'y' position - column
 * @param leftright left-hand or right-hand rule value
 * @pre Expect successfully allocated maze and correct position (numbers).
 * @post The [r, c] position is printed to the stdout in format: 'r, c'
 */
void print_path(Map *map, int r, int c, int leftright);

#endif
