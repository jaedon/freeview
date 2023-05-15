/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bods_leap_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 12/7/12 2:16p $
 *
 * Module Description:
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ods/src/leap/bods_leap_priv.c $
 * 
 * Hydra_Software_Devel/15   12/7/12 2:16p atanugul
 * SW3472-36: Fix ResetStatus for DVB-C2 and DVB-T2
 * 
 * Hydra_Software_Devel/14   12/6/12 11:23a atanugul
 * SW7563-26: Add PI support for 7563
 * 
 * Hydra_Software_Devel/13   12/5/12 12:38p atanugul
 * SW3472-22: Merge to main
 * 
 * Hydra_Software_Devel/12   12/4/12 4:09p atanugul
 * SW3462-51: Add Support for GetVersionInfo() API
 * 
 * Hydra_Software_Devel/SW3472-17/2   9/18/12 3:13p atanugul
 * SW3472-22: Update signal strength resolution for ISDB-T
 * 
 * Hydra_Software_Devel/SW3472-17/1   9/17/12 3:17p atanugul
 * SW3472-22: Add SSI and SQI to ISDBT layer status
 * 
 * Hydra_Software_Devel/11   9/7/12 11:36a atanugul
 * SW7425-3788: restructure HAB directory
 * 
 * Hydra_Software_Devel/10   8/28/12 3:06p atanugul
 * SW3462-7: Cleanup
 * 
 * Hydra_Software_Devel/9   8/3/12 5:16p atanugul
 * SW3462-41: Send Receiver Configuration HAB Command to the FW
 * 
 * Hydra_Software_Devel/8   7/25/12 6:49p atanugul
 * SW3472-8: Fix BODS_Leap_ResetStatus()
 * 
 * Hydra_Software_Devel/7   7/20/12 6:40p atanugul
 * SW3472-8: Add support for 3472
 * 
 * Hydra_Software_Devel/6   7/19/12 5:40p atanugul
 * SW3472-8: Add support for 3472
 * 
 * Hydra_Software_Devel/5   7/19/12 12:07p atanugul
 * SW3472-8: Add EWS support to 3472; power on LEAP memories before FW
 * download for 3472
 * 
 * Hydra_Software_Devel/4   7/19/12 11:33a atanugul
 * SW3472-8: Add support for 3472
 * 
 * Hydra_Software_Devel/3   7/12/12 4:58p atanugul
 * SW3472-8: Add support for 3472
 * 
 * Hydra_Software_Devel/2   7/10/12 6:34p atanugul
 * SW3472-8: Add PI support for 3472
 * 
 * Hydra_Software_Devel/1   5/25/12 2:43p atanugul
 * SW3462-7: Initial Version
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bods.h"
#include "bhab.h"
#include "bods_priv.h"
#include "bods_leap_priv.h"
#if (BODS_CHIP == 7563)
#include "bchp_leap_ctrl.h"
#else
#include "../../a0/bchp_leap_ctrl.h"
#endif
BDBG_MODULE(bods_Leap_priv);

#define CHK_RETCODE( rc, func )             \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)

#if ((BODS_CHIP == 3462) || (BODS_CHIP == 7563))
#define MAX_DVBT_CHANNELS       (1)
#define MAX_DVBT2_CHANNELS      (1)
#define MAX_DVBC2_CHANNELS      (1)
#define MAX_ISDBT_CHANNELS      (0)
#elif (BODS_CHIP == 3472)
#define MAX_DVBT_CHANNELS       (0)
#define MAX_DVBT2_CHANNELS      (0)
#define MAX_DVBC2_CHANNELS      (0)
#define MAX_ISDBT_CHANNELS      (2)
#endif
#define MAX_ODS_CHANNELS        (MAX_DVBT_CHANNELS + MAX_DVBT2_CHANNELS + MAX_DVBC2_CHANNELS + MAX_ISDBT_CHANNELS)

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/
typedef struct BODS_P_Leap_Handle               *BODS_Leap_Handle;
typedef struct BODS_P_Leap_Handle
{
    BCHP_Handle hChip;
    BREG_Handle hRegister;
    BINT_Handle hInterrupt;
    BHAB_DevId devId;
    BHAB_Handle hHab;
    BODS_Version verInfo;
    unsigned int mxChnNo;
    BODS_ChannelHandle hOdsChn[MAX_ODS_CHANNELS];
} BODS_P_Leap_Handle;

typedef struct BODS_P_Leap_ChannelHandle            *BODS_Leap_ChannelHandle;
typedef struct BODS_P_Leap_ChannelHandle
{
    BODS_Handle hOds;
    unsigned int chnNo;
    BODS_Standard standard;
    BHAB_DevId devId;
    BHAB_Handle hHab;
    BODS_CallbackFunc pCallback[BODS_Callback_eLast];
    void *pCallbackParam[BODS_Callback_eLast];
    BODS_ChannelSettings settings;
    bool isLock;                        /* current lock status */
    BKNI_MutexHandle mutex;             /* mutex to protect lock status*/
    BHAB_InterruptType event;
    bool bPowerdown;
    BODS_Version verInfo;
    BODS_AcquireParams previousAcquireParams;   
    uint8_t readBuf[HAB_MEM_SIZE];
} BODS_P_Leap_ChannelHandle;

/*******************************************************************************
*
*   Private Module Functions
*
*******************************************************************************/
BERR_Code BODS_Leap_P_EventCallback_isr(
    void * pParam1, int param2
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_ChannelHandle hChn = (BODS_ChannelHandle) pParam1;
    BODS_Leap_ChannelHandle hImplChnDev;
    BHAB_InterruptType event = (BHAB_InterruptType) param2;

    BDBG_ENTER(BODS_Leap_P_EventCallback_isr);
    BDBG_ASSERT( hChn );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );


    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab);

    switch (event) {
        case BHAB_Interrupt_eLockChange:
            {
                if( hImplChnDev->pCallback[BODS_Callback_eLockChange] != NULL )
                {
                    (hImplChnDev->pCallback[BODS_Callback_eLockChange])(hImplChnDev->pCallbackParam[BODS_Callback_eLockChange] );
                }
            }
            break;
        case BHAB_Interrupt_eUpdateGain:
            {
                if( hImplChnDev->pCallback[BODS_Callback_eUpdateGain] != NULL )
                {
                    (hImplChnDev->pCallback[BODS_Callback_eUpdateGain])(hImplChnDev->pCallbackParam[BODS_Callback_eUpdateGain] );
                }
            }
            break;
        case BHAB_Interrupt_eNoSignal:
            {
                if( hImplChnDev->pCallback[BODS_Callback_eNoSignal] != NULL )
                {
                    (hImplChnDev->pCallback[BODS_Callback_eNoSignal])(hImplChnDev->pCallbackParam[BODS_Callback_eNoSignal] );
                }
            }
            break;
        case BHAB_Interrupt_eOdsAsyncStatusReady:
            {               
                if( hImplChnDev->pCallback[BODS_Callback_eAsyncStatusReady] != NULL )
                {            
                    (hImplChnDev->pCallback[BODS_Callback_eAsyncStatusReady])(hImplChnDev->pCallbackParam[BODS_Callback_eAsyncStatusReady] );
                }
            }
            break;
        case BHAB_Interrupt_eEmergencyWarningSystem:
            {               
                if( hImplChnDev->pCallback[BODS_Callback_eEmergencyWarningSystem] != NULL )
                {            
                    (hImplChnDev->pCallback[BODS_Callback_eEmergencyWarningSystem])(hImplChnDev->pCallbackParam[BODS_Callback_eEmergencyWarningSystem] );
                }
            }
            break;      
        default:
            BDBG_WRN((" unknown event code from leap"));
            break;
    }

    BDBG_LEAVE(BODS_Leap_P_EventCallback_isr);
    return retCode;
}

BERR_Code BODS_Leap_P_GetDvbtSelectiveAsyncStatus(
    BODS_ChannelHandle hChn,
    BODS_SelectiveStatus *pStatus   /* [out] */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;
    uint8_t buf[0x45] = HAB_MSG_HDR(BODS_eGetAsyncStatus, 0, BODS_DVBT_CORE_TYPE, BODS_CORE_ID);
    int16_t signalStrength;    

    BDBG_ENTER(BODS_Leap_P_GetDvbtSelectiveAsyncStatus);

    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );
    BDBG_ASSERT( pStatus );

    BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));

    buf[3] = hImplChnDev->chnNo;

    /* Get Status */
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, hImplChnDev->readBuf, 0x45, false, true, 0x45 ));

    pStatus->type = BODS_SelectiveAsyncStatusType_eDvbt;    
    pStatus->status.dvbt.receiverLock = (hImplChnDev->readBuf[0x5] & 0x1);
    pStatus->status.dvbt.fecLock = (hImplChnDev->readBuf[0x5] & 0x1);
    pStatus->status.dvbt.reacqCount = (hImplChnDev->readBuf[0x8] << 8) | hImplChnDev->readBuf[0x9] ;    
    pStatus->status.dvbt.snr = (hImplChnDev->readBuf[0xc] << 8) | hImplChnDev->readBuf[0xd];
    pStatus->status.dvbt.decodeMode = !((hImplChnDev->readBuf[0x18] & 0x2) >> 1);
    if (pStatus->status.dvbt.decodeMode == BODS_DvbtDecodeMode_eHighPriority)
        pStatus->status.dvbt.codeRate = (hImplChnDev->readBuf[0x19] & 0x7);
    else
        pStatus->status.dvbt.codeRate = (hImplChnDev->readBuf[0x19] & 0x70) >> 4;
    pStatus->status.dvbt.transmissionMode = (hImplChnDev->readBuf[0x5] & 0x30) >> 4;
    pStatus->status.dvbt.guardInterval = (hImplChnDev->readBuf[0x5] & 0xc0) >> 6;    
    pStatus->status.dvbt.modulation = (hImplChnDev->readBuf[0x18] & 0xc) >> 2;
    pStatus->status.dvbt.hierarchy = (hImplChnDev->readBuf[0x18] & 0x30) >> 4;
    pStatus->status.dvbt.gainOffset = (int32_t)((hImplChnDev->readBuf[0xE] << 8) | hImplChnDev->readBuf[0xF]);    
    pStatus->status.dvbt.carrierOffset = (hImplChnDev->readBuf[0x10] << 24) | (hImplChnDev->readBuf[0x11] << 16) | (hImplChnDev->readBuf[0x12] << 8) | hImplChnDev->readBuf[0x13];
    pStatus->status.dvbt.timingOffset = (hImplChnDev->readBuf[0x14] << 24) | (hImplChnDev->readBuf[0x15] << 16) | (hImplChnDev->readBuf[0x16] << 8) | hImplChnDev->readBuf[0x17];
    pStatus->status.dvbt.viterbiBer = (hImplChnDev->readBuf[0x24] << 24) | (hImplChnDev->readBuf[0x25] << 16) | (hImplChnDev->readBuf[0x26] << 8) | hImplChnDev->readBuf[0x27];
    pStatus->status.dvbt.preViterbiBer = (hImplChnDev->readBuf[0x20]  << 8) | ((hImplChnDev->readBuf[0x21]) << 16) | (hImplChnDev->readBuf[0x22] << 8) | hImplChnDev->readBuf[0x23];
    pStatus->status.dvbt.viterbiUncorrectedBits = 0; /* Not Supported */
    pStatus->status.dvbt.rsUncorrectedBlocks = (hImplChnDev->readBuf[0x3c] << 24) | (hImplChnDev->readBuf[0x3d] << 16) | (hImplChnDev->readBuf[0x3e] << 8) | hImplChnDev->readBuf[0x3f];
    pStatus->status.dvbt.rsCorrectedBlocks = (hImplChnDev->readBuf[0x38] << 24) | (hImplChnDev->readBuf[0x39] << 16) | (hImplChnDev->readBuf[0x3a] << 8) | hImplChnDev->readBuf[0x3b];
    pStatus->status.dvbt.rsCleanBlocks = (hImplChnDev->readBuf[0x34] << 24) | (hImplChnDev->readBuf[0x35] << 16) | (hImplChnDev->readBuf[0x36] << 8) | hImplChnDev->readBuf[0x37];
    pStatus->status.dvbt.rsTotalBlocks = (hImplChnDev->readBuf[0x30] << 24) | (hImplChnDev->readBuf[0x31] << 16) | (hImplChnDev->readBuf[0x32] << 8) | hImplChnDev->readBuf[0x33];
    pStatus->status.dvbt.spectrumInverted = (hImplChnDev->readBuf[0x5] & 0x8) >> 3;
    pStatus->status.dvbt.cellId = (hImplChnDev->readBuf[0x1c] << 8) | hImplChnDev->readBuf[0x1d];
    signalStrength= ((hImplChnDev->readBuf[0x40] << 8) | (hImplChnDev->readBuf[0x41] & 0xFF));
    pStatus->status.dvbt.signalStrength = signalStrength*100/256 + 4875;      
    pStatus->status.dvbt.signalLevelPercent = hImplChnDev->readBuf[0x42];
    pStatus->status.dvbt.signalQualityPercent = hImplChnDev->readBuf[0x43];  
    
done:    
    BDBG_LEAVE(BODS_Leap_P_GetDvbtSelectiveAsyncStatus);
    return retCode;
}

