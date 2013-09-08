/*
*  C Implementation: plx_proc
*
* Description: 
*
*
* Author: Fredrik Svärd <fredrik@mammoth>, (C) 2009
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include "plx_driver.h"

extern driver_t *PL_devp;


int interpret_cmd(char *buf, int len, int data) {

 int i =len;

  if(data&0x1) {
    i += sprintf(buf+i," Finite number of frames ");
  }
  else {
    i += sprintf(buf+i," Infinite number of frames ");
  }

 if((data&0x2) != 0) {
    i += sprintf(buf+i," Trigger each frame ");
  }

 if(data&0x4) {
    i += sprintf(buf+i," Armed ");
  }

 if(data&0x8) {
    i += sprintf(buf+i," Delayed Trigger ");
  }

  i += sprintf(buf+i,"\n");
  return i;  
}

int interpret_setup(char *buf, int len, int data) {

 int i =len;

  if((data&0x3) == 0x0) {
    i += sprintf(buf+i," 3,70mV/fC ");
  }

  if((data&0x3) == 0x1) {
    i += sprintf(buf+i," 125uV/fC ");
  }

  if((data&0x3) == 0x2) {
    i += sprintf(buf+i," 1,25mV/fC ");
  }
  if((data&0x3) == 0x3) {
    i += sprintf(buf+i," 36uV/fC ");
  }


  if(data&0x4) {
    i += sprintf(buf+i," INON ");
  }

  if(data&0x8) {
    i += sprintf(buf+i," TESTON ");
  }

  if(data&0x60) {
    i += sprintf(buf+i," GD-mode:0x%x",(data&0x60)>>4);
  }

 if(data&0x80) {
    i += sprintf(buf+i," A_DRESET ");
  }
 

  i += sprintf(buf+i,"\n");
  return i;  

}

int interpret_status(char *buf, int len, int data) {
 int i =len;

  if(data&0xff) {
    i += sprintf(buf+i," Buffer Number:%x", data&0xff);
  }

 if(data&(0xff<<16)) {
    i += sprintf(buf+i," Error flags:%x", (data&(0xff<<16)) >> 16);
  }

 if(data&(1>>29)) {
    i += sprintf(buf+i," Force Interrupt ");
  }

 if(data&(1>>30)) {
    i += sprintf(buf+i," Interrupt flag ");
  }

 if(data&(1>>31)) {
    i += sprintf(buf+i,"Int overrun ");
  }
 
  i += sprintf(buf+i,"\n");
  return i;  

}
/*
**===========================================================================
** 8.0			plx_read_procmem()
**===========================================================================
** Description:  Read some stuff from the hardware
**
** Parameters:
**
**
** Returns:     none
**
** Globals:
*/

