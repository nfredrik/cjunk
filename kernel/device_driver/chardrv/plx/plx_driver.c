/*
	                     *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : Gemini                                                      **
** filename :  plx_driver.c                                                **
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
Description : Support functions.

Version     : 3
Date        : Aug 21, 2008
Revised by  : Fredrik Svard
Description : Reset of hardware in probe()

TODO:             - DMA hangs if a driver is unloaded/loaded, check how things a released!

*/
#include <linux/proc_fs.h>   /* For debuggning purpose */
#include <linux/fs.h>        /* alloc_chrdev_region(), unregister_chrdev_region() */
#include <linux/cdev.h>      /* cdev_init() cdev_add() */


//#include <linux/delay.h>

#include "plx_driver.h"



#include "plx_drv.h"
#include "plx_interrupt.h"


extern struct file_operations pci_drv_fops;


static int plx_drv_major =   PLX_DRV_MAJOR;
static int plx_drv_minor =   0;
int debug = 1;

#define PCI_DRIVER_MAJOR  0

module_param(plx_drv_major, int, S_IRUGO);
module_param(plx_drv_minor, int, S_IRUGO);
module_param(debug, int, S_IRUGO);

/* This is our PCI device */
#define PCI_VENDOR_ID_PLX 0x10b5
#define PCI_DEVICE_ID_PLX 0x86e1

#define TIMEOUT_HW_REG_ACCESS 2000000000

static int before = 0;



/* __devinitdata forces the kernel to release the datastructure after the device
   initialization
*/


static  struct pci_device_id  __devinitdata pci_device_id[] =
  {
    { PCI_DEVICE ( PCI_VENDOR_ID_PLX, PCI_DEVICE_ID_PLX ), },
    { 0, }
  };

MODULE_DEVICE_TABLE ( pci, pci_device_id );

/* Allocated and initiated in probe() */
driver_t *PL_devp;    



/*
**===========================================================================
** 8.0			plx_chip_dma_control()
**===========================================================================
** Description:  Orders the DMA engine what to do ...
**
** Parameters:
**
**
** Returns:     none
**
** Globals:
*/

PLX_STATUS
plx_chip_dma_control(
		   driver_t *pdx,
		   U8                channel,
		   PLX_DMA_COMMAND   command
		   )
{
  U8  i;
  U8  shift;
  U32 RegValue;


  // Verify valid DMA channel
  switch (channel)
    {
    case 0:
    case 1:
      i     = channel;
      shift = (channel * 8);
      break;

    default:
      ERROR(("ERROR - Invalid DMA channel\n"));
      return ApiDmaChannelInvalid;
    }

  switch (command)
    {
    case DmaPause:
      // Pause the DMA Channel
      RegValue =
	PLX_9000_REG_READ(
			  pdx,
			  PCI8311_DMA_COMMAND_STAT
			  );

      PLX_9000_REG_WRITE(
			 pdx,
			 PCI8311_DMA_COMMAND_STAT,
			 RegValue & ~((1 << 0) << shift)
			 );

      // Check if the transfer has completed
      RegValue =
	PLX_9000_REG_READ(
			  pdx,
			  PCI8311_DMA_COMMAND_STAT
			  );

      if (RegValue & ((1 << 4) << shift))
	return ApiDmaDone;
      break;

    case DmaResume:
      // Verify that the DMA Channel is paused
      RegValue =
	PLX_9000_REG_READ(
			  pdx,
			  PCI8311_DMA_COMMAND_STAT
			  );

      if ((RegValue & (((1 << 4) | (1 << 0)) << shift)) == 0)
	{
	  PLX_9000_REG_WRITE(
			     pdx,
			     PCI8311_DMA_COMMAND_STAT,
			     RegValue | ((1 << 0) << shift)
			     );
	}
      else
	{
	  return ApiDmaInProgress;
	}
      break;

    case DmaAbort:
      // Pause the DMA Channel
      RegValue =
	PLX_9000_REG_READ(
			  pdx,
			  PCI8311_DMA_COMMAND_STAT
			  );

      PLX_9000_REG_WRITE(
			 pdx,
			 PCI8311_DMA_COMMAND_STAT,
			 RegValue & ~((1 << 0) << shift)  & ~((1 << 1) << shift) & ~((1 << 2) << shift)
			 );

      // Check if the transfer has completed
      RegValue =
	PLX_9000_REG_READ(
			  pdx,
			  PCI8311_DMA_COMMAND_STAT
			  );

      if (RegValue & ((1 << 4) << shift))
	return ApiDmaDone;

      // Abort the transfer (should cause an interrupt)
      PLX_9000_REG_WRITE(
			 pdx,
			 PCI8311_DMA_COMMAND_STAT,
			 RegValue | ((1 << 2) << shift)
			 );
      break;

    default:
      return ApiDmaCommandInvalid;
    }

  return ApiSuccess;
}


/******************************************************************************
 *
 * Function   :  plx_board_reset
 *
 * Description:  Resets the device using software reset feature of PLX chip
 *
 ******************************************************************************/
PLX_STATUS plx_board_reset(driver_t *pdx)
{
    U8  MU_Enabled;
    U8  EepromPresent;
    U32 RegValue;
    U32 RegInterrupt;
    U32 RegHotSwap;
    U32 RegPowerMgmnt;


    /* Clear any PCI errors */
    PLX_PCI_REG_READ(
        pdx,
        CFG_COMMAND,
        &RegValue
        );

    if (RegValue & (0xf8 << 24))
    {
        /* Write value back to clear aborts */
        PLX_PCI_REG_WRITE(
            pdx,
            CFG_COMMAND,
            RegValue
            );
    }

    /* Save state of I2O Decode Enable */
    RegValue =
        PLX_9000_REG_READ(
            pdx,
            PCI8311_FIFO_CTRL_STAT
            );

    MU_Enabled = (U8)(RegValue & (1 << 0));

    /* Determine if an EEPROM is present */
    RegValue =
        PLX_9000_REG_READ(
            pdx,
            PCI8311_EEPROM_CTRL_STAT
            );

    /* Make sure S/W Reset & EEPROM reload bits are clear */
    RegValue &= ~((1 << 30) | (1 << 29));

    /* Remember if EEPROM is present */
    EepromPresent = (U8)((RegValue >> 28) & (1 << 0));

    /* Save interrupt line */
    PLX_PCI_REG_READ(
        pdx,
        CFG_INT_LINE,
        &RegInterrupt
        );

    /* Save some registers if EEPROM present */
    if (EepromPresent)
    {
        PLX_PCI_REG_READ(
            pdx,
            PCI8311_HS_CAP_ID,
            &RegHotSwap
            );

        PLX_PCI_REG_READ(
            pdx,
            PCI8311_PM_CSR,
            &RegPowerMgmnt
            );
    }

    /* Issue Software Reset to hold PLX chip in reset */
    PLX_9000_REG_WRITE(
        pdx,
        PCI8311_EEPROM_CTRL_STAT,
        RegValue | (1 << 30)
        );

    /* Delay for a bit */
    mdelay(100);

    /* Bring chip out of reset */
    PLX_9000_REG_WRITE(
        pdx,
        PCI8311_EEPROM_CTRL_STAT,
        RegValue
        );

    /* Issue EEPROM reload in case now programmed */
    PLX_9000_REG_WRITE(
        pdx,
        PCI8311_EEPROM_CTRL_STAT,
        RegValue | (1 << 29)
        );

    /* Delay for a bit */
    mdelay(10);

    /* Clear EEPROM reload */
    PLX_9000_REG_WRITE(
        pdx,
        PCI8311_EEPROM_CTRL_STAT,
        RegValue
        );

    /* Restore I2O Decode Enable state */
    if (MU_Enabled)
    {
        /* Save state of I2O Decode Enable */
        RegValue =
            PLX_9000_REG_READ(
                pdx,
                PCI8311_FIFO_CTRL_STAT
                );

        PLX_9000_REG_WRITE(
            pdx,
            PCI8311_FIFO_CTRL_STAT,
            RegValue | (1 << 0)
            );
    }

    /* Restore interrupt line */
    PLX_PCI_REG_WRITE(
        pdx,
        CFG_INT_LINE,
        RegInterrupt
        );

    /* If EEPROM was present, restore registers */
    if (EepromPresent)
    {
        /* Mask out HS bits that can be cleared */
        RegHotSwap &= ~((1 << 23) | (1 << 22) | (1 << 17));

        PLX_PCI_REG_WRITE(
            pdx,
            PCI8311_HS_CAP_ID,
            RegHotSwap
            );

        /* Mask out PM bits that can be cleared */
        RegPowerMgmnt &= ~(1 << 15);

        PLX_PCI_REG_READ(
            pdx,
            PCI8311_PM_CSR,
            &RegPowerMgmnt
            );
    }

    return ApiSuccess;
}

