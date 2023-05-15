/***************************************************************************
 *     Copyright (c) 2009-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnf.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/18/10 10:05a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/magnum/1394/tnf/7420/btnf.h $
 * 
 * Hydra_Software_Devel/2   3/18/10 10:05a rjlewis
 * SW7420-380: error id now being set in berr module.
 * 
 * Hydra_Software_Devel/1   10/9/09 5:51p rjlewis
 * SW7420-380: Initial release.
 * 
 * 
 ***************************************************************************/

/*= Module Overview *********************************************************
<verbatim>

Overview
This module represents a generic interface to the 1394 hardware registers.

Usage
The usage of BTNF involves the following:

   * Opening the device.
   * Using the supplied interface routines to read/write specific registers.
   * Closing the device.

Sample Code

volatile interrupt = false;
static void myIntHandler(context *myParam1, int myParam2) { interrupt = true; }

void main( void )
{
    BTNF_Handle  h1394Device;
    BTNF_DefaultSettings defSettings;
    BREG_Handle  hReg;
    BCHP_Handle  hChip;
    BINT_Handle  hInt;
    
    // Do other initialization, i.e. for BREG, BCHP, BINT, etc

    // Now, ready to setup the BTNF
    BTNF_GetDefaultSettings(&defSettings);

    BTNF_Open(&h1394Device, hChip, hReg, hMem, hInt, &defSettings);

    // PI is now ready to be used.

    BTNF_Close(h1394Device);
}
    
</verbatim>
***************************************************************************/

#ifndef BTNF_H__
#define BTNF_H__

#include "bchp.h"
#include "bint.h"
#include "bmem.h"
#include "berr.h"
#include "berr_ids.h"

/***************************************************************************
Summary:
	Error Codes specific to BTNF

Description:
    These define the set of error codes unique to the 1394 functions.
    This list does not limit the returned error codes (also uses general errors defined elsewhere).

****************************************************************************/
#define BTNF_ERR_TODO              BERR_MAKE_CODE(BERR_TNF_ID, 0)
#define BTNF_ERR_PHY_TIMEOUT       BERR_MAKE_CODE(BERR_TNF_ID, 1)
#define BTNF_ERR_NOT_READY         BERR_MAKE_CODE(BERR_TNF_ID, 2)
#define BTNF_ERR_INVALID_EMI       BERR_MAKE_CODE(BERR_TNF_ID, 3)
#define BTNF_ERR_INVALID_KEY       BERR_MAKE_CODE(BERR_TNF_ID, 4)
#define BTNF_ERR_INVALID_PARAMETER BERR_MAKE_CODE(BERR_TNF_ID, 5)

/***************************************************************************
Summary:
	The handle for 1394 device.

Description:
    An opaque handle for BTNF device.
    This is returned on a successful Open call and is passed into the
    other PI functions.

See Also:
	BTNF_Open(), BTNF_Close().

****************************************************************************/
typedef struct BTNF_P_Context *BTNF_Handle;


/***************************************************************************
Summary:
	The format for the callback.

Description:
    When an operation is requested with a callback (for completion) that callback
    will be called when the operation completes or times out.
    The callback will occur in interrupt context.

See Also:
	BTNF_xxx().

****************************************************************************/
typedef void (*BTNF_CallbackFunc)(
    void *pParm1,                /* [in] First parameter used to define context */
    int parm2                    /* [in] Second parameter used to define context */
    ); 


/***************************************************************************
Summary:
****************************************************************************/
typedef struct BTNF_DefaultSettings {
    uint32_t undefined;
} BTNF_DefaultSettings;

/***************************************************************************
Summary:
	Defines the different interrupt registers.

Description:
    The interrupt register values are accumulated (saved) and cleared on an interrupt.
	This lists the different interrupt registers that are accumulated.

See Also:
	BTNF_ReadInterruptStatus(), BTNF_ClearInterruptStatus().

****************************************************************************/
typedef enum { csr, host, tx, rx } BTNF_InterruptRegister;


#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
	This function opens and creates the 1394 handle.

Description:
    This function is responsible for  ...
    This function will register for the 1394 interrupts.
    This is typically created at the system level and the resulting handle is passed to specific modules.
	
Returns:
    A valid handle that can be used to close on success.
    Status of the request.

See Also:
	BTNF_Close().

****************************************************************************/
BERR_Code BTNF_Open( 
    BTNF_Handle *phDevice,                       /* [out] Return device handle on successful open */
    BCHP_Handle hChip,                           /* [in] Chip handle */
    BREG_Handle hRegister,                       /* [in] Register handle */
    BINT_Handle hInterrupt,                      /* [in] Interrupt handle */
    const BTNF_DefaultSettings *pDefaultSettings /* [in] Default settings */
    );

