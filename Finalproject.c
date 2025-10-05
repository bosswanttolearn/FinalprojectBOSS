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

int readAll(TrainingRecord recs[], int max) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;

    int count = 0;
    while (count < max &&
    fscanf(fp, "%63[^,],%63[^,],%15[^,],%63[^\n]\n",
        recs[count].TraineeName,
        recs[count].CourseName,
        recs[count].TrainingDate,
        recs[count].TrainingLocation) == 4 ) {
            trim(recs[count].TraineeName);
            trim(recs[count].CourseName);
            trim(recs[count].TrainingDate);
            trim(recs[count].TrainingLocation);
            count++;
        }
    
        fclose(fp);
    return count;
}
void writeAll(TrainingRecord recs[], int count) {
    FILE *fp = fopen(FILE_NAME, "w");
    if (!fp) { printf("Cannot write file.\n"); return; }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%s,%s\n",
                recs[i].TraineeName,
                recs[i].CourseName,
                recs[i].TrainingDate,
                recs[i].TrainingLocation);
    }
    fclose(fp);
}


