#include "s21_grep.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        output_error_and_exit();
    } else {
        start_grep_function(argc, argv);
    }
    return 0;
}

void output_error_and_exit() {
    printf("usage: s21_grep [-eivclnhso] [-f file] [file ...]\n");
    exit(1);
}

void start_grep_function(int argc, char **argv) {
    flags_t *flags = create_flags_struct();
    list_node *keywords_head = NULL;
    list_node *files_head = NULL;
    get_all_flags(argc, argv, flags);
    get_keywords_and_files(argc, argv, &keywords_head, &files_head, flags);
    go_through_files(files_head, keywords_head, flags);
    free(flags);
    free_list(files_head);
    free_list(keywords_head);
}

flags_t *create_flags_struct() {
    flags_t *flags = malloc(sizeof(flags_t));
    flags->e = 0;
    flags->i = 0;
    flags->v = 0;
    flags->c = 0;
    flags->l = 0;
    flags->n = 0;
    flags->h = 0;
    flags->s = 0;
    flags->f = 0;
    flags->o = 0;
    return flags;
}

void get_all_flags(int argc, char **argv, flags_t *flags) {
    for (int i = 1; i < argc; i++) {
        if (is_flags(argv[i]) && !is_e_flag(i - 1, argv)) {
            int status = check_flags(argv[i], flags);
            if (!status)
                output_error_and_exit();
        }
    }
}

int is_flags(const char *arg_str) {
    int ret_value = 0;
    if (*arg_str == '-') {
        ret_value = 1;
    }
    return ret_value;
}

int is_e_flag(int arg_index, char **argv) {
    int ret_val = 0;
    if (arg_index >= 1) {
        int len = strlen(argv[arg_index]);
        if (*(argv[arg_index]) == '-' && *(argv[arg_index] + 1) == 'e' && *(argv[arg_index] + 2) == 0)
            ret_val = 1;
        else if (argv[arg_index][len - 1] == 'e' && argv[arg_index][0] == '-' && argv[arg_index][1] != 'e')
            ret_val = 1;
    }
    return ret_val;
}

int check_flags(char *str_with_flags, flags_t *flags) {
    int return_val = 1;
    if (*str_with_flags == '-') str_with_flags++;  // [-eivclnhso]
    while (*str_with_flags) {
        if (*str_with_flags == 'e') {
            flags->e = 1;
            break;
        } else if (*str_with_flags == 'i') {
            flags->i = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 's') {
            flags->s = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 'v') {
            flags->v = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 'c') {
            flags->c = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 'l') {
            flags->l = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 'n') {
            flags->n = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 'h') {
            flags->h = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 'o') {
            flags->o = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 'f') {
            flags->f = 1;
            break;
        } else {
            printf("s21_grep: invalid option -- %c\n", *str_with_flags);
            return_val = 0;
            break;
        }
    }
    return return_val;
}

list_node *create_node(char *string) {
    list_node *keyword_node = malloc(sizeof(list_node));
    strcpy(keyword_node->name, string);
    keyword_node->next = NULL;
    return keyword_node;
}

void add_element_to_list(list_node *head, char *keyword_str) {
    while (head) {
        if (!head->next) {
            head->next = create_node(keyword_str);
            break;
        } else {
            head = head->next;
        }
    }
}

void free_list(list_node *head) {
    while (head) {
        list_node *tmp = head;
        head = head->next;
        free(tmp);
    }
}