/*********************************************************************
 *
 * Function   :  plx_chk_rdr
 *
 * Description:  Loop until receive data ready
 *
 ********************************************************************/
#ifndef FASTER_CL

int plx_chk_rdr(driver_t *p)
{
  int stat;

  cycles_t limit;

  /* Add a couple of cycles for timeout */
  limit = get_cycles() + TIMEOUT_HW_REG_ACCESS;
  PDEBUG(("function:%s limit (%0x)\n", __FUNCTION__, (unsigned int) limit)) ;
  //PDEBUG ( ("function: %s, file: %s line: %d invoked\n", __FUNCTION__, __FILE__, __LINE__ ));

  stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);

  /*  If NOT Receive Data Ready */
  while(!(stat & SPIM_STAT_RECV_DATA_READY)) {  
    stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);
    if (printk_ratelimit()) 
       PDEBUG(("function:%s waiting for RDR (%0x)\n", __FUNCTION__, stat)) ;

   if (unlikely(limit < get_cycles())) {
        PDEBUG(("function:%s limit (%0x)\n", __FUNCTION__, (unsigned int) get_cycles())) ;
       return (-1);
    }

  }
  return(0) ;
}

#else
int plx_chk_rdr(driver_t *p)
{
  int stat;

  cycles_t limit;

  /* Add a couple of cycles for timeout */
  limit = get_cycles() + TIMEOUT_HW_REG_ACCESS;

  stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);

  /*  If not (Receive Data Ready) */
//  while(!(stat & SPIM_STAT_RECV_DATA_READY)) {  
 while(!(stat & SPIM_STAT_READ_DATA_AVAIL)) {  
    stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);
    if (printk_ratelimit()) 
       PDEBUG(("function:%s waiting for RDR (%0x)\n", __FUNCTION__, stat)) ;

   if (unlikely(limit < (get_cycles()))) {
       p->cntr_tmo_rdr++;
       return (-1);
    }
  }
  return(0) ;
}
#endif

#if FASTER_CL
int plx_chk_rsd(driver_t *p)
{
  int stat;

  cycles_t limit;

  /* Add a couple of cycles for timeout */
  limit = get_cycles() + TIMEOUT_HW_REG_ACCESS;

  stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);

  /*  If not (Receive Data Ready) */

 while(!(stat & SPIM_STAT_RSD)) {  
    stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);
    if (printk_ratelimit()) 
       PDEBUG(("function:%s waiting for RSD (%0x)\n", __FUNCTION__, stat)) ;

   if (unlikely(limit < (get_cycles()))) {
       p->cntr_tmo_rdr++;
       return (-1);
    }
  }
  return(0) ;
}
#endif

/*********************************************************************
 *
 * Function   :  plx_chk_tbe
 *
 * Description:  Check for Transmit Buffer Empty
 *
 ********************************************************************/

int plx_chk_tbe(driver_t *p)
{
  int stat;

  cycles_t limit;

  /* Add a couple of cycles for timeout */
  limit = get_cycles() + TIMEOUT_HW_REG_ACCESS;

  stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);

  /*  If not (Receive Data Ready) and not(Transmit Data Empty) */
  while(!(stat & SPIM_STAT_TRANSMIT_DATA_EMPTY)) {  
    stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);
    if (printk_ratelimit()) 
       PDEBUG(("function:%s waiting for TBE(%0x)\n", __FILE__, stat)) ;

   if (unlikely(limit < (get_cycles()))) {
       p->cntr_tmo_tbe++;
       return (-1);
    }
  }
  return(0) ;
}

#if FASTER_CL
int plx_chk_tbusy(driver_t *p)
{
  int stat;

  cycles_t limit;

  /* Add a couple of cycles for timeout */
  limit = get_cycles() + TIMEOUT_HW_REG_ACCESS;

  stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);

  /*  If not (Receive Data Ready) and not(Transmit Data Empty) */
  while(stat & SPIM_STAT_TBUSY) {  
    stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);
    if (printk_ratelimit()) 
       PDEBUG(("function:%s waiting for TBUSY(%0x)\n", __FILE__, stat)) ;

   if (unlikely(limit < (get_cycles()))) {
       p->cntr_tmo_tbe++;
       return (-1);
    }
  }
  return(0) ;
}
#endif

int plx_chk_rdr_and_tbe(driver_t *p)
{
  int stat;

  cycles_t limit;

  /* Add a couple of cycles for timeout */
  limit = get_cycles() + TIMEOUT_HW_REG_ACCESS;

  stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);

  /*  If not (Receive Data Ready) and not(Transmit Data Empty) */
  while((!(stat & SPIM_STAT_RECV_DATA_READY)) && 
        (!(stat & SPIM_STAT_TRANSMIT_DATA_EMPTY))) {  
    stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);
    PDEBUG(("function:%s waiting for RDR and TBE(%0x)\n", __FUNCTION__, stat)) ;

   if (unlikely(limit < (get_cycles()))) {
       return (-1);
    }
  }
  return(0) ;
}

/*********************************************************************
 *
 * Function   :  plx_send_SPI
 *
 * Description:  Send data over SPI command link
 *
 ********************************************************************/
#ifndef FASTER_CL
int plx_send_SPI(driver_t *p, int data)
{
  int stat;
  cycles_t limit;

  /* Add a couple of cycles for timeout */
  limit = get_cycles() + TIMEOUT_HW_REG_ACCESS;
  stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);
//  PDEBUG(("plx_send_SPI: limit: %0x\n", limit)) ;
  // If NOT Transmit Buffer Empty
  while(!(stat & SPIM_STAT_TRANSMIT_DATA_EMPTY)) {  

    stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);
    PDEBUG(("%s waiting for TBE (%0x)\n", __FILE__, data)) ;

    if (unlikely(limit < get_cycles())) {
//         PDEBUG(("plx_send_SPI: timeout: %0x\n", get_cycles())) ;
       return (-1);
    }
 
  }
  COMMAND_LINK_REG_WRITE(p, SPIM_DATA, data);

  return(0);
}

