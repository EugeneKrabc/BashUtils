#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

typedef struct flag_struct {
    int b;
    int e;
    int E;
    int n;
    int s;
    int t;
    int T;
    int v;
}flags_t;

void repeat_user_input_mode();
void start_cat_function(int argc, char **argv);
flags_t *create_flags_struct();
int is_default_flags(char *arg_str);
int is_GNU_flags(char *arg_str);
int check_default_flags(flags_t *flags, char *str_with_flags, char *prog_name);
int check_GNU_flags(flags_t *flags, char *str_with_flags, char *prog_name);
void go_through_files(int agrc, char **argv, int file_index, flags_t *flags);
void output_file(FILE *fp, flags_t *flags);

#endif  // SRC_CAT_S21_CAT_H_
