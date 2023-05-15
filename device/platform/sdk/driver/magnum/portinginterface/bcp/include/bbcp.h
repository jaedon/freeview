/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bbcp.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 3/21/12 7:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/bcp/7346/bbcp.h $
 * 
 * Hydra_Software_Devel/4   3/21/12 7:34p agin
 * SWNOOS-518:  More design changes.
 * 
 * Hydra_Software_Devel/3   3/5/12 6:55p agin
 * SWNOOS-518:  Use pointer to structures instead of passing individual
 * variables.
 * 
 * Hydra_Software_Devel/2   3/4/12 9:52p agin
 * SWNOOS-518:  Support Nexus.
 * 
 * Hydra_Software_Devel/1   2/26/12 7:19p agin
 * SWNOOS-518:  Create BICAP PI.
 * 
 ***************************************************************************/


/*= Module Overview *********************************************************
<verbatim>

Overview
The ICP PI module controls the Input Capture core within the BCM7038.  
The ICP core supports up to 4 separate capture pins.  Each pin is individually
configurable as positive or negative edge triggered.  When a pin is triggered,
the count is captured and can be read.

Design
The design for BICP PI API is broken into two parts.
  Part 1 (open/close/configuration):
    These APIs are used for opening and closing BICP device/device channel.
    When a device channel is opened, the glitch rejector count for
	that channel can be programmed.
    
  Part 2 (command):
    These APIs are used to configure and arm the trigger.

Usage
The usage of BICP involves the following:
   * Configure/Open of BICP
      * Configure BICP device for the target system
      * Open BICP device
      * Configure BICP device channel for the target system
      * Open BICP device channel
   * Configure and arm the trigger
      * Configure edge triggerring
   * Get count after trigger
      * Read counter after edge is triggered

Sample Code
void main( void )
{
    BICP_Handle       hIcp;
    BICP_ChannelHandle   hIcpChan;
    BREG_Handle       hReg;
    BCHP_Handle       hChip;
    BINT_Handle       hInt;
    BICP_ChannelSettings defChnSettings;
    unsigned int ChanNo;
    bool triggered = FALSE;
    uint16_t cnt;

    // Do other initialization, i.e. for BREG, BCHP, etc
    BBCP_Open( &hIcp, hChip, hReg, hInt, (BICP_Settings *)NULL );
   
    ChanNo = 0;	// example for channel 0
    BICP_GetChannelDefaultSettings( hIcp, ChanNo, &defChnSettings );

    // Make any changes required from the default values
    defChnSettings.rejectCnt = 8;
    BBCP_OpenChannel( hIcp, &hIcpChan, ChanNo, &defChnSettings );

    // Set pin to negative edge trigger
    BICP_EnableEdge (hIcpChan, BICP_EdgeConfig_eNegative);

    // Wait until the pin is triggered
    do
    {
        BICP_PollTimer (hIcpChan, &triggered, &cnt);
    } while (!triggered);
}

</verbatim>
***************************************************************************/


#ifndef BBCP_H__
#define BBCP_H__

#include "bchp.h"
#include "breg_mem.h"
#include "bint.h"
#include "bkni.h"
#include "berr_ids.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
	Maximum number of PWM channels supported

Description:

See Also:

****************************************************************************/
#define MAX_BCP_CHANNELS			2

/***************************************************************************
Summary:
	Error Codes specific to BBCP

Description:

See Also:

****************************************************************************/
#define BBCP_ERR_NOTAVAIL_CHN_NO			BERR_MAKE_CODE(BERR_BCP_ID, 0)

/***************************************************************************
Summary:
	The handles for icp module.

Description:
	Since BBCP is a device channel, it has main device handle as well
	as a device channel handle.

See Also:
	BBCP_Open(), BBCP_OpenChannel()

****************************************************************************/
typedef struct BBCP_P_Handle				*BBCP_Handle;
typedef struct BBCP_P_ChannelHandle			*BBCP_ChannelHandle;