#else
int plx_send_SPI(driver_t *p, int code, int adr,int data)
{

  switch (code) {
  case XL_WC:
     COMMAND_LINK_REG_WRITE(p, SPIM_LW_DATA, (0x02<<24) | ((0xff&data)<<8)| XL_WC|(0xff&adr)); // no of bytes | 0 | data | addr 
      break;
  case XL_RC:
     COMMAND_LINK_REG_WRITE(p, SPIM_LW_DATA, (0x03<<24)                  | XL_RC|(0xff&adr)); // no of bytes | 0 |    0 | addr 
      break;
 case XL_EXEC: case XL_STOP: case XL_INIT:case XL_NOP:
     COMMAND_LINK_REG_WRITE(p, SPIM_LW_DATA, (0x01<<24)                  | code); // no of bytes | 0 |    0 | code 
      break;
  default:
//      WARNING(("%s wrong code(%0x)\n", __FUNCTION__, code));
     ERROR(("%s wrong code(%0x)\n", __FUNCTION__, code));
     break;
  }
  return 0;
}

#endif


/*********************************************************************
 *
 * Function   :  plx_read_fpga_reg/plx_write_fpga_reg
 *
 * Description:  Read from FPGA Register/Write to FPGA Register
 *
 ********************************************************************/

int plx_read_fpga_reg(driver_t *p, int adr, int *data) 
{
  *data = GENERAL_IO_READ(p, adr); 
  if (printk_ratelimit()) 
     PDEBUG(("%s adr=0x%0x,, data=0x%0x\n",__FUNCTION__, adr,*data)) ;
  return 0;
}
int plx_write_fpga_reg(driver_t *p, int adr, int data) 
{
  PDEBUG(("%s adr=0x%0x, data=0x%0x\n",__FUNCTION__, adr, data)) ;
 GENERAL_IO_WRITE(p, adr, data);
 return 0;
}


/*********************************************************************
 *
 * Function   :  plx_immediate_cmd
 *
 * Description:  Immediate command to Command Link
 *               XL_NOP  - Do nothing
 *               XL_INIT - Initialise
 *               XL_EXEC - Execute (now)
 *               XL_STOP - Stop (now)
 
 *
 ********************************************************************/
#ifndef FASTER_CL
int plx_immediate_cmd(driver_t *p, int data) {

 int curr;

// if(plx_chk_rdr(p)) {
   if (plx_chk_rdr_and_tbe(p)) {
    return -ENODEV;
  }

 plx_read_fpga_reg(p, XX_BOARD_TIMER, &curr);

 PDEBUG(("%s code(%0x), time(%x)\n", __FUNCTION__, data, curr));
// print_some_stuff(p);
   if(plx_send_SPI(p, data)) {
    return -ENODEV;
  }

 
  return 0;
}
#else
int plx_immediate_cmd(driver_t *p, int code) {

int curr;

  if(plx_chk_tbe(p)) {
    return -ENODEV;
  }

 plx_read_fpga_reg(p, XX_BOARD_TIMER, &curr);
 PDEBUG(("%s code(%0x), time(%x)\n", __FUNCTION__, code, curr));

   if(plx_send_SPI(p, code, 0, 0)) {
    return -ENODEV;
  }

 
  return 0;
}
#endif
/*********************************************************************
 *
 * Function   :  plx_write_reg
 *
 * Description:  Write to Command Link Register
 *
 ********************************************************************/

#ifndef FASTER_CL
int plx_write_reg(driver_t *p, int adr, int data) 
{

  if(plx_send_SPI(p, XL_WC|adr)) {
    return -ENODEV;
  }
  PDEBUG(("%s adr=0x%0x data=0x%0x \n",__FUNCTION__, (XL_WC |adr), data)) ;
 
 if(plx_chk_rdr(p)) {
    return -ENODEV;
  }
  if(plx_send_SPI(p,data)){
    return -ENODEV;
  }

  if(plx_chk_rdr(p)) {
    return -ENODEV;
  }
  return 0;
}

#else
int plx_write_reg(driver_t *p, int adr, int data) 
{

  if(plx_chk_tbusy(p)) {
    return -ENODEV;
  }

  if(plx_send_SPI(p, XL_WC, adr, data)) {
    return -ENODEV;
  }
  PDEBUG(("%s adr=0x%0x data=0x%0x \n",__FUNCTION__, adr, data)) ;
 
 
  return 0;
}

#endif
/*********************************************************************
 *
 * Function   :  plx_read_reg
 *
 * Description:  Read from Command Link Register
 *
 ********************************************************************/
#ifndef FASTER_CL

int plx_read_reg(driver_t *p, int adr, int *data) 
{
  int dummy ;
  int stat;

  PDEBUG(("%s adr=0x%0x\n",__FUNCTION__, adr)) ;
  dummy = COMMAND_LINK_REG_READ(p, SPIM_DATA); // pre (dummy) read to clear RDR
 
  if(plx_send_SPI(p,XL_RC|adr)){
    return -ENODEV;
  }
  
  if (plx_chk_rdr(p)) {
    return -ENODEV;
  }

  dummy = COMMAND_LINK_REG_READ(p, SPIM_DATA);  // dummy read  or shift in data
  if(plx_send_SPI(p,XL_NOP)){
    return -ENODEV;
  }

  if(plx_chk_rdr(p)){
    return -ENODEV;
  }

  dummy = COMMAND_LINK_REG_READ(p, SPIM_DATA);  // dummy read  or shift in data

  stat = COMMAND_LINK_REG_READ(p, SPIM_STAT);
  PDEBUG(("%s stat=%0x\n",__FUNCTION__, stat)) ;

  if(plx_send_SPI(p,XL_NOP)){
    return -ENODEV;
  }



  if(plx_chk_rdr(p)){
    return -ENODEV;
  }

  *data = COMMAND_LINK_REG_READ(p, SPIM_DATA);  // REAL data read
  PDEBUG(("%s data=%0x\n",__FUNCTION__, *data)) ;
  return 0;
}

#else

int plx_read_reg(driver_t *p, int adr, int *data) 
{
 

  PDEBUG(("%s adr=0x%0x\n",__FUNCTION__, adr)) ;

  if (plx_chk_tbusy(p)) {
    return -ENODEV;
  }

  if(plx_send_SPI(p,XL_RC,adr, 0)){
    return -ENODEV;
  }

  if (plx_chk_rsd(p)) {
    return -ENODEV;
  }
 
  *data = COMMAND_LINK_REG_READ(p, SPIM_DATA);  // REAL data read
  PDEBUG(("%s data=%0x\n",__FUNCTION__, *data)) ;
  return 0;
}

#endif

int plx_write_sram(driver_t *p, int adr, int data) 
{
  PDEBUG(("%s adr=0x%0x, data=0x%0x\n",__FUNCTION__,adr, data)) ;
  SRAM_WRITE(p, adr, data);
 return 0;
}



int plx_read_plx_reg(driver_t *p, int adr, int *data) 
{
  *data = PLX_9000_REG_READ(p, adr); 
 PDEBUG(("%s adr=0x%0x,, data=0x%0x\n",__FUNCTION__, adr,*data)) ;
  return 0;
}

int plx_write_plx_reg(driver_t *p, int adr, int data) 
{
  PDEBUG(("%s adr=0x%0x, data=0x%0x\n",__FUNCTION__, adr, data)) ;
 PLX_9000_REG_WRITE(p, adr, data);
 return 0;
}

/*********************************************************************
 *
 * Function   :  plx_diff_since_read
 *
 * Description:  Read current value and make a diff with previous
 *               read value. Take care of wrap (24 bits)
 *
 *               First time used will return zero
 ********************************************************************/
