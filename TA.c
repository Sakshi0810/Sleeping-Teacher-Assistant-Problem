#include <stdio.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <pthread.h>  
#include <semaphore.h> 
#include<time.h>
sem_t chair_sem[3];
sem_t student;
sem_t ta_sleep; 
pthread_t *Students; 
pthread_t TA; 
int chair_count = 0;
int index_chair = 0;
pthread_mutex_t ChairAccess;
int timeout ( int seconds )
{
    clock_t endwait;
    endwait = clock () + seconds * CLOCKS_PER_SEC ;
    while (clock() < endwait) {}
    return  1;
}
void * TA_check()
{	
      while(timeout(5)==1)
              {
		sem_wait(&ta_sleep);	
		printf("student come\n");
		printf("Teacher's assistant has been awakened by a student..................\n");
		while(1)
		{
			pthread_mutex_lock(&ChairAccess);
			if(chair_count == 0) 
			{
				pthread_mutex_unlock(&ChairAccess);
				break;
			}
			
                        sem_post(&chair_sem[index_chair]); 
			chair_count--;  
			printf("Student  left his chair in Hallway and goes to the teacher now remaining Chairs are %d\n",3 - chair_count);
			index_chair = (index_chair + 1) % 3;
			pthread_mutex_unlock(&ChairAccess);
			printf("\t Teacher is currently solving the doubts of the student.\n");
			sleep(5);
			sem_post(&student);
			usleep(100);
                   }
	}
}
void *Student_Check(void *threadID) 
{
	int Time_with_ta;
	while(1)
	{
		printf("Student %ld doing assignment .\n", (long)threadID);
		Time_with_ta = rand() % 10 + 1;; 
		sleep(Time_with_ta);		
		printf("Student %ld needs help from the TA\n", (long)threadID);
		pthread_mutex_lock(&ChairAccess); 
		int count = chair_count;
		pthread_mutex_unlock(&ChairAccess);
		if(count < 3)	
		{
			if(count == 0)		
				sem_post(&ta_sleep);
			else
			printf("Student %ld sat on a chair waiting for the taecher to finish explaining the  previous student \n", (long)threadID);
			pthread_mutex_lock(&ChairAccess);
			int index = (index_chair + chair_count) % 3;
			chair_count++;
			printf("Student sat on chair.Chairs Remaining: %d\n", 3 - chair_count);
			pthread_mutex_unlock(&ChairAccess);
			sem_wait(&chair_sem[index]);	
			printf("\t Student %ld is getting help from the TA. \n", (long)threadID);
			sem_wait(&student);		
			printf("Student %ld left TA room.\n",(long)threadID);
		}
		else 
			printf("Student %ld will return at another time. \n", (long)threadID);
	}
}
int main()
{

	int number_of_students ;		
        printf("\n enter the number of students : ");
        scanf("%d",&number_of_students);
	int id;
	srand(time(NULL));
	sem_init(&ta_sleep, 0, 0);
	sem_init(&student, 0, 0);
	for(id = 0; id < 3; ++id)			
		sem_init(&chair_sem[id], 0, 0);
	pthread_mutex_init(&ChairAccess, NULL);
	Students = (pthread_t*) malloc(sizeof(pthread_t)*number_of_students);
	pthread_create(&TA, NULL, TA_check, (void*) (long)id);	
	for(id = 0; id < number_of_students; id++)
		pthread_create(&Students[id], NULL, Student_Check,(void*) (long)id);
        pthread_join(TA, NULL);
	for(id = 0; id < number_of_students; id++)
       pthread_join(Students[id], NULL);
       free(Students);
	return 0;
}