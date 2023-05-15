/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: intmgr.c $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 10/10/12 6:35p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/linuxemu/client/intmgr.c $
* 
* Hydra_Software_Devel/2   10/10/12 6:35p vanessah
* SW7445-10: add emu support for 7445
* 
* Hydra_Software_Devel/16   9/4/12 1:39p darnstein
* SW7346-572: roll back recent changes. They broke the check test
* environment.
* 
* Hydra_Software_Devel/15   8/31/12 6:06p darnstein
* SW7346-572: in AppFramework, destroy task before shutting down
* interrupt callback that it might use.
* 
* Hydra_Software_Devel/14   5/22/12 10:58a jgarrett
* SW7425-3091: Reducing idle timeout from 500ms to 10ms to avoid startup
* race conditions.  Matches nexus behavior
* 
* Hydra_Software_Devel/13   10/15/10 3:10p hongtaoz
* SW7425-1, SW7425-10: added MEMC1 support to the emulation framework;
* 
* Hydra_Software_Devel/12   9/28/10 2:33p hongtaoz
* SW7425-10: adding generic interrupt polling support for all PI modules;
* 
* Hydra_Software_Devel/11   9/10/09 1:22p darnstein
* SW7403-891: Assume new Linux usermode driver is used. Use same logic as
* support for 7400 chipset.
* 
* Hydra_Software_Devel/10   8/25/09 10:14a mward
* SW7400-2352: Under default BUILD_SYSTEM=nexus, 7400 uses bcm_driver,
* but is not entirely BINT_NEW_INT_MODEL.
* 
* Hydra_Software_Devel/9   8/13/09 5:45p tdo
* PR55225: Increase support to 96 interrupt bits
* 
* Hydra_Software_Devel/8   7/24/09 6:06p pntruong
* PR55861: Further refactored the new int macro to ease porting of new
* chips.
*
* Hydra_Software_Devel/7   12/3/08 10:40p pntruong
* PR49691: Refactored ifdefs for new interrupt model.
*
* Hydra_Software_Devel/6   9/29/08 4:54p rpan
* PR47411: Enable interrupt support for 3548/3556 for new user mode
* driver.
*
* Hydra_Software_Devel/5   12/3/07 11:04a yuxiaz
* PR37720: Use bcm_driver.h.
*
* Hydra_Software_Devel/4   11/28/07 5:11p yuxiaz
* PR37720: Add 128 bit L1 interrupt support in appframework for 7405,
* 7325 and 7335
*
* Hydra_Software_Devel/3   1/24/07 3:09p maivu
* PR 25962: Changed L1 messages to BDBG_MSG instead of BDBG_WRN, so we
* dont see them by default
*
* Hydra_Software_Devel/2   7/7/06 1:54p maivu
* PR 20495: Modified to support 7411E0 and future revisions
*
* Hydra_Software_Devel/1   6/16/06 4:12p maivu
* PR 22212: Move to linuxuser directory
*
* Hydra_Software_Devel/5   3/10/06 10:52a maivu
* PR 19599: Fixed previous check-in built error with non-97398 platforms
*
* Hydra_Software_Devel/4   3/9/06 5:00p maivu
* PR 19599: Move OS dependent code to prepare for Vxworks support
*
* Hydra_Software_Devel/3   8/2/05 5:25p jasonh
* PR 16397: Clearing out intmgr structure before use.
*
* Hydra_Software_Devel/2   7/27/05 3:51p jasonh
* PR 16397: General clean-up. Added function to enable all registered L1
* interrupts.
*
* Hydra_Software_Devel/9   12/29/04 4:34p marcusk
* PR12333: Reduced new interrupt timeout to 500 msec to improve
* performance of xvd_vdc_test application startup.
*
* Hydra_Software_Devel/8   10/7/04 5:26p pntruong
* PR12728: Fixed -pdantic build warnings and errors.
*
* Hydra_Software_Devel/7   12/2/03 3:38p jasonh
* Fixed previous version's compile issues.
*
* Hydra_Software_Devel/6   12/2/03 3:07p hongtaoz
* Use critical section to protect isr.
*
* Hydra_Software_Devel/5   10/27/03 11:51a aram
* update the intmgr to handle both w0 and w1 interrupts
*
* Hydra_Software_Devel/4   10/22/03 4:37p aram
* first try work with audio test app
*
* Hydra_Software_Devel/3   9/29/03 12:04p aram
* integrated with int1
*
* Hydra_Software_Devel/2   9/25/03 1:22p aram
* integrate to L1 interrupt later
*
* Hydra_Software_Devel/1   9/11/03 5:58p aram
* genesis of these files
***************************************************************************/

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "bstd.h"
#include "intmgr.h"
#include "int1.h"
#include "btst_kni.h"
#include "bkni.h"