int plx_read_procmem(char *buf, char **start, off_t offset,
                   int count, int *eof, void *data)
{
       int len = 0;
       u32 temp, temp1, temp2, temp3;

       /* CS regs */     
       len += sprintf(buf+len,"\n -- CS registers --\n");
       plx_read_fpga_reg(PL_devp, 0x0, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Test register\n", 0x0, temp);
       plx_read_fpga_reg(PL_devp, 0x4, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Status (Interrupt handling, Error flags, Current Buffer Number) ->", 0x4, temp);
       len = interpret_status(buf, len, temp);

       plx_read_fpga_reg(PL_devp, 0x8, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  R/W Generic Output Port\n", 0x8, temp);
       plx_read_fpga_reg(PL_devp, 0xc, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  RO Generic Input Port\n", 0xc, temp);
       plx_read_fpga_reg(PL_devp, 0x1000, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Frame Count\n", 0x1000, temp);
       plx_read_fpga_reg(PL_devp, 0x1004, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Frame Error Count\n", 0x1004, temp);
       plx_read_fpga_reg(PL_devp, 0x1008, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Sync Lost Count\n", 0x1008, temp);
       plx_read_fpga_reg(PL_devp, 0x100c, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  R/W Incoming sample counter (Write to clear)\n", 0x100c, temp);
       plx_read_fpga_reg(PL_devp, 0x3000, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Detector Command Link\n", 0x3000, temp);
       plx_read_fpga_reg(PL_devp, 0x3004, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  \n", 0x3004, temp);
       plx_read_fpga_reg(PL_devp, 0x3008, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  \n", 0x3008, temp);
       plx_read_fpga_reg(PL_devp, 0x4000, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Temperature\n", 0x4000, temp);
       plx_read_fpga_reg(PL_devp, XX_BOARD_TIMER, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Timer\n", XX_BOARD_TIMER, temp);
       plx_read_fpga_reg(PL_devp, XX_BOARD_FPGA_VERSION, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  FPGA version\n", XX_BOARD_FPGA_VERSION, temp);
       plx_read_fpga_reg(PL_devp, XX_BOARD_SRAM_COUNTER_VAL, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  SRAM Address Counter\n", XX_BOARD_SRAM_COUNTER_VAL, temp);
       plx_read_fpga_reg(PL_devp, 0x5000, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  SelfTest Good Data Counter\n", 0x5000, temp);
       plx_read_fpga_reg(PL_devp, 0x5004, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  SelfTest Bad Data Counter\n", 0x5004, temp);
       plx_read_fpga_reg(PL_devp, 0x5008, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  SelfTest Command\n", 0x5008, temp);

       /* DC regs */
       len += sprintf(buf+len,"\n -- DC registers --\n");
       plx_read_reg(PL_devp, DC_rwtest, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  R/W Test Register\n", DC_rwtest, temp);
       plx_read_reg(PL_devp, DC_echo, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Echo Register\n", DC_echo, temp);
       plx_read_reg(PL_devp, DC_temp, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Temperature\n", DC_temp, temp);
       plx_read_reg(PL_devp, 0x3, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Not used\n", 0x3, temp);
       plx_read_reg(PL_devp, 0x4, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Error Register\n", 0x4, temp);
       plx_read_reg(PL_devp, 0x5, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Reply Channel Select\n", 0x5, temp);
       plx_read_reg(PL_devp, 0x6, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Test Command Register\n", 0x6, temp);
       plx_read_reg(PL_devp, DC_FPGA_version, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  FPGA version\n", DC_FPGA_version, temp);
       plx_read_reg(PL_devp, 0xa, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Frame Size Code  ((RO) see LimitsAndFormats)\n", 0xa, temp);
       plx_read_reg(PL_devp, 0xb, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Frame Size Set (for testing, normally 0x13)\n", 0xb, temp);
       plx_read_reg(PL_devp, 0xc, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Sample Counter N-16\n", 0xc, temp);
       plx_read_reg(PL_devp, 0xd, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Sample Counter 15-8\n", 0xd, temp);
       plx_read_reg(PL_devp, 0xe, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Sample Counter 7-0\n", 0xe, temp);

       /* MB regs */
       len += sprintf(buf+len,"\n -- MB registers --\n");

        plx_read_reg(PL_devp, MB_cmd, &temp);
        len += sprintf(buf+len,"addr:%04x %08x   Command ->", MB_cmd, temp);
        len = interpret_cmd(buf, len, temp);

        plx_read_reg(PL_devp, MB_setup, &temp);
        len += sprintf(buf+len,"addr:%04x %08x  Setup ->", MB_setup, temp);
        len = interpret_setup(buf, len, temp);

       plx_read_reg(PL_devp, MB_fc1, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Frame Count MSB\n", MB_fc1, temp);
       plx_read_reg(PL_devp, MB_fc0, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Frame Count LSB\n", MB_fc0, temp);
       plx_read_reg(PL_devp, MB_it2, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Integration 2\n", MB_it2, temp);
       plx_read_reg(PL_devp, MB_it1, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Integration 1\n", MB_it1, temp);
       plx_read_reg(PL_devp, MB_it0, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Integration 0\n", MB_it0, temp);
       plx_read_reg(PL_devp, MB_err, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Status\n", MB_err, temp);
       plx_read_reg(PL_devp, MB_rt2, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Reset Timer 2\n", MB_rt2, temp);
       plx_read_reg(PL_devp, MB_rt1, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Reset Timer 1\n", MB_rt1, temp);
       plx_read_reg(PL_devp, MB_rt0, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Reset Timer 0\n", MB_rt0, temp);
       plx_read_reg(PL_devp, MB_eco, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Echo\n", MB_eco, temp);
       plx_read_reg(PL_devp, MB_td1, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Trigger Delay MSB\n", MB_td1, temp);
       plx_read_reg(PL_devp, MB_td0, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Trigger Delay LSB\n", MB_td0, temp);
        plx_read_reg(PL_devp, MB_tp0, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Temperature \n", MB_tp0, temp);
       plx_read_reg(PL_devp, MB_tp1, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Not Used \n", MB_tp1, temp);
       plx_read_reg(PL_devp, MB_px1, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Frame Size Code\n", MB_px1, temp);
       plx_read_reg(PL_devp, MB_px0, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Frame Size Set\n", MB_px0, temp);
       plx_read_reg(PL_devp, MB_aec, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  ADC Status\n", MB_aec, temp);
       plx_read_reg(PL_devp, MB_ctrl, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Test Mode Data(Mask)\n", MB_ctrl, temp);
       plx_read_reg(PL_devp, 0x34, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Sent Samples  23-16\n", 0x34, temp);
       plx_read_reg(PL_devp, 0x35, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Sent Samples  15-8\n", 0x35, temp);
       plx_read_reg(PL_devp, 0x36, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Sent Samples  7-0\n", 0x36, temp);
       plx_read_reg(PL_devp, MB_vers, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  FPGA Version\n", MB_vers, temp);
       plx_read_reg(PL_devp, MB_ivt, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Initial Value timer (240ns tick)\n", MB_ivt, temp);
       plx_read_reg(PL_devp, 0x39, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Column Select\n", 0x39, temp);
       plx_read_reg(PL_devp, MB_idt1, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Initial Column Delay timer 1 (30.72us/col) \n", MB_idt1, temp);
       plx_read_reg(PL_devp, MB_idt0, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Initial Column Delay timer 1 (120ns/col)\n", MB_idt0, temp);
       plx_read_reg(PL_devp, 0x3c, &temp);
       len += sprintf(buf+len,"addr:%04x %08x  Frame Measurement timer 2 (n2 * 65536us)\n", 0x3c, temp);
       plx_read_reg(PL_devp, 0x3d, &temp1);
       len += sprintf(buf+len,"addr:%04x %08x  Frame Measurement timer 1 (n1 * 256us)\n", 0x3d, temp1);
       plx_read_reg(PL_devp, 0x3e, &temp2);
       len += sprintf(buf+len,"addr:%04x %08x  Frame Measurement timer 0 (n0 * 1us)\n", 0x3e, temp2);
       plx_read_reg(PL_devp, 0x3f, &temp3);
       len += sprintf(buf+len,"addr:%04x %08x  Misc test \n", 0x3f, temp3);


       /* Compute Frames per millisecond */
       if (temp != 0)
       { int secs, msecs;
          secs  =  1000000/(temp*65536 + temp1*256 + temp2);
          msecs = 1000/((temp*65536 + temp1*256 + temp2) - (secs*1000000)); 
          len += sprintf(buf+len,"\napprox: %dframes/sec\n", secs);
        }


       /* Driver stuff */
       len += sprintf(buf+len,"\n -- Device Driver Stuff --\n");
       len += sprintf(buf+len,"%04d ip\n", PL_devp->ip);
       len += sprintf(buf+len,"%04d wp\n", PL_devp->wp);
       len += sprintf(buf+len,"%04d rp\n", PL_devp->rp);
       len += sprintf(buf+len,"%04d device driver sequence number\n", PL_devp->seq_cntr);              
       len += sprintf(buf+len,"%04d received local interrupts\n", PL_devp->cntr_irq_count);  
       len += sprintf(buf+len,"%04d completed DMAs\n", PL_devp->cntr_irq_processed);  
       len += sprintf(buf+len,"%04d wrong address for interrupt\n", PL_devp->cntr_irq_none);  
      len += sprintf(buf+len, "%04d low power state\n", PL_devp->cntr_low_power_state);  
      len += sprintf(buf+len, "%04d PCI master disabled\n", PL_devp->cntr_pci_master_disabled);  
      len += sprintf(buf+len, "%04d Failed DMAs\n", PL_devp->cntr_failed_dma);  
      len += sprintf(buf+len, "%04d read() invoked\n", PL_devp->cntr_read);  
      len += sprintf(buf+len, "%04d poll() invoked\n", PL_devp->cntr_poll);  
      len += sprintf(buf+len, "%04d frames copied to user space\n", PL_devp->cntr_2_user_space);  
      len += sprintf(buf+len, "%04d too long wait for RDR\n", PL_devp->cntr_tmo_rdr);  
      len += sprintf(buf+len, "%04d too long wait for TBE\n", PL_devp->cntr_tmo_tbe);  
      len += sprintf(buf+len, "%04d number of lost ints acc to driver\n", PL_devp->cntr_lost_ints);  
      len += sprintf(buf+len, "%04d wp has reached rp\n", PL_devp->cntr_circ_full_wp);  
      len += sprintf(buf+len, "%04d Number of lost ints acc to hw\n", PL_devp->cntr_lost_hw_ints);  
      len += sprintf(buf+len, "%04d ip has reached rp\n", PL_devp->cntr_circ_full_ip);  
      len += sprintf(buf+len, "%04d rp equal ip\n", PL_devp->cntr_circ_empty_rp);  


        /* Signal EOF */
        *eof = 1;

        return len;
}







