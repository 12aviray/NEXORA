NEXORA is a high-performance, terminal-based productivity suite written in C.
It combines Task Management, Note-Taking, and an Automated Reminder System into a single lightweight application.

Designed for speed and efficiency, NEXORA supports multi-user environments, smart fuzzy search, and background processing—all while remaining fully terminal-driven.

✨ Key Features
🔐 Multi-User System

Secure user registration and login

Complete data isolation for each user

✅ Task Board

Create, delete, and track tasks

Real-time progress bars for productivity tracking


📝 Smart Notes

Folder-based note management

Notes saved as organized .txt files


🔍 Smart Search

Uses the Levenshtein Distance Algorithm

Finds tasks or notes even with typos or partial matches

⏰ Background Alarms

Set reminders and alarms

Runs using multithreading, allowing uninterrupted workflow

Alerts trigger notifications and voice output


🧩 System Integration

Linux Notifications: notify-send
Voice Alerts: espeak-ng


🛠 Technical Highlights

Multithreading (Pthreads)
Handles background alarms without blocking the main UI

Thread Safety
Mutex locks prevent race conditions and data corruption

Dynamic File Naming
Automatically generates user-specific directories and binary data files

Fuzzy Logic Search
Approximate string matching for a smarter search experience

📁 Project Structure
├── main.c        # Dashboard and main menu logic
├── auth.c        # User authentication (login & registration)
├── tasks.c       # Task management & progress tracking
├── notes.c       # Note-taking system & file I/O
├── reminders.c   # Multithreaded alarms & notifications
├── shared.c      # UI helpers & string algorithms
├── shared.h      # Global structures & function prototypes
└── README.md     # Project documentation


▶️ How to Use

Register
Create a new account on the first run.

Dashboard
View your task progress and navigate between features.

Smart Search
Use option [6] or [12] to perform approximate searches.

Set an Alarm
Enter a task ID and time (e.g., 05:30 PM).
A background thread will notify you and speak when the time arrives.


👤 Author

Avishek Ray
Github:https://github.com/12aviray
