/*
	                     *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : Gemini                                                      **
** filename :  plx_interrupt.c                                             **
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
Description : 

Version     : 3
Date        : November 7, 2008
Revised by  : Fredrik Svard
Description : Interrupt service routine and support functions

TODO:       - Handle 2 DMA channels? 0 and 1?
            

*/
#include <linux/kernel.h>       /* container_of() */

#include "plx_interrupt.h"
#include "plx_driver.h"

/**********************************************
*               Definitions
**********************************************/
#define INTR_TYPE_NONE                        0              // Interrupt identifiers
#define INTR_TYPE_LOCAL                 (1 << 0)
#define INTR_TYPE_LOCAL_2               (1 << 1)
#define INTR_TYPE_PCI_ABORT             (1 << 2)
#define INTR_TYPE_DOORBELL              (1 << 3)
#define INTR_TYPE_OUTBOUND_POST         (1 << 4)
#define INTR_TYPE_DMA_0                 (1 << 5)
#define INTR_TYPE_DMA_1                 (1 << 6)
#define INTR_TYPE_SOFTWARE              (1 << 7)


/******************************************************************************
 *
 * Function   :  flip
 *
 * Description:  flip one bit in an array (unsigned int)
 *
 ******************************************************************************/
static inline unsigned flip( unsigned x, unsigned bit )
{
  return x ^ (1UL << bit);
}

/******************************************************************************
 *
 * Function   :  alert_read
 *
 * Description:  Inform read() that there's a frame waiting
 *
 ******************************************************************************/
static void alert_read(driver_t *pdx, int channel) {

 u32 RegValue;
 u8  shift;

 /* Verify DMA channel */
  switch (channel)
    {
    case 0: case 1:
      shift = (channel * 8);
      break;

    default:
      ERROR(("ERROR - Invalid DMA channel,%s %d, channel:%d\n",__FUNCTION__, __LINE__,channel));
      return;
    }

     spin_lock(&(pdx->Lock_HwAccess));

      /* Get DMA Control/Status */
      RegValue = PLX_9000_REG_READ(pdx,PCI8311_DMA_COMMAND_STAT);

      /* Clear DMA interrupt, i.e bit 3 for DMA0 or bit 11 for DMA1 */
      PLX_9000_REG_WRITE(pdx,PCI8311_DMA_COMMAND_STAT,RegValue | ((1 << 3 )<< shift));


      RegValue = PLX_9000_REG_READ(pdx, PCI8311_DMA0_MODE);

  // Check if we have reached the read pointer
  if ((pdx->wp+1 == pdx->rp) || (pdx->wp+1==NO_OF_BUFFERS && !(pdx->rp))) {
     pdx->cntr_circ_full_wp++;
     ERROR(("ERROR function: %s, file: %s line: %d invoked, rp:%d, wp:%d\n", __FUNCTION__, __FILE__, __LINE__,pdx->rp, pdx->wp));
     
     /* We try to avoid a problem with the write pointer here, skip the increment of wp ! */
     goto no_increment_dma;
}

   // Increment and check if we need to wrap
   pdx->wp++;
        if (pdx->wp == NO_OF_BUFFERS)
                pdx->wp = 0;

no_increment_dma:

      PDEBUG ( ("%s, and: %d  frame from DMA:%d: %d\n",__FUNCTION__, __LINE__, channel, plx_diff_since_read(pdx)));
      wake_up_interruptible(&pdx->read_queue);  /* blocked in read() and select() */

  /* Keep track on Completed DMAs and if read() has been noticed */
  pdx->cntr_irq_processed++;

  spin_unlock(&(pdx->Lock_HwAccess));
}

/******************************************************************************
 *
 * Function   :  plx_on_interrupt
 *
 * Description:  The Interrupt Service Routine for the PLX device
 *
 ******************************************************************************/

