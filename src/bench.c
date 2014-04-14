#include "bench.h"

bench_result_t write_benchmark(const char *path, long bs, long count, int cleanup)
{
    bench_result_t result;
    int i, f;
    struct timeval stop, start;
    char *out_buf = malloc(bs);

    result.bytes = 0;
    result.throughput = 0;
    result.status = 0;
    result.runtime = 0;

    f = open(path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

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

bench_result_t read_benchmark(const char *path, long bs, long count, int cleanup)
{
    bench_result_t result;
    int i, f;
    struct timeval stop, start;
    char *in_buf = malloc(bs);

    result.bytes = 0;
    result.throughput = 0;
    result.status = 0;
    result.runtime = 0;

    f = open(path, O_RDONLY);

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