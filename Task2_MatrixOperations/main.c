
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAX 10
#define MAX_VALUE 10000
#define INPUT_SIZE 128

/* Read and validate integer input. */
static int read_integer(const char *prompt, long long *value)
{
    char line[INPUT_SIZE];
    char *end;
    long long parsed;

    while (1)
    {
        int ch;

        printf("%s", prompt);

        if (fgets(line, sizeof(line), stdin) == NULL)
            return 0;

        /* Handle excessively long input. */
        if (strchr(line, '\n') == NULL && !feof(stdin))
        {
            while ((ch = getchar()) != '\n' && ch != EOF)
            {
                /* Discard remaining characters. */
            }

            printf("Input is too long. Try again.\n");
            continue;
        }

        errno = 0;
        parsed = strtoll(line, &end, 10);

        if (end == line)
        {
            printf("Invalid input. Enter an integer.\n");
            continue;
        }

        while (isspace((unsigned char)*end))
            end++;

        if (*end != '\0' || errno == ERANGE)
        {
            printf("Invalid integer. Try again.\n");
            continue;
        }

        *value = parsed;
        return 1;
    }
}

/* Validate matrix dimensions. */
static int read_dimension(const char *prompt, int *dimension)
{
    long long value;

    while (1)
    {
        if (!read_integer(prompt, &value))
            return 0;

        if (value < 1 || value > MAX)
        {
            printf("Dimension must be between 1 and %d.\n", MAX);
            continue;
        }

        *dimension = (int)value;
        return 1;
    }
}

/* Accept matrix elements with input validation. */
static int input_matrix(
    long long matrix[MAX][MAX],
    int rows,
    int cols,
    const char *name)
{
    char prompt[64];
    long long value;

    printf("\nEnter elements of %s:\n", name);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            snprintf(prompt, sizeof(prompt),
                     "%s[%d][%d]: ", name, i + 1, j + 1);

            while (1)
            {
                if (!read_integer(prompt, &value))
                    return 0;

                if (value < -MAX_VALUE || value > MAX_VALUE)
                {
                    printf("Enter a value between -%d and %d.\n",
                           MAX_VALUE, MAX_VALUE);
                    continue;
                }

                matrix[i][j] = value;
                break;
            }
        }
    }

    return 1;
}

/* Display a matrix in formatted rows and columns.
   The matrix is read-only by convention. */
static void display_matrix(
    const char *title,
    long long matrix[MAX][MAX],
    int rows,
    int cols)
{
    printf("\n%s (%d x %d):\n", title, rows, cols);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%12lld", matrix[i][j]);
        }

        printf("\n");
    }
}

/* Perform matrix addition. */
static void add_matrices(
    long long a[MAX][MAX],
    long long b[MAX][MAX],
    long long result[MAX][MAX],
    int rows,
    int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result[i][j] = a[i][j] + b[i][j];
        }
    }
}