BERR_Code BODS_Leap_P_GetIsdbtSelectiveAsyncStatus(
    BODS_ChannelHandle hChn,
    BODS_SelectiveStatus *pStatus   /* [out] */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;
    uint8_t buf[141] = HAB_MSG_HDR(BODS_eGetAsyncStatus, 0, BODS_ISDBT_CORE_TYPE, BODS_CORE_ID);  

    BDBG_ENTER(BODS_Leap_P_GetIsdbtSelectiveAsyncStatus);

    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );
    BDBG_ASSERT( pStatus );

    BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));

    buf[3] = hImplChnDev->chnNo;

    /* Get Status */
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, hImplChnDev->readBuf, 141, false, true, 141 ));

    pStatus->type = BODS_SelectiveAsyncStatusType_eIsdbt;    
    pStatus->status.isdbt.receiverLock = (hImplChnDev->readBuf[5] & 0x1);
    pStatus->status.isdbt.fecLock = (hImplChnDev->readBuf[5] & 0x1);
    pStatus->status.isdbt.noSignalDetected = (hImplChnDev->readBuf[5] & 0x4) >> 2;
    pStatus->status.isdbt.transmissionMode = (hImplChnDev->readBuf[5] & 0x30) >> 4;
    pStatus->status.isdbt.guardInterval = (hImplChnDev->readBuf[5] & 0xC0) >> 6;
    pStatus->status.isdbt.gainOffset = (int32_t)((hImplChnDev->readBuf[14] << 8) | hImplChnDev->readBuf[15]);
    pStatus->status.isdbt.carrierOffset = (int32_t)((hImplChnDev->readBuf[16] << 24) | (hImplChnDev->readBuf[17] << 16) | (hImplChnDev->readBuf[18] << 8) | hImplChnDev->readBuf[19]);
    pStatus->status.isdbt.timingOffset = (int32_t)((hImplChnDev->readBuf[20] << 24) | (hImplChnDev->readBuf[21] << 16) | (hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23]);
    pStatus->status.isdbt.signalStrength = (int16_t)((hImplChnDev->readBuf[24] << 8) | hImplChnDev->readBuf[25])*100/256;    
    pStatus->status.isdbt.snr = (int16_t)((hImplChnDev->readBuf[12] << 8) | hImplChnDev->readBuf[13]);
    pStatus->status.isdbt.spectrumInverted = (hImplChnDev->readBuf[5] & 0x8) >> 3;
    pStatus->status.isdbt.reacqCount = (hImplChnDev->readBuf[8] << 8) | hImplChnDev->readBuf[9];
    pStatus->status.isdbt.ews = (hImplChnDev->readBuf[28] & 0x4) >> 2;
    pStatus->status.isdbt.partialReception = (hImplChnDev->readBuf[28] & 0x2) >> 2;
    pStatus->status.isdbt.layerAStatus.modulation = (BODS_IsdbtModulation)(hImplChnDev->readBuf[32] & 0x3);
    pStatus->status.isdbt.layerAStatus.codeRate = (BODS_IsdbtCodeRate)((hImplChnDev->readBuf[32] & 0x70) >> 4);
    pStatus->status.isdbt.layerAStatus.timeInterleaving = (BODS_IsdbtTimeInterleaving)((hImplChnDev->readBuf[32] & 0xC) >> 2);
    pStatus->status.isdbt.layerAStatus.numSegments = hImplChnDev->readBuf[33] & 0xF;
    pStatus->status.isdbt.layerAStatus.rsTotalBlocks = (hImplChnDev->readBuf[52] << 24) | (hImplChnDev->readBuf[53] << 16) | (hImplChnDev->readBuf[54] << 8) | hImplChnDev->readBuf[55];
    pStatus->status.isdbt.layerAStatus.rsCleanBlocks = (hImplChnDev->readBuf[56] << 24) | (hImplChnDev->readBuf[57] << 16) | (hImplChnDev->readBuf[58] << 8) | hImplChnDev->readBuf[59];    
    pStatus->status.isdbt.layerAStatus.rsCorrectedBlocks = (hImplChnDev->readBuf[60] << 24) | (hImplChnDev->readBuf[61] << 16) | (hImplChnDev->readBuf[62] << 8) | hImplChnDev->readBuf[63];
    pStatus->status.isdbt.layerAStatus.rsUncorrectedBlocks = (hImplChnDev->readBuf[64] << 24) | (hImplChnDev->readBuf[65] << 16) | (hImplChnDev->readBuf[66] << 8) | hImplChnDev->readBuf[67];    
    pStatus->status.isdbt.layerAStatus.signalLevelPercent = hImplChnDev->readBuf[36];
    pStatus->status.isdbt.layerAStatus.signalQualityPercent = hImplChnDev->readBuf[37];
    pStatus->status.isdbt.layerBStatus.modulation = (BODS_IsdbtModulation)(hImplChnDev->readBuf[68] & 0x3);
    pStatus->status.isdbt.layerBStatus.codeRate = (BODS_IsdbtCodeRate)((hImplChnDev->readBuf[68] & 0x70) >> 4);
    pStatus->status.isdbt.layerBStatus.timeInterleaving = (BODS_IsdbtTimeInterleaving)((hImplChnDev->readBuf[68] & 0xC) >> 2);
    pStatus->status.isdbt.layerBStatus.numSegments = hImplChnDev->readBuf[69] & 0xF;
    pStatus->status.isdbt.layerBStatus.rsTotalBlocks = (hImplChnDev->readBuf[88] << 24) | (hImplChnDev->readBuf[89] << 16) | (hImplChnDev->readBuf[90] << 8) | hImplChnDev->readBuf[91];    
    pStatus->status.isdbt.layerBStatus.rsCleanBlocks = (hImplChnDev->readBuf[92] << 24) | (hImplChnDev->readBuf[93] << 16) | (hImplChnDev->readBuf[94] << 8) | hImplChnDev->readBuf[95];    
    pStatus->status.isdbt.layerBStatus.rsCorrectedBlocks = (hImplChnDev->readBuf[96] << 24) | (hImplChnDev->readBuf[97] << 16) | (hImplChnDev->readBuf[98] << 8) | hImplChnDev->readBuf[99];
    pStatus->status.isdbt.layerBStatus.rsUncorrectedBlocks = (hImplChnDev->readBuf[100] << 24) | (hImplChnDev->readBuf[101] << 16) | (hImplChnDev->readBuf[102] << 8) | hImplChnDev->readBuf[103];   
    pStatus->status.isdbt.layerBStatus.signalLevelPercent = hImplChnDev->readBuf[72];
    pStatus->status.isdbt.layerBStatus.signalQualityPercent = hImplChnDev->readBuf[73];    
    pStatus->status.isdbt.layerCStatus.modulation = (BODS_IsdbtModulation)(hImplChnDev->readBuf[104] & 0x3);
    pStatus->status.isdbt.layerCStatus.codeRate = (BODS_IsdbtCodeRate)((hImplChnDev->readBuf[104] & 0x70) >> 4);
    pStatus->status.isdbt.layerCStatus.timeInterleaving = (BODS_IsdbtTimeInterleaving)((hImplChnDev->readBuf[104] & 0xC) >> 2);
    pStatus->status.isdbt.layerCStatus.numSegments = hImplChnDev->readBuf[105] & 0xF;
    pStatus->status.isdbt.layerCStatus.rsTotalBlocks = (hImplChnDev->readBuf[124] << 24) | (hImplChnDev->readBuf[125] << 16) | (hImplChnDev->readBuf[126] << 8) | hImplChnDev->readBuf[127];
    pStatus->status.isdbt.layerCStatus.rsCleanBlocks = (hImplChnDev->readBuf[128] << 24) | (hImplChnDev->readBuf[129] << 16) | (hImplChnDev->readBuf[130] << 8) | hImplChnDev->readBuf[131];    
    pStatus->status.isdbt.layerCStatus.rsCorrectedBlocks = (hImplChnDev->readBuf[132] << 24) | (hImplChnDev->readBuf[133] << 16) | (hImplChnDev->readBuf[134] << 8) | hImplChnDev->readBuf[135];
    pStatus->status.isdbt.layerCStatus.rsUncorrectedBlocks = (hImplChnDev->readBuf[136] << 24) | (hImplChnDev->readBuf[137] << 16) | (hImplChnDev->readBuf[138] << 8) | hImplChnDev->readBuf[139];   
    pStatus->status.isdbt.layerCStatus.signalLevelPercent = hImplChnDev->readBuf[108];
    pStatus->status.isdbt.layerCStatus.signalQualityPercent = hImplChnDev->readBuf[109];
    
done:
    BDBG_LEAVE(BODS_Leap_P_GetIsdbtSelectiveAsyncStatus);
    return retCode;
}
    
