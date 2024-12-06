#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#define MAX_ELEMENTS_IN_FILE 100001

int* fun(int* ints, int size){

    int a[] = {1,2,3,4,5,6};
    return a[0];
}

int main(int argc, char **argv)
{	
	int ints[] = {0,1,2,3, 4, 5};
    int size = 6;
    int* r = fun(ints, size);
    for(int i = 0; i < size; i++){
        printf("r%d ", r[i]);
    }
	return 0;
}