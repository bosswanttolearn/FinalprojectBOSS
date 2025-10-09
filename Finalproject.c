#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define FILE_NAME "training.csv"
#define MAX_RECS 5000

typedef struct {
    char TraineeID[16];
    char TraineeName[80];
    char CourseName[80];
    char TrainingDate[20];
    char TrainingLocation[80];
} TrainingRecord;


static void chomp(char *s) { s[strcspn(s, "\r\n")] = 0; }

static void trim(char *s) {
    size_t i = 0, n = strlen(s);
    while (i < n && isspace((unsigned char)s[i])) i++;
    if (i) memmove(s, s + i, n - i + 1);
    n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) s[--n] = 0;
}

static int has_comma(const char *s) { return strchr(s, ',') != NULL; }

static int is_letters_or_space(const char *s) {
    for (int i = 0; s[i]; i++)
        if (!isalpha((unsigned char)s[i]) && s[i] != ' ')
            return 0;
    return 1;
}

static void read_line(const char *prompt, char *buf, size_t cap, int no_comma) {
    for (;;) {
        printf("%s", prompt);
        if (!fgets(buf, (int)cap, stdin)) { printf("\nInput closed.\n"); exit(1); }
        chomp(buf); trim(buf);
        if (no_comma && has_comma(buf)) { puts("Error: cannot contain comma (,)."); continue; }
        if (!buf[0]) { puts("Error: field cannot be empty."); continue; }
        return;
    }
}

static void first_token(const char *name, char *out, size_t cap) {
    out[0] = 0;
    if (!name || !*name) return;
    size_t i = 0;
    while (name[i] && name[i] != ' ' && i + 1 < cap) {
        out[i] = name[i];
        i++;
    }
    out[i] = 0;
}

static int contains_icase(const char *text, const char *key) {
    if (!text || !key || !*key) return 0;
    for (const char *p = text; *p; ++p) {
        const char *h = p, *n = key;
        while (*h && *n && tolower((unsigned char)*h) == tolower((unsigned char)*n)) { ++h; ++n; }
        if (*n == '\0') return 1;
    }
    return 0;
}
static int equals_icase(const char *a, const char *b) {
    if (!a || !b) return 0;
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        ++a; ++b;
    }
    return *a == '\0' && *b == '\0';
}


static int is_leap(int y) { return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0); }
static int get_current_year(void) { time_t t = time(NULL); struct tm *tm_info = localtime(&t); return tm_info->tm_year + 1900; }

static int valid_date(const char *s) {
    if (!s || strlen(s) != 10) return 0;
    if (s[4] != '-' || s[7] != '-') return 0;
    for (int i = 0; i < 10; ++i) { if (i == 4 || i == 7) continue; if (!isdigit((unsigned char)s[i])) return 0; }
    int y = atoi(s), m = atoi(s+5), d = atoi(s+8);
    int current_year = get_current_year();
    if (y < 1900 || y > current_year + 5) return 0;
    if (m < 1 || m > 12) return 0;
    int mdays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (m == 2 && is_leap(y)) mdays[2] = 29;
    if (d < 1 || d > mdays[m]) return 0;
    return 1;
}

static void read_date(const char *prompt, char *buf, size_t cap) {
    for (;;) {
        read_line(prompt, buf, cap, 1);
        if (valid_date(buf)) return;
        puts("Invalid date. Please use YYYY-MM-DD (real date).");
    }
}


static void regenerate_ids(TrainingRecord recs[], int count) {
    for (int i = 0; i < count; i++)
        snprintf(recs[i].TraineeID, sizeof(recs[i].TraineeID), "T%03d", i + 1);
}


int readAll(TrainingRecord recs[], int max) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;
    int count = 0;
    char line[1024];
    while (count < max && fgets(line, sizeof(line), fp)) {
        chomp(line);
        if (!line[0]) continue;
        char *tok[5] = {0};
        int i = 0;
        tok[i++] = strtok(line, ",");
        while (i < 5 && (tok[i] = strtok(NULL, ","))) i++;
        if (i < 4) continue;

        for (int k = 0; k < i; k++) if (tok[k]) trim(tok[k]);

        if (i == 5) {
            strcpy(recs[count].TraineeID, tok[0]);
            strcpy(recs[count].TraineeName, tok[1]);
            strcpy(recs[count].CourseName, tok[2]);
            strcpy(recs[count].TrainingDate, tok[3]);
            strcpy(recs[count].TrainingLocation, tok[4]);
        } else {
            recs[count].TraineeID[0] = 0;
            strcpy(recs[count].TraineeName, tok[0]);
            strcpy(recs[count].CourseName, tok[1]);
            strcpy(recs[count].TrainingDate, tok[2]);
            strcpy(recs[count].TrainingLocation, tok[3]);
        }
        count++;
    }
    fclose(fp);
    regenerate_ids(recs, count);
    return count;
}

void writeAll(TrainingRecord recs[], int count) {
    regenerate_ids(recs, count);
    FILE *fp = fopen(FILE_NAME, "w");
    if (!fp) { puts("Cannot write file."); return; }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%s,%s,%s\n",
                recs[i].TraineeID,
                recs[i].TraineeName,
                recs[i].CourseName,
                recs[i].TrainingDate,
                recs[i].TrainingLocation);
    }
    fclose(fp);
}