BERR_Code BODS_Leap_P_GetDvbt2SelectiveAsyncStatus(
    BODS_ChannelHandle hChn,
    BODS_SelectiveAsyncStatusType type,
    BODS_SelectiveStatus *pStatus   /* [out] */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;
    uint8_t buf[13] = HAB_MSG_HDR(BODS_eGetSelectiveAsyncStatus, 0x8, BODS_DVBT2_CORE_TYPE, BODS_CORE_ID);
    uint8_t mp, i;
    uint16_t plpCount = 0, read_len = 0;
    int16_t signalStrength;

    BDBG_ENTER(BODS_Leap_P_GetDvbt2SelectiveAsyncStatus);
    BDBG_ASSERT( hChn );

    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );
    
    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("ODS core Powered Off"));    
        retCode = BODS_ERR_POWER_DOWN;
    }
    else
    {
        BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));

        switch(type)
        {
            case BODS_SelectiveAsyncStatusType_eDvbt2FecStatisticsL1Pre:
                buf[9] = 6;
                read_len = 57;               
                break;
            case BODS_SelectiveAsyncStatusType_eDvbt2FecStatisticsL1Post:
                buf[9] = 7;
                read_len = 57;                
                break;
            case BODS_SelectiveAsyncStatusType_eDvbt2FecStatisticsPlpA:
                buf[9] = 8;
                read_len = 57;                
                break;
            case BODS_SelectiveAsyncStatusType_eDvbt2FecStatisticsPlpB:
                buf[9] = 9;
                read_len = 57;                
                break;
            case BODS_SelectiveAsyncStatusType_eDvbt2L1Pre:
                buf[9] = 1;
                read_len = 41;                
                break;    
            case BODS_SelectiveAsyncStatusType_eDvbt2L1PostConfigurable:
                buf[9] = 2;
                read_len = 61;                
                break;        
            case BODS_SelectiveAsyncStatusType_eDvbt2L1PostDynamic:
                buf[9] = 3;
                read_len = 53;                
                break; 
            case BODS_SelectiveAsyncStatusType_eDvbt2L1Plp:
                buf[9] = 5;
                read_len = BODS_L1PLP_BUF_MAX;
                break; 
            case BODS_SelectiveAsyncStatusType_eDvbt2Short:
                buf[9] = 10;
                read_len = 37;                
                break;                 
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                break;
        }

        buf[3] = hImplChnDev->chnNo;

        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 13, hImplChnDev->readBuf, read_len, false, true, read_len));
      
        switch(hImplChnDev->readBuf[9])
        {
            case 1:
                pStatus->type = BODS_SelectiveAsyncStatusType_eDvbt2L1Pre;
                pStatus->status.dvbt2L1Pre.streamType = hImplChnDev->readBuf[12];
                pStatus->status.dvbt2L1Pre.bwtExt= hImplChnDev->readBuf[13] & 0x1;
                pStatus->status.dvbt2L1Pre.s1= (hImplChnDev->readBuf[13] & 0xE) >> 1;
                pStatus->status.dvbt2L1Pre.s2= (hImplChnDev->readBuf[13] & 0xF0) >> 4;
                pStatus->status.dvbt2L1Pre.l1RepetitionFlag = hImplChnDev->readBuf[14] & 0x1;
                pStatus->status.dvbt2L1Pre.guardInterval = (hImplChnDev->readBuf[14] & 0xE) >> 1;
                pStatus->status.dvbt2L1Pre.papr = (hImplChnDev->readBuf[14] & 0xF0) >> 4;
                pStatus->status.dvbt2L1Pre.l1Modulation = hImplChnDev->readBuf[15] & 0xF;
                pStatus->status.dvbt2L1Pre.l1CodeRate = (hImplChnDev->readBuf[15] & 0x30) >> 4;
                pStatus->status.dvbt2L1Pre.l1FecType = (hImplChnDev->readBuf[15] & 0xC0) >> 6;
                pStatus->status.dvbt2L1Pre.pilotPattern = hImplChnDev->readBuf[16] & 0xF;
                pStatus->status.dvbt2L1Pre.regenFlag = (hImplChnDev->readBuf[16] & 0x70) >> 4;
                pStatus->status.dvbt2L1Pre.l1PostExt = (hImplChnDev->readBuf[16] & 0x80) >> 6;
                pStatus->status.dvbt2L1Pre.numRf= hImplChnDev->readBuf[17] & 0x7;
                pStatus->status.dvbt2L1Pre.currentRfIndex = (hImplChnDev->readBuf[17] & 0x70) >> 4;
                pStatus->status.dvbt2L1Pre.txIdAvailability = hImplChnDev->readBuf[18];
                pStatus->status.dvbt2L1Pre.numT2Frames = hImplChnDev->readBuf[19];
                pStatus->status.dvbt2L1Pre.numDataSymbols = (hImplChnDev->readBuf[20] << 8) | hImplChnDev->readBuf[21];
                pStatus->status.dvbt2L1Pre.cellId = (hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23];
                pStatus->status.dvbt2L1Pre.networkId = (hImplChnDev->readBuf[24] << 8) | hImplChnDev->readBuf[25];
                pStatus->status.dvbt2L1Pre.t2SystemId = (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27];
                pStatus->status.dvbt2L1Pre.l1PostSize = (hImplChnDev->readBuf[32] << 24) | (hImplChnDev->readBuf[33] << 16) | (hImplChnDev->readBuf[34] << 8) | hImplChnDev->readBuf[35];
                pStatus->status.dvbt2L1Pre.l1PostInfoSize = (hImplChnDev->readBuf[36] << 24) | (hImplChnDev->readBuf[37] << 16) | (hImplChnDev->readBuf[38] << 8) | hImplChnDev->readBuf[39];            
                break;                
            case 2:
                pStatus->type = BODS_SelectiveAsyncStatusType_eDvbt2L1PostConfigurable;
                pStatus->status.dvbt2L1PostConfigurable.numPlp = hImplChnDev->readBuf[12];
                pStatus->status.dvbt2L1PostConfigurable.numAux = hImplChnDev->readBuf[13] & 0xF;
                pStatus->status.dvbt2L1PostConfigurable.auxConfigRFu = hImplChnDev->readBuf[14];
                pStatus->status.dvbt2L1PostConfigurable.rfIdx = hImplChnDev->readBuf[15] & 0x7;
                pStatus->status.dvbt2L1PostConfigurable.fefType = (hImplChnDev->readBuf[15] & 0xF0) >> 4;
                pStatus->status.dvbt2L1PostConfigurable.fefInterval = hImplChnDev->readBuf[16];
                pStatus->status.dvbt2L1PostConfigurable.subSlicesPerFrame = (hImplChnDev->readBuf[18] << 8) | hImplChnDev->readBuf[19];
                pStatus->status.dvbt2L1PostConfigurable.frequency= (hImplChnDev->readBuf[20] << 24) | (hImplChnDev->readBuf[21] << 16) | (hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23];
                pStatus->status.dvbt2L1PostConfigurable.fefLength = (hImplChnDev->readBuf[24] << 24) | (hImplChnDev->readBuf[25] << 16) | (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27];
                pStatus->status.dvbt2L1PostConfigurable.plpA.plpId = hImplChnDev->readBuf[28];
                pStatus->status.dvbt2L1PostConfigurable.plpA.plpGroupId = hImplChnDev->readBuf[29];
                pStatus->status.dvbt2L1PostConfigurable.plpA.plpType = hImplChnDev->readBuf[30] & 0x7;
                pStatus->status.dvbt2L1PostConfigurable.plpA.plpPayloadType = (hImplChnDev->readBuf[30] & 0xF8) >> 3;
                pStatus->status.dvbt2L1PostConfigurable.plpA.ffFlag= hImplChnDev->readBuf[31] & 0x1;
                pStatus->status.dvbt2L1PostConfigurable.plpA.firstRfIdx = (hImplChnDev->readBuf[31] & 0xE) >> 1;
                pStatus->status.dvbt2L1PostConfigurable.plpA.plpFecType = (hImplChnDev->readBuf[31] & 0x30) >> 4;
                pStatus->status.dvbt2L1PostConfigurable.plpA.timeIlType = (hImplChnDev->readBuf[31] & 0x40) >> 6;
                pStatus->status.dvbt2L1PostConfigurable.plpA.inBandFlag = (hImplChnDev->readBuf[31] & 0x80) >> 7;
                pStatus->status.dvbt2L1PostConfigurable.plpA.codeRate = hImplChnDev->readBuf[32] & 0x7;
                pStatus->status.dvbt2L1PostConfigurable.plpA.modulation = (hImplChnDev->readBuf[32] & 0x38) >> 3;
                pStatus->status.dvbt2L1PostConfigurable.plpA.plpRotation = (hImplChnDev->readBuf[32] & 0x40) >> 6;
                pStatus->status.dvbt2L1PostConfigurable.plpA.firstFrameIdx = hImplChnDev->readBuf[33];
                pStatus->status.dvbt2L1PostConfigurable.plpA.frameInterval = hImplChnDev->readBuf[34];
                pStatus->status.dvbt2L1PostConfigurable.plpA.timeIlLength = hImplChnDev->readBuf[35];
                pStatus->status.dvbt2L1PostConfigurable.plpA.plpNumBlocksMax = (hImplChnDev->readBuf[36] << 8) | hImplChnDev->readBuf[37];
                pStatus->status.dvbt2L1PostConfigurable.plpB.plpId = hImplChnDev->readBuf[40];
                pStatus->status.dvbt2L1PostConfigurable.plpB.plpGroupId = hImplChnDev->readBuf[41];
                pStatus->status.dvbt2L1PostConfigurable.plpB.plpType = hImplChnDev->readBuf[42] & 0x7;
                pStatus->status.dvbt2L1PostConfigurable.plpB.plpPayloadType = (hImplChnDev->readBuf[42] & 0xF8) >> 3;
                pStatus->status.dvbt2L1PostConfigurable.plpB.ffFlag= hImplChnDev->readBuf[43] & 0x1;
                pStatus->status.dvbt2L1PostConfigurable.plpB.firstRfIdx = (hImplChnDev->readBuf[43] & 0xE) >> 1;
                pStatus->status.dvbt2L1PostConfigurable.plpB.plpFecType = (hImplChnDev->readBuf[43] & 0x30) >> 4;
                pStatus->status.dvbt2L1PostConfigurable.plpB.timeIlType = (hImplChnDev->readBuf[43] & 0x40) >> 6;
                pStatus->status.dvbt2L1PostConfigurable.plpB.inBandFlag = (hImplChnDev->readBuf[43] & 0x80) >> 7;
                pStatus->status.dvbt2L1PostConfigurable.plpB.codeRate = hImplChnDev->readBuf[44] & 0x7;
                pStatus->status.dvbt2L1PostConfigurable.plpB.modulation = (hImplChnDev->readBuf[44] & 0x38) >> 3;
                pStatus->status.dvbt2L1PostConfigurable.plpB.plpRotation = (hImplChnDev->readBuf[44] & 0x40) >> 6;
                pStatus->status.dvbt2L1PostConfigurable.plpB.firstFrameIdx = hImplChnDev->readBuf[45];
                pStatus->status.dvbt2L1PostConfigurable.plpB.frameInterval = hImplChnDev->readBuf[46];
                pStatus->status.dvbt2L1PostConfigurable.plpB.timeIlLength = hImplChnDev->readBuf[47];
                pStatus->status.dvbt2L1PostConfigurable.plpB.plpNumBlocksMax = (hImplChnDev->readBuf[48] << 8) | hImplChnDev->readBuf[49];
                pStatus->status.dvbt2L1PostConfigurable.auxStreamType = (hImplChnDev->readBuf[56] & 0xF0) >> 4;
                pStatus->status.dvbt2L1PostConfigurable.auxPrivateConf = ((hImplChnDev->readBuf[56] & 0xF) << 24) | (hImplChnDev->readBuf[57] << 16) | (hImplChnDev->readBuf[58] << 8) | hImplChnDev->readBuf[59];
                break;                
            case 3:
                pStatus->type = BODS_SelectiveAsyncStatusType_eDvbt2L1PostDynamic;
                pStatus->status.dvbt2L1PostDynamic.frameIdx = hImplChnDev->readBuf[12];
                pStatus->status.dvbt2L1PostDynamic.l1ChanlgeCounter = hImplChnDev->readBuf[13];
                pStatus->status.dvbt2L1PostDynamic.startRfIdx = hImplChnDev->readBuf[14] & 0x7;
                pStatus->status.dvbt2L1PostDynamic.subSliceInterval = (hImplChnDev->readBuf[20]<< 24) | (hImplChnDev->readBuf[21] << 16) | (hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23];
                pStatus->status.dvbt2L1PostDynamic.type2Start = (hImplChnDev->readBuf[24]<< 24) | (hImplChnDev->readBuf[25] << 16) | (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27];
                pStatus->status.dvbt2L1PostDynamic.plpA.plpId = hImplChnDev->readBuf[28];
                pStatus->status.dvbt2L1PostDynamic.plpA.plpNumBlocks = (hImplChnDev->readBuf[30] << 8) | hImplChnDev->readBuf[31];
                pStatus->status.dvbt2L1PostDynamic.plpA.plpStart = (hImplChnDev->readBuf[32]<< 24) | (hImplChnDev->readBuf[33] << 16) | (hImplChnDev->readBuf[34] << 8) | hImplChnDev->readBuf[35];
                pStatus->status.dvbt2L1PostDynamic.plpB.plpId = hImplChnDev->readBuf[36];
                pStatus->status.dvbt2L1PostDynamic.plpB.plpNumBlocks = (hImplChnDev->readBuf[38] << 8) | hImplChnDev->readBuf[39];
                pStatus->status.dvbt2L1PostDynamic.plpB.plpStart = (hImplChnDev->readBuf[40]<< 24) | (hImplChnDev->readBuf[41] << 16) | (hImplChnDev->readBuf[42] << 8) | hImplChnDev->readBuf[43];
                pStatus->status.dvbt2L1PostDynamic.auxPrivateDyn_47_32 = (hImplChnDev->readBuf[44] << 8) | hImplChnDev->readBuf[45];
                pStatus->status.dvbt2L1PostDynamic.auxPrivateDyn_31_0 = (hImplChnDev->readBuf[46]<< 24) | (hImplChnDev->readBuf[47] << 16) | (hImplChnDev->readBuf[48] << 8) | hImplChnDev->readBuf[49];   
                break;
            case 5:
                pStatus->type = BODS_SelectiveAsyncStatusType_eDvbt2L1Plp;                
                mp = hImplChnDev->readBuf[4] & 0x80;      
                pStatus->status.dvbt2L1Plp.numPlp = hImplChnDev->readBuf[12];
                
                if(pStatus->status.dvbt2L1Plp.numPlp)
                {
                    for(i = 0; i < BODS_SEL_L1PLP_PKT_0_PLP_COUNT; i++)        
                    {
                        pStatus->status.dvbt2L1Plp.plp[plpCount].plpId=  hImplChnDev->readBuf[16+i*4];
                        pStatus->status.dvbt2L1Plp.plp[plpCount].plpGroupId =  hImplChnDev->readBuf[17+i*4];
                        pStatus->status.dvbt2L1Plp.plp[plpCount].plpPayloadType =  (hImplChnDev->readBuf[18+i*4] & 0xF8) >> 3;		
                        pStatus->status.dvbt2L1Plp.plp[plpCount].plpType =  hImplChnDev->readBuf[18+i*4] & 0x7;	  
                        plpCount++;
                        if(plpCount == pStatus->status.dvbt2L1Plp.numPlp)
                            break;
                    }

                    while(mp)
                    {      
                        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 9, hImplChnDev->readBuf, BODS_L1PLP_BUF_MAX, false, true, BODS_L1PLP_BUF_MAX));
                        mp = hImplChnDev->readBuf[4] & 0x80;
                        for(i = 0; i < BODS_SEL_L1PLP_PKT_1_N_PLP_COUNT; i++)
                        {
                            pStatus->status.dvbt2L1Plp.plp[plpCount].plpId =  hImplChnDev->readBuf[12+i*4];
                            pStatus->status.dvbt2L1Plp.plp[plpCount].plpGroupId =	hImplChnDev->readBuf[13+i*4];
                            pStatus->status.dvbt2L1Plp.plp[plpCount].plpPayloadType =	(hImplChnDev->readBuf[14+i*4] & 0xF8) >> 3; 	
                            pStatus->status.dvbt2L1Plp.plp[plpCount].plpType =  hImplChnDev->readBuf[14+i*4] & 0x7;                       
                            plpCount++;                
                            if((plpCount == pStatus->status.dvbt2L1Plp.numPlp) || (plpCount > sizeof(pStatus->status.dvbt2L1Plp.plp)/sizeof(BODS_Dvbt2Plp)))
                            {
                                mp = 0;
                                break;	
                            }
                        }
                    }
                }
                break;            
            case 6:            
            case 7:           
            case 8:            
            case 9:
                pStatus->type = (BODS_SelectiveAsyncStatusType)(hImplChnDev->readBuf[9] - 4);
                pStatus->status.dvbt2FecStatistics.lock = hImplChnDev->readBuf[12] & 0x1;
                pStatus->status.dvbt2FecStatistics.snrData = ((hImplChnDev->readBuf[14] << 8) | hImplChnDev->readBuf[15]);
                pStatus->status.dvbt2FecStatistics.ldpcAvgIter = ((hImplChnDev->readBuf[18] << 8) | hImplChnDev->readBuf[19]);
                pStatus->status.dvbt2FecStatistics.ldpcTotIter = ((hImplChnDev->readBuf[20] << 24) | (hImplChnDev->readBuf[21] << 16) | (hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23]);
                pStatus->status.dvbt2FecStatistics.ldpcTotFrm = ((hImplChnDev->readBuf[24] << 24) | (hImplChnDev->readBuf[25] << 16) | (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27]);
                pStatus->status.dvbt2FecStatistics.ldpcUncFrm = ((hImplChnDev->readBuf[28] << 24) | (hImplChnDev->readBuf[29] << 16) | (hImplChnDev->readBuf[30] << 8) | hImplChnDev->readBuf[31]);
                pStatus->status.dvbt2FecStatistics.ldpcBER = ((hImplChnDev->readBuf[32] << 24) | (hImplChnDev->readBuf[33] << 16) | (hImplChnDev->readBuf[34] << 8) | hImplChnDev->readBuf[35]);
                pStatus->status.dvbt2FecStatistics.bchCorBit = ((hImplChnDev->readBuf[36] << 24) | (hImplChnDev->readBuf[37] << 16) | (hImplChnDev->readBuf[38] << 8) | hImplChnDev->readBuf[39]);
                pStatus->status.dvbt2FecStatistics.bchTotBlk = ((hImplChnDev->readBuf[40] << 24) | (hImplChnDev->readBuf[41] << 16) | (hImplChnDev->readBuf[42] << 8) | hImplChnDev->readBuf[43]);
                pStatus->status.dvbt2FecStatistics.bchClnBlk = ((hImplChnDev->readBuf[44] << 24) | (hImplChnDev->readBuf[45] << 16) | (hImplChnDev->readBuf[46] << 8) | hImplChnDev->readBuf[47]);
                pStatus->status.dvbt2FecStatistics.bchCorBlk = ((hImplChnDev->readBuf[48] << 24) | (hImplChnDev->readBuf[49] << 16) | (hImplChnDev->readBuf[50] << 8) | hImplChnDev->readBuf[51]);
                pStatus->status.dvbt2FecStatistics.bchUncBlk = ((hImplChnDev->readBuf[52] << 24) | (hImplChnDev->readBuf[53] << 16) | (hImplChnDev->readBuf[54] << 8) | hImplChnDev->readBuf[55]);               
                break;
            case 10:
                pStatus->type = BODS_SelectiveAsyncStatusType_eDvbt2Short;            
                pStatus->status.dvbt2Short.acquisitionMode = hImplChnDev->readBuf[12] & 0x1;
                pStatus->status.dvbt2Short.bandwidth = (hImplChnDev->readBuf[12] & 0x1c) >> 2;               
                pStatus->status.dvbt2Short.lock = hImplChnDev->readBuf[13] & 0x1;
                pStatus->status.dvbt2Short.spectrumInverted = (hImplChnDev->readBuf[13] & 0x8) >> 3;
                pStatus->status.dvbt2Short.reacqCount = ((hImplChnDev->readBuf[16] << 8) | hImplChnDev->readBuf[17]);
                pStatus->status.dvbt2Short.snrEstimate = ((hImplChnDev->readBuf[20] << 8) | hImplChnDev->readBuf[21]);
                pStatus->status.dvbt2Short.gainOffset = ((hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23]);
                pStatus->status.dvbt2Short.carrierFreqOffset = ((hImplChnDev->readBuf[24] << 24) | (hImplChnDev->readBuf[25] << 16) | (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27]);
                pStatus->status.dvbt2Short.timingOffset = ((hImplChnDev->readBuf[28] << 24) | (hImplChnDev->readBuf[29] << 16) | (hImplChnDev->readBuf[30] << 8) | hImplChnDev->readBuf[31]);   
                signalStrength = ((hImplChnDev->readBuf[32] << 8) | (hImplChnDev->readBuf[33] & 0xFF));
                pStatus->status.dvbt2Short.signalStrength = signalStrength*100/256 + 4875;                 
                pStatus->status.dvbt2Short.signalLevelPercent = hImplChnDev->readBuf[34];           
                pStatus->status.dvbt2Short.signalQualityPercent = hImplChnDev->readBuf[35];
                break;
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                break;        
        }        
    }

done:
    BDBG_LEAVE(BODS_Leap_P_GetDvbt2SelectiveAsyncStatus);
    return retCode;
}

