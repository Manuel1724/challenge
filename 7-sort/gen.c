#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <time.h>
#include <omp.h>


#define LENGTH 8

#define TAM 65530



int main(int argc, char *argv[]) {

	char str[LENGTH];

	long int tam;

	long int c;



	FILE *fout;



	srand(time(NULL));

	fout = stdout;

//	fout = fopen("sort.in", "w");

//	if (fout == NULL) {

//		perror("sort.in");

//		exit(EXIT_FAILURE);

//	}



	if (argc == 1)

		tam = TAM;

	else

		tam = atol(argv[1]);



	fprintf(fout, "%ld\n", tam);



	while (tam > 0) {
                #pragma omp parallel for
		for (c = 0; c < LENGTH-1; c++) {

			str[c] = 33 + (rand() % 94);

		}

		str[c] = 0;

		fprintf(fout, "%s\n", str);

		fflush(fout);

		tam--;

	}



//	fclose(fout);



	return EXIT_SUCCESS;

}
