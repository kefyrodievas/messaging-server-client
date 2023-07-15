#ifndef STR_H
#define STR_H

#include <stdlib.h>
#include <string.h>

// char *join(const char *str1, const char *str2);
// char *substr(char *string, int start, int count);
// int find(char *string, char character);
// char *replace(char *str1, int start, int end, char *str2);



// // Joins 2 character strings together
// char *join(const char *str1, const char *str2)
// {
//     char *ret = (char*)malloc(strlen(str1) + strlen(str2));
//     strcpy(ret, str1);
//     strcat(ret, str2);
//     return ret;
// }

// Gets a portion of a string
char *substr(char *str, int start, int end)
{
    int length = end - start;
    char *ret = (char*)malloc(length * sizeof(char));
    for (int i = 0; i < length; i++)
    {
        ret[i] = str[i + start];
    }
    return ret;
}

// Finds the 1st match of a character in str
int find(char *str, char character)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (character == str[i])
            return i;
    }
    return -1;
}

// Replaces a part of str1 with str2
char *replace(char *str1, int start, int end, char *str2)
{
    char *ret = (char*)malloc(strlen(str1) - (end - start) + strlen(str2));
    for (int i = 0; i < start-1; i++)
    {
        ret[i] = str1[i];
    }
    strcat(ret, str2);
    strcat(ret, substr(str1, end, strlen(str1)));
    return ret;
}

#endif