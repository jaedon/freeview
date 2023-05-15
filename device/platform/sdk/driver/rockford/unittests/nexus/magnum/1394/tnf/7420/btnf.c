/***************************************************************************
 *     Copyright (c) 2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnf.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/9/09 5:51p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/magnum/1394/tnf/7420/btnf.c $
 * 
 * Hydra_Software_Devel/1   10/9/09 5:51p rjlewis
 * SW7420-380: Initial release.
 * 
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "bint.h"
#include "bmem.h"

#include "bchp_avlink_5c.h"
#include "bchp_avlink_avrx.h"
#include "bchp_avlink_avtx.h"
#include "bchp_avlink_csr.h"
#include "bchp_avlink_global_cfr.h"
#include "bchp_avlink_host.h"
#include "bchp_avlink_phy.h"
#include "bchp_avlink_bridge.h"
#include "bchp_avlink_debug.h"
#include "bchp_avlink_gr_bridge.h"
#include "bchp_avlink_intr2.h"

#include "bchp_xpt_rmx0.h"
#include "bchp_xpt_rmx0_io.h"
#include "bchp_xpt_rmx1.h"
#include "bchp_xpt_rmx1_io.h"

#include "bchp_hif_cpu_intr1.h"

#include "btnf.h"
#include "btnf_phy.h"

#include "bdbg.h"
BDBG_MODULE(btnf);
BDBG_OBJECT_ID(btnf_device_t);

/* Compile time options */
/*#define ENABLE_INTERRUPTS*/ 
	/* Note: in order to enable interrupts you need to modify two other files: bint_7420.c and interrupt_table.c
	** These files enable the interrupt to be used via the BINT function interface.
	*/
/*#define USE_RMX1*/ /* define to use (test) the second RMX port instead */

/* We need a lock for each type of operation and an event to know when the operation is complete. */
typedef struct {
    BKNI_EventHandle event;
    BKNI_MutexHandle lock;
} BTNF_P_LockContext;

/* This is the data associated with the 1394 module.
** A pointer to one of these (handle) is returned on a successful open call.
*/
struct BTNF_P_Context {
    BDBG_OBJECT(btnf_device_t) /* used to check if structure is valid */

    BCHP_Handle hChip;       /* the chip handle (passed in on open) */
    BREG_Handle hRegister;   /* the register handle (passed in on open) */
    BINT_Handle hInterrupt;  /* the interrupt handle (passed in on open) */

    BMEM_Heap_Handle hHeap; /* hack */

    BINT_CallbackHandle CbHandle; /* interrupt callback */

    BTNF_DefaultSettings DefaultSettings;

    struct {
		void *buffer; /* where the requestor wanted it */
		int length;
		uint8_t *offset; /* the buffer I allocated for the dma */
    } ReceiveData;

	/* Keep two copies of the interrupts:
	** 1) The last interrupt we processed (in case someone needs this).
	** 2) The accumulated interrupts.
	** If you want to know if a particular interrupt occurred then you need to clear the accumulated bit and check it later.
	** Don't depend on being able to catch the 'last' interrupt because a second interrupt can occur and destroy the last contents.
	*/
	struct { uint32_t csr, host, tx, rx; } last, accumulated;

    /* We want to create a number of locks & events for the different I/O operations.
    ** But we con't want to have to reference each item when we need to refer to all of them.
    ** So we create a union with an array that lets us talk to them in a generic manner.
    ** Note: the array dimension of "1" may cause a bounds check error when tested.
    */
    union {
        struct {
            BTNF_P_LockContext IsochTransmit;
            BTNF_P_LockContext IsochReceive;
            BTNF_P_LockContext AsynchTransmit;
            BTNF_P_LockContext AsynchReceive;
        } locks;
        BTNF_P_LockContext eachLock[1]; /* we use 1 here, but want open ended */
    } theLocks;
    /* simple way of finding the number of elements without having to refer to allocated storage */
    #define NumberOfLocks (sizeof(((struct BTNF_P_Context *)0)->theLocks.locks)/sizeof(BTNF_P_LockContext))

#if 0
    /* We need buffers in order to perform I/O operations */
    /* Allocate and map these once so we don't have to do it on every operation */
    struct {
		uint32_t qptr;   /* queue element pointer */
		uint32_t buffer; /* buffer allcoated from system heap */
		uint32_t physical; /* mapped memory address */
		uint32_t size; /* size of the current operation */
    } transmit, reveive;
#endif

	BTNF_RemuxSettings remux;       /* these are the remux values we'll use when initiating a transmit operation */
	BTNF_TransmitSettings transmit; /* these are the transmit values we'll use when initiating a transmit operation */
	BTNF_ReceiveSettings receive;   /* these are the receive values we'll use when initiating a receive operation */
};

#if 0
BERR_Code BKNI_WaitForEvent(BKNI_EventHandle event, int timeoutMsec);
void BKNI_SetEvent(BKNI_EventHandle event);

BERR_Code BKNI_AcquireMutex(BKNI_MutexHandle mutex);
void BKNI_ReleaseMutex(BKNI_MutexHandle mutex);
#endif

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

/*
** This is my ISR function for the 1394 block!
** Its registered and enabled at 'open' time, disabled and de-registered at 'close'.
** Note: this is also used when interrupts are NOT being used to read the current values of the interrupt registers (for status purposes).
*/
static void BTNF_Isr(BTNF_Handle device, int verbose)
{
	uint32_t l2, global_cfr, host, csr, tx, rx;

    if (!device) {
        BDBG_ERR(("device is NULL!"));
        return;
    }

    global_cfr = BREG_Read32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_INT_EVENT_STATUS);
    l2 = BREG_Read32(device->hRegister, BCHP_AVLINK_INTR2_CPU_STATUS);

    /* make a copy of all the interrupts */
    csr  = BREG_Read32(device->hRegister, BCHP_AVLINK_CSR_INT_EVENT_SET);
    host = BREG_Read32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HOST_INT_EVENT_SET);
    tx   = BREG_Read32(device->hRegister, BCHP_AVLINK_AVTX_INT_EVENT_SET);
    rx   = BREG_Read32(device->hRegister, BCHP_AVLINK_AVRX_INT_EVENT_SET);

    /* clear them */
    BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_INT_EVENT_CLR, csr);
    BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HOST_INT_EVENT_CLR, host);
    BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_INT_EVENT_CLR, tx);
    BREG_Write32(device->hRegister, BCHP_AVLINK_AVRX_INT_EVENT_CLR, rx);

#if 0 /* This gets done by BINT code */
	{
    	uint32_t reg; 
    	/* clear top level interrupt too */
    	reg = BREG_Read32(device->hRegister, BCHP_AVLINK_INTR2_CPU_STATUS);
    	reg &= ~(BCHP_MASK(AVLINK_INTR2_CPU_SET, AVLINK_INTR));
    	BREG_Write32(device->hRegister, BCHP_AVLINK_INTR2_CPU_CLEAR, reg);
	}
#endif

	/* Save a copy so anyone can ask what the current interrupt state was at the last interrupt */
	/* We keep two copies -- 1) the accumulated (so they can see if the interrupt has occured) and 2) the last one received. */
	device->accumulated.csr  |= device->last.csr = csr;
	device->accumulated.host |= device->last.host = host;
	device->accumulated.tx   |= device->last.tx = tx;
	device->accumulated.rx   |= device->last.rx = rx;

#if 1
	if (verbose)
	{
    	uint32_t csr_m    = BREG_Read32(device->hRegister, BCHP_AVLINK_CSR_INTR_MASK);
    	uint32_t host_m   = BREG_Read32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HOST_INT_MASK);
    	uint32_t tx_m     = BREG_Read32(device->hRegister, BCHP_AVLINK_AVTX_INT_MASK);
    	uint32_t rx_m     = BREG_Read32(device->hRegister, BCHP_AVLINK_AVRX_INT_MASK);

    	BDBG_ERR(("global_cfr=0x%x (l2=0x%x)", global_cfr, l2));
     	if (csr & ~csr_m)   { BDBG_ERR(("csr  = 0x%x (is=%x, mask=%x)", (csr & ~csr_m), csr, csr_m)); }
     	if (host & ~host_m) { BDBG_ERR(("host = 0x%x (is=%x, mask=%x)", (host & ~host_m), host, host_m)); }
     	if (tx & ~tx_m)     { BDBG_ERR(("tx   = 0x%x (is=%x, mask=%x)", (tx & ~tx_m), tx, tx_m)); }
     	if (rx & ~rx_m)     { BDBG_ERR(("rx   = 0x%x (is=%x, mask=%x)", (rx & ~rx_m), rx, rx_m)); }
    }
#endif
}

/* These functions are used to poll the interrupt atatus (when interrupts are not being used) 
** and to read & clear the status from specific registers.
** If the interrupts are not enabled (use AreInterruptsBeingUsed to determine) then the PollInterrupt Status
** should be called in the waiting loop.  The ReadInterrupt and ClearInterrupt functions should then be used
** to minipulate the accumulated values.
*/
bool BTNF_AreInterruptsBeingUsed(void)
{
#ifdef ENABLE_INTERRUPTS
	return true;
#else
	return false;
#endif
}
void BTNF_ReadInterruptStatus(BTNF_Handle device, BTNF_InterruptRegister reg, uint32_t *value)
{
	if (!value) return; /* don't write to invalid address */

#ifndef ENABLE_INTERRUPTS
	BTNF_Isr(device, false); /* if not using interrupts, call interrupt function to get current values */
#endif

	switch (reg) {
		case csr:  *value = device->accumulated.csr;  break;
		case host: *value = device->accumulated.host; break;
		case tx:   *value = device->accumulated.tx;   break;
		case rx:   *value = device->accumulated.rx;   break;
	}
}
void BTNF_ClearInterruptStatus(BTNF_Handle device, BTNF_InterruptRegister reg, uint32_t mask)
{
	switch (reg) {
		case csr:  device->accumulated.csr  &= ~mask; break;
		case host: device->accumulated.host &= ~mask; break;
		case tx:   device->accumulated.tx   &= ~mask; break;
		case rx:   device->accumulated.rx   &= ~mask; break;
	}
}
void BTNF_PollInterruptStatus(BTNF_Handle device)
{
	BTNF_Isr(device, 0 /*verbose*/);
}

