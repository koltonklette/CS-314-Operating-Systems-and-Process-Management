/* **************************************************************** *
 * project_03.c                                                     *
 *                                                                  *
 *   This C source code file is for demonstrating:                  *
 *                                                                  *
 *         (1) Create and start running a new child thread          *
 *         (2) pthread sempahore for binary sempahore to            *
 *             synchronize threads in a process.                    *
 *         (3) Let the parent thread wait for the termination of    *
 *             a child thread it created.                           *
 *                                                                  *
 *   YOUR NAME: Kolton Klette                                       *
 *                                                                  *
 *   YOUR 3-DIGITS: 147                                             *
 *                                                                  *
 *   server: os.cs.siue.edu                                         *
 *   compile: gcc project3_147.c -lpthread                          *
 *                                                                  *
 *                                                                  *
 * **************************************************************** */

#include <stdio.h>               // for pirntf
#include <pthread.h>             // for pthread
#include <unistd.h>              // for usleep   
#include <semaphore.h>           // for semaphores
#include <time.h>                // for "time" (srand)

//Other Include Libraries
#include <string.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/sem.h>
#include <sys/shm.h>

#define  NUM_REPEATS    30
#define  MSG_SIZE       256      // the message size = 256 bytes

#define  TRUE           1        // the TRUE code
#define  FALSE          0        // the FALSE code

 /* ---------------------------------------------------------- */
#define H1_TIME_IN      200000      //    200ms  = 0.2 seconds
#define H1_TIME_OUT     200000      //    200ms  = 0.2 seconds

#define H2_TIME_IN      200000      //    200ms  = 0.2 seconds
#define H2_TIME_OUT     200000      //    200ms  = 0.2 seconds

#define H3_TIME_IN      200000      //    200ms  = 0.2 seconds
#define H3_TIME_OUT     200000      //    200ms  = 0.2 seconds

/* ----------------------------------------------------------- */
#define L1_TIME_IN      100000      //    100ms  = 0.1 seconds 
#define L1_TIME_OUT     600000      //    600ms  = 0.6 seconds

#define L2_TIME_IN      100000      //    100ms  = 0.1 seconds
#define L2_TIME_OUT     600000      //    600ms  = 0.6 seconds

#define L3_TIME_IN      100000      //    100ms  = 0.1 seconds
#define L3_TIME_OUT     600000      //    600ms  = 0.6 seconds

/* PROTOTYPES ------------------------------------------------------ */
void millisleep(unsigned ms);      // micro-sleep    
void* process_Lx(sem_t semaphore, int processNum, int processTimeIn, int processTimeOut); // for low-priority processes
void* process_Hx(sem_t semaphore, int processNum, int processTimeIn, int processTimeOut); // for high-priority processes

//Control Prototypes For pthread_create Function Calls
void* ThreadH1Control(void* arg);
void* ThreadH2Control(void* arg);
void* ThreadH3Control(void* arg);
void* ThreadL1Control(void* arg);
void* ThreadL2Control(void* arg);
void* ThreadL3Control(void* arg);

/* GLOBAL VARIABLES ------------------------------------------------ */
char message[MSG_SIZE];               /* the shared message          */

//Additional Global Variables
int childCount; //keeps track of currently active child threads
int parentCount; //keeps track of currently active parent threads

sem_t mutex1;
sem_t mutex2;
sem_t mutex3;
sem_t mutex4;

/* MODULE MAIN ----------------------------------------------------- */
void main(void)
{
	int i;                             /* loop counter                */
	int ret;                           /* return code                 */

	/* shuffle the random seed -------------------------------------- */
	srand((unsigned)time(NULL));

	/* initialize the shared message -------------------------------- */
	for (i = 0; i < MSG_SIZE; i++)
	{
		message[i] = '\0';
	}

	/* Semaphore Creation/Initialization Block ------------------------------------- */

	//Semaphore Initialization
	printf("Creating semaphores ... \n");

	sem_init(&mutex1, 0, 1);
	sem_init(&mutex2, 0, 2);
	sem_init(&mutex3, 0, 3);
	sem_init(&mutex4, 0, 4);

	//Thread ID Creation
	pthread_t ptidH2; //ID used to create Thread H2
	pthread_t ptidH3; //ID used to create Thread H3
	pthread_t ptidL1; //ID used to create Thread L1
	pthread_t ptidL2; //ID used to create Thread L2
	pthread_t ptidL3; //ID used to create Thread L3
	pthread_t ptidH1; //ID used to create Thread H1

	//Thread Progression Block
		
		//Thread Creation Block
		pthread_create(&ptidH2, NULL, ThreadH2Control, NULL);
		pthread_create(&ptidH3, NULL, ThreadH3Control, NULL);
		pthread_create(&ptidL1, NULL, ThreadL1Control, NULL);
		pthread_create(&ptidL2, NULL, ThreadL2Control, NULL);
		pthread_create(&ptidL3, NULL, ThreadL3Control, NULL);
		pthread_create(&ptidH1, NULL, ThreadH1Control, NULL);

		//Thread "Join" Block
		pthread_join(ptidH2, NULL); //start process H2
		pthread_join(ptidH3, NULL); //start process H3
		pthread_join(ptidL1, NULL); //start process L1
		pthread_join(ptidL2, NULL); //start process L2
		pthread_join(ptidL3, NULL); //start process L3
		pthread_join(ptidH1, NULL); //start process H1

		//Semaphore Termination Block
		sem_destroy(&mutex1);
		sem_destroy(&mutex2);
		sem_destroy(&mutex3);
		sem_destroy(&mutex4);

	/* parent is terminating ----------------------------------------- */
	printf("the parent thread is terminating ...\n");
}

