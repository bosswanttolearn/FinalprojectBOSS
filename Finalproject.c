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
        if (!fgets(buf, (int)cap, stdin)) { 
            printf("\nInput closed.\n");
            exit(1);
        }
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
    fscanf(fp, "%79[^,],%79[^,],%19[^,],%79[^\n]\n",
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


void addRecord(void) {
    FILE *fp = fopen(FILE_NAME, "a");
    if (!fp) { printf("Cannot open file.\n"); return; }

    TrainingRecord r;
    read_line("Enter Trainee Name: ", r.TraineeName, sizeof(r.TraineeName), 1);
    read_line("Enter Course Name: ", r.CourseName, sizeof(r.CourseName), 1);
    read_line("Enter Training Date (YYYY-MM-DD): ", r.TrainingDate, sizeof(r.TrainingDate), 1);
    read_line("Enter Training Location: ", r.TrainingLocation, sizeof(r.TrainingLocation), 1);

    fprintf(fp, "%s,%s,%s,%s\n", r.TraineeName, r.CourseName, r.TrainingDate, r.TrainingLocation);
    fclose(fp);
    printf("Record added successfully!\n");
}

void showAll(void) {
    TrainingRecord recs[MAX_RECS];
    int count = readAll(recs, MAX_RECS);
    if (count == 0) { printf("No records.\n"); return; }

    printf("\n%-20s | %-20s | %-12s | %-20s\n",
           "TraineeName", "CourseName", "Date", "Location");
    printf("-------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-20s | %-20s | %-12s | %-20s\n",
               recs[i].TraineeName, recs[i].CourseName,
               recs[i].TrainingDate, recs[i].TrainingLocation);
    }
}

void searchRecord(void) {
    char key[80];
    read_line("Search by TraineeName or CourseName: ", key, sizeof(key), 0);

    TrainingRecord recs[MAX_RECS];
    int count = readAll(recs, MAX_RECS);
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (strstr(recs[i].TraineeName, key) || strstr(recs[i].CourseName, key)) {
            printf("%s | %s | %s | %s\n",
                   recs[i].TraineeName, recs[i].CourseName,
                   recs[i].TrainingDate, recs[i].TrainingLocation);
            found = 1;
        }
    }
    if (!found) printf("No matching record found.\n");
}

void updateRecord(void) {
    char name[80];
    read_line("Enter TraineeName to update: ", name, sizeof(name), 0);

    TrainingRecord recs[MAX_RECS];
    int count = readAll(recs, MAX_RECS);
    int idx = -1;

    for (int i = 0; i < count; i++) {
        if (strcmp(recs[i].TraineeName, name) == 0) { idx = i; break; }
    }
    if (idx == -1) { printf("Record not found.\n"); return; }

    char buf[128];
    printf("New Course Name (empty=keep): ");
    if (fgets(buf, sizeof(buf), stdin)) {
        chomp(buf); trim(buf);
        if (strlen(buf) && !has_comma(buf))
            strcpy(recs[idx].CourseName, buf);
    }

    printf("New Training Date (YYYY-MM-DD, empty=keep): ");
    if (fgets(buf, sizeof(buf), stdin)) {
        chomp(buf); trim(buf);
        if (strlen(buf)) {
            if (!has_comma(buf)) {
                strcpy(recs[idx].TrainingDate, buf);
            } else {
                printf("Skip: date contains comma.\n");
            }
        }
    }

    printf("New Training Location (empty=keep): ");
    if (fgets(buf, sizeof(buf), stdin)) {
        chomp(buf); trim(buf);
        if (strlen(buf)) {
            if (!has_comma(buf)) {
                strcpy(recs[idx].TrainingLocation, buf);
            } else {
                printf("Skip: location contains comma.\n");
            }
        }
    }

    writeAll(recs, count);
    printf("Record updated.\n");
}

void deleteRecord(void) {
    char name[80];
    read_line("Enter TraineeName to delete: ", name, sizeof(name), 0);

    TrainingRecord recs[MAX_RECS];
    int count = readAll(recs, MAX_RECS);
    int idx = -1;

    for (int i = 0; i < count; i++) {
        if (strcmp(recs[i].TraineeName, name) == 0) { idx = i; break; }
    }
    if (idx == -1) { printf("Record not found.\n"); return; }

    for (int j = idx; j < count-1; j++) recs[j] = recs[j+1];
    count--;

    writeAll(recs, count);
    printf("Record deleted.\n");
}


void displayMenu(void) {
    int choice;
    do {
        printf("\n================= TRAINING MANAGEMENT MENU =================\n");
        printf(" 1. Show all records\n");
        printf(" 2. Add new record\n");
        printf(" 3. Search record\n");
        printf(" 4. Update record\n");
        printf(" 5. Delete record\n");
        printf(" 0. Exit\n");
        printf("============================================================\n");
        printf("Choice: ");

        char line[32];
        if (!fgets(line, sizeof(line), stdin)) return;
        choice = atoi(line);

        switch (choice) {
            case 1: showAll(); break;
            case 2: addRecord(); break;
            case 3: searchRecord(); break;
            case 4: updateRecord(); break;
            case 5: deleteRecord(); break;
            case 0: printf("Exiting program.\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

int main(void) {
    displayMenu();
    return 0;
}
