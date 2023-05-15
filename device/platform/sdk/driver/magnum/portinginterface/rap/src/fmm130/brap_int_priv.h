/***************************************************************************
*     Copyright (c) 2004-2006, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_int_priv.h $
* $brcm_Revision: Hydra_Software_Devel/14 $
* $brcm_Date: 12/13/06 4:41p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_int_priv.h $
* 
* Hydra_Software_Devel/14   12/13/06 4:41p speter
* PR 25346: [3563] SPDIF/HDMI In Feature
* 
* Hydra_Software_Devel/13   7/12/06 4:29p bselva
* PR 22288: Checking in the usage of timer for full mark interrupt
* 
* Hydra_Software_Devel/12   6/27/06 2:00p bhanus
* PR 22288 : Checking in timer workaround for water mark interrupt bug
* 
* Hydra_Software_Devel/11   4/12/06 1:40p bselva
* PR 20736: Implemented the Interrupt code review comments
* 
* Hydra_Software_Devel/9   11/2/05 3:26p bselva
* PR 17590: Added watchdog support for 7401
* 
* Hydra_Software_Devel/8   10/18/05 4:55p rjain
* PR 17651: Changing checks for BCHP_CHIP ==/!= 7401 to check for
* BCHP_7411_VER instead
* 
* Hydra_Software_Devel/7   10/5/05 7:29p bselva
* PR 16148: Added interrupt support for 7401
* 
* Hydra_Software_Devel/6   9/30/05 12:14p rjain
* PR 16960: To work around the problem, we now enable the RBUF underflow
* interrupts on a channel start, and mask them on the first occurence
* 
* Hydra_Software_Devel/5   9/22/05 10:02p nitinb
* PR 16889: Added prototypes of private functions installing and
* servicing device level interrupts
* 
* Hydra_Software_Devel/4   8/27/05 6:22a rjain
* PR 16731: added Pi to disable and destroy interrupt callback on channel
* close.
* 
* Hydra_Software_Devel/3   5/30/05 6:50p rjain
* PR 15272: Updating interrupt infrastructure
* 
* Hydra_Software_Devel/2   5/25/05 7:33p nitinb
* PR 15272: Removing prototype of function BRAP_P_FirstPTS_isr
* 
* Hydra_Software_Devel/1   5/25/05 4:45p rjain
* PR 15272: Adding interrupt handling framework.
* 
***************************************************************************/

#ifndef BRAP_INT_PRIV_H__
#define BRAP_INT_PRIV_H__

#include "brap_priv.h"