#ifdef ENABLE_INTERRUPTS

#if 0 /* This gets done by BINT code */
static void EnableInterrupts(BTNF_Handle device)
{
    uint32_t reg;
    /* enable the top level interrupt */
    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_INTR2_CPU_MASK_STATUS);
    reg |= BCHP_FIELD_DATA(AVLINK_INTR2_CPU_MASK_STATUS, AVLINK_INTR, 1);
    BREG_Write32(device->hRegister, BCHP_AVLINK_INTR2_CPU_MASK_STATUS, reg);
}
static void DisableInterrupts(BTNF_Handle device)
{
    uint32_t reg;
    /* disable the top level interrupt */
    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_INTR2_CPU_MASK_STATUS);
    reg &= ~(BCHP_MASK(AVLINK_INTR2_CPU_MASK_STATUS, AVLINK_INTR));
    BREG_Write32(device->hRegister, BCHP_AVLINK_INTR2_CPU_MASK_STATUS, reg);
}
#endif

static void DisableAllInterrupts(BTNF_Handle device)
{
    uint32_t reg = 0xffffffff; /* mask everything */

#if 0 /* This gets done by BINT code */
    DisableInterrupts(device); /* first disable the top level interrupts */
#endif

    BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HOST_INT_MASK, reg);
    BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_INTR_MASK, reg);
    BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_INT_MASK, reg);
    BREG_Write32(device->hRegister, BCHP_AVLINK_AVRX_INT_MASK, reg);

#if 0
    BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HOST_NMI_MASK, reg);
    BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_NMI_MASK, reg);
    BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_NMI_MASK, reg);
    BREG_Write32(device->hRegister, BCHP_AVLINK_AVRX_NMI_MASK, reg);
#endif
}

#endif

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

typedef enum { ReceiveRegisters, TransmitRegisters } RegisterSet;

static void SelectRegisters(BTNF_Handle device, RegisterSet which)
{
#if (BCHP_VER == BCHP_VER_A0 || BCHP_VER == BCHP_VER_A1)
    uint32_t reg = BREG_Read32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_AV_PATH_SEL);
	if (which == ReceiveRegisters)
    	reg |= (BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_AV_PATH_SEL, av_Rx_Sel, 1)); /*1=receive*/
	else
    	reg &= ~(BCHP_MASK(AVLINK_GLOBAL_CFR_AV_PATH_SEL, av_Rx_Sel));         /*0=transmit*/
    BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_AV_PATH_SEL, reg);
#else
    uint32_t reg = BREG_Read32(device->hRegister, BCHP_AVLINK_HOST_AV_PATH_SEL);
	if (which == ReceiveRegisters)
    	reg |= (BCHP_FIELD_DATA(AVLINK_HOST_AV_PATH_SEL, av_Rx_Sel, 1)); /*1=receive*/
	else
    	reg &= ~(BCHP_MASK(AVLINK_HOST_AV_PATH_SEL, av_Rx_Sel));         /*0=transmit*/
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_AV_PATH_SEL, reg);
#endif
}

typedef enum { flushAll, av_Rx_Buf_Flush, av_Tx_Buf_Flush, ir_FIFO_Flush, it_FIFO_Flush, ar_FIFO_Flush, at_Req_FIFO_Flush, at_Resp_FIFO_Flush } BTNF_FIFO;

/* Flush a particular FIFO or all of them */
static void FlushFIFOs(BTNF_Handle device, BTNF_FIFO which)
{
    uint32_t reg = 0;

    switch (which) 
    {
	case av_Rx_Buf_Flush:    reg |= BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_FIFO_FLUSH, av_Rx_Buf_Flush, 1);    break;
	case av_Tx_Buf_Flush:    reg |= BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_FIFO_FLUSH, av_Tx_Buf_Flush, 1);    break;
	case ir_FIFO_Flush:      reg |= BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_FIFO_FLUSH, ir_FIFO_Flush, 1);      break;
	case it_FIFO_Flush:      reg |= BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_FIFO_FLUSH, it_FIFO_Flush, 1);      break;
	case ar_FIFO_Flush:      reg |= BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_FIFO_FLUSH, ar_FIFO_Flush, 1);      break;
	case at_Req_FIFO_Flush:  reg |= BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_FIFO_FLUSH, at_Req_FIFO_Flush, 1);  break;
	case at_Resp_FIFO_Flush: reg |= BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_FIFO_FLUSH, at_Resp_FIFO_Flush, 1); break;
	default: 
		reg = 0xffffffff; /* clear everything */
    }
    BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_FIFO_FLUSH, reg);
}

/* This performs some initial setup that lets the PHY become a valid entity on the bus */
static void DoSetup(BTNF_Handle device)
{
    uint32_t reg;

    /* Do a soft reset */
    reg = BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_HC_CONTROL_SET, soft_Reset, 1);
    BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HC_CONTROL_SET, reg);
    BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HC_CONTROL_CLR, reg);

    reg = (
		BCHP_FIELD_DATA(AVLINK_CSR_LINK_CTRL_SET, cycle_Master, 1) |
		BCHP_FIELD_DATA(AVLINK_CSR_LINK_CTRL_SET, cycle_Timer_Enable, 1) |
		BCHP_FIELD_DATA(AVLINK_CSR_LINK_CTRL_SET, rcv_Self_ID, 1) |
		BCHP_FIELD_DATA(AVLINK_CSR_LINK_CTRL_SET, phy_AMode, 1)
    );
    BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_LINK_CTRL_SET, reg);

    reg = BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_HC_CONTROL_SET, link_Enable, 1);
    BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HC_CONTROL_SET, reg);

    /* Bcm7420.AVLINK.AVLINK_BRIDGE.AVLINK_BRIDGE_CTRL.BR_SEQ_EN = 0 */
    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_BRIDGE_CTRL);
    reg &= ~(BCHP_MASK(AVLINK_BRIDGE_CTRL, BR_SEQ_EN));
    BREG_Write32(device->hRegister, BCHP_AVLINK_BRIDGE_CTRL, reg);

    /* Make sure transmit is disabled */
    reg = 0; BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_CTRL, reg);

#if 0 /* Toontun says I don't have to do this... */
	/* B0 part includes these two new registers that enable fixes -- make sure they are set to (what should be) the default vaules */
    reg = 0x3fff3fff; BREG_Write32(device->hRegister, BCHP_AVLINK_5C_LOGIC_CONFIG, reg);
    reg = 0x800003ff; BREG_Write32(device->hRegister, BCHP_AVLINK_5C_LOGIC_CONFIG2, reg);
#endif
}

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

/* This is used to cleanup context if we fail any part of the open or on close */
static void CleanupContext(BTNF_Handle device)
{
    unsigned int i;
    for (i=0; i<NumberOfLocks; i++)
    {
        if (device->theLocks.eachLock[i].event) BKNI_DestroyEvent(device->theLocks.eachLock[i].event);
        if (device->theLocks.eachLock[i].lock)  BKNI_DestroyMutex(device->theLocks.eachLock[i].lock);
    }

#if 0
    if (device->transmit.qptr) BMEM_Free(device->hHeap, device->transmit.qptr);
    if (device->receive.qptr)  BMEM_Free(device->hHeap, device->receive.qptr);
#endif

    /* Mark it invalid in case he tries to give it to me again (close twice) */
    /* Note: this probably won't happen as the memory will be reassigned and stepped on, but ... */
    BDBG_OBJECT_DESTROY(device, btnf_device_t);
    BKNI_Free(device);
}

static BTNF_RemuxSettings defaultRemux = { 
	3,     /*rate - the clock rate for the remux (3=20.25)*/
	false, /*null_packet_disable*/
	false, /*bypass_enable*/
	0x10,  /*null_packet_threshold*/
	true,  /*nrun_enable*/
	0      /*packet_delay_count*/
};

static BTNF_TransmitSettings defaultTransmit = {
	true,   /*useVariableRate*/
	5,      /*packetLength*/
	0x3000  /*futureTimeStamp*/
};

static BTNF_ReceiveSettings defaultReceive = {
	true /*jitterCompensationDisable*/
};

