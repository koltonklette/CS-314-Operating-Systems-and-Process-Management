/* *************************************************** *
 * project2_147.c:                                     *
 *                                                     *
 * compile: cc project2_147.c                          *
 *                                                     *
 * Kolton Klette                                       *
 * kolklet@siue.edu                                    *
 * Last Three Digits: "147"                            *
 * October 26th, 2020                                  *
 *                                                     *
 * *************************************************** */
 //Information From "required_labels.c" (as provided by professor)
#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <unistd.h>  

// the followings are for semaphores ----- 
#include <sys/sem.h>
#include <sys/ipc.h>

// the followings are for shared memory ----
#include <sys/shm.h>

//Other Includes
#include <string.h>

//Required Labels (as provided by the professor)
#define NUM_REPEATS           100      // number of loops for the high-priority processes
#define MAX_MSG_SIZE          512      // the max. msg size (this paramter will NOT be changed for testing)

#define H1_TIME_IN         300000      //   300 ms = 0.300 seconds (inside of the critical section)
#define H1_TIME_OUT       2000000      //  2000 ms = 2.000 seconds (outside of the critical section)

#define H2_TIME_IN         300000      //   300 ms = 0.300 seconds (inside of the critical section)
#define H2_TIME_OUT       2000000      //  2000 ms = 2.000 seconds (outside of the critical section)

#define H3_TIME_IN         300000      //   300 ms = 0.300 seconds (inside of the critical section)
#define H3_TIME_OUT       2000000      //  2000 ms = 2.000 seconds (outside of the critical section)

#define L1_TIME_IN        1000000      //  1000 ms = 1.000 seconds (inside of the critical section)
#define L1_TIME_OUT        200000      //   200 ms = 0.200 seconds (outside of the critical section)

#define L2_TIME_IN        1000000      //  1000 ms = 1.000 seconds (inside of the critical section)
#define L2_TIME_OUT        200000      //   200 ms = 0.200 seconds (outside of the critical section)

#define L3_TIME_IN        1000000      //  1000 ms = 1.000 seconds (inside of the critical section)
#define L3_TIME_OUT        200000      //   200 ms = 0.200 seconds (outside of the critical section)

//Semaphore & Shared Memory Keys (as assigned by professor)
	//Semaphore Keys
#define SEM_KEY1          8210         //NOTE: change amount of keys provided depending on how many semaphores are needed.
#define SEM_KEY2          8211
#define SEM_KEY3          8213
#define SEM_KEY4          8214

//Shared Memory Keys
#define SHM_KEY           7125         //NOTE: only one key for the shared memory should be necessary for program progression

//Global Variable Declaration

	//Semaphore Control Structure
	union semunion 
	{
		int    val;
		struct semid_ds* buf;
		ushort* arry;
	} argument;

	//Establish Shared Memory Space For Reference
	struct my_mem 
	{
		char shared_msg[MAX_MSG_SIZE];
		long int counter;
		int parent;
		int child;
	} memory;

	//Regular Variable Declaration
	int	   process_id;            //used in main method process control
	int    H1_WAIT_Value;         //used to allow H1 to properly wait on other processes

	int    sem_ID_1;              // Sempahore 1 ID  
	int    sem_ID_2;			  // Semaphore 2 ID
	int    sem_ID_3;			  // Semaphore 3 ID
	int    sem_ID_4;              // Semaphore 4 ID
	struct sembuf operations[1];  // define semaphore operations    

	int    shm_id;                // the shared memory ID 
	int    shm_size;              // the size of the shared memoy  
	struct my_mem* p_shm;         // pointer to the attached shared memory

//Function Declaration
void millisleep(unsigned ms);     // for random sleep time
void process_Lx(int key, int processNum, int processTimeIn, int processTimeOut); // for low-priority processes
void process_Hx(int key, int processNum, int processTimeIn, int processTimeOut); // for high-priority processes


