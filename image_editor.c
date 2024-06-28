// Coitu Sebastian-Teodor 314CA
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#define maxv 255

typedef struct {
	int R, G, B;
} rgb;

typedef struct {
	int length, width; // dimensiunile imaginii
	int loaded; // arata daca este sau nu incarcata o imagine
	int img_type;
	char magic_word[3];
	int **grey;		// matricea pentru imaginea in nuante de gri
	rgb **color;	// matricea pentru imaginea color
	int x[2], y[2]; // vectori ce retin selectia
} image;

// eliberarea memoriei si reinitializarea datelor
void mem_free(image *img)
{
	// cazuri in functie de tipul imaginii
	if ((*img).img_type == 1) {
		for (int i = 0; i < (*img).length; i++)
			free((*img).grey[i]);
		free((*img).grey);
	} else {
		for (int i = 0; i < (*img).length; i++)
			free((*img).color[i]);
		free((*img).color);
	}

	(*img).loaded = 0;
	(*img).length = 0;
	(*img).width = 0;
	(*img).img_type = 0;
	(*img).magic_word[0] = '\0';
	(*img).x[0] = 0;
	(*img).x[1] = 0;
	(*img).y[0] = 0;
	(*img).y[1] = 0;
}

// alocarea memoriei pentru imaginea in tonuri de gri
int **matrix_alloc(int l, int c)
{
	int **matrix;
	int i;

	matrix = (int **)malloc(l * sizeof(int *));
	for (i = 0; i < l; i++)
		matrix[i] = (int *)malloc(c * sizeof(int));

	return matrix;
}

// alocarea memoriei pentru imaginea colora
rgb **color_matrix_alloc(int l, int c)
{
	rgb **matrix;
	int i;

	matrix = (rgb **)malloc(l * sizeof(rgb *));
	for (i = 0; i < l; i++)
		matrix[i] = (rgb *)malloc(c * sizeof(rgb));

	return matrix;
}

// trecerea peste comentarii
void comm_skip(FILE *input)
{
	char pos, buffer[1000];
	fscanf(input, "%c", &pos);

	// daca caracterul este '#', atunci se citeste linia pana la final,
	// alfel se intoarce de unde a inceput
	if (pos == '#')
		fgets(buffer, sizeof(buffer), input);
	else
		fseek(input, -1, SEEK_CUR);
}

// modificarea detaliilor imaginii in tonuri de gri
void grey(image *img)
{
	(*img).loaded = 1;
	(*img).img_type = 1;
	(*img).x[0] = 0;
	(*img).x[1] = (*img).width;
	(*img).y[0] = 0;
	(*img).y[1] = (*img).length;
}

// modificarea detaliilor imaginii colore
void color(image *img)
{
	(*img).loaded = 1;
	(*img).img_type = 2;
	(*img).x[0] = 0;
	(*img).x[1] = (*img).width;
	(*img).y[0] = 0;
	(*img).y[1] = (*img).length;
}

void load_p2(image *img, int l, int w, FILE *input)
{
	grey(img);
	(*img).grey = matrix_alloc(l, w);

	for (int i = 0; i < l; i++)
		for (int j = 0; j < w; j++)
			fscanf(input, "%d", &(*img).grey[i][j]);
}

void load_p3(image *img, int l, int w, FILE *input)
{
	int value;
	color(img);
	(*img).color = color_matrix_alloc(l, w);

	for (int i = 0; i < l; i++)
		for (int j = 0; j < w; j++) {
			fscanf(input, "%d", &value);
			(*img).color[i][j].R = value;
			fscanf(input, "%d", &value);
			(*img).color[i][j].G = value;
			fscanf(input, "%d", &value);
			(*img).color[i][j].B = value;
		}
}

void load_p5(image *img, int l, int w, FILE *input)
{
	grey(img);
	(*img).grey = matrix_alloc(l, w);
	unsigned char charater;

	for (int i = 0; i < l; i++)
		for (int j = 0; j < w; j++) {
			fread(&charater, sizeof(char), 1, input);
			(*img).grey[i][j] = (int)charater;
		}
}

void load_p6(image *img, int l, int w, FILE *input)
{
	color(img);
	(*img).color = color_matrix_alloc(l, w);
	unsigned char value;

	for (int i = 0; i < l; i++)
		for (int j = 0; j < w; j++) {
			fread(&value, sizeof(char), 1, input);
			(*img).color[i][j].R = (int)value;
			fread(&value, sizeof(char), 1, input);
			(*img).color[i][j].G = (int)value;
			fread(&value, sizeof(char), 1, input);
			(*img).color[i][j].B = (int)value;
		}
}