BERR_Code BODS_Leap_P_GetDvbc2SelectiveAsyncStatus(
    BODS_ChannelHandle hChn,
    BODS_SelectiveAsyncStatusType type,
    BODS_SelectiveStatus *pStatus   /* [out] */
    )
{

    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;
    uint8_t buf[13] = HAB_MSG_HDR(BODS_eGetSelectiveAsyncStatus, 0x8, BODS_DVBC2_CORE_TYPE, BODS_CORE_ID);
    uint8_t mp, i;
    uint16_t plpCount = 0, read_len = 0;
    int16_t signalStrength;

    BDBG_ENTER(BODS_Leap_P_GetDvbt2SelectiveAsyncStatus);
    BDBG_ASSERT( hChn );

    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );
    
    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("ODS core Powered Off"));    
        retCode = BODS_ERR_POWER_DOWN;
    }
    else
    {
        BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));   

        switch(type)
        {
            case BODS_SelectiveAsyncStatusType_eDvbc2L1Part2:
                buf[9] = 1;
                read_len = 512;               
                break;
            case BODS_SelectiveAsyncStatusType_eDvbc2L1Dslice:
                buf[9] = 2;
                read_len = 512;                
                break;
            case BODS_SelectiveAsyncStatusType_eDvbc2L1Notch:
                buf[9] = 3;
                read_len = 512;                
                break;
            case BODS_SelectiveAsyncStatusType_eDvbc2L1Plp:
                buf[9] = 4;
                read_len = BODS_L1PLP_BUF_MAX;
                break;                 
            case BODS_SelectiveAsyncStatusType_eDvbc2FecStatisticsPlpA:
                buf[9] = 6;
                read_len = 57;                
                break;
            case BODS_SelectiveAsyncStatusType_eDvbc2FecStatisticsPlpB:
                buf[9] = 7;
                read_len = 57;                
                break;
            case BODS_SelectiveAsyncStatusType_eDvbc2FecStatisticsL1Part2:
                buf[9] = 5;
                read_len = 57;                
                break;
            case BODS_SelectiveAsyncStatusType_eDvbc2Short:
                buf[9] = 8;
                read_len = 37;                
                break;                 
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                break;
        }

        buf[3] = hImplChnDev->chnNo;        
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 13, hImplChnDev->readBuf, read_len, false, true, read_len));
     
        switch(hImplChnDev->readBuf[9])
        {
            case 1:
                pStatus->type = BODS_SelectiveAsyncStatusType_eDvbc2L1Part2;
                break;                
            case 2:
                pStatus->type = BODS_SelectiveAsyncStatusType_eDvbc2L1Dslice;  
                break;                
            case 3:
                pStatus->type = BODS_SelectiveAsyncStatusType_eDvbc2L1Notch;
                break;                
            case 4:
                pStatus->type = BODS_SelectiveAsyncStatusType_eDvbc2L1Plp;                
                mp = hImplChnDev->readBuf[4] & 0x80;      
                pStatus->status.dvbc2L1Plp.numPlp = hImplChnDev->readBuf[12];
                
                if(pStatus->status.dvbc2L1Plp.numPlp)
                {
                    for(i = 0; i < BODS_SEL_L1PLP_PKT_0_PLP_COUNT; i++)        
                    {
                        pStatus->status.dvbc2L1Plp.plp[plpCount].plpId=  hImplChnDev->readBuf[16+i*4];
                        pStatus->status.dvbc2L1Plp.plp[plpCount].plpGroupId =  hImplChnDev->readBuf[17+i*4];
                        pStatus->status.dvbc2L1Plp.plp[plpCount].plpPayloadType =  (hImplChnDev->readBuf[18+i*4] & 0xF8) >> 3;		
                        pStatus->status.dvbc2L1Plp.plp[plpCount].plpType =  hImplChnDev->readBuf[18+i*4] & 0x7;	  
                        plpCount++;
                        if(plpCount == pStatus->status.dvbc2L1Plp.numPlp)
                            break;
                    }

                    while(mp)
                    {      
                        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 9, hImplChnDev->readBuf, BODS_L1PLP_BUF_MAX, false, true, BODS_L1PLP_BUF_MAX));
                        mp = hImplChnDev->readBuf[4] & 0x80;
                        for(i = 0; i < BODS_SEL_L1PLP_PKT_1_N_PLP_COUNT; i++)
                        {
                            pStatus->status.dvbc2L1Plp.plp[plpCount].plpId =  hImplChnDev->readBuf[12+i*4];
                            pStatus->status.dvbc2L1Plp.plp[plpCount].plpGroupId =	hImplChnDev->readBuf[13+i*4];
                            pStatus->status.dvbc2L1Plp.plp[plpCount].plpPayloadType =	(hImplChnDev->readBuf[14+i*4] & 0xF8) >> 3; 	
                            pStatus->status.dvbc2L1Plp.plp[plpCount].plpType =  hImplChnDev->readBuf[14+i*4] & 0x7;                       
                            plpCount++;                
                            if((plpCount == pStatus->status.dvbc2L1Plp.numPlp) || (plpCount > sizeof(pStatus->status.dvbc2L1Plp.plp)/sizeof(BODS_Dvbt2Plp)))
                            {
                                mp = 0;
                                break;	
                            }
                        }
                    }
                }
                break;            
            case 5:
            case 6:
            case 7:
                pStatus->type = (BODS_SelectiveAsyncStatusType)(hImplChnDev->readBuf[9] + 6);
                pStatus->status.dvbc2FecStatistics.lock = hImplChnDev->readBuf[12] & 0x1;
                pStatus->status.dvbc2FecStatistics.snrData = ((hImplChnDev->readBuf[14] << 8) | hImplChnDev->readBuf[15]);
                pStatus->status.dvbc2FecStatistics.ldpcAvgIter = ((hImplChnDev->readBuf[18] << 8) | hImplChnDev->readBuf[19]);
                pStatus->status.dvbc2FecStatistics.ldpcTotIter = ((hImplChnDev->readBuf[20] << 24) | (hImplChnDev->readBuf[21] << 16) | (hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23]);
                pStatus->status.dvbc2FecStatistics.ldpcTotFrm = ((hImplChnDev->readBuf[24] << 24) | (hImplChnDev->readBuf[25] << 16) | (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27]);
                pStatus->status.dvbc2FecStatistics.ldpcUncFrm = ((hImplChnDev->readBuf[28] << 24) | (hImplChnDev->readBuf[29] << 16) | (hImplChnDev->readBuf[30] << 8) | hImplChnDev->readBuf[31]);
                pStatus->status.dvbc2FecStatistics.ldpcBER = ((hImplChnDev->readBuf[32] << 24) | (hImplChnDev->readBuf[33] << 16) | (hImplChnDev->readBuf[34] << 8) | hImplChnDev->readBuf[35]);
                pStatus->status.dvbc2FecStatistics.bchCorBit = ((hImplChnDev->readBuf[36] << 24) | (hImplChnDev->readBuf[37] << 16) | (hImplChnDev->readBuf[38] << 8) | hImplChnDev->readBuf[39]);
                pStatus->status.dvbc2FecStatistics.bchTotBlk = ((hImplChnDev->readBuf[40] << 24) | (hImplChnDev->readBuf[41] << 16) | (hImplChnDev->readBuf[42] << 8) | hImplChnDev->readBuf[43]);
                pStatus->status.dvbc2FecStatistics.bchClnBlk = ((hImplChnDev->readBuf[44] << 24) | (hImplChnDev->readBuf[45] << 16) | (hImplChnDev->readBuf[46] << 8) | hImplChnDev->readBuf[47]);
                pStatus->status.dvbc2FecStatistics.bchCorBlk = ((hImplChnDev->readBuf[48] << 24) | (hImplChnDev->readBuf[49] << 16) | (hImplChnDev->readBuf[50] << 8) | hImplChnDev->readBuf[51]);
                pStatus->status.dvbc2FecStatistics.bchUncBlk = ((hImplChnDev->readBuf[52] << 24) | (hImplChnDev->readBuf[53] << 16) | (hImplChnDev->readBuf[54] << 8) | hImplChnDev->readBuf[55]);               
                break;
            case 8:
                pStatus->type = BODS_SelectiveAsyncStatusType_eDvbc2Short;            
                pStatus->status.dvbc2Short.acquisitionMode = hImplChnDev->readBuf[12] & 0x1;
                pStatus->status.dvbc2Short.bandwidth = (hImplChnDev->readBuf[12] & 0x1c) >> 2;               
                pStatus->status.dvbc2Short.lock = hImplChnDev->readBuf[13] & 0x1;
                pStatus->status.dvbc2Short.spectrumInverted = (hImplChnDev->readBuf[13] & 0x8) >> 3;
                pStatus->status.dvbc2Short.reacqCount = ((hImplChnDev->readBuf[16] << 8) | hImplChnDev->readBuf[17]);
                pStatus->status.dvbc2Short.snrEstimate = ((hImplChnDev->readBuf[20] << 8) | hImplChnDev->readBuf[21]);
                pStatus->status.dvbc2Short.gainOffset = ((hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23]);
                pStatus->status.dvbc2Short.carrierFreqOffset = ((hImplChnDev->readBuf[24] << 24) | (hImplChnDev->readBuf[25] << 16) | (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27]);
                pStatus->status.dvbc2Short.timingOffset = ((hImplChnDev->readBuf[28] << 24) | (hImplChnDev->readBuf[29] << 16) | (hImplChnDev->readBuf[30] << 8) | hImplChnDev->readBuf[31]);   
                signalStrength = ((hImplChnDev->readBuf[32] << 8) | (hImplChnDev->readBuf[33] & 0xFF));
                pStatus->status.dvbc2Short.signalStrength = signalStrength*100/256 + 4875;                 
                pStatus->status.dvbc2Short.signalLevelPercent = hImplChnDev->readBuf[34];           
                pStatus->status.dvbc2Short.signalQualityPercent = hImplChnDev->readBuf[35];
                break;
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                break;        
        }        
    }

done:
    BDBG_LEAVE(BODS_Leap_P_GetDvbt2SelectiveAsyncStatus);
    return retCode;
}

/*******************************************************************************
*
*   Public Module Functions
*
*******************************************************************************/

BERR_Code BODS_Leap_Open(
    BODS_Handle *pOds,                  /* [out] Returns handle */
    BCHP_Handle hChip,                  /* [in] Chip handle */
    BREG_Handle hRegister,              /* [in] Register handle */
    BINT_Handle hInterrupt,             /* [in] Interrupt handle, Bcm3250 */
    const struct BODS_Settings *pDefSettings    /* [in] Default settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Handle hDev;
    unsigned int chnIdx;
    BODS_Leap_Handle hImplDev = NULL;

    BDBG_ENTER(BODS_Leap_Open);

    /* Alloc memory from the system heap */
    hDev = (BODS_Handle) BKNI_Malloc( sizeof( BODS_P_Handle ) );
    if( hDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BODS_Open: BKNI_malloc() failed"));
        goto done;
    }
    
    BKNI_Memset( hDev, 0x00, sizeof( BODS_P_Handle ) );
    hDev->settings = *pDefSettings;
    
    BDBG_OBJECT_SET(hDev, BODS);

    hImplDev = (BODS_Leap_Handle) BKNI_Malloc( sizeof( BODS_P_Leap_Handle ) );
    if( hImplDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BODS_Open: BKNI_malloc() failed, impl"));
        goto done;
    }

    BKNI_Memset( hImplDev, 0x00, sizeof( BODS_P_Leap_Handle ) );
    hImplDev->hChip = hChip;
    hImplDev->hRegister = hRegister;
    hImplDev->hInterrupt = hInterrupt;
    hImplDev->hHab = (BHAB_Handle) pDefSettings->hGeneric;    /* For this device, we need the HAB handle */
    hImplDev->devId = BHAB_DevId_eODS0; /* Here the device id is always defaulted to channel 0. */
    hImplDev->mxChnNo = MAX_ODS_CHANNELS;

    for( chnIdx = 0; chnIdx < MAX_ODS_CHANNELS; chnIdx++ )
    {
        hImplDev->hOdsChn[chnIdx] = NULL;
    }
    hDev->pImpl = hImplDev;
    *pOds = hDev;

done:
    if( retCode != BERR_SUCCESS )
    {
        if( hDev != NULL )
        {
            BKNI_Free( hDev );
        }
        if( hImplDev != NULL )
        {
            BKNI_Free( hImplDev );
        }       
        *pOds = NULL;
    }
    BDBG_LEAVE(BODS_Leap_Open);
    return retCode;
}

BERR_Code BODS_Leap_Close(
    BODS_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BODS_Leap_Close);
    BDBG_ASSERT( hDev );
    BDBG_OBJECT_ASSERT( hDev,BODS );

    BKNI_Free( (void *) hDev->pImpl );
    BKNI_Free( (void *) hDev );

    BDBG_LEAVE(BODS_Leap_Close);
    return retCode;
}
    
BERR_Code BODS_Leap_Init(
    BODS_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_Handle hImplDev;
    uint32_t familyId, chipId;
    uint16_t chipVer;
    BFEC_SystemVersionInfo  versionInfo;    
    
    BDBG_ENTER(BODS_Leap_Init);
    BDBG_ASSERT( hDev );
    BDBG_OBJECT_ASSERT( hDev,BODS );

    hImplDev = (BODS_Leap_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );
    BDBG_ASSERT( hImplDev->hHab );

    retCode = BHAB_GetApVersion(hImplDev->hHab, &familyId, &chipId, &chipVer, &hImplDev->verInfo.majFwVer, &hImplDev->verInfo.minFwVer);
   
    switch(chipVer >> 8)
    {
        case 0:
            hImplDev->verInfo.majVer = 0xA;           
            break;
        case 1:
            hImplDev->verInfo.majVer = 0xB;      
            break;
        case 2:
            hImplDev->verInfo.majVer = 0xC;        
            break;            
        default:
            retCode = BERR_INVALID_PARAMETER;
            goto done;            
    }
    
    hImplDev->verInfo.minVer = chipVer & 0xFF;   
    hImplDev->verInfo.chipId = chipId & 0xFFFF;

    if((hImplDev->verInfo.majFwVer >= 6) || ((hImplDev->verInfo.majFwVer >= 5) & ( hImplDev->verInfo.minFwVer>= 3)))
    {
        BHAB_GetVersionInfo(hImplDev->hHab, &versionInfo);
        BDBG_WRN(("LEAP FW version is %d.%d.%d.%d", versionInfo.firmware.majorVersion, versionInfo.firmware.minorVersion, versionInfo.firmware.buildType, versionInfo.firmware.buildId));
    }
    else
        BDBG_WRN(("LEAP FW version is %d.%d", hImplDev->verInfo.majFwVer, hImplDev->verInfo.minFwVer));        

    
    BDBG_LEAVE(BODS_Leap_Init);
    
done:    
    return retCode;
}

BERR_Code BODS_Leap_GetVersion(
    BODS_Handle hDev,                   /* [in] Device handle */
    BODS_Version *pVersion              /* [out] Returns version */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_Handle hImplDev;

    BDBG_ENTER(BODS_Leap_GetVersion);
    BDBG_ASSERT( hDev );
    BDBG_OBJECT_ASSERT( hDev,BODS );

    hImplDev = (BODS_Leap_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );

    *pVersion = hImplDev->verInfo;      /* use saved data */

    BDBG_LEAVE(BODS_Leap_GetVersion);
    return retCode;
}