irqreturn_t plx_on_interrupt(int irq, void *dev_id)
{
  U32               RegValue;
  U32               RegPciInt;
  U32               InterruptSource;
  driver_t          *pdx;
  U32               RegDetectorInt;

  /* Get the device driver data */
  pdx = (driver_t *)dev_id;

  /* Read interrupt status register */
  RegPciInt =    PLX_9000_REG_READ(pdx, PCI8311_INT_CTRL_STAT);

  /****************************************************
   * If the chip is in a low power state, then local
   * register reads are disabled and will always return
   * 0xFFFFFFFF.  If the PLX chip's interrupt is shared
   * with another PCI device, the PXL ISR may continue
   * to be called.  This case is handled to avoid
   * erroneous reporting of an active interrupt.
   ***************************************************/
  if (RegPciInt == 0xFFFFFFFF) {
    pdx->cntr_low_power_state++;
    return IRQ_RETVAL(IRQ_NONE);
  }

#if DMA_DEBUG
 if (!(RegPciInt & (1 << 24))) {
   ERROR ( ("interrupt handler: Direct Master was Bus Master at abort!\n"));
 }

if (!(RegPciInt & (1 << 25))) {
   ERROR ( ("interrupt handler: DMA Channel 0 was Bus Master at abort!\n"));
 }

if (!(RegPciInt & (1 << 26))) {
   ERROR ( ("interrupt handler: DMA Channel 1was Bus Master at abort!\n"));
 }
if (!(RegPciInt & (1 << 27))) {
   ERROR ( ("interrupt handler: PEX 8311 asserted at abort!\n"));
 }

#endif

  /* Check for master PCI interrupt enable */
  if ((RegPciInt & (1 << 8)) == 0) {
     pdx->cntr_pci_master_disabled++;
    return IRQ_RETVAL(IRQ_NONE);
  }


  /* Clear the interrupt type flag */
  InterruptSource = INTR_TYPE_NONE;

 
  /* Check if Local->PCI Interrupt is active and not masked and it originate from the detector */

  RegDetectorInt = COMMAND_LINK_REG_READ(pdx, PCI_MC_CARD_STATUS);

  if (likely((RegPciInt & (1 << 15)) && (RegPciInt & (1 << 11)) &&(RegDetectorInt & (1<<30))))
    {
      InterruptSource |= INTR_TYPE_LOCAL;

      /* Count number of lost ints from a hw perspective */
      if ((RegDetectorInt>>31)&0x1) {
         pdx->cntr_lost_hw_ints++;
      }
      /* Count number of interrupts */
      pdx->cntr_irq_count++;
      PDEBUG(("Got a local interrupt, missed int:%d,from hw:%d,internal count:%d,ip:%d,  last read time:%d\n",(RegDetectorInt>>31)&0x1 ,(RegDetectorInt&0x3),  (pdx->hw_buff_check)%4, pdx->ip, plx_diff_since_read(pdx) ));

      if((RegDetectorInt&0x3) != ((pdx->hw_buff_check)%4)) {
         pdx->hw_buff_check = RegDetectorInt&0x3;
         pdx->cntr_lost_ints++;
      }
      
      pdx->hw_buff_check++;
      

      /* Clear interrupt on PCI_MC */
      RegDetectorInt |= (1<<30);
      COMMAND_LINK_REG_WRITE(pdx,PCI_MC_CARD_STATUS,RegDetectorInt); 

      /* Write a sequence number on first position in frame */
//      plx_write_sram(pdx, DMA_SIZE*(RegDetectorInt&0x3), pdx->seq_cntr++);
      /* Try to write in a position that later will show up in origo on frame */ 
      plx_write_sram(pdx, DMA_SIZE*(RegDetectorInt&0x3) + (1048560 *2), pdx->seq_cntr++);  /* origo? upper left corner? */

      /*Save current hw buffer number in x = 0, y = 1*/
     plx_write_sram(pdx, DMA_SIZE*(RegDetectorInt&0x3) + (1048544 *2), (RegDetectorInt&0x3));

     /* Spare, x =0, and y = 2,3,4,5,6,7 */ 
     plx_write_sram(pdx, DMA_SIZE*(RegDetectorInt&0x3) + (1048528 *2), pdx->ip); 
     plx_write_sram(pdx, DMA_SIZE*(RegDetectorInt&0x3) + (1048512 *2), pdx->wp); 
     plx_write_sram(pdx, DMA_SIZE*(RegDetectorInt&0x3) + (1048496 *2), pdx->rp); 
     plx_write_sram(pdx, DMA_SIZE*(RegDetectorInt&0x3) + (1048480 *2), ((RegDetectorInt>>31)&0x1));    // Lost ints according to hw
     plx_write_sram(pdx, DMA_SIZE*(RegDetectorInt&0x3) + (1048464 *2), pdx->cntr_lost_ints);           // Lost ints according to driver  
     plx_write_sram(pdx, DMA_SIZE*(RegDetectorInt&0x3) + (1048448 *2), 0xebbe);                        // end marker 

     /* Four hardware buffers today, get bit 0 and 1 */
     // FIXME toggle between DMA channel 0 and 1 ! flip( unsigned x, unsigned bit ), protect toggle bit with spinlock or resource atomic lock?
#if DMA1
     pdx->toggle = flip(1,pdx->toggle);
  if((plx_dma_transfer_block(pdx, (RegDetectorInt&0x3), pdx->toggle)) != ApiSuccess) {
#else
      if((plx_dma_transfer_block(pdx, (RegDetectorInt&0x3), 0)) != ApiSuccess) {
#endif
        pdx->cntr_failed_dma++;
	ERROR ( ("interrupt handler: DMA transfer block failed\n"));	
      }

#if STEP_IN_INT
         // Check if we have reached the read pointer
        if ((pdx->ip+1 == pdx->rp) || (pdx->ip+1==NO_OF_BUFFERS && !(pdx->rp))) {
            
            ERROR(("ERROR function: %s, file: %s line: %d invoked, rp:%d, wp:%d, ip:%d\n", __FUNCTION__, __FILE__, __LINE__,pdx->rp, pdx->wp, pdx->ip));


            pdx->cntr_circ_full_ip++;
            /* This will lead frames not taken care of, will be overwritten */
            return IRQ_RETVAL(IRQ_HANDLED);
        }
        // Increment and check if we need to wrap
       pdx->ip++;
        if (pdx->ip == NO_OF_BUFFERS)
                pdx->ip = 0;
#endif

    }

  /* Check if DMA Channel 0 Interrupt is active and not masked */
  if (likely((RegPciInt & (1 << 21)) && (RegPciInt & (1 << 18))))
    {
      /* Verify the DMA interrupt is routed to PCI */
      RegValue =
	PLX_9000_REG_READ(pdx,PCI8311_DMA0_MODE);

      if (likely(RegValue & (1 << 17)))
        {
	  InterruptSource |= INTR_TYPE_DMA_0;
          PDEBUG(("plx_on_interrupt: DMA channel 0 complete, last read:%d\n",plx_diff_since_read(pdx)));

        }
      else {
        ERROR ( ("interrupt handler: bit 17 in DMA_MODE_0 not set!\n"));
      }

    }

  /* Check if DMA Channel 1 Interrupt is active and not masked */
  if ((RegPciInt & (1 << 22)) && (RegPciInt & (1 << 19)))
    {
      /* Verify the DMA interrupt is routed to PCI */
      RegValue = PLX_9000_REG_READ(pdx,PCI8311_DMA1_MODE);

      if (RegValue & (1 << 17))
        {
	  InterruptSource |= INTR_TYPE_DMA_1;
          PDEBUG(("plx_on_interrupt: DMA channel 1 complete, last read:%d\n",plx_diff_since_read(pdx)));
        }
      else {
        ERROR ( ("interrupt handler: bit 17 in DMA_MODE_1 not set!\n"));
      }
    }



  /* Return if no interrupts are active */
  if (InterruptSource == INTR_TYPE_NONE) {

     pdx->cntr_irq_none++;
    return IRQ_RETVAL(IRQ_NONE);
  }


  plx_chip_interrupts_disable(pdx);


  /* Schedule deferred procedure (DPC) to complete interrupt processing */

  /* Provide interrupt source to DPC */
  pdx->Source_Ints = InterruptSource;

  /* Set up our own workqueue, not sharing with the system ... */
  queue_work(pdx->my_workqueue, &(pdx->Task_plx_dpc_for_isr));

  return IRQ_RETVAL(IRQ_HANDLED);
}

/******************************************************************************
 *
 * Function   :  plx_dpc_for_isr
 *
 * Description:  This routine will be triggered by the ISR to service an interrupt
 *
 ******************************************************************************/

void plx_dpc_for_isr(PLX_DPC_PARAM *pArg1)
{
  U32                 RegValue;
  driver_t            *pdx;
  PLX_INTERRUPT_DATA  IntData;


  /* Get the device driver data */
  pdx = container_of( pArg1,driver_t,Task_plx_dpc_for_isr);


  /* Get interrupt source */
  IntData.Source_Ints     = pdx->Source_Ints;
 
  /* Local Interrupt */ 
  if (likely(IntData.Source_Ints & INTR_TYPE_LOCAL))
    {

      PDEBUG ( ("plx_dpc_for_isr: wp:%d, no of microsecs since last read: %d\n",  pdx->wp, plx_diff_since_read(pdx)));
  

   /* Clear local interrupt on PLX */
  {
    PLX_REG_DATA RegData;
 
  /* Setup for synchronized register access */
  RegData.pdx         = pdx;
  RegData.offset      = PCI8311_INT_CTRL_STAT;
  RegData.BitsToSet   = 0;
  RegData.BitsToClear = (1 << 11);

  plx_synchronized_register_modify(&RegData);

  }



    }


  /* DMA Channel 0 interrupt */
  if (likely(IntData.Source_Ints & INTR_TYPE_DMA_0))
    {
      /* Get DMA Control/Status */
      RegValue = PLX_9000_REG_READ(pdx,PCI8311_DMA_COMMAND_STAT);

      /* Clear DMA interrupt */
      PLX_9000_REG_WRITE(pdx,PCI8311_DMA_COMMAND_STAT,RegValue | (1 << 3));


      RegValue = PLX_9000_REG_READ(pdx, PCI8311_DMA0_MODE);

#if STEP_IN_INT_DPC
  // Check if we have reached the read pointer
  if ((pdx->wp+1 == pdx->rp) || (pdx->wp+1==NO_OF_BUFFERS && !(pdx->rp))) {
     pdx->cntr_circ_full_wp++;
     ERROR(("ERROR function: %s, file: %s line: %d invoked, rp:%d, wp:%d\n", __FUNCTION__, __FILE__, __LINE__,pdx->rp, pdx->wp));
     
     /* We try to avoid a problem with the write pointer here, skip the increment of wp ! */
     goto no_increment_dma0;
}

   // Increment and check if we need to wrap
   pdx->wp++;
        if (pdx->wp == NO_OF_BUFFERS)
                pdx->wp = 0;




no_increment_dma0:
#endif

      PDEBUG ( ("plx_dpc_for_isr: let plx_read know that there's data: %d\n", plx_diff_since_read(pdx)));
      wake_up_interruptible(&pdx->read_queue);  /* blocked in read() and select() */

  /* Keep track on Completed DMAs and if read() has been noticed */
  pdx->cntr_irq_processed++;
 
  }

  /* DMA Channel 1 interrupt */
  if (likely(IntData.Source_Ints & INTR_TYPE_DMA_1))
    {
      alert_read(pdx,1);
    }

#if 1
  /* Re-enable interrupts */
  plx_chip_interrupts_enable( pdx );
#endif
}

