/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */


#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <unistd.h>

typedef struct {
	int n, customers;
	sem_t * dataCopied;
	sem_t * mutex;
	sem_t * customer;
	sem_t * barber;
	sem_t * customerDone;
	sem_t * barberDone;
} Data;

int barber(void *inputData) {
	Data data = *((Data*)inputData);
	
	assert(sem_post(data.dataCopied) == 0);
	
	while(1) {
		assert(sem_wait(data.customer) == 0);
  	assert(sem_post(data.barber) == 0);
		printf("Barber is cutting hair\n");
		assert(sem_wait(data.customerDone) == 0);
		assert(sem_post(data.barberDone) == 0);
		  
	}
}

int customer(void *inputData) {
	Data data = *((Data*)inputData);
  
	assert(sem_post(data.dataCopied) == 0);
	
	while(1) {
  		usleep(rand() % 1000);
		assert(sem_wait(data.mutex) == 0);
			
		if (data.customers == data.n){
			assert(sem_post(data.mutex) == 0);
    			continue; //customer leaves      
 		}
		data.customers += 1;
		assert(sem_post(data.mutex) == 0);
		
		assert(sem_post(data.customer) == 0);
		assert(sem_wait(data.barber) == 0);
		
		printf("Customer is getting haircut done\n");
		
		assert(sem_post(data.customerDone) == 0);
		assert(sem_wait(data.barberDone) == 0);
			
		assert(sem_wait(data.mutex) == 0);
		data.customers -= 1;    
			assert(sem_post(data.mutex) == 0);
			  
		}  
}

int main() {
	int i = 0;
	pthread_t id[7];

	Data data;
	sem_t dataCopied, mutex, customer, customerDone, barber , barberDone;
  
	data.n = 4;
	data.customers = 0;

	assert(sem_init(&dataCopied, 0, 0) == 0);
	data.dataCopied = &dataCopied;
	assert(sem_init(&mutex, 0, 1) == 0);
	data.mutex = &mutex;
  
	assert(sem_init(&customer, 0, 0) == 0);
	data.customer = &customer;
  
	assert(sem_init(&customerDone, 0, 0) == 0);
	data.customerDone = &customerDone;
  
	assert(sem_init(&barber, 0, 0) == 0);
	data.barber = &barber;
  
	assert(sem_init(&barberDone, 0, 0) == 0);
	data.barberDone = &barberDone;
  
	assert(pthread_create(&id[0], NULL, barber, (void*)&data) == 0);
	assert(sem_wait(&dataCopied) == 0);

	for (i = 1; i < 7; i++){
		assert(pthread_create(&id[i], NULL, customer, (void*)&data) == 0);
		assert(sem_wait(&dataCopied) == 0);
	
	}

	for (i = 0; i < 7; i++) {
		assert(pthread_join(id[i], NULL) == 0);
	}

	return 0;
}
