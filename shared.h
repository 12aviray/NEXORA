#ifndef SHARED_H
#define SHARED_H

#define _XOPEN_SOURCE 700 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>      
#include <stdbool.h>   
#include <pthread.h>   
#include <unistd.h>    
#include <dirent.h>    
#include <sys/stat.h>  
#include <errno.h>     

// --- ANSI COLORS FOR UI ---
#define AC_RESET   "\x1b[0m"
#define AC_RED     "\x1b[31m"
#define AC_GREEN   "\x1b[32m"
#define AC_YELLOW  "\x1b[33m"
#define AC_BLUE    "\x1b[34m"
#define AC_MAGENTA "\x1b[35m"
#define AC_CYAN    "\x1b[36m"
#define AC_BOLD    "\x1b[1m"

// --- CONSTANTS ---
#define MAX_TASK_LENGTH 256
#define MAX_DESCRIPTION_LENGTH 512
#define MAX_TASKS 100
#define TODO_FILE "tasks.dat" 
#define MAX_NOTE_TITLE 100
#define MAX_NOTE_CONTENT 1024
#define NOTES_DIR_NAME "notes"

// --- STRUCTURES ---
typedef struct {
    int id;
    char title[MAX_TASK_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    time_t due_date; 
    bool completed;
    bool has_reminder;
    time_t reminder_time;
    int priority; // 1=High, 2=Medium, 3=Low
} TodoItem;

// --- GLOBALS ---
extern TodoItem todo_list[MAX_TASKS];
extern int todo_count;
extern pthread_mutex_t todo_list_mutex;
extern char current_user_name[100];

// --- HELPERS ---
void clear_stdin_buffer();
void clear_screen(); // New helper to wipe screen

#endif