// incarcarea imaginii in memorie
void LOAD(image *img, char file[])
{
	// daca exista deja o imagine incarcata in memorie, atunci se elibereaza
	if ((*img).loaded == 1)
		mem_free(img);

	FILE *input = fopen(file, "rt");
	if (!input) {
		printf("Failed to load %s\n", file);
		return;
	}

	int var; // valoarea maxima a pixelilor imaginii
	comm_skip(input);
	fscanf(input, "%s", (*img).magic_word);
	comm_skip(input);
	fscanf(input, "%d%d", &(*img).width, &(*img).length);
	comm_skip(input);

	// se executa incarcarea in memorie in functie de tipul imaginii
	if (strcmp((*img).magic_word, "P2") == 0) {
		fscanf(input, "%d", &var);
		comm_skip(input);

		load_p2(img, (*img).length, (*img).width, input);
	} else if (strcmp((*img).magic_word, "P3") == 0) {
		fscanf(input, "%d", &var);
		comm_skip(input);

		load_p3(img, (*img).length, (*img).width, input);
	} else {
		fscanf(input, "%d", &var);
		comm_skip(input);

		int position = ftell(input);
		fclose(input);
		input = fopen(file, "rb");
		fseek(input, position + 1, SEEK_SET);

		if (strcmp((*img).magic_word, "P5") == 0)
			load_p5(img, (*img).length, (*img).width, input);
		else if (strcmp((*img).magic_word, "P6") == 0)
			load_p6(img, (*img).length, (*img).width, input);
	}

	fclose(input);
	printf("Loaded %s\n", file);
}

int verify(image *img, int x1, int x2, int y1, int y2)
{
	if (x1 < 0 || x1 > (*img).width || x2 < 0 || x2 > (*img).width) {
		printf("Invalid set of coordinates\n");
		return 0;
	}

	if (y1 < 0 || y1 > (*img).length || y2 < 0 || y2 > (*img).length) {
		printf("Invalid set of coordinates\n");
		return 0;
	}

	if (x1 == x2 || y1 == y2) {
		printf("Invalid set of coordinates\n");
		return 0;
	}

	return 1;
}

// selectarea in functie de coordonate
void select_coordinates(image *img, int x1, int x2, int y1, int y2)
{
	// ordonarea coordonatelor
	if (x1 > x2) {
		int aux = x1;
		x1 = x2;
		x2 = aux;
	}

	if (y1 > y2) {
		int aux = y1;
		y1 = y2;
		y2 = aux;
	}

	// daca se verifica si restul conditiilor, atunci se va realiza selectia
	int check = verify(img, x1, x2, y1, y2);
	if (!check)
		return;

	(*img).x[0] = x1;
	(*img).x[1] = x2;
	(*img).y[0] = y1;
	(*img).y[1] = y2;

	printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
}

// in functie de parametrii, se alege ce selectie se realizeaza
void SELECT(image *img, char input[])
{
	if ((*img).loaded == 0) {
		printf("No image loaded\n");
		return;
	}

	char *p;
	p = strtok(input, "\n ");
	p = strtok(NULL, "\n ");

	if (!p) {
		printf("Invalid command\n");
		return;
	}

	if (strcmp(p, "ALL") == 0) {
		(*img).x[0] = 0;
		(*img).x[1] = (*img).width;
		(*img).y[0] = 0;
		(*img).y[1] = (*img).length;

		printf("Selected ALL\n");
	} else {
		int x1, x2, y1, y2;

		// se verifica daca coordonatele sunt corecte
		if ((p[0] >= '0' && p[0] <= '9') || p[0] == '-') {
			x1 = (int)atoi(p);
		} else {
			printf("Invalid command\n");
			return;
		}

		p = strtok(NULL, "\n ");
		if (p) {
			if ((p[0] >= '0' && p[0] <= '9') || p[0] == '-') {
				y1 = (int)atoi(p);
			} else {
				printf("Invalid command\n");
				return;
			}
		} else {
			printf("Invalid command\n");
			return;
		}

		p = strtok(NULL, "\n ");
		if (p) {
			if ((p[0] >= '0' && p[0] <= '9') || p[0] == '-') {
				x2 = (int)atoi(p);
			} else {
				printf("Invalid command\n");
				return;
			}
		} else {
			printf("Invalid command\n");
			return;
		}

		p = strtok(NULL, "\n ");
		if (p) {
			if ((p[0] >= '0' && p[0] <= '9') || p[0] == '-') {
				y2 = (int)atoi(p);
			} else {
				printf("Invalid command\n");
				return;
			}
		} else {
			printf("Invalid command\n");
			return;
		}

		select_coordinates(img, x1, x2, y1, y2);
	}
}

