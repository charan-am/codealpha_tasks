
# CampusLedger – Student Record Management System

![Language](https://img.shields.io/badge/Language-C-blue)
![Compiler](https://img.shields.io/badge/Compiler-GCC-orange)
![Internship](https://img.shields.io/badge/Internship-CodeAlpha-green)

## About the Project

CampusLedger is a menu-driven Student Record Management System developed using C programming as part of the CodeAlpha C Programming Internship.

It allows users to create, search, update, delete and display student records. Student information is stored permanently using binary file handling.

## Features

- Add new student records
- Display all active students
- Search students using unique student ID
- Update existing student information
- Delete student records using logical deletion
- Academic dashboard
- Calculate average CGPA
- Identify the top-performing student
- Input validation
- Permanent data storage using file handling

## Technologies Used

- C Programming
- GCC Compiler
- Structures
- Functions
- File Handling
- Binary File Operations
- Git and GitHub

## How to Compile

Install GCC and execute:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 main.c -o campusledger
```

## How to Run

On Ubuntu / WSL:

```bash
./campusledger
```

## Database Storage

The application automatically creates a binary database file:

`students.dat`

Student records remain available after the program exits.

Deleted records are marked inactive rather than physically removed.

The database is stored locally and is intentionally excluded from this public repository.

## Project Structure

```text
CodeAlpha_StudentManagementSystem/
├── main.c
├── README.md
├── .gitignore
└── screenshots/
    ├── add-student.png
    ├── display-students.png
    ├── search-student.png
    ├── update-student.png
    ├── delete-student.png
    ├── academic-dashboard.png
    └── data-persistence.png
```

## Screenshots

### 1. Add Student
![Add Student](screenshots/add-student.png)

### 2. Display Student Records
![Display Students](screenshots/display-students.png)

### 3. Search Student
![Search Student](screenshots/search-student.png)

### 4. Update Student
![Update Student](screenshots/update-student.png)

### 5. Delete Student
![Delete Student](screenshots/delete-student.png)

### 6. Academic Dashboard
![Academic Dashboard](screenshots/academic-dashboard.png)

### 7. Data Persistence
![Data Persistence](screenshots/data-persistence.png)

## Concepts Demonstrated

- Structures
- Functions
- Arrays and strings
- Loops and conditional statements
- Switch-case
- Binary file handling
- Record searching and updating
- Logical deletion
- Input validation
- Modular programming

## Testing

The application was tested for:

- Adding student records
- Displaying student information
- Searching by student ID
- Updating existing records
- Deleting student records
- Academic statistics and average CGPA
- Data persistence after restarting the application

## Internship Information

**Organization:** CodeAlpha  
**Domain:** C Programming  
**Batch:** 10 October – 10 November 2026

## Author

Charan A M