BERR_Code BODS_Leap_GetVersionInfo(
    BODS_Handle hDev,              /* [in] Device handle */
    BFEC_VersionInfo *pVersionInfo /* [out] Returns version Info */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[29] = HAB_MSG_HDR(BODS_eGetVersionInfo, 0, BODS_DVBC2_CORE_TYPE, BODS_CORE_ID);   
    BODS_Leap_Handle hImplDev;

    BDBG_ENTER(BODS_Leap_GetVersionInfo);
    BDBG_ASSERT( hDev );

    hImplDev = (BODS_Leap_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );

    buf[3] = 0;
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplDev->hHab, buf, 5, buf, 29, false, true, 29 ));
    pVersionInfo->majorVersion = (buf[4] << 8) | buf[5];
    pVersionInfo->minorVersion = (buf[6] << 8) | buf[7];
    pVersionInfo->buildType = (buf[8] << 8) | buf[9];
    pVersionInfo->buildId = (buf[10] << 8) | buf[11];

    BDBG_LEAVE(BODS_Leap_GetVersionInfo);

done:    
    return retCode;
}


BERR_Code BODS_Leap_GetTotalChannels(
    BODS_Handle hDev,                   /* [in] Device handle */
    BODS_Standard standard,             /* [in] Standard */     
    unsigned int *totalChannels         /* [out] Returns total number downstream channels supported */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_Handle hImplDev;

    BDBG_ENTER(BODS_Leap_GetTotalChannels);
    BDBG_ASSERT( hDev );
    BDBG_OBJECT_ASSERT( hDev,BODS );

    hImplDev = (BODS_Leap_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );

    switch(standard) {
        case BODS_Standard_eDvbt:
            *totalChannels = MAX_DVBT_CHANNELS;
            break;
        case BODS_Standard_eIsdbt:
            *totalChannels = MAX_ISDBT_CHANNELS;
            break;            
        case BODS_Standard_eDvbt2:
            *totalChannels = MAX_DVBT2_CHANNELS;
            break;
        case BODS_Standard_eDvbc2:
            *totalChannels = MAX_DVBC2_CHANNELS;
            break;
        default:
            *totalChannels = MAX_DVBT_CHANNELS;
            break;
    }


    BDBG_LEAVE(BODS_Leap_GetTotalChannels);
    return retCode;
}

BERR_Code BODS_Leap_GetCapabilities(
    BODS_Handle hDev,                           /* [in] Device handle */
    BODS_FrontendCapabilities *pCapabilities    /* [out] Returns frontend capabilities, all the standards supported by the chip */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BODS_Leap_GetCapabilities);
    BDBG_ASSERT( hDev );
    BDBG_OBJECT_ASSERT( hDev,BODS );
    
#if ((BODS_CHIP == 3462) || (BODS_CHIP == 7563))
    pCapabilities->dvbt = true;
    pCapabilities->isdbt = false;
    pCapabilities->dvbt2 = true;
    pCapabilities->dvbc2 = true;
#elif (BODS_CHIP == 3472)
    pCapabilities->dvbt = false;
    pCapabilities->isdbt = true;
    pCapabilities->dvbt2 = false;
    pCapabilities->dvbc2 = false;
#endif

    BDBG_LEAVE(BODS_Leap_GetCapabilities);
    return retCode;
}

BERR_Code BODS_Leap_OpenChannel(
    BODS_Handle hDev,                   /* [in] Device handle */
    BODS_ChannelHandle *phChn,          /* [out] Returns channel handle */
    const struct BODS_ChannelSettings *pChnDefSettings /* [in] Channel default setting */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_Handle hImplDev;
    BODS_Leap_ChannelHandle hImplChnDev = NULL;
    BODS_ChannelHandle hChnDev;
    unsigned int event=0;
    uint8_t i;

    BDBG_ENTER(BODS_Leap_OpenChannel);
    BDBG_ASSERT( hDev );
    BDBG_OBJECT_ASSERT( hDev,BODS );

    hImplDev = (BODS_Leap_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );
    BDBG_ASSERT( hImplDev->hHab);
   
    hChnDev = NULL;
 
    switch(pChnDefSettings->standard)
    {
#if ((BODS_CHIP == 3462) || (BODS_CHIP == 7563))    
        case BODS_Standard_eDvbt:
            if( pChnDefSettings->channelNo >= MAX_DVBT_CHANNELS )
                retCode = BERR_TRACE(BODS_ERR_NOTAVAIL_CHN_NO);            
            break;          
        case BODS_Standard_eDvbt2:
            if( pChnDefSettings->channelNo >= MAX_DVBT2_CHANNELS )
                retCode = BERR_TRACE(BODS_ERR_NOTAVAIL_CHN_NO);            
            break;
        case BODS_Standard_eDvbc2:
            if( pChnDefSettings->channelNo >= MAX_DVBC2_CHANNELS )
                retCode = BERR_TRACE(BODS_ERR_NOTAVAIL_CHN_NO);          
            break;
#elif (BODS_CHIP == 3472)
        case BODS_Standard_eIsdbt:
            if( pChnDefSettings->channelNo >= MAX_ISDBT_CHANNELS )
                retCode = BERR_TRACE(BODS_ERR_NOTAVAIL_CHN_NO);          
            break;
#endif            
        default:
            retCode = BERR_TRACE(BODS_ERR_NOTAVAIL_CHN_NO);
            break;
    }

	if( retCode == BERR_SUCCESS )
    {  
        for(i=0; i<hImplDev->mxChnNo; i++)
        {          
            if( hImplDev->hOdsChn[i] == NULL )
            {           
                /* Alloc memory from the system heap */
                hChnDev = (BODS_ChannelHandle) BKNI_Malloc( sizeof( BODS_P_ChannelHandle ) );
                if( hChnDev == NULL )
                {
                    retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                    BDBG_ERR(("BODS_OpenChannel: BKNI_malloc() failed"));
                    goto done;
                }
                
                BKNI_Memset( hChnDev, 0x00, sizeof( BODS_P_ChannelHandle ) );
                hChnDev->hOds = hDev;

                BDBG_OBJECT_SET(hChnDev, BODS_CHANNEL);
        
                hImplChnDev = (BODS_Leap_ChannelHandle) BKNI_Malloc( sizeof( BODS_P_Leap_ChannelHandle ) );
                if( hImplChnDev == NULL )
                {
                    retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                    BDBG_ERR(("BODS_OpenChannel: BKNI_malloc() failed, impl"));
                    goto done;
                }

                BKNI_Memset( hImplChnDev, 0x00, sizeof( BODS_P_Leap_ChannelHandle ) );
                
                hImplChnDev->chnNo = pChnDefSettings->channelNo;
                hImplChnDev->standard = hImplChnDev->settings.standard;
                hImplChnDev->devId = (BHAB_DevId) BHAB_DevId_eODS0 + i;
                /* set the bandwidth to an invalid value so that the acquire parameters get sent */
                hImplChnDev->previousAcquireParams.acquireParams.dvbt.bandwidth = BODS_DvbtBandwidth_eLast;

                BHAB_InstallInterruptCallback( hImplDev->hHab,  hImplChnDev->devId, BODS_Leap_P_EventCallback_isr , (void *)hChnDev, event);
                
                if (pChnDefSettings) hImplChnDev->settings = *pChnDefSettings;
                hImplChnDev->hHab = hImplDev->hHab;
                hImplChnDev->verInfo = hImplDev->verInfo;
                CHK_RETCODE(retCode, BKNI_CreateMutex(&hImplChnDev->mutex));
                hImplDev->hOdsChn[i] = hChnDev;
                hImplChnDev->bPowerdown = true;          
                hChnDev->pImpl = hImplChnDev;

                *phChn = hChnDev;
                break;
            }       
        }        
    } 
done:
    if( retCode != BERR_SUCCESS )
    {
        if( hChnDev != NULL )
        {
            BKNI_Free( hChnDev );
            hImplDev->hOdsChn[pChnDefSettings->channelNo] = NULL;
        }
        if( hImplChnDev != NULL )
        {
            BKNI_Free( hImplChnDev );
        }
        *phChn = NULL;
    }
    BDBG_LEAVE(BODS_Leap_OpenChannel);
    return retCode;
}

BERR_Code BODS_Leap_CloseChannel(
    BODS_ChannelHandle hChn             /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_Handle hImplDev;
    BODS_Leap_ChannelHandle hImplChnDev;
    BODS_Handle hOds;
    unsigned int chnNo;
    
    BDBG_ENTER(BODS_Leap_CloseChannel);
    BDBG_ASSERT( hChn );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );


    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    hOds = hChn->hOds;
    hImplDev = (BODS_Leap_Handle) hOds->pImpl;
    BDBG_ASSERT( hImplDev );
    
    BHAB_UnInstallInterruptCallback(hImplChnDev->hHab, hImplChnDev->devId);

    BKNI_DestroyMutex(hImplChnDev->mutex);
    chnNo = 0;
    BKNI_Free( hChn->pImpl );
    BKNI_Free( hChn );
    hImplDev->hOdsChn[chnNo] = NULL;

    BDBG_LEAVE(BODS_Leap_CloseChannel);
    return retCode;
}

BERR_Code BODS_Leap_GetDevice(
    BODS_ChannelHandle hChn,            /* [in] Device channel handle */
    BODS_Handle *pOds                  /* [out] Returns Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BODS_Leap_GetDevice);
    BDBG_ASSERT( hChn );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );


    *pOds = hChn->hOds;

    BDBG_LEAVE(BODS_Leap_GetDevice);
    return retCode;
}

BERR_Code BODS_Leap_GetChannelDefaultSettings(
    BODS_Handle hDev,                       /* [in] Device handle */
    BODS_Standard standard,                 /* [in] Channel number to default setting for */
    BODS_ChannelSettings *pChnDefSettings   /* [out] Returns channel default setting */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_Handle hImplDev;

    BDBG_ENTER(BODS_Leap_GetChannelDefaultSettings);
    BDBG_ASSERT( hDev );
    BDBG_OBJECT_ASSERT( hDev,BODS );

    hImplDev = (BODS_Leap_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );
    BDBG_ASSERT( hImplDev->hHab );

    if(standard < BODS_Standard_eLast) {
        pChnDefSettings->channelNo = 0;
        pChnDefSettings->standard = standard;
    }
    else
        BDBG_ERR(("Unsupported Standard"));
    
    BDBG_LEAVE(BODS_Leap_GetChannelDefaultSettings);
    return retCode;
}

BERR_Code BODS_Leap_GetLockStatus(
    BODS_ChannelHandle hChn,            /* [in] Device channel handle */
    BODS_LockStatus *pLockStatus         /* [out] Returns lock status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;    
    BODS_Leap_ChannelHandle hImplChnDev;   
    uint32_t sb;   
    uint8_t status = 0;   

    BDBG_ENTER(BODS_Leap_GetLockStatus);   
    BDBG_ASSERT( hChn );    
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );

	
    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;   
    BDBG_ASSERT( hImplChnDev );    
    BDBG_ASSERT( hImplChnDev->hHab );
    
    CHK_RETCODE(retCode, BHAB_ReadRegister(hImplChnDev->hHab, BCHP_LEAP_CTRL_SW_SPARE1, &sb));

    switch(hImplChnDev->settings.standard)
    {
        case BODS_Standard_eDvbt:
            status = sb & 0xF;
            break;        
        case BODS_Standard_eIsdbt:
            if(hImplChnDev->chnNo)
                status = (sb & 0xF000F) >> 16;
            else
                status = (sb & 0xF);
            break;
        case BODS_Standard_eDvbt2:
            status = (sb & 0xF0) >> 4;
            break;
        case BODS_Standard_eDvbc2:
            status = (sb & 0xF00) >> 8;
            break;
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }
    
    switch(status)
    {
        case 1:
            *pLockStatus = BODS_LockStatus_eLocked;
            break;
        case 2:
            *pLockStatus = BODS_LockStatus_eUnlocked;
            break;
        case 3:
            *pLockStatus = BODS_LockStatus_eNoSignal;
            break;
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }

done:    
    BDBG_LEAVE(BODS_Leap_GetLockStatus);        
    return retCode;
}

BERR_Code BODS_Leap_GetSoftDecision(
    BODS_ChannelHandle hChn,            /* [in] Device channel handle */
    int16_t nbrToGet,                   /* [in] Number values to get */
    int16_t *iVal,                      /* [out] Ptr to array to store output I soft decision */
    int16_t *qVal,                      /* [out] Ptr to array to store output Q soft decision */
    int16_t *nbrGotten                  /* [out] Number of values gotten/read */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;
    uint8_t i, write_len = 0;
    uint8_t buf[125] = HAB_MSG_HDR(BODS_eGetConstellation, 0x4, BODS_DVBC2_CORE_TYPE, BODS_CORE_ID);    

    BDBG_ENTER(BODS_Leap_GetSoftDecision);
    BDBG_ASSERT( hChn );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );


    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("ODS core Powered Off"));    
        retCode = BODS_ERR_POWER_DOWN;
    }
    else
    {
        BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));
        switch(hImplChnDev->settings.standard)
        {
            case BODS_Standard_eDvbt:
                buf[2] = BODS_DVBT_CORE_TYPE;
                write_len = 5;
                break;
            case BODS_Standard_eIsdbt:
                buf[2] = BODS_ISDBT_CORE_TYPE;
                write_len = 5;
                break;                
            case BODS_Standard_eDvbt2:
                buf[2] = (buf[2] & 0xF0) | BODS_DVBT2_CORE_TYPE;            
                buf[4] = 0x13;
                write_len = 9;				
                break;
            case BODS_Standard_eDvbc2:
                buf[4] = 0x13;
                write_len = 9;				
                break;
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);                
                break;
        }
        buf[3] = hImplChnDev->chnNo;        
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, write_len, hImplChnDev->readBuf, 125, false, true, 125));
        
        for (i = 0; i < 30; i++)
        {
            iVal[i] = (hImplChnDev->readBuf[4+(4*i)] << 8) | hImplChnDev->readBuf[5+(4*i)];
            qVal[i] = (hImplChnDev->readBuf[6+(4*i)] << 8) | hImplChnDev->readBuf[7+(4*i)];
        }

        *nbrGotten = nbrToGet;
    }

done:
    BDBG_LEAVE(BODS_Leap_GetSoftDecision);
    return retCode;
}

BERR_Code BODS_Leap_InstallCallback(
    BODS_ChannelHandle hChn,            /* [in] Device channel handle */
    BODS_Callback callbackType,         /* [in] type of Callback */
    BODS_CallbackFunc pCallbackFunc,    /* [in] Pointer to completion callback. */
    void *pParam                        /* [in] Pointer to callback user data. */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;

    BDBG_ENTER(BODS_Leap_InstallCallback);
    BDBG_ASSERT( hChn );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );


    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );

    switch( callbackType )
    {
        case BODS_Callback_eLockChange:
            hImplChnDev->pCallback[callbackType] = pCallbackFunc;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;
        case BODS_Callback_eUpdateGain:
            hImplChnDev->pCallback[callbackType] = pCallbackFunc;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;
        case BODS_Callback_eNoSignal:
            hImplChnDev->pCallback[callbackType] = pCallbackFunc;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;
        case BODS_Callback_eAsyncStatusReady:      
            hImplChnDev->pCallback[callbackType] = pCallbackFunc;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;
        case BODS_Callback_eEmergencyWarningSystem:
            hImplChnDev->pCallback[callbackType] = pCallbackFunc;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;            
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }

    BDBG_LEAVE(BODS_Leap_InstallCallback);
    return retCode;
}

