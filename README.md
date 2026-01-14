# NEXORA
NEXORA is a high-performance, terminal-based productivity suite written in C.
It combines Task Management, Note-taking, and an Automated Reminder system into one lightweight application.
It features multi-user support, smart search algorithms, and background processing.
<br>

## **Key Features**<br>
       1.**Multi-User System**: Secure login/registration with independent data isolation for every user.<br>
       2.**Task Board**: Create, delete, and track tasks with real-time Progress Bars.<br>
       3.**Smart Notes**: A folder-based note system that saves your thoughts into organized .txt files.<br>
       4.**Smart Search**: Uses the Levenshtein Distance Algorithm to find tasks/notes even if you make a typo.<br>
       5.**Background Alarms**: Set reminders or alarms using Multithreading. The program alerts you while you continue working.<br><br>         

## **System Integration**:<br>
       **Linux**: Native notify-send pop-ups and espeak-ng voice alerts.<br>
          <br><br>
## **Technical Highlights**<br>
       **Multithreading (Pthreads)**: Used to handle background alarms without freezing the main UI.<br>
       **Dynamic File Naming**: Automatically generates user-specific binary data and directory structures at runtime.<br>
       **Fuzzy Logic**: Implements string matching algorithms for an "approximate" search experience.<br>
       **Thread Safety**: Implements Mutex Locks to prevent data corruption during concurrent operations<br>
           <br>
## **Project Structure**<br>
        ├── main.c           # Dashboard and Main Menu Logic<br>
        ├── auth.c           # User Authentication (Login/Register)<br>
        ├── tasks.c          # Task Management & Progress Tracking<br>
        ├── notes.c          # Note-taking System & File I/O<br>
        ├── reminders.c      # Multithreaded Alarms & Notifications<br>
        ├── shared.c         # UI Helpers and String Algorithms<br>
        ├── shared.h         # Global Structures and Prototypes<br>
        └── README.md        # Project Documentation<br>
<br>

## **How to Use**<br>
      **1. Register**: Create a new account on your first run.<br>
      **2.Dashboard**: View your progress bar and navigate through tasks or notes.<br>
      **3.Smart Search**: Use option [6] or [12] to search using approximate names.<br>
      **4.Set Alarm**: Enter a task ID and time (e.g., 05:30 PM). A background thread will trigger a notification and speak to you when the time comes!<br>
   <br><br>

### **Author: Avishek Ray**

