
/*
	                     *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : Gemini                                                      **
** filename  : plx_driversdefs.h                                           **
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
Description : Device driver structure

TODO:


*/
#ifndef __DRIVER_DEFS_H
#define __DRIVER_DEFS_H


#ifndef LINUX_VERSION_CODE
#include <linux/version.h>
#endif

#include <linux/cdev.h>
#include <asm/io.h>       /* ioread8() etc */
#include <linux/fs.h>
#include "plx_drv.h"
#include "plx_reg8311.h"
#include <linux/types.h>  

typedef u8                    U8;
typedef u16                   U16;
typedef u32                   U32;
typedef u64                   U64;
typedef unsigned long         PLX_UINT_PTR;
typedef bool                  BOOLEAN;

#define NO_OF_BUFFERS 8

#ifndef PLX_DRV_MAJOR
#define PLX_DRV_MAJOR 0   /* dynamic major by default */
#endif

extern int debug;

/* DMA Command Definitions */
typedef enum _PLX_DMA_COMMAND
{
    DmaPause,
    DmaResume,
    DmaAbort
} PLX_DMA_COMMAND;


/* PCI BAR Properties */
typedef struct _PLX_PCI_BAR_PROP
{
    U32      BarValue;               // Actual value in BAR
    U64      Physical;               // BAR Physical Address
    U64      Size;                   // Size of BAR space
    BOOLEAN  bIoSpace;               // Memory or I/O space?
    BOOLEAN  bPrefetchable;          // Is space pre-fetchable?
    BOOLEAN  b64bit;                 // Is PCI BAR 64-bit?
} PLX_PCI_BAR_PROP;


#define PCI_NUM_BARS_TYPE_00             6              // Total PCI BARs for Type 0 Header

#define NUMBER_OF_DMA_CHANNELS              2                        // Total number of DMA Channels

#define LOCAL_PCI_SRAM_BASE            (0x50000000)
//#define LOCAL_PCI_SRAM_SIZE            (0xfc000000)
#define FPGA_ADDR_BASE                 (0x2000000)


// FIXME: These addresses can be used together with COMMAND_LINK_REG_READ/WRITE
#define GENERAL_IO                     (0x0000)
#define COMMAND_MODULE_HVPS            (0x2000)
#define COMMAND_MODULE_DETECTOR        (0x3000)
#define TEMP_AND_TIMER                 (0x4000)


#define PLX_STATUS_START               0x200   // Starting status code

/* API Return Code Values */
typedef enum _PLX_STATUS
{
    ApiSuccess = PLX_STATUS_START,
    ApiFailed,
    ApiNullParam,
    ApiUnsupportedFunction,
    ApiNoActiveDriver,
    ApiConfigAccessFailed,
    ApiInvalidDeviceInfo,
    ApiInvalidDriverVersion,
    ApiInvalidOffset,
    ApiInvalidData,
    ApiInvalidSize,
    ApiInvalidAddress,
    ApiInvalidAccessType,
    ApiInvalidIndex,
    ApiInvalidPowerState,
    ApiInvalidIopSpace,
    ApiInvalidHandle,
    ApiInvalidPciSpace,
    ApiInvalidBusIndex,
    ApiInsufficientResources,
    ApiWaitTimeout,
    ApiWaitCanceled,
    ApiDmaChannelUnavailable,
    ApiDmaChannelInvalid,
    ApiDmaDone,
    ApiDmaPaused,
    ApiDmaInProgress,
    ApiDmaCommandInvalid,
    ApiDmaInvalidChannelPriority,
    ApiDmaSglPagesGetError,
    ApiDmaSglPagesLockError,
    ApiMuFifoEmpty,
    ApiMuFifoFull,
    ApiPowerDown,
    ApiHSNotSupported,
    ApiVPDNotSupported,
    ApiLastError               // Do not add API errors below this line
} PLX_STATUS;



// Macros to support Kernel-level logging in Debug builds
#if defined(PLX_DEBUG)
    #define ERROR(arg)                _Error_Print_Macro        arg
    #define WARNING(arg)              _Warn_Print_Macro         arg
    #define INFO(arg)                 _Info_Print_Macro        arg
    #define PDEBUG(arg)                _Debug_Print_Macro        arg
    #define PDEBUG_NoInfo(arg)         _Debug_Print_NoInfo_Macro arg
#else
    #define ERROR(arg)                _Error_Print_Macro arg
    #define WARNING(arg)
    #define INFO(arg)
    #define PDEBUG(arg)
    #define PDEBUG_NoInfo(arg)
#endif