BERR_Code BODS_Leap_GetDefaultAcquireParams(
    BODS_ChannelHandle hChn ,           /* [in] Device channel handle */
    BODS_AcquireParams *acquireParams   /* [out] default Acquire Parameters */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;
    
    BDBG_ENTER(BODS_Leap_GetDefaultAcquireParams);
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );

    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    
    *acquireParams = defAcquireParams;

    switch(hImplChnDev->settings.standard)
    {
        case BODS_Standard_eDvbt:               
            break;
        case BODS_Standard_eIsdbt:
            acquireParams->acquireParams.isdbt.bandwidth = BODS_IsdbtBandwidth_e6MHz;        
            acquireParams->acquireParams.isdbt.cciMode = BODS_IsdbtCciMode_eAuto;        
            acquireParams->acquireParams.isdbt.carrierRange = BODS_IsdbtCarrierRange_eWide;        
            acquireParams->acquireParams.isdbt.transGuardMode = BODS_IsdbtOfdmMode_eAutoBrazil;        
            acquireParams->acquireParams.isdbt.transmissionMode = BODS_IsdbtTransmissionMode_e8K;        
            acquireParams->acquireParams.isdbt.guardInterval = BODS_IsdbtGuardInterval_e1_8;        
            acquireParams->acquireParams.isdbt.tmccAcquire = true;        
            acquireParams->acquireParams.isdbt.partialReception = false;        
            acquireParams->acquireParams.isdbt.layerAParams.modulation = BODS_IsdbtModulation_e64Qam;
            acquireParams->acquireParams.isdbt.layerAParams.codeRate = BODS_IsdbtCodeRate_e3_4;
            acquireParams->acquireParams.isdbt.layerAParams.timeInterleaving = BODS_IsdbtTimeInterleaving_e2x;
            acquireParams->acquireParams.isdbt.layerAParams.numSegments = 13;
            acquireParams->acquireParams.isdbt.layerBParams.modulation = BODS_IsdbtModulation_eQpsk;
            acquireParams->acquireParams.isdbt.layerBParams.codeRate = BODS_IsdbtCodeRate_e1_2;
            acquireParams->acquireParams.isdbt.layerBParams.timeInterleaving = BODS_IsdbtTimeInterleaving_e0x;
            acquireParams->acquireParams.isdbt.layerBParams.numSegments = 0;
            acquireParams->acquireParams.isdbt.layerCParams.modulation = BODS_IsdbtModulation_eQpsk;
            acquireParams->acquireParams.isdbt.layerCParams.codeRate = BODS_IsdbtCodeRate_e1_2;
            acquireParams->acquireParams.isdbt.layerCParams.timeInterleaving = BODS_IsdbtTimeInterleaving_e0x;
            acquireParams->acquireParams.isdbt.layerCParams.numSegments = 0;            
            break;            
        case BODS_Standard_eDvbt2:
            acquireParams->acquireParams.dvbt2.plpMode = true;
            acquireParams->acquireParams.dvbt2.plpId = 0;           
            break;
        case BODS_Standard_eDvbc2:
            acquireParams->acquireParams.dvbc2.plpMode = true;
            acquireParams->acquireParams.dvbc2.plpId = 0;           
            break;
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);                
            break;
    }    
    
    BDBG_LEAVE(BODS_Leap_GetDefaultAcquireParams);
    return( retCode );
}

BERR_Code BODS_Leap_SetAcquireParams(
    BODS_ChannelHandle hChn ,           /* [in] Device channel handle */
    const BODS_AcquireParams *acquireParams     /* [in] Acquire Parameters to use */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;
    uint8_t buf[29] = HAB_MSG_HDR(BODS_eAcquireParamsWrite, 0x4, BODS_DVBC2_CORE_TYPE, BODS_CORE_ID);
    uint8_t transGuardMode = 0;


    BDBG_ENTER(BODS_Leap_SetAcquireParams);
    BDBG_ASSERT( hChn );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );  

    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    BKNI_AcquireMutex(hImplChnDev->mutex);
    hImplChnDev->isLock = false;
    BKNI_ReleaseMutex(hImplChnDev->mutex);

    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("ODS core Powered Off"));
        retCode = BODS_ERR_POWER_DOWN;
    }
    else
    {
        BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));
       
        switch(hImplChnDev->settings.standard)
        {
            case BODS_Standard_eDvbt:
                if(!((hImplChnDev->previousAcquireParams.acquisitionMode == acquireParams->acquisitionMode) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt.bandwidth == acquireParams->acquireParams.dvbt.bandwidth) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt.carrierRange == acquireParams->acquireParams.dvbt.carrierRange) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt.cciMode == acquireParams->acquireParams.dvbt.cciMode) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt.transGuardMode == acquireParams->acquireParams.dvbt.transGuardMode) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt.guardInterval == acquireParams->acquireParams.dvbt.guardInterval) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt.transmissionMode == acquireParams->acquireParams.dvbt.transmissionMode) &&           
                (hImplChnDev->previousAcquireParams.tuneAcquire == acquireParams->tuneAcquire) &&
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt.tpsMode == acquireParams->acquireParams.dvbt.tpsMode) &&                
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt.hierarchy == acquireParams->acquireParams.dvbt.hierarchy) &&                
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt.modulation == acquireParams->acquireParams.dvbt.modulation) &&                
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt.decodeMode == acquireParams->acquireParams.dvbt.decodeMode) &&                
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt.codeRateLowPriority == acquireParams->acquireParams.dvbt.codeRateLowPriority) &&                
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt.codeRateHighPriority == acquireParams->acquireParams.dvbt.codeRateHighPriority))) 
                {                 
                    buf[2] = (0xC << 4) | BODS_DVBT_CORE_TYPE;       
                    /* set Acquire Parameters */
                    buf[4] = !acquireParams->acquisitionMode;
                    buf[4] |= (acquireParams->acquireParams.dvbt.bandwidth << 2);
                    buf[4] |= (acquireParams->acquireParams.dvbt.carrierRange << 4);
                    buf[4] |= !(acquireParams->acquireParams.dvbt.cciMode << 5);        
                    if(acquireParams->acquireParams.dvbt.transGuardMode == BODS_DvbtOfdmMode_eManual)
                        buf[5] = (acquireParams->acquireParams.dvbt.guardInterval << 4) | (acquireParams->acquireParams.dvbt.transmissionMode << 2) | !(acquireParams->acquireParams.dvbt.transGuardMode);
                    else
                        buf[5] = !(acquireParams->acquireParams.dvbt.transGuardMode);
                    
                    buf[6] = acquireParams->tuneAcquire << 7;
                    buf[6] |= 0x1; /* TS out Serial */

                    /* If TPS acquire is set, than user will need to pass in coderate HP, coderate LP, and hierarchy */
                    if (acquireParams->acquireParams.dvbt.tpsMode == BODS_DvbtTpsMode_eManual)
                    {
                        buf[12] = !(acquireParams->acquireParams.dvbt.tpsMode);
                        buf[12] |= (acquireParams->acquireParams.dvbt.hierarchy << 4) | (acquireParams->acquireParams.dvbt.modulation << 2) | (!(acquireParams->acquireParams.dvbt.decodeMode) << 1);                    
                        buf[13] |= (acquireParams->acquireParams.dvbt.codeRateLowPriority << 4);
                        buf[13] |= acquireParams->acquireParams.dvbt.codeRateHighPriority;                    
                    }
                    else
                    {                 
                        buf[12] = !(acquireParams->acquireParams.dvbt.tpsMode);
                        buf[12] |= (acquireParams->acquireParams.dvbt.hierarchy << 4);
                    }
                    buf[3] = hImplChnDev->chnNo;                    
                    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 17, buf, 0, false, true, 17 ));
                    hImplChnDev->previousAcquireParams = *acquireParams;
                }
                break;
            case BODS_Standard_eIsdbt:
                if(!((hImplChnDev->previousAcquireParams.acquisitionMode == acquireParams->acquisitionMode) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.bandwidth == acquireParams->acquireParams.isdbt.bandwidth) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.carrierRange == acquireParams->acquireParams.isdbt.carrierRange) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.cciMode == acquireParams->acquireParams.isdbt.cciMode) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.transGuardMode == acquireParams->acquireParams.isdbt.transGuardMode) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.transmissionMode == acquireParams->acquireParams.isdbt.transmissionMode) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.guardInterval == acquireParams->acquireParams.isdbt.guardInterval) &&            
                (hImplChnDev->previousAcquireParams.tuneAcquire == acquireParams->tuneAcquire) &&
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.tmccAcquire == acquireParams->acquireParams.isdbt.tmccAcquire) &&
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.partialReception == acquireParams->acquireParams.isdbt.partialReception) &&          
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.layerAParams.modulation == acquireParams->acquireParams.isdbt.layerAParams.modulation) &&          
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.layerAParams.codeRate == acquireParams->acquireParams.isdbt.layerAParams.codeRate) &&          
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.layerAParams.codeRate == acquireParams->acquireParams.isdbt.layerAParams.codeRate) &&          
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.layerAParams.numSegments == acquireParams->acquireParams.isdbt.layerAParams.numSegments) &&
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.layerBParams.modulation == acquireParams->acquireParams.isdbt.layerBParams.modulation) &&          
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.layerBParams.codeRate == acquireParams->acquireParams.isdbt.layerBParams.codeRate) &&          
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.layerBParams.codeRate == acquireParams->acquireParams.isdbt.layerBParams.codeRate) &&          
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.layerBParams.numSegments == acquireParams->acquireParams.isdbt.layerBParams.numSegments) && 
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.layerCParams.modulation == acquireParams->acquireParams.isdbt.layerCParams.modulation) &&          
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.layerCParams.codeRate == acquireParams->acquireParams.isdbt.layerCParams.codeRate) &&          
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.layerCParams.codeRate == acquireParams->acquireParams.isdbt.layerCParams.codeRate) &&                
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.layerCParams.numSegments == acquireParams->acquireParams.isdbt.layerCParams.numSegments) &&          
                (hImplChnDev->previousAcquireParams.acquireParams.isdbt.layerCParams.numSegments == acquireParams->acquireParams.isdbt.layerCParams.numSegments)))  
                {              
                    buf[1] = buf[1] | 0x1;          
                    buf[2] = (0x8 << 4) | BODS_ISDBT_CORE_TYPE;
                    buf[3] = hImplChnDev->chnNo;                    
                    /* set Acquire Parameters */
                    buf[4] = !acquireParams->acquisitionMode;
                    buf[4] |= (acquireParams->acquireParams.isdbt.bandwidth << 2);
                    buf[4] |= (acquireParams->acquireParams.isdbt.carrierRange << 4);
                    buf[4] |= !(acquireParams->acquireParams.isdbt.cciMode << 5);

                    switch(acquireParams->acquireParams.isdbt.transGuardMode)
                    {
                        case BODS_IsdbtOfdmMode_eManual:
                            transGuardMode = 0;
                            break;
                        case BODS_IsdbtOfdmMode_eAutoBrazil:
                            transGuardMode = 1;
                            break;    
                        case BODS_IsdbtOfdmMode_eAutoJapan:
                            transGuardMode = 2;
                            break;                            
                        default:
                            BDBG_ERR(("Invalid ofdm Mode, switching to BODS_IsdbtOfdmMode_eAutoBrazil"));
                            transGuardMode = 1;                            
                            break;                            
                    }
                    if(acquireParams->acquireParams.isdbt.transGuardMode == BODS_IsdbtOfdmMode_eManual)
                        buf[5] = (acquireParams->acquireParams.isdbt.guardInterval << 4) | (acquireParams->acquireParams.isdbt.transmissionMode << 2) | transGuardMode;
                    else
                        buf[5] = transGuardMode;
                        
                    buf[6] = acquireParams->tuneAcquire << 7;
                  
                    if (acquireParams->acquireParams.isdbt.tmccAcquire == true)
                        buf[12] = acquireParams->acquireParams.isdbt.tmccAcquire;
                    else
                    {                 
                        buf[12] = acquireParams->acquireParams.isdbt.tmccAcquire;
                        buf[12] = (acquireParams->acquireParams.isdbt.partialReception << 1);
                        buf[16] = acquireParams->acquireParams.isdbt.layerAParams.modulation;
                        buf[16] = (acquireParams->acquireParams.isdbt.layerAParams.timeInterleaving << 2);
                        buf[16] = (acquireParams->acquireParams.isdbt.layerAParams.codeRate << 4);                    
                        buf[17] = acquireParams->acquireParams.isdbt.layerAParams.numSegments;
                        buf[20] = acquireParams->acquireParams.isdbt.layerBParams.modulation;
                        buf[20] = (acquireParams->acquireParams.isdbt.layerBParams.timeInterleaving << 2);                    
                        buf[20] = (acquireParams->acquireParams.isdbt.layerBParams.codeRate << 4);
                        buf[21] = acquireParams->acquireParams.isdbt.layerBParams.numSegments;
                        buf[24] = acquireParams->acquireParams.isdbt.layerCParams.modulation;
                        buf[24] = (acquireParams->acquireParams.isdbt.layerCParams.timeInterleaving << 2);                    
                        buf[24] = (acquireParams->acquireParams.isdbt.layerCParams.codeRate << 4);
                        buf[25] = acquireParams->acquireParams.isdbt.layerCParams.numSegments;          
                    }
                     
                    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 29, buf, 0, false, true, 29 ));
                 
                    hImplChnDev->previousAcquireParams = *acquireParams;
                }                
                break;                
            case BODS_Standard_eDvbt2:       
                if(!((hImplChnDev->previousAcquireParams.acquisitionMode == acquireParams->acquisitionMode) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt2.bandwidth == acquireParams->acquireParams.dvbt2.bandwidth) &&            
                (hImplChnDev->previousAcquireParams.tuneAcquire == acquireParams->tuneAcquire) &&
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt2.plpId == acquireParams->acquireParams.dvbt2.plpId) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.dvbt2.plpMode == acquireParams->acquireParams.dvbt2.plpMode)))  
                {                   
                    /* set Acquire Parameters */
                    buf[2] = (buf[2] & 0xF0) | BODS_DVBT2_CORE_TYPE;
                    switch(acquireParams->acquisitionMode) 
                    {                
                        case BODS_AcquisitionMode_eAuto:                    
                            buf[4] = 0x1;                    
                            break;                
                        case BODS_AcquisitionMode_eManual:                    
                            buf[4] = 0;                    
                            break;                            
                        case BODS_AcquisitionMode_eScan:                    
                            buf[4] = 0x2;                   
                            break;                             
                        default:                    
                            buf[4] = 0x1;                       
                    }
                    
                    buf[4] |= (0x1 << 2);            
                    buf[4] |= (0x1 << 3);            
                    buf[4] |= acquireParams->acquireParams.dvbt2.plpMode << 4;            
                    buf[4] |= (acquireParams->acquireParams.dvbt2.bandwidth << 5); 
                    
                    if(!acquireParams->acquireParams.dvbt2.plpMode)                
                        buf[5] = acquireParams->acquireParams.dvbt2.plpId;                       
                    buf[7] |= (acquireParams->tuneAcquire << 7);
                    buf[3] = hImplChnDev->chnNo;
                    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 9, buf, 0, false, true, 9 ));           
                    hImplChnDev->previousAcquireParams = *acquireParams;                    
                }  
                break;
            case BODS_Standard_eDvbc2:              
                if(!((hImplChnDev->previousAcquireParams.acquisitionMode == acquireParams->acquisitionMode) &&
                (hImplChnDev->previousAcquireParams.acquireParams.dvbc2.bandwidth == acquireParams->acquireParams.dvbc2.bandwidth) &&            
                (hImplChnDev->previousAcquireParams.tuneAcquire == acquireParams->tuneAcquire) &&
                (hImplChnDev->previousAcquireParams.acquireParams.dvbc2.plpId == acquireParams->acquireParams.dvbc2.plpId) &&            
                (hImplChnDev->previousAcquireParams.acquireParams.dvbc2.plpMode == acquireParams->acquireParams.dvbc2.plpMode)))   
                {                   
                    /* set Acquire Parameters */                         
                    switch(acquireParams->acquisitionMode) 
                    {                
                        case BODS_AcquisitionMode_eAuto:                    
                            buf[4] = 0x1;                    
                            break;                
                        case BODS_AcquisitionMode_eManual:                    
                            buf[4] = 0;                    
                            break;                            
                        case BODS_AcquisitionMode_eScan:                    
                            buf[4] = 0x2;                   
                            break;                             
                        default:                    
                            buf[4] = 0x1;                       
                    }
                    
                    buf[4] |= (0x1 << 2);            
                    buf[4] |= (0x1 << 3);            
                    buf[4] |= acquireParams->acquireParams.dvbc2.plpMode << 4;            
                    buf[4] |= (acquireParams->acquireParams.dvbc2.bandwidth << 5);
                    
                    if(!acquireParams->acquireParams.dvbc2.plpMode)                
                        buf[5] = acquireParams->acquireParams.dvbc2.plpId;                       
                    buf[7] |= (acquireParams->tuneAcquire << 7);
                    buf[3] = hImplChnDev->chnNo;
                    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 9, buf, 0, false, true, 9 ));           
                    hImplChnDev->previousAcquireParams = *acquireParams;                    
                }  
                break;
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);                
                break;
        }
    }
 