#ifndef EMULATION
#if (BINT_NEW_INT_MODEL) || (BCHP_CHIP==7400) || (BCHP_CHIP==7403)
#include "bcm_driver.h"
#else
#include "umdrv.h"
#endif
#endif

#ifdef BCHP_7411_VER
#include "bchp_hif_cpu_intr1.h"
#include "bint_7411.h"
#endif

BDBG_MODULE(INTMGR);

static volatile Int1Control gInterruptControlStruct;

static int              gInitialized = 0;
static volatile int     gStopThread = 0;
static BTST_TaskHandle  gTaskId=NULL;
static int              fd = -1;


#ifdef EMULATION
#include "client.h"
static void intmgr_waitForInterrupts
(
	uint32_t numMsec
	)
{
	BSTD_UNUSED(numMsec);

#ifdef INTMGR_POLL_ADVANCE_CLOCK
	BEMU_Client_AdvanceClock(16000000);
#endif
    /* periodic polling */
	BKNI_Sleep(numMsec);
    
	/* emulation polling will iterate through all ENABLED L1 interrupts */
	gInterruptControlStruct.IntrW0Status = gInterruptControlStruct.IntrW0MaskStatus;
	gInterruptControlStruct.IntrW1Status = gInterruptControlStruct.IntrW1MaskStatus;
	gInterruptControlStruct.IntrW2Status = gInterruptControlStruct.IntrW2MaskStatus;

	/*printf("intmgr_waitForInterrupts()w0=%x w1=%x\n", 
	 gInterruptControlStruct.IntrW0Status,  
	 gInterruptControlStruct.IntrW1Status,
	 gInterruptControlStruct.IntrW2Status); */

	return;
}
#else
#if (BINT_NEW_INT_MODEL)
static void intmgr_waitForInterrupts
(
	uint32_t numMsec
	)
{
	tbcm_linux_dd_interrupt bcm_intr;
	int    err;

	bcm_intr.timeout = numMsec;
	bcm_intr.interruptmask[0] = gInterruptControlStruct.IntrW0MaskStatus;
	bcm_intr.interruptmask[1] = gInterruptControlStruct.IntrW1MaskStatus;
	bcm_intr.interruptmask[2] = gInterruptControlStruct.IntrW2MaskStatus;

	gInterruptControlStruct.IntrW0Status = gInterruptControlStruct.IntrW1Status = gInterruptControlStruct.IntrW2Status = 0;
	bcm_intr.interruptstatus[0] = bcm_intr.interruptstatus[1] = bcm_intr.interruptstatus[2] = 0;

	err = ioctl(fd,BRCM_IOCTL_WAIT_FOR_INTERRUPTS,&bcm_intr);

	if ( err != 0)
	{
		/* We got a timeout error, so zero out the status */
		bcm_intr.interruptstatus[0] = bcm_intr.interruptstatus[1] = bcm_intr.interruptstatus[2] = 0;
		/* printf("intmgr_waitForInterrupts() ioctl to user level driver failed %d\n", err); */

	}
	else
	{
		gInterruptControlStruct.IntrW0Status = bcm_intr.interruptstatus[0];
		gInterruptControlStruct.IntrW1Status = bcm_intr.interruptstatus[1];
		gInterruptControlStruct.IntrW2Status = bcm_intr.interruptstatus[2];
	}

	/* printf("intmgr_waitForInterrupts()w0=%x w1=%x\n", bcm_intr.interruptstatus[0],  bcm_intr.interruptstatus[1]); */

	return;
}
#elif (BCHP_CHIP==7400) || (BCHP_CHIP==7403)
static void intmgr_waitForInterrupts
(
	uint32_t numMsec
	)
{
	tbcm_linux_dd_interrupt bcm_intr;
	int    err;

	bcm_intr.timeout = numMsec;
	bcm_intr.interruptmask[0] = gInterruptControlStruct.IntrW0MaskStatus;
	bcm_intr.interruptmask[1] = gInterruptControlStruct.IntrW1MaskStatus;

	gInterruptControlStruct.IntrW0Status = gInterruptControlStruct.IntrW1Status = 0;
	bcm_intr.interruptstatus[0] = bcm_intr.interruptstatus[1] = 0;

	err = ioctl(fd,BRCM_IOCTL_WAIT_FOR_INTERRUPTS,&bcm_intr);

	if ( err != 0)
	{
		/* We got a timeout error, so zero out the status */
		bcm_intr.interruptstatus[0] = bcm_intr.interruptstatus[1] = 0;
		/* printf("intmgr_waitForInterrupts() ioctl to user level driver failed %d\n", err); */

	}
	else
	{
		gInterruptControlStruct.IntrW0Status = bcm_intr.interruptstatus[0];
		gInterruptControlStruct.IntrW1Status = bcm_intr.interruptstatus[1];
	}

	/* printf("intmgr_waitForInterrupts()w0=%x w1=%x\n", bcm_intr.interruptstatus[0],  bcm_intr.interruptstatus[1]); */

	return;
}
#else 
static void intmgr_waitForInterrupts
(
	uint32_t numMsec
	)
{
	tbcm_linux_dd_interrupt bcm_intr;
	int    err;

	bcm_intr.timeout = numMsec;
	bcm_intr.interruptW0mask = gInterruptControlStruct.IntrW0MaskStatus;
	bcm_intr.interruptW1mask = gInterruptControlStruct.IntrW1MaskStatus;

	gInterruptControlStruct.IntrW0Status = gInterruptControlStruct.IntrW1Status = 0;
	bcm_intr.interruptW0status = bcm_intr.interruptW1status = 0;

	err = ioctl(fd,BRCM_IOCTL_WAIT_FOR_INTERRUPTS,&bcm_intr);

	if ( err != 0)
	{
		/* We got a timeout error, so zero out the status */
		bcm_intr.interruptW0status = bcm_intr.interruptW1status = 0;
		/* printf("intmgr_waitForInterrupts() ioctl to user level driver failed %d\n", err); */

	}
	else
	{
		gInterruptControlStruct.IntrW0Status = bcm_intr.interruptW0status;
		gInterruptControlStruct.IntrW1Status = bcm_intr.interruptW1status;
	}

	/* printf("intmgr_waitForInterrupts()w0=%x w1=%x\n", bcm_intr.interruptW0status,  bcm_intr.interruptW1status); */

	return;
}
#endif
#endif


