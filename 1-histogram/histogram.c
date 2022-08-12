/* 
Este programa realiza o histograma de cores com 64 pontos de um arquivo de imagem no formato PPM. 
Há um cabeçalho inicial que informa o tamanho da imagem em pixels (largura x altura),
o maior valor para cada cor de cada pixel (8 ou 16 bits). A seguir a informação da imagem é apresentada, com os valores de cor vermelho, verde e azul para cada pixel, com um ou dois bytes dependendo do maior valor informado.  
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "mpi.h" 

#define RGB_COMPONENT_COLOR 255

typedef struct {
	unsigned char red, green, blue;
} PPMPixel;

typedef struct {
	int x, y;
	PPMPixel *data;
} PPMImage;

static PPMImage *readPPM() {
	char buff[16];
	PPMImage *img;
	FILE *fp;
	int c, rgb_comp_color;

	fp = stdin;

	if (!fgets(buff, sizeof(buff), fp)) {
		perror("entrada padrão");
		exit(1);
	}

	/* Faz a leitura do número mágico do arquivo */
	if (buff[0] != 'P' || buff[1] != '6') {
		fprintf(stderr, "Formato inválido: número mágico deve ser igual a 'P6'\n");
		exit(1);
	}
	/* Aloca a memória para a ponteiro da imagem */
	img = (PPMImage *) malloc(sizeof(PPMImage));
	if (!img) {
		fprintf(stderr, "Não foi possível alocar a memória para o ponteiro.\n");
		exit(1);
	}
	/* Pula os comentários */
	c = getc(fp);
	while (c == '#') {
		while (getc(fp) != '\n')
			;
		c = getc(fp);
	}
	/* Faz a leitura do tamanho da imagem: largura (X) e altura (Y)*/
	ungetc(c, fp);
	if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
		fprintf(stderr, "Tamanho de imagem inválido! \n");
		exit(1);
	}
	/* Faz a leitura do maior valor de cor da figura */
	if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
		fprintf(stderr, "Componente RGB inválido! \n");
		exit(1);
	}
	/* Se não for igual a 255 dá erro */
	if (rgb_comp_color != RGB_COMPONENT_COLOR) {
		fprintf(stderr, "A imagem não tem componentes de 8 bits. \n");
		exit(1);
	}
        /* Avança até o final da linha */
	while (fgetc(fp) != '\n')
		;
        /* Faz a alocação de memória para a imagem a ser lida */
	img->data = (PPMPixel*) malloc(img->x * img->y * sizeof(PPMPixel));

	if (!img) {
		fprintf(stderr, "Não foi possível alocar a memória.\n");
		exit(1);
	}
        /* Faz a leitura da imagem */
        /* Cada pixel é representado por 3 bytes com valores em binário*/
	if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
		fprintf(stderr, "Erro carregando a imagem.\n");
		exit(1);
	}

	return img;
}


void Histogram(PPMImage *image, float *h) {

	int i, j,  k, l, x, count;
	int rows, cols;

	float n = image->y * image->x;  /* Total de pixels da imagem */

	cols = image->x;
	rows = image->y;
        /* Faz a leitura de cada pixel, normalizando entre 0 e 3 */
	for (i = 0; i < n; i++) {
		image->data[i].red = floor((image->data[i].red * 4) / 256);
		image->data[i].blue = floor((image->data[i].blue * 4) / 256);
		image->data[i].green = floor((image->data[i].green * 4) / 256);
	}
        /* Faz a montagem do histograma */
	count = 0;
	x = 0;
        /* Para cada um dos 64 pontos do histograma ... */
	omp_set_num_threads(3);
	#pragma omp parallel for	
	for (j = 0; j <= 3; j++) {
		for (k = 0; k <= 3; k++) {
			for (l = 0; l <= 3; l++) {
                                /* ... percorre todos os pixels da imagem */
				for (i = 0; i < n; i++) {
					if (image->data[i].red == j && 
                                            image->data[i].green == k && 
                                            image->data[i].blue == l) {
						count++;
					}
				}
                                /* Monta o histograma percentual com 64 pontos */
				h[x] = count / n;
				count = 0;
				x++;
			}
		}
	}
}

int main(int argc, char *argv[]) {

	int i;

	PPMImage *image = readPPM();
        /* Aloca memória para o vetor do histograma */
	float *h = (float*)malloc(sizeof(float) * 64);
        
	/* Zera o vetor de histograma */
	#pragma omp parallel for private (i)
	for(i=0; i < 64; i++) h[i] = 0.0;
        
	/* Monta o histograma da imagem com 64 pontos */
	Histogram(image, h);
	
	/* Imprime o resultado do histograma */
	#pragma omp parallel for private (i)
	for (i = 0; i < 64; i++){
		printf("%0.3f ", h[i]);
	}
	printf("\n");
	free(h);

	return 0;
}
