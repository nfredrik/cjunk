/*
	                     *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : Gemini                                                      **
** filename  : plx_drv.h                                                   **
** version   : 2                                                           **
** date      : July 29, 2008                                               **
**                                                                         **
*****************************************************************************
**                                                                         **
** Copyright (c) 2008,  C-RAD Imaging                                      **
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
Description : Interface to user space

TODO:

*/

#ifndef __PCI_DRV_H
#define __PCI_DRV_H


/* PCI_MC_Card registers */

#define PCI_MC_CARD_TEST         0x0000
#define PCI_MC_CARD_STATUS       0x0004
#define PCI_MC_CARD_OUTPUT_PORT  0x0008
#define PCI_MC_CARD_INPUT_PORT   0x000c

/* Misc registers ... FIXME: check what to call them, i.e. prefix */

#define XX_INCOMING_SAMPLE_COUNTER    0x100c

#define XX_BOARD_TEMP                 0x4000
#define XX_BOARD_TIMER                0x4004
#define XX_BOARD_FPGA_VERSION         0x4008
#define XX_BOARD_SRAM_COUNTER_VAL     0x400c


/* Command Link Registers */

#define SPIM_DATA    0x3000
#define SPIM_STAT    0x3004
#define SPIM_LW_DATA 0x3008      /* Long word access ? */

/* SPIM STAT */
#define SPIM_STAT_READ_DATA_AVAIL 0x04  
#define SPIM_STAT_RECV_DATA_READY 0x02  
#define SPIM_STAT_TRANSMIT_DATA_EMPTY 0x01

#if FASTER_CL
#define SPIM_STAT_TBUSY           0x08
#define SPIM_STAT_RSD             0x04
#endif


/* SPIM constants */
#define XL_NOP  0x00
#define XL_WC   0x80
#define XL_RC   0xc0
#define XL_EXEC 0x09
#define XL_INIT 0x05
#define XL_STOP 0x0c
/* Module Backplane Registers,  bit5 = 1 */
#define MB_cmd   0x20
#define MB_setup 0x21
#define MB_fc1   0x22
#define MB_fc0   0x23
#define MB_it2   0x24
#define MB_it1   0x25
#define MB_it0   0x26
#define MB_err   0x27
#define MB_rt2   0x28
#define MB_rt1   0x29
#define MB_rt0   0x2a
#define MB_eco   0x2b 
#define MB_td1   0x2c
#define MB_td0   0x2d
#define MB_tp0   0x2e 
#define MB_tp1   0x2f 

#define MB_px1   0x30 
#define MB_px0   0x31  
#define MB_aec   0x32
#define MB_ctrl  0x33
#define MB_vers  0x37
#define MB_ivt   0x38
#define MB_idt1  0x3a
#define MB_idt0  0x3b
/* HVPS */

#define HVPS_DATA     0x2000
#define HVPS_CMD      0x2004
#define HVPS_LW_DATA  0x2008

/* Data Concentrator Registers, bit5 = 0 */

#define DC_rwtest                0x00
#define DC_echo                  0x01  
#define DC_temp                  0x02  
#define DC_error                 0x04
#define DC_reply_module_select   0x05
#define DC_FPGA_version          0x07

#define DC_clear_frame_cntr      0x0f





/* Bit disposition MB_cmd */
#define FINITE_NO_OF_FRAMES     0x01     /* bit 0 */
#define INFINITE_NO_OF_FRAMES   0xfe
#define NO_TRIGGER              0xfe     /* bit 1 : Trigger each frame */
#define TRIGGER_EACH_FRAME      0x02     

#define EXTERNAL_TRIG           0x02




#define DMA_SIZE  (1024 * 1024*2)


#define MAG_NUM			'k'    /* Use 'k' as magic number */ 
#define READ_CONFIG_REG         _IOWR(MAG_NUM, 27, Register)
#define DRIVER_VERSION          _IOR(MAG_NUM, 28, Register)
#define WRITE_FPGA_REG          _IOW(MAG_NUM, 29, Register)
#define READ_FPGA_REG           _IOWR(MAG_NUM, 30, Register)
#define READ_COMMAND_LINK_REG   _IOWR(MAG_NUM, 31, Register)
#define WRITE_COMMAND_LINK_REG	_IOW(MAG_NUM, 32, Register)
#define IMMEDIATE_COMMAND	_IOW(MAG_NUM, 33, long)
#define SET_DETECTOR_VERSION    _IO(MAG_NUM, 34)
#define WRITE_HVPS_CHANNEL	_IO(MAG_NUM, 35)
#define ORDER_HW                _IOWR(MAG_NUM, 36, Container)
#define WRITE_SRAM              _IOW(MAG_NUM, 37, Register)
#define READ_PLX_REG            _IOWR(MAG_NUM, 50, Register)
#define WRITE_PLX_REG           _IOW(MAG_NUM, 51, Register)
#define FILL_PCI_MEM            _IO(MAG_NUM, 52)
#define IOC_MAXNR                            52 /* max implemented */



#define DRIVER_NAME	  "plx_drv"
#define DRIVER_PATH       "/dev/plx_drv"


/* iotcl() */

typedef struct Register {

  unsigned int addr;
  unsigned int data;
} Register;

typedef struct Channel {

  unsigned int channel;
  unsigned int value;
} Channel;

typedef struct  {
  unsigned int order;
  unsigned int start_addr;
  unsigned int array[16];
} Container;


#endif

