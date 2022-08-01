#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <regex.h>
#include <unistd.h>

typedef struct flags {
    int e;
    int i;
    int v;
    int c;
    int l;
    int n;
    int h;
    int s;
    int f;
    int o;
}flags_t;

typedef struct list {
    char name[256];
    struct list *next;
}list_node;

void output_error_and_exit();
void start_grep_function(int argc, char **argv);
flags_t *create_flags_struct();
void get_all_flags(int argc, char **argv, flags_t *flags);
int is_flags(const char *arg_str);
int is_e_flag(int arg_index, char **argv);
int check_flags(char *str_with_flags, flags_t *flags);
list_node *create_node(char *string);
void add_element_to_list(list_node *head, char *keyword_str);
void free_list(list_node *head);
void get_keywords_and_files(int argc, char **argv, list_node **keywords_head,
                            list_node **files_head, flags_t *flags);
int check_e_in_middle(char *arg_str, list_node **keyword_head, flags_t *flags);
void go_through_files(list_node *files, list_node *keywords, flags_t *flags);
void file_processing(FILE *fp, list_node *keywords, flags_t *flags, int files_nmb, char *file_name);
int find_keyword_in_string(char *str, list_node *keyword, flags_t *flags);
int count_files(list_node *files_head);
void handle_c_flag(flags_t *flags, int matches, int no_matches, int files_count, char *file_name);
int handle_L_flag(flags_t *flags, int find, int *need_L_print);
int handle_f_flag(int argc, char **argv, int *i, list_node **keyword_head);
void add_keywords_from_file(char *file_name, list_node **keyword_head);
void file_processing_O(FILE *fp, list_node *keywords, flags_t *flags, int files_nmb, char *file_name);
void handle_O_pattern(char *str, list_node *keywords, char *file_name, int files_count,
                        flags_t *flags, int line_nmb, int *line_printed);
void print_o_match(size_t offset, regmatch_t match, char *buff, flags_t *flags,
                    int line_nmb, int *line_printed, int files_count, char *file_name);
int do_regcomp(regex_t *regex, flags_t *flags, char *keyword);
void trim_n(char *line);
void s21_itoa(int m, char *str);

#endif  // SRC_GREP_S21_GREP_H_