int plx_diff_since_read(driver_t *p)
{

  int result;  // first time value, will be non-zero all other times

  int curr;

  /* Read current value */
  plx_read_fpga_reg(p, XX_BOARD_TIMER, &curr);

  /* if first time, don't care to diff, just store current value and return
     zero */

    if (curr < before) 
      result = ((1<<24)-1 + before + curr) & 0xffffff;   // timer reg 24 bit
    else 
      result = (curr - before);

    before = curr;
  

 return result;
}

/*********************************************************************
 *
 * Function   :  plx_synchronized_register_modify
 *
 * Description:  Synchronized function with ISR to modify a PLX register
 *
 ********************************************************************/
BOOLEAN plx_synchronized_register_modify( PLX_REG_DATA *pRegData)
					     
{
  unsigned long flags;
  U32           RegValue;


  /*************************************************
   * This routine sychronizes modification of a
   * register with the ISR.  To do this, it uses
   * a special spinlock routine provided by the
   * kernel, which will temporarily disable interrupts.
   * This code should also work on SMP systems.
   ************************************************/

  /* Disable interrupts and acquire lock */

  spin_lock_irqsave(&(pRegData->pdx->Lock_Isr),flags);

  // Update the register
  RegValue =
    PLX_9000_REG_READ(
		      pRegData->pdx,
		      pRegData->offset
		      );

  RegValue |= pRegData->BitsToSet;
  RegValue &= ~(pRegData->BitsToClear);

  PLX_9000_REG_WRITE(
		     pRegData->pdx,
		     pRegData->offset,
		     RegValue
		     );

  // Re-enable interrupts and release lock


  spin_unlock_irqrestore( &(pRegData->pdx->Lock_Isr), flags );

  return true;
}


/******************************************************************************
 *
 * Function   :  plx_chip_interrupts_enable
 *
 * Description:  Globally enables PLX chip interrupts
 *
 *****************************************************************************/

BOOLEAN plx_chip_interrupts_enable( driver_t *pdx)
{
  PLX_REG_DATA RegData;


  // Setup for synchronized register access
  RegData.pdx         = pdx;
  RegData.offset      = PCI8311_INT_CTRL_STAT;
//  RegData.BitsToSet   = (1 << 8);    
 
#if DMA_DEBUG
  RegData.BitsToSet   = (1 << 8) |(1<<10) | (1<<11) | (1<<12);  // Enable a limited number of retries to become Master (256)
#else
 RegData.BitsToSet   = (1 << 8) |(1<<11);  
#endif   
  RegData.BitsToClear = (0<<16);     //Disable Local Interrupt Output enable

  plx_synchronized_register_modify(
				&RegData
				);

  return true;
}



/******************************************************************************
 *
 * Function   :  plx_chip_interrupts_disable
 *
 * Description:  Globally disables PLX chip interrupts
 *
 *****************************************************************************/
BOOLEAN	plx_chip_interrupts_disable( driver_t *pdx	)
{
  PLX_REG_DATA RegData;


  // Setup for synchronized register access
  RegData.pdx         = pdx;
  RegData.offset      = PCI8311_INT_CTRL_STAT;
  RegData.BitsToSet   = 0;
  RegData.BitsToClear = (1 << 8);
#if ZERO
  RegData.BitsToClear = (1 << 8) | (1<<11);
#endif
  plx_synchronized_register_modify(
				&RegData
				);

  return true;
}


/******************************************************************************
 *
 * Function   :  plx_pci_bar_resource_map
 *
 * Description:  Maps a PCI BAR resource into kernel space
 *
 ******************************************************************************/
int plx_pci_bar_resource_map( driver_t *pdx, U8 BarIndex )
{
  // Default resource to not claimed
  pdx->PciBar[BarIndex].bResourceClaimed = false;

  // Default to NULL kernel VA
  pdx->PciBar[BarIndex].pVa = NULL;

  // Request and Map space
  if (pdx->PciBar[BarIndex].Properties.bIoSpace)
    {
      // Request I/O port region
      if (request_region(
			 pdx->PciBar[BarIndex].Properties.Physical,
			 pdx->PciBar[BarIndex].Properties.Size,
			 DRIVER_NAME
			 ) != NULL)
	{
	  // Note that resource was claimed
	  pdx->PciBar[BarIndex].bResourceClaimed = true;
	}
    }
  else
    {
      // Request memory region     FIXME pci_request_region instead???
      if (request_mem_region(
			     pdx->PciBar[BarIndex].Properties.Physical,
			     pdx->PciBar[BarIndex].Properties.Size,
			     DRIVER_NAME
			     ) == NULL)
	{
	  return (-ENOMEM);
	}
      else
	{
	  // Note that resource was claimed
	  pdx->PciBar[BarIndex].bResourceClaimed = true;

	  // Get a kernel-mapped virtual address
	  pdx->PciBar[BarIndex].pVa =
	    ioremap(
		    pdx->PciBar[BarIndex].Properties.Physical,
		    pdx->PciBar[BarIndex].Properties.Size
		    );

	  if (pdx->PciBar[BarIndex].pVa == NULL)
	    {
	      return (-ENOMEM);
	    }
	}
    }

  return 0;
}


/******************************************************************************
 *
 * Function   :  plx_pci_bar_resources_unmap
 *
 * Description:  Unmap all mapped PCI BAR memory for a device
 *
 ******************************************************************************/
void plx_pci_bar_resources_unmap(driver_t *pdx)
{
  U8 i;


  // Go through all the BARS
  for (i = 0; i < PCI_NUM_BARS_TYPE_00; i++)
    {
      // Unmap the space from Kernel space if previously mapped
      if (pdx->PciBar[i].Properties.Physical != 0)
	{
	  if (pdx->PciBar[i].Properties.bIoSpace)
	    {
	      // Release I/O port region if it was claimed
	      if (pdx->PciBar[i].bResourceClaimed)
		{
		  release_region(
				 pdx->PciBar[i].Properties.Physical,
				 pdx->PciBar[i].Properties.Size
				 );
		}
	    }
	  else
	    {
	      INFO(("Unmapping BAR %d...\n", i));

	      // Unmap from kernel space
	      if (pdx->PciBar[i].pVa != NULL)
		{
		  iounmap(
			  pdx->PciBar[i].pVa
			  );

		  pdx->PciBar[i].pVa = NULL;
		}

	      // Release memory region if it was claimed
	      if (pdx->PciBar[i].bResourceClaimed)
		{
		  release_mem_region(
				     pdx->PciBar[i].Properties.Physical,
				     pdx->PciBar[i].Properties.Size
				     );
		}
	    }
	}
    }
}


/******************************************************************************
 *
 * Function   :  plx_install_interrupt_handler
 *
 * Description:  Install interrupt handler
 *
 *****************************************************************************/

//int plx_install_interrupt_handler(driver_t *dev ) {
int plx_install_interrupt_handler(driver_t *dev ) {
 
  int retval = ApiFailed;	
  int rc;

  // Disable all interrupts
  plx_chip_interrupts_disable(dev);




  /* Clear DMA interrupts */
  {
      U32 RegValue;
      RegValue =
	PLX_9000_REG_READ(dev, PCI8311_DMA_COMMAND_STAT);

      // Clear DMA interrupt
      PLX_9000_REG_WRITE(dev,PCI8311_DMA_COMMAND_STAT,
			 RegValue | (1 << 11) | (1 << 3)
			 );
  }

  /* Clear local interrupt */
 {
  U32 RegDetectorInt;  
  RegDetectorInt = COMMAND_LINK_REG_READ(dev, PCI_MC_CARD_STATUS);
  RegDetectorInt |= (1<<30); // Clear interrupt
  COMMAND_LINK_REG_WRITE(dev,PCI_MC_CARD_STATUS,RegDetectorInt); 
 }
  // Install the ISR if available
  if (dev->pPciDevice->irq != 0)
    {
      rc =
	request_irq(
		    dev->pPciDevice->irq,    // The device IRQ
		    plx_on_interrupt,             // Interrupt handler
		    IRQF_SHARED,                // Flags, support interrupt sharing
		    DRIVER_NAME,         // The driver name
		    dev                      // Parameter to the ISR
		    );

      if (rc != 0)
	{
	  ERROR(("ERROR - Unable to install ISR\n"));
	}
      else
	{
	  INFO(("Installed ISR for interrupt\n"));

	  /* Flag that the interrupt resource was claimed */
	  dev->Flag_Interrupt = true;

          retval = ApiSuccess;
	}
    }
  else
    {
      ERROR(("Device is not using a PCI interrupt resource\n"));
    }

// Re-enable interrupts
    plx_chip_interrupts_enable(dev);   
  return retval;
}

