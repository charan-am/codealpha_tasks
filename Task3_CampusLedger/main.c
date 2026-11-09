
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

#define DATA_FILE "students.dat"
#define MAX_LINE 256
#define NAME_SIZE 60
#define COURSE_SIZE 50

/* Student record structure. */
typedef struct
{
    int id;
    char name[NAME_SIZE];
    char course[COURSE_SIZE];
    int year;
    float cgpa;
    int active;
} Student;

/* Remove leading and trailing whitespace. */
static void trim_whitespace(char *text)
{
    char *start = text;
    size_t length;

    while (isspace((unsigned char)*start))
        start++;

    if (start != text)
        memmove(text, start, strlen(start) + 1);

    length = strlen(text);

    while (length > 0 &&
           isspace((unsigned char)text[length - 1]))
    {
        text[--length] = '\0';
    }
}

/* Safely read one line from the user. */
static int read_line(const char *prompt, char *buffer, size_t size)
{
    int ch;

    while (1)
    {
        printf("%s", prompt);

        if (fgets(buffer, (int)size, stdin) == NULL)
            return 0;

        /*
         * Detect and discard input exceeding the buffer.
         * An exactly full line followed by newline is accepted.
         */
        if (strchr(buffer, '\n') == NULL)
        {
            ch = getchar();

            if (ch != '\n' && ch != EOF)
            {
                while (ch != '\n' && ch != EOF)
                    ch = getchar();

                printf("Input is too long. Try again.\n");
                continue;
            }
        }

        buffer[strcspn(buffer, "\r\n")] = '\0';
        trim_whitespace(buffer);

        return 1;
    }
}

/* Read an integer within the specified range. */
static int read_integer_range(
    const char *prompt,
    long *value,
    long minimum,
    long maximum)
{
    char line[MAX_LINE];
    char *end;
    long parsed;

    while (1)
    {
        if (!read_line(prompt, line, sizeof(line)))
            return 0;

        errno = 0;
        parsed = strtol(line, &end, 10);

        if (end == line)
        {
            printf("Invalid input. Enter a whole number.\n");
            continue;
        }

        while (isspace((unsigned char)*end))
            end++;

        if (*end != '\0' || errno == ERANGE)
        {
            printf("Invalid integer. Try again.\n");
            continue;
        }

        if (parsed < minimum || parsed > maximum)
        {
            printf("Enter a value between %ld and %ld.\n",
                   minimum, maximum);
            continue;
        }

        *value = parsed;
        return 1;
    }
}

/* Read and validate CGPA. */
static int read_cgpa(float *cgpa)
{
    char line[MAX_LINE];
    char *end;
    float value;

    while (1)
    {
        if (!read_line("Enter CGPA (0.0 - 10.0): ",
                       line, sizeof(line)))
            return 0;

        errno = 0;
        value = strtof(line, &end);

        while (isspace((unsigned char)*end))
            end++;

        if (end == line || *end != '\0' ||
            errno == ERANGE || !isfinite(value))
        {
            printf("Invalid CGPA. Enter a valid number.\n");
            continue;
        }

        if (value < 0.0f || value > 10.0f)
        {
            printf("CGPA must be between 0.0 and 10.0.\n");
            continue;
        }

        *cgpa = value;
        return 1;
    }
}

/* Read non-empty text. */
static int read_text(
    const char *prompt,
    char *text,
    size_t size)
{
    while (1)
    {
        if (!read_line(prompt, text, size))
            return 0;

        if (text[0] == '\0')
        {
            printf("This field cannot be empty.\n");
            continue;
        }

        return 1;
    }
}

/* Find an active student using the student ID. */
static int find_student(
    int id,
    Student *found,
    long *position)
{
    FILE *file = fopen(DATA_FILE, "rb");
    Student student;

    if (file == NULL)
    {
        if (errno != ENOENT)
            perror("Unable to open student database");

        return 0;
    }

    while (fread(&student, sizeof(Student), 1, file) == 1)
    {
        if (student.active && student.id == id)
        {
            if (found != NULL)
                *found = student;

            if (position != NULL)
            {
                *position = ftell(file) - (long)sizeof(Student);

                if (*position < 0)
                {
                    perror("Unable to locate student record");
                    fclose(file);
                    return 0;
                }
            }

            fclose(file);
            return 1;
        }
    }

    if (ferror(file))
        perror("Error reading student database");

    fclose(file);
    return 0;
}