/***************************************************************************
Summary:
	This function closes a previously opened 1394 handle.

Description:
    This function is responsible for releasing a previously opened 1394 device.
    This function will deregister for the interrupts.
    This is used to undo the actions of the open.
	
Returns:
    The status of the operation.

See Also:
	BTNF_Open().

****************************************************************************/
BERR_Code BTNF_Close(
    BTNF_Handle hDevice /* [in] Device handle */
    ); 

/***************************************************************************
Summary:
	This function returns the default device settings.

Description:
    This function is responsible for providing the default values for the device settings.

Returns:
    A copy of the default settings and the status of the request.

See Also:
	BTNF_Open().

****************************************************************************/
BERR_Code BTNF_GetDefaultSettings( 
    BTNF_DefaultSettings *pSettings /* [out] Default settings */
    );

/* Hack until I can figure out how to allocate my own memory */
/* Maybe this should be something they have to set as part of the settings??? */
BERR_Code BTNF_SetMem(BTNF_Handle hDevice, BMEM_Heap_Handle hHeap);

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

/* PHY operations: */

BERR_Code BTNF_ReadPhyRegister(
    BTNF_Handle hDevice, /* [in] Device handle */
    uint8_t address,     /* [in] phy address to read */
    uint8_t *data        /* [out] put data here */
    );
BERR_Code BTNF_WritePhyRegister(
    BTNF_Handle hDevice, /* [in] Device handle */
    uint8_t address,     /* [in] phy address to read */
    uint8_t data         /* [in] data to write */
    );

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

#define BTNF_IssueBusReset BTNF_IssueShortBusReset /* use default */
BERR_Code BTNF_IssueShortBusReset(BTNF_Handle hDevice);
BERR_Code BTNF_IssueLongBusReset(BTNF_Handle hDevice);

BERR_Code BTNF_ForceRoot(BTNF_Handle hDevice);
BERR_Code BTNF_DontForceRoot(BTNF_Handle hDevice);
BERR_Code BTNF_EnableContender(BTNF_Handle hDevice);
BERR_Code BTNF_DisableContender(BTNF_Handle hDevice);

BERR_Code BTNF_GetGeneration(BTNF_Handle hDevice, int *generation);
BERR_Code BTNF_GetNodeID(BTNF_Handle hDevice, uint16_t *node); /* note: this will return error if the node id is invalid */

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

#define RMX_Rate 3 /* 0=81.0 Mhz, 1=54, 2=40.5, 3=20.25 */
typedef struct {
	int rate;                  /* the clock rate for the remux (default is 3 = 20.25) */
	bool null_packet_disable;  /* default is false */
	bool bypass_enable;        /* default is false */
	int null_packet_threshold; /* default is 0x10 */
	bool nrun_enable;          /* default is true */
	int packet_delay_count;    /* default is 0 */
} BTNF_RemuxSettings;

typedef struct {
	bool useVariableRate;      /* default is true */
	int packetLength;          /* default is 5 packets */
	uint32_t futureTimeStamp;  /* default is 0x3000 */
} BTNF_TransmitSettings;

typedef struct {
	bool jitterCompensationDisable; /* default is true */
} BTNF_ReceiveSettings;

/* The following functions lets the caller change the remux and/or transmit settings before starting a transmit operation */
BERR_Code BTNF_GetRemuxSettings(BTNF_Handle hDevice, BTNF_RemuxSettings *settings);
BERR_Code BTNF_SetRemuxSettings(BTNF_Handle hDevice, BTNF_RemuxSettings *settings);
BERR_Code BTNF_GetTransmitSettings(BTNF_Handle hDevice, BTNF_TransmitSettings *settings);
BERR_Code BTNF_SetTransmitSettings(BTNF_Handle hDevice, BTNF_TransmitSettings *settings);

/* The following functions lets the caller change the receive settings before starting a receive operation */
BERR_Code BTNF_GetReceiveSettings(BTNF_Handle hDevice, BTNF_ReceiveSettings *settings);
BERR_Code BTNF_SetReceiveSettings(BTNF_Handle hDevice, BTNF_ReceiveSettings *settings);

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

/* Streaming Isoch operations: */
/* These utilize the streaming data paths (i.e. no DMA and no memory)  */
/* Receive data goes to input band, Transmit data comes from Remux */

BERR_Code BTNF_EnableTransmit(BTNF_Handle hDevice, int speed, int channel, uint8_t sync);
BERR_Code BTNF_DisableTransmit(BTNF_Handle hDevice);
BERR_Code BTNF_EnableReceive(BTNF_Handle hDevice, int channel, uint8_t sync);
BERR_Code BTNF_DisableReceive(BTNF_Handle hDevice);