// reducerea histogramei la numarul de bin-uri cerut
void reduction(int y, int *count)
{
	int n = 0, i, j;
	int interval = 256 / y;
	int *aux = (int *)calloc(256, sizeof(int));

	for (i = 0; i < 256; i = i + interval) {
		j = 0;
		while (j < interval) {
			aux[n] = aux[n] + count[j + i];
			j++;
		}
		n++;
	}

	for (i = 0; i < y; i++)
		count[i] = aux[i];

	free(aux);
}

// afisarea histogramei
void histogram_print (int *count, int x, int y)
{
	int i, j, star_nr, max = 0;
	double sum;

	// calcularea valorii maxime din vectorul de frecventa
	for (i = 0; i < y; i++)
		if (max < count[i])
			max = count[i];

	for (i = 0; i < y; i++) {
		sum = 0;
		// calcularea numarului de stelute
		sum = (double)count[i] / max * x;
		star_nr = (int)sum;

		printf("%d\t|\t", star_nr);
		for (j = 0; j < star_nr; j++)
			printf("*");
		printf("\n");
	}
}

// se realizeaza histograma imaginii
void HISTOGRAM(image *img, char input[])
{
	if ((*img).loaded == 0) {
		printf("No image loaded\n");
		return;
	}

	char *p = strtok(input, "\n ");
	p = strtok(NULL, "\n ");

	int x, y;
	if (p) {
		x = atoi(p);
	} else {
		printf("Invalid command\n");
		return;
	}

	p = strtok(NULL, "\n ");
	if (p) {
		y = atoi(p);
		if (y > 256) {
			printf("Invalid set of parameters\n");
			return;
		}
	} else {
		printf("Invalid command\n");
		return;
	}

	p = strtok(NULL, "\n ");
	if (p) {
		printf("Invalid command\n");
		return;
	}

	if ((*img).img_type == 2) {
		printf("Black and white image needed\n");
		return;
	}

	int i, j;
	// vector de frecventa pentru valorile pixelilor
	int *count = (int *)calloc(256, sizeof(int));
	for (i = 0; i < (*img).length; i++)
		for (j = 0; j < (*img).width; j++)
			count[(*img).grey[i][j]]++;

	reduction(y, count);
	histogram_print(count, x, y);

	free(count);
}

// realizarea calculelor
void formula(image *img, int *count, int i, int j)
{
	int s = 0, k;
	double res;
	double area = (double)(*img).length * (*img).width;

	for (k = 0; k <= (*img).grey[i][j]; k++)
		s += count[k];

	res = 255 * (double)(1 / area) * s;
	(*img).grey[i][j] = round(res);

	if ((*img).grey[i][j] < 0)
		(*img).grey[i][j] = 0;
	else if ((*img).grey[i][j] > 255)
		(*img).grey[i][j] = 255;
}

// egalizarea imaginii
void EQUALIZE(image *img)
{
	int i, j;
	if ((*img).loaded == 0) {
		printf("No image loaded\n");
		return;
	} else if ((*img).img_type == 2) {
		printf("Black and white image needed\n");
		return;
	}

	int *count = (int *)calloc(256, sizeof(int));

	// vector de frecventa pentru valorile pixelilor
	for (i = 0; i < (*img).length; i++)
		for (j = 0; j < (*img).width; j++)
			count[(*img).grey[i][j]]++;

	// se aplica formula de calcul pe fiecare pixel in parte
	for (i = 0; i < (*img).length; i++)
		for (j = 0; j < (*img).width; j++)
			formula(img, count, i, j);

	free(count);
	printf("Equalize done\n");
}