/******************************************************************************
 *
 * Function   :  plx_uninstall_interrupt_handler
 *
 * Description:  Uninstall interrupt handler
 *
 *****************************************************************************/

int plx_uninstall_interrupt_handler(driver_t *dev ) {

  if ( dev->Flag_Interrupt == true )
    {
      // First PlxChipFn.c and then  SupportFunc.c plx_synchronized_register_modify
      // Disable all interrupts
      plx_chip_interrupts_disable (
				dev
				);

      INFO ( (
		     "Removing interrupt handler (IRQ = %02d [0x%02x])...\n",
		     dev->pPciDevice->irq, dev->pPciDevice->irq
		     ) );


      // Release IRQ
      free_irq ( dev->pPciDevice->irq,dev);

      // Flag that the interrupt resource was released
      dev->Flag_Interrupt = false;
    }
   else 
    {
        ERROR(("No one has allocated the IRQ!\n"));
    }

    return 0;

}
/*
**===========================================================================
** 8.0			plx_dma_transfer_block()
**===========================================================================
** Description: Initiate a DMA transfer
**
** Parameters:
**
**
** Returns:     none
**
** Globals:
*/

int plx_dma_transfer_block(driver_t *dev, U8 current_hw_buf, U8 channel) {
//  U8 channel = 0;		
  U8  shift;
  U16 OffsetDmaMode;
  U32 RegValue;

  // Verify DMA channel & setup register offsets
  switch (channel)
    {
    case 0:
      OffsetDmaMode = PCI8311_DMA0_MODE;
      break;

    case 1:
      OffsetDmaMode = PCI8311_DMA1_MODE;
      break;

    default:
      ERROR(("ERROR - Invalid DMA channel\n"));
      return ApiDmaChannelInvalid;
    }

  // Set shift for status register
  shift = (channel * 8);

  // Verify that DMA is not in progress
  RegValue =
    PLX_9000_REG_READ(dev,PCI8311_DMA_COMMAND_STAT);

  if ((RegValue & ((1 << 4) << shift)) == 0)
    {
      ERROR(("ERROR - DMA channel is currently active\n"));
      return ApiDmaInProgress;
    }

  spin_lock(&(dev->Lock_DmaChannel));
  /* Verify DMA channel was opened */
//  if (unlikely(dev->DmaInfo[channel].bOpen == false))
 if (unlikely(dev->dma_channels[channel] == false))
    {
      ERROR(("ERROR - DMA channel has not been opened\n"));

      spin_unlock( &(dev->Lock_DmaChannel));

      return ApiDmaChannelUnavailable;
    }
  spin_unlock(&(dev->Lock_DmaChannel));


  spin_lock(&(dev->Lock_HwAccess));
  /* Get DMA mode */
  RegValue =
    PLX_9000_REG_READ(dev,OffsetDmaMode);

   // FIXME: needed to be setup for every transfer? no..
  // Disable DMA chaining
  RegValue &= ~(1 << 9);
  // Enable interrupt & route interrupt to PCI
  RegValue |= (1 << 10) | (1 << 17);

  PLX_9000_REG_WRITE(
		     dev,
		     OffsetDmaMode,
		     RegValue
		     );


 PDEBUG(("Starting DMA channel:%d, buffer: %d, bus addr:%x, frame addr:%x\n", channel, current_hw_buf, dev->bus_addresses[dev->wp], (unsigned int) dev->frames[dev->wp]));

  // Write PCI Address
  PLX_9000_REG_WRITE( dev,
		     OffsetDmaMode + 0x4,     // PCI8311_DMAxx_PCI_ADDR
#if STEP_IN_INT
                     dev->bus_addresses[dev->ip] 
#else
		     dev->bus_addresses[dev->wp] 
#endif
		     );

#if DMA_DEBUG

  /* Write a test pattern at start address  in kernel mem */

  *(unsigned long *)(dev->frames[dev->wp]) = 0xdeadbeef;

#endif


#if OBSOLETE

  // Check if we have reached the read pointer
  if ((dev->wp+1 == dev->rp) || (dev->wp+1==NO_OF_BUFFERS && !(dev->rp))) {
     ERROR(("ERROR function: %s, file: %s line: %d invoked, rp:%d, wp:%d\n", __FUNCTION__, __FILE__, __LINE__,dev->rp, dev->wp));
}
   // Increment and check if we need to wrap
   dev->wp++;
        if (dev->wp == NO_OF_BUFFERS)
                dev->wp = 0;
#endif


  // Write Local Address
  PLX_9000_REG_WRITE( dev,
		     OffsetDmaMode + 0x8,    // PCI8311_DMAxx_LOCAL_ADDR

                     LOCAL_PCI_SRAM_BASE + ( current_hw_buf *DMA_SIZE)
		     );                      



  // Write Transfer Count
  PLX_9000_REG_WRITE(dev,
		     OffsetDmaMode + 0xc,   // PCI8311_DMAxx_COUNT
		     DMA_SIZE               /* Transfer Size in bytes */
		     );

  // Write Descriptor Pointer
  RegValue = 
    (1 << 2) |  /* Interrupt after terminal count */ 
    (1 << 3);  /* Transfer from Local Bus to the internal PCI Bus */

  PLX_9000_REG_WRITE( dev,
		     OffsetDmaMode + 0x10,   // PCI8311_DMAxx_DESC_PTR
		     RegValue
		     );

  // Enable DMA channel
  RegValue =
    PLX_9000_REG_READ(dev,
		      PCI8311_DMA_COMMAND_STAT
		      );

  PLX_9000_REG_WRITE(dev,
		     PCI8311_DMA_COMMAND_STAT,
		     RegValue | ((1 << 0) << shift)
		     );

  spin_unlock(&(dev->Lock_HwAccess));

 

// FIXME: Outside Lock_HwAccess ???

  // Start DMA
  PLX_9000_REG_WRITE(
		     dev,
		     PCI8311_DMA_COMMAND_STAT,
		     RegValue | (((1 << 0) | (1 << 1)) << shift)
		     );

  return ApiSuccess;
}

