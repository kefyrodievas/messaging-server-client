#ifndef STR_H
#define STR_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// #include <assert.h>

// Gets a portion of a string from pos with length of len
char *substr(const char *str, size_t pos, size_t len) {
    if (len == 0) { return NULL; }
    if (len < 0) {
        len *= -1;
        pos -= len;
    }
    char *ret = (char *)malloc(len * sizeof(char));
    memcpy(ret, str + pos, len);
    return ret;
}

// Finds the first match of a character in str
// Returns -1 if not found
int findf(const char *str, char c) {
    char *ret = strchr(str, c);
    if (ret == NULL) { return -1; }
    return (ret - str + 1);
}

// Finds the last match of a character in str
// Returns -1 if not found
int findl(const char *str, char c) {
    char *ret = strrchr(str, c);
    if (ret == NULL) { return -1; }
    return (ret - str + 1);
}

// Replaces a part of str1 with str2
char *replace(char *str1, size_t pos, size_t len, const char *str2) {
    // assert(pos > 0);
    if (len == 0) { return str1; }
    if (len < 0) {
        len *= -1;
        pos -= len;
    }
    int l1 = strlen(str1), l2 = strlen(str2);
    char *ret = (char *)malloc(l1 - len + l2);
    //copies a part of str1 to ret
    memcpy(ret, str1, pos);
    //insterts the replacement string
    memcpy(ret + pos, str2, l2);
    //copies the rest of str1
    memcpy(ret + pos + l2, str1 + pos + len, l1 - (pos + len));
    return ret;
}

// Erases from str starting from pos with length len 
char *erase(char *str, size_t pos, size_t len) {
    // assert(pos > 0);
    if (len == 0) { return str; }
    if (len < 0) {
        len *= -1;
        pos -= len;
    }
    int slen = strlen(str);
    char *ret = (char *)malloc(slen - len);
    //copies a part of str to ret
    memcpy(ret, str, pos);
    //copies the rest of str
    memcpy(ret + pos, str + pos + len, slen - (pos + len));
    return ret;
}

int readline(FILE *stream, char *s, size_t len) {
    int c = EOF;
    size_t i;
    for (i = 0; i < len - 1 && (c = fgetc(stream)) != EOF && c != '\n'; ++i) {
        s[i] = c;
    }
    if (c == EOF) {
        return EOF;
    }
    // if (c == '\n') {
    //     s[i] = c;
    //     ++i;
    // }
    s[i] = '\0';
    return i;
}

#endif