#define _Error_Print_Macro(fmt, args...)         printk(KERN_ERR DRIVER_NAME ": " fmt, ## args)
#define _Warning_Print_Macro(fmt, args...)       printk(KERN_WARN DRIVER_NAME ": " fmt, ## args)
#define _Info_Print_Macro(fmt, args...)          printk(KERN_INFO DRIVER_NAME ": " fmt, ## args)
#define _Debug_Print_Macro(fmt, args...)         if (debug) printk(KERN_DEBUG   DRIVER_NAME ": " fmt, ## args)
#define _Debug_Print_NoInfo_Macro(fmt, args...)  printk(KERN_DEBUG   "\b\b\b   \b\b\b" fmt, ## args)


#if 0
// Macros for Memory access to/from user-space addresses
#define DEV_MEM_TO_USER_8( VaUser, VaDev, count)    Plx_dev_mem_to_user_8(  (U8*)(VaUser),  (U8*)(VaDev), (count))
#define DEV_MEM_TO_USER_16(VaUser, VaDev, count)    Plx_dev_mem_to_user_16((U16*)(VaUser), (U16*)(VaDev), (count))
#define DEV_MEM_TO_USER_32(VaUser, VaDev, count)    Plx_dev_mem_to_user_32((U32*)(VaUser), (U32*)(VaDev), (count))
#define USER_TO_DEV_MEM_8( VaDev, VaUser, count)    Plx_user_to_dev_mem_8(  (U8*)(VaDev),   (U8*)(VaUser), (count))
#define USER_TO_DEV_MEM_16(VaDev, VaUser, count)    Plx_user_to_dev_mem_16((U16*)(VaDev),  (U16*)(VaUser), (count))
#define USER_TO_DEV_MEM_32(VaDev, VaUser, count)    Plx_user_to_dev_mem_32((U32*)(VaDev),  (U32*)(VaUser), (count))
#endif


/***********************************************************
 * Macros for device memory access
 *
 * ioreadX() and iowriteX() functions were added to kernel 2.6,
 * but do not seem to be present in all kernel distributions.
 **********************************************************/

#define DEV_MEM_READ_8                          ioread8
#define DEV_MEM_READ_16                         ioread16
#define DEV_MEM_READ_32                         ioread32
#define DEV_MEM_WRITE_8(addr, data)             iowrite8 ((data), (addr))
#define DEV_MEM_WRITE_16(addr, data)            iowrite16((data), (addr))
#define DEV_MEM_WRITE_32(addr, data)            iowrite32((data), (addr))
#define DEV_MEM_WRITE_CHUNK(addr, data, count)  memset_io((addr), (data), (count))




/* Macros for PLX chip register access */
#define PLX_9000_REG_READ(pdx, offset)                   \
    DEV_MEM_READ_32(                                     \
        (U32*)(((U8*)((pdx)->PciBar[0].pVa))  + (offset)) \
        )

#define PLX_9000_REG_WRITE(pdx, offset, value)            \
    DEV_MEM_WRITE_32(                                     \
        (U32*)(((U8*)((pdx)->PciBar[0].pVa)) + (offset)), \
        (value)                                           \
        )



/* Macros for Command Link register access */
// FIXME: Finally cast to void*???

#define GENERAL_IO_READ(pdx,offset)  COMMAND_LINK_REG_READ(pdx, GENERAL_IO+(offset))
#define GENERAL_IO_WRITE(pdx,offset,value)  COMMAND_LINK_REG_WRITE(pdx, GENERAL_IO+(offset), value)


#define COMMAND_LINK_REG_READ(pdx, offset)                   \
    DEV_MEM_READ_32(                                     \
        (U32*)(((U8*)((pdx)->PciBar[2].pVa)) + FPGA_ADDR_BASE + (offset)) \
        )

#define COMMAND_LINK_REG_WRITE(pdx, offset, value)            \
    DEV_MEM_WRITE_32(                                     \
        (U32*)(((U8*)((pdx)->PciBar[2].pVa)) + FPGA_ADDR_BASE +  (offset)), \
        (value)                                           \
        )


#define SRAM_WRITE(pdx, offset, value)            \
    DEV_MEM_WRITE_32(                                     \
        (U32*)(((U8*)((pdx)->PciBar[2].pVa)) + (offset)), \
        (value)                                           \
        )

#define SRAM_READ(pdx, offset)                   \
DEV_MEM_READ_32(                                     \
                (U32*)(((U8*)((pdx)->PciBar[2].pVa)) + (offset)) \
                                                     )  

/* Macros for PCI register access */
#define PLX_PCI_REG_READ(pdx, offset, pValue) \
    pci_read_config_dword(   \
        (pdx)->pPciDevice,   \
        (U16)(offset),       \
        (pValue)             \
        )