// crop pentru imaginea in tonuri de gri
void crop_grey(image *img)
{
	int i, j;
	int new_l = (*img).y[1] - (*img).y[0];
	int new_c = (*img).x[1] - (*img).x[0];
	int **aux_arr = matrix_alloc(new_l, new_c);

	// se copiaza elementele din selectie in matricea auxiliara
	for (i = 0; i < new_l; i++)
		for (j = 0; j < new_c; j++)
			aux_arr[i][j] = (*img).grey[i + (*img).y[0]][j + (*img).x[0]];

	// eliberarea memoriei matricei principale
	for (i = 0; i < (*img).length; i++)
		free((*img).grey[i]);
	free((*img).grey);

	// se creeaza noua imagine
	(*img).grey = matrix_alloc(new_l, new_c);
	for (i = 0; i < new_l; i++)
		for (j = 0; j < new_c; j++)
			(*img).grey[i][j] = aux_arr[i][j];

	// actualizarea datelor
	(*img).length = new_l;
	(*img).width = new_c;
	(*img).x[0] = 0;
	(*img).x[1] = new_c;
	(*img).y[0] = 0;
	(*img).y[1] = new_l;

	// eliberarea memoriei matricei auxiliare
	for (i = 0; i < new_l; i++)
		free(aux_arr[i]);
	free(aux_arr);
}

// crop pentru imaginea colora
void crop_color(image *img)
{
	int i, j;
	int new_l = (*img).y[1] - (*img).y[0];
	int new_c = (*img).x[1] - (*img).x[0];
	rgb **aux_arr = color_matrix_alloc(new_l, new_c);

	// se copiaza elementele din selectie in matricea auxiliara
	for (i = 0; i < new_l; i++)
		for (j = 0; j < new_c; j++) {
			aux_arr[i][j].R = (*img).color[i + (*img).y[0]][j + (*img).x[0]].R;
			aux_arr[i][j].G = (*img).color[i + (*img).y[0]][j + (*img).x[0]].G;
			aux_arr[i][j].B = (*img).color[i + (*img).y[0]][j + (*img).x[0]].B;
		}

	// eliberarea memoriei matricei principale
	for (i = 0; i < (*img).length; i++)
		free((*img).color[i]);
	free((*img).color);

	// se creeaza noua imagine
	(*img).color = color_matrix_alloc(new_l, new_c);
	for (i = 0; i < new_l; i++)
		for (j = 0; j < new_c; j++) {
			(*img).color[i][j].R = aux_arr[i][j].R;
			(*img).color[i][j].G = aux_arr[i][j].G;
			(*img).color[i][j].B = aux_arr[i][j].B;
		}

	// actualizarea datelor
	(*img).length = new_l;
	(*img).width = new_c;
	(*img).x[0] = 0;
	(*img).x[1] = new_c;
	(*img).y[0] = 0;
	(*img).y[1] = new_l;

	// eliberarea memoriei matricei auxiliare
	for (i = 0; i < new_l; i++)
		free(aux_arr[i]);
	free(aux_arr);
}

void CROP(image *img)
{
	if ((*img).loaded == 0) {
		printf("No image loaded\n");
		return;
	}

	if ((*img).img_type == 1)
		crop_grey(img);

	if ((*img).img_type == 2)
		crop_color(img);

	printf("Image cropped\n");
}

// construirea kernelului de imagine
double **kernel_build(int value)
{
	int i, j;
	double **kernel = (double **)malloc(3 * sizeof(double *));
	for (i = 0; i < 3; i++)
		kernel[i] = (double *)malloc(3 * sizeof(double));

	// EDGE
	if (value == 1) {
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				if (i == 1 && j == 1)
					kernel[i][j] = 8;
				else
					kernel[i][j] = -1;
	// SHARPEN
	} else if (value == 2) {
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				if (i != 1 && j != 1)
					kernel[i][j] = 0;
				else if (i == 1 && j == 1)
					kernel[i][j] = 5;
				else
					kernel[i][j] = -1;
	// BLUR
	} else if (value == 3) {
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				kernel[i][j] = (double)1 / 9;
	// GAUSSIAN BLUR
	} else {
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				if (i != 1 && j != 1)
					kernel[i][j] = 1 * (double)1 / 16;
				else if (i == 1 && j == 1)
					kernel[i][j] = 4 * (double)1 / 16;
				else
					kernel[i][j] = 2 * (double)1 / 16;
	}

	return kernel;
}

int clamp(double value)
{
	if (value < 0)
		return 0;
	else if (value > 255)
		return 255;

	return value;
}