void
intmgr_interruptTask(BTST_TaskHandle task, void *data)

{
	BSTD_UNUSED(task);
	BSTD_UNUSED(data);

	while(1)
	{
		if( gStopThread )
		{
			/* done */
			break;
		}
		else
		{
			if( gInterruptControlStruct.IntrW0MaskStatus == 0 &&
			    gInterruptControlStruct.IntrW1MaskStatus == 0 &&
			    gInterruptControlStruct.IntrW2MaskStatus == 0 )
			{
				BKNI_Sleep(10);
			}
			else
			{
				/* Wait for the new ints to come in */
				intmgr_waitForInterrupts( 500 );
				if (gStopThread)
				{
					/* done */
					break;
				}

				if( gInterruptControlStruct.IntrW0Status || gInterruptControlStruct.IntrW1Status || gInterruptControlStruct.IntrW2Status )
				{
					/* use critical section to protect isr */
					BKNI_EnterCriticalSection();
					CPUINT1_Isr();
					BKNI_LeaveCriticalSection();
				}
			}
		}
	}

	return;
}

static int intmgr_Start(void)
{
	BERR_Code rc;

	if (gTaskId)
	{
			/* Interrupt manager already running */;
			return -1;
	}

	/* Create the task for interrupt handling */
	rc = BTST_CreateTask( &gTaskId, intmgr_interruptTask, 0 );
	if ( rc != 0 )
	{
		printf("intmgr_Start() BTST_CreateTask failed\n");
		return -1;
	}

	return 0;
}

int intmgr_DebugLevelSet(int level)
{
#ifdef EMULATION
	BSTD_UNUSED(level);
	return 0;
#else
	int ret=-1;
	int debugLevel=level;

	if ( gInitialized )
	{
		ret = ioctl(fd, BRCM_IOCTL_DEBUG_LEVEL, (void *)debugLevel);

		if ( ret != 0)
		{
			printf("intmgr_DebugLevelSet() ioctl  BRCM_IOCTL_DEBUG_LEVEL failed %d\n", ret);
		}
	}
	return ret;
#endif    
}

int intmgr_Init( void )
{

	if ( !gInitialized )
	{
		/* clear structure */
		memset((void *)&gInterruptControlStruct, 0x0, sizeof(Int1Control));

		CPUINT1_SetInt1Control( (Int1Control*)&gInterruptControlStruct );
#ifndef EMULATION
		fd = open("/dev/brcm0",O_RDWR);
		if( fd < 0 )
		{
			printf("Device open on /dev/brcm0 failed: errno %d\n", errno);
			return -1;
		}
#endif
		gInitialized = 1;
	}
	else
	{
		printf("LinuxUserInterruptInit(): Already Initialized!!\n");
	}
	return intmgr_Start();
}

void intmgr_Uninit( void )
{
	if( gInitialized )
		gStopThread = 1;

	BTST_DestroyTask( gTaskId );
	close( fd );
}

