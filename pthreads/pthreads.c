#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

void *functionC();
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int  counter = 0;

#define MAX 8
#define DMA_SIZE 20
typedef unsigned short Array[DMA_SIZE];
Array arrays[MAX];

static char wp = 0;
static char rp = 0;

static char testwp = 0;
static char testrp = 0;



/* This part should be included in frame_grabber */
void *writer()
{

  int i;
  
  for (;;) {
   
   i= rand()%5;

   /* The writer has catched up to reader, this could happen if the device driver 
      frame_grabber are to fast for cmd_handler. */
   sleep(i);
   if (wp+1==rp || (wp+1==MAX && !rp)) {
    printf("writer have to wait, wp:%d rp:%d\n", wp, rp);
     continue;
    /* We should read from device driver anyway otherwise we will get into trouble
       should we have a /dev/null array for this occasion? */
       // read(device_driver, dummy_array, DMA_SIZE)
       // Set rp = wp = 0??  
     // swap with return ERROR_FULL_FRAME_LIST;
    }
   /* Ready to read() stuff from the device driver */
   // read(device_driver, arrays[wp], DMA_SIZE)

   memset(arrays[wp],i,sizeof(Array));
//   printf("Write: array[0]: %x\n", arrays[0][wp]);
//   printf("Write: array[19]: %x\n", arrays[19][wp]);
   printf("Write pointer: %d\n",wp);

   /* Increment and check if we have to wrap */
   wp++;
   if (wp == MAX) wp =0;

//   testwp++;
   testwp = (++testwp) % MAX;
   printf("Write pointer:%d, testwp:%d\n",wp, testwp);

  }
}

void *nenewriter()
{

  int i;
  
  for (;;) {
   
   i= rand()%10;

   sleep(i);
   if (wp+1==rp || (wp+1==MAX && !rp)) {
     continue;
    /* The writer has catched up to reader, this should not happen!! */
    }
   /*Ready to store stuff */
   memset(arrays[wp],i,sizeof(Array));
   printf("Write: array[0]: %x\n", arrays[0][wp]);
   printf("Write pointer: %d\n",wp);
   wp++;
   if (wp == MAX) wp =0;


  }
}


void *ollywriter()
{
  int i;


//  for (i=0; i<100; i++) {
  for (;;) {

   i= rand()%10;
   sleep(i);
   if (wp+1==rp || (wp+1==MAX && !rp)) {
//     printf("writer have to wait, wp:%d, rp:%d\n",wp, rp);
     continue;
    }

   /*Ready to store stuff */
   memset(arrays[wp],i,sizeof(Array));

   printf("Write pointer: %d\n",wp);
   wp++;
   if (wp == MAX) wp =0;


  }
}


void *oldwriter()
{
  int i;


  for (i=0; i<100; i++) {
   memset(arrays[wp],0xabba,sizeof(arrays[0]));
//   pthread_mutex_lock( &mutex1 );
   // FIXME: check that wp never reaches rp ...
   if (wp+1==rp || (wp+1==MAX && !rp))
     {printf("writer have to wait, wp:%d, rp:%d\n",wp, rp);}
   else {
   wp++;
//   wp =wp%MAX;
   if (wp == MAX) wp =0;
   }
   printf("Write pointer: %d\n",wp);
//   pthread_mutex_unlock( &mutex1 );

  }
}



/* This part should be included in cmd_handler */
void *reader()
{

  int i;
  for (;;) {

   i= rand()%5;


   sleep(i);

   /* Time to wrap? */
   testrp = (testrp) % MAX;
   if (rp == MAX) rp =0;

   /* The circular list is empty, this should NEVER happen since
      frame_grabber sends a GET_FRAME_REPLY message for every frame */
   if(rp == wp) {
    printf("reader have to wait, wp:%d rp:%d\n", wp, rp);
    // The frame_grabber have signaled for a new frame but there's nothing here...
    // Set rp = wp = 0??  
    continue; 
    // swap with return ERROR_NO_FRAME_IN_LIST;
   }

   /* Send the reply to the requester */
   // send(socketId, arrays[rp], DMA_SIZE)

   printf("Read pointer: %d\n",rp);
//   printf("Read: array[0]: %x\n", arrays[0][rp]);
   rp++;

  }
}

void *nenereader()
{

  int i;
//  for (i=0; i<100; i++) {
  for (;;) {

   // FIXME: dont read if empty ...
   if (rp == MAX) rp =0;
   if(rp == wp) {
//    printf("reader have to wait, wp:%d rp:%d\n", wp, rp);
    continue;
   /* This should never happen since frame_grabber always sends a GET_FRAME
      before an array is ready ...
   */
   }
   printf("Read pointer: %d\n",rp);
   printf("Read: array[0]: %x\n", arrays[0][rp]);
//   printf("Read: array[0]: %x\n", arrays[rp]);
   rp++;

  }
}

void *oldreader()
{

  int i;
  for (i=0; i<100; i++) {

//   pthread_mutex_lock( &mutex1 );
   // FIXME: dont read if empty ...
   if (rp == MAX) rp =0;
   if(rp == wp) {
    printf("reader have to wait, wp:%d rp:%d\n", wp, rp);
    continue;
   }
   rp++;
//   pthread_mutex_unlock( &mutex1 );

  }
}


main()
{
   int rc1, rc2;
   pthread_t thread1, thread2;
   int i;
   Array nisse;


   for(i=0; i<MAX;i++) {
       memset(arrays[i],0xabba,sizeof(Array));
   }

   memcpy(nisse, arrays[0], sizeof(Array));


   /* Create independent threads each of which will execute functionC */

   if( (rc1=pthread_create( &thread1, NULL, &writer, NULL)) )
   {
      printf("Thread creation failed: %d\n", rc1);
   }

   if( (rc2=pthread_create( &thread2, NULL, &reader, NULL)) )
   {
      printf("Thread creation failed: %d\n", rc2);
   }

   /* Wait till threads are complete before main continues. Unless we  */
   /* wait we run the risk of executing an exit which will terminate   */
   /* the process and all threads before the threads have completed.   */

   pthread_join( thread1, NULL);
   pthread_join( thread2, NULL); 

   exit(0);
}

void *functionC()
{
   pthread_mutex_lock( &mutex1 );
   counter++;
   printf("Counter value: %d\n",counter);
   pthread_mutex_unlock( &mutex1 );
}