BERR_Code BTNF_Open(BTNF_Handle *phHandle,
                    BCHP_Handle hChip, BREG_Handle hRegister, BINT_Handle hInterrupt,
                    const BTNF_DefaultSettings *pDefaultSettings)
{
    unsigned int i;
    BERR_Code retCode = BERR_SUCCESS;
    BTNF_Handle device;

#ifdef ENABLE_INTERRUPTS
	BINT_Id intId = BCHP_INT_ID_CREATE(BCHP_AVLINK_INTR2_CPU_STATUS, BCHP_AVLINK_INTR2_CPU_STATUS_AVLINK_INTR_SHIFT);
#endif

    /* Sanity check on the handles we've been given */
    BDBG_ENTER(BTNF_Open);
    BDBG_ASSERT(phHandle);
    BDBG_ASSERT(hRegister);
    BDBG_ASSERT(hInterrupt);

    /* Note: They don't have to give me the settings, I'll just use the default */

    /* Some say I shouldn't mess with the return variable unless I return success... */
    *phHandle = NULL; /* just to be sure caller doesn't use it */

    device = (BTNF_Handle) BKNI_Malloc(sizeof(*device));
    if (device == NULL)
    {
        BDBG_ERR(("BTNF_Open: BKNI_malloc() failed"));
        retCode =  BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto done;
    }

    BKNI_Memset(device, 0, sizeof(*device));
    BDBG_OBJECT_SET(device, btnf_device_t);

    device->hChip      = hChip;
    device->hRegister  = hRegister;
    device->hInterrupt = hInterrupt;

    if (pDefaultSettings)
        BKNI_Memcpy(&device->DefaultSettings, pDefaultSettings, sizeof(BTNF_DefaultSettings));

	/* use the defaults until they decide to change them */
	BKNI_Memcpy((char*)&device->remux,    (char*)&defaultRemux,    sizeof(BTNF_RemuxSettings));
	BKNI_Memcpy((char*)&device->transmit, (char*)&defaultTransmit, sizeof(BTNF_TransmitSettings));
	BKNI_Memcpy((char*)&device->receive,  (char*)&defaultReceive,  sizeof(BTNF_ReceiveSettings));

#ifdef ENABLE_INTERRUPTS
    DisableAllInterrupts(device); /* before enabling any interrupts, disable everything */
    retCode = BINT_CreateCallback(&device->CbHandle, hInterrupt, intId,
			      (BINT_CallbackFunc)BTNF_Isr, (void *)device, 1 /*verbose*/);
    if (retCode != BERR_SUCCESS) {
        retCode = BERR_TRACE(retCode);
        BDBG_ERR(("BTNF_Open: BINT_CreateCallback() failed"));
		goto error;
    }
    retCode = BINT_EnableCallback(device->CbHandle);
    if (retCode != BERR_SUCCESS) {
        BDBG_ERR(("BTNF_Open: BINT_EnableCallback() failed"));
        goto error;
    }
#endif

    /* We need a lock and an event for each of the operations we perform */
    for (i=0; i<NumberOfLocks; i++)
    {
        if ((retCode = BKNI_CreateEvent(&device->theLocks.eachLock[i].event))) goto error;
        if ((retCode = BKNI_CreateMutex(&device->theLocks.eachLock[i].lock)))  goto error;
    }

    DoSetup(device);

    *phHandle = device; /* here's your handle! */
    goto done;

  error:
    CleanupContext(device);

  done:
    BDBG_LEAVE(BTNF_Open);
    return (retCode);
}

#if 0
/* We need mapped memory for DMA operations */
static BERR_Code initQPtrs(BTNF_Handle device)
{
    device->transmit.qptr = BMEM_AllocAligned(device->hHeap, 5, 4, 0);
    if (device->transmit.qptr == NULL)
    {
        BDBG_ERR(("BMEM_Alloc() failed to allocate transmit queue pointer!"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    device->receive.qptr = BMEM_AllocAligned(device->hHeap, 5, 4, 0);
    if (device->receive.qptr == NULL)
    {
        BDBG_ERR(("BMEM_Alloc() failed to allocate receive queue pointer!"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    return BERR_SUCCESS;
}
#endif

BERR_Code BTNF_SetMem(BTNF_Handle device, BMEM_Heap_Handle hHeap)
{
    BDBG_ASSERT(device);

    /* Q: Is there a way to validate the heap handle? */
    /*BDBG_ERR(("Setting mem heap handle to %x\n", hHeap));*/
    device->hHeap = hHeap;

#if 0
    /* Now that we have a heap, allocate the queue header pointers we'll need later */
    /* TODO: figure out how to integrate this into the open -- do we need to require a heap handle as part of the open?? */
    initQPtrs(device); 
#endif

    return BERR_SUCCESS;
}

BERR_Code BTNF_Close(BTNF_Handle device)
{
    /* Sanity check on the handle we've been given */
    BDBG_ENTER(BTNF_Close);
    BDBG_ASSERT(device);
    BDBG_OBJECT_ASSERT(device, btnf_device_t);

#ifdef ENABLE_INTERRUPTSS
    DisableAllInterrupts(device);
    BINT_DisableCallback(device->CbHandle); /* should I care if it failed? */
    BINT_DestroyCallback(device->CbHandle); /* should I care if it failed? */
#endif

    CleanupContext(device);

    BDBG_LEAVE(BTNF_Close);
    return BERR_SUCCESS;
}

static const BTNF_DefaultSettings DefaultSettings = { 0 };

BERR_Code BTNF_GetDefaultSettings(BTNF_DefaultSettings *pSettings)
{
    /* Sanity check on the handle we've been given */
    BDBG_ASSERT(pSettings);

#if 0
    /* I don't have any default settings.  Don't know what to return here.  I'm just returning all zeros! */
    BKNI_Memset(pSettings, 0, sizeof(BTNF_DefaultSettings));
#else
    /* Just return a copy of the default */
    BKNI_Memcpy(pSettings, &DefaultSettings, sizeof(BTNF_DefaultSettings));
#endif

    return BERR_SUCCESS;
}

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

/* TODO: need to use one of the (hardware?) timers in order to wait for a PHY operation to complete
** but not wait forever -- how do I use another PI from within this PI??
*/

BERR_Code BTNF_ReadPhyRegister(BTNF_Handle device, uint8_t address, uint8_t *data)
{
    uint32_t reg;

    BDBG_ASSERT(device);
    BDBG_ASSERT(data); /* I have to put the data somewhere */

#if 0
    /* Should I make sure this is clear before starting??? */
    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_CSR_PHY_CTRL);
    while (reg & BCHP_MASK(AVLINK_CSR_PHY_CTRL, phy_Reg_Rd_Done)) {
        /* need to wait a bit for command to be done??? */
        reg = BREG_Read32(device->hRegister, BCHP_AVLINK_CSR_PHY_CTRL);
        /* break on timeout -- return error! */
    }
#endif

    reg = (
		BCHP_FIELD_DATA(AVLINK_CSR_PHY_CTRL, rd_Phy_Reg, 1) |
		BCHP_FIELD_DATA(AVLINK_CSR_PHY_CTRL, phy_Reg_Rd_Wr_Addr, address)
    );
    BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_PHY_CTRL, reg);

    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_CSR_PHY_CTRL);
    /* Wait for the read done bit to be set */
    while (!(reg & BCHP_MASK(AVLINK_CSR_PHY_CTRL, phy_Reg_Rd_Done))) {
        /* need to wait a bit for command to be done??? */
        reg = BREG_Read32(device->hRegister, BCHP_AVLINK_CSR_PHY_CTRL);
        /* break on timeout -- return error! */
    }

    *data = (uint8_t)BCHP_GET_FIELD_DATA(reg, AVLINK_CSR_PHY_CTRL, phy_Reg_Rd_Data);

    return BERR_SUCCESS;
}

BERR_Code BTNF_WritePhyRegister(BTNF_Handle device, uint8_t address, uint8_t data)
{
    uint32_t reg;

    BDBG_ASSERT(device);

    reg = (
		BCHP_FIELD_DATA(AVLINK_CSR_PHY_CTRL, wr_Phy_Reg, 1) |
		BCHP_FIELD_DATA(AVLINK_CSR_PHY_CTRL, phy_Reg_Rd_Wr_Addr, address) |
		BCHP_FIELD_DATA(AVLINK_CSR_PHY_CTRL, phy_Reg_Wr_data, data)
    );
    BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_PHY_CTRL, reg);

    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_CSR_PHY_CTRL);
    /* Wait for the write register bit to go away */
    while (reg & BCHP_MASK(AVLINK_CSR_PHY_CTRL, wr_Phy_Reg)) {
        /* need to wait a bit for command to be done??? */
        reg = BREG_Read32(device->hRegister, BCHP_AVLINK_CSR_PHY_CTRL);
        /* break on timeout -- return error! */
    }

    return BERR_SUCCESS;
}

BERR_Code BTNF_IssueShortBusReset(BTNF_Handle device)
{
    uint8_t reg;
    BDBG_ASSERT(device);
    if (BTNF_ReadPhyRegister(device, 5, &reg)) 
        return BERR_TRACE(BTNF_ERR_PHY_TIMEOUT);
    reg |= PHY_INITIATE_BUS_RESET; 
    BTNF_WritePhyRegister(device, 5, reg);
    return BERR_SUCCESS;
}
BERR_Code BTNF_IssueLongBusReset(BTNF_Handle device)
{
    uint8_t reg;
    BDBG_ASSERT(device);
    if (BTNF_ReadPhyRegister(device, 1, &reg)) 
        return BERR_TRACE(BTNF_ERR_PHY_TIMEOUT);
    reg |= PHY_INITIATE_BUS_RESET; 
    BTNF_WritePhyRegister(device, 1, reg); /* this self clears */
    return BERR_SUCCESS;
}

BERR_Code BTNF_ForceRoot(BTNF_Handle device)
{
    uint8_t reg;
    BDBG_ASSERT(device);
    if (BTNF_ReadPhyRegister(device, 1, &reg)) 
        return BERR_TRACE(BTNF_ERR_PHY_TIMEOUT);
    reg |= PHY_ROOT_HOLD_OFF_BIT; 
    BTNF_WritePhyRegister(device, 1, reg);
    return BERR_SUCCESS;
}
BERR_Code BTNF_DontForceRoot(BTNF_Handle device)
{
    uint8_t reg;
    BDBG_ASSERT(device);
    if (BTNF_ReadPhyRegister(device, 1, &reg)) 
        return BERR_TRACE(BTNF_ERR_PHY_TIMEOUT);
    reg &= ~PHY_ROOT_HOLD_OFF_BIT; 
    BTNF_WritePhyRegister(device, 1, reg);
    return BERR_SUCCESS;
}

BERR_Code BTNF_EnableContender(BTNF_Handle device)
{
    uint8_t reg;
    BDBG_ASSERT(device);
    if (BTNF_ReadPhyRegister(device, 5, &reg)) 
        return BERR_TRACE(BTNF_ERR_PHY_TIMEOUT);
    reg |= PHY_CONTENDER_ENABLE; 
    BTNF_WritePhyRegister(device, 5, reg);
    return BERR_SUCCESS;
}
BERR_Code BTNF_DisableContender(BTNF_Handle device)
{
    uint8_t reg;
    BDBG_ASSERT(device);
    if (BTNF_ReadPhyRegister(device, 5, &reg)) 
        return BERR_TRACE(BTNF_ERR_PHY_TIMEOUT);
    reg &= ~PHY_CONTENDER_ENABLE; 
    BTNF_WritePhyRegister(device, 5, reg);
    return BERR_SUCCESS;
}

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

BERR_Code BTNF_GetGeneration(BTNF_Handle device, int *generation)
{
    uint32_t reg;
    BDBG_ASSERT(device);
    BDBG_ASSERT(generation); /* need a place to put the data */

    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_CSR_SELF_ID_COUNT);
    if (!BCHP_GET_FIELD_DATA(reg, AVLINK_CSR_SELF_ID_COUNT, Self_ID_Error))
        return BERR_TRACE(BTNF_ERR_NOT_READY);

    *generation = (int)BCHP_GET_FIELD_DATA(reg, AVLINK_CSR_SELF_ID_COUNT, Self_ID_Generation);

    return BERR_SUCCESS;
}