/* Enable all Level 1 _isrs that are mapped in the INT module */
#if (BINT_NEW_INT_MODEL)
void intmgr_EnableIsrs( BINT_Handle intHandle )
{
	unsigned long i;
	bool enableIsr;
	uint32_t l1mask[4];

	BINT_GetL1BitMask( intHandle, l1mask);

	for( i=0; i<128; i++ )
	{
		enableIsr = false;

		if(i < 32)
		{
			if( l1mask[0] & 1ul<<i )
			{
				enableIsr = true;
			}
		}
		else if(i < 64)
		{
			if( l1mask[1] & 1ul<<(i-32) )
			{
				enableIsr = true;
			}
		}
		else if(i < 96)
		{
			if( l1mask[2] & 1ul<<(i-64) )
			{
				enableIsr = true;
			}
		}
		else
		{
			if( l1mask[3] & 1ul<<(i-96) )
			{
				enableIsr = true;
			}
		}

		if( enableIsr )
		{
			BDBG_MSG(("Enabling L1 interrupt %ld", i));
			CPUINT1_ConnectIsr(i, (FN_L1_ISR)BINT_Isr, intHandle, i );
			CPUINT1_Enable(i);
		}
	}
}

#else
void intmgr_EnableIsrs( BINT_Handle intHandle )
{
	unsigned long i;
	bool enableIsr;
	uint32_t l1masklo, l1maskhi;

	BINT_GetL1BitMask( intHandle, &l1masklo, &l1maskhi );

	for( i=0; i<64; i++ )
	{
		enableIsr = false;
		if( i >=32 )
		{
			if( l1maskhi & 1ul<<(i-32) )
			{
				enableIsr = true;
			}
		}
		else
		{
			if( l1masklo & 1ul<<i )
			{
				enableIsr = true;
			}
		}
		if( enableIsr )
		{
			BDBG_MSG(("Enabling L1 interrupt %ld", i));
			CPUINT1_ConnectIsr(i, (FN_L1_ISR)BINT_Isr, intHandle, i );
			CPUINT1_Enable(i);
		}
	}
}
#endif

#if (BINT_NEW_INT_MODEL)
void intmgr_DisableIsrs( BINT_Handle intHandle )
{
   unsigned long i;
   bool enableIsr;
   uint32_t l1mask[4];

   BINT_GetL1BitMask( intHandle, l1mask);

   for( i=0; i<128; i++ )
   {
      enableIsr = true;

      if(i < 32)
      {
         if( l1mask[0] & 1ul<<i )
         {
            enableIsr = false;
         }
      }
      else if(i < 64)
      {
         if( l1mask[1] & 1ul<<(i-32) )
         {
            enableIsr = false;
         }
      }
      else if(i < 96)
      {
         if( l1mask[2] & 1ul<<(i-64) )
         {
            enableIsr = false;
         }
      }
      else
      {
         if( l1mask[3] & 1ul<<(i-96) )
         {
            enableIsr = false;
         }
      }

      if( false == enableIsr )
      {
         BDBG_MSG(("Disabling L1 interrupt %ld", i));
         CPUINT1_Disable(i);
         CPUINT1_DisconnectIsr(i);
      }
   }
}

#else
void intmgr_DisableIsrs( BINT_Handle intHandle )
{
   unsigned long i;
   bool enableIsr;
   uint32_t l1masklo, l1maskhi;

   BINT_GetL1BitMask( intHandle, &l1masklo, &l1maskhi );

   for( i=0; i<64; i++ )
   {
      enableIsr = true;
      if( i >=32 )
      {
         if( l1maskhi & 1ul<<(i-32) )
         {
            enableIsr = false;
         }
      }
      else
      {
         if( l1masklo & 1ul<<i )
         {
            enableIsr = false;
         }
      }
      if( false == enableIsr )
      {
         BDBG_MSG(("Disabling L1 interrupt %ld", i));
         CPUINT1_Disable(i);
         CPUINT1_DisconnectIsr(i);
      }
   }
}
#endif
#ifdef BCHP_7411_VER
void intmgr_Enable7411_Isr
(
	BINT_Handle *phInt0,
	BINT_Handle *phInt1
)
{
	CPUINT1_ConnectIsr(BCHP_HIF_CPU_INTR1_INTR_W1_MASK_STATUS_EXT_IRQ_4_CPU_INTR_SHIFT+32,
	                     (FN_L1_ISR)BINT_Isr, *phInt0, BINT_7411_IRQ_0);
	CPUINT1_ConnectIsr(BCHP_HIF_CPU_INTR1_INTR_W1_MASK_STATUS_EXT_IRQ_5_CPU_INTR_SHIFT+32,
	                     (FN_L1_ISR)BINT_Isr,*phInt1, BINT_7411_IRQ_0);
	CPUINT1_Enable(BCHP_HIF_CPU_INTR1_INTR_W1_MASK_STATUS_EXT_IRQ_4_CPU_INTR_SHIFT+32);
	CPUINT1_Enable(BCHP_HIF_CPU_INTR1_INTR_W1_MASK_STATUS_EXT_IRQ_5_CPU_INTR_SHIFT+32);
}
#endif

/* end of file */

