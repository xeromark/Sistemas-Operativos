#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/* Tarea de Omar Javier Marca Perez. Rut: 25.824.728-0
Compilar:
gcc tarea.c -lpthread
*/
static int count = 0;

sem_t mutex; //Paso 1
sem_t S;	// Paso 4
sem_t x_num;	// Paso 2 variable de condicion

int S_count = 0; // Paso 5
int x_count = 0; // Paso 3 variable de condicion

void x_wait(){
	x_count++;

	if(S_count > 0){
        sem_post(&S);

	}
	else{
        sem_post(&mutex);

	}

	x_count--;

}

void x_signal(){
	if (x_count > 0) {
		S_count++;
        sem_post(&x_num);
        sem_wait(&S);
		S_count--;

	}
}

void *Monitor(void *arg) {

	//int VecesPorThread  = 0; 

	while(1){

		sem_wait(&mutex); // mutex = 0; detiene a los demas.
		
		if(count < 100){	// Condicion a tomar en consideracion.

			count++;	//Aumentamos en 1.
			//VecesPorThread++;
			printf("Thread Numero %ld sumando: %d \n", pthread_self(),count);

			x_wait();

		}
		else{
			sem_post(&mutex);
			break;
		}
	}
	//printf("Thread %ld sumo: %d \n", pthread_self(),VecesPorThread);
	pthread_exit(NULL);

}

int main(){

	sem_init(&mutex, 0, 1);
    sem_init(&S, 0, 0);		//Paso 4
    sem_init(&x_num, 0, 0);		//Paso 3 variable de condicion

    pthread_t HiloMonitor[10];

	for(int i = 0; i < 10;i++){
    	pthread_create(&HiloMonitor[i], NULL, Monitor, NULL);

	}

	for(int i = 0; i < 10;i++){
    	pthread_join(HiloMonitor[i],NULL);

	}

	printf("Valor final: %d \n" ,count);

	return 0;
}