/*
**===========================================================================
** 8.0			plx_dma_open_channel()
**===========================================================================
** Description: Open a DMA channel
**
** Parameters:
**
**
** Returns:     none
**
** Globals:
*/
int plx_dma_open_channel( driver_t *dev, u8 channel)
{
  U8  shift;
  int r;
  U32          RegValue;
  PLX_REG_DATA RegData;

  /* Set shift for status register */
  shift = (channel * 8);

  spin_lock(&(dev->Lock_DmaChannel));

  // Verify that we can open the channel
  if (dev->dma_channels[channel] == true)
    {
      ERROR(("ERROR - DMA channel already opened\n"));

      spin_unlock(&(dev->Lock_DmaChannel));

      return ApiDmaChannelUnavailable;
    }

  // Open the channel
  dev->dma_channels[channel] = true;


  spin_unlock( &(dev->Lock_DmaChannel));

#if ZERO
  // Force DMA abort, which may generate a DMA done interrupt
  plx_chip_dma_control(dev,
			 channel,
			 DmaAbort);
#endif
  {
    U32 RegValue;

    /* Make it possible for others, than the PLX device, to access the Local Bus */
    RegValue = PLX_9000_REG_READ(dev,PCI8311_DMA_ARBIT);

    PLX_9000_REG_WRITE(dev,PCI8311_DMA_ARBIT, RegValue | ((1 << 18)));
    

  }  

  {

   
     U32 RegValue;

    /* Disable DMA channel */
    RegValue =
    PLX_9000_REG_READ(dev,
		      PCI8311_DMA_COMMAND_STAT
		      );

    PLX_9000_REG_WRITE(dev,
		     PCI8311_DMA_COMMAND_STAT,
		     RegValue & ~((1 << 0) << shift)   
		     );

    // Abort if any ongoing transfer

    PLX_9000_REG_WRITE(dev,
		     PCI8311_DMA_COMMAND_STAT,
		     RegValue | ((1 << 2)<< shift)   
		     );


    // Enable DMA channel
     PLX_9000_REG_WRITE(dev,
		     PCI8311_DMA_COMMAND_STAT,
		     RegValue | ((1 << 0)<< shift)
		     );
  }			 


  // Setup for synchronized access to Interrupt register
  RegData.pdx        = dev;
  RegData.offset      = PCI8311_INT_CTRL_STAT;
  RegData.BitsToClear = 0;

  spin_lock(&(dev->Lock_HwAccess) );

  // Setup DMA properties
  RegValue =
    (0 <<  9) |                  // Disable chaining
    (1 << 10) |                  // Enable DMA done interrupt 
    (1 << 17) |                  // Route interrupt to PCI     
    (0 << 18) |                  // Disable Dual-Addressing
    (0 << 20) |                  // Disable Valid mode
    (3 <<  0) |                  // Local Bus Data width set to 32 bit
    (0   <<  2) |  // Wait State Counter
    (1   <<  6) |  // READY#/TA# Input Enable
    (1   <<  7) |  // Continuous Burst Enable
    (1   <<  8) |  // Local Burst Enable
    (0   << 11) |  // Local Addressing Mode
    (0   << 12) |  // Demand Mode
    (0   << 13) |  // Write Invalid Mode
    (0   << 14) |  // EOT# Enable
    (0   << 15);   // Fast/Slow Terminate Mode Select
  if (channel == 0)
    {
      // Write DMA mode
      PLX_9000_REG_WRITE( dev,PCI8311_DMA0_MODE,RegValue );

      // Clear Dual Address cycle register
      PLX_9000_REG_WRITE( dev, PCI8311_DMA0_PCI_DAC, 0 );

      // Enable DMA channel interrupt

      RegData.BitsToSet = (1 << 18);

    }
  else
    {
      // Write DMA mode
      PLX_9000_REG_WRITE(dev, PCI8311_DMA1_MODE, RegValue );

      // Clear Dual Address cycle register
      PLX_9000_REG_WRITE( dev,PCI8311_DMA1_PCI_DAC, 0 );

      // Enable DMA channel interrupt
      RegData.BitsToSet = (1 << 19);
    }

  /* Update interrupt register */
  plx_synchronized_register_modify(&RegData);
 

 
// FIXME: allocate only once .... If Channel 0 is open, skip this part.
#if DMA1
 if (dev->dma_channels[0] == true) {
  spin_unlock( &(dev->Lock_HwAccess) );
  return ApiSuccess;
 }
#endif

  dev->hw_buff_check = 0;
  dev->rp = dev->wp = dev->ip = 0; 
  for (r=0; r<NO_OF_BUFFERS; r++) {
     dev->frames[r] =  dma_alloc_coherent(&(dev->pPciDevice->dev), DMA_SIZE, &(dev->bus_addresses[r]), GFP_KERNEL);

    if (dev->frames[r] == NULL) {
     spin_unlock( &(dev->Lock_HwAccess) );
     return ApiFailed;	
     }
    
    PDEBUG(("plx_dma_open_channel, frame:%d, got virtual addr: %lx, bus addr:%lx\n", r, (unsigned long) dev->frames[r],(unsigned long) dev->bus_addresses[r]));	

#if DMA_DEBUG
    *(unsigned long *)(dev->frames[r]) = 0x44443333;
#endif
 }

  INFO(("Opened DMA channel %d\n",channel));

  spin_unlock( &(dev->Lock_HwAccess) );


  return ApiSuccess;
}

/******************************************************************************
 *
 * Function   :  plx_chip_dma_status
 *
 * Description:  Get status of a DMA channel
 *
 ******************************************************************************/
PLX_STATUS plx_chip_dma_status( driver_t *pdx, U8 channel  )
{
  U32 RegValue;


  // Verify valid DMA channel
  switch (channel)
    {
    case 0:
    case 1:
      break;

    default:
      ERROR(("ERROR - Invalid DMA channel\n"));
      return ApiDmaChannelInvalid;
    }

  // Return the current DMA status
  RegValue =
    PLX_9000_REG_READ(pdx,
		      PCI8311_DMA_COMMAND_STAT
		      );

  // Shift status for channel 1
  if (channel == 1)
    RegValue = RegValue >> 8;

  if ((RegValue & ((1 << 4) | (1 << 0))) == 0)
    return ApiDmaPaused;

  if (RegValue & (1 << 4))
    return ApiDmaDone;

  return ApiDmaInProgress;
}

/*
**===========================================================================
** 8.0			plx_dma_close_channel()
**===========================================================================
** Description: Close a DMA channel
**
** Parameters:
**
**
** Returns:     none
**
** Globals:
*/
int plx_dma_close_channel ( driver_t *dev, u8 channel, BOOLEAN bCheckInProgress )
{
//  U8 channel = 0;
  U32 RegValue;

  INFO(("Closing DMA channel %d...\n",channel));

  // Verify DMA channel was opened
  if (dev->dma_channels[channel] == false)
    {
      ERROR(("ERROR - DMA channel has not been opened\n"));
      return ApiDmaChannelUnavailable;
    }

  // Check DMA status
  RegValue =
    PLX_9000_REG_READ(dev,
		      PCI8311_DMA_COMMAND_STAT
		      );

  // Shift status for channel 1
  if (channel == 1)
    RegValue = RegValue >> 8;

  // Verify DMA is not in progress
  if ((RegValue & (1 << 4)) == 0)
    {
      // DMA is still in progress
      if (bCheckInProgress)
        {
	  if (RegValue & (1 << 0))
	    return ApiDmaInProgress;
	  else
	    return ApiDmaPaused;
        }

      PDEBUG(("DMA in progress, aborting...\n"));

      // Force DMA abort, which may generate a DMA done interrupt
      plx_chip_dma_control(dev,
			 channel,
			 DmaAbort
			 );
    }

  spin_lock( &(dev->Lock_DmaChannel) );

  // Close the channel
  dev->dma_channels[channel] = false;

  spin_unlock(&(dev->Lock_DmaChannel));



#if DMA1
 if (dev->dma_channels[0] != true) {
  return ApiSuccess;
 }
#endif

  {
   int i;
  for(i=0;i<NO_OF_BUFFERS; i++) {
     dma_free_coherent(&(dev->pPciDevice->dev), DMA_SIZE, dev->frames[i], dev->bus_addresses[i]);
     //FIXME: what about the virtual mapping, need to deref?
  }
 }



  return ApiSuccess;
}
/*
**===========================================================================
** 8.0			plx_request_resources()
**===========================================================================
** Description: Init internal device structure
**
** Parameters:
**
**
** Returns:     none
**
** Globals:
*/

