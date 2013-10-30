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
    long    written_bytes;
    double  throughput;
    double 	runtime;
    int 	status;
} bench_result_t;

bench_result_t benchmark(const char *path, long bs, long count, int cleanup);