BERR_Code BTNF_GetNodeID(BTNF_Handle device, uint16_t *node)
{
    uint32_t reg;
    BDBG_ASSERT(device);
    BDBG_ASSERT(node); /* need a place to put the data */

    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_CSR_NODE_ID);
    if (!BCHP_GET_FIELD_DATA(reg, AVLINK_CSR_NODE_ID, ID_Valid)) 
        return BERR_TRACE(BTNF_ERR_NOT_READY);

    *node = (uint16_t) (
	BCHP_GET_FIELD_DATA(reg, AVLINK_CSR_NODE_ID, bus_Number) << BCHP_SHIFT(AVLINK_CSR_NODE_ID, bus_Number) |
	BCHP_GET_FIELD_DATA(reg, AVLINK_CSR_NODE_ID, node_Number)
    );

    return BERR_SUCCESS;
}

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

#define Build_Get_Set(type, variable) \
	BERR_Code BTNF_Get##type##Settings(BTNF_Handle device, BTNF_##type##Settings *settings) \
	{ \
		if (!settings) return BERR_TRACE(BTNF_ERR_INVALID_PARAMETER); \
		BKNI_Memcpy((char*)settings, (char*)&device-> variable, sizeof(BTNF_##type##Settings)); \
    	return BERR_SUCCESS; \
	} \
	BERR_Code BTNF_Set##type##Settings(BTNF_Handle device, BTNF_##type##Settings *settings) \
	{ \
		if (!settings) return BERR_TRACE(BTNF_ERR_INVALID_PARAMETER); \
		BKNI_Memcpy((char*)&device-> variable, (char*)settings, sizeof(BTNF_##type##Settings)); \
    	return BERR_SUCCESS; \
	}

/* These functions are identical in function so make this easier to read. */
Build_Get_Set(Remux, remux)
Build_Get_Set(Transmit, transmit)
Build_Get_Set(Receive, receive)

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

/* Note: RDB is lacking the definitions for the RMX1 bit definitions.  They are the same as RMX0, so fake it */
#ifdef USE_RMX1
#define BCHP_XPT_RMX_IO_FORMAT BCHP_XPT_RMX1_IO_FORMAT
#define BCHP_XPT_RMX_CTRL      BCHP_XPT_RMX1_CTRL
#define BCHP_XPT_RMX_IO_PKT_DLY_CNT BCHP_XPT_RMX1_IO_PKT_DLY_CNT
#else
#define BCHP_XPT_RMX_IO_FORMAT BCHP_XPT_RMX0_IO_FORMAT
#define BCHP_XPT_RMX_CTRL      BCHP_XPT_RMX0_CTRL
#define BCHP_XPT_RMX_IO_PKT_DLY_CNT BCHP_XPT_RMX0_IO_PKT_DLY_CNT
#endif

#define MODIFY_REMUX /* enable this to modify remux without using PI (let app do this) */

static void EnableRemux(BTNF_Handle device)
{
#ifndef MODIFY_REMUX
    BSTD_UNUSED(device);
#else
    uint32_t reg;

	/* theory is that we need to set (write) the clock select register before setting the format */
    reg = 0;
    BREG_Write32(device->hRegister, BCHP_XPT_RMX_CTRL, reg);

    /* Set RMX CLK0 to be free running */
    reg = 0;
#if 0
    reg = BREG_Read32(device->hRegister, BCHP_XPT_RMX_IO_FORMAT);
    reg &= ~(BCHP_MASK(XPT_RMX0_IO_FORMAT, RMXP_ONLY_RATE));
#endif
    reg |= (BCHP_FIELD_DATA(XPT_RMX0_IO_FORMAT, RMXP_ONLY_RATE, 3));
	if (device->remux.nrun_enable)
    	reg |= (BCHP_FIELD_DATA(XPT_RMX0_IO_FORMAT, RMX_CLK_NRUN_EN, 1));
    BREG_Write32(device->hRegister, BCHP_XPT_RMX_IO_FORMAT, reg);

    reg = device->remux.packet_delay_count;
    BREG_Write32(device->hRegister, BCHP_XPT_RMX_IO_PKT_DLY_CNT, reg);

    /* Re-enabled the RMX */
    reg = 0;
#if 0
    reg = BREG_Read32(device->hRegister, BCHP_XPT_RMX_CTRL);
    reg &= ~(
        BCHP_MASK(XPT_RMX0_CTRL, RMX_NULL_PKT_THRESHOLD) |
        BCHP_MASK(XPT_RMX0_CTRL, RMX_PKT_LENGTH) |
        BCHP_MASK(XPT_RMX0_CTRL, RMX_CLK_SEL)
    );
#endif
    reg |= (
        BCHP_FIELD_DATA(XPT_RMX0_CTRL, RMX_NULL_PKT_THRESHOLD, device->remux.null_packet_threshold) |
        BCHP_FIELD_DATA(XPT_RMX0_CTRL, RMX_PKT_LENGTH, 188) | /* MPEG packet length */
        BCHP_FIELD_DATA(XPT_RMX0_CTRL, RMX_ENABLE, 1)
    );
	if (device->remux.bypass_enable)
    	reg |= BCHP_FIELD_DATA(XPT_RMX0_CTRL, RMX_BYPASS, 1);
	if (device->remux.null_packet_disable)
    	reg |= BCHP_FIELD_DATA(XPT_RMX0_CTRL, RMX_NULL_PKT_DIS, 1);
    reg |= BCHP_FIELD_DATA(XPT_RMX0_CTRL, RMX_CLK_SEL, device->remux.rate);
    BREG_Write32(device->hRegister, BCHP_XPT_RMX_CTRL, reg);
#endif
}


static void DisableRemux(BTNF_Handle device)
{
#ifndef MODIFY_REMUX
    BSTD_UNUSED(device);
#else
	uint32_t reg;

    /* Disable RMX */
    reg = BREG_Read32(device->hRegister, BCHP_XPT_RMX_CTRL);
    reg &= ~(BCHP_MASK(XPT_RMX0_CTRL, RMX_ENABLE));
    BREG_Write32(device->hRegister, BCHP_XPT_RMX_CTRL, reg);
#endif
}

/********************************************************************************************
**********************      Isoch Transmit Operations     ***********************************
*********************************************************************************************/

BERR_Code BTNF_EnableTransmit(BTNF_Handle device, int speed, int channel, uint8_t sync)
{
    BERR_Code err;
    uint32_t reg;
    BDBG_ASSERT(device);

    /* Start off by disabling the transmit (which also disables remux) */
    err = BTNF_DisableTransmit(device);
    if (err) return err;

    /* Make sure we're NOT selecting the RX registers */
	SelectRegisters(device, TransmitRegisters);

    reg = 0x0006c400; BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_CIP0, reg);
    reg = 0xa0800000; BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_CIP1, reg);

    reg = (
		BCHP_FIELD_DATA(AVLINK_AVTX_IT_HEADER, speed, speed) |
		BCHP_FIELD_DATA(AVLINK_AVTX_IT_HEADER, tag, 1) |
		BCHP_FIELD_DATA(AVLINK_AVTX_IT_HEADER, channel_Number, channel) |
		BCHP_FIELD_DATA(AVLINK_AVTX_IT_HEADER, tCode, 0xa) |
        BCHP_FIELD_DATA(AVLINK_AVTX_IT_HEADER, synch_Tx, sync)
    );
    BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_IT_HEADER, reg);

    reg = device->transmit.packetLength;
	BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_IT_PKT_SIZE, reg);
    reg = device->transmit.futureTimeStamp;
	BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_FUT_TIME_STAMP, reg);

    reg = 0xffffffff; BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_INT_EVENT_CLR, reg);
    reg = 0xffffffff; BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_FIFO_FLUSH, reg);