static int __devinit  plx_request_resources( struct pci_dev *pcidev,  driver_t *dev )
{

  U8 i;
  int rc;
  U8 ResourceCount = 0;

	
  // Save the OS-supplied PCI object
  dev->pPciDevice = pcidev;
	
  for ( i = 0; i < PCI_NUM_BARS_TYPE_00; ++i )
    {
      // Verify the address is valid, skip otherwise
      if ( pci_resource_start (dev->pPciDevice,i) == 0 )
	{
	  continue;
	}

      INFO (("   Resource %02d\n", ResourceCount ));

      // Increment resource count
      ResourceCount++;

      // Get PCI physical address
      dev->PciBar[i].Properties.Physical =
	pci_resource_start ( dev->pPciDevice, i );

      // Determine resource type
      if ( pci_resource_flags (dev->pPciDevice, i ) & IORESOURCE_IO )
	{
	  INFO (("     Type     : I/O Port\n" ));

	  // Make sure flags are cleared properly
	  dev->PciBar[i].Properties.Physical &= ~ ( 0x3 );
	  dev->PciBar[i].Properties.bIoSpace  = true;
	}
      else
	{
	  INFO (("     Type     : Memory Space\n" ));

	  // Make sure flags are cleared properly
	  dev->PciBar[i].Properties.Physical &= ~ ( 0xf );
	  dev->PciBar[i].Properties.bIoSpace  = false;
	}

      // Get the actual BAR value
      PLX_PCI_REG_READ (dev, PCI8311_RTR_BASE + ( i * sizeof ( U32 ) ),&( dev->PciBar[i].Properties.BarValue )
			);

      INFO ((
	       "     PCI BAR %d: %08X\n",
	       i, dev->PciBar[i].Properties.BarValue
	       ));

      INFO ((
	       "     Phys Addr: %08lX\n",
	       ( PLX_UINT_PTR ) dev->PciBar[i].Properties.Physical
	       ));

      // Get the size
      dev->PciBar[i].Properties.Size =
	pci_resource_len ( dev->pPciDevice, i);

      if ( dev->PciBar[i].Properties.Size >= ( 1 << 10 ) )
	{
	  INFO ((
		   "     Size     : %08lx  (%ld Kb)\n",
		   ( PLX_UINT_PTR ) dev->PciBar[i].Properties.Size,
		   ( PLX_UINT_PTR ) dev->PciBar[i].Properties.Size >> 10
		   ));
	}
      else
	{
	  INFO ((
		   "     Size     : %08lx  (%ld bytes)\n",
		   ( PLX_UINT_PTR ) dev->PciBar[i].Properties.Size,
		   ( PLX_UINT_PTR ) dev->PciBar[i].Properties.Size
		   ));
	}

      // Claim and map the resource
      rc =
	plx_pci_bar_resource_map ( dev, i);

      if ( rc == 0 )
	{
	  if ( dev->PciBar[i].Properties.bIoSpace == false )
	    {
	      INFO ((
		       "     Kernel VA: %p\n",
		       dev->PciBar[i].pVa
		       ));
	    }
	}
      else
	{
	  if ( dev->PciBar[i].Properties.bIoSpace == false )
	    {
	      ERROR ( (
			     "     Kernel VA: ERROR - Unable to map space to Kernel VA\n"
			     ) );
	    }
	}
		
    }

  // Check for interrupt resource
  if ( dev->pPciDevice->irq != 0 )
    {
      INFO (("   Resource %02d\n", ResourceCount ));
      INFO (("     Type     : Interrupt\n" ));
      INFO (("     IRQ      : %02d [0x%02x]\n",
	       dev->pPciDevice->irq, dev->pPciDevice->irq));

 
    }





  // Initialize work queue for ISR DPC queueing
  INIT_WORK(&(dev->Task_plx_dpc_for_isr), plx_dpc_for_isr);
 

  /* Initialize device open spinlock */
  spin_lock_init( &(dev->Lock_DeviceOpen) );

  // Initialize ISR spinlock
  spin_lock_init( &(dev->Lock_Isr)	 );

  // Initialize Register Access spinlock
  spin_lock_init( &(dev->Lock_HwAccess) );

  // Initialize DMA channel spinlock
  spin_lock_init( &(dev->Lock_DmaChannel) );
    
  // Initialize wait read_queue for read()
  init_waitqueue_head(&(dev->read_queue));

  dev->my_workqueue = create_singlethread_workqueue("my_queue"); 

  init_MUTEX(&dev->read_semph);

  /* Update device started flag */
  dev->Flag_started = true;


  return 0;
}

/******************************************************************************
 *
 * Function   :  plx_release_resources
 *
 * Description:  Release device resources ...
 *
 ******************************************************************************/
static void __devexit plx_release_resources(driver_t *dev)
{
	

  /* Only stop devices which have been started */
  if ( dev->Flag_started == false )
    {
       ERROR ( ( "Only stop the device if it has been started\n" ) );
      return;
    }

  INFO ( ( "Releasing device resources...\n" ) );


  // Unmap I/O regions from kernel space (No local register access after this)
  {
    U8 i;


    // Go through all the BARS
    for ( i = 0; i < PCI_NUM_BARS_TYPE_00; i++ )
      {
	// Unmap the space from Kernel space if previously mapped
	if ( dev->PciBar[i].Properties.Physical != 0 )
	  {
	    if ( dev->PciBar[i].Properties.bIoSpace )
	      {
		// Release I/O port region if it was claimed
		if ( dev->PciBar[i].bResourceClaimed )
		  {
                    INFO ( ( "Unmapping BAR %d...\n", i ) );
		    release_region (
				    dev->PciBar[i].Properties.Physical,
				    dev->PciBar[i].Properties.Size
				    );
		  }
	      }
	    else
	      {
		INFO ( ( "Unmapping BAR %d...\n", i ) );

		// Unmap from kernel space
		if ( dev->PciBar[i].pVa != NULL )
		  {
		    iounmap (
			     dev->PciBar[i].pVa
			     );

		    dev->PciBar[i].pVa = NULL;
		  }

		// Release memory region if it was claimed
		if ( dev->PciBar[i].bResourceClaimed )
		  {
		    release_mem_region (
					dev->PciBar[i].Properties.Physical,
					dev->PciBar[i].Properties.Size
					);
		  }
	      }
	  }
      }
  }



  /* Mark that device is not started */
  dev->Flag_started = false;
}


static int __devinit probe ( struct pci_dev *pcidev, const struct pci_device_id *id );
static void  __devexit remove ( struct pci_dev *pcidev );

static struct pci_driver pci_driver =
  {
    .name        =  DRIVER_NAME,
    .id_table    = pci_device_id,
    .probe       = probe,
    .remove      = remove,
    .suspend     = NULL,
    .resume      = NULL,

  };

		
/*
**===========================================================================
** 8.0			probe()
**===========================================================================
** Description: kernel found a match. 
**
** Parameters:  *dpciev - pointer to the our PCI peripheral in the kernel's
**                     PCI layer
**              *id  - pointer to the structure matching the peripheral
**
**              __devinit forces the kernel to release function code after
**              the initialization.
**
**
** Returns:     return 0 on sucess, error code otherwise
**
** Globals:
*/

