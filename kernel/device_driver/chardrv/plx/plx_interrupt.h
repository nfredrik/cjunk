/*
	                     *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : Gemini                                                      **
** filename  : plx_interrupt.h                                             **
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
Description : 

TODO:

*/

#ifndef __PLX_INTERRUPT_H
#define __PLX_INTERRUPT_H


#include <linux/interrupt.h>
#include "plx_driverdefs.h"

/**********************************************
*               Functions
**********************************************/

irqreturn_t plx_on_interrupt(int irq, void *dev_id );
void plx_dpc_for_isr(PLX_DPC_PARAM *pArg1);

#endif