done:
    BDBG_LEAVE(BODS_Leap_SetAcquireParams);
    return retCode;
}

BERR_Code BODS_Leap_GetAcquireParams(
    BODS_ChannelHandle hChn ,           /* [in] Device channel handle */
    BODS_AcquireParams *acquireParams   /* [in] Acquire Parameters to use */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev; 
    uint8_t buf[29] = HAB_MSG_HDR(BODS_eAcquireParamsRead, 0, BODS_DVBC2_CORE_TYPE, BODS_CORE_ID);

    BDBG_ENTER(BODS_Leap_GetAcquireParams);
    BDBG_ASSERT( hChn );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );
    BSTD_UNUSED(acquireParams);

    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    BKNI_AcquireMutex(hImplChnDev->mutex);
    hImplChnDev->isLock = false;
    BKNI_ReleaseMutex(hImplChnDev->mutex);

    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("ODS core Powered Off"));
        retCode = BODS_ERR_POWER_DOWN;
    }
    else
    {
        BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));
        
        switch(hImplChnDev->settings.standard)
        {
            case BODS_Standard_eDvbt:
                BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));
                 
                buf[2] = (buf[2] & 0xF0) | BODS_DVBT_CORE_TYPE;
                buf[3] = hImplChnDev->chnNo;                
                CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, hImplChnDev->readBuf, 29, false, true, 29 ));
                acquireParams->acquisitionMode = (BODS_AcquisitionMode)(hImplChnDev->readBuf[4] & 0x3);
                acquireParams->acquireParams.dvbt.bandwidth =  (BODS_DvbtBandwidth)((hImplChnDev->readBuf[4] & 0xC) >> 2);
                acquireParams->acquireParams.dvbt.carrierRange = (BODS_DvbtCarrierRange)((hImplChnDev->readBuf[4] & 0x10) >> 4);
                acquireParams->acquireParams.dvbt.cciMode = (BODS_DvbtCciMode)((hImplChnDev->readBuf[4] & 0x20) >> 5);
                acquireParams->acquireParams.dvbt.transGuardMode = (BODS_DvbtOfdmMode)(hImplChnDev->readBuf[5] & 0x3);
                acquireParams->acquireParams.dvbt.transmissionMode = (BODS_DvbtTransmissionMode)((hImplChnDev->readBuf[5] & 0xC) >> 2);
                acquireParams->acquireParams.dvbt.guardInterval = (BODS_DvbtGuardInterval)((hImplChnDev->readBuf[5] & 0x30) >> 4);
                acquireParams->tuneAcquire = (hImplChnDev->readBuf[6] & 0x80) >> 7;
                acquireParams->acquireParams.dvbt.tpsMode = (BODS_DvbtTransmissionMode)(hImplChnDev->readBuf[12] & 0x1);
                acquireParams->acquireParams.dvbt.decodeMode = (BODS_DvbtDecodeMode)((hImplChnDev->readBuf[12] & 0x2) >> 1);           
                acquireParams->acquireParams.dvbt.modulation = (BODS_DvbtTransmissionMode)((hImplChnDev->readBuf[12] & 0xC) >> 2);
                acquireParams->acquireParams.dvbt.hierarchy = (BODS_DvbtHierarchy)((hImplChnDev->readBuf[12] & 0x30) >> 4);                
                acquireParams->acquireParams.dvbt.codeRateHighPriority = (BODS_DvbtTransmissionMode)(hImplChnDev->readBuf[13] & 0x7);
                acquireParams->acquireParams.dvbt.codeRateLowPriority = (BODS_DvbtTransmissionMode)((hImplChnDev->readBuf[5] & 0x70) >> 4);
                break;
            case BODS_Standard_eIsdbt:
                BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));
                 
                buf[2] = (buf[2] & 0xF0) | BODS_ISDBT_CORE_TYPE;
                buf[3] = hImplChnDev->chnNo;                
                CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, hImplChnDev->readBuf, 17, false, true, 17 ));
                acquireParams->acquisitionMode = (BODS_AcquisitionMode)(hImplChnDev->readBuf[4] & 0x3);
                acquireParams->acquireParams.isdbt.bandwidth = (BODS_IsdbtBandwidth)((hImplChnDev->readBuf[4] & 0xC) >> 2);
                acquireParams->acquireParams.isdbt.carrierRange = (BODS_IsdbtCarrierRange)((hImplChnDev->readBuf[4] & 0x10) >> 4);
                acquireParams->acquireParams.isdbt.cciMode = (BODS_IsdbtCciMode)((hImplChnDev->readBuf[4] & 0x20) >> 5);
                acquireParams->acquireParams.isdbt.transGuardMode = (BODS_IsdbtOfdmMode)(hImplChnDev->readBuf[5] & 0x3);
                acquireParams->acquireParams.isdbt.transmissionMode = (BODS_IsdbtTransmissionMode)((hImplChnDev->readBuf[5] & 0xC) >> 2);
                acquireParams->acquireParams.isdbt.guardInterval = (BODS_IsdbtGuardInterval)((hImplChnDev->readBuf[5] & 0x30) >> 4);
                acquireParams->tuneAcquire = (hImplChnDev->readBuf[6] & 0x80) >> 7;
                acquireParams->acquireParams.isdbt.tmccAcquire = hImplChnDev->readBuf[12] & 0x1;
                acquireParams->acquireParams.isdbt.partialReception = ((hImplChnDev->readBuf[12] & 0x2) >> 1);
                acquireParams->acquireParams.isdbt.layerAParams.modulation = (BODS_IsdbtModulation)(hImplChnDev->readBuf[16] & 0x30);
                acquireParams->acquireParams.isdbt.layerAParams.timeInterleaving = (BODS_IsdbtTimeInterleaving)((hImplChnDev->readBuf[16] & 0xC) >> 2);
                acquireParams->acquireParams.isdbt.layerAParams.codeRate = (BODS_IsdbtCodeRate)((hImplChnDev->readBuf[16] & 0x70) >> 4);
                acquireParams->acquireParams.isdbt.layerAParams.numSegments = ((hImplChnDev->readBuf[5] & 0x30) >> 4);
                acquireParams->acquireParams.isdbt.layerBParams.modulation = (BODS_IsdbtModulation)(hImplChnDev->readBuf[20] & 0x30);
                acquireParams->acquireParams.isdbt.layerBParams.timeInterleaving = (BODS_IsdbtTimeInterleaving)((hImplChnDev->readBuf[20] & 0xC) >> 2);
                acquireParams->acquireParams.isdbt.layerBParams.codeRate = (BODS_IsdbtCodeRate)((hImplChnDev->readBuf[20] & 0x70) >> 4);
                acquireParams->acquireParams.isdbt.layerBParams.numSegments = ((hImplChnDev->readBuf[21] & 0x30) >> 4);
                acquireParams->acquireParams.isdbt.layerCParams.modulation = (BODS_IsdbtModulation)(hImplChnDev->readBuf[24] & 0x30);
                acquireParams->acquireParams.isdbt.layerCParams.timeInterleaving = (BODS_IsdbtTimeInterleaving)((hImplChnDev->readBuf[24] & 0xC) >> 2);
                acquireParams->acquireParams.isdbt.layerCParams.codeRate = (BODS_IsdbtCodeRate)((hImplChnDev->readBuf[24] & 0x70) >> 4);
                acquireParams->acquireParams.isdbt.layerCParams.numSegments = ((hImplChnDev->readBuf[24] & 0x30) >> 4);
                break;                
            case BODS_Standard_eDvbt2:        
                BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));
                 
                buf[2] = (buf[2] & 0xF0) | BODS_DVBT2_CORE_TYPE;
                buf[3] = hImplChnDev->chnNo;                
                CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, hImplChnDev->readBuf, 9, false, true, 9 ));
                acquireParams->acquisitionMode = (BODS_AcquisitionMode)(hImplChnDev->readBuf[4] & 0x3);                
                acquireParams->acquireParams.dvbt2.plpMode = (hImplChnDev->readBuf[4] & 0x10) >> 4;
                acquireParams->acquireParams.dvbt2.bandwidth = (BODS_Dvbt2Bandwidth)((hImplChnDev->readBuf[4] & 0xE0) >> 5);
                if(!acquireParams->acquireParams.dvbt2.plpMode)
                   acquireParams->acquireParams.dvbt2.plpId = hImplChnDev->readBuf[5];
                acquireParams->tuneAcquire = (hImplChnDev->readBuf[7] & 0x80) >> 7;
                break;
            case BODS_Standard_eDvbc2:            
                BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));
                
                buf[3] = hImplChnDev->chnNo;
                CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, hImplChnDev->readBuf, 9, false, true, 9 ));
                acquireParams->acquisitionMode = (BODS_AcquisitionMode)(hImplChnDev->readBuf[4] & 0x3);                
                acquireParams->acquireParams.dvbc2.plpMode = (hImplChnDev->readBuf[4] & 0x10) >> 4;
                acquireParams->acquireParams.dvbc2.bandwidth = (BODS_Dvbc2Bandwidth)((hImplChnDev->readBuf[4] & 0xE0) >> 5);
                if(!acquireParams->acquireParams.dvbc2.plpMode)
                   acquireParams->acquireParams.dvbc2.plpId = hImplChnDev->readBuf[5];
                acquireParams->tuneAcquire = (hImplChnDev->readBuf[7] & 0x80) >> 7;
                break;
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);                
                break;
        }

    }

done:
    BDBG_LEAVE(BODS_Leap_GetAcquireParams);
    return retCode;
}
    
BERR_Code BODS_Leap_Acquire(
    BODS_ChannelHandle hChn,             /* [in] Device channel handle */
    BODS_AcquireParams *acquireParams     /* [in] Acquire Parameters to use */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;   
    uint8_t buf[5] = HAB_MSG_HDR(BODS_eAcquire, 0, BODS_DVBC2_CORE_TYPE, BODS_CORE_ID);  

    BDBG_ENTER(BODS_Leap_Acquire);
    BDBG_ASSERT( hChn );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );  
    BSTD_UNUSED(acquireParams); /* TODO: Remove acquireParams parameter as it is not needed ?????? */               
            
    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    BKNI_AcquireMutex(hImplChnDev->mutex);
    hImplChnDev->isLock = false;
    BKNI_ReleaseMutex(hImplChnDev->mutex); 
    
    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("ODS core Powered Off"));
        retCode = BODS_ERR_POWER_DOWN;
    }
    else
    {  
        /* Acquire */
		switch(hImplChnDev->settings.standard)
        {
            case BODS_Standard_eDvbt:
                buf[2] = (buf[2] & 0xF0) | BODS_DVBT_CORE_TYPE;                
                break;
            case BODS_Standard_eIsdbt:      
                buf[2] = (buf[2] & 0xF0) | BODS_ISDBT_CORE_TYPE;                
                break;                
            case BODS_Standard_eDvbt2:
                buf[2] = (buf[2] & 0xF0) | BODS_DVBT2_CORE_TYPE;            
                break;
            case BODS_Standard_eDvbc2:
                break;
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);                
                break;
        }

        buf[3] = hImplChnDev->chnNo;        
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, buf, 0, false, true, 5 ));
        CHK_RETCODE(retCode, BHAB_EnableLockInterrupt(hImplChnDev->hHab, hImplChnDev->devId, true));
    }
  
done:
    BDBG_LEAVE(BODS_Leap_Acquire);
    return retCode;
}

