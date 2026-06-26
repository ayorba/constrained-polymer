#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static volatile double sink = 0.0;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        return 1;
    }

    char *endptr;
    size_t num_ops = (size_t)strtoull(argv[1], &endptr, 10);
    if (*endptr != '\0') {
        return 1;
    }

    srand((unsigned)time(NULL));
    // double a = rand() / (RAND_MAX + 1.0);
    // double b = rand() / (RAND_MAX + 1.0);

    double a = .01;
    double b = .01;

    for (size_t i = 0; i < num_ops; ++i) {
        sink = a * b;
    }

    return 0;
}
