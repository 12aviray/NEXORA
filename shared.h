#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>      
#include <stdbool.h>   
#include <pthread.h>   
#include <dirent.h>    
#include <sys/stat.h>  
#include <ctype.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <strings.h> 

// OS Specific
#ifdef _WIN32
    #include <windows.h>
    #define MKDIR(dir) _mkdir(dir)
    #define SLEEP_SEC(x) Sleep((x) * 1000)
#else
    #define MKDIR(dir) mkdir(dir, 0700)
    #define SLEEP_SEC(x) sleep(x)
#endif

// Colors
#define AC_RESET   "\x1b[0m"
#define AC_RED     "\x1b[31m"
#define AC_GREEN   "\x1b[32m"
#define AC_YELLOW  "\x1b[33m"
#define AC_BLUE    "\x1b[34m"
#define AC_MAGENTA "\x1b[35m"
#define AC_CYAN    "\x1b[36m"
#define AC_WHITE   "\x1b[37m"
#define AC_BOLD    "\x1b[1m"

// Constants
#define MAX_TASKS 100
#define MAX_TASK_LENGTH 256
#define MAX_DESCRIPTION_LENGTH 512
#define MAX_NOTE_TITLE 100
#define MAX_NOTE_CONTENT 2048
#define USER_FILE "users.bin"
#define NOTES_DIR_NAME "notes"

// Structures
typedef struct {
    int id;
    char title[MAX_TASK_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    int priority; 
    bool completed;
    time_t due_date; 
    bool has_reminder;
    time_t reminder_time;
} TodoItem;

typedef struct {
    char username[50];
    char password[50];
} User;

typedef struct {
    int task_id;
    time_t reminder_time;
} ReminderArgs;

// Global Variables
extern TodoItem todo_list[MAX_TASKS];
extern int todo_count;
extern pthread_mutex_t todo_list_mutex;
extern char current_user_name[100];


void print_centered(const char* text, const char* color);
void print_centered_prompt(const char* text, const char* color);
void print_vertical_padding(int content_lines);
int levenshtein(const char *s1, const char *s2);
void clear_stdin_buffer();
void clear_screen();
bool login_system(); 

// Tasks
void add_task();
void view_tasks();
void mark_task_complete();
void delete_task();
void fuzzy_search_tasks();
void save_tasks_to_file();
void load_tasks_from_file();
void print_progress_bar();

// Notes
void create_notes_directory();
void get_user_notes_path(char *path, int size);
void add_note();
void view_notes();
void delete_note();
void edit_note();
void search_notes(); 


void setup_thread_reminder(int task_id, int delay_minutes);
void setup_alarm(int task_id);
void view_tasks_mini();

#endif