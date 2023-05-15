/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_hw.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/12/08 4:42p $
 * Broadcom Security Processor Code
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7601/a0/common/aegis/bsp_s_hw.h $
 * 
 * Hydra_Software_Devel/1   8/12/08 4:42p atruong
 * PR45631: HSM PI support for BCM7601 platform
 * 
 * Hydra_Software_Devel/3   10/26/06 4:20p lyang
 * PR 23412: hsm pi include files should be re-factored so they are not
 * order dependent. PR 25133:HSM header files should include dependencies
 * 
 * Hydra_Software_Devel/2   4/26/06 11:07a btan
 * PR 20446: HSM needs to compile for 7400 A0
 * 
 * Hydra_Software_Devel/1   3/1/06 10:40a btan
 * PR 19931: Added HSM support for 7400A0
 * 
 * Hydra_Software_Devel/1   1/20/06 5:11p btan
 * PR 19140: Do not encrypt generic mode files.
 * 
 * Hydra_Software_Devel/4   1/17/06 9:39p btan
 * PR 19140: Do not encrypt generic mode files.
 * 
 * Aegis_Firmware_Devel/4   1/6/06 1:48a btan
 * PR 255: Update shared files to sync up both Aegis and Host sides.
 * 
 * Aegis_Firmware_Devel/3   9/21/05 1:36p grasche
 * PR 186: "NONSEC" to "CONTROL" variable names
 * 
 * Aegis_Firmware_Devel/2   9/19/05 7:01p btan
 * PR 185: Updated the shared file for host compilation
 * 
 * Aegis_Firmware_Devel/1   9/19/05 3:10p btan
 * PR 185: Updated the shared file for host compilation
 * 
 * 
 * 
 ***************************************************************************/
#ifndef BSP_S_HW_7400_H__
#define BSP_S_HW_7400_H__


/* multi2 system keys supported by hardware*/ 
#define BCMD_MULTI2_MAXSYSKEY     4
/* Total number of pid channels supported */
#define BCMD_TOTAL_PIDCHANNELS    256
/* User Ram defines for 2048 bit ram*/ 
#define BCMD_USERKEY_KEYRAM2048_SIZE  256 /* in bytes */

/* Mem2mem key table boundary assigned to key slot*/
#define BCMD_MEM2MEMKEYSLOTSIZE  6
#define BCMD_MEM2MEMKEYTABLE_BASE 0
#define BCMD_MEM2MEMKEYTABLE_TOP  96 

#define BCMD_MAX_M2M_KEY_SLOT   ((BCMD_MEM2MEMKEYTABLE_TOP-BCMD_MEM2MEMKEYTABLE_BASE)/BCMD_MEM2MEMKEYSLOTSIZE)

typedef enum BHSM_PidChannelType {
	BHSM_PidChannelType_ePrimary, 
	BHSM_PidChannelType_eSecondary,
	BHSM_PidChannelType_eMax
} BHSM_PidChannelType_e;


/* Move to hw shared file */
#ifdef BHSM_HOST_MIPS
/* Host side */
#define BHSM_IN_BUF1_ADDR		BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE
#define BHSM_IN_BUF2_ADDR  		BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE + (BCMD_BUFFER_BYTE_SIZE * 1)
#define BHSM_OUT_BUF1_ADDR  	BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE + (BCMD_BUFFER_BYTE_SIZE * 2)
#define BHSM_OUT_BUF2_ADDR  	BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE + (BCMD_BUFFER_BYTE_SIZE * 3)


#else
/* Aegis Side */

#ifdef MAIN_C__ 
    
/* Host/BSP Comm Buffer Allocation */

/*  0x7800 + (0*384) = 0x7800 */
BSP_PUBLIC XDATA UINT8 BCMD_CONTROL_inBuffer1[BCMD_BUFFER_BYTE_SIZE] \
    _at_ BSP_CMDBUF_DMEM0;

/*  0x7800 + (1*384) = 0x7980 */
BSP_PUBLIC XDATA UINT8 BCMD_CONTROL_inBuffer2[BCMD_BUFFER_BYTE_SIZE] \
    _at_ BSP_CMDBUF_DMEM0 + (BCMD_BUFFER_BYTE_SIZE * 1);
    
/*  0x7800 + (2*384) = 0x7B00 */
BSP_PUBLIC XDATA UINT8 BCMD_CONTROL_outBuffer1[BCMD_BUFFER_BYTE_SIZE] \
    _at_ BSP_CMDBUF_DMEM0 + (BCMD_BUFFER_BYTE_SIZE * 2);

/*  0x7800 + (3*384) = 0x7C80 */
BSP_PUBLIC XDATA UINT8 BCMD_CONTROL_outBuffer2[BCMD_BUFFER_BYTE_SIZE] \
    _at_ BSP_CMDBUF_DMEM0 + (BCMD_BUFFER_BYTE_SIZE * 3);

#if defined(BSP_BROADCOM_ONLY)
/*  0x7800 + (4*384) = 0x7E00 */
/* Note: OTP code writes directly to the memory and does not use this variable */
BSP_PUBLIC XDATA UINT8 BCMD_CONTROL_outBuffer2OtpRetryCount[BCMD_BUFFER_BYTE_SIZE] \
    _at_ BSP_CMDBUF_DMEM0 + (BCMD_BUFFER_BYTE_SIZE * 4);
#endif

#if !defined(BCM7401A0) /* this is a 7401 B0 change */
/*  0x7800 + (5*384) = 0x7f80 to 0x7FD3 */
BSP_PUBLIC XDATA UINT8 BCMD_CONTROL_inBufferCR[BCMD_CR_IN_BUFFER_BYTE_SIZE] \
    _at_ BSP_CMDBUF_DMEM0 + (BCMD_BUFFER_BYTE_SIZE * 5);
BSP_PUBLIC XDATA UINT8 BCMD_CONTROL_outBufferCR[BCMD_CR_OUT_BUFFER_BYTE_SIZE] \
    _at_ BSP_CMDBUF_DMEM0 + (BCMD_BUFFER_BYTE_SIZE * 5) + BCMD_CR_IN_BUFFER_BYTE_SIZE;
#endif

/*  0x7fD4 to 0x7FFF available */

     
#else

extern BSP_PUBLIC UINT8 BCMD_CONTROL_inBuffer1[BCMD_BUFFER_BYTE_SIZE];
extern BSP_PUBLIC UINT8 BCMD_CONTROL_inBuffer2[BCMD_BUFFER_BYTE_SIZE];
extern BSP_PUBLIC UINT8 BCMD_CONTROL_outBuffer1[BCMD_BUFFER_BYTE_SIZE];
extern BSP_PUBLIC UINT8 BCMD_CONTROL_outBuffer2[BCMD_BUFFER_BYTE_SIZE];

#if defined(BSP_BROADCOM_ONLY)
extern BSP_PUBLIC UINT8 BCMD_CONTROL_outBuffer2OtpRetryCount[BCMD_BUFFER_BYTE_SIZE];
#endif

#if !defined(BCM7401A0) /* this is a 7401 B0 change */
extern BSP_PUBLIC UINT8 BCMD_CONTROL_inBufferCR[BCMD_CR_IN_BUFFER_BYTE_SIZE];
extern BSP_PUBLIC UINT8 BCMD_CONTROL_outBufferCR[BCMD_CR_OUT_BUFFER_BYTE_SIZE];
#endif


#endif   /* end of #ifdef MAIN_C__ */

#endif /* end of #ifdef BHSM_HOST_MIPS */

#endif  /* BSP_S_HW_7400_H__ end of header file*/