#if (BCHP_VER == BCHP_VER_A0 || BCHP_VER == BCHP_VER_A1)
#ifdef USE_RMX1
    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_AV_PATH_SEL);
    reg &= ~(BCHP_MASK(AVLINK_GLOBAL_CFR_AV_PATH_SEL, av_RMX_Sel_Tx));         /*0=Remux #1*/
    BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_AV_PATH_SEL, reg);
#else
    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_AV_PATH_SEL);
    reg |= (BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_AV_PATH_SEL, av_RMX_Sel_Tx, 1)); /*1=Remux #0*/
    BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_AV_PATH_SEL, reg);
#endif
#else
#ifdef USE_RMX1
    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_HOST_AV_PATH_SEL);
    reg &= ~(BCHP_MASK(AVLINK_HOST_AV_PATH_SEL, av_RMX_Sel_Tx));         /*0=Remux #1*/
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_AV_PATH_SEL, reg);
#else
    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_HOST_AV_PATH_SEL);
    reg |= (BCHP_FIELD_DATA(AVLINK_HOST_AV_PATH_SEL, av_RMX_Sel_Tx, 1)); /*1=Remux #0*/
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_AV_PATH_SEL, reg);
#endif
#endif

    /* Enable transmit */
    reg = (
		BCHP_FIELD_DATA(AVLINK_AVTX_CTRL, av_Iso_Tx_En, 1) |
		BCHP_FIELD_DATA(AVLINK_AVTX_CTRL, av_Tx_Data_Serial, 1) |
		BCHP_FIELD_DATA(AVLINK_AVTX_CTRL, force_One_Sec_Over, 1)
    );
	if (device->transmit.useVariableRate)
		reg |= BCHP_FIELD_DATA(AVLINK_AVTX_CTRL, It_Rate_Var, 1);
    if (sync) {
        reg |= (
	    	BCHP_FIELD_DATA(AVLINK_AVTX_CTRL, cont_Key_Valid, 1) |
	    	BCHP_FIELD_DATA(AVLINK_AVTX_CTRL, av_Tx_Cp_Prot_En, 1)
        );
    }
    BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_CTRL, reg);

	EnableRemux(device);

    return BERR_SUCCESS;
}

BERR_Code BTNF_DisableTransmit(BTNF_Handle device)
{
    uint32_t reg;
    BDBG_ASSERT(device);

    /* Make sure we're NOT selecting the RX registers */
	SelectRegisters(device, TransmitRegisters);

	DisableRemux(device);

    /* Wait awhile before disabing transmit so we see some packets go out! */
    BKNI_Delay(80);

    /* Disable transmit */
    reg = 0; BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_CTRL, reg);

    /* Flush the FIFOs and clear any pending interrupts */
    reg = 0xffffffff; BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_FIFO_FLUSH, reg);
    reg = 0xffffffff; BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_INT_EVENT_CLR, reg);

    return BERR_SUCCESS;
}

/********************************************************************************************
**********************      Isoch Receive Operations      ***********************************
*********************************************************************************************/

BERR_Code BTNF_EnableReceive(BTNF_Handle device, int channel, uint8_t sync)
{
    uint32_t reg;
    BDBG_ASSERT(device);

    /* Make sure we're selecting the RX registers */
	SelectRegisters(device, ReceiveRegisters);

	/* disable receives first */
    reg = 0; BREG_Write32(device->hRegister, BCHP_AVLINK_AVRX_CTRL, reg);

#if 0
    if (channel>32) {
	reg = 1<(channel-32);
        BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_CH_RX_HI_SET, reg);
	reg = 0;
        BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_CH_RX_LO_SET, reg);
    } else {
	reg = 1<<channel;
        BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_CH_RX_LO_SET, reg);
	reg = 0;
        BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_CH_RX_HI_SET, reg);
    }
#else
    reg = 0xffffffff; /* receive on any channel */
    BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_CH_RX_HI_SET, reg);
    BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_CH_RX_LO_SET, reg);
#endif

    reg = (BCHP_FIELD_DATA(AVLINK_AVRX_AV_IR_HEADER, av_Rx_Channel, channel));
#if 0
	/* If we do this then we won't be able to detect changes to the emi and odd/even */
	if (sync) 
	{
    	reg |= (BCHP_FIELD_DATA(AVLINK_AVRX_AV_IR_HEADER, synch_En_Rx, 1));
        reg |= (BCHP_FIELD_DATA(AVLINK_AVRX_AV_IR_HEADER, synch_Rx, sync));
	}
#endif
    BREG_Write32(device->hRegister, BCHP_AVLINK_AVRX_AV_IR_HEADER, reg);

    reg = 0xffffffff; BREG_Write32(device->hRegister, BCHP_AVLINK_AVRX_INT_EVENT_CLR, reg);
    reg = 0xffffffff; BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_FIFO_FLUSH, reg);

    /* Enable receive */
    reg = (
		BCHP_FIELD_DATA(AVLINK_AVRX_CTRL, force_One_Sec_Over, 1) |
		BCHP_FIELD_DATA(AVLINK_AVRX_CTRL, av_Iso_Rx_En, 1) |
		BCHP_FIELD_DATA(AVLINK_AVRX_CTRL, av_Rx_Serial, 1) |
		BCHP_FIELD_DATA(AVLINK_AVRX_CTRL, rx_FMT_En, 1) |
		BCHP_FIELD_DATA(AVLINK_AVRX_CTRL, rx_FMT, 0x20) /*20=MPEG*/
    );
	if (device->receive.jitterCompensationDisable)
		reg |= BCHP_FIELD_DATA(AVLINK_AVRX_CTRL, jit_Comp_Disable, 1);
    if (sync) {
        reg |= (BCHP_FIELD_DATA(AVLINK_AVRX_CTRL, av_Rx_Cp_Prot_En, 1));
    }
    BREG_Write32(device->hRegister, BCHP_AVLINK_AVRX_CTRL, reg);

    return BERR_SUCCESS;
}

BERR_Code BTNF_DisableReceive(BTNF_Handle device)
{
    uint32_t reg;
    BDBG_ASSERT(device);

    /* Make sure we're selecting the RX registers */
	SelectRegisters(device, ReceiveRegisters);

    /* Disable receive */
    reg = 0; BREG_Write32(device->hRegister, BCHP_AVLINK_AVRX_CTRL, reg);

    /* Flush the FIFOs and clear any pending interrupts */
    reg = 0xffffffff; BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_FIFO_FLUSH, reg);
    reg = 0xffffffff; BREG_Write32(device->hRegister, BCHP_AVLINK_AVRX_INT_EVENT_CLR, reg);

    return BERR_SUCCESS;
}

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

BERR_Code BTNF_SetKeys(BTNF_Handle device, isoch_operation op, unsigned int emi, bool odd, uint32_t high, uint32_t low)
{
    BDBG_ASSERT(device);

    /* The high order bits are reserved -- don't let them set a key thats invalid */
    if (high & 0xFF000000) return BERR_TRACE(BTNF_ERR_INVALID_KEY);
    /* We only have three valid EMI values to choose from */
    if (emi > 3) return (BTNF_ERR_INVALID_EMI);

    /* TODO: The keys are shared between the transmit and receive -- do the selection here too */
	SelectRegisters(device, op==e_transmit? TransmitRegisters : ReceiveRegisters);

    if (odd) {
        switch (emi) {
        case 3: /* emi A */
            BREG_Write32(device->hRegister, BCHP_AVLINK_5C_KEY_ODD_A_HIGH, high);
            BREG_Write32(device->hRegister, BCHP_AVLINK_5C_KEY_ODD_A_LOW, low);
            break;
        case 2: /* emi B */
            BREG_Write32(device->hRegister, BCHP_AVLINK_5C_KEY_ODD_B_HIGH, high);
            BREG_Write32(device->hRegister, BCHP_AVLINK_5C_KEY_ODD_B_LOW, low);
            break;
        case 1: /* emi C */
            BREG_Write32(device->hRegister, BCHP_AVLINK_5C_KEY_ODD_C_HIGH, high);
            BREG_Write32(device->hRegister, BCHP_AVLINK_5C_KEY_ODD_C_LOW, low);
            break;
        }
    } else {
        switch (emi) {
        case 3: /* emi A */
            BREG_Write32(device->hRegister, BCHP_AVLINK_5C_KEY_EVEN_A_HIGH, high);
            BREG_Write32(device->hRegister, BCHP_AVLINK_5C_KEY_EVEN_A_LOW, low);
            break;
        case 2: /* emi B */
            BREG_Write32(device->hRegister, BCHP_AVLINK_5C_KEY_EVEN_B_HIGH, high);
            BREG_Write32(device->hRegister, BCHP_AVLINK_5C_KEY_EVEN_B_LOW, low);
            break;
        case 1: /* emi C */
            BREG_Write32(device->hRegister, BCHP_AVLINK_5C_KEY_EVEN_C_HIGH, high);
            BREG_Write32(device->hRegister, BCHP_AVLINK_5C_KEY_EVEN_C_LOW, low);
            break;
        }
    }
    return BERR_SUCCESS;
}

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

/* 
** There is a question about whether I need to have transmit disabled before doing some operations.
** The next two functions allow you to disable the transmit and re-enable it (with the previous control settings).
** I've included stubs so the rest of the code doesn't have to change.
*/
#if 0 /*disable this until we find out we need it */
#if 1
/* stubs */
static uint32_t disableTransmit(BTNF_Handle device) {return 0;}
static void enableTransmit(BTNF_Handle device, uint32_t control) {}
#else

