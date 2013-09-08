
/*
	                     *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : Gemini                                                      **
** filename :  plx_ioctl.c                                                 **
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
Description : Interface to application, i.e system call ioctl().

TODO:


*/

#include <linux/sched.h>
#include <linux/delay.h>
#include <asm/uaccess.h>

#include "plx_drv.h"
#include "plx_driver.h"


#define IOC_BUFSIZE 128 /* some space for passing parameters .. */

#define DEVICE_DRIVER_VERSION  0x081216 // YYMMDDver

/*
**===========================================================================
** 8.0			pcidriver_ioctl()
**===========================================================================
** Description: 
**
** Parameters:  cmd and a argument
**              
**
** Returns:     some stuff ...
**
** Globals:     
*/
int plx_drv_ioctl(struct inode *inode, struct file *file,
		    unsigned int cmd, unsigned long arg)
{

driver_t  *dev = file->private_data;

  static unsigned char localbuf[IOC_BUFSIZE];
  /* Define "alias" names for the localbuf */
  void *karg = localbuf;
  Register *reg = karg;
  unsigned long *klong = karg;
  Container *container = karg;
 int size = _IOC_SIZE(cmd); /* the size bitfield in cmd */
  int retval = 0; /* success by default */

#if ZERO
PDEBUG(("function: %s, file: %s line: %d invoked\n", __FUNCTION__, __FILE__, __LINE__));
#endif

 /*
   * Extract the type and number bitfields, and don't decode
   * wrong cmds: return EINVAL before verify_area()
   */
  if (_IOC_TYPE(cmd) != MAG_NUM) return -ENOTTY;
  if (_IOC_NR(cmd) > IOC_MAXNR) return -ENOTTY;

  /*
   * The direction is a bitmask, and VERIFY_WRITE catches R/W
   * transfers. `Dir' is user-oriented, while
   * verify_area is kernel-oriented, so the concept of "read" and
   * "write" is reversed
   */
  if (_IOC_DIR(cmd) & _IOC_READ) {
    if (!access_ok(VERIFY_WRITE, (void *)arg, size))
      return -EFAULT;
  }
  else if (_IOC_DIR(cmd) & _IOC_WRITE) {
    if (!access_ok(VERIFY_READ, (void *)arg, size))
      return -EFAULT;
  }

#if ZERO
  PDEBUG(("ioctl.c: %08x %08lx: nr %i, size %i, dir %i\n",
	 cmd, arg, _IOC_NR(cmd), _IOC_SIZE(cmd), _IOC_DIR(cmd)));
#endif

  /* First, retrieve data from userspace */
  if ((_IOC_DIR(cmd) & _IOC_WRITE) && (size <= IOC_BUFSIZE))
    if (copy_from_user(karg, (void *)arg, size))
      return -EFAULT;

  /* We are ready to switch ... */
  switch (cmd) {

  case READ_PLX_REG:
   {
     if ((plx_read_plx_reg(dev, reg->addr, &(reg->data)) == 0)) { 
//       PDEBUG(("plx_read_fpga_reg: addr: %x and data %x\n", reg->addr, reg->data));
      }
      else
       return -EIO;
 
   }
   break;
  case WRITE_PLX_REG:
   {
     if((plx_write_plx_reg(dev, reg->addr, reg->data)) == 0) {
//         PDEBUG(("plx_write_fpga_reg: addr: %x and data %x\n", reg->addr, reg->data));
      }
      else
       return -EIO; 
 
   }

  break;


  case FILL_PCI_MEM:
    PDEBUG(("plx_ioctl: Fill PCI memory with test pattern!\n")); 

   memset_io(dev->PciBar[2].pVa, 0x11, DMA_SIZE/4);
   memset_io((dev->PciBar[2].pVa)+0x2000, 0x22, DMA_SIZE/4); 
   memset_io((dev->PciBar[2].pVa)+(0x2000*2), 0x33, DMA_SIZE/4); 
   memset_io((dev->PciBar[2].pVa)+(0x2000*3), 0x44, DMA_SIZE/4); 
  break;


 case READ_FPGA_REG:
      if ((plx_read_fpga_reg(dev, reg->addr, &(reg->data)) == 0)) { 
       PDEBUG(("plx_read_fpga_reg: addr: %x and data %x\n", reg->addr, reg->data));
      }
      else
       return -EIO; // Physical input/output error occured
   break;

case WRITE_FPGA_REG:
     if((plx_write_fpga_reg(dev, reg->addr, reg->data)) == 0) {
         PDEBUG(("plx_write_fpga_reg: addr: %x and data %x\n", reg->addr, reg->data));
      }
      else
       return -EIO; 

   break;

case WRITE_SRAM:
     if((plx_write_sram(dev, reg->addr, reg->data)) == 0) {
         PDEBUG(("plx_write_sram: addr: %x and data %x\n", reg->addr, reg->data));
      }
      else
       return -EIO; 

   break;


 case READ_COMMAND_LINK_REG:
    
      if ((plx_read_reg(dev, reg->addr, &(reg->data)) == 0)) { 
       PDEBUG(("plx_read_reg: addr: %x and data %x\n", reg->addr, reg->data));
      }
      else
       return -EIO; 
   
    break;

  case WRITE_COMMAND_LINK_REG:
    PDEBUG(("ioctl.c: addr: %x and data %x\n", reg->addr, reg->data));
  
      if((plx_write_reg(dev, reg->addr, reg->data)) == 0) {
         PDEBUG(("plx_write_reg: addr: %x and data %x\n", reg->addr, reg->data));
      }
      else
       return -EIO; 
    break;

    case IMMEDIATE_COMMAND:
      switch( *klong) {
 
      case XL_EXEC:
        /* Read the micro sec counter */
        plx_diff_since_read(dev);
      case XL_NOP:
      case XL_INIT:
      case XL_STOP:
        if((plx_immediate_cmd(dev, *klong) == 0)) {
       }
       else {
	retval = -EIO;
        }
       break;
    default :
      retval = -EFAULT;     
   }
   break;


  case ORDER_HW:

     switch (container->order) {
 
     case 0x1:  // Dump PCI memory
       { int i; unsigned int tmp;
       PDEBUG(("ioctl: order_hw, start_addr%x\n", container->start_addr));
       for (i = 0; i <16; i++) { 
         tmp = SRAM_READ(dev, (container->start_addr) +(i*4)); 
         container->array[i] = tmp; 

       } 

       }

       break; 
     case 0x2:   // Reset HW
       plx_board_reset(dev);
       break;
     case 0x3:  // Read and clear device driver counters
      {
       container->array[0] = dev->cntr_circ_empty_rp;
       container->array[1] = dev->cntr_irq_count;
       container->array[2] = dev->cntr_irq_processed;
       container->array[3] = dev->cntr_irq_none;
       container->array[4] = dev->cntr_low_power_state;
       container->array[5] = dev->cntr_pci_master_disabled;
       container->array[6] = dev->cntr_lost_ints;
       container->array[7] = dev->cntr_read;
       container->array[8] = dev->cntr_poll;
       container->array[9] = dev->cntr_failed_dma;
       container->array[10] = dev->cntr_circ_full_wp;
       container->array[11] = dev->cntr_circ_full_ip;
       container->array[12] = dev->cntr_2_user_space;
       container->array[13] = dev->cntr_tmo_rdr;
       container->array[14] = dev->cntr_tmo_tbe;
       container->array[15] = dev->cntr_lost_hw_ints;

       dev->cntr_circ_empty_rp = dev->cntr_irq_count = dev->cntr_irq_processed = dev->cntr_irq_none = 0;
       dev->cntr_low_power_state = dev->cntr_pci_master_disabled =  0;
       dev->cntr_read = dev->cntr_poll = dev->cntr_failed_dma = dev->cntr_2_user_space = 0;
       dev->cntr_tmo_rdr = dev->cntr_tmo_tbe = dev->cntr_circ_full_wp = dev->cntr_lost_ints = 0;
       dev->cntr_lost_hw_ints = dev->cntr_circ_full_ip = 0;
      }
      break;
     case 0x4:

     break;
     case 0x5:
#if ZERO
       PDEBUG(("order_hw: read time:%d\n", plx_diff_since_read(dev)));
#endif
     break;

#if DMA_DEBUG
     case 0x6:

     {
        int r;

        /* Write a test pattern at start address  in kernel mem */

       for(r = 0; r < NO_OF_BUFFERS ; r++) {
        *(unsigned long *)(dev->frames[r]) = 0xcdcdfbfb;

       }
     }
     break;

     case 0x7:

     {
        int r;

        /* Read test pattern at start address  in kernel mem */

       for(r = 0; r < NO_OF_BUFFERS ; r++) {
       PDEBUG(("r:%d:%lx\n", r, *(unsigned long *)(dev->frames[r])));

       }
     }
     break;

     case 0x8:
      {
          U32 RegValue;


   /* Abort a DMA, should generate an interrupt */
    RegValue =
	PLX_9000_REG_READ(
			  dev,
			  PCI8311_DMA_COMMAND_STAT
			  );

     // Abort the transfer (should cause an interrupt)
      PLX_9000_REG_WRITE(
			 dev,
			 PCI8311_DMA_COMMAND_STAT,
			 RegValue | ((1 << 2))
			 );

      }
     break;
#endif
     case 0x9:
        PDEBUG(("order_hw: ip%d, wp%d, rp%d\n", dev->ip , dev->wp, dev->rp));
        dev->wp = dev->rp = dev->ip = 0;
      break;

     /* Disable driver debug */
     case 0xa:
        debug = 0;
      break;

     /* Enable driver debug */
     case 0xb:
        debug = 1;
      break;

     /* Reset sequence counters in hw and driver */
     case 0xc:
        dev->seq_cntr = 0;
        plx_immediate_cmd(dev, XL_INIT);
      break;

     /* Generate a local interrupt */
     case 0xd:
      { u32 RegDetectorInt;

        RegDetectorInt = COMMAND_LINK_REG_READ(dev, PCI_MC_CARD_STATUS);
        RegDetectorInt |= (1<<29);
        COMMAND_LINK_REG_WRITE(dev,PCI_MC_CARD_STATUS,RegDetectorInt); 

      }  
      break;

     default:
       ERROR(("ioctl: no such order: %x\n", container->order));
       retval = -EFAULT;     
       break;
     }

  case DRIVER_VERSION:
   reg->data = DEVICE_DRIVER_VERSION;
   break;	

  case READ_CONFIG_REG:
  {
    PLX_PCI_REG_READ(dev, reg->addr,&reg->data);
   }
   break;

  default:
    ERROR(("ioctl: no such command: %x\n", cmd));
    return -ENOIOCTLCMD; 
}

  /* Finally, copy data to user space and return */
  if (retval < 0) return retval;
  if ((_IOC_DIR(cmd) & _IOC_READ) && (size <= IOC_BUFSIZE))
    if (copy_to_user((void *)arg, karg, size))
      return -EFAULT;
  return retval; /* sometimes, positive is what I want */
}
