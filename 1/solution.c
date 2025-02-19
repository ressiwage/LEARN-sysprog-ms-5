#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libcoro.h"
#include <assert.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAX_ELEMENTS_IN_FILE 100001
#define PRINT_SORTED_ARRAYS 0

typedef struct my_context{
	char *name;
	int num_coroutines;
	/** ADD HERE YOUR OWN MEMBERS, SUCH AS FILE NAME, WORK TIME, ... */
	int *ints;
	int *result;
	int ints_size;
} my_context;

typedef struct coro coro;

static my_context* my_context_new(const char *name, int *ints, int ints_size, int *result)
{
	my_context *ctx = malloc(sizeof(*ctx));
	ctx->name = strdup(name);
	ctx->num_coroutines = 0;
	ctx->ints = ints;
	ctx->ints_size = ints_size;
	ctx->result = result;
	return ctx;
}

static void my_context_delete(my_context *ctx)
{
	free(ctx->name);
	free(ctx);
}


static int* coro_merge_sort(int* ints, int size);

static int* coro_merge_sort_wrapper(void *context){
	coro *this = coro_this();
	my_context *ctx = context;
	char *name = ctx->name;

	int *returned = coro_merge_sort(ctx->ints, ctx->ints_size);
	memcpy(ctx->result, returned, sizeof(int)*ctx->ints_size );
	// for(int i=0;i<ctx->ints_size;i++){
	// 	printf("%d ", returned[i]);
	// }
	// for(int i = 0; i <ctx->ints_size;i++){printf("%d ", ctx->result[i]);}
	
	
	my_context_delete(ctx);
	free(returned);
	return 0;
}

static int* coro_merge_sort(int* ints, int size){

	coro *this = coro_this();

	int mid = size/2;
	int sizeA = mid-0;
	int sizeB = size-mid;
	assert(size>0);
	
	if (size==1){return &ints[0];}
	
	int	*a = coro_merge_sort(&ints[0],	mid);
	
	int	*b = coro_merge_sort(&ints[mid],	size-mid);
	
	int counterA = 0, counterB=0, counterRes=0;
	int *result = malloc(size * sizeof(int));

	while (counterA < sizeA || counterB < sizeB){
		if (counterA == sizeA){
			result[counterRes++]=b[counterB++];
		}else if (counterB == sizeB){
			result[counterRes++]=a[counterA++];
		} else {
			if (a[counterA]<b[counterB]){
				result[counterRes++]=a[counterA++];
			} else{
			    result[counterRes++]=b[counterB++];
			}
		}
	}

	coro_yield();
	
	if(mid>1)free(a);
	if(size-mid>1)free(b);
	return result;
}

static int* simple_merge_sort(int* ints, int size){
	int mid = size/2;
	int sizeA = mid-0;
	int sizeB = size-mid;
	assert(size>0);
	
	if (size==1){return &ints[0];}
	
	int	*a = simple_merge_sort(&ints[0],	mid);
	
	int	*b = simple_merge_sort(&ints[mid],	size-mid);
	
	int counterA = 0, counterB=0, counterRes=0;
	int *result = malloc(size * sizeof(int));

	while (counterA < sizeA || counterB < sizeB){
		if (counterA == sizeA){
			result[counterRes++]=b[counterB++];
		}else if (counterB == sizeB){
			result[counterRes++]=a[counterA++];
		} else {
			if (a[counterA]<b[counterB]){
				result[counterRes++]=a[counterA++];
			} else{
			    result[counterRes++]=b[counterB++];
			}
		}
	}

	
	if(mid>1)free(a);
	if(size-mid>1)free(b);
	return result;
}

int main(int argc, char **argv)
{	
	int argind=0, optind=1, some_temp_var=0;
	while ((argind = getopt (argc, argv, "abc:")) != -1){optind++;}
	int num_files = argc-optind;

	int **p_arrays = malloc(sizeof(int*) * num_files + sizeof(int)* MAX_ELEMENTS_IN_FILE * num_files); //указатели на массивы с числами
	int **output = malloc(sizeof(int*) * num_files + sizeof(int) * MAX_ELEMENTS_IN_FILE * num_files); 
	// memset(p_arrays, 0, sizeof(int*) * (argc-optind))
	unsigned int sizes[num_files];
	char names[num_files][1024];
	for (int optindex = optind, index=0; optindex < argc; optindex++, index++){
		printf ("got argument %s\n", argv[optindex]);
		//open it
		FILE *file = fopen(argv[optindex], "r");
		sprintf(names[index], "%s", argv[optindex]);
		int elem_ind=0;
		int elements[MAX_ELEMENTS_IN_FILE];
		//scan elements
		while(fscanf(file, "%d", &elements[elem_ind++])==1){}
		//set nth size
		sizes[index] = elem_ind;
		//set it to 2d array
		p_arrays[index] = (int *)(p_arrays+num_files) + ((index) * MAX_ELEMENTS_IN_FILE); 
		output[index] = (int *)(output+num_files) + ((index) * MAX_ELEMENTS_IN_FILE); 
		// почему так?
		// потому что сначала мы берем указательное смещение, т.е. первую строку arr[0]
		// затем мы берем i-ю строку
		
		memcpy(p_arrays[index], elements, sizeof(int) * sizes[index]);
		// memcpy instead of p_arrays[index-optind] = &elements;
		}
	printf("test, %d ", sizes[5]);
    printf("test %d %d\n", p_arrays[0][0], p_arrays[0][1]);

	clock_t begin = clock();
	for(int i=0; i<sizeof(sizes)/sizeof(sizes[0]); i++){
		int* sortedArr = simple_merge_sort(p_arrays[5], sizes[5]);
		if(PRINT_SORTED_ARRAYS){for(int i = 0; i <sizes[0];i++){printf("%d ", sortedArr[i]);}}
		free(sortedArr);
	}
	clock_t end = clock();
	printf("ts sync %f\n", (double)(end - begin) / CLOCKS_PER_SEC);
	
	/* Initialize our coroutine global cooperative scheduler. */
	coro_sched_init();
	/* Start several coroutines. */
	printf("1\n");
	// char name[16];
	// sprintf(name, "coro_%d", 0);
	printf("2\n");
	char name[16];
	sprintf(name, "coro_%d", 0);
	begin = clock();

	for(int i=0; i<num_files; i++){
		printf("3\n");
		
		coro_new(coro_merge_sort_wrapper, my_context_new(name, p_arrays[i], sizes[i], output[i]));
	}
	printf("5\n");
	/* Wait for all the coroutines to end. */
	struct coro *c;
	while ((c = coro_sched_wait()) != NULL) {
		/*
		 * Each 'wait' returns a finished coroutine with which you can
		 * do anything you want. Like check its exit status, for
		 * example. Don't forget to free the coroutine afterwards.
		 */
		printf("Finished %d\n", coro_status(c));
		coro_delete(c);
	}
	end = clock();
	printf("1\n");
	printf("ts async %f\n", (double)(end - begin) / CLOCKS_PER_SEC);
	printf("4\n");
	for (int index=0; index < num_files; index++){
		printf ("%s\n", names[index]);
		FILE *out = fopen(names[index],  "w+");
		for(int i = 0; i < sizes[index]; i++){
			fprintf(out, "%d ", output[index][i]);
		}
		//scan elements
		// while(fscanf(file, "%d", &elements[elem_ind++])==1){}

	}
	/* All coroutines have finished. */
	free(p_arrays);
	free(output);
	/* IMPLEMENT MERGING OF THE SORTED ARRAYS HERE. */
	return 0;
}