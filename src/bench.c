#include "bench.h"

long random_in_range (long min, long max)
{
    return (random() % (max+1-min))+min;
}

bench_result_t write_sequential_benchmark(const char *path, long bs, long count, int cleanup, int sync_io)
{
    bench_result_t result;
    int i, f;
    struct timeval stop, start;
    char *out_buf = malloc(bs);

    result.bytes = 0;
    result.throughput = 0;
    result.status = 0;
    result.runtime = 0;

    f = open(path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | (sync_io ? O_SYNC : 0));

    gettimeofday(&start, NULL); 
    
    for(i = 0; i < count; i++)
    {
        int ret = pwrite(f, out_buf, bs, bs * i);   
        
        if(ret < 0)
        {
            printf("Error while writing file\n");
            perror("Error");
            break;
        }
        else
        {
            result.bytes += ret;
        }
    }
    fsync(f);

    gettimeofday(&stop, NULL);

    close(f);

    if(cleanup)
    {
        result.status = remove(path);
    }
    result.runtime = (double) (stop.tv_sec - start.tv_sec);
    result.throughput = ((double) result.bytes / (1024.0 * 1024.0)) / (double) (stop.tv_sec - start.tv_sec);
    return result;
}

bench_result_t read_sequential_benchmark(const char *path, long bs, long count, int cleanup, int sync_io)
{
    bench_result_t result;
    int i, f;
    struct timeval stop, start;
    char *in_buf = malloc(bs);

    result.bytes = 0;
    result.throughput = 0;
    result.status = 0;
    result.runtime = 0;

    f = open(path, O_RDONLY | (sync_io ? O_SYNC : 0));

    gettimeofday(&start, NULL); 
    
    for(i = 0; i < count; i++)
    {
        int ret = pread(f, in_buf, bs, bs * i);   
        
        if(ret < 0)
        {
            printf("Error while writing file\n");
            perror("Error");
            break;
        }
        else
        {
            result.bytes += ret;
        }
    }
    fsync(f);

    gettimeofday(&stop, NULL);

    close(f);

    if(cleanup)
    {
        result.status = remove(path);
    }
    result.runtime = (double) (stop.tv_sec - start.tv_sec);
    result.throughput = ((double) result.bytes / (1024.0 * 1024.0)) / (double) (stop.tv_sec - start.tv_sec);
    return result;
}

bench_result_t write_random_benchmark(const char *path, long bs, long count, long max_offset, int cleanup, int sync_io)
{
    bench_result_t result;
    int f;
    long i;
    struct timeval stop, start;
    char *out_buf = malloc(bs);
    
    result.bytes = 0;
    result.throughput = 0;
    result.status = 0;
    result.runtime = 0;

    f = open(path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | (sync_io ? O_SYNC : 0));

    gettimeofday(&start, NULL); 

    for(i = 0; i < count; i++)
    {
        int next_offset, ret;
        next_offset = random_in_range(0, max_offset - bs);
        ret = pwrite(f, out_buf, bs, next_offset);

        if(ret < 0)
        {
            printf("Error while writing file\n");
            perror("Error");
            break;
        }
        else
        {
            result.bytes += ret;
        }
    }

    fsync(f);

    gettimeofday(&stop, NULL);

    close(f);

    if(cleanup)
    {
        result.status = remove(path);
    }

    result.runtime = (double) (stop.tv_sec - start.tv_sec);
    result.throughput = ((double) result.bytes / (1024.0 * 1024.0)) / (double) (stop.tv_sec - start.tv_sec);    
    return result;
}

bench_result_t read_random_benchmark(const char *path, long bs, long count, long max_offset,  int cleanup, int sync_io)
{
    bench_result_t result;
    return result;
}