TARGET = plx_drv
MDIR = drivers/misc
CURRENT = $(shell uname -r)

# Use channel 1 for DMA
# - WITH_INTERRUPT_CHANNEL_1
# use faster CL access
# - DFASTER_CL   
# Enable MSI
# -DENABLE_MSI

# -DSTEP_IN_INT_DPC
# -DSTEP_IN_INT_HANDLER
# -DSTEP_IN_INT_DMA_COMPLETE
# -DFASTER_CL
#  -DDMA_DEBUG 
# -DDMA1
# 
EXTRA_CFLAGS = -DEXPORT_SYMTAB -DPLX_LITTLE_ENDIAN -DPLX_LINUX_DRIVER -DPCI8311 -DPLX_DEBUG  -DSTEP_IN_INT_DPC -DSTEP_IN_INT -DFASTER_CL

KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/kernel/$(MDIR)

plx_drv-objs	:=  plx_interrupt.o plx_driver.o plx_fops.o plx_ioctl.o plx_proc.o
obj-m      := $(TARGET).o

modules:
	make -C $(KDIR) SUBDIRS=$(PWD)  LDDINC=$(PWD)/../include modules


plx_interrupt.o: plx_interrupt.h plx_driver.h

plx_driver.o: plx_interrupt.h plx_driver.h plx_drv.h

plx_fops.o: plx_driver.h

plx_ioctl.o: plx_driver.h plx_drv.h



clean:
	-rm -f *.o *.ko .*.cmd .*.flags *.mod.c

splint:
	splint  -warnposix *.c

-include $(KDIR)/Rules.make
