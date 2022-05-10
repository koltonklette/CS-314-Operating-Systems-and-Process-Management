/* *************************************************** *
 * Receiver.c:                                         *
 *                                                     *
 * compile: cc Receiver.c                              *
 *                                                     *
 * Kolton Klette                                       *
 * kolklet@siue.edu                                    *
 * Last Three Digits: "147"                            *
 * September 22, 2020                                  *
 *                                                     *
 * *************************************************** */
//Preliminary #include Statements from sender.c

	#include <stdio.h>         // for printf
	#include <stdlib.h>        // for sleep()
	#include <time.h>          // for srand
	#include <sys/types.h>     // for message queue
	#include <sys/ipc.h>       // for message queue
	#include <sys/msg.h>       // for message queue
	#include <unistd.h>        // for sleep()

//#define Statements/Constants

	#define BUFFER_SIZE 256  //max size of sendable message received from sender.c

//Functions
int main(void)
{
	//Variable Instantiation
		
		int msgID;								  //saves message queue ID for M2, M3, and M4 respectively
		int inputNumber;                          //saves the number called for from keyboard input
		int numCount;							  //used for tracking the numbers received from Mman.c

	//Message "struct" Definition
		
		struct message
		{
			long mtype;
			unsigned char mtext[BUFFER_SIZE];
		};
												  
	//Preliminary Set-Up
		
		struct message buffer;

    //Prompt Input From Keyboard
		
		printf("Enter your message queue key from a mm: ");
		scanf("%d", &inputNumber);
		printf("Message Queue Key Typed In: %d \n", inputNumber);

	//Input Check
		
		msgID = msgget(inputNumber, 0666 | IPC_CREAT);
		if (msgID <= -1)
		{
			printf("the receiver failed to connect to the message queue .... \n");
		}
		else
		{
			printf("the receiver is ready to receive messages from the middleman .... \n");
		}

	//Input Receive/Print
		
		//Preliminary Set-Up

			numCount = 0;
	
	    //Receiving From Message Queue
			while (msgrcv(msgID, (struct msgbuf*)&buffer, BUFFER_SIZE, 0, 0) > 0)
			{
				printf("received number: %d \n", buffer.mtext[0]);
				numCount++;
			}
			printf("receiver is completed ...\n");
			printf("received: %d numbers from the middleman ...\n", numCount);
};