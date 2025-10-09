
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define static 
#define main main_shadow

#undef FILE_NAME
#define FILE_NAME "e2e_training.csv"

#include "Finalproject.c"
#undef main
#undef static

static void reset_csv(void) { remove(FILE_NAME); }

static void seed_data(void) {
    FILE *f = fopen(FILE_NAME, "w");
    assert(f);
    fprintf(f, "T001,Jane Smith,Data Science,2025-08-03,Room B\n");
    fclose(f);
}

int main(void) {
    printf(" เริ่มทดสอบ E2E Finalproject.c \n");

    reset_csv();
    seed_data();


    TrainingRecord recs[MAX_RECS];
    int count = readAll(recs, MAX_RECS);
    strcpy(recs[count].TraineeName, "John Miller");
    strcpy(recs[count].CourseName, "Leadership Skills");
    strcpy(recs[count].TrainingDate, "2025-08-01");
    strcpy(recs[count].TrainingLocation, "Room A");
    count++;
    writeAll(recs, count);
    assert(count == 2);
    printf(" Add: เพิ่มข้อมูลใหม่สำเร็จ (%d รายการ)\n", count);


    TrainingRecord r2[MAX_RECS];
    int n = readAll(r2, MAX_RECS);
    int found = -1;
    for (int i = 0; i < n; ++i)
        if (equals_icase(r2[i].TraineeName, "John Miller")) found = i;
    assert(found >= 0);
    printf(" Search: พบ John Miller index=%d\n", found);


    strcpy(r2[found].TrainingLocation, "Room Z");
    writeAll(r2, n);
    TrainingRecord chk[MAX_RECS];
    int m = readAll(chk, MAX_RECS);
    int updated = 0;
    for (int i = 0; i < m; ++i)
        if (equals_icase(chk[i].TraineeName, "John Miller") &&
            strcmp(chk[i].TrainingLocation, "Room Z") == 0)
            updated = 1;
    assert(updated);
    printf(" Update: Location เปลี่ยนเป็น Room Z แล้ว\n");


    int kept = 0;
    for (int i = 0; i < m; ++i) {
        if (equals_icase(chk[i].TraineeName, "John Miller")) continue;
        chk[kept++] = chk[i];
    }
    writeAll(chk, kept);
    TrainingRecord fin[MAX_RECS];
    int z = readAll(fin, MAX_RECS);
    int john_exists = 0;
    for (int i = 0; i < z; ++i)
        if (equals_icase(fin[i].TraineeName, "John Miller")) john_exists = 1;
    assert(!john_exists);
    printf(" Delete: ลบ John Miller แล้ว (เหลือ %d รายการ)\n", z);


    printf("\n แสดงข้อมูลสุดท้ายในไฟล์:\n");
    showAll();

    printf("\n E2E Test ผ่านทั้งหมด!\n");
    return 0;
}