/* use this when making some register changes that need the transmit disabled. */
/* use enableTransmit to start transmitting again. */
static uint32_t disableTransmit(BTNF_Handle device)
{
    uint32_t reg = BREG_Read32(device->hRegister, BCHP_AVLINK_AVTX_CTRL);
	BTNF_DisableTransmit(device);
	return reg;
}

/* use this, after disabling the transmit, to toggle the transmit (you need to pass in the previous control value) */
static void enableTransmit(BTNF_Handle device, uint32_t control)
{
    BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_CTRL, control);
	EnableRemux(device);
    return BERR_SUCCESS;
}
#endif
#endif

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

/* Use this to change the emi and or odd/even sync setting */
BERR_Code BTNF_SetNewSync(BTNF_Handle device, unsigned int emi, bool odd)
{
    uint32_t reg;
	SelectRegisters(device, TransmitRegisters);

    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_AVTX_IT_HEADER);
    reg &= ~BCHP_MASK(AVLINK_AVTX_IT_HEADER, synch_Tx);
    reg |= BCHP_FIELD_DATA(AVLINK_AVTX_IT_HEADER, synch_Tx, ((emi<<2)|(odd?2:0)));
    BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_IT_HEADER, reg);

    return BERR_SUCCESS;
}

/* Use this to toggle the odd/even key */
BERR_Code BTNF_Toggle(BTNF_Handle device)
{
    uint32_t reg;
	SelectRegisters(device, TransmitRegisters);

    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_AVTX_IT_HEADER);
	if (reg & 2) reg &= ~2; else reg |= 2; /* If on, turn it off;  If off, turn it on */
    BREG_Write32(device->hRegister, BCHP_AVLINK_AVTX_IT_HEADER, reg);
    return BERR_SUCCESS;
}

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

/* Note: we don't have an invalidate function so we need to flush which does an invalidate */
static void FlushAddress(BTNF_Handle device, void *offset, int size)
{
	void *address;
	BMEM_Heap_ConvertAddressToCached(device->hHeap, offset, &address);
	BMEM_FlushCache(device->hHeap, address, size);
}

#define nxtQElementPtrAddressValid 1
#define intOnComplete 0x20