void addRecord(void) {
    TrainingRecord recs[MAX_RECS];
    int count = readAll(recs, MAX_RECS);
    TrainingRecord r;

    for (;;) {
        read_line("Enter Trainee Name: ", r.TraineeName, sizeof(r.TraineeName), 1);
        if (!is_letters_or_space(r.TraineeName)) { puts("Error: name must contain only letters."); continue; }
        break;
    }
    read_line("Enter Course Name: ", r.CourseName, sizeof(r.CourseName), 1);
    read_date("Enter Training Date (YYYY-MM-DD): ", r.TrainingDate, sizeof(r.TrainingDate));
    read_line("Enter Training Location: ", r.TrainingLocation, sizeof(r.TrainingLocation), 1);

    recs[count++] = (TrainingRecord){"", "", "", "", ""};
    strcpy(recs[count-1].TraineeName, r.TraineeName);
    strcpy(recs[count-1].CourseName, r.CourseName);
    strcpy(recs[count-1].TrainingDate, r.TrainingDate);
    strcpy(recs[count-1].TrainingLocation, r.TrainingLocation);

    writeAll(recs, count);
    puts("Record added successfully!");
}


static int select_by_name(TrainingRecord recs[], int count, const char *input_name) {
    if (strchr(input_name, ' ') != NULL) {
        for (int i = 0; i < count; i++)
            if (equals_icase(recs[i].TraineeName, input_name)) return i;
        return -1;
    }
    int idxs[MAX_RECS], n = 0; char key[80]; strcpy(key, input_name);
    for (int i = 0; i < count; i++) {
        char first[80]; first_token(recs[i].TraineeName, first, sizeof(first));
        if (equals_icase(first, key)) idxs[n++] = i;
    }
    if (n == 0) return -1;
    if (n == 1) return idxs[0];

    printf("\nMultiple matches for \"%s\":\n", key);
    for (int k = 0; k < n; k++) {
        int i = idxs[k];
        printf("  [%d] ID:%s | %s | %s | %s | %s\n",
               k + 1, recs[i].TraineeID,
               recs[i].TraineeName,
               recs[i].CourseName,
               recs[i].TrainingDate,
               recs[i].TrainingLocation);
    }
    char idbuf[32];
    printf("Enter TraineeID (e.g., T002) or 0 to cancel: ");
    if (!fgets(idbuf, sizeof(idbuf), stdin)) return -2;
    chomp(idbuf); trim(idbuf);
    if (!strcmp(idbuf, "0")) return -2;
    for (int k = 0; k < n; k++) { int i = idxs[k]; if (equals_icase(recs[i].TraineeID, idbuf)) return i; }
    puts("Invalid ID.");
    return -2;
}


void showAll(void) {
    TrainingRecord recs[MAX_RECS];
    int count = readAll(recs, MAX_RECS);
    if (count == 0) { puts("No records."); return; }

    printf("\n%-20s | %-25s | %-12s | %-20s\n",
           "TraineeName", "CourseName", "Date", "Location");
    printf("------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++)
        printf("%-20s | %-25s | %-12s | %-20s\n",
               recs[i].TraineeName,
               recs[i].CourseName,
               recs[i].TrainingDate,
               recs[i].TrainingLocation);
}


void searchRecord(void) {
    TrainingRecord recs[MAX_RECS];
    int count = readAll(recs, MAX_RECS);
    char key[80];
    read_line("Search by TraineeName (first/full) or CourseName: ", key, sizeof(key), 0);
    int idx = select_by_name(recs, count, key);
    if (idx >= 0) {
        printf("%s | %s | %s | %s\n",
               recs[idx].TraineeName, recs[idx].CourseName,
               recs[idx].TrainingDate, recs[idx].TrainingLocation);
        return;
    }
    if (idx == -2) return;
    int found = 0;
    for (int i = 0; i < count; i++)
        if (contains_icase(recs[i].TraineeName, key) || contains_icase(recs[i].CourseName, key)) {
            printf("%s | %s | %s | %s\n",
                   recs[i].TraineeName, recs[i].CourseName,
                   recs[i].TrainingDate, recs[i].TrainingLocation);
            found = 1;
        }
    if (!found) puts("No matching record found.");
}

void updateRecord(void) {
    TrainingRecord recs[MAX_RECS];
    int count = readAll(recs, MAX_RECS);
    char input[80];
    read_line("Enter TraineeName (first/full) to update: ", input, sizeof(input), 0);
    int idx = select_by_name(recs, count, input);
    if (idx < 0) { puts("Record not found or cancelled."); return; }

    char buf[128];
    printf("New Course Name (empty=keep): ");
    if (fgets(buf, sizeof(buf), stdin)) { chomp(buf); trim(buf);
        if (strlen(buf) && !has_comma(buf)) strcpy(recs[idx].CourseName, buf); }

    char dbuf[32];
    printf("New Date (YYYY-MM-DD, empty=keep): ");
    if (fgets(dbuf, sizeof(dbuf), stdin)) {
        chomp(dbuf); trim(dbuf);
        if (strlen(dbuf) && valid_date(dbuf)) strcpy(recs[idx].TrainingDate, dbuf);
    }

    printf("New Location (empty=keep): ");
    if (fgets(buf, sizeof(buf), stdin)) { chomp(buf); trim(buf);
        if (strlen(buf) && !has_comma(buf)) strcpy(recs[idx].TrainingLocation, buf); }

    writeAll(recs, count);
    puts("Record updated.");
}

void deleteRecord(void) {
    TrainingRecord recs[MAX_RECS];
    int count = readAll(recs, MAX_RECS);
    char input[80];
    read_line("Enter TraineeName (first/full) to delete: ", input, sizeof(input), 0);
    int idx = select_by_name(recs, count, input);
    if (idx < 0) { puts("Record not found or cancelled."); return; }
    for (int j = idx; j < count - 1; j++) recs[j] = recs[j + 1];
    count--;
    writeAll(recs, count);
    puts("Record deleted.");
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
            case 0: puts("Exiting program."); break;
            default: puts("Invalid choice.");
        }
    } while (choice != 0);
}

int main(void) {
    displayMenu();
    return 0;
}
