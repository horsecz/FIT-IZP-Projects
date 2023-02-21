/*
 * IZP:             PROJECT 1
 * 
 * @author:         Dominik Horky
 * @login:          xhorky32
 * @academic year:  2019/20
 * @lecture group:  1BIT
 * @about:          This program finds contacts and its numbers from standard input.
 *                  User filters results by typing numbers in program's first argument.
 *                  Each number represents itself and some characters - like on the old cellphones (T9 keyboard layout).
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#define MAX_COLUMN_SIZE 101
#define EXCEED -1
#define ARRAY_ZERO_CHARACTER -1

// save stdin to array
int saveToArray(char *data)
{
    int input_character = fgetc(stdin);    
    int total_characters = 0;
    int column = 0;
    if (input_character == EOF)
    {
        return false;
    }
    
    while (input_character != EOF && column == 0)
    {          
        if (input_character == '\n')
        {
            column += 1;
        }
        else
        {
            data[total_characters] = tolower(input_character);
            total_characters += 1;
        }
        if (total_characters > MAX_COLUMN_SIZE-1)
        {
            return EXCEED;
        }
        
        if (column == 1)
        {
            return total_characters;
        }
        else
        {
            input_character = fgetc(stdin);
        }
    }
    
    return total_characters;
}

// print one contact (2 columns of stdin) to stdout
void printColumns(int array1_chars, int array2_chars, char *array1, char *array2)
{
    for (int i = 0; i < array1_chars; i += 1)
    {
        fprintf(stdout, "%c", array1[i]);
    }
    fprintf(stdout, ", ");
    
    for (int i = 0; i < array2_chars; i += 1)
    {
        fprintf(stdout, "%c", array2[i]);
    }
    fprintf(stdout, "\n");
}

// output contacts till EOF or given signal & check if column size (=characters) did not exceed MAX_COLUMN_SIZE (otherwise stop)
void printAllContacts(int array1_chars, int array2_chars, char *array1, char *array2)
{
    int exceed = false;
    for (;array1_chars != false || array2_chars != false;)
    {
        if (array1_chars == EXCEED || array2_chars == EXCEED)
        {
            exceed = true;
        }
        else
        {
            printColumns(array1_chars, array2_chars, array1, array2);
        }
        array1_chars = saveToArray(array1);
        array2_chars = saveToArray(array2);
    }
    
    if (exceed == true)
    {
        fprintf(stdout, "\nWarning:\nInput contains columns with more than 100 characters, output may not be correct !\n");
    }
}

// counts characters in char array
int getArrayCharSize(char *array)
{
    int size = 0;
    for (int i = 0; array[i] != '\0'; i++)
    {
        size += 1;
    }
    
    return size;
}      

// returns array with lowercase characters only
char *charToLowerCase(char *array)
{
    static char modified_array[MAX_COLUMN_SIZE];
    for (int i = 0; array[i] != '\0'; i++)
    {
        modified_array[i] = tolower(array[i]);
    }
    
    return modified_array;
}

// converts one character (A-Z or +) to number
char convertLetterToNumber(char letter)
{
    char char_number;
    switch (letter)
    {
        case 'a' ... 'c':
            char_number = '2';
            break;
        case 'd' ... 'f':
            char_number = '3';
            break;
        case 'g' ... 'i':
            char_number = '4';
            break;
        case 'j' ... 'l':
            char_number = '5';
            break;
        case 'm' ... 'o':
            char_number = '6';
            break;
        case 'p' ... 's':
            char_number = '7';
            break;
        case 't' ... 'v':
            char_number = '8';
            break;
        case 'w' ... 'z':
            char_number = '9';
            break;
        case '+':
            char_number = ARRAY_ZERO_CHARACTER;
            break;
        default:
            char_number = letter;
            break;
    }
    
    return char_number;
}

// returns array with letters (converted from numbers by alghorithm)
char *arrayLettersToNumbers(char *array)
{
    char converted;
    static char converted_array[MAX_COLUMN_SIZE];
    for (int i = 0; array[i] != '\0'; i++)
    {
        converted = convertLetterToNumber(array[i]);
        if (converted != false)
        {
            converted_array[i] = converted;
        }
    }
    
    return converted_array;
}

// replace + and 0 chars with -1
char *arrayConvertNumbers(char *array)
{
    static char converted_numbers[MAX_COLUMN_SIZE];
    for (int i = 0; array[i] != '\0'; i++)
    {
        if (array[i] == '+' || array[i] == '0')
        {
            converted_numbers[i] = ARRAY_ZERO_CHARACTER;
        }
        else
        {
            converted_numbers[i] = array[i];
        }
    }
    
    return converted_numbers;
}

// look for same characters / numbers in stdin, compare with search-argument and print matching contacts
void findMatches(int array1_chars, int array2_chars, char *array1, char *array2, char *search_argument)
{
    char *search = charToLowerCase(search_argument);
    char *array1_converted = arrayLettersToNumbers(array1);
    char *array2_converted = arrayConvertNumbers(array2);
    
    char *comparsion_array1 = strstr(array1, search);
    char *comparsion_array2 = strstr(array2, search);
    char *comparsion_array1_converted = strstr(array1_converted, search);
    char *comparsion_array2_converted = strstr(array2_converted, search);
    int matches = 0;
    while (array1_chars != false || array2_chars != false)
    {
        if (array1_chars == EXCEED || array2_chars == EXCEED)
        {
            fprintf(stdout, "\nWarning:\nInput contains columns with more than 100 characters, so result may not be correct !\n");
            break;
        }
        if (comparsion_array1 != NULL || comparsion_array2 != NULL || comparsion_array1_converted != NULL || comparsion_array2_converted != NULL)
        {
            printColumns(array1_chars, array2_chars, array1, array2);
            matches += 1;
        }
        array1_chars = saveToArray(array1);
        array2_chars = saveToArray(array2);
        array1_converted = arrayLettersToNumbers(array1);
        array2_converted = arrayConvertNumbers(array2);
        
        comparsion_array1 = strstr(array1, search);
        comparsion_array2 = strstr(array2, search);
        comparsion_array1_converted = strstr(array1_converted, search);
        comparsion_array2_converted = strstr(array2_converted, search);
    }
    
    if (!matches)
    {
        fprintf(stdout, "Not Found.\n");
    }
}

// check if argument - array contains only number characters and convert 0 character to -1 to avoid sideeffects
int hasArrayNumbersOnly(char *array)
{
    int result = true;
    for (int i = 0; array[i] != '\0'; i++)
    {
        if (array[i] >= '0' && array[i] <= '9')
        {
            result = true;
            if (array[i] == '0')
            {
                array[i] = ARRAY_ZERO_CHARACTER;
            }
        }
        else
        {
            result = false;
            break;
        }
    }
    
    return result;
}

// look for arguments & check them, check if input is not empty
int main(int argc, char **argv)
{
    if (argc > 2)
    {
        fprintf(stderr, "Error: \nToo much arguments or bad usage. Use: ./proj1 [numbers]\n");
        return 1;
    }

    char contacts[MAX_COLUMN_SIZE];
    char numbers[MAX_COLUMN_SIZE];
    int contacts_chars = saveToArray(contacts);
    int numbers_chars = saveToArray(numbers);
    if (!contacts_chars || !numbers_chars)
    {
        fprintf(stderr, "Not Found.\nEmpty or bad input.\n");
        return 1;
    }    
    
    if (argc == 1)
    {        
        printAllContacts(contacts_chars, numbers_chars, contacts, numbers);
    }
    else if (argc == 2)
    {
        if (getArrayCharSize(argv[1]) > MAX_COLUMN_SIZE-1)
        {
            fprintf(stderr, "Not Found.\nTry using less characters in first argument!\n");
            return 1;
        }
        if (hasArrayNumbersOnly(argv[1]) == false)
        {
            fprintf(stderr, "Not Found.\nUse numbers only!\n");
            return 1;
        }
        findMatches(contacts_chars, numbers_chars, contacts, numbers, argv[1]);
            
    }
    
    return 0;
}