void filter(image *img, int value)
{
	int i, j, k, l;
	double **kernel = kernel_build(value);
	rgb **aux_arr = color_matrix_alloc((*img).length, (*img).width);

	// se construieste o copie a imaginii
	for (i = 0; i < (*img).length; i++)
		for (j = 0; j < (*img).width; j++) {
			aux_arr[i][j].R = (*img).color[i][j].R;
			aux_arr[i][j].G = (*img).color[i][j].G;
			aux_arr[i][j].B = (*img).color[i][j].B;
		}

	// se elimina marginile imaginii din selectie, daca este nevoie
	int s_x, s_y, f_x, f_y;
	if ((*img).x[0] == 0)
		s_x = 1;
	else
		s_x = (*img).x[0];

	if ((*img).x[1] == (*img).width)
		f_x = (*img).width - 1;
	else
		f_x = (*img).x[1];

	if ((*img).y[0] == 0)
		s_y = 1;
	else
		s_y = (*img).y[0];

	if ((*img).y[1] == (*img).length)
		f_y = (*img).length - 1;
	else
		f_y = (*img).y[1];

	// se aplica kernelul pe imagine
	for (i = s_y; i < f_y; i++)
		for (j = s_x; j < f_x; j++) {
			double aux_R, aux_G, aux_B;
			aux_R = 0, aux_G = 0, aux_B = 0;

			for (k = i - 1; k <= i + 1; k++)
				for (l = j - 1; l <= j + 1; l++) {
					int line = k - i + 1;
					int column = l - j + 1;
					aux_R += (*img).color[k][l].R * kernel[line][column];
					aux_G += (*img).color[k][l].G * kernel[line][column];
					aux_B += (*img).color[k][l].B * kernel[line][column];
				}

			aux_arr[i][j].R = clamp(round(aux_R));
			aux_arr[i][j].G = clamp(round(aux_G));
			aux_arr[i][j].B = clamp(round(aux_B));
		}

	// se copiaza matricea modificata inapoi in cea principala
	for (i = 0; i < (*img).length; i++)
		for (j = 0; j < (*img).width; j++) {
			(*img).color[i][j].R = aux_arr[i][j].R;
			(*img).color[i][j].G = aux_arr[i][j].G;
			(*img).color[i][j].B = aux_arr[i][j].B;
		}

	for (i = 0; i < 3; i++)
		free(kernel[i]);
	free(kernel);

	for (i = 0; i < (*img).length; i++)
		free(aux_arr[i]);
	free(aux_arr);
}

// aplicarea filtrului
void APPLY(image *img, char input[])
{
	if ((*img).loaded == 0) {
		printf("No image loaded\n");
		return;
	}

	char *p, copy[20];
	p = strtok(input, "\n ");
	p = strtok(NULL, "\n ");

	// cazul in care nu exista un parametru
	if (!p) {
		printf("Invalid command\n");
		return;
	}

	if ((*img).img_type == 1) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	// cazurile pentru fiecare parametru
	if (strcmp(p, "EDGE") == 0) {
		// verific daca dupa parametrul mai exista un cuvant
		strcpy(copy, p);
		p = strtok(NULL, "\n ");
		if (p) {
			printf("APPLY parameter invalid\n");
			return;
		}
		filter(img, 1);
	} else if (strcmp(p, "SHARPEN") == 0) {
		strcpy(copy, p);
		p = strtok(NULL, "\n ");
		if (p) {
			printf("APPLY parameter invalid\n");
			return;
		}
		filter(img, 2);
	} else if (strcmp(p, "BLUR") == 0) {
		strcpy(copy, p);
		p = strtok(NULL, "\n ");
		if (p) {
			printf("APPLY parameter invalid\n");
			return;
		}
		filter(img, 3);
	} else if (strcmp(p, "GAUSSIAN_BLUR") == 0) {
		strcpy(copy, p);
		p = strtok(NULL, "\n ");
		if (p) {
			printf("APPLY parameter invalid\n");
			return;
		}
		filter(img, 4);
	} else {
		printf("APPLY parameter invalid\n");
		return;
	}

	printf("APPLY %s done\n", copy);
}

// salveaza imaginea in tonuri de gri in format ascii
void save_a_grey(image *img, FILE *output)
{
	fprintf(output, "%s\n", "P2");
	fprintf(output, "%d %d\n%d\n", (*img).width, (*img).length, maxv);

	for (int i = 0; i < (*img).length; i++) {
		for (int j = 0; j < (*img).width; j++)
			fprintf(output, "%d ", (*img).grey[i][j]);
		fprintf(output, "\n");
	}
}