int main(void)
{
	//Variable Declaration
	int tempReturn = 0; // value to control final parent process wait
	
	//Preliminary Code
	argument.val = 1;   // the initial value of the semaphore

	//Shared Memory Size Check
	//Added to ensure that shared memory is accessible by other processes
	shm_size = sizeof(MAX_MSG_SIZE);
	if (shm_size <= 0)
	{
		fprintf(stderr, "sizeof error in acquiring the shared memory size. Terminating ..\n");
		exit(0);
	}

	//Semaphore Creation
	sem_ID_1 = semget(SEM_KEY1, 1, 0666 | IPC_CREAT);
	if (sem_ID_1 < 0)
	{
		fprintf(stderr, "Failed to create a new semaphore (1). Terminating ..\n");
		exit(0);
	}
	else
	{
		printf("---Semaphore 1 has been created---\n");
	}

	sem_ID_2 = semget(SEM_KEY2, 1, 0666 | IPC_CREAT);
	if (sem_ID_2 < 0)
	{
		fprintf(stderr, "Failed to create a new semaphore (2). Terminating ..\n");
		exit(0);
	}
	else
	{
		printf("---Semaphore 2 has been created---\n");
	}

	sem_ID_3 = semget(SEM_KEY3, 1, 0666 | IPC_CREAT);
	if (sem_ID_1 < 0)
	{
		fprintf(stderr, "Failed to create a new semaphore (3). Terminating ..\n");
		exit(0);
	}
	else
	{
		printf("---Semaphore 3 has been created---\n");
	}

	sem_ID_4 = semget(SEM_KEY4, 1, 0666 | IPC_CREAT);
	if (sem_ID_4 < 0)
	{
		fprintf(stderr, "Failed to create a new semaphore (4). Terminating ..\n");
		exit(0);
	}
	else
	{
		printf("---Semaphore 4 has been created---\n");
	}
	
	//Semaphore Initialization
	if (semctl(sem_ID_1, 0, SETVAL, argument) < 0)
	{
		fprintf(stderr, "Failed to initialize the semaphore(1) by 1. Terminating ..\n");
		exit(0);
	}
	else
	{
		printf("---Semaphore 1 has been initialized---\n");
	}

	if (semctl(sem_ID_2, 0, SETVAL, argument) < 0)
	{
		fprintf(stderr, "Failed to initialize the semaphore(2) by 1. Terminating ..\n");
		exit(0);
	}
	else
	{
		printf("---Semaphore 2 has been initialized---\n");
	}

	if (semctl(sem_ID_3, 0, SETVAL, argument) < 0)
	{
		fprintf(stderr, "Failed to initialize the semaphore(3) by 1. Terminating ..\n");
		exit(0);
	}
	else
	{
		printf("---Semaphore 3 has been initialized---\n");
	}

	if (semctl(sem_ID_4, 0, SETVAL, argument) < 0)
	{
		fprintf(stderr, "Failed to initialize the semaphore(3) by 1. Terminating ..\n");
		exit(0);
	}
	else
	{
		printf("---Semaphore 4 has been initialized---\n");
	}

	//Shared Memory Creation
	shm_id = shmget(SHM_KEY, shm_size, 0666 | IPC_CREAT);
	if (shm_id < 0)
	{
		fprintf(stderr, "Failed to create the shared memory. Terminating ..\n");
		exit(0);
	}
	else
	{
		printf("---Shared Memory space successfully created---\n");
	}

	//Attaching Shared Memory to the Created Semaphore
	p_shm = (struct my_mem*)shmat(shm_id, NULL, 0);
	if (p_shm == (struct my_mem*) - 1)
	{
		fprintf(stderr, "Failed to attach the shared memory.  Terminating ..\n");
		exit(0);
	}
	else
	{
		printf("---Successfully attached Shared Memory to created Semaphores---\n");
	}

	//Shared Memory Initialization
	p_shm->counter = 0;
	p_shm->parent = 0;
	p_shm->child = 0;

	//Process Progression Block
	process_id = fork();
	if (process_id == 0)
	{
		process_Hx(sem_ID_2, 2, H2_TIME_IN, H2_TIME_OUT);
	}
	else
	{
		process_id = fork();
		if (process_id == 0)
		{
			process_Hx(sem_ID_3, 3, H3_TIME_IN, H3_TIME_OUT);
		}
		else
		{
			process_id = fork();
			if (process_id == 0)
			{
				process_Lx(sem_ID_1, 1, L1_TIME_IN, L1_TIME_OUT);
			}
			else
			{
				process_id = fork();
				if (process_id == 0)
				{
					process_Lx(sem_ID_2, 2, L2_TIME_IN, L2_TIME_OUT);
				}
				else
				{
					process_id = fork();
					if (process_id == 0)
					{
						process_Lx(sem_ID_3, 3, L3_TIME_IN, L3_TIME_OUT);
					}
					else
					{
						//Wait Process to have parent process
						millisleep(100000);

						operations[0].sem_num = 0;  // semaphore
						operations[0].sem_op = 1;   // "wait" for the semaphore
						operations[0].sem_flg = 0;  // blocks the mf semaphore
						H1_WAIT_Value = semop(sem_ID_4, operations, 1);

						printf("H1 is the last high priority process ....\n");
						process_Hx(sem_ID_4, 1, H1_TIME_IN, H1_TIME_OUT);

						//Program Termination Operations

						//H1 Parent Process Termination and Wait
						while (p_shm->child > 0)
						{
							millisleep(1000000); //wait for one second while other processes still exist
						}

						printf("The parent process is terminating ...\n");

						//Shared Memory Detach
						tempReturn = shmdt(p_shm);
						if (tempReturn != 0)
						{
							printf("Shared Memory Detach unsuccessful....\n");
						}
						tempReturn = shmctl(shm_id, IPC_RMID, 0);

						//Semaphore Removal Operations
						tempReturn = semctl(sem_ID_1, IPC_RMID, 0);
						if (tempReturn != 0)
						{
							printf("Semaphore 1 removal unsuccessful....\n");
						}

						tempReturn = semctl(sem_ID_2, IPC_RMID, 0);
						if (tempReturn != 0)
						{
							printf("Semaphore 2 removal unsuccessful....\n");
						}

						tempReturn = semctl(sem_ID_3, IPC_RMID, 0);
						if (tempReturn != 0)
						{
							printf("Semaphore 3 removal unsuccessful....\n");
						}

						tempReturn = semctl(sem_ID_4, IPC_RMID, 0);
						if (tempReturn != 0)
						{
							printf("Semaphore 4 removal unsuccessful....\n");
						}
					}
				}
			}
		}
	}

	//Program Completion
	return 0;
};