/* function "millisleep" -------------------------------------------- */
void millisleep(unsigned micro_seconds)
{
	unsigned sleep_time;    // sleep time

	/* randomly generate the sleep time ---------- */
	sleep_time = rand() % micro_seconds;

	/* start sleeping ---------------------------- */
	usleep(sleep_time);
}

/* function "low priority process" ---------------------------------- */
void* process_Lx(sem_t semaphore, int processNum, int processTimeIn, int processTimeOut)
{
	//Variable Declaration
	int i = 0;

	//Initialize Child Process
	childCount++;

	// Low-Priority Process Begins
	printf("L%d starts ....\n", processNum);

	//Main While Loop
	while (parentCount > 0)
	{
		//Wait Semaphore Operations
		sem_wait(&semaphore);

		//External Sleep
		millisleep(processTimeOut);

		// START OF CRITICAL SECTION 

		printf("        L%d starts reading ...\n", processNum);

		//Reading/Showing Shared Message
		printf("                              L%d reads: %s!\n", processNum, message);

		//Internal Sleep
		millisleep(processTimeIn);

		//Signal Semaphore Operations
		sem_post(&semaphore);

		//Low-Prioirity Process Finishes
		printf("        L%d finishes reading ...\n", processNum);

		// END OF CRITICAL SECTION 
	}
	//Completion of Low-Priority Process
	childCount--;

	//Process Termination
	printf("L%d is terminating ...\n", processNum);
};

/* function "high-priority process" --------------------------------- */
void* process_Hx(sem_t semaphore, int processNum, int processTimeIn, int processTimeOut)
{
	//int valueReturn = 0;
	int i = 0;
	int j = 0;

	//Initialize Parent Process
	parentCount++;
	
	//High-Priority Process Starts
	printf("H%d starts ....\n", processNum);

	//Main For Loop
	for (i = 0; i < NUM_REPEATS; i++)
	{
		//External Sleep
		millisleep(processTimeOut);

		//Wait Semaphore Operations
		sem_wait(&semaphore);

		printf("H%d would like to update ...\n", processNum);

		/* START OF CRITICAL SECTION */

		printf("H%d starts updating ...\n", processNum);

		//Updating Process
		for (j = 0; j < MSG_SIZE; j++)   // reset the msg
		{
			message[j] = '\0';
		}

		//Update Control Block (based on process)
		if (processNum == 1)
		{
			strcpy(message, "I am H1"); // the new msg
		}
		else if (processNum == 2)
		{
			strcpy(message, "I am H2"); // the new msg
		}
		else
		{
			strcpy(message, "I am H3"); // the new msg
		}

		//Internal Sleep
		millisleep(processTimeIn);

		printf("H%d finishes updating ...\n", processNum);

		/* END OF CRITICAL SECTION */

		//Signal Semaphore Operations
		sem_post(&semaphore);
	}

	//Parent Process Completion
	parentCount--;

	//Termination #1
	printf("H%d is terminating ...\n", processNum);

	if (processNum == 1)
	{
		printf("The parent process now waits for all children ...\n");
	}
}

/*function "thread control functions -------------------------------- */
void* ThreadH1Control(void* arg)
{
	process_Hx(mutex1, 1, H1_TIME_IN, H1_TIME_OUT);
}

void* ThreadH2Control(void* arg)
{
	process_Hx(mutex2, 2, H2_TIME_IN, H2_TIME_OUT);
}

void* ThreadH3Control(void* arg)
{
	process_Hx(mutex3, 3, H3_TIME_IN, H3_TIME_OUT);
}

void* ThreadL1Control(void* arg)
{
	process_Lx(mutex1, 1, L1_TIME_IN, L1_TIME_OUT);
}

void* ThreadL2Control(void* arg)
{
	process_Lx(mutex2, 2, L2_TIME_IN, L2_TIME_OUT);
}

void* ThreadL3Control(void* arg)
{
	process_Lx(mutex3, 3, L3_TIME_IN, L3_TIME_OUT);
}