/***************************************************************************
Summary:
	Enumeration for edge configuration

Description:
	This enumeration defines edge trigger configuration for each ICP pin

See Also:
	None.

****************************************************************************/
typedef enum
{
   BBCP_EdgeConfig_eNegative,
   BBCP_EdgeConfig_ePositive,
   BBCP_EdgeConfig_eBoth
} BBCP_EdgeConfig;

/***************************************************************************
Summary:
	Required default settings structure for ICP module.

Description:
	The default setting structure defines the default configure of
	ICP when the device is opened.  Since BBCP is a device
	channel, it also has default settings for a device channel.
	Currently there are no parameters for device setting.

See Also:
	BBCP_Open(), BBCP_OpenChannel()

****************************************************************************/
typedef struct BBCP_Settings
{
   bool					intMode;		/* interrupt mode flag */
} BBCP_Settings;

typedef struct BBCP_Edge_Settings
{
	uint32_t 			tout_clk_div;	/* clk divisor for tout  */
	uint32_t 			sys_clk_div;	/* clk divisor for BICAP clk */
	uint32_t 			cnt_mode;		/* count mode */
	uint32_t 			pedgedet_en; 	/* rising edge enable */
	uint32_t 			nedgedet_en; 	/* falling edge enable */
	uint32_t 			insig_inv;		/* input signal invert */
	uint32_t 			bicap_en;  		/* bicap enable */
} BBCP_Edge_Settings;

typedef struct BBCP_Filter_Settings
{
	uint32_t 			bypass;	 		/* bypass filter */
	uint32_t  			clk_sel;		/* which clock */
	uint32_t  			val;	 		/* value */
} BBCP_Filter_Settings;

typedef struct BBCP_Timeout_Settings
{
	uint32_t 			input_sel;
	uint32_t 			edge_sel;
	uint32_t 			clk_sel;
	uint32_t 			tout;
} BBCP_Timeout_Settings;

typedef struct BBCP_FifoInactTimeout_Settings
{
	uint32_t 			bicap_clk_sel;
	uint32_t 			event_sel;
	uint32_t 			clk_sel;
	uint32_t 			tout;
} BBCP_FifoInactTimeout_Settings;

typedef struct BBCP_ChannelSettings
{
	BBCP_Edge_Settings 		edge;
	BBCP_Filter_Settings 	filter;
} BBCP_ChannelSettings;

/***************************************************************************
Summary:
	Callback used for event notification.

Description:
	When this PI wants to notify an application, it will call this callback
	function the callback function is registered.

See Also:
	None.

****************************************************************************/
typedef void (*BBCP_Callback)(uint16_t ctrl, uint8_t mode, uint16_t data);

/***************************************************************************
Summary:
	This function opens ICP module.

Description:
	This function is responsible for opening BBCP module. When BBCP is
	opened, it will create a module handle and configure the module based
	on the default settings. Once the device is opened, it must be closed
	before it can be opened again.

Returns:
	TODO:

See Also:
	BBCP_Close(), BBCP_OpenChannel(), BBCP_CloseChannel(),
	BBCP_GetDefaultSettings()

****************************************************************************/
BERR_Code BBCP_Open(
	BBCP_Handle *pICP,					/* [out] Returns handle */
	BCHP_Handle hChip,					/* [in] Chip handle */
	BREG_Handle hRegister,				/* [in] Register handle */
	BINT_Handle hInterrupt,				/* [in] Interrupt handle */
	const BBCP_Settings *pDefSettings	/* [in] Default settings */
	);

/***************************************************************************
Summary:
	This function closes ICP module.

Description:
	This function is responsible for closing BBCP module. Closing BBCP 
	will free main BBCP handle. It is required that all opened 
	BBCP channels must be closed before calling this function. If this 
	is not done, the results will be unpredicable.

Returns:
	TODO:

See Also:
	BBCP_Open(), BBCP_CloseChannel()

****************************************************************************/
BERR_Code BBCP_Close(
	BBCP_Handle hDev					/* [in] Device handle */
	);