#ifdef __cplusplus
extern "C" {
#endif



    
    

/******************************************************************************
Summary: 
    This structure maintains information for the BRAP application callbacks 
    associated with a Raptor Interrupt.
    
Note:  
    The parameters passed to BRAP_CallbackFunc are
        void *    pParm1    : Application specific (passed unchanged)
        int       iParm2    : Application specific (passed unchanged)
        void *    pRAP_data : Raptor specific data exchanged between the 
                              BINT_Callback and the Application callback
******************************************************************************/
typedef struct BRAP_P_AppIntCbInfo
{
    BRAP_CallbackFunc pfAppCb;  /* Application specified Callback fnc */
    void *            pParm1;   /* Application specified parameter that needs 
                                   to be passed unchanged to the callback */
    int               iParm2;   /* Application specified parameter that needs 
                                   to be passed unchanged to the callback */
} BRAP_P_AppIntCbInfo;



/***************************************************************************
Summary:
	Clear all previous Raptor interrupts and Mask ALL raptor interrupts

Description:    
    This should be called in device open ie BRAP_Open(). It masks all raptor 
    interrupts making sure no interrupts come before the interrupt interface
    has been installed.

****************************************************************************/
BERR_Code BRAP_P_ClearInterrupts (
        BREG_Handle hRegister   /* [in] The register handle */
);


/***************************************************************************
Summary:
	Initialises interrupt handling in Raptor

Description:    
    This installs a BINT callback for the top level Audio L2 interrupt  
    for each Raptor channel (with Param1 as the raptor channel handle, 
    param2 is not used).

    This may differ across chip versions.

****************************************************************************/
BERR_Code BRAP_P_InterruptInstall (
	BRAP_ChannelHandle 		hRapCh		/* [in] Raptor Channel handle */
);

#ifdef BCHP_7411_VER /* Only for the 7411 */ 
/***************************************************************************
Summary:
	Enables the RBUF underflow interrupts. Should be called on a channel start.

Description:    
   PR 16544, 16960: on a channel stop, first the DSP is stopped and then 
   the RBUF. Depending on the timing underflows may/not occur. Its seen 
   that the RBUF underflow interrupt is triggered repeatedly instead 
   of only once. Hence  the processor is stuck in the ISR, there's a 
   deadlock and things dont move ahead.

    To avoid a race condition on a channel stop, make sure that the underflow interrupt   
    is masked immediately on the first occurence. Then unmask it when the channel 
    is re-started  This is required only for the 7411, not for the 7401	

****************************************************************************/
BERR_Code BRAP_P_EnableRbufUnderflowInt (
	BRAP_ChannelHandle 		hRapCh		/* [in] Raptor Channel handle */
);
#endif

/***************************************************************************
Summary:
	CLoses interrupt handling in Raptor

Description:    
    This may differ across chip versions.

****************************************************************************/
BERR_Code BRAP_P_InterruptUnInstall (
	BRAP_ChannelHandle 		hRapCh		/* [in] Raptor Channel handle */
);


#ifdef BCHP_7411_VER /* Only for the 7411 */
/***************************************************************************
Summary:
	Top level Interrupt Service Routine for RAptor

Description:    
    IN 7411, all raptor interrupts are L3 and fan out from a common L2 interrupt.
    This is the ISR for the top level L2 interrupt.
    Internally, this checks the various Raptor ESR registers to see which 
    interrupt has occured and calls the corresponding ISRs.

****************************************************************************/
void BRAP_P_TopLevel_isr (
        void * pParm1, /* [in] Raptor channel handle */
        int    iParm2  /* [in] Not used */        
);
#else
void BRAP_P_DSP2Host_isr (
        void * pParm1, /* [in] Raptor channel handle */
        int    iParm2  /* [in] Not used */        
);

void BRAP_P_FMM_BF_isr (
        void * pParm1, /* [in] Raptor channel handle */
        int    iParm2  /* [in] Not used */        
);
#if ( BCHP_CHIP == 3563 )
void BRAP_P_SPDIF_RX_isr (
        void * pParm1, /* [in] Raptor channel handle */
        int    iParm2  /* [in] Not used */        
);
#endif
#endif

BERR_Code BRAP_P_DeviceLevelInterruptInstall (
	BRAP_Handle		hRap
);

BERR_Code BRAP_P_DeviceLevelInterruptUnInstall (
	BRAP_Handle		hRap
);

#ifdef BCHP_7411_VER /* Only for the 7411 */
void BRAP_P_TopLevelDevice_isr (
		void *pParm1,	/* [in] Raptor handle */
		int iParm2		/* [in] Not used */
);
#endif

void BRAP_P_FmmRbufFreeWaterMark_isr (
        void * pParm1, /* [in] Raptor channel handle */
        int    iParm2  /* [in] RBUF index */
);

void BRAP_P_FmmDstRbufFullWaterMark_isr (
	void *pParm1, /* [in] Raptor channel handle */
	int dummy /* [in] RBUF index */
);

#if 0 /* To be used for 7401 where the raptor intrpts are L2 */
    
/******************************************************************************
Summary: This structure maintains information for the BINT interface. 
It keeps track of the BINT_Id, BINT_CallbackFunc and the BINT_CallbackHandle 
associated with a Raptor Interrupt.

Note:  The parameters passed to BINT_CallbackFunc are
    void *  pParm1 : Raptor channel handle
     int    iParm2 : Internal Module Index. For eg. for DSP context 0, this will
                     be 0, for DSP context 1, it will be 1, for SrcCh4 it be be 
                     4 etc.
******************************************************************************/
typedef struct BRAP_P_L2IntCbInfo
{
    BINT_Id             IntId;         /* BINT Id*/
    BINT_CallbackFunc   pfCallback;    /* Interrupt callback function */
    BRAP_Interrupt      eInterrupt;    /* The interrupt type */    
    unsigned int        uiModuleIndex; /* Internal Module Index*/
} BRAP_P_L2IntCbInfo;


/*****************************************************************************
Summary:
  This function goes through the sL2IntCbInfo table and finds 
  out the index in the table of the desired Interrupt Id for this module. 
  This can be used to get the BINT id and ISR for a particular module interrupt
Returns:
    BERR_SUCCESS on success
    Error code on failure
******************************************************************************/
BERR_Code BRAP_P_GetInterruptIndex (
    BRAP_Interrupt  eInterrupt,      /* [in] The interrupt type */
    unsigned int    uiModuleIndex,   /* [in] Module index/context no. */
    unsigned int *  pIndex           /* [out] Index of corresponsing entry in
                                        sL2IntCbInfo */
);


/*****************************************************************************
Summary:
    Used to register all DSP PI related interrupt callbacks with the BINT
    interface and enable those interrupts

Returns:
    BERR_SUCCESS on success
    Error code on failure
******************************************************************************/
BERR_Code BRAP_P_InstallDspIntCb(
    BRAP_ChannelHandle hRapCh    /* [in] The RAP channel handle */
);

/*****************************************************************************
Summary:
    Used to register all FMM related interrupt callbacks with the BINT
    interface and enable those interrupts

Returns:
    BERR_SUCCESS on success
    Error code on failure
******************************************************************************/
BERR_Code BRAP_P_InstallFmmIntCb(
    BRAP_ChannelHandle      hRapCh,     /* [in] The RAP channel handle */
	BRAP_OutputChannelPair	eOpChannel	/* [in] Output Audio channel type */    
);
#endif



#ifdef __cplusplus
}
#endif

#endif /* BRAP_INT_PRIV_H__ */
/* End of File */    

