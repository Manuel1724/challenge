#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "src/matrix.h"

int main(int argc, char **argv){
	char *awnser[] = { "bad", "ok" };
	
	int i, j, n, is_ok;
	double start, end, elapsed;
	
	if(argc < 2){
		fprintf(stderr, "Usage: %s <matrix size>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	n = atoi(argv[1]);

	n = n % 8 == 0 ? n : n + (8 - n % 8);
	
	printf("number of rows x cols: %d x %d\n", n, n);
	
	double *A = malloc(n * n * sizeof(double));
	if(A == NULL){
		fprintf(stderr, "Out of memory!\n");
		exit(EXIT_FAILURE);
	}
	
	double *B = malloc(n * n * sizeof(double));
	if(B == NULL){
		fprintf(stderr, "Out of memory!\n");
		exit(EXIT_FAILURE);
	}
	
	double *C = malloc(n * n * sizeof(double));
	if(C == NULL){
		fprintf(stderr, "Out of memory!\n");
		exit(EXIT_FAILURE);
	}
	#pragma omp parallel for private(i)
	for(i = 0; i < n; i++)
                #pragma omp parallel for private(j)
		for(j = 0; j < n; j++){
			A[i * n + j] = 1;
			B[i * n + j] = 1;
			C[i * n + j] = 0;
		}
	
	start = omp_get_wtime();
	matrix_mult(A, B, C, n);
	end = omp_get_wtime();
	
	elapsed = end - start;

	is_ok = 1;	
        #pragma omp parallel for private(i,j)
	for(i = 0; i < n; i++)
		for(j = 0; j < n; j++)
			if(C[i * n + j] != (double) n){
				is_ok = 0;
				i = n;
				j = n;
			}
	
	printf("multiply is %s\ntime: %.3f seconds\n", awnser[is_ok], elapsed);
	
	free(A);
	free(B);
	free(C);

	return 0;
}
