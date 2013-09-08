#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include "log4c.h"
#include <log4c/appender_type_rollingfile.h>
#include <log4c/rollingpolicy_type_sizewin.h>



/*
 * rolling file specific params
*/
//char *param_log_dir = ROLLINGFILE_DEFAULT_LOG_DIR;
char *param_log_dir = "log";
char* param_log_prefix = ROLLINGFILE_DEFAULT_LOG_PREFIX"fredde";
long param_max_file_size = 1000*1024; /* 0 means 'no_limit' */
long param_max_num_files = 2;

char *param_layout_to_use = "dated"; /* could also be "dated" */

log4c_category_t* root = NULL;
log4c_appender_t* file_appender = NULL;
log4c_layout_t* basic_layout = NULL;


#define MAX 8
#define DMA_SIZE 20

/* These four chaps have to be global since both frame_grabber and
   cmd_handler needs them 
*/
typedef unsigned short frame[DMA_SIZE];
frame frames[MAX];
int wp = 0;
int rp = 0;


log4c_category_t* mycat = NULL;	


/* This part should be included in frame_grabber */
void *writer()
{

  int i;
  
  for (;;) {
   
   i= rand()%1;

   /* The writer has catched up to reader, the circular list is FULL! this could happen if the device driver 
      and the frame_grabber are to fast for the cmd_handler. */
   sleep(i);

   if (wp+1==rp || (wp+1==MAX && !rp)) {
    printf("writer have to wait, wp:%d rp:%d\n", wp, rp);
#if 0
   {

    log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
      "some error from writer at line %d in file %s - loop %d",
      __LINE__, __FILE__, i);
   }
#endif
   {
   log4c_category_fatal(root,
      "some error from writer at line %d in function %s - loop %d",
      __LINE__, __FUNCTION__, i
       );
   }

     continue;
    /* We should read from device driver anyway otherwise we will get into trouble
       should we have a /dev/null array for this occasion? */
       // read(device_driver, dummy_array, DMA_SIZE; return;  No inc of wp!
      // Should probably not lead to a fault code BUT a log of the event.
     // swap with return ERROR_FULL_FRAME_LIST;
    }

   /* Ready to read() stuff from the device driver */
   // read(device_driver, frames[wp], DMA_SIZE)

   memset(frames[wp],i,sizeof(frame));
   printf("Write pointer: %d\n",wp);

   /* Increment and check if we have to wrap */
   wp++;
   if (wp == MAX) wp =0;



  }
}


/* This part should be included in cmd_handler */
void *reader()
{

  int i;
  for (;;) {

   i= rand()%1;


   sleep(i);

   /* Time to wrap? */
   if (rp == MAX) rp =0;

   /* The circular list is empty, this should NEVER happen since
      frame_grabber sends a GET_FRAME_REPLY message for every frame */
   if(rp == wp) {
    printf("reader have to wait, wp:%d rp:%d\n", wp, rp);
   {
   log4c_category_fatal(root,
      "some error from reader at line %d in function %s - loop %d",
      __LINE__, __FUNCTION__, i
       );
   }
#if 0
   {

    log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
      "some error from reader at line %d in function %s - loop %d",
      __LINE__, __FILE__, i);
   }
#endif
    // The frame_grabber have signaled for a new frame but there's nothing here...
    // Set rp = wp = 0??  
    continue; 
    // swap with return ERROR_NO_FRAME_IN_LIST;
   }

   /* Send the reply to the requester */
   // send(socketId, frames[rp], DMA_SIZE)
   printf("Read pointer: %d\n",rp);
   rp++;

  }
}


/******************************************************************************/