//Low-Priority Process (as provided by professor)
void process_Lx(int key, int processNum, int processTimeIn, int processTimeOut)
{
	int valueReturn = 0;
	int i = 0;

	//Intialize Child Process
	p_shm->child++;

	// Low-Priority Process Begins
	printf("L%d starts ....\n", processNum);
	
	//Main While Loop
	while(p_shm->parent > 0)
	{
		//Wait Semaphore Operations
		operations[0].sem_num = 0;  // semaphore
		operations[0].sem_op = -1; // "wait" for the semaphore
		operations[0].sem_flg = 0;  // blocks the mf semaphore
		valueReturn = semop(key, operations, 1);
		if (valueReturn != 0)
		{
			fprintf(stderr, "L%d (wait) failed (child) ....\a\n", processNum);
		}

		//Subtract 1 From Shared Memory
		p_shm->counter--;
		
		//External Sleep
		millisleep(processTimeOut);

		// START OF CRITICAL SECTION 
		
		printf("        L%d starts reading ...\n", processNum);

		//Reading/Showing Shared Message
		printf("                              L%d READ: %s\n", processNum, p_shm->shared_msg);

		//Internal Sleep
		millisleep(processTimeIn);

		//Signal Semaphore Operations
		operations[0].sem_num = 0;
		operations[0].sem_op = 1;
		operations[0].sem_flg = 0;
		valueReturn = semop(key, operations, 1);
		if (valueReturn != 0)
		{
			fprintf(stderr, "L%d (wait) failed (child) ....\a\n", processNum);
		}

		//Low-Prioirity Process Finishes
		printf("        L%d finishes reading ...\n", processNum);

		// END OF CRITICAL SECTION 
	}

	//Completion of Low-Priority Process
	p_shm->child--;

	//Process Termination
	printf("L%d is terminating ...\n", processNum);
};

//High-Priority Process (as provided by professor)
void process_Hx(int key, int processNum, int processTimeIn, int processTimeOut)
{
	int valueReturn = 0;
	int i = 0;
	int j = 0;

	//Initializing Parent Process
	p_shm->parent++;
	
	//High-Priority Process Starts
	printf("H%d starts ....\n", processNum);

	//Main For Loop
	for (i = 0; i < NUM_REPEATS; i++)
	{
		//External Sleep
		millisleep(processTimeOut);

		//Wait Semaphore Operations
		operations[0].sem_num = 0;  // semaphore
		operations[0].sem_op = -1; // "wait" for the semaphore
		operations[0].sem_flg = 0;  // blocks the mf semaphore

		printf("H%d would like to update ...\n", processNum);
		
		valueReturn = semop(key, operations, 1);
		if (valueReturn != 0)
		{
			fprintf(stderr, "H%d (wait) failed (child) ....\a\n", processNum);
		}

		/* START OF CRITICAL SECTION */

		//Add One to Counter
		p_shm->counter++;

		printf("H%d starts updating ...\n", processNum);


		//Updating Process
		for (j = 0; j < MAX_MSG_SIZE; j++)   // reset the msg
		{
			p_shm->shared_msg[j] = '\0';
		}

		//Update Control Block (based on process)
		if (processNum == 1)
		{
			strcpy(p_shm->shared_msg, "I am H1"); // the new msg
		}
		else if (processNum == 2)
		{
			strcpy(p_shm->shared_msg, "I am H2"); // the new msg
		}
		else
		{
			strcpy(p_shm->shared_msg, "I am H3"); // the new msg
		}

		//Internal Sleep
		millisleep(processTimeIn);

		printf("H%d finishes updating ...\n", processNum);
		
		/* END OF CRITICAL SECTION */

		//Signal Semaphore Operations
		operations[0].sem_num = 0;
		operations[0].sem_op = 1; // "signal" the semaphore
		operations[0].sem_flg = 0;
		valueReturn = semop(key, operations, 1);
		if (valueReturn != 0)
		{
			fprintf(stderr, "H%d (signal) failed ....\a\n", processNum);
		}
	}

	//Parent-Process Completion
	p_shm->parent--;

	//Termination #1
	printf("H%d is terminating ...\n", processNum);

	if (processNum == 1)
	{
		printf("The parent process now waits for all children ...\n");
	}
}

//Millisleep Function For Reference
void millisleep(unsigned micro_seconds)
{
	usleep(micro_seconds);
};