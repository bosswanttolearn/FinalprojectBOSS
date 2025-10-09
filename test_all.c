
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#ifdef FILE_NAME
#undef FILE_NAME
#endif
#define FILE_NAME "test_training.csv"


#define main main_shadow


#define static 


#include "Finalproject.c"


#undef static
#undef main


static void remove_if_exists(const char* p){ remove(p); }


static void feed_stdin(const char *all_inputs) {
    const char *tmp = "tmp_stdin.txt";
    FILE *f = fopen(tmp, "w");
    assert(f);
    fputs(all_inputs, f);
    fclose(f);
    freopen(tmp, "r", stdin);
}


static void write_csv_4cols_sample(void) {
    FILE *f = fopen(FILE_NAME, "w");
    assert(f);
    fprintf(f, "Jane Smith,Data Science,2025-08-03,Room B\n");
    fprintf(f, "John Doe,Leadership Skills,2025-08-01,Room A\n");
    fclose(f);
}


static void write_csv_5cols_sample(void) {
    FILE *f = fopen(FILE_NAME, "w");
    assert(f);
    fprintf(f, "T001,Jane Smith,Data Science,2025-08-03,Room B\n");
    fprintf(f, "T002,John Doe,Leadership Skills,2025-08-01,Room A\n");
    fclose(f);
}


static void test_trim_chomp_and_basic_helpers(void) {
    char a[64] = "   Hello World   \r\n";
    trim(a);
    assert(strcmp(a, "Hello World") == 0);

    char b[64] = "Hi there\n";
    chomp(b);
    assert(strcmp(b, "Hi there") == 0);

    assert(has_comma("ab,cd") == 1);
    assert(has_comma("abcd") == 0);

    assert(is_letters_or_space("Somchai Prasert") == 1);
    assert(is_letters_or_space("John3") == 0);
}


static void test_icase_helpers(void) {
    assert(equals_icase("Hello","hello") == 1);
    assert(equals_icase("Data","Date") == 0);

    assert(contains_icase("Network Automation", "work") == 1);
    assert(contains_icase("Network Automation", "WORK") == 1);
    assert(contains_icase("Network","x") == 0);
}


static void test_date_and_input_helpers(void) {
    
    assert(valid_date("2024-02-29") == 1);
    assert(valid_date("2025-02-29") == 0);
    assert(valid_date("2025-13-01") == 0);
    assert(valid_date("2025-08-01") == 1);


    char buf[128];
    feed_stdin("bad,comma\nok\n");
    read_line("Enter:", buf, sizeof(buf), 1);
    assert(strcmp(buf, "ok") == 0);


    char datebuf[32];
    feed_stdin("2025-02-30\n2025-08-01\n");
    read_date("Date:", datebuf, sizeof(datebuf));
    assert(strcmp(datebuf, "2025-08-01") == 0);
}


static void test_read_write_and_select(void) {
    remove_if_exists(FILE_NAME);


    write_csv_4cols_sample();
    TrainingRecord recs[16];
    int n = readAll(recs, 16);
    assert(n == 2);
    assert(strcmp(recs[0].TraineeID, "T001") == 0);
    assert(strcmp(recs[1].TraineeID, "T002") == 0);


    writeAll(recs, n);
    TrainingRecord out[16];
    int m = readAll(out, 16);
    assert(m == 2);
    assert(strcmp(out[0].TraineeName, "Jane Smith") == 0);
    assert(strcmp(out[1].TrainingLocation, "Room A") == 0);


    strcpy(recs[0].TraineeName, "Napat Charoen");
    strcpy(recs[1].TraineeName, "Napat Thongdee");
    strcpy(recs[0].TraineeID, "T001");
    strcpy(recs[1].TraineeID, "T002");
    writeAll(recs, 2);

    TrainingRecord rs[16];
    int k = readAll(rs, 16);
    assert(k == 2);


    int idx1 = select_by_name(rs, k, "Napat Thongdee");
    assert(idx1 >= 0 && strcmp(rs[idx1].TraineeID, "T002") == 0);


    feed_stdin("T001\n");
    int idx2 = select_by_name(rs, k, "Napat");
    assert(idx2 >= 0 && strcmp(rs[idx2].TraineeID, "T001") == 0);
}


static void test_crud_interactive(void) {
    remove_if_exists(FILE_NAME);

    FILE *f = fopen(FILE_NAME, "w");
    assert(f);
    fprintf(f, "T001,Jane Smith,Data Science,2025-08-03,Room B\n");
    fclose(f);


    feed_stdin(
        "John Miller\n"
        "Leadership Skills\n"
        "2025-08-01\n"
        "Room A\n"
    );
    addRecord();


    TrainingRecord recs[16]; int n = readAll(recs, 16);
    assert(n == 2);


    feed_stdin("John\n");
    searchRecord();


    feed_stdin(
        "John\n"   
        "\n"       
        "\n"       
        "\n"       
        "Room Z\n" 
    );
    updateRecord();


    TrainingRecord out[16]; int m = readAll(out, 16);
    assert(m == 2);
    int found = -1;
    for (int i = 0; i < m; ++i)
        if (equals_icase(out[i].TraineeName, "John Miller")) found = i;
    assert(found >= 0 && strcmp(out[found].TrainingLocation, "Room Z") == 0);


    feed_stdin("John\n");
    deleteRecord();

    TrainingRecord out2[16]; int d = readAll(out2, 16);
    assert(d == 1);
    assert(equals_icase(out2[0].TraineeName, "Jane Smith"));


    showAll();
}


int main(void) {
    printf("Running ALL unit tests for Finalproject.c ...\n");

    test_trim_chomp_and_basic_helpers();
    test_icase_helpers();
    test_date_and_input_helpers();
    test_read_write_and_select();
    test_crud_interactive();


    remove_if_exists(FILE_NAME);
    remove_if_exists("tmp_stdin.txt");

    printf("✅ All tests passed!\n");
    return 0;
}
