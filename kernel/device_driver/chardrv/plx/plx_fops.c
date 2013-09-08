/*
	                     *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : Gemini                                                      **
** filename :  plx_fops.c                                                  **
** version   : 1                                                           **
** date      : July 18, 2008                                               **
**                                                                         **
*****************************************************************************
**                                                                         **
** Copyright (c) 2008, C-RAD Imaging                                       **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 1
Date        : July 29, 2008
Revised by  : Fredrik Svard
Description : Original version.

Version     : 2
Date        : July 29, 2008
Revised by  : Fredrik Svard
Description : Interface to application, i.e system calls read(), open(), close()

TODO:  
       


*/



#include <linux/kernel.h>       /* container_of() */
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <asm/uaccess.h>	/* For copy_to_user/put_user/... */


#include "plx_driver.h"




/*
**===========================================================================
** 8.0			plx_drv_open()
**===========================================================================
** Description: Save structure in private_data to be able to reach it from
**              read() and ioctl()
**
**              We will probably setup a number of DMA descriptors, i.e. 4.
**              The hardware will complete DMA for all 4 before generating
**              a interrupt.
**
**
** Parameters:  
**              
**
** Returns:     none
**
** Globals:     
*/
static int plx_drv_open(struct inode *inode, struct file *file)
{


  driver_t *dev = container_of(inode->i_cdev, driver_t, cdev); /* device information */

  spin_lock(&(dev->Lock_DeviceOpen));

  /* Save our driver structure in the file's private structure */
  file->private_data = dev;    

   PDEBUG(("function: %s, file: %s line: %d invoked, dev %lx\n", __FUNCTION__, __FILE__, __LINE__, (unsigned long)dev));

   /* Not defined ? */	
   if (!dev) 
  {
        spin_unlock(&(dev->Lock_DeviceOpen));
        return -ENODEV; 
  }

  /* If already called don't do the inialization again ... */

  dev->OpenCount++;

  if(dev->OpenCount > 1) {
     PDEBUG(("function:%s, file:%s, everything already installed: %d\n", __FUNCTION__, __FILE__, dev->OpenCount));
     spin_unlock(&(dev->Lock_DeviceOpen));
     return 0;
  }




  /* Setup a DMA channel 0 */
  if (plx_dma_open_channel ( dev, 0 ) != ApiSuccess) {
   spin_unlock(&(dev->Lock_DeviceOpen));
   return -ENODEV;
  }

#if DMA1
 /* Setup a DMA channel 1 */
  if (plx_dma_open_channel ( dev, 1 ) != ApiSuccess) {
   spin_unlock(&(dev->Lock_DeviceOpen));
   return -ENODEV;
  }
#endif



  /* Install interrupt handler */
  if(plx_install_interrupt_handler(dev)!= ApiSuccess) {
   spin_unlock(&(dev->Lock_DeviceOpen));
   return -ENODEV; 
  }

 spin_unlock(&(dev->Lock_DeviceOpen));

 return 0;
}



/*
**===========================================================================
** 8.0			plx_drv_read()
**===========================================================================
** Description: 
**
** Parameters:  
**              
**
** Returns:     none
**
** Globals:     

*/


static ssize_t plx_drv_read(struct file *file, char __user *data,
			      size_t len, loff_t *ppos)
{


 int err;
 ssize_t retval = 0;

driver_t *dev = file->private_data;



 PDEBUG(("function: %s, file: %s line: %d invoked, dev %lx\n", __FUNCTION__, __FILE__, __LINE__, (unsigned long)dev)); 


   if(down_interruptible(&dev->read_semph))
	return -ERESTARTSYS;

   dev->cntr_read++;
/* Wait for interrupt handler to signal that it's OK to read */

  while (dev->rp == dev->wp) { /* nothing to read */

    PDEBUG(("function: %s, line: %d, wp and rp eq:%d\n", __FUNCTION__, __LINE__, dev->wp)); 
    up(&dev->read_semph); /* release the lock */
    if (file->f_flags & O_NONBLOCK)
      return -EAGAIN;
    PDEBUG(("\"%s\" reading: going to sleep\n", current->comm));
    if (wait_event_interruptible(dev->read_queue, (dev->rp != dev->wp)))
      return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
    /* otherwise loop, but first reacquire the lock */
    if (down_interruptible(&dev->read_semph))
     return -ERESTARTSYS;

  }


/* Copy to user space */

PDEBUG(("function: %s, line: %d, data: %x, frame addr: %lx , len: %lx, rp%d\n", __FUNCTION__, __LINE__, (unsigned int)data, (unsigned long)dev->frames[dev->rp],(unsigned long)len, dev->rp)); 
  err = copy_to_user(data, (void *) dev->frames[dev->rp],len);
    if (err) {
      ERROR(("function: %s, line: %d, couldn't copy to user\n", __FUNCTION__, __LINE__)); 
      retval = -EFAULT;
      goto out;
    }

   dev->cntr_2_user_space++;

  /* This should never occur! */
    if (dev->rp == dev->ip) {
     ERROR(("function: %s, line: %d, ERROR rp reached ip :%d\n", __FUNCTION__, __LINE__, dev->rp)); 
     dev->cntr_circ_empty_rp++;
     return -EFAULT;
    }

    /* Increment read pointer and check for wrap */
    dev->rp++;
        if (dev->rp == NO_OF_BUFFERS)
                dev->rp = 0;

    PDEBUG(("function: %s, after copy_to_user read time:%d\n", __FUNCTION__, plx_diff_since_read(dev))); 
 
 
    /* Nonnegative value represents successfully read bytes */
    retval = len;




out:

/* Unlock .. */
up(&dev->read_semph);


 return retval;
}