/* Perform matrix multiplication. */
static void multiply_matrices(
    long long a[MAX][MAX],
    long long b[MAX][MAX],
    long long result[MAX][MAX],
    int rows_a,
    int cols_a,
    int cols_b)
{
    for (int i = 0; i < rows_a; i++)
    {
        for (int j = 0; j < cols_b; j++)
        {
            result[i][j] = 0;

            for (int k = 0; k < cols_a; k++)
            {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

/* Calculate matrix transpose. */
static void transpose_matrix(
    long long original[MAX][MAX],
    long long transpose[MAX][MAX],
    int rows,
    int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            transpose[j][i] = original[i][j];
        }
    }
}

/* Display the application menu. */
static void display_menu(void)
{
    printf("\n");
    printf("+------------------------------------------+\n");
    printf("|                 MATRIXLAB                |\n");
    printf("|          Matrix Operations Toolkit      |\n");
    printf("+------------------------------------------+\n");
    printf("|  1. Matrix Addition                      |\n");
    printf("|  2. Matrix Multiplication                |\n");
    printf("|  3. Matrix Transpose                     |\n");
    printf("|  4. Exit                                 |\n");
    printf("+------------------------------------------+\n");
}

/* Main function. */
int main(void)
{
    int choice;
    int rows_a, cols_a;
    int rows_b, cols_b;

    long long a[MAX][MAX];
    long long b[MAX][MAX];
    long long result[MAX][MAX];

    printf("Welcome to MatrixLab!\n");

    while (1)
    {
        long long selected;

        display_menu();

        if (!read_integer("Select an option (1-4): ", &selected))
        {
            printf("\nInput ended. Exiting MatrixLab.\n");
            break;
        }

        if (selected < 1 || selected > 4)
        {
            printf("Invalid selection. Choose 1 to 4.\n");
            continue;
        }

        choice = (int)selected;

        switch (choice)
        {
            case 1:

                printf("\n--- MATRIX ADDITION ---\n");

                if (!read_dimension("Rows of Matrix A: ", &rows_a) ||
                    !read_dimension("Columns of Matrix A: ", &cols_a) ||
                    !read_dimension("Rows of Matrix B: ", &rows_b) ||
                    !read_dimension("Columns of Matrix B: ", &cols_b))
                {
                    printf("\nInput ended.\n");
                    return 0;
                }

                if (rows_a != rows_b || cols_a != cols_b)
                {
                    printf("Addition requires both matrices "
                           "to have the same dimensions.\n");
                    break;
                }

                if (!input_matrix(a, rows_a, cols_a, "Matrix A") ||
                    !input_matrix(b, rows_b, cols_b, "Matrix B"))
                {
                    printf("\nInput ended.\n");
                    return 0;
                }

                add_matrices(a, b, result, rows_a, cols_a);

                display_matrix("Matrix A", a, rows_a, cols_a);
                display_matrix("Matrix B", b, rows_b, cols_b);

                display_matrix("Addition Result", result,
                               rows_a, cols_a);

                break;

            case 2:

                printf("\n--- MATRIX MULTIPLICATION ---\n");

                if (!read_dimension("Rows of Matrix A: ", &rows_a) ||
                    !read_dimension("Columns of Matrix A: ", &cols_a) ||
                    !read_dimension("Rows of Matrix B: ", &rows_b) ||
                    !read_dimension("Columns of Matrix B: ", &cols_b))
                {
                    printf("\nInput ended.\n");
                    return 0;
                }

                if (cols_a != rows_b)
                {
                    printf("Multiplication is not possible.\n");
                    printf("Columns of A must equal rows of B.\n");
                    break;
                }

                if (!input_matrix(a, rows_a, cols_a, "Matrix A") ||
                    !input_matrix(b, rows_b, cols_b, "Matrix B"))
                {
                    printf("\nInput ended.\n");
                    return 0;
                }

                multiply_matrices(a, b, result,
                                  rows_a, cols_a, cols_b);

                display_matrix("Matrix A", a, rows_a, cols_a);
                display_matrix("Matrix B", b, rows_b, cols_b);

                display_matrix("Multiplication Result", result,
                               rows_a, cols_b);

                break;

            case 3:

                printf("\n--- MATRIX TRANSPOSE ---\n");

                if (!read_dimension("Rows of Matrix: ", &rows_a) ||
                    !read_dimension("Columns of Matrix: ", &cols_a))
                {
                    printf("\nInput ended.\n");
                    return 0;
                }

                if (!input_matrix(a, rows_a, cols_a, "Matrix"))
                {
                    printf("\nInput ended.\n");
                    return 0;
                }

                transpose_matrix(a, result, rows_a, cols_a);

                display_matrix("Original Matrix", a,
                               rows_a, cols_a);

                display_matrix("Transpose Matrix", result,
                               cols_a, rows_a);

                break;

            case 4:

                printf("\nThank you for using MatrixLab!\n");
                return 0;

            default:
                break;
        }
    }

    return 0;
}
