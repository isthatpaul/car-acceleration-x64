#include "acceleration.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define VALUES_PER_CAR 3
#define BENCHMARK_RUNS 30
#define BENCHMARK_SIZE_COUNT 4

static int allocate_inputs(int rows, float **matrix, int **c_output, int **asm_output)
{
    *matrix = (float *)malloc((size_t)rows * VALUES_PER_CAR * sizeof(float));
    *c_output = (int *)malloc((size_t)rows * sizeof(int));
    *asm_output = (int *)malloc((size_t)rows * sizeof(int));

    if (*matrix == NULL || *c_output == NULL || *asm_output == NULL) {
        free(*matrix);
        free(*c_output);
        free(*asm_output);
        *matrix = NULL;
        *c_output = NULL;
        *asm_output = NULL;
        return 0;
    }

    return 1;
}

static int compare_outputs(const int *c_output, const int *asm_output, int rows)
{
    for (int row = 0; row < rows; ++row) {
        if (c_output[row] != asm_output[row]) {
            return 0;
        }
    }

    return 1;
}

static void print_results(const int *c_output, const int *asm_output, int rows)
{
    printf("\nAcceleration results (m/s^2):\n");
    printf("%-8s %-8s %-8s\n", "Car", "C", "x86-64");

    for (int row = 0; row < rows; ++row) {
        printf("%-8d %-8d %-8d\n", row + 1, c_output[row], asm_output[row]);
    }

    printf("\nCorrectness check: %s\n",
           compare_outputs(c_output, asm_output, rows) ? "PASS" : "FAIL");
}

static int read_row(float *values, int row)
{
    char line[256];

    printf("Car %d (Vi,Vf,time): ", row + 1);
    if (fgets(line, sizeof(line), stdin) == NULL) {
        return 0;
    }

    if (sscanf_s(line, "%f,%f,%f", &values[0], &values[1], &values[2]) == 3 ||
        sscanf_s(line, "%f %f %f", &values[0], &values[1], &values[2]) == 3) {
        return values[2] > 0.0f;
    }

    return 0;
}

static int run_interactive(void)
{
    int rows = 0;
    float *matrix = NULL;
    int *c_output = NULL;
    int *asm_output = NULL;

    printf("Enter the number of cars: ");
    if (scanf_s("%d", &rows) != 1 || rows <= 0) {
        fprintf(stderr, "The number of cars must be a positive integer.\n");
        return EXIT_FAILURE;
    }
    getchar();

    if (!allocate_inputs(rows, &matrix, &c_output, &asm_output)) {
        fprintf(stderr, "Unable to allocate memory for %d cars.\n", rows);
        return EXIT_FAILURE;
    }

    printf("Enter Vi (km/h), Vf (km/h), and time (seconds) for each car.\n");
    for (int row = 0; row < rows; ++row) {
        if (!read_row(matrix + (row * VALUES_PER_CAR), row)) {
            fprintf(stderr, "Invalid input for car %d; time must be greater than zero.\n", row + 1);
            free(matrix);
            free(c_output);
            free(asm_output);
            return EXIT_FAILURE;
        }
    }

    acceleration_c(matrix, rows, c_output);
    acceleration_asm(matrix, rows, asm_output);
    print_results(c_output, asm_output, rows);

    free(matrix);
    free(c_output);
    free(asm_output);
    return EXIT_SUCCESS;
}

static unsigned int next_random(unsigned int *state)
{
    *state = (*state * 1664525u) + 1013904223u;
    return *state;
}

static void fill_random_matrix(float *matrix, int rows, unsigned int *state)
{
    for (int row = 0; row < rows; ++row) {
        const float initial_velocity = (float)(next_random(state) % 121u);
        const float velocity_increase = 20.0f + (float)(next_random(state) % 181u);
        const float time_seconds = 1.0f + (float)(next_random(state) % 1000u) / 100.0f;
        float *values = matrix + (row * VALUES_PER_CAR);

        values[0] = initial_velocity;
        values[1] = initial_velocity + velocity_increase;
        values[2] = time_seconds;
    }
}

static int run_benchmark(void)
{
    static const int benchmark_sizes[BENCHMARK_SIZE_COUNT] = {10, 100, 1000, 10000};
    LARGE_INTEGER frequency;
    unsigned int random_state = 0xC0FFEEu;

    if (!QueryPerformanceFrequency(&frequency)) {
        fprintf(stderr, "High-resolution timer is unavailable.\n");
        return EXIT_FAILURE;
    }

    printf("Benchmarking the x86-64 function (%d runs per size)\n", BENCHMARK_RUNS);
    printf("%-12s %-20s %-12s\n", "Cars", "Average time (us)", "Correctness");

    for (int size_index = 0; size_index < BENCHMARK_SIZE_COUNT; ++size_index) {
        const int rows = benchmark_sizes[size_index];
        float *matrix = NULL;
        int *c_output = NULL;
        int *asm_output = NULL;
        LARGE_INTEGER start;
        LARGE_INTEGER end;
        double elapsed_ticks = 0.0;

        if (!allocate_inputs(rows, &matrix, &c_output, &asm_output)) {
            fprintf(stderr, "Unable to allocate benchmark data for %d cars.\n", rows);
            return EXIT_FAILURE;
        }

        fill_random_matrix(matrix, rows, &random_state);
        acceleration_c(matrix, rows, c_output);
        acceleration_asm(matrix, rows, asm_output);
        const int correct = compare_outputs(c_output, asm_output, rows);

        for (int run = 0; run < BENCHMARK_RUNS; ++run) {
            QueryPerformanceCounter(&start);
            acceleration_asm(matrix, rows, asm_output);
            QueryPerformanceCounter(&end);
            elapsed_ticks += (double)(end.QuadPart - start.QuadPart);
        }

        const double average_microseconds =
            (elapsed_ticks / BENCHMARK_RUNS) * 1000000.0 / (double)frequency.QuadPart;

        printf("%-12d %-20.3f %-12s\n",
               rows,
               average_microseconds,
               correct ? "PASS" : "FAIL");

        free(matrix);
        free(c_output);
        free(asm_output);
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--benchmark") == 0) {
        return run_benchmark();
    }

    if (argc != 1) {
        fprintf(stderr, "Usage: %s [--benchmark]\n", argv[0]);
        return EXIT_FAILURE;
    }

    return run_interactive();
}