// salveaza imaginea in tonuri de gri in format binar
void save_b_grey(image *img, FILE *output)
{
	fprintf(output, "%s\n", "P5");
	fprintf(output, "%d %d\n%d\n", (*img).width, (*img).length, maxv);

	unsigned char character;
	for (int i = 0; i < (*img).length; i++)
		for (int j = 0; j < (*img).width; j++) {
			character = (unsigned char)(*img).grey[i][j];
			fwrite(&character, 1, 1, output);
		}
}

// salveaza imaginea color in format ascii
void save_a_color(image *img, FILE *output)
{
	fprintf(output, "%s\n", "P3");
	fprintf(output, "%d %d\n%d\n", (*img).width, (*img).length, maxv);

	for (int i = 0; i < (*img).length; i++) {
		for (int j = 0; j < (*img).width; j++) {
			fprintf(output, "%d ", (*img).color[i][j].R);
			fprintf(output, "%d ", (*img).color[i][j].G);
			fprintf(output, "%d ", (*img).color[i][j].B);
		}
		fprintf(output, "\n");
	}
}

// salveaza imaginea color in format binar
void save_b_color(image *img, FILE *output)
{
	fprintf(output, "%s\n", "P6");
	fprintf(output, "%d %d\n%d\n", (*img).width, (*img).length, maxv);

	unsigned char value;
	for (int i = 0; i < (*img).length; i++)
		for (int j = 0; j < (*img).width; j++) {
			value = (unsigned char)(*img).color[i][j].R;
			fwrite(&value, 1, 1, output);
			value = (unsigned char)(*img).color[i][j].G;
			fwrite(&value, 1, 1, output);
			value = (unsigned char)(*img).color[i][j].B;
			fwrite(&value, 1, 1, output);
		}
}

// salvarea imaginii
void SAVE(image *img, char input[])
{
	if ((*img).loaded == 0) {
		printf("No image loaded\n");
		return;
	}

	// cautarea fisierului in care se salveaza si verificarea formatului
	char file_name[50], *p;
	p = strtok(input, "\n ");
	p = strtok(NULL, "\n ");
	strcpy(file_name, p);

	p = strtok(NULL, "\n ");
	if (p) {
		if (strcmp(p, "ascii") == 0) {
			FILE *output = fopen(file_name, "wt");
			if (!output) {
				printf("File open failed\n");
				return;
			}

			if ((*img).img_type == 1)
				save_a_grey(img, output);
			else if ((*img).img_type == 2)
				save_a_color(img, output);

			fclose(output);
		} else {
			printf("SAVE parameter invalid\n");
		}
	} else {
		FILE *output = fopen(file_name, "wb");
		if (!output) {
			printf("File open failed\n");
			return;
		}

		if ((*img).img_type == 1)
			save_b_grey(img, output);
		else if ((*img).img_type == 2)
			save_b_color(img, output);

		fclose(output);
	}

	printf("Saved %s\n", file_name);
}

// finalizeaza programul
int EXIT(image *img)
{
	if ((*img).loaded == 0) {
		printf("No image loaded\n");
		return 1;
	}
	mem_free(img);
	return 1;
}

int main(void)
{
	char command[50], string[100], aux[50];
	char *p;
	bool run = true;
	image img;
	// inainte de while nu este nicio imagine incarcata
	img.loaded = 0;

	while (run) {
		// se citeste input-ul de la tastatura
		fgets(string, 100, stdin);
		// se face o copie pentru verificarea cerintelor
		strcpy(aux, string);
		// extragerea comenzii
		p = strtok(string, "\n ");
		strcpy(command, p);

		if (strcmp(command, "LOAD") == 0) {
			// se extrage numele fisierului
			p = strtok(NULL, "\n ");
			LOAD(&img, p);
		} else if (strcmp(command, "SELECT") == 0) {
			SELECT(&img, aux);
		} else if (strcmp(command, "HISTOGRAM") == 0) {
			HISTOGRAM(&img, aux);
		} else if (strcmp(command, "EQUALIZE") == 0) {
			EQUALIZE(&img);
		} else if (strcmp(command, "CROP") == 0) {
			CROP(&img);
		} else if (strcmp(command, "APPLY") == 0) {
			APPLY(&img, aux);
		} else if (strcmp(command, "SAVE") == 0) {
			SAVE(&img, aux);
		} else if (strcmp(command, "EXIT") == 0) {
			if (EXIT(&img))
				run = false;
		} else {
			printf("Invalid command\n");
		}
	}

	return 0;
}
