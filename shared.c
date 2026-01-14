#include "shared.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <ctype.h>


TodoItem todo_list[MAX_TASKS];
int todo_count = 0;
pthread_mutex_t todo_list_mutex = PTHREAD_MUTEX_INITIALIZER;
char current_user_name[100] = "";

void clear_stdin_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


void print_centered(const char* text, const char* color) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        
        printf("%s%s" AC_RESET "\n", color, text);
        return;
    }
    
    int len = 0;
    int i = 0;
   
    while (text[i] != '\0') {
        if (text[i] == '\x1b') {
            while (text[i] != 'm' && text[i] != '\0') i++;
        } else {
            len++;
        }
        if (text[i] != '\0') i++;
    }

    int padding = (w.ws_col - len) / 2;
    if (padding < 0) padding = 0;
    for (int k = 0; k < padding; k++) printf(" ");
    printf("%s%s" AC_RESET "\n", color, text);
}

void print_centered_prompt(const char* text, const char* color) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int len = strlen(text);
    int padding = (w.ws_col - 50) / 2; 
    if (padding < 0) padding = 0;
    for (int k = 0; k < padding; k++) printf(" ");
    printf("%s%s" AC_RESET, color, text);
}

void print_vertical_padding(int content_lines) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int v_padding = (w.ws_row - content_lines) / 2;
    if (v_padding < 0) v_padding = 0;
    for (int i = 0; i < v_padding; i++) printf("\n");
}

int levenshtein(const char *s1, const char *s2) {
    int len1 = strlen(s1), len2 = strlen(s2);
    int matrix[len1 + 1][len2 + 1];
    for (int i = 0; i <= len1; i++) matrix[i][0] = i;
    for (int j = 0; j <= len2; j++) matrix[0][j] = j;
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (tolower((unsigned char)s1[i - 1]) == tolower((unsigned char)s2[j - 1])) ? 0 : 1;
            int a = matrix[i - 1][j] + 1;
            int b = matrix[i][j - 1] + 1;
            int c = matrix[i - 1][j - 1] + cost;
            int min = a < b ? a : b;
            matrix[i][j] = min < c ? min : c;
        }
    }
    return matrix[len1][len2];
}