/***************************************************************************
Summary:
	This function returns the default settings for ICP module.

Description:
	This function is responsible for returns the default setting for 
	BBCP module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BBCP_Open()

****************************************************************************/
BERR_Code BBCP_GetDefaultSettings(
	BBCP_Settings *pDefSettings		/* [out] Returns default setting */
	);

/***************************************************************************
Summary:
	This function returns the total number of channels supported by 
	ICP module.

Description:
	This function is responsible for getting total number of channels
	supported by BBCP module, since BBCP device is implemented as a
	device channel.

Returns:
	TODO:

See Also:
	BBCP_OpenChannel(), BBCP_ChannelDefaultSettings()

****************************************************************************/
BERR_Code BBCP_GetTotalChannels(
	BBCP_Handle 		hDev,					/* [in] Device handle */
	uint32_t 			*totalChannels			/* [out] Returns total number downstream channels supported */
	);

/***************************************************************************
Summary:
	This function gets default setting for a ICP module channel.

Description:
	This function is responsible for returning the default setting for
	channel of BBCP. The return default setting is used when opening
	a channel.

Returns:
	TODO:

See Also:
	BBCP_OpenChannel()

****************************************************************************/
BERR_Code BBCP_GetChannelDefaultSettings(
	BBCP_Handle 		hDev,					/* [in] Device handle */
	uint32_t 			channelNo,				/* [in] Channel number to default setting for */
    BBCP_ChannelSettings *pChnDefSettings 		/* [out] Returns channel default setting */
    );

/***************************************************************************
Summary:
	This function opens ICP module channel.

Description:
	This function is responsible for opening BBCP module channel. When a
	BBCP channel is	opened, it will create a module channel handle and
	configure the module based on the channel default settings. Once a 
	channel is opened, it must be closed before it can be opened again.

Returns:
	TODO:

See Also:
	BBCP_CloseChannel(), BBCP_GetChannelDefaultSettings()

****************************************************************************/
BERR_Code BBCP_OpenChannel(
	BBCP_Handle 		hDev,					/* [in] Device handle */
	BBCP_ChannelHandle 	*phChn,					/* [out] Returns channel handle */
	uint32_t 			channelNo,				/* [in] Channel number to open */
	const BBCP_ChannelSettings *pChnDefSettings /* [in] Channel default setting */
	);

/***************************************************************************
Summary:
	This function closes ICP module channel.

Description:
	This function is responsible for closing BBCP module channel. Closing
	BBCP channel it will free BBCP channel handle. It is required that all
	opened BBCP channels must be closed before closing BBCP.

Returns:
	TODO:

See Also:
	BBCP_OpenChannel(), BBCP_CloseChannel()

****************************************************************************/
BERR_Code BBCP_CloseChannel(
	BBCP_ChannelHandle 	hChn				/* [in] Device channel handle */
	);

/***************************************************************************
Summary:
	This function gets ICP module device handle based on
	the device channel handle.

Description:
	This function is responsible returning BBCP module handle based on the
	BBCP module channel.

Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BBCP_GetDevice(
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	BBCP_Handle 		*pICP			/* [out] Returns Device handle */
	);

/***************************************************************************
Summary:
	This function gets all the values of the edge detection.

Description:
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_GetEdge( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	BBCP_Edge_Settings 	*pEdge			/* [out] edge information */
	);

/***************************************************************************
Summary:
	This function sets all the values of the edge detection.

Description:
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_SetEdge( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	BBCP_Edge_Settings 	*pEdge			/* [out] edge information */
	);

/***************************************************************************
Summary:
	This function sets the value of the tout_clk_div.

Description:
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_SetToutClkDiv( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	uint32_t			val				/* [in] value */
	);