/*
 * Init log4c and configure a rolling file appender
 *
*/
static void init_log4c_with_rollingfile_appender(){
  int rc = 2;
  rollingfile_udata_t *rfup = NULL;
  log4c_rollingpolicy_t *policyp = NULL;
  rollingpolicy_sizewin_udata_t *sizewin_confp = NULL;

  printf("using the rolling file appender with multiple logging threads"
          "to write test log files\n"
          "to log directory '%s', log file prefix is '%s'"
          ", max file size is '%ld'\n"
          "max num files is '%ld'\n",
          param_log_dir, param_log_prefix, param_max_file_size,
          param_max_num_files);
  
  if ( (rc = log4c_init()) == 0 ) {
    printf("log4c init success\n");
  } else {
    printf("log4c init failed--error %d\n", rc);
    return;
  }

  /*
   * Get a reference to the root category
  */
//  root = log4c_category_get("root");
  root = log4c_category_get("data acq");
  log4c_category_set_priority(root,
			      LOG4C_PRIORITY_WARN);

  /* 
   * Get a file appender and set the type to rollingfile
  */
  file_appender = log4c_appender_get("aname");
  log4c_appender_set_type(file_appender, 
    log4c_appender_type_get("rollingfile"));
  
  /*
   * Make a rolling file conf object and set the basic parameters 
  */
  rfup = rollingfile_make_udata();              
  rollingfile_udata_set_logdir(rfup, param_log_dir);
  rollingfile_udata_set_files_prefix(rfup, param_log_prefix);

  /*
   * Get a new rollingpolicy
   * type defaults to "sizewin" but set the type explicitly here
   * to show how to do it.
  */
  policyp = log4c_rollingpolicy_get("a_policy_name");
  log4c_rollingpolicy_set_type(policyp,
              log4c_rollingpolicy_type_get("sizewin"));

  /*
   * Get a new sizewin policy type and configure it.
   * Then attach it to the policy object.
  */
  sizewin_confp = sizewin_make_udata();
  sizewin_udata_set_file_maxsize(sizewin_confp, param_max_file_size);
  sizewin_udata_set_max_num_files(sizewin_confp, param_max_num_files);
  log4c_rollingpolicy_set_udata(policyp,sizewin_confp);

  /*
   * Configure our rolling policy, then use that to configure
   * our rolling file appender.
  */
  
  rollingfile_udata_set_policy(rfup, policyp);
  log4c_appender_set_udata(file_appender, rfup);
 
  /*
   * Open the rollingfile appender.  This will also trigger
   * the initialization of the rollingpolicy.
   * Log4c is not thread safe with respect to creation/initialization
   * of categories and appenders, so we must call the open
   * once, before all the threads kick in logging with this appender.
   * Calling open as a side effect of the first call to append does
   * not work in a MT environment.
   *
   * For info, could also call init directly on the rollingpolicy here:
   *   log4c_rollingpolicy_init(policyp, rfup);
   * 
   *
  */
  log4c_appender_open(file_appender);

  /*
   * Configure a layout for the rolling file appender 
   * (could also have chosen "basic" here, for example)
  */
  log4c_appender_set_layout(file_appender, 
                            log4c_layout_get(param_layout_to_use) );

  /*
   * Configure the root category with our rolling file appender...
   * and we can then start logging to it.
   *
  */
  log4c_category_set_appender(root,file_appender );
  
  log4c_dump_all_instances(stderr);


}

void getit(int sig) {

 exit(0);
}


main()
{
   int rc1, rc2;
   pthread_t thread1, thread2;
   int i;
   frame nisse;
 rollingpolicy_sizewin_udata_t *sizewin_confp = NULL;

  init_log4c_with_rollingfile_appender();	

#if 0
  if(log4c_init())
   printf("We failed to init log4c\n");
  else
   printf("We succeded to init log4c\n");

  mycat = log4c_category_get("six13log.log.app.application1");



  /*
   * Get a new sizewin policy type and configure it.
   * Then attach it to the policy object.
  */
 
 sizewin_confp = sizewin_make_udata();
 sizewin_udata_set_file_maxsize(sizewin_confp, param_max_file_size);
  sizewin_udata_set_max_num_files(sizewin_confp, param_max_num_files);
#endif

#if 0
LOG4C_API int sizewin_udata_set_file_maxsize  	(  	rollingpolicy_sizewin_udata_t *   	 swup,
		long  	max_size	 
	) 	

LOG4C_API int sizewin_udata_set_max_num_files  	(  	rollingpolicy_sizewin_udata_t *   	 swup,
		long  	max_num	 
	) 	

#endif

   {

    log4c_category_log(mycat, LOG4C_PRIORITY_ERROR,
      "some error from main at line %d in function %s - loop %d",
      __LINE__, __FUNCTION__, i);
   }


   for(i=0; i<MAX;i++) {
       memset(frames[i],0xabba,sizeof(frame));
   }

   memcpy(nisse, frames[0], sizeof(frame));


   signal(SIGINT, getit);

   /* Create independent threads each of which will execute functionC */


 
   if( (rc2=pthread_create( &thread2, NULL, &reader, NULL)) )
   {
      printf("Thread creation failed: %d\n", rc2);
   }

  if( (rc1=pthread_create( &thread1, NULL, &writer, NULL)) )
   {
      printf("Thread creation failed: %d\n", rc1);
   }


   /* Wait till threads are complete before main continues. Unless we  */
   /* wait we run the risk of executing an exit which will terminate   */
   /* the process and all threads before the threads have completed.   */

   pthread_join( thread1, NULL);
   pthread_join( thread2, NULL); 



   // FIXME: if we have a catch routine, use this ... 
  /* Explicitly call the log4c cleanup routine */
  if ( log4c_fini()){
    printf("log4c_fini() failed");
  }

   exit(0);
}