/* Save an updated student at its original file position. */
static int save_student_at(long position, const Student *student)
{
    FILE *file = fopen(DATA_FILE, "r+b");

    if (file == NULL)
    {
        perror("Unable to update student database");
        return 0;
    }

    if (fseek(file, position, SEEK_SET) != 0)
    {
        perror("Unable to locate student record");
        fclose(file);
        return 0;
    }

    if (fwrite(student, sizeof(Student), 1, file) != 1 ||
        fflush(file) != 0)
    {
        perror("Unable to save student record");
        fclose(file);
        return 0;
    }

    if (fclose(file) != 0)
    {
        perror("Unable to close student database");
        return 0;
    }

    return 1;
}

/* Print the details of one student. */
static void print_student(const Student *student)
{
    printf("\n+-----------------------------------------------+\n");
    printf("|              STUDENT DETAILS                 |\n");
    printf("+-----------------------------------------------+\n");
    printf(" Student ID : %d\n", student->id);
    printf(" Name       : %s\n", student->name);
    printf(" Course     : %s\n", student->course);
    printf(" Year       : %d\n", student->year);
    printf(" CGPA       : %.2f\n", student->cgpa);
    printf("+-----------------------------------------------+\n");
}

/* Add a new student record. */
static void add_student(void)
{
    Student student;
    long id;
    FILE *file;

    printf("\n========== ADD STUDENT ==========\n");

    if (!read_integer_range(
            "Enter student ID: ", &id, 1, INT_MAX))
        return;

    if (find_student((int)id, NULL, NULL))
    {
        printf("Error: This student ID already exists.\n");
        return;
    }

    student.id = (int)id;

    if (!read_text("Enter student name: ",
                   student.name, sizeof(student.name)))
        return;

    if (!read_text("Enter course/department: ",
                   student.course, sizeof(student.course)))
        return;

    {
        long year;

        if (!read_integer_range(
                "Enter academic year (1-6): ", &year, 1, 6))
            return;

        student.year = (int)year;
    }

    if (!read_cgpa(&student.cgpa))
        return;

    student.active = 1;

    file = fopen(DATA_FILE, "ab");

    if (file == NULL)
    {
        perror("Unable to open student database");
        return;
    }

    if (fwrite(&student, sizeof(Student), 1, file) != 1)
    {
        perror("Unable to save student record");
        fclose(file);
        return;
    }

    if (fclose(file) != 0)
    {
        perror("Unable to close student database");
        return;
    }

    printf("\nStudent added successfully!\n");
    print_student(&student);
}

/* Display all active students. */
static void display_students(void)
{
    FILE *file = fopen(DATA_FILE, "rb");
    Student student;
    int count = 0;

    printf("\n========== STUDENT RECORDS ==========\n");

    if (file == NULL)
    {
        if (errno == ENOENT)
            printf("No student records available yet.\n");
        else
            perror("Unable to open student database");

        return;
    }

    printf("+------+----------------------+--------------------+"
           "------+--------+\n");

    printf("| ID   | Name                 | Course             |"
           " Year | CGPA   |\n");

    printf("+------+----------------------+--------------------+"
           "------+--------+\n");

    while (fread(&student, sizeof(Student), 1, file) == 1)
    {
        if (student.active)
        {
            printf("| %-4d | %-20.20s | %-18.18s |"
                   " %-4d | %-6.2f |\n",
                   student.id,
                   student.name,
                   student.course,
                   student.year,
                   student.cgpa);

            count++;
        }
    }

    if (ferror(file))
        perror("Error reading student database");

    printf("+------+----------------------+--------------------+"
           "------+--------+\n");

    printf("Total active students: %d\n", count);

    fclose(file);
}

/* Search for a student by ID. */
static void search_student(void)
{
    Student student;
    long id;

    printf("\n========== SEARCH STUDENT ==========\n");

    if (!read_integer_range(
            "Enter student ID to search: ",
            &id, 1, INT_MAX))
        return;

    if (find_student((int)id, &student, NULL))
    {
        printf("\nStudent record found!\n");
        print_student(&student);
    }
    else
    {
        printf("Student ID %ld not found.\n", id);
    }
}

