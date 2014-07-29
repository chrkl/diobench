#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

typedef struct bench_result 
{
    long    bytes;
    double  throughput;
    double 	runtime;
    int 	status;
} bench_result_t;

bench_result_t write_sequential_benchmark(const char *path, long bs, long count, int cleanup, int sync_io);
bench_result_t read_sequential_benchmark(const char *path, long bs, long count, int cleanup, int sync_io);
bench_result_t write_random_benchmark(const char *path, long bs, long count,  long max_offset, int cleanup, int sync_io);
bench_result_t read_random_benchmark(const char *path, long bs, long count,  long max_offset, int cleanup, int sync_io);