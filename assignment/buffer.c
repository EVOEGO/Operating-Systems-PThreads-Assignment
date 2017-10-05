#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h> 
#include "buffer.h"
#include <unistd.h>


#define TRUE 1

buffer_item buffer[BUFFER_SIZE];  //shared buffer	

int count = 0;

pthread_mutex_t mutex; 

sem_t full, empty; 
pthread_t tid; 
pthread_attr_t threadAttributes; 


void *producer(void *param); //producer thread
void *consumer(void *param); //consumer thread

void createDataToUse() {

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations" //this supresses wdeprecated declartaion exceptions, as this was happening alot when trying to compile the code.

    pthread_mutex_init(&mutex, NULL); 
    sem_init(&full, 0, 0); 
    sem_init(&empty, 0, BUFFER_SIZE); 
    pthread_attr_init(&threadAttributes);	
}


int insert_item(buffer_item item) {
	if (count < BUFFER_SIZE) {
		buffer[count] = item; 
		count++; 
		return 0; 
	}
	else {
		return -1; 
	}
}

int remove_item(buffer_item *item) {

	if (count > 0) {
		*item = buffer[(count - 1)]; 
		count--; 
		return 0; 
	} 
	else {
		return -1; 
	}

}

void *producer(void *param) {

	buffer_item item; 
	 
	while (TRUE) {
		int randNum = rand() / 100000000;

		// rand() % max + min
		sleep(randNum); 

		item = rand(); 

		sem_wait(&empty); 

		pthread_mutex_lock(&mutex); 

		if (insert_item(item)) {
			fprintf(stderr, "Producer Function not working :(\n"); 
		}
		else {
			printf("Producer produced %d\n", item); 
		}
		pthread_mutex_unlock(&mutex); 
		sem_post(&full);
	}

}

void *consumer(void *param) {

	buffer_item item; 

	while (TRUE) {
        int randNum = rand() / 100000000;
        
		sleep(randNum);

		sem_wait(&full); 

		pthread_mutex_lock(&mutex); 

		if (remove_item(&item)) {
			fprintf(stderr, "Consumer Failed :(\n"); 
		}
		else {
			printf("Consumer consumed %d\n", item); 

		}

		pthread_mutex_unlock(&mutex); 

		sem_post(&empty); 
	}

}

int main(int argc, char *argv[]) {
    
        pthread_t tid1, tid2; //thread identifiers
    
        int i; 
    
        if (argc != 4) {
            fprintf(stderr, "USAGE:./main.out <INT> <INT> <INT> \n"); 
        }
    
        int mainSleepTime = atoi(argv[1]); 
        int numberOfProducers = atoi(argv[2]); 
        int numberOfConsumers = atoi(argv[3]); 
    
        createDataToUse();

        for (int x = 0; x < numberOfProducers; x++) {
            pthread_create(&tid, &threadAttributes, producer, NULL); 
        }
    
        for (int y = 0; y < numberOfConsumers; y++) {
            pthread_create(&tid, &threadAttributes, consumer, NULL); 
        }
    
        sleep(mainSleepTime);
        
        printf("Exiting Program\n"); 
        exit(0); 
}