static int __devinit probe ( struct pci_dev *pcidev, const struct pci_device_id *id )
{
  int ret =-EIO;

  PDEBUG ( ("function: %s, file: %s line: %d invoked\n", __FUNCTION__, __FILE__, __LINE__ ));

  /* Enable the device */
  if ( pci_enable_device ( pcidev ) )
    {
      ERROR (("probe: Error enabling device\n" ));
      return -ENODEV;
    }

  /* Check if any IRQ assigned */
  if ( ! ( pcidev->irq ) )
    {
      ERROR ( ("probe: IRQ not assigned\n" ));
      ret =  -ENODEV;
      goto err_out_disable_device;
    }

  /* Enable DMA and check that the device can handle 32 bit DMA*/
  if ( dma_set_mask ( &(pcidev->dev), DMA_32BIT_MASK ) )
    {
      ERROR ( ("probe: architecture do not support 32bit PCI busmaster DMA\n" ));
      ret =  -ENODEV;
      goto err_out_disable_device;
    }


  /* Try map resources in the structure */
  if ( plx_request_resources ( pcidev,  PL_devp ) )
    {

      ERROR ( ("probe: mapping of regs failed\n" ));
      goto err_out_disable_device;
    }


  /* Make it possible to reach the structure from the pci_device struct */
  
  pci_set_drvdata ( pcidev, PL_devp );


  /* Get a define state. Reset the hardware */
  plx_board_reset(PL_devp);

  /* Reset local interrupt on PCI board */
  COMMAND_LINK_REG_WRITE(PL_devp,PCI_MC_CARD_STATUS,(1<<30)); 

/* Stop, if any ongoing acqusition */
 plx_immediate_cmd(PL_devp, XL_STOP);

 /* Reset peripheral hardware */
 plx_immediate_cmd(PL_devp, XL_INIT);


#if LATENCY_TIMER
/* Change the Internal PCI Bus Latency Timer 128 bytes could perhaps be changed up to 240 (0xf0)  */
 { 
   u32 data;

   plx_read_plx_reg(PL_devp, PCI8311_CACHE_SIZE, &data);

/*
  RegValue |= pRegData->BitsToSet;
  RegValue &= ~(pRegData->BitsToClear);

*/
   data |= (0x80<<8);
   plx_write_plx_reg(PL_devp, PCI8311_CACHE_SIZE, data); 


 }
#endif

  return 0;


 err_out_disable_device:

  pci_disable_device (pcidev);
  return ret;
}


/*
**===========================================================================
** 8.0			plx_drv_init_module()
**===========================================================================
** Description:  Called at load of the the device driver module 
**
** Parameters:  

** Returns:     
**
** Globals:
*/

static __init int plx_drv_init_module(void)
{

int result = 0;
dev_t dev = MKDEV(plx_drv_major, plx_drv_minor);
	

PDEBUG ( ("function: %s, line: %d invoked\n", __FUNCTION__,__LINE__ ));


	/*
	 * Register and accept a dynamic number.
	 */

	if (plx_drv_major) 
        {
                dev = MKDEV(plx_drv_major, plx_drv_minor);
		result = register_chrdev_region(dev, 1, DRIVER_NAME);   /* No of devices set to 1 */
        }
	else {
		result = alloc_chrdev_region(&dev, plx_drv_minor, 1, DRIVER_NAME); /* No of devices eq to 1 */
		plx_drv_major = MAJOR(dev);
	}
	if (result < 0) {
                ERROR ( ("function: %s, line: %d, unable to get major:%d\n", __FUNCTION__,__LINE__,plx_drv_major ));
		return result;
        }

        /* Allocate an internal structure for the driver */
        PL_devp = kzalloc ( sizeof ( driver_t ), GFP_KERNEL );
        if ( !PL_devp )
        {
          result = -ENOMEM;
          goto fail_alloc;
        }


        PDEBUG(("function: %s, file: %s line: %d invoked, dev %lx\n", __FUNCTION__, __FILE__, __LINE__, (unsigned long)PL_devp));

	cdev_init(&(PL_devp->cdev), &pci_drv_fops);
	PL_devp->cdev.owner = THIS_MODULE;
	PL_devp->cdev.ops = &pci_drv_fops;

        /* Only one device number associated with the device (argument 3), page 56 */
        dev = MKDEV(plx_drv_major, plx_drv_minor);
	result = cdev_add (&(PL_devp->cdev), dev, 1);
	
        /* Fail gracefully if need be */
	if (result) {
                ERROR ( ("function: %s, line: %d, err:%d\n", __FUNCTION__,__LINE__ , result));
          goto failed_add;
        }

        if(create_proc_read_entry("plx_drv", 0 /* default mode */,
                        NULL /* parent dir */, plx_read_procmem,
                        NULL /* client data */) == 0) {

                ERROR ( ("function: %s, line: %d, Unable to register plx_drv proc file\n", __FUNCTION__,__LINE__));
               
                return -ENOMEM;
        }


	return result;
 
failed_add:
	
       kfree(PL_devp);

fail_alloc:
       
    
        unregister_chrdev_region(MKDEV(plx_drv_major, plx_drv_minor), 1);
	return result;

}



/*
**===========================================================================
** 8.0			remove()
**===========================================================================
** Description:  Executed when the device handled by this driver is removed.
**               This function will be called by plx_drv_exit_module
**               
**
** Parameters:  *pcidev - pointer to the our PCI peripheral in the kernel's
**                     PCI layer

** Returns:     return 0 on sucess, error code otherwise
**
** Globals:
*/
static void  __devexit remove ( struct pci_dev *pcidev )
{

  driver_t *dev = pci_get_drvdata (pcidev);

  PDEBUG ( ("function: %s, line: %d invoked\n", __FUNCTION__,__LINE__ ));

  /* Remove assoc to the device driver */
  pci_set_drvdata (pcidev, NULL);

  /* To be sure, abort */
  plx_immediate_cmd(dev, XL_STOP);

  /* Flush and delete our work queue */
  flush_workqueue(dev->my_workqueue); 
  destroy_workqueue(dev->my_workqueue); 

  /* Release the resources */
  plx_release_resources(dev);	
  
  pci_disable_device(dev->pPciDevice);

 

}


/*
**===========================================================================
** 8.0			plx_drv_exit_module
**===========================================================================
** Description:  Called at unload of the the device driver module 
**
** Parameters:  
** Returns:     
**
** Globals:
*/

static void __exit plx_drv_exit_module ( void )
{


   PDEBUG ( ("function: %s, line: %d invoked\n", __FUNCTION__,__LINE__ ));
  

  cdev_del(&(PL_devp->cdev));

  unregister_chrdev_region(MKDEV(plx_drv_major, plx_drv_minor), 1);
 

  /* Calls remove() before returning here */
  pci_unregister_driver (&pci_driver);

 /* Free driver resources */
  if( PL_devp != NULL) {
     kfree ( PL_devp );
  }

 INFO ( ("function: %s, line: %d we are free!\n", __FUNCTION__,__LINE__ ));

  
}

/* Declare the module's initialization and cleanup functions */
module_init ( plx_drv_init_module );
module_exit ( plx_drv_exit_module );

/* Place documentation on the module in object file (plx_drv.ko) */
MODULE_DESCRIPTION ( "PLX device driver" );
MODULE_AUTHOR ( "Fredrik Svard (fredrik@c-rad.se)" );
MODULE_VERSION ("0.2");  
MODULE_LICENSE ( "GPL" );

