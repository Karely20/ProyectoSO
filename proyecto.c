#define _GNU_SOURCE
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

/* Constantes */
#define MAX_LINEA 2048
#define MAX_FILAS 1000
#define NUM_HILOS 3

/* Estructura de una transacciòn */
typedef struct{
	int id;
	float streams;
	float in_shazam;
	char key[10];
	char mode[15];
} Transaccion;

/* Variables globales */
Transaccion dataset[MAX_FILAS];
int total_registros = 0;

pthread_mutex_t mutex_stats = PTHREAD_MUTEX_INITIALIZER;
int total_nulos = 0;
int total_modas = 0;

/* Estructura de argumentos para cada hilo */
typedef struct{
	int id_hilo;
	int inicio;
	int fin;
} ArgsHilo;

/* calc_tiempo: diferencia entre dos timespec en segundo */
double calc_tiempo(struct timespec ini, struct timespec fin){
	return (fin.tv_sec - ini.tv_sec) + (fin.tv_nsec - ini.tv_nsec) / 1e9;
}

/* cargarDatos: lee el csv y llena dataset[]*/
/* Campos vacìos quedan con -1.0 o "VACIO" segun el tipo */
void cargarDatos(){
	FILE *archivo = fopen("spotify_2023.csv", "r");
	if(!archivo){
		printf("Error: No se encontrò el archivo spotify_2023.csv\n");
		exit(1);
	}

	char linea[MAX_LINEA];
	fgets(linea, MAX_LINEA, archivo); //saltar el encabezado

	int i = 0;
	while(fgets(linea, MAX_LINEA, archivo) && i < MAX_FILAS){
		/* Inicializar con valores nulos */
		dataset[i].id = i + 1;
		dataset[i].streams = -1.0;
		dataset[i].in_shazam = -1.0;
		strcpy(dataset[i].key, "VACIO");
		strcpy(dataset[i].mode, "VACIO");

		char *resto = linea;
		char *token;
		int col = 0;

		while ((token = strsep(&resto, ",")) != NULL){
			token[strcspn(token, "\r\n")] = 0;

			if(col == 6){
				/*Streams*/
				dataset[i].streams = (strlen(token) > 0) ? atof(token) : -1.0;
			}
			else if (col == 11){
				/*bpm*/
				dataset[i].in_shazam = (strlen(token) > 0) ? atof(token) : -1.0;
			}
			else if(col == 13){
				/*key - puede estar vacio*/
				if(strlen(token) > 0)
					strcpy(dataset[i].key, token);
			}
			else if(col == 14){
				/*mode*/
				if(strlen(token) > 0)
					strcpy(dataset[i].mode, token);
			}

			col++;
		}

		i++;
	}

	total_registros = i;
	fclose(archivo);
}

/* procesar_transaccion aplica las 3 transformaciones a una fila */
void procesar_transaccion(int index){
	//Paso 1: limpieza de vaores numéricos
	if(dataset[index].streams < 0){
		dataset[index].streams = 500000000.0; //Media aprox.
		pthread_mutex_lock(&mutex_stats);
		total_nulos++;
		pthread_mutex_unlock(&mutex_stats);
	}

	if(dataset[index].in_shazam < 0){
		dataset[index].in_shazam = 120.0; //Media aprox.
		pthread_mutex_lock(&mutex_stats);
		total_nulos++;
		pthread_mutex_unlock(&mutex_stats);
	}

	//Paso 2: imputacion de valores categoricos por moda
	if(strcmp(dataset[index].key, "VACIO") == 0){
		strcpy(dataset[index].key, "C#"); //Moda del dataset
		pthread_mutex_lock(&mutex_stats);
		total_modas++;
		pthread_mutex_unlock(&mutex_stats);
	}

	//Paso 3: nomralizacion min-max
	// Streams: min = 0; max = 3 500 000 000
	dataset[index].streams = dataset[index].streams / 3500000000;

	//bpm: min = 65, max = 206
	dataset[index].in_shazam = (dataset[index].in_shazam - 65.0) / (206.0 - 65.0);
}

/* Funcion que ejecuta cada pthread */
void *hilo_trabajo(void *args){
	struct timespec ini_hilo, fin_hilo;
	clock_gettime(CLOCK_MONOTONIC, &ini_hilo);

	ArgsHilo *params = (ArgsHilo *)args;
	for(int i = params -> inicio; i < params -> fin; i++){
		procesar_transaccion(i);
	}

	clock_gettime(CLOCK_MONOTONIC, &fin_hilo);
	double t_hilo = calc_tiempo(ini_hilo, fin_hilo);

	pthread_mutex_lock(&mutex_stats);
	printf("Hilo %d finalizado - filas [%d, %d] - tiempo: %.6f s\n",
		params -> id_hilo, params -> inicio, params -> fin, t_hilo);
	pthread_mutex_unlock(&mutex_stats);

	pthread_exit(NULL);
}

/* MAIN */
int main(){
	struct timespec ini_sec, fin_sec, ini_par, fin_par;

	// 1. Cargar datos
	cargarDatos();
	printf("Cargados %d registros.\n\n", total_registros);

	// 2. Modo SECUENCIAL
	printf("=== MODO SECUENCIAL (1 hilo) ===\n");
	clock_gettime(CLOCK_MONOTONIC, &ini_sec);
	for(int i = 0; i < total_registros; i++){
		procesar_transaccion(i);
	}

	clock_gettime(CLOCK_MONOTONIC, &fin_sec);
	double t_sec = calc_tiempo(ini_sec, fin_sec);
	printf("Tiempo secuencial: %.6f s\n", t_sec);
	printf("Nulos numèricos limpiados: %d\n", total_nulos);
	printf("Imputaciones con la moda: %d\n\n", total_modas);

	// 3. Reiniciar para el modo paralelo
	total_nulos = 0;
	total_modas = 0;
	cargarDatos(); //Recarga csv con nulos originales

	// 4. MODO PARALELO
	printf("=== MODO PARALELO (% hilos) ===\n", NUM_HILOS);
	pthread_t hilos[NUM_HILOS];
	ArgsHilo args[NUM_HILOS];
	int bloque = total_registros / NUM_HILOS;

	clock_gettime(CLOCK_MONOTONIC, &ini_par);
	for(int i = 0; i < NUM_HILOS; i++){
		args[i].id_hilo = i + 1;
		args[i].inicio = i * bloque;
		args[i].fin = (i == NUM_HILOS - 1) ? total_registros : (i + 1) * bloque;
		pthread_create(&hilos[i], NULL, hilo_trabajo, (void *)&args[i]);
	}
	for(int i = 0; i < NUM_HILOS; i++){
		pthread_join(hilos[i], NULL);
	}

	clock_gettime(CLOCK_MONOTONIC, &fin_par);
	double t_par = calc_tiempo(ini_par, fin_par);

	// 5. Resumen final
	printf("\n========================\n");
	printf("TIEMPO SECUENCIAL: %.6f s\n", t_sec);
	printf("TIEMPO PARALELO: %.6f s\n", t_par);
	printf("Speedup: %.2fx\n", t_sec / t_par);
	printf("Eficiencia: %.1f%%\n", (t_sec / t_par / NUM_HILOS) * 100.0);
	printf("=========================\n");
	printf("Nulos numéricos limpiados: %d\n", total_nulos);
	printf("Imputaciones con la moda: %d\n", total_modas);

	return 0;
}
