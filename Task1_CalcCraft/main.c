
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#define HISTORY_FILE "calccraft_history.txt"
#define INPUT_SIZE 128

/* Read one complete line.
   Returns 1 on success, 0 on EOF, -1 if too long. */
static int read_line(const char *prompt, char *buffer, size_t size)
{
    size_t length;
    int ch;

    printf("%s", prompt);

    if (fgets(buffer, (int)size, stdin) == NULL)
        return 0;

    length = strlen(buffer);

    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
        return 1;
    }

    if (!feof(stdin)) {
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* Discard excess characters. */
        }
        return -1;
    }

    return 1;
}

/* Validate menu input. */
static int read_integer(const char *prompt, int *value)
{
    char line[INPUT_SIZE];
    char *end;
    long parsed;
    int status;

    while ((status = read_line(prompt, line, sizeof(line))) != 0) {

        if (status == -1) {
            puts("Input is too long. Try again.");
            continue;
        }

        errno = 0;
        parsed = strtol(line, &end, 10);

        while (isspace((unsigned char)*end))
            end++;

        if (end == line || *end != '\0' || errno == ERANGE ||
            parsed < INT_MIN || parsed > INT_MAX) {
            puts("Invalid choice. Enter a whole number.");
            continue;
        }

        *value = (int)parsed;
        return 1;
    }

    return 0;
}

/* Validate decimal and integer inputs. */
static int read_double(const char *prompt, double *value)
{
    char line[INPUT_SIZE];
    char *end;
    double parsed;
    int status;

    while ((status = read_line(prompt, line, sizeof(line))) != 0) {

        if (status == -1) {
            puts("Input is too long. Try again.");
            continue;
        }

        errno = 0;
        parsed = strtod(line, &end);

        while (isspace((unsigned char)*end))
            end++;

        if (end == line || *end != '\0' ||
            errno == ERANGE || !isfinite(parsed)) {
            puts("Invalid number. Enter a valid number.");
            continue;
        }

        *value = parsed;
        return 1;
    }

    return 0;
}

/* Display the application menu. */
static void display_menu(void)
{
    puts("\n+------------------------------------------+");
    puts("|                CALCCRAFT                 |");
    puts("|       Interactive C Calculator           |");
    puts("+------------------------------------------+");
    puts("|  1. Addition                             |");
    puts("|  2. Subtraction                          |");
    puts("|  3. Multiplication                       |");
    puts("|  4. Division                             |");
    puts("|  5. View calculation history             |");
    puts("|  6. Exit                                 |");
    puts("+------------------------------------------+");
}

/* Save calculations permanently in a text file. */
static void append_history(
    double first, char operation, double second, double result)
{
    FILE *file = fopen(HISTORY_FILE, "a");

    if (file == NULL) {
        perror("Warning: Could not open history file");
        return;
    }

    if (fprintf(file, "%.10g %c %.10g = %.10g\n",
                first, operation, second, result) < 0) {
        perror("Warning: Could not save calculation");
    }

    if (fclose(file) != 0) {
        perror("Warning: Could not close history file");
    }
}

/* Display previously saved calculations. */
static void view_history(void)
{
    FILE *file = fopen(HISTORY_FILE, "r");
    char line[INPUT_SIZE];
    int found_entry = 0;

    if (file == NULL) {
        if (errno == ENOENT)
            puts("No saved calculations yet.");
        else
            perror("Could not open history file");

        return;
    }

    puts("\n--- Saved Calculation History ---");

    while (fgets(line, sizeof(line), file) != NULL) {
        fputs(line, stdout);
        found_entry = 1;
    }

    if (!found_entry)
        puts("History is empty.");

    puts("---------------------------------");
    fclose(file);
}

int main(void)
{
    int choice;
    double first, second, result;
    char operation;

    puts("Welcome to CalcCraft!");

    while (1) {

        display_menu();

        if (!read_integer("Select an option (1-6): ", &choice)) {
            puts("\nInput ended. Exiting CalcCraft.");
            break;
        }

        if (choice == 6) {
            puts("Thank you for using CalcCraft. Goodbye!");
            break;
        }

        if (choice == 5) {
            view_history();
            continue;
        }

        if (choice < 1 || choice > 4) {
            puts("Please select an option from 1 to 6.");
            continue;
        }

        if (!read_double("Enter the first number: ", &first) ||
            !read_double("Enter the second number: ", &second)) {
            puts("\nInput ended. Exiting CalcCraft.");
            break;
        }

        /* Select the arithmetic operation using switch case. */
        switch (choice) {

            case 1:
                operation = '+';
                result = first + second;
                break;

            case 2:
                operation = '-';
                result = first - second;
                break;

            case 3:
                operation = '*';
                result = first * second;
                break;

            case 4:
                if (second == 0.0) {
                    puts("Error: Division by zero is not allowed.");
                    continue;
                }

                operation = '/';
                result = first / second;
                break;

            default:
                puts("Unexpected menu selection.");
                continue;
        }

        /* Reject arithmetic overflow. */
        if (!isfinite(result)) {
            puts("Error: Result is outside the supported range.");
            continue;
        }

        printf("\nResult: %.10g %c %.10g = %.10g\n",
               first, operation, second, result);

        append_history(first, operation, second, result);
    }

    return 0;
}