#define PLX_PCI_REG_WRITE(pdx, offset, value) \
    pci_write_config_dword(  \
        (pdx)->pPciDevice,   \
        (U16)(offset),       \
        (value)              \
        )


/* Macro for code portability */
#define RtlZeroMemory(pDest, ByteCount)            memset((pDest), 0, (ByteCount))
#define RtlCopyMemory(pDest, pSrc, ByteCount)      memcpy((pDest), (pSrc), (ByteCount))




/* Argument for interrupt source access functions */
typedef struct _PLX_INTERRUPT_DATA
{
    struct _driver_t *pdx;
    U32                       Source_Ints;

} PLX_INTERRUPT_DATA;


/* PCI BAR Space information */
typedef struct _PLX_PCI_BAR_INFO
{
    U32              *pVa;                      // BAR Kernel Virtual Address
    PLX_PCI_BAR_PROP  Properties;               // BAR Properties
    BOOLEAN           bResourceClaimed;         // Was driver able to claim region?
} PLX_PCI_BAR_INFO;




/* Argument for ISR synchronized register access */
typedef struct _PLX_REG_DATA
{
    struct _driver_t *pdx;
    U16                       offset;
    U32                       BitsToSet;
    U32                       BitsToClear;
} PLX_REG_DATA;


/* All relevant information about the device */
typedef struct _driver_t
{

    struct pci_dev        *pPciDevice;                    // Pointer to OS-supplied PCI device information
    BOOLEAN                Flag_started;                  // Keeps track whether device has been started
    BOOLEAN                Flag_Interrupt;                // Keeps track whether device has claimed an IRQ
    U8                     OpenCount;                     // Count of active open descriptors to the device

    
    spinlock_t             Lock_DeviceOpen;               // Spinlock for opening/closing the device

    PLX_PCI_BAR_INFO       PciBar[PCI_NUM_BARS_TYPE_00];

    spinlock_t             Lock_HwAccess;                 // Spinlock used for register access

    spinlock_t             Lock_Isr;                      // Spinlock used to sync with ISR

    struct work_struct     Task_plx_dpc_for_isr;                // Task queue used by ISR to queue DPC
    struct workqueue_struct *my_workqueue;                 // Alternative to shared wrokqueue
    U32                    Source_Ints;                   // Interrupts detected by ISR

 
    struct cdev             cdev;	                  /* Char device structure		*/

    BOOLEAN                dma_channels[NUMBER_OF_DMA_CHANNELS];

    spinlock_t             Lock_DmaChannel;               // Spinlock for DMA channel access

    wait_queue_head_t      read_queue;                    // Wait queue for read()
 
    struct semaphore       read_semph;                    // mutual exclusion semaphore

    u8                hw_buff_check;                     // Shadow variable to compare hw's idea what frame buffer to DMA
    void              *frames[NO_OF_BUFFERS];
    U8                rp, wp, ip;                        // read, write and interrupt pointers to frames[]
    BOOLEAN           toggle;                            // toggle between what DMA engine to use, 0 or 1
    U16               seq_cntr;                          // Count frames
    dma_addr_t        bus_addresses[NO_OF_BUFFERS];


    U32              cntr_irq_count;                              // Keep track on number of interrupts generated, will wrap
    U32              cntr_irq_processed;                          // Counter in workqueue task
    U32              last_read_timer;                            // Not used 
    U32              cntr_dma_not_ready;                         // Not used  
    U32              cntr_irq_none;
    U32              cntr_low_power_state;
    U32              cntr_pci_master_disabled;
    U32              cntr_failed_dma;
    U32              cntr_read;
    U32              cntr_poll;
    U32              cntr_2_user_space;
    U32              cntr_tmo_rdr;
    U32              cntr_tmo_tbe;
    U32              cntr_lost_ints;
    U32              cntr_circ_full_wp;
    U32              cntr_lost_hw_ints;
    U32              cntr_circ_full_ip;
    U32              cntr_circ_empty_rp;
}  driver_t; 



/***********************************************************
 * PLX_DPC_PARAM
 *
 * In kernel 2.6.20, the parameter to a work queue function
 * was made to always be a pointer to the work_struct itself.
 * In previous kernels, this was always a VOID*.  Since
 * PLX drivers use work queue functions for the DPC/bottom-half
 * processing, the parameter had to be changed.  For cleaner
 * source code, the definition PLX_DPC_PARAM is used and is
 * defined below.  This also allows 2.4.x compatible source code.
 **********************************************************/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20))
    #define PLX_DPC_PARAM      VOID
#else
    #define PLX_DPC_PARAM      struct work_struct
#endif



#endif