/***************************************************************************
Summary:
	This function sets the value of the sys_clk_div.

Description:
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_SetSysClkDiv( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	uint32_t			val				/* [in] value */
	);

/***************************************************************************
Summary:
	This function gets the cnt mode.

Description:
	This function gets the cnt mode.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_GetCntMode( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	uint32_t			*mode			/* [out] mode */
	);

/***************************************************************************
Summary:
	This function sets the cnt mode.

Description:
	This function sets the cnt mode.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_SetCntMode( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	uint32_t			mode 			/* [in] mode */
	);

/***************************************************************************
Summary:
	This function enables an edge trigger

Description:
	This function is used to configure an edge and arm the trigger.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_EnableEdge( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	BBCP_EdgeConfig 	edge			/* [in] edge config */
	);

/***************************************************************************
Summary:
	This function disables an edge trigger

Description:
	This function is used to configure an edge and disable the trigger.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_DisableEdge( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	BBCP_EdgeConfig 	edge			/* [in] edge config */
	);

/***************************************************************************
Summary:
	This function gets the value of the edge trigger

Description:
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_GetRisingEdge( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	bool 	*edge						/* [out] edge value */
	);

/***************************************************************************
Summary:
	This function gets the value of the falling trigger

Description:
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_GetFallingEdge( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	bool 	*edge						/* [out] edge value */
	);

/***************************************************************************
Summary:
	This function gets the value of the invert.

Description:
	This function gets the value of the invert.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_GetInvertInput( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	bool 				*invert			/* [out] invert value */
	);

/***************************************************************************
Summary:
	This function sets the value of the invert.

Description:
	This function sets the value of the invert.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_SetInvertInput( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	bool 				invert			/* [in] invert value */
	);

/***************************************************************************
Summary:
	This function gets the BICAP clock.

Description:
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_GetBicapEnable( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	bool 				*enable			/* [in] enable */
	);

/***************************************************************************
Summary:
	This function sets the BICAP clock.

Description:
	This function sets the BICAP clock.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_SetBicapEnable( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	bool 	enable						/* [in] enable */
	);

/***************************************************************************
Summary:
	This function gets the BICAP filter info.

Description:
	This function gets the BICAP filter info.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
void BBCP_GetFilter( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	BBCP_Filter_Settings	*filter		/* [out] filter settings */
	);

/***************************************************************************
Summary:
	This function sets the BICAP filter info.

Description:
	This function sets the BICAP filter info.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_SetFilter( 
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	BBCP_Filter_Settings	*filter		/* [out] filter settings */
	);

/***************************************************************************
Summary:
	This function gets the BICAP timeout info.

Description:
	This function gets the BICAP timeout info.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_GetTimeout( 
	BBCP_Handle 		hDev,			/* [in] Device handle */
	uint32_t 			which,
	BBCP_Timeout_Settings	*timeout	/* {out} timeout settings */
	);

/***************************************************************************
Summary:
	This function sets the BICAP timeout info.

Description:
	This function sets the BICAP timeout info.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_SetTimeout( 
	BBCP_Handle 		hDev,			/* [in] Device handle */
	uint32_t 			which,
	BBCP_Timeout_Settings	*timeout	/* {in} timeout settings */
	);

/***************************************************************************
Summary:
	This function gets the BICAP FIFO inactivity timeout info.

Description:
	This function gets the BICAP FIFO inactivity timeout info.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_GetFifoInactTimeout( 
	BBCP_Handle 		hDev,			/* [in] Device handle */
	BBCP_FifoInactTimeout_Settings *inact
	);

/***************************************************************************
Summary:
	This function sets the BICAP FIFO inactivity timeout info.

Description:
	This function sets the BICAP FIFO inactivity timeout info.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_SetFifoInactTimeout( 
	BBCP_Handle 		hDev,			/* [in] Device handle */
	BBCP_FifoInactTimeout_Settings *inact
	);