void get_keywords_and_files(int argc, char **argv, list_node **keywords_head,
                            list_node **files_head, flags_t *flags) {
    int meet_keyword = 0;
    for (int i = 1; i < argc; i++) {
        if (flags->f)
            if (handle_f_flag(argc, argv, &i, keywords_head))
                continue;
        if (is_flags(argv[i]) && check_e_in_middle(argv[i], keywords_head, flags))
            continue;
        if ((flags->e && is_e_flag(i - 1, argv)) ||
        (!flags->e && !is_flags(argv[i]) && !meet_keyword && !flags->f)) {
            if (*keywords_head == NULL)
                *keywords_head = create_node(argv[i]);
            else
                add_element_to_list(*keywords_head, argv[i]);
            meet_keyword = 1;
        } else if (!is_flags(argv[i])) {
            if (*files_head == NULL)
                *files_head = create_node(argv[i]);
            else
                add_element_to_list(*files_head, argv[i]);
        }
    }
}

int check_e_in_middle(char *arg_str, list_node **keyword_head, flags_t *flags) {
    int ret_val = 0;
    if (flags->e) {
        while (*arg_str) {
            if (*arg_str == 'e' && *(arg_str + 1) != 0) {
                if (*keyword_head == NULL)
                    *keyword_head = create_node((arg_str + 1));
                else
                    add_element_to_list(*keyword_head, (arg_str + 1));
                ret_val = 1;
                break;
            }
            arg_str++;
        }
    }
    return ret_val;
}

void go_through_files(list_node *files, list_node *keywords, flags_t *flags) {
    int files_nmb = count_files(files);
    while (files) {
        FILE *fp = fopen(files->name, "r+");
        if (!fp) {
            if (!flags->s)
                printf("s21_grep: %s: %s\n", files->name, strerror(errno));
        } else {
            if (flags->o && !flags->c && !flags->l && !flags->v) {
                file_processing_O(fp, keywords, flags, files_nmb, files->name);
            } else {
                file_processing(fp, keywords, flags, files_nmb, files->name);
            }
            fclose(fp);
        }
        files = files->next;
    }
}

void file_processing(FILE *fp, list_node *keywords, flags_t *flags, int files_nmb, char *file_name) {
    char buff[500];
    int line_counter = 0, matches_count = 0, no_matches_count = 0, need_L_print = 0;;
    while (fgets(buff, sizeof(buff), fp)) {
        line_counter++;
        int find = find_keyword_in_string(buff, keywords, flags);
        int len = strlen(buff);
        if (buff[len - 1] != '\n')
            strcpy(&buff[len], "\n");
        if (flags->c && find)
            matches_count++;
        else
            matches_count += find;
        if (find == 0) {
            no_matches_count++;
        }
        if (handle_L_flag(flags, find, &need_L_print))
            break;
        if (((find && !flags->v) || (!find && flags->v)) && !flags->c) {
            if (files_nmb > 1 && !flags->h)
                printf("%s:", file_name);
            if (flags->n)
                printf("%d:", line_counter);
            printf("%s", buff);
        }
    }
    handle_c_flag(flags, matches_count, no_matches_count, files_nmb, file_name);
    if (need_L_print)
        printf("%s\n", file_name);
}

int find_keyword_in_string(char *str, list_node *keyword, flags_t *flags) {
    int ret_val = 0, find, regcomp_result;
    regex_t regex;
    while (keyword) {
        if (flags->i)
           regcomp_result = regcomp(&regex, keyword->name, REG_ICASE);
        else
           regcomp_result = regcomp(&regex, keyword->name, 0);
        if (regcomp_result != 0) {
            keyword = keyword->next;
            regfree(&regex);
            continue;
        }
        find = regexec(&regex, str, 0, NULL, 0);
        if (find == 0)
            ret_val++;
        regfree(&regex);
        keyword = keyword->next;
    }
    return ret_val;
}

int count_files(list_node *files_head) {
    int result = 0;
    while (files_head) {
        result++;
        files_head = files_head->next;
    }
    return result;
}

void handle_c_flag(flags_t *flags, int matches, int no_matches, int files_count, char *file_name) {
    if (flags->c) {
        if (files_count > 1 && !flags->h)
            printf("%s:", file_name);
        if (flags->v)
            printf("%d\n", no_matches);
        else
            printf("%d\n", matches);
    }
}

