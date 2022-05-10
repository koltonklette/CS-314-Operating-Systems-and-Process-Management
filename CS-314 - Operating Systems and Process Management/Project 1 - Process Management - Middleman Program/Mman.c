/* *************************************************** *
 * Mman.c:                                             *
 *                                                     *
 * compile: cc Mman.c                                  *
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

     #define NUM_REPEATS 100  //the number of the messages; provided per professor instruction
     #define M1_key 4210      //message key queue for M1
     #define M2_key 4211      //message key queue for M2
     #define M3_key 4212      //message key queue for M3
     #define M4_key 4213      //message key queue for M4
     #define BUFFER_SIZE 256  //max size of sendable message received from sender.c

//Functions
int main(void)
{

     //Variable Instatiation

          key_t msgKeyOne, msgKeyTwo, msgKeyThree, msgKeyFour; //saves corresponding message keys from define statements
          int msgIDOne, msgIDTwo, msgIDThree, msgIDFour;       //saves message queue ID for M2, M3, and M4 respectively
          int tempNum;                                         //used for handling checks 
          int status_Receive;                                  //used for returning value yielded from msgrcv(...) function call
          int status_Send;                                     //used for returning value yielded from msgsnd(...) function call
          int status_CTL;                                      //used for returning value yielded from msgctl(...) function call  
          int countTwo, countThree, countFour;                 //used to keep track of numbers sent by sender.c

     //Message "struct" Definition
     
          struct message
          {
             long mtype;
             unsigned char mtext[BUFFER_SIZE];
          };

     //Connecting to M1 Message Queue
          
          struct message buffer; //used in order to receive incoming information from M1
          msgKeyOne = M1_key;
          msgIDOne = msgget(msgKeyOne, 0666 | IPC_CREAT);

          //Error Check
          if (msgIDOne <= -1) 
          {
             printf("Connection to message queue (M1) was not established .... \n");
          }
          else
          {
             printf("Connection to message queue (M1) was established .... \n");
          }

     //Message Queue Creation - M2

          msgKeyTwo = M2_key;
          msgIDTwo = msgget(msgKeyTwo, 0666 | IPC_CREAT);

          //Error Check
          if (msgIDTwo <= -1)
          { 
             printf ("Your new message queue (M2) is not created .... \n");  
          }
          else
          { 
             printf("Your new message queue (M2) is successfully created .... \n"); 
          } 
     
     //Message Queue Creation - M3

          msgKeyThree = M3_key;
          msgIDThree = msgget(msgKeyThree, 0666 | IPC_CREAT);

          //Error Check
          if (msgIDThree <= -1)
          { 
             printf ("Your new message queue (M3) is not created .... \n");  
          }
          else
          { 
             printf("Your new message queue (M3) is successfully created .... \n"); 
          } 

     //Message Queue Creation - M4

          msgKeyFour = M4_key;
          msgIDFour = msgget(msgKeyFour, 0666 | IPC_CREAT);

          //Error Check
          if (msgIDFour <= -1)
          { 
             printf ("Your new message queue (M4) is not created .... \n");  
          }
          else
          { 
             printf("Your new message queue (M4) is successfully created .... \n"); 
          } 

     //Receiving Information From M1
          countTwo = 0;
          countThree = 0;
          countFour = 0;
          for (int i = 0; i < NUM_REPEATS; i++)
          {
              //Receive Random Number From M1 
              msgrcv(msgIDOne, (struct msgbuf *)&buffer, BUFFER_SIZE, 0, 0);

              //Received Random Number Confirmation Message
              printf("mm rcvd.: %d\n", buffer.mtext[0]);

              //Number Processing As Soon As Random Number is Received
              tempNum = buffer.mtext[0];

              //M2 Classification Receiver (if tempNum is 3x)
              if ((tempNum % 3) == 0)
              {
                  //Send Random Number to M2
                  status_Send = msgsnd(msgIDTwo, (struct msgbuf *)&buffer, BUFFER_SIZE, 0);

                  //Break Out Of Loop If Error Occurs
                  if (status_Send <= -1)
                  {
                      printf("sending a number over M2 failed ... \n\a");
                      break;
                  }
                  countTwo++;
              }
  
              //M3 Classification Receiver (if tempNum is (3x+1))
              else if ((tempNum % 3) == 1)
              {
                  //Send Random Number to M2
                  status_Send = msgsnd(msgIDThree, (struct msgbuf *)&buffer, BUFFER_SIZE, 0);

                  //Break Out Of Loop If Error Occurs
                  if (status_Send <= -1)
                  {
                      printf("sending a number over M3 failed ... \n\a");
                      break;
                  }
                  countThree++;
              }

              //M4 Classification Receiver (if tempNum is (3x+2))
              else if ((tempNum % 3) == 2)
              {
                  //Send Random Number to M2
                  status_Send = msgsnd(msgIDFour, (struct msgbuf *)&buffer, BUFFER_SIZE, 0);

                  //Break Out Of Loop If Error Occurs
                  if (status_Send <= -1)
                  {
                      printf("sending a number over M4 failed ... \n\a");
                      break;
                  }
                  countFour++;
              }
          }

     //Terminate Connection of All Message Queues When Finished

          //M2 Termination
          status_CTL = msgctl(msgIDTwo, IPC_RMID, NULL);
          if (status_CTL <= -1)
          {
              printf("Connection with message queue (M2) was not terminated successfully .... \n");
          }
          else
          {
              printf("Connection with message queue (M2) has been terminated successfully .... \n");
          }

          //M3 Termination
          status_CTL = msgctl(msgIDThree, IPC_RMID, NULL);
          if (status_CTL <= -1)
          {
              printf("Connection with message queue (M3) was not terminated successfully .... \n");
          }
          else
          {
              printf("Connection with message queue (M3) has been terminated successfully .... \n");
          }

          //M4 Termination
          status_CTL = msgctl(msgIDFour, IPC_RMID, NULL);
          if (status_CTL <= -1)
          {
              printf("Connection with message queue (M4) was not terminated successfully .... \n");
          }
          else
          {
              printf("Connection with message queue (M4) has been terminated successfully .... \n");
          }

     //Print Results
          printf("3x: %d, (3x+1): %d, (3x+2): %d \n", countTwo, countThree, countFour);
};