/* TODO: incorporate these into the function APIs */
typedef enum { e_transmit, e_receive } isoch_operation;
#if 0
typedef enum { e_even, e_odd } odd_even;
typedef struct { uint8_t emi:2; uint8_t odd_even:1; uint8_t unused:1; } sync;
#endif

/* Use this to set the appropriate key registers (based on emi and odd/even field) */
BERR_Code BTNF_SetKeys(BTNF_Handle device, isoch_operation op, unsigned int emi, bool odd, uint32_t high, uint32_t low);

/* Use this to change the emi and or odd/even sync setting */
BERR_Code BTNF_SetNewSync(BTNF_Handle device, unsigned int emi, bool odd);
/* Use this to toggle the odd/even key */
BERR_Code BTNF_Toggle(BTNF_Handle device);

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

/* Isoch operations: */

/* These will send/receive a single isoch packet via DMA operations */
BERR_Code BTNF_IsochReceive(BTNF_Handle hDevice, int channel, void *buffer, int length);
bool BTNF_IsochReceiveComplete(BTNF_Handle hDevice);
BERR_Code BTNF_IsochSend(BTNF_Handle hDevice, void *buffer, int length);
bool BTNF_IsochSendComplete(BTNF_Handle hDevice);

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

/* Asynch operations: */

BERR_Code BTNF_AsynchRead(BTNF_Handle hDevice, void *buffer, int length);
bool BTNF_AsynchReadComplete(BTNF_Handle hDevice);
BERR_Code BTNF_AsynchWrite(BTNF_Handle hDevice, void *buffer, int length);
bool BTNF_AsynchWriteComplete(BTNF_Handle hDevice);

#if 0
/* This is the callback function used for the read/write calls */
typedef void (*BTNF_AsynchCallback)(BERR_Code status, void *data, int dataLength, void *context, void *cleanup);

BERR_Code BTNF_AsynchReadWithCallback(
    BTNF_Handle hDevice, uint16_t node, int generation, uint64_t address, int length, 
    BTNF_AsynchCallback callback, void *context);
BERR_Code BTNF_AsynchWriteWithCallback(
    BTNF_Handle hDevice, uint16_t node, int generation, uint64_t address, int length, void *buffer, 
    BTNF_AsynchCallback callback, void *context);

/* This must be called by the callback function */
void BTNF_CleanupResponse(void *cleanup);
#endif

/***************************************************************************
Summary:
	This function is used to read the accumulated interrupt status values. 

Description:
    The interrupt status is accumulated for later processing.  
	This is used to read the current values.

Returns:
    A copy of the current accumulated status for the specified register.

See Also:
	BTNF_ClearInterruptStatus().

****************************************************************************/
void BTNF_ReadInterruptStatus(BTNF_Handle device, BTNF_InterruptRegister reg, uint32_t *value);

/***************************************************************************
Summary:
	This function is used to clear the accumulated status values. 

Description:
    The interrupt status is accumulated for later processing.  This is used to clear that status.  
	The BTNF_ReadStatus function is used to provide the current value.  The mask specifies which bits to clear.

See Also:
	BTNF_ReadInterruptStatus().

****************************************************************************/
void BTNF_ClearInterruptStatus(BTNF_Handle device, BTNF_InterruptRegister reg, uint32_t mask);

/***************************************************************************
Summary:
	This is used to determine if the code was built with interrupt support.

Description:
    If not using interrupts, the interrupt status needs to be polled if interested in whether a particular
	interrupt status has been set.  After polling, the ReadStatus and ClearStatus functions can be used to
	process a specific interrupt register.

Returns:
    true if the code was built with interrupt support (no polling necessary).

See Also:
	BTNF_PollInterruptStatus(), BTNF_ReadInterruptStatus(), BTNF_ClearInterruptStatus().

****************************************************************************/
bool BTNF_AreInterruptsBeingUsed(void);

/***************************************************************************
Summary:
	This can be used to poll the interrupt status registers When interrupts are not being used.

Description:
    When not using interrupts, the interrupt status bits are still accumulated (with no interrupt generated).
	This is used to process the status values into a set of accumulated registers that can be read and cleared.

Returns:
    Success if the register was successfully updated.

See Also:
	BTNF_ReadInterruptStatus(), BTNF_ClearInterruptStatus().

****************************************************************************/
void BTNF_PollInterruptStatus(BTNF_Handle device);

#if 0
/* Why are these special??? */
BERR_Code BTNF_SendPHYpacket(BTNF_Handle hDevice, uint8_t *packet, unsigned int size);
void BTNF_AsynchPHY(BTNF_Handle hDevice, void *packetData); /* send a PHY packet */
#endif

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

#if 0 /* use this template for the new functions */
BERR_Code BTNF_xxx(
    BTNF_Handle hDevice /* [in] Device handle */
    );
#endif

#ifdef __cplusplus
}
#endif
 
#endif /* BTNF_H__ */

