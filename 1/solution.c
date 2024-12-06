#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libcoro.h"
#include <assert.h>
#include <time.h>
#define MAX_ELEMENTS_IN_FILE 100001
#define PRINT_SORTED_ARRAYS 0

/**
 * You can compile and run this code using the commands:
 *
 * $> gcc solution.c libcoro.c
 * $> ./a.out
 */

typedef struct my_context{
	char *name;
	int num_coroutines;
	/** ADD HERE YOUR OWN MEMBERS, SUCH AS FILE NAME, WORK TIME, ... */
	int *ints;
	int ints_size;
} my_context;

typedef struct coro coro;

static my_context* my_context_new(const char *name, int *ints, int ints_size)
{
	my_context *ctx = malloc(sizeof(*ctx));
	ctx->name = strdup(name);
	ctx->num_coroutines = 0;
	ctx->ints = ints;
	ctx->ints_size = ints_size;
	return ctx;
}

static void my_context_delete(my_context *ctx)
{
	free(ctx->name);
	free(ctx);
}

/**
 * A function, called from inside of coroutines recursively. Just to demonstrate
 * the example. You can split your code into multiple functions, that usually
 * helps to keep the individual code blocks simple.
 */
static void other_function(const char *name, int depth)
{
	printf("%s: entered function, depth = %d\n", name, depth);
	coro_yield();
	if (depth < 3)
		other_function(name, depth + 1);
}

/**
 * Coroutine body. This code is executed by all the coroutines. Here you
 * implement your solution, sort each individual file.
 */
static int coroutine_func_f(void *context)
{
	/* IMPLEMENT SORTING OF INDIVIDUAL FILES HERE. */

	coro *this = coro_this();
	my_context *ctx = context;
	char *name = ctx->name;
	printf("Started coroutine in ctx %s\n", name);
	printf("%s: ctx switch count %lld\n", name, coro_switch_count(this));
	printf("%s: yield\n", name);
	coro_yield();

	printf("%s: ctx switch count %lld\n", name, coro_switch_count(this));
	printf("%s: yield\n", name);
	coro_yield();

	printf("%s: ctx switch count %lld\n", name, coro_switch_count(this));
	other_function(name, 1);
	printf("%s: ctx switch count after other function %lld\n", name,
	       coro_switch_count(this));

	my_context_delete(ctx);
	/* This will be returned from coro_status(). */
	return 0;
}

static int* coro_merge_sort(int* ints, int size);

static int* coro_merge_sort_wrapper(void *context){
	coro *this = coro_this();
	my_context *ctx = context;
	char *name = ctx->name;

	int *returned = coro_merge_sort(ctx->ints, ctx->ints_size);
	int *result = malloc(sizeof(int)*ctx->ints_size);
	memcpy(result, returned, ctx->ints_size);

	if(PRINT_SORTED_ARRAYS){for(int i = 0; i <ctx->ints_size;i++){printf("%d ", result[i]);}}
	
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

/*
ход мыслей: псевдокод
main(){
	int[] array, int size;
	
	mergesort(array, size)
}

int[] mergesort(int* ints, int size){
	if (size == 1){
		return ints;

	int mid;

	sizeA = mid-0;
	int[] a = mergesort(ints[0], mid);

	sizeB = size-mid;
	int[] b = mergesort(ints[mid], size-mid);
	 
	int counterA = 0, counterB=0, counterRes=0;
	int[size] result;

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
	return result;
}
*/


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
	// memset(p_arrays, 0, sizeof(int*) * (argc-optind))
	unsigned int sizes[num_files];
	for (int optindex = optind, index=0; optindex < argc; optindex++, index++){
		printf ("got argument %s\n", argv[optindex]);
		//open it
		FILE *file = fopen(argv[optindex], "r");
		int elem_ind=0;
		int elements[MAX_ELEMENTS_IN_FILE];
		//scan elements
		while(fscanf(file, "%d", &elements[elem_ind++])==1){}
		//set nth size
		sizes[index] = elem_ind;
		//set it to 2d array
		p_arrays[index] = (int *)(p_arrays+num_files) + ((index) * MAX_ELEMENTS_IN_FILE); 
		// почему так?
		// потому что сначала мы берем указательное смещение, т.е. первую строку arr[0]
		// затем мы берем i-ю строку
		
		memcpy(p_arrays[index], elements, sizeof(int) * sizes[index]);

		// for(int i = 0; i < sizes[index]; i++){printf("%d ", p_arrays[index][i]);}
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

	char name[16];
	sprintf(name, "coro_%d", 0);

	begin = clock();

	for(int i=0; i<sizeof(sizes)/sizeof(sizes[0]); i++){
		coro_new(coro_merge_sort_wrapper, my_context_new(name, p_arrays[i], sizes[i]));
	}
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
	printf("ts async %f\n", (double)(end - begin) / CLOCKS_PER_SEC);
	/* All coroutines have finished. */
	free(p_arrays);
	/* IMPLEMENT MERGING OF THE SORTED ARRAYS HERE. */
	return 0;
}