#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libcoro.h"
#define MAX_ELEMENTS_IN_FILE 100001

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
} my_context;

typedef struct coro coro;

static my_context* my_context_new(const char *name)
{
	my_context *ctx = malloc(sizeof(*ctx));
	ctx->name = strdup(name);
	ctx->num_coroutines = 0;
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
	printf("Started coroutine %s\n", name);
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

int main(int argc, char **argv)
{	
	int argind=0, optind=1, some_temp_var=0;
	while ((argind = getopt (argc, argv, "abc:")) != -1){optind++;}
	int num_files = argc-optind;

	int **p_arrays = malloc(sizeof(int*) * num_files + MAX_ELEMENTS_IN_FILE * sizeof(int)); //указатели на массивы с числами
	// memset(p_arrays, 0, sizeof(int*) * (argc-optind))
	int sizes[num_files];
	for (int index = optind; index < argc; index++){
		printf ("got argument %s\n", argv[index]);
		//open it
		FILE *file = fopen(argv[index], "r");
		int elem_ind=0;
		int elements[MAX_ELEMENTS_IN_FILE];
		//scan elements
		while(fscanf(file, "%d", &elements[elem_ind++])==1){}
		//set nth size
		sizes[index-optind] = elem_ind;
		//set it to 2d array
		p_arrays[index-optind] = &elements;
		}
    printf("test %d %d\n", p_arrays[0][0], p_arrays[0][1]);


	/* Initialize our coroutine global cooperative scheduler. */
	coro_sched_init();
	/* Start several coroutines. */
	for (int i = 0; i < 3; ++i) {
		/*
		 * The coroutines can take any 'void *' interpretation of which
		 * depends on what you want. Here as an example I give them
		 * some names.
		 */
		char name[16];
		sprintf(name, "coro_%d", i);
		/*
		 * I have to copy the name. Otherwise all the coroutines would
		 * have the same name when they finally start.
		 */
		coro_new(coroutine_func_f, my_context_new(name));
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
	/* All coroutines have finished. */
	free(p_arrays);
	/* IMPLEMENT MERGING OF THE SORTED ARRAYS HERE. */
	return 0;
}