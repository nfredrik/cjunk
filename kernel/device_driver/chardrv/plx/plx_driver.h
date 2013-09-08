#ifndef _PLX_DRIVER_H
#define _PLX_DRIVER_H
//
// 
//
// Description: 
//
//
// Author: Fredrik Svärd <fredrik@mammoth>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <linux/pci.h>
#include "plx_driverdefs.h"



int plx_dma_open_channel( driver_t *dev, u8 channel);

int plx_dma_close_channel ( driver_t *dev, u8 channel, BOOLEAN bCheckInProgress );

int plx_dma_transfer_block(driver_t *dev, U8 current_hw_buf, U8 ch);


int plx_install_interrupt_handler(driver_t *dev );
int plx_uninstall_interrupt_handler(driver_t *dev);
PLX_STATUS plx_chip_dma_status( driver_t *pdx, U8 channel);
int plx_drv_ioctl(struct inode *inode, struct file *file,
		    unsigned int cmd, unsigned long arg);

int plx_read_fpga_reg(driver_t *p, int adr, int *data);
int plx_write_fpga_reg(driver_t *p, int adr, int data);
int plx_read_reg(driver_t *p, int adr, int *data);
int plx_write_reg(driver_t *p, int adr, int data) ;
int plx_immediate_cmd(driver_t *p, int data);
int plx_diff_since_read(driver_t *p);
PLX_STATUS plx_board_reset(driver_t *pdx);
int plx_read_plx_reg(driver_t *p, int adr, int *data) ;
int plx_write_plx_reg(driver_t *p, int adr, int data);
void print_some_stuff(driver_t *pdx);
int plx_write_sram(driver_t *p, int adr, int data);
BOOLEAN plx_chip_interrupts_enable(driver_t *pdx);
BOOLEAN plx_chip_interrupts_disable(driver_t *pdx);
BOOLEAN plx_synchronized_register_modify( PLX_REG_DATA *pRegData);

int plx_read_procmem(char *buf, char **start, off_t offset,
                   int count, int *eof, void *data);

#endif /* _PLX_DRIVER_H */