#if ZERO
/*
**===========================================================================
** 8.0			plx_drv_write()
**===========================================================================
** Description: This function will only be used for testing, not supported
**              in the product.
**
**
** Parameters:  
**              
**
** Returns:     none
**
** Globals:     
*/
static ssize_t plx_drv_write(struct file *file, const char *buf,
		size_t count, loff_t *ppos) {

int err;

 driver_t *dev = file->private_data;


 PDEBUG(("function: %s, file: %s line: %d invoked, dev %lx\n", __FUNCTION__, __FILE__, __LINE__, (unsigned long)dev));


  /* We will use first buffer all the time */
  err = copy_from_user(dev->frames[0],buf,count);   /* We're copying u16 ! */
  if (err != 0)
   return -EFAULT;


  

return count;

}
#endif
/*
**===========================================================================
** 8.0			plx_drv_release()
**===========================================================================
** Description: 
**
** Parameters: 
**              
**
** Returns:     none
**
** Globals:     
*/
static int plx_drv_release(struct inode *inode, struct file *file)
{

driver_t *dev = file->private_data;

 PDEBUG(("function: %s, file: %s line: %d invoked, dev %lx\n", __FUNCTION__, __FILE__, __LINE__, (unsigned long)dev));


 spin_lock(&(dev->Lock_DeviceOpen));

   if (dev->OpenCount == 0) {
       PDEBUG(("function:%s, file:%s, major problem OpenCount already zero! \n", __FUNCTION__, __FILE__));
       spin_unlock(&(dev->Lock_DeviceOpen));
       return 0;
   } 
   dev->OpenCount--;

   if (dev->OpenCount > 0) {
      PDEBUG(("function:%s, file:%s, Counting down now: %d\n", __FUNCTION__, __FILE__, dev->OpenCount));
      spin_unlock(&(dev->Lock_DeviceOpen));
      return 0;
   }

  PDEBUG(("function:%s, file:%s, this was the last user uninstall isr and close dma channel(s) \n", __FUNCTION__, __FILE__));

  /* Remove association with interrupt handler */
  plx_uninstall_interrupt_handler(dev);

  /* Close DMA channel 0 */
  plx_dma_close_channel (dev, 0, false );

#if DMA1
 /* Close DMA channel 1 */
  plx_dma_close_channel (dev, 1, false );
#endif

 spin_unlock(&(dev->Lock_DeviceOpen));

 return 0;
}

/*
**===========================================================================
** 8.0			plx_drv_poll()
**===========================================================================
** Description: Back end of all system calls: poll, select and epoll
**
** Parameters:  
**              
**
** Returns:     
**
** Globals:     
*/

static unsigned int plx_drv_poll(
                                  struct file * filp, 
                                  struct poll_table_struct * wait) {
// U32 curr;
 unsigned int mask = 0;



driver_t  *dev = filp->private_data;

	/*
	 * The buffer is circular; it is considered full
	 * if "wp" or "ip" are right behind "rp" and empty if the
	 * two are equal.
	 */


#if ZERO
       down(&dev->read_semph);
#endif
   if(down_interruptible(&dev->read_semph)) {
	return -ERESTARTSYS;
   }

    
        /* Adds a wait queue, read_queue, to poll_table structure */
	poll_wait(filp, &dev->read_queue,  wait);

	if (dev->rp != dev->wp)

        {
            mask |= POLLIN | POLLRDNORM;	/* readable */
            dev->cntr_poll++;

        }

	up(&dev->read_semph);
	return mask;


  return 0;

}


/*
 *	Kernel Interfaces
 */
struct file_operations pci_drv_fops = {
	.owner =	THIS_MODULE,
#if ZERO
	.llseek =	no_llseek,
#endif
	.read =	        plx_drv_read,
#if ZERO
	.write =	plx_drv_write,
#endif
	.poll =	        plx_drv_poll,
	.ioctl =	plx_drv_ioctl,
	.open =		plx_drv_open,
	.release =	plx_drv_release,
};