/***************************************************************************
Summary:
	This function empties out all the elements in the fifo.

Description:
	This function empties out all the elements in the fifo.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_GetFifoData( 
	BBCP_Handle 		hDev,			/* [in] Device handle */
	uint32_t			*count,			/* [out] number of elements */
	uint32_t			*data			/* [out] pointer to data */
	);

/***************************************************************************
Summary:
	This function retrives at most only one fifo element.

Description:
	This function retrives at most only one fifo element.  But it still
	returns the actual count of elements.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_GetOneFifoData( 
	BBCP_Handle 		hDev,			/* [in] Device handle */
	uint32_t			*count,			/* [out] number of elements */
	uint32_t			*data			/* [out] pointer to data */
	);

/***************************************************************************
Summary:
	This function sets the FIFO control information.

Description:
	This function sets the FIFO control information.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_SetFifo( 
	BBCP_Handle 		hDev,			/* [in] Device handle */
	uint32_t			trig_lvl,		/* [in] when fifo reaches this level, event is generated */
	uint32_t			reset			/* [in] reset the fifo */
	);

/***************************************************************************
Summary:
	This function retrieves FIFO control information.

Description:
	This function retrieves FIFO control information.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_GetFifo( 
	BBCP_Handle 		hDev,			/* [in] Device handle */
	uint32_t			*fifo_depth,    /* [out] size of the fifo */
	uint32_t			*trig_lvl		/* [out] current value of the trigger level */
	);

/***************************************************************************
Summary:
	This function sets the FIFO trigger level.

Description:
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_SetFifoTrigLevel( 
	BBCP_Handle 		hDev,			/* [in] Device handle */
	uint16_t 			level		    /* [in] trigger level */
	);

/***************************************************************************
Summary:
	This function resets the FIFO.

Description:
	This function resets the FIFO.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
BERR_Code BBCP_ResetFifo( 
	BBCP_ChannelHandle 	hChn			/* [in] Device channel handle */
	);

/***************************************************************************
Summary:
	This function gets the FIFO status.

Description:
	This function gets the FIFO status.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
uint16_t BBCP_GetFifoStatus( 
	BBCP_Handle 		hDev,				/* [in] Device handle */
	uint32_t			*fifo_count,		/* [in] */
	uint32_t			*fifo_full,			/* [in] */
	uint32_t			*fifo_empty,		/* [in] */
	uint32_t			*fifo_overflow,		/* [in] */
	uint32_t			*fifo_lvl_event,	/* [in] */
	uint32_t			*fifo_inact_event	/* [in] */
	);

/***************************************************************************
Summary:
	This function gets the FIFO count.

Description:
	This function gets the FIFO count.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
uint16_t BBCP_GetFifoCount( 
	BBCP_Handle 		hDev			/* [in] Device handle */
	);

/***************************************************************************
Summary:
	This function checks to see if the FIFO is full.

Description:
	This function checks to see if the FIFO is full.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
bool BBCP_IsFifoFull( 
	BBCP_Handle 		hDev			/* [in] Device handle */
	);

/***************************************************************************
Summary:
	This function checks to see if the FIFO is empty.

Description:
	This function checks to see if the FIFO is empty.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
bool BBCP_IsFifoEmpty( 
	BBCP_Handle 		hDev			/* [in] Device handle */
	);

/***************************************************************************
Summary:
	This function checks to see if the FIFO overflowed.

Description:
	This function checks to see if the FIFO overflowed.
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
bool BBCP_IsFifoOverflowed( 
	BBCP_Handle 		hDev			/* [in] Device handle */
	);

/***************************************************************************
Summary:
	This function gets the event handle for BBCP module channel.

Description:
	This function is responsible for getting the event handle. The
	application code should use this function get BBCP's event handle, 
	which the application should use to pend on.  The ICP ISR will 
	set the event.
	
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BBCP_GetEventHandle(
	BBCP_Handle 		hChn,	  		/* [in] Device handle */
	BKNI_EventHandle 	*phEvent  		/* [out] Returns event handle */
	);

