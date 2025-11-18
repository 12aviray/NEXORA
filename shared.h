#ifndef SHARED_H
#define SHARED_H

// এই ম্যাক্রোটি strptime() ফাংশন এনাবল করার জন্য আবশ্যিক।
// এটি অবশ্যই অন্য লাইব্রেরি ইনক্লুড করার আগে থাকতে হবে।
#define _XOPEN_SOURCE 700 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>      // For time_t, struct tm, localtime, strftime, strptime, mktime
#include <stdbool.h>   // For bool type
#include <pthread.h>   // For multithreading (reminders)
#include <unistd.h>    // For sleep, access
#include <dirent.h>    // For listing directory contents (notes)
#include <sys/stat.h>  // For mkdir (notes)
#include <errno.h>     // For errno (notes)

// --- GLOBAL CONSTANTS ---
#define MAX_TASK_LENGTH 256
#define MAX_DESCRIPTION_LENGTH 512
#define MAX_TASKS 100
#define TODO_FILE "tasks.dat" // File to save/load tasks

#define MAX_NOTE_TITLE 100
#define MAX_NOTE_CONTENT 1024
#define NOTES_DIR_NAME "notes" // Directory for notes

// --- GLOBAL DATA STRUCTURES ---

// Structure to hold a To-Do item
typedef struct {
    int id;
    char title[MAX_TASK_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    time_t due_date; // Unix timestamp
    bool completed;
    bool has_reminder;
    time_t reminder_time;
} TodoItem;

// --- GLOBAL VARIABLES (EXTERN) ---
// 'extern' মানে হলো ভেরিয়েবলগুলো এখানে ডিক্লেয়ার করা হয়েছে, 
// কিন্তু এদের মেমোরি shared.c ফাইলে তৈরি হবে।

extern TodoItem todo_list[MAX_TASKS];
extern int todo_count;
extern pthread_mutex_t todo_list_mutex;

// --- HELPER FUNCTION PROTOTYPES ---
void clear_stdin_buffer();

#endif