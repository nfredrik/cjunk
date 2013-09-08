/*
 * File:         timer_test.c
 * Author:       Mike Frysinger
 * Description:  example code for playing with kernel timers
 *
 * Rev:          $Id: timer_test.c 3342 2006-08-09 20:32:43Z vapier $
 *               Copyright 2007 Analog Devices Inc.
 *
 * Bugs:         Enter bugs at http://blackfin.uclinux.org/
 *
 * Licensed under the GPL-2 or later.
 * http://www.gnu.org/licenses/gpl.txt
 */
 
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/types.h>
#include <linux/completion.h> 
#define PRINTK(x...) printk(KERN_DEBUG "timer_test: " x)
 
static struct timer_list timer_test_1;

struct completion timer_done;
static int still_going = 1;

static int frame_count = 0;

static unsigned long frames;

 
static ulong delay = 5;
module_param(delay, ulong, 0);
MODULE_PARM_DESC(delay, "number of seconds to delay before firing; default = 5 seconds");

 
static void timer_test_func(unsigned long data)
{
  int ret;

  // Howto have a one time shooter? a static variable that takes the first value?
  frames = data;

	PRINTK("timer_test_func: here i am with my data '%li'!\n", data);

        if (still_going == 0) {
	  complete(&timer_done);
          return;
        }

	if (frames) {
	  ret = mod_timer(&timer_test_1, jiffies + msecs_to_jiffies(delay * 1000));
	  if (ret)
		PRINTK("mod_timer() returned %i!  that's not good!\n", ret);
          frames--;
        }


}

void xl_exec(void) {

  int ret;

       
  if(frame_count == 0 ) { 
        // Read frame count register, pass to timer_test_func()
        frame_count = 1234;
	PRINTK("arming timer 1 to fire %lu seconds from now\n", delay);
	setup_timer(&timer_test_1, timer_test_func, frame_count);

	ret = mod_timer(&timer_test_1, jiffies + msecs_to_jiffies(delay * 1000));
	if (ret)
		PRINTK("mod_timer() returned %i!  that's not good!\n", ret);
	else {
	  PRINTK("Error:frame_count still:%d!!\n", frame_count);
	  
	}

  }
}


void xl_stop(void) {

   still_going = 0;
   mod_timer(&timer_test_1, jiffies + 1);
   wait_for_completion(&timer_done);
   frame_count = 0;

  if (del_timer(&timer_test_1))
		PRINTK("timer 1 is still in use!\n");
   

}

 
static int __init timer_test_init(void)
{

	PRINTK("timer module init\n");

       // add this somewhere else before using the completion thing
        init_completion(&timer_done);

 
	return 0;
}
 
static void __exit timer_test_cleanup(void)
{
	PRINTK("timer module cleanup\n");
	if (del_timer(&timer_test_1))
		PRINTK("timer 1 is still in use!\n");
}
 
module_init(timer_test_init);
module_exit(timer_test_cleanup);
 
MODULE_DESCRIPTION("example kernel timer driver");
MODULE_LICENSE("GPL");