#if 0 /* TODO: can I make a common routine for this ??? */
static BERR_Code SetupDMA(char *function, BTNF_Handle device, void *buffer, int length, uint32_t qptr, void *buf))
{
    uint32_t *qptr, *offset;

    /* I need to be able to allocate memory for the DMA operations */
    /* Make sure they set the heap I'm to use for these operations */
    if (!device->hHeap) {
        BDBG_ERR(("%s (buffer): BMEM_Alloc() failed", function));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    qptr = BMEM_AllocAligned(device->hHeap, 5, 4, 0);
    if (qptr == NULL)
    {
        BDBG_ERR(("%s (qptr): BMEM_Alloc() failed", function));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    offset = BMEM_AllocAligned(device->hHeap, length, 4, 0);
    if (offset == NULL)
    {
        BDBG_ERR(("%s (buffer): BMEM_Alloc() failed", function));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

	FlushAddress(device, offset, length);

    err = BMEM_Heap_ConvertAddressToOffset(device->hHeap, (void *)offset, &buf);
    if (err)
    {
        BDBG_ERR(("%s (convert): failed", function));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

/*BDBG_ERR(("Addresses: offset=%x, buf=%x\n", offset, buf));*/

    qptr[0] = 0;  /* make sure the next pointer is not zero */
    qptr[1] = buf;
    qptr[2] = (length/4)<<8 | intOnComplete;
    qptr[3] = (length/4)<<12 | (length/4);
    qptr[4] = 0; /* next pointer (invalid) */

	FlushAddress(device, qptr, 5);

    err = BMEM_Heap_ConvertAddressToOffset(device->hHeap, (void *)qptr, &pointer);
    if (err)
    {
        BDBG_ERR(("BTNF_IsochReceive (convert): failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

   return BERR_SUCCESS;
}
#endif

/********************************************************************************************
**********************       Host Isoch Operations        ***********************************
*********************************************************************************************/

/* Setup to receive a single isoch packet -- have to check when buffer received and DMA has completed */
BERR_Code BTNF_IsochReceive(BTNF_Handle device, int channel, void *buffer, int length)
{
    uint32_t reg;
    uint32_t *qptr;
    uint8_t *offset;
    uint32_t buf=0, pointer=0;
    BERR_Code err;

    BDBG_ASSERT(device);
    BDBG_ASSERT(buffer); /* can't be receiving to a null pointer */

#if 0
    if (channel>32) {
	reg = 1<(channel-32);
        BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_CH_RX_HI_SET, reg);
	reg = 0;
        BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_CH_RX_LO_SET, reg);
    } else {
	reg = 1<<channel;
        BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_CH_RX_LO_SET, reg);
	reg = 0;
        BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_CH_RX_HI_SET, reg);
    }
#else
    BSTD_UNUSED(channel);
    reg = 0xffffffff; /* receive on any channel */
    BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_CH_RX_HI_SET, reg);
    BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_CH_RX_LO_SET, reg);
#endif

    /* I need to be able to allocate memory for these DMA operations */
    /* Make sure they set the heap I'm to use for these operations */
    if (!device->hHeap) {
        BDBG_ERR(("BTNF_IsochReceive (buffer): BMEM_Alloc() failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    qptr = BMEM_AllocAligned(device->hHeap, 5, 4, 0);
    if (qptr == NULL)
    {
        BDBG_ERR(("BTNF_IsochReceive (qptr): BMEM_Alloc() failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    offset = BMEM_AllocAligned(device->hHeap, length, 4, 0);
    if (offset == NULL)
    {
        BDBG_ERR(("BTNF_IsochReceive (buffer): BMEM_Alloc() failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

	FlushAddress(device, offset, length);

    err = BMEM_Heap_ConvertAddressToOffset(device->hHeap, (void *)offset, &buf);
    if (err)
    {
        BDBG_ERR(("BTNF_IsochReceive (convert): failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

/*BDBG_ERR(("Addresses: offset=%x, buf=%x\n", offset, buf));*/

    qptr[0] = 0;  /* make sure the next pointer is not zero */
    qptr[1] = buf;
    qptr[2] = (length/4)<<8 | intOnComplete;
    qptr[3] = (length/4)<<12 | (length/4);
    qptr[4] = 0; /* next pointer (invalid) */

	FlushAddress(device, qptr, 5);

    err = BMEM_Heap_ConvertAddressToOffset(device->hHeap, (void *)qptr, &pointer);
    if (err)
    {
        BDBG_ERR(("BTNF_IsochReceive (convert): failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

/*BDBG_ERR(("Addresses: qptr=%x, pointer=%x\n", qptr, pointer));*/

    /* Clear any left over event status for a previous transfer */
    reg = BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_HOST_INT_EVENT_CLR, ir_Rx_Packet, 1);
    reg = 0xffffffff; /* hack: just clear everything */
    BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HOST_INT_EVENT_CLR, reg);

    reg = (uint32_t)pointer;
    reg |= BCHP_FIELD_DATA(AVLINK_HOST_IR_DMA_QHPTR, DMA_QHPtr_Addr_Valid, 1);
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_IR_DMA_QHPTR, reg);

    reg = (BCHP_FIELD_DATA(AVLINK_HOST_IR_DMA_CTRL_STATUS, DMA_Reset, 1));
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_IR_DMA_CTRL_STATUS, reg);

    reg = (
		/*BCHP_FIELD_DATA(AVLINK_HOST_IR_DMA_CTRL_STATUS, DMA_Halted, 1) |*/
		BCHP_FIELD_DATA(AVLINK_HOST_IR_DMA_CTRL_STATUS, DMA_Run, 1)
    );
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_IR_DMA_CTRL_STATUS, reg);

	/* Enable the isoch receive so we can get a packet (use per packet mode for DMA) */
    reg = (
		BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_HC_CONTROL_SET, iso_Rx_En, 1) |
		BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_HC_CONTROL_SET, iso_Dma_Mode_Select, 1)
	);
    BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HC_CONTROL_SET, reg);

	/* I'll have to wait for results here or use a callback to indicate completion */

    device->ReceiveData.buffer = buffer;
    device->ReceiveData.length = length;
    device->ReceiveData.offset = offset;

    return BERR_SUCCESS;
}

bool BTNF_IsochReceiveComplete(BTNF_Handle device)
{
    uint32_t reg = BREG_Read32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HOST_INT_EVENT_SET);
#if 0
    /* Wait for packet to arrive in FIFO */
    reg &= BCHP_MASK(AVLINK_GLOBAL_CFR_HOST_INT_EVENT_SET, ir_Rx_Packet);
#else
    /* Wait for packet to pass from FIFO to memory (using DMA) */
    reg &= BCHP_MASK(AVLINK_GLOBAL_CFR_HOST_INT_EVENT_SET, ir_Dma_IOC);
#endif
    if (reg)
    {
	int i;
    	for (i=0; i<device->ReceiveData.length; i++) 
		((char*)device->ReceiveData.buffer)[i] = device->ReceiveData.offset[i];
        /*BMEM_Free(device->hHeap, device->ReceiveData.offset);*/
	return true;
    }

    return false;
}

/* Send an isoch packet -- need to check when buffer has been sent out */
BERR_Code BTNF_IsochSend(BTNF_Handle device, void *buffer, int length)
{
    int i;
    uint32_t reg;
    uint32_t *qptr;
    uint8_t *offset;
    uint32_t buf=0, pointer=0;
    BERR_Code err;

    BDBG_ASSERT(device);
    BDBG_ASSERT(buffer); /* can't be sriting from a null pointer */

    /* I need to be able to allocate memory for these DMA operations */
    /* Make sure they set the heap I'm to use for these operations */
    if (!device->hHeap) {
        BDBG_ERR(("BTNF_IsochSend (buffer): BMEM_Alloc() failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    qptr = BMEM_AllocAligned(device->hHeap, 5, 4, 0);
    if (qptr == NULL)
    {
        BDBG_ERR(("BTNF_IsochSend (qptr): BMEM_Alloc() failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    offset = BMEM_AllocAligned(device->hHeap, length, 4, 0);
    if (offset == NULL)
    {
        BDBG_ERR(("BTNF_IsochSend (buffer): BMEM_Alloc() failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    for (i=0; i<length; i++) offset[i] = ((char*)buffer)[i];

	FlushAddress(device, offset, length);

    err = BMEM_Heap_ConvertAddressToOffset(device->hHeap, (void *)offset, &buf);
    if (err)
    {
        BDBG_ERR(("BTNF_IsochSend (convert): failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

/*BDBG_ERR(("Addresses: offset=%x, buf=%x\n", offset, buf));*/

    qptr = BMEM_AllocAligned(device->hHeap, 5, 4, 0);
    if (qptr == NULL)
    {
        BDBG_ERR(("BTNF_IsochSend (qptr): BMEM_Alloc() failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    qptr[0] = 0;  /* make sure the next pointer is not zero */
    qptr[1] = buf;
    qptr[2] = (length/4)<<8 | intOnComplete;
    qptr[3] = 0; /* return status */
    qptr[4] = 0; /* next pointer (invalid) */

	FlushAddress(device, qptr, 5);

    err = BMEM_Heap_ConvertAddressToOffset(device->hHeap, (void *)qptr, &pointer);
    if (err)
    {
        BDBG_ERR(("BTNF_IsochSend (convert): failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

/*BDBG_ERR(("Addresses: qptr=%x, pointer=%x\n", qptr, pointer));*/

    /* Clear any left over event status for a previous transfer */
    reg = BCHP_FIELD_DATA(AVLINK_CSR_INT_EVENT_CLR, it_Complete, 1);
    reg = 0xffffffff; /* hack: just clear everything */
    BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_INT_EVENT_CLR, reg);

    reg = (uint32_t)pointer;
    reg |= BCHP_FIELD_DATA(AVLINK_HOST_IT_DMA_QHPTR, DMA_QHPtr_Addr_Valid, 1);
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_IT_DMA_QHPTR, reg);

    reg = (BCHP_FIELD_DATA(AVLINK_HOST_IT_DMA_CTRL_STATUS, DMA_Reset, 1));
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_IT_DMA_CTRL_STATUS, reg);

    reg = (
		/*BCHP_FIELD_DATA(AVLINK_HOST_IT_DMA_CTRL_STATUS, DMA_Halted, 1) | */
		BCHP_FIELD_DATA(AVLINK_HOST_IT_DMA_CTRL_STATUS, DMA_Run, 1)
    );
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_IT_DMA_CTRL_STATUS, reg);
    return BERR_SUCCESS;
}

bool BTNF_IsochSendComplete(BTNF_Handle device)
{
    /* Wait for packet to go out on bus */
    uint32_t reg = BREG_Read32(device->hRegister, BCHP_AVLINK_CSR_INT_EVENT_SET);
    if (reg & BCHP_MASK(AVLINK_CSR_INT_EVENT_SET, it_Complete)) 
    {
        /*BMEM_Free(device->hHeap, device->ReceiveData.offset);*/
	return true;
    }

    return false;
}

/********************************************************************************************
**********************      Asynchronous Operations       ***********************************
*********************************************************************************************/

BERR_Code BTNF_AsynchRead(BTNF_Handle device, void *buffer, int length)
{
    uint32_t reg;
    uint32_t *qptr;
    uint8_t *offset;
    uint32_t buf=0, pointer=0;
    BERR_Code err;

    BDBG_ASSERT(device);
    BDBG_ASSERT(buffer); /* can't be receiving to a null pointer */

#if 1
    /* I need to be able to allocate memory for these DMA operations */
    /* Make sure they set the heap I'm to use for these operations */
    if (!device->hHeap) {
        BDBG_ERR(("BTNF_AsynchRead (buffer): BMEM_Alloc() failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

	/* Disable the receive and flush the receive FIFO */
    reg = ~(
		BCHP_MASK(AVLINK_GLOBAL_CFR_HC_CONTROL_SET, asy_Rx_En) |
		BCHP_MASK(AVLINK_GLOBAL_CFR_HC_CONTROL_SET, asy_Dma_Mode_Select)
	);
	BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HC_CONTROL_SET, reg);

	FlushFIFOs(device, ar_FIFO_Flush);

    qptr = BMEM_AllocAligned(device->hHeap, 5, 4, 0);
    if (qptr == NULL)
    {
        BDBG_ERR(("BTNF_AsynchRead (qptr): BMEM_Alloc() failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    offset = BMEM_AllocAligned(device->hHeap, length, 4, 0);
    if (offset == NULL)
    {
        BDBG_ERR(("BTNF_AsynchRead (buffer): BMEM_Alloc() failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

	FlushAddress(device, offset, length);

    err = BMEM_Heap_ConvertAddressToOffset(device->hHeap, (void *)offset, &buf);
    if (err)
    {
        BDBG_ERR(("BTNF_AsynchRead (convert): failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

/*BDBG_ERR(("Addresses: offset=%x, buf=%x\n", offset, buf));*/

    qptr[0] = 0;  /* make sure the next pointer is not zero */
    qptr[1] = buf;
    qptr[2] = (length/4)<<8 | intOnComplete;
    qptr[3] = (length/4)<<12 | (length/4);
    qptr[4] = 0; /* next pointer (invalid) */

	FlushAddress(device, qptr, 5);

    err = BMEM_Heap_ConvertAddressToOffset(device->hHeap, (void *)qptr, &pointer);
    if (err)
    {
        BDBG_ERR(("BTNF_AsynchRead (convert): failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

/*BDBG_ERR(("Addresses: qptr=%x, pointer=%x\n", qptr, pointer));*/

    reg = (uint32_t)pointer;
    reg |= BCHP_FIELD_DATA(AVLINK_HOST_AR_DMA_QHPTR, DMA_QHPtr_Addr_Valid, 1);
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_AR_DMA_QHPTR, reg);

#if 1
	{
		/* Reset the DMA before starting a new operation */
		int i;
		reg = BCHP_FIELD_DATA(AVLINK_HOST_AR_DMA_CTRL_STATUS, DMA_Reset, 1);
    	BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_AR_DMA_CTRL_STATUS, reg);

#define ARBITRARY_TIMEOUT 1000
		for (i=0; i < ARBITRARY_TIMEOUT; i++)
		{
			/* First we wait for the reset bit to go away -- means reset accepted */
    		reg = BREG_Read32(device->hRegister, BCHP_AVLINK_HOST_AR_DMA_CTRL_STATUS);
			if (!(reg & BCHP_MASK(AVLINK_HOST_AR_DMA_CTRL_STATUS, DMA_Reset))) break;
		}
		for (i=0; i < ARBITRARY_TIMEOUT; i++)
		{
			/* Now we wait for the reset to complete by waiting for the 'halt' bit */
    		reg = BREG_Read32(device->hRegister, BCHP_AVLINK_HOST_AR_DMA_CTRL_STATUS);
			if (reg & BCHP_MASK(AVLINK_HOST_AR_DMA_CTRL_STATUS, DMA_Halted)) break;
		}
	}
#endif
#endif

    /* Clear any pending interrupt/status from previous operation */
    reg = (
		BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_HOST_INT_EVENT_CLR, ar_Rx_Packet, 1) |
    	BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_HOST_INT_EVENT_SET, ar_Dma_IOC, 1)
	);
    reg = 0xffffffff; /* hack: just clear everything */
    BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HOST_INT_EVENT_CLR, reg);

    reg = (
		/*BCHP_FIELD_DATA(AVLINK_HOST_AR_DMA_CTRL_STATUS, DMA_Halted, 1) | */
		BCHP_FIELD_DATA(AVLINK_HOST_AR_DMA_CTRL_STATUS, DMA_Run, 1)
    );
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_AR_DMA_CTRL_STATUS, reg);

#ifdef ENABLE_INTERRUPTS
    reg = (
		BCHP_MASK(AVLINK_GLOBAL_CFR_HOST_INT_MASK, ar_Dma_IOC) |
		BCHP_MASK(AVLINK_GLOBAL_CFR_HOST_INT_MASK, ar_Rx_Packet)
	);
    device->accumulated.host &= ~reg; /* clear this status for next operation */

	/* Enable the interrupt so we can see that we received a packet */
    reg = BREG_Read32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HOST_INT_MASK);
    reg &= ~(
		BCHP_MASK(AVLINK_GLOBAL_CFR_HOST_INT_MASK, ar_Dma_IOC) |
		BCHP_MASK(AVLINK_GLOBAL_CFR_HOST_INT_MASK, ar_Rx_Packet)
	);
	BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HOST_INT_MASK, reg);
#endif

	/* Enable the async receive so we can get a packet (use per packet mode for DMA) */
    reg = (
		BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_HC_CONTROL_SET, asy_Rx_En, 1) |
		BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_HC_CONTROL_SET, asy_Dma_Mode_Select, 1)
	);
	BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HC_CONTROL_SET, reg);

	/* I'll have to wait for results here or use a callback to indicate completion */

    device->ReceiveData.buffer = buffer;
    device->ReceiveData.length = length;
    device->ReceiveData.offset = offset;

    return BERR_SUCCESS;
}

bool BTNF_AsynchReadComplete(BTNF_Handle device)
{
    int i;
#ifdef ENABLE_INTERRUPTS
    uint32_t reg = device->accumulated.host; /* check the saved status */
#else
    uint32_t reg = BREG_Read32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HOST_INT_EVENT_SET);
#endif
	bool result = false;

#if 1
    /* Wait for receive data in FIFO */
    reg &= BCHP_MASK(AVLINK_GLOBAL_CFR_HOST_INT_EVENT_SET, ar_Rx_Packet);
#else
    /* Wait for receive data to pass from FIFO to memory (via DMA) */
    reg &= BCHP_MASK(AVLINK_GLOBAL_CFR_HOST_INT_EVENT_SET, ar_Dma_IOC);
#endif

    if (reg)
    {
		result = true; /* yes, we did receive data */

    	for (i=0; i<device->ReceiveData.length; i++) 
			((char*)device->ReceiveData.buffer)[i] = device->ReceiveData.offset[i];

        /*BMEM_Free(device->hHeap, device->ReceiveData.offset);*/

		/* Disable the receive */
    	reg = BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_HC_CONTROL_CLR, asy_Rx_En, 1);
		BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HC_CONTROL_CLR, reg);

#ifdef ENABLE_INTERRUPTS
		/* Disable this interrupt(s) too */
		reg = BREG_Read32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HOST_INT_MASK);
    	reg |= (
			BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_HOST_INT_MASK, ar_Dma_IOC, 1) |
			BCHP_FIELD_DATA(AVLINK_GLOBAL_CFR_HOST_INT_MASK, ar_Rx_Packet, 1)
		);
		BREG_Write32(device->hRegister, BCHP_AVLINK_GLOBAL_CFR_HOST_INT_MASK, reg);
#endif
    }

    return result;
}

BERR_Code BTNF_AsynchWrite(BTNF_Handle device, void *buffer, int length)
{
    int i;
    uint32_t reg;
    uint32_t *qptr;
    uint8_t *offset;
    uint32_t buf=0, pointer=0;
    BERR_Code err;

    BDBG_ASSERT(device);
    BDBG_ASSERT(buffer);

    /* I need to be able to allocate memory for these DMA operations */
    /* Make sure they set the heap I'm to use for these operations */
    if (!device->hHeap) {
        BDBG_ERR(("BTNF_AsynchWrite (buffer): BMEM_Alloc() failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    qptr = BMEM_AllocAligned(device->hHeap, 5, 4, 0);
    if (qptr == NULL)
    {
        BDBG_ERR(("BTNF_AsynchWrite (qptr): BMEM_Alloc() failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    offset = BMEM_AllocAligned(device->hHeap, length, 4, 0);
    if (offset == NULL)
    {
        BDBG_ERR(("BTNF_AsynchWrite (buffer): BMEM_Alloc() failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    for (i=0; i<length; i++) offset[i] = ((char*)buffer)[i];

	FlushAddress(device, offset, length);

    err = BMEM_Heap_ConvertAddressToOffset(device->hHeap, (void *)offset, &buf);
    if (err)
    {
        BDBG_ERR(("BTNF_AsynchWrite (convert): failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

/*BDBG_ERR(("Addresses: offset=%x, buf=%x\n", offset, buf));*/

    qptr = BMEM_AllocAligned(device->hHeap, 5, 4, 0);
    if (qptr == NULL)
    {
        BDBG_ERR(("BTNF_AsynchWrite (qptr): BMEM_Alloc() failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    qptr[0] = 0;  /* make sure the next pointer is not zero */
    qptr[1] = buf;
    qptr[2] = (length/4)<<8 | intOnComplete;
    qptr[3] = 0; /* return status */
    qptr[4] = 0; /* next pointer (invalid) */

	FlushAddress(device, qptr, 5);

    err = BMEM_Heap_ConvertAddressToOffset(device->hHeap, (void *)qptr, &pointer);
    if (err)
    {
        BDBG_ERR(("BTNF_AsynchWrite (convert): failed"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

/*BDBG_ERR(("Addresses: qptr=%x, pointer=%x\n", qptr, pointer));*/

    /* Clear any left over event status for a previous transfer */
    reg = BCHP_FIELD_DATA(AVLINK_CSR_INT_EVENT_CLR, at_Req_Complete, 1);
    reg = 0xffffffff; /* hack: just clear everything */
    BREG_Write32(device->hRegister, BCHP_AVLINK_CSR_INT_EVENT_CLR, reg);

    /* Setup the DMA and tell it to go */
    reg = (uint32_t)pointer;
    reg |= BCHP_FIELD_DATA(AVLINK_HOST_AT_REQ_DMA_QHPTR, DMA_QHPtr_Addr_Valid, 1);
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_AT_REQ_DMA_QHPTR, reg);

    reg = (BCHP_FIELD_DATA(AVLINK_HOST_AT_REQ_DMA_CTRL_STATUS, DMA_Reset, 1));
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_AT_REQ_DMA_CTRL_STATUS, reg);

    reg = (
		/*BCHP_FIELD_DATA(AVLINK_HOST_AT_REQ_DMA_CTRL_STATUS, DMA_Halted, 1) |*/
		BCHP_FIELD_DATA(AVLINK_HOST_AT_REQ_DMA_CTRL_STATUS, DMA_Run, 1)
    );
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_AT_REQ_DMA_CTRL_STATUS, reg);

    return BERR_SUCCESS;
}

bool BTNF_AsynchWriteComplete(BTNF_Handle device)
{
    /* Wait for packet to go out on bus */
    uint32_t reg = BREG_Read32(device->hRegister, BCHP_AVLINK_CSR_INT_EVENT_SET);
    if (reg & BCHP_MASK(AVLINK_CSR_INT_EVENT_SET, at_Req_Complete)) 
    {
        /*BMEM_Free(device->hHeap, device->ReceiveData.offset);*/
	return true;
    }

    return false;
}

#if 0
BERR_Code BTNF_AsynchReadWithCallback(
	BTNF_Handle device, uint16_t node, int generation, uint64_t address, int length, 
	BTNF_AsynchCallback callback, void *context)
{
    return BERR_SUCCESS;
}

BERR_Code BTNF_AsynchWriteWithCallback(
	BTNF_Handle device, uint16_t node, int generation, uint64_t address, int length, void *buffer, 
	BTNF_AsynchCallback callback, void *context)
{
    return BERR_SUCCESS;
}

void BTNF_CleanupResponse(void *cleanup)
{
    return BERR_SUCCESS;
}
#endif


/********************************************************************************************
**********************           PHY Operations           ***********************************
*********************************************************************************************/

#if 0
void BTNF_AsynchPHY(BTNF_Handle device, void *packetData)
{
    uint32_t reg;
    BDBG_ASSERT(device);
    BDBG_ASSERT(packetData);
    return BERR_SUCCESS;
}
#endif

#if 0
BERR_Code BTNF_SendPHYpacket(BTNF_Handle device, uint8_t *packet, unsigned int size)
{
    BERR_Code retVal = BERR_SUCCESS;
    uint32_t reg;
    uint16_t node;

    uint32_t qptr[5] = { 0 }; /* link list for ATReq */
    uint32_t PhyPacket[] = { 
	0xe0,       /* Header 1 used by link:  spd[18:16] = 100Mbps for PHY packet tcode[7:4] */
	0x02200002, /* Header 2 for PHY packet: PHY packet - SUSPEND: PHY_ID[29:24] TYPE[21:18] = 8 PORT[14:11 = 0 CMD[2:0] = 2 */
	0           /* crc goes here */
        };

    BDBG_ASSERT(device);
    BDBG_ASSERT(packet); /* can't be sending from a null pointer */

    /* We're going to need our node ID for this operation */
    if ((retVal = BTNF_GetNodeID(device, &node))) return retVal;

    qptr[0] = (uint32_t)&qptr[4];  /* have the next pointer set to something in our control */
    qptr[1] = (uint32_t)&PhyPacket[0]; /* start of PHY packet */
    qptr[2] = sizeof(PhyPacket)<<8 | intOnComplete;
    qptr[3] = 0; /* return status */
 
    PhyPacket[2] = ~PhyPacket[1]; /*crc*/ 

    reg = (uint32_t)qptr;
    reg |= BCHP_FIELD_DATA(AVLINK_HOST_AT_REQ_DMA_QHPTR, DMA_QHPtr_Addr_Valid, 1);
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_AT_REQ_DMA_QHPTR, reg);

    reg = 0x0; /* clear left over */
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_AT_REQ_DMA_CTRL_STATUS, reg);
    reg |= (
		/*BCHP_FIELD_DATA(AVLINK_HOST_AT_REQ_DMA_CTRL_STATUS, DMA_Halted, 1) |*/
		BCHP_FIELD_DATA(AVLINK_HOST_AT_REQ_DMA_CTRL_STATUS, DMA_Run, 1)
    );
    BREG_Write32(device->hRegister, BCHP_AVLINK_HOST_AT_REQ_DMA_CTRL_STATUS, reg);

done:
    return BERR_SUCCESS;
}
#endif

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