BERR_Code BODS_Leap_EnablePowerSaver(
    BODS_ChannelHandle hChn,                /* [in] Device channel handle */
    BODS_PowerSaverSettings *pwrSettings    /* [in] Power saver settings */   
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;    
    uint8_t buf[5] = HAB_MSG_HDR(BODS_ePowerCtrlOff, 0x0, BODS_DVBC2_CORE_TYPE, BODS_CORE_ID);    

    BDBG_ENTER(BODS_Leap_EnablePowerSaver);
    BDBG_ASSERT( hChn );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );

    BSTD_UNUSED(pwrSettings);   

    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    
    if(!hImplChnDev->bPowerdown)
    {
        switch(hImplChnDev->settings.standard)
        {
            case BODS_Standard_eDvbt:
                buf[2] = (buf[2] & 0xF0) | BODS_DVBT_CORE_TYPE;                
                break;
            case BODS_Standard_eIsdbt:
                buf[2] = (buf[2] & 0xF0) | BODS_ISDBT_CORE_TYPE;                
                break;                
            case BODS_Standard_eDvbt2:
                buf[2] = (buf[2] & 0xF0) | BODS_DVBT2_CORE_TYPE;            
                break;
            case BODS_Standard_eDvbc2:
                break;
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);                
                break;
        }

        buf[3] = hImplChnDev->chnNo;        
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, buf, 0, false, true, 5));
        hImplChnDev->bPowerdown = true;
    }
    
done:
    BDBG_LEAVE(BODS_Leap_EnablePowerSaver);
    return retCode;
}

BERR_Code BODS_Leap_DisablePowerSaver(
    BODS_ChannelHandle hChn,                /* [in] Device channel handle */
    BODS_PowerSaverSettings *pwrSettings    /* [in] Power saver settings */    
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;
    uint8_t buf[5] = HAB_MSG_HDR(BODS_ePowerCtrlOn, 0x0, BODS_DVBC2_CORE_TYPE, BODS_CORE_ID);   
    uint8_t configParams[9] = HAB_MSG_HDR(BODS_CONFIG_PARAMS_WRITE, 0x4, BODS_DVBC2_CORE_TYPE, BODS_CORE_ID);

    BDBG_ENTER(BODS_Leap_DisablePowerSaver);
    BDBG_ASSERT( hChn );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );

    BSTD_UNUSED(pwrSettings);    

    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    if(hImplChnDev->bPowerdown)
    {  
        switch(hImplChnDev->settings.standard)
        {
            case BODS_Standard_eDvbt:
                buf[2] = (buf[2] & 0xF0) | BODS_DVBT_CORE_TYPE;
                configParams[4] = BODS_THD_CONFIG_PARAMS_BUF1;
                configParams[5] = BODS_THD_CONFIG_PARAMS_BUF2;
                configParams[6] = BODS_THD_CONFIG_PARAMS_BUF3;             
                configParams[2] = (configParams[2] & 0xF0) | BODS_DVBT_CORE_TYPE;                
                break;
            case BODS_Standard_eIsdbt:
                buf[2] = (buf[2] & 0xF0) | BODS_ISDBT_CORE_TYPE;
                break;                
            case BODS_Standard_eDvbt2:
                buf[2] = (buf[2] & 0xF0) | BODS_DVBT2_CORE_TYPE;            
                configParams[2] = (configParams[2] & 0xF0) | BODS_DVBT2_CORE_TYPE;
                configParams[4] = BODS_DVBT2_CONFIG_PARAMS_BUF1;
                configParams[5] = BODS_DVBT2_CONFIG_PARAMS_BUF2;
                configParams[6] = BODS_DVBT2_CONFIG_PARAMS_BUF3;
                break;
            case BODS_Standard_eDvbc2:
                configParams[4] = BODS_DVBC2_CONFIG_PARAMS_BUF1;
                configParams[5] = BODS_DVBC2_CONFIG_PARAMS_BUF2;
                configParams[6] = BODS_DVBC2_CONFIG_PARAMS_BUF3;            
                break;
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);                
                break;
        }

        buf[3] = hImplChnDev->chnNo;
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, buf, 0, false, true, 5));
        hImplChnDev->bPowerdown = false;        
    }
    
#if ((BODS_CHIP == 3462) || (BODS_CHIP == 7563))
    if(hImplChnDev->verInfo.majFwVer >= 5) {
	    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, configParams, 9, hImplChnDev->readBuf, 0, false, true, 9));
		}
#endif   
  
done:
    BDBG_LEAVE(BODS_Leap_DisablePowerSaver);
    return retCode;
}

BERR_Code BODS_Leap_ResetStatus(
    BODS_ChannelHandle hChn             /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;
    uint8_t selReset[9] = HAB_MSG_HDR(BODS_eResetSelectiveAsyncStatus, 0x4, BODS_DVBC2_CORE_TYPE, BODS_CORE_ID);
    uint8_t reset[5] = HAB_MSG_HDR(BODS_eResetStatus, 0, BODS_DVBT_CORE_TYPE, BODS_CORE_ID);
    uint8_t write_len = 0;    
    
    BDBG_ENTER(BODS_Leap_ResetStatus);
    BDBG_ASSERT( hChn );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );


    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("ODS core Powered Off"));
        retCode = BODS_ERR_POWER_DOWN;
    }
    else
    {
        switch(hImplChnDev->settings.standard)
        {
            case BODS_Standard_eIsdbt:
                reset[2] = BODS_ISDBT_CORE_TYPE;     
            case BODS_Standard_eDvbt:
                reset[3] = hImplChnDev->chnNo;            
                write_len = 5;
                CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, reset, write_len, reset, 0, false, true, write_len));                
                break;
            case BODS_Standard_eDvbt2:
                selReset[2] = (selReset[2] & 0xF0) | BODS_DVBT2_CORE_TYPE;
            case BODS_Standard_eDvbc2:            
                selReset[3] = hImplChnDev->chnNo;
                write_len = 9;
                CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, selReset, write_len, selReset, 0, false, true, write_len));
                break;
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);                
                break;
        }
    }

done:
    BDBG_LEAVE(BODS_Leap_ResetStatus);
    return retCode;
}

BERR_Code BODS_Leap_RequestSelectiveAsyncStatus(
    BODS_ChannelHandle hChn,
    BODS_SelectiveAsyncStatusType type
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;
    uint8_t buf[9] = HAB_MSG_HDR(BODS_eRequestSelectiveAsyncStatus, 0x4, BODS_DVBC2_CORE_TYPE, BODS_CORE_ID);    
    
    BDBG_ENTER(BODS_Leap_GetSelectiveAsyncStatus);
    BDBG_ASSERT( hChn );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );

    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("ODS core Powered Off"));
        retCode = BODS_ERR_POWER_DOWN;
    }
    else
    {   
        switch(hImplChnDev->settings.standard)
        {
            case BODS_Standard_eDvbt:
                buf[0] = 0x5;
                buf[1] = 0x80;
                buf[2] = BODS_DVBT_CORE_TYPE;
                buf[3] = hImplChnDev->chnNo;                
                CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, buf, 0, false, true, 5));                
                break;
            case BODS_Standard_eIsdbt:
                buf[0] = 0x5;
                buf[1] = 0x80;
                buf[2] = BODS_ISDBT_CORE_TYPE;              
                buf[3] = hImplChnDev->chnNo;                
                CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, buf, 0, false, true, 5));                
                break;                
            case BODS_Standard_eDvbt2:
                buf[2] = (buf[2] & 0xF0) | BODS_DVBT2_CORE_TYPE;            
                switch(type)
                {
                    case BODS_SelectiveAsyncStatusType_eDvbt2FecStatisticsL1Pre:
                        buf[5] = 6;
                        break;
                    case BODS_SelectiveAsyncStatusType_eDvbt2FecStatisticsL1Post:
                        buf[5] = 7;
                        break;
                    case BODS_SelectiveAsyncStatusType_eDvbt2FecStatisticsPlpA:
                        buf[5] = 8;
                        break;
                    case BODS_SelectiveAsyncStatusType_eDvbt2FecStatisticsPlpB:
                        buf[5] = 9;
                        break;
                    case BODS_SelectiveAsyncStatusType_eDvbt2L1Pre:
                        buf[5] = 1;
                        break;    
                    case BODS_SelectiveAsyncStatusType_eDvbt2L1PostConfigurable:
                        buf[5] = 2;
                        break;        
                    case BODS_SelectiveAsyncStatusType_eDvbt2L1PostDynamic:
                        buf[5] = 3;
                        break; 
                    case BODS_SelectiveAsyncStatusType_eDvbt2L1Plp:
                        buf[5] = 5;
                        break; 
                    case BODS_SelectiveAsyncStatusType_eDvbt2Short:
                        buf[5] = 10;
                        break;                 
                    default:
                        retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                        break;
                }
                buf[3] = hImplChnDev->chnNo;                
                CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 9, buf, 0, false, true, 9));            
                break;
            case BODS_Standard_eDvbc2:
                switch(type)
                {
                    case BODS_SelectiveAsyncStatusType_eDvbc2FecStatisticsL1Part2:
                        buf[5] = 5;
                        break;
                    case BODS_SelectiveAsyncStatusType_eDvbc2FecStatisticsPlpA:
                        buf[5] = 6;
                        break;
                    case BODS_SelectiveAsyncStatusType_eDvbc2FecStatisticsPlpB:
                        buf[5] = 7;
                        break;
                    case BODS_SelectiveAsyncStatusType_eDvbc2L1Part2:
                        buf[5] = 1;
                        break;    
                    case BODS_SelectiveAsyncStatusType_eDvbc2L1Dslice:
                        buf[5] = 2;
                        break;        
                    case BODS_SelectiveAsyncStatusType_eDvbc2L1Notch:
                        buf[5] = 3;
                        break; 
                    case BODS_SelectiveAsyncStatusType_eDvbc2L1Plp:
                        buf[5] = 4;
                        break; 
                    case BODS_SelectiveAsyncStatusType_eDvbc2Short:
                        buf[5] = 8;
                        break;                 
                    default:
                        retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                        break;
                }
                buf[3] = hImplChnDev->chnNo;                
                CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 9, buf, 0, false, true, 9));                 
                break;
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);    
                break;
        }
    }

done:
    BDBG_LEAVE(BODS_Leap_GetSelectiveAsyncStatus);
    return retCode;
}

BERR_Code BODS_Leap_GetSelectiveAsyncStatusReadyType(
    BODS_ChannelHandle hChn,
    BODS_SelectiveAsyncStatusReadyType *ready
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;
    uint8_t buf[9] = HAB_MSG_HDR(BODS_eGetSelectiveAsyncStatusReadyType, 0, BODS_DVBC2_CORE_TYPE, BODS_CORE_ID);    
    uint16_t statusReady = 0, i=0;
    
    BDBG_ENTER(BODS_Leap_GetSelectiveAsyncStatusReadyType);
    BDBG_ASSERT( hChn );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );

    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("ODS core Powered Off"));
        retCode = BODS_ERR_POWER_DOWN;
    }
    else
    {
        BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));    
        switch(hImplChnDev->settings.standard)
        {
            case BODS_Standard_eDvbt:          
                ready->dvbt = 1;
                break;
            case BODS_Standard_eIsdbt:          
                ready->isdbt = 1;
                break;                
            case BODS_Standard_eDvbt2:        
                buf[2] = (buf[2] & 0xF0) | BODS_DVBT2_CORE_TYPE;
                buf[3] = hImplChnDev->chnNo;                
                CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, hImplChnDev->readBuf, 9, false, true, 9));
                   
                for(i=1; i< 11; i++)
                {
                    if((((hImplChnDev->readBuf[6] << 8) | hImplChnDev->readBuf[7]) >> i) & 0x1)
                        statusReady = i;
                    switch(statusReady)
                    {
                        case 0:
                            break;
                        case 1:
                            ready->dvbt2L1Pre = 1;
                            break;
                        case 2:
                            ready->dvbt2L1PostConfigurable = 1;
                            break;
                        case 3:
                            ready->dvbt2L1PostDynamic = 1;
                            break;
                        case 5:
                            ready->dvbt2L1Plp = 1;
                            break;
                        case 6:
                            ready->dvbt2FecStatisticsL1Pre = 1;
                            break;    
                        case 7:
                            ready->dvbt2FecStatisticsL1Post = 1;
                            break;        
                        case 8:
                            ready->dvbt2FecStatisticsPlpA = 1;
                            break; 
                        case 9:
                            ready->dvbt2FecStatisticsPlpB = 1;
                            break; 
                        case 10:
                            ready->dvbt2ShortStatus = 1;
                            break;                 
                        default:
                            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                            break;
                    }           
                }
                break;
            case BODS_Standard_eDvbc2:
                buf[3] = hImplChnDev->chnNo;                
                CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, hImplChnDev->readBuf, 9, false, true, 9));
                for(i=0; i< 11; i++)
                {
                    if((((hImplChnDev->readBuf[6] << 8) | hImplChnDev->readBuf[7]) >> i) & 0x1)
                        statusReady = i;
              
                    switch(statusReady)
                    {
                        case 0:
                            break;
                        case 1:
                            ready->dvbc2L1Part2 = 1;
                            break;
                        case 2:
                            ready->dvbc2L1Dslice = 1;
                            break;
                        case 3:
                            ready->dvbc2L1Notch = 1;
                            break;
                        case 4:
                            ready->dvbc2L1Plp = 1;
                            break;
                        case 5:
                            ready->dvbc2FecStatisticsL1Part2 = 1;
                            break;       
                        case 6:
                            ready->dvbc2FecStatisticsPlpA = 1;
                            break; 
                        case 7:
                            ready->dvbc2FecStatisticsPlpB = 1;
                            break; 
                        case 8:
                            ready->dvbc2ShortStatus = 1;
                            break;                 
                        default:
                            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                            break;
                    }
                }
                break;
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);    
                break;
        }
    }

done:    
    BDBG_LEAVE(BODS_Leap_GetSelectiveAsyncStatusReadyType);
    return retCode;
}

BERR_Code BODS_Leap_GetSelectiveAsyncStatus(
    BODS_ChannelHandle hChn,
    BODS_SelectiveAsyncStatusType type,
    BODS_SelectiveStatus *pStatus   /* [out] */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BODS_Leap_ChannelHandle hImplChnDev;
    
    BDBG_ENTER(BODS_Leap_GetSelectiveAsyncStatus);
    BDBG_ASSERT( hChn );
    BDBG_OBJECT_ASSERT( hChn,BODS_CHANNEL );

    hImplChnDev = (BODS_Leap_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("ODS core Powered Off"));
        retCode = BODS_ERR_POWER_DOWN;
    }
    else
    {
        switch(hImplChnDev->settings.standard)
        {
            case BODS_Standard_eDvbt:
                BODS_Leap_P_GetDvbtSelectiveAsyncStatus(hChn, pStatus);            
                break;
            case BODS_Standard_eIsdbt:
                BODS_Leap_P_GetIsdbtSelectiveAsyncStatus(hChn, pStatus);            
                break;                
            case BODS_Standard_eDvbt2:
                BODS_Leap_P_GetDvbt2SelectiveAsyncStatus(hChn, type, pStatus);
                break;
            case BODS_Standard_eDvbc2:
                BODS_Leap_P_GetDvbc2SelectiveAsyncStatus(hChn, type, pStatus);            
                break;
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);    
                break;
        }
    }

    BDBG_LEAVE(BODS_Leap_GetSelectiveAsyncStatus);
    return retCode;
}

