#define _GNU_SOURCE

#include <argtable2.h>
#include "mpi.h"

#include "bench.h"

#define PATH_LEN 4096

long bs = 131072L;
long count = 8192L;

struct arg_str *block_count;
struct arg_lit *own_dir;
struct arg_str *block_size;
struct arg_lit *help;
struct arg_lit *no_cleanup;
struct arg_str *work_dir;
struct arg_int *repititions;
struct arg_end *end;

int main(int argc, char **argv)
{
    char *path  = malloc(PATH_LEN * sizeof(char));
    char *dir = malloc(PATH_LEN * sizeof(char));
    int myrank, ntasks, nerrors, cleanup, rep, i;
    double total_tp;

    void *argtable[] = {
        block_count = arg_str0("c", "count", "<block-count>", "Number of blocks"),
        own_dir = arg_lit0("d", "own-dir", "Own directory per process"),
        block_size = arg_str0("s", "block-size", "<block-size>", "Block size"),
        help = arg_lit0("h", "help", "Show help text"),
        no_cleanup = arg_lit0(NULL, "no-cleanup", "Do not clean up benchmark files"),
        work_dir = arg_str0("w", "work-dir", "<dir>", "Work directory"),
        repititions = arg_int0("r", "repititions", "<repititions>", "Number of repititions"),
        end = arg_end(5)
    };

    /* verify the argtable[] entries were allocated sucessfully */
    if (arg_nullcheck(argtable) != 0)
    {
    	/* NULL entries were detected, some allocations must have failed */
    	printf("%s: insufficient memory\n", argv[0]);
        return 1;
    }

    /* Parse the command line as defined by argtable[] */
    nerrors = arg_parse(argc, argv, argtable);

    /* special case: '--help' takes precedence over error reporting */
    if (help->count > 0)
    {
        printf("Usage: %s", argv[0]);
        arg_print_syntax(stdout, argtable,"\n");
        printf("Run distributed file system benchmark\n");
        arg_print_glossary(stdout, argtable,"  %-30s %s\n");
        return 1;
    }

    /* If the parser returned any errors then display them and exit */
    if (nerrors > 0)
    {
        /* Display the error details contained in the arg_end struct.*/
        arg_print_errors(stdout, end, argv[0]);
        printf("Try '%s --help' for more information.\n",argv[0]);
        return 1;
    }

    if(block_count->count > 0)
    {
    	count = atol(block_count->sval[0]);
    }

    if(block_size->count > 0)
    {
    	bs = atol(block_size->sval[0]);
    }

    if(no_cleanup->count > 0)
    {
    	cleanup = 0;
    }
    else
    {
    	cleanup = 1;
    }

    if(work_dir->count > 0)
    {
    	strncpy(dir, work_dir->sval[0], PATH_LEN);
    }
    else
    {
    	getcwd(dir, PATH_LEN);
    }

    if(repititions->count > 0)
    {
        rep = repititions->ival[0];
    }
    else
    {
        rep = 1;
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

    if(myrank == 0)
    {
    	printf("Starting diobench\n");
    	printf("Running %d process(es)\n", ntasks);
    }

    if(own_dir->count > 0)
    {
        char *own_dir_str;
        asprintf(&own_dir_str, "/%d", myrank);
        strncat(dir, own_dir_str, PATH_LEN);
        mkdir(dir, 0700);
    }

    snprintf(path, PATH_LEN, "%s/diobench_%d.out", dir, myrank);

    for(i = 0; i < rep; i++)
    {
        bench_result_t result;

        if(rep > 1 && myrank == 0)
        {
            printf("Starting run number %d\n", i+1);
        }

        result = benchmark(path, bs, count, cleanup);

        printf("Rank %d: written %lu MB in %f seconds (%f MB/s)\n",
               myrank, result.written_bytes / (1024 * 1024),  
               result.runtime, result.throughput);
        
        if(result.status != 0 )
        {
            printf("Unable to delete the file\n");
            perror("Error");
        }
        
        if(own_dir->count > 0 && cleanup)
        {
        	rmdir(dir);
        }

        arg_free(argtable);

        if(ntasks > 1)
        	MPI_Reduce(&result.throughput, &total_tp, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    	else
    		total_tp = result.throughput;

        if(myrank == 0)
            printf("Total throughput: %f MB/s\n", total_tp);
    }

    free(dir);
    free(path);
    MPI_Finalize();
    return 0;
}
