#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILE_NAME "training.csv"
#define MAX_RECS 2000

typedef struct {
    char TraineeName[80];
    char CourseName[80];
    char TrainingDate[20];
    char TrainingLocation[80];
} TrainingRecord;

static void chomp(char *s) {
    s[strcspn(s, "\r\n")] = 0;
}

static void trim(char *s) {
    size_t i = 0, n = strlen(s);
    while (i < n && isspace((unsigned char)s[i])) i++;
    if (i) memmove(s, s+i, n-i+1);
    n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n-1])) s[--n] = 0; 
}

static int has_comma(const char *s) {
    return strchr(s, ',') != NULL;
}

static void read_line(const char *prompt, char *buf, size_t cap, int no_comma) {
    for (;;) {
        printf("%s", prompt);
        if (!fgets(buf, (int)cap, stdin)) { buf[0] = 0; }
        chomp(buf); trim(buf);
        if (no_comma && has_comma(buf)){
            printf("Error: field must not contain comma (,).\n");
            continue;
        }
        if(buf[0] == 0) {
            printf("Error: field cannot be empty.\n");
            continue;
        }
        return;
    }
}