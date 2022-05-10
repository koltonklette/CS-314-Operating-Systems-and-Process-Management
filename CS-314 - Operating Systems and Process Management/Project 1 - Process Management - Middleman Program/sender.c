/* *************************************************** *
 * sender.c:                                           *
 *                                                     *
 * compile: cc sender.c                                *
 *                                                     *
 * September 15, 2020                                  *
 *                                                     *
 * *************************************************** */

#include <stdio.h>         // for printf
#include <stdlib.h>        // for sleep()
#include <time.h>          // for srand
#include <sys/types.h>     // for message queue
#include <sys/ipc.h>       // for message queue
#include <sys/msg.h>       // for message queue

#include <unistd.h>        // for sleep()

#define NUM_REPEATS   100  // the number of the messages

#define MSG_key      4210  // message queue key for M1

#define BUFFER_SIZE   256  // max. message size

int main (void)
{
   int  i;                 // loop counter

   int  status_code;       // result status

   int  msqid;             // message queue ID (M1)
   key_t msgkey;           // message-queue key (M1)

   unsigned int       rand_num;  // a 32-bit random number
   float             temp_rand;  // the raw random numbber
   unsigned char eight_bit_num;  // an 8-bit random number

   int zero_counter = 0;   // counter for 3x numbers
   int one_counter  = 0;   // counter for (3x+1) numbers
   int two_counter  = 0;   // counter for (3x+2) numbers

   /* Use current time to shuffle random seed */
   srand(time(0)); 

   /* definition of message -------------------- */
   struct message{
        long mtype;
        unsigned char mtext[BUFFER_SIZE];
   };

   /* instantiate the message buffer ------------*/
   struct message buffer;

   /* set up the message queue key --------------*/
   msgkey = MSG_key; 

   /* create a new message queue --------------- */
   msqid = msgget(msgkey, 0666 | IPC_CREAT);

   /* error check ------------------------------ */
   if (msqid <= -1)
   { printf ("your new message queue (M1) is not created ....\n");  }
   else
   { printf("your new message queue (M1) is successfully created ....\n"); }

   /* confirm the start of the two receivers --- */
   printf("start your Middleman then start the three receivers ... press any key when ready ....\n\n");

   /* wait for a key stroke at the keyboard ---- */
   getchar();

   /* take care of "mtype" --------------------- */
   buffer.mtype = 1;  // UNIX standard says, any number

   /* send one eigh-bit number, one at a time  ------------ */
   for (i = 0; i < NUM_REPEATS; i++)
   {
      /* generate an 8-bit random number (0-255) ---------- */
      temp_rand = ((float)rand()/(float)RAND_MAX)*255.0;   
      rand_num = (int)temp_rand;
      eight_bit_num = (unsigned char)rand_num; 

      /* if 3x -------------------------------------------- */
      if ((eight_bit_num % 3) == 0)  
      { 
         printf("3x:    %d\n", eight_bit_num);
         zero_counter ++;
      }
  
      /* if (3x+1) ---------------------------------------- */
      else if ((eight_bit_num % 3) == 1)
      {
         printf("3x+1:         %d\n", eight_bit_num);
         one_counter ++;
      }

      /* if (3x+2) ---------------------------------------- */
      else
      {
         printf("3x+2:                   %d\n", eight_bit_num);
         two_counter ++;
      }
     
      /* se the new eight-bit number to the message buffers ------ */  
      buffer.mtext[0] = eight_bit_num;   // copy the 8-bit number     
      buffer.mtext[1] = '\0';            // null-terminate it
      
      /* send a 8-bit number to the two message queues ---- */
      status_code = msgsnd(msqid, (struct msgbuf *)&buffer, sizeof(buffer.mtext), 0);

      /* detect a message transmission error -------------- */
      if (status_code <= -1)
      {
         printf("sending a number over M1 failed ... \n\a"); 
         
         break;
      }
   }

   /* give everyone else enough time to complete their tasks --- */
   sleep(1);

   /* delete the emsage queue ----------------------------- */
   status_code = msgctl(msqid, IPC_RMID, NULL);

   /* error check ----------------------------------------- */
   if (status_code <= -1)
   { printf("deleting M1 failed ...\n\a");  } 

   else
   { printf("M1 message queue has been successfully deleted ...\n"); }

   /* statistics ------------------------------------------ */
   printf("3x: %d, (3x+1): %d, (3x+2): %d.\n", zero_counter, one_counter, two_counter);
}

/* END OF LINES ============================================================= */
