#include "s21_cat.h"

int main(int argc, char **argv) {
    if (argc == 1)
        repeat_user_input_mode();
    else if (argc >= 2)
        start_cat_function(argc, argv);
    return 0;
}

void repeat_user_input_mode() {
    int c;
    while ((c = getchar()) != EOF) {
        putchar(c);
    }
}

void start_cat_function(int argc, char **argv) {
    flags_t *flags = create_flags_struct();
    int arg_index = 1;
    while (arg_index < argc &&
    (is_GNU_flags(argv[arg_index]) || is_default_flags(argv[arg_index]))) {
        int ret_val = 0;
        if (is_default_flags(argv[arg_index]))
            ret_val = check_default_flags(flags, argv[arg_index], argv[0]);
        else
            ret_val = check_GNU_flags(flags, argv[arg_index], argv[0]);
        if (ret_val == 0) {
            printf("usage: %s [-benstv] [file ...]\n", argv[0]);
            exit(1);
        }
        if (flags->b && flags->n) {
            flags->n = 0;
        }
        arg_index++;
    }
    go_through_files(argc, argv, arg_index, flags);
    free(flags);
}

flags_t *create_flags_struct() {
    flags_t *flags = malloc(sizeof(flags_t));
    flags->b = 0;
    flags->e = 0;
    flags->E = 0;
    flags->n = 0;
    flags->s = 0;
    flags->t = 0;
    flags->T = 0;
    flags->v = 0;
    return flags;
}

int is_default_flags(char *arg_str) {
    int ret_val = 0;
    if (*arg_str == '-' && *(arg_str + 1) != '-')
        ret_val = 1;
    return ret_val;
}

int is_GNU_flags(char *arg_str) {
    int ret_val = 0;
    if (*arg_str == '-' && *(arg_str + 1) == '-')
        ret_val = 1;
    return ret_val;
}

int check_default_flags(flags_t *flags, char *str_with_flags, char *prog_name) {
    int return_val = 1;
    if (*str_with_flags == '-') str_with_flags++;
    while (*str_with_flags) {
        if (*str_with_flags == 'b') {
            flags->b = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 'e') {
            flags->e = 1;
            flags->v = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 'E') {
            flags->e = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 'n') {
            flags->n = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 's') {
            flags->s = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 't') {
            flags->t = 1;
            flags->v = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 'T') {
            flags->t = 1;
            str_with_flags++;
            continue;
        } else if (*str_with_flags == 'v') {
            flags->v = 1;
            str_with_flags++;
            continue;
        } else {
            printf("%s: illegal option -- %c\n", prog_name, *str_with_flags);
            return_val = 0;
            break;
        }
    }
    return return_val;
}

int check_GNU_flags(flags_t *flags, char *str_with_flags, char *prog_name) {
    int ret_val = 1;
    if (strcmp(str_with_flags, "--number-nonblank") == 0) {
        flags->b = 1;
    } else if (strcmp(str_with_flags, "--number") == 0) {
        flags->n = 1;
    } else if (strcmp(str_with_flags, "--squeeze-blank") == 0) {
        flags->s = 1;
    } else {
        printf("%s: illegal option -- %s\n", prog_name, str_with_flags);
        ret_val = 0;
    }
    return ret_val;
}

void go_through_files(int argc, char **argv, int file_index, flags_t *flags) {
    while (file_index < argc) {
        FILE *fp = fopen(argv[file_index], "r+");
        if (!fp) {
            printf("%s: %s: %s\n", argv[0], argv[file_index], strerror(errno));
            file_index++;
            continue;
        }
        output_file(fp, flags);
        fclose(fp);
        file_index++;
    }
}

void output_file(FILE *fp, flags_t *flags) {
    int ch, new_line = 1;
    unsigned line_counter = 0, was_new_line = 0;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch != '\n')
            was_new_line = 0;
        if (flags->n && new_line) {
            line_counter++;
            printf("%6u\t", line_counter);
            new_line = 0;
        } else if (flags->b && new_line && ch != '\n') {
            line_counter++;
            printf("%6u\t", line_counter);
            new_line = 0;
        }
        if (flags->e && ch == '\n' && (was_new_line <= 1 || !flags->s))
            putchar('$');
        if (flags->s && was_new_line >= 2 && ch == '\n')
            continue;
        if (flags->t && ch == '\t') {
            putchar('^');
            putchar('I');
            continue;
        }
        if (flags->v && ((ch >= 0 && ch <= 31) || (ch >= 127 && ch <= 159))
            && ch != 10 && ch != 9) {
            if (ch >= 0 && ch <= 31) {
                ch += 64;
                printf("^%c", ch);
            } else if (ch >= 127 && ch <= 255) {
                ch -= 64;
                ch == '?' ? printf("^%c", ch) : printf("M-^%c", ch);
            }
            continue;
        }
        putchar(ch);
        if (ch == '\n') {
            new_line = 1;
            was_new_line++;
        }
    }
}