/***************************************************************************
Summary:
	This function gets the interrupt mode.

Description:
	
Returns:
	TODO:

See Also:

****************************************************************************/
bool BBCP_IntMode(
	BBCP_Handle 		hDev			/* [in] Device handle */
	);

/***************************************************************************
Summary:
	This function enables the FIFO overflow interrupt..

Description:
	
Returns:
	TODO:

See Also:

****************************************************************************/
void BBCP_P_EnableFifoOfInt(
	BBCP_Handle 		hDev			/* Device handle */
	);

/***************************************************************************
Summary:
	This function enables the FIFO level interrupt..

Description:
	
Returns:
	TODO:

See Also:

****************************************************************************/
void BBCP_P_EnableFifoIrqInt(
	BBCP_Handle 		hDev			/* Device handle */
	);

/***************************************************************************
Summary:
	This function enables the FIFO inactivity interrupt..

Description:
	
Returns:
	TODO:

See Also:

****************************************************************************/
void BBCP_P_EnableCpuTimeoutInt(
	BBCP_Handle 		hDev			/* Device handle */
	);

/***************************************************************************
Summary:
	This function enables the edge timeout 0 interrupt..

Description:
	
Returns:
	TODO:

See Also:

****************************************************************************/
void BBCP_P_EnableEdgeTimeoutInt(
	BBCP_Handle 		hDev,			/* Device handle */
	uint32_t			timeout
	);

/***************************************************************************
Summary:
	This function disables the FIFO overflow interrupt..

Description:
	
Returns:
	TODO:

See Also:

****************************************************************************/
void BBCP_P_DisableFifoOfInt(
	BBCP_Handle 		hDev			/* Device handle */
	);

/***************************************************************************
Summary:
	This function disables the FIFO level interrupt..

Description:
	
Returns:
	TODO:

See Also:

****************************************************************************/
void BBCP_P_DisableFifoIrqInt(
	BBCP_Handle 		hDev			/* Device handle */
	);

/***************************************************************************
Summary:
	This function disables the FIFO inactivity interrupt..

Description:
	
Returns:
	TODO:

See Also:

****************************************************************************/
void BBCP_P_DisableCpuTimeoutInt(
	BBCP_Handle 		hDev			/* Device handle */
	);

/***************************************************************************
Summary:
	This function disables the edge timeout interrupt..

Description:
	
Returns:
	TODO:

See Also:

****************************************************************************/
void BBCP_P_DisableEdgeTimeoutInt(
	BBCP_Handle 		hDev,			/* Device handle */
	uint32_t			timeout
	);
	
/***************************************************************************
Summary:
	This function resets the interrupt count.

Description:
	
Returns:
	TODO:

See Also:

****************************************************************************/
void BBCP_ResetIntCount(
	BBCP_Handle 		hDev			/* [in] Device handle */
	);
	
/***************************************************************************
Summary:
	This function gets the interrupt count.

Description:
	
Returns:
	TODO:

See Also:

****************************************************************************/
void BBCP_GetIntCount(
	BBCP_Handle 		hDev,			/* [in] Device handle */
	uint32_t 			*data
	);
	
/***************************************************************************
Summary:
	This function enables the handling of the RC6 protocol

Description:
	
Returns:
	TODO:

See Also:

****************************************************************************/
void BBCP_EnableRC6(
	BBCP_ChannelHandle 	hChn,			/* [in] Device channel handle */
	BBCP_Callback 		pCallback  		/* Pointer to completion callback. */
	);

/***************************************************************************
Summary:
	This function disables the handling of the RC6 protocol

Description:
	
Returns:
	TODO:

See Also:

****************************************************************************/
void BBCP_DisableRC6(
	BBCP_ChannelHandle 	hChn				/* [in] Device channel handle */
	);

#ifdef __cplusplus
}
#endif
 
#endif