int handle_L_flag(flags_t *flags, int find, int *need_L_print) {
    int res = 0;
    if (((find && !flags->v) || (!find && flags->v)) && flags->l) {
        res = 1;
        *need_L_print = 1;
    }
    return res;
}

int handle_f_flag(int argc, char **argv, int *i, list_node **keyword_head) {
    int ret_value = 0;
    int len = strlen(argv[*i]);
    char *ptr_to_f;
    if (argv[*i][0] == '-' && ((argv[*i][1] == 'f' && argv[*i][2] == '\0') || argv[*i][len - 1] == 'f')) {
        if (*(i + 1) < argc) {
            add_keywords_from_file(argv[*i + 1], keyword_head);
            *i += 1;
            ret_value = 1;
        } else {
            output_error_and_exit();
        }
    } else if (argv[*i][0] == '-' && (ptr_to_f = strchr(argv[*i], 'f')) && *(ptr_to_f + 1) != '\0') {
        add_keywords_from_file(ptr_to_f + 1, keyword_head);
    }
    return ret_value;
}

void add_keywords_from_file(char *file_name, list_node **keyword_head) {
    FILE *fp = fopen(file_name, "r+");
    if (!fp) {
        printf("s21_grep: %s: %s\n", file_name, strerror(errno));
        exit(2);
    }
    char buff[256];
    while (fgets(buff, sizeof(buff), fp)) {
        int len = strlen(buff);
        if (buff[len - 1] == '\n' && len != 1)
            buff[len - 1] = 0;
        if (*keyword_head == NULL)
            *keyword_head = create_node(buff);
        else
            add_element_to_list(*keyword_head, buff);
    }
}

void file_processing_O(FILE *fp, list_node *keywords, flags_t *flags, int files_nmb, char *file_name) {
    char buff[500];
    int line_nmb = 0;
    while (fgets(buff, sizeof(buff), fp)) {
        line_nmb++;
        int line_printed = 0;
        handle_O_pattern(buff, keywords, file_name, files_nmb, flags, line_nmb, &line_printed);
    }
}

void handle_O_pattern(char *str, list_node *keywords, char *file_name, int files_count,
                        flags_t *flags, int line_nmb, int *line_printed) {
    regex_t regex;
    trim_n(str);
    while (keywords) {
        int comp_res = do_regcomp(&regex, flags, keywords->name);
        if (!comp_res) {
            regmatch_t match;
            size_t offset = 0;
            size_t len = strlen(str);
            int eflags = 0;
            while (regexec(&regex, str + offset, 1, &match, eflags) == 0) {
                eflags = REG_NOTBOL;
                print_o_match(offset, match, str, flags, line_nmb, line_printed, files_count, file_name);
                offset += match.rm_eo;
                if (match.rm_so == match.rm_eo)
                    offset++;
                if (offset > len)
                    break;
            }
        }
        regfree(&regex);
        keywords = keywords->next;
    }
}

void print_o_match(size_t offset, regmatch_t match, char *buff, flags_t *flags,
                    int line_nmb, int *line_printed, int files_count, char *file_name) {
    if (!flags->c && !flags->l) {
        if (files_count > 1 && !flags->h)
            printf("%s:", file_name);
        if (flags->n && !(*line_printed)) {
            printf("%d:", line_nmb);
            *line_printed = 1;
        }
    }
    for (size_t i = offset + match.rm_so; i < offset + match.rm_eo; i++)
        printf("%c", buff[i]);
    printf("\n");
}

int do_regcomp(regex_t *regex, flags_t *flags, char *keyword) {
    int comp_val;
    if (flags->i)
        comp_val = regcomp(regex, keyword, REG_ICASE);
    else
        comp_val = regcomp(regex, keyword, 0);
    return comp_val;
}

void trim_n(char *line) {
    if (strlen(line) == 1 && line[0] == '\n') line[0] = '.';
    if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
}