/* Update an existing student record. */
static void update_student(void)
{
    Student student;
    long id;
    long position;

    printf("\n========== UPDATE STUDENT ==========\n");

    if (!read_integer_range(
            "Enter student ID to update: ",
            &id, 1, INT_MAX))
        return;

    if (!find_student((int)id, &student, &position))
    {
        printf("Student ID %ld not found.\n", id);
        return;
    }

    printf("\nCurrent student details:\n");
    print_student(&student);

    printf("\nEnter updated information:\n");

    if (!read_text("New student name: ",
                   student.name, sizeof(student.name)))
        return;

    if (!read_text("New course/department: ",
                   student.course, sizeof(student.course)))
        return;

    {
        long year;

        if (!read_integer_range(
                "New academic year (1-6): ", &year, 1, 6))
            return;

        student.year = (int)year;
    }

    if (!read_cgpa(&student.cgpa))
        return;

    if (save_student_at(position, &student))
    {
        printf("\nStudent record updated successfully!\n");
        print_student(&student);
    }
}

/* Delete a student using logical deletion. */
static void delete_student(void)
{
    Student student;
    long id;
    long position;

    printf("\n========== DELETE STUDENT ==========\n");

    if (!read_integer_range(
            "Enter student ID to delete: ",
            &id, 1, INT_MAX))
        return;

    if (!find_student((int)id, &student, &position))
    {
        printf("Student ID %ld not found.\n", id);
        return;
    }

    printf("\nRecord selected for deletion:\n");
    print_student(&student);

    /* Mark the record inactive instead of erasing it. */
    student.active = 0;

    if (save_student_at(position, &student))
    {
        printf("\nStudent record deleted successfully!\n");
        printf("Inactive record is retained in the database.\n");
    }
}

/* Display academic statistics and the top performer. */
static void show_dashboard(void)
{
    FILE *file = fopen(DATA_FILE, "rb");
    Student student;
    Student topper;

    int count = 0;
    float total_cgpa = 0.0f;
    int has_topper = 0;

    printf("\n========== ACADEMIC DASHBOARD ==========\n");

    if (file == NULL)
    {
        if (errno == ENOENT)
            printf("No student records available yet.\n");
        else
            perror("Unable to open student database");

        return;
    }

    while (fread(&student, sizeof(Student), 1, file) == 1)
    {
        if (!student.active)
            continue;

        count++;
        total_cgpa += student.cgpa;

        if (!has_topper || student.cgpa > topper.cgpa)
        {
            topper = student;
            has_topper = 1;
        }
    }

    if (ferror(file))
    {
        perror("Error reading student database");
        fclose(file);
        return;
    }

    fclose(file);

    if (count == 0)
    {
        printf("No active student records available.\n");
        return;
    }

    printf("\nTotal active students : %d\n", count);
    printf("Average CGPA          : %.2f\n",
           total_cgpa / count);

    printf("\nTop Performer:\n");
    printf("Student ID : %d\n", topper.id);
    printf("Name       : %s\n", topper.name);
    printf("Course     : %s\n", topper.course);
    printf("CGPA       : %.2f\n", topper.cgpa);

    printf("\n========================================\n");
}

/* Display application menu. */
static void display_menu(void)
{
    printf("\n");
    printf("+------------------------------------------+\n");
    printf("|               CAMPUSLEDGER               |\n");
    printf("|        Student Record Management         |\n");
    printf("+------------------------------------------+\n");
    printf("|  1. Add Student                          |\n");
    printf("|  2. Display All Students                 |\n");
    printf("|  3. Search Student                       |\n");
    printf("|  4. Update Student                       |\n");
    printf("|  5. Delete Student                       |\n");
    printf("|  6. Academic Dashboard                   |\n");
    printf("|  7. Exit                                 |\n");
    printf("+------------------------------------------+\n");
}

/* Main function. */
int main(void)
{
    long choice;

    printf("\nWelcome to CampusLedger!\n");
    printf("Student Record Management System\n");

    while (1)
    {
        display_menu();

        if (!read_integer_range(
                "Select an option (1-7): ",
                &choice, 1, 7))
        {
            printf("\nInput ended. Exiting CampusLedger.\n");
            break;
        }

        switch (choice)
        {
            case 1:
                add_student();
                break;

            case 2:
                display_students();
                break;

            case 3:
                search_student();
                break;

            case 4:
                update_student();
                break;

            case 5:
                delete_student();
                break;

            case 6:
                show_dashboard();
                break;

            case 7:
                printf("\nThank you for using CampusLedger!\n");
                return 0;
        }
    }

    return 0;
}
