/***************************************************************************
*	  Copyright (c) 2003-2010, Broadcom Corporation
*	  All Rights Reserved
*	  Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: client.h $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 10/15/10 3:09p $
*
* Module Description: Emulator API
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/linuxemu/client/client.h $
* 
* Hydra_Software_Devel/2   10/15/10 3:09p hongtaoz
* SW7425-1, SW7425-10: added MEMC1 support to the emulation framework;
* 
* Hydra_Software_Devel/3   4/28/09 3:17p jessem
* PR 54380: Changed all references to IKOS to EMULATION, EMU or BEMU.
* 
* Hydra_Software_Devel/2   10/16/07 3:24p jessem
* PR 36129: Modified header to include description.
* 
* Hydra_Software_Devel/1   10/16/07 3:08p jessem
* PR 36129: Initial version. API for IKOS register and memory access,
* sokcet connection, and interrupt handler.
* 
***************************************************************************/
#ifndef __BEMU_CLIENT_H__
#define __BEMU_CLIENT_H__

#include "bstd.h"
#include "bint_plat.h"
#include "btst_kni.h"

int BEMU_Client_Open
	( void );
int BEMU_Client_Close
		  ( void );
int BEMU_Client_OpenSocket
	( const char *hostname );
int BEMU_Client_CloseSocket
	( void );
int BEMU_Client_ReadRegister
	( uint32_t reg_addr, 
	  uint32_t *data );
int BEMU_Client_WriteRegister
	( uint32_t reg_addr, 
	  uint32_t data );
int BEMU_Client_ReadMemory
	( uint32_t mem_addr, 
	  void *data, 
	  size_t size );
int BEMU_Client_WriteMemory
	( uint32_t mem_addr, 
	  void *data, 
	  size_t size );
int BEMU_Client_Command
	( uint32_t  ulCmd, 
	  int32_t   ilSize, 
	  int32_t  *pilData );
void BEMU_Client_Interrupt_Monitor
	( BTST_TaskHandle  hTask,
	  void			*pContext );
int BEMU_Client_InitCapture
	( uint32_t uiCmd, 
  	  uint32_t size, 
	  const char *acPath );
int BEMU_Client_StartCapture
	( void );
int BEMU_Client_StopCapture
	( void );
int BEMU_Client_AdvanceClock
	( uint32_t nanoseconds );

#endif /* __BEMU_CLIENT_H__ */	
