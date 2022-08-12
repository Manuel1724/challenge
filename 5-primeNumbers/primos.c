#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>
#include <stdbool.h>
#include <omp.h>

bool primo (int n) {
        int i;
	bool primo_bool = true;

	#pragma omp parallel for 
        for (i = 3; i < (int)(sqrt(n) + 1); i+=2)
             if (n%i == 0)
                primo_bool = false;
        return primo_bool;
}


int main(int argc, char *argv[]) {
	int cont = 0, total = 0;
	long int i, n;
	int inicio, salto;

	if (argc < 2) {
        	printf("Valor inválido! Entre com um valor do maior inteiro\n");
       	 	return 0;
    	} else
        n = strtol(argv[1], (char **) NULL, 10);

    inicio = 3;
    salto = 1;
	
	#pragma omp parallel for
	for (i = inicio; i <= n; i += salto) 
		if(primo(i)) cont++;
		
	total = cont;
	
    total += 1;    /* Acrescenta o dois, que também é primo */
	printf("Quant. de primos entre 1 e n: %d \n", total);

	return(0);
}

