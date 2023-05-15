/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btc2_3461_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/58 $
 * $brcm_Date: 10/25/12 2:54p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tc2/src/3461/btc2_3461_priv.c $
 * 
 * Hydra_Software_Devel/58   10/25/12 2:54p atanugul
 * SW3461-269: Update BTC2_3461_Init() and BADS_Leap_Init() based on the
 * VersionInfo() API
 * 
 * Hydra_Software_Devel/57   10/18/12 5:16p atanugul
 * SW3461-269: Merge to main
 * 
 * Hydra_Software_Devel/SW3461-252/2   10/17/12 12:04p vishk
 * SW3461-270: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/SW3461-252/1   9/28/12 5:06p atanugul
 * SW3461-269: Add support for BTC2_GetVersionInfo()
 * 
 * Hydra_Software_Devel/56   9/7/12 11:41a atanugul
 * SW7425-3788: restructure HAB directory
 * 
 * Hydra_Software_Devel/55   8/3/12 3:06p atanugul
 * SW3461-229: Send receiver configuration HAB command for FW version 5.0
 * or newer
 * 
 * Hydra_Software_Devel/54   5/23/12 6:04p atanugul
 * SW3461-212: Add TS out to THD and TC2 acquireparameters
 * 
 * Hydra_Software_Devel/53   3/12/12 6:56p atanugul
 * SW3461-176: 3461 lock recovery issue after Untune()
 * 
 * Hydra_Software_Devel/52   2/15/12 11:05a atanugul
 * SW3461-157: Initialize bandwidth during openchannel()
 * 
 * Hydra_Software_Devel/51   2/15/12 10:57a atanugul
 * SW3461-157: Memset handles in openchannel()
 * 
 * Hydra_Software_Devel/50   2/14/12 11:50a atanugul
 * SW7231-317: Change BDBG_ERR to BDBG_WRN for LEAP version
 * 
 * Hydra_Software_Devel/49   2/10/12 10:43a atanugul
 * SW3461-148: Using malloc instead of using memory from the stack
 * 
 * Hydra_Software_Devel/48   2/8/12 12:24a atanugul
 * SW3461-141: Add check for plpId in BTC2_3461_SetAcquireParams()
 * 
 * Hydra_Software_Devel/47   2/7/12 5:08p atanugul
 * SW3461-144: Change signalstrength units to 1/100th of a dBmV
 * 
 * Hydra_Software_Devel/46   2/7/12 11:50a atanugul
 * SW3461-146: Add "Scan Mode" to T and T2 Acquire Parameters to 3461 THD
 * and TC2 PI
 * 
 * Hydra_Software_Devel/45   2/2/12 6:18p atanugul
 * SW3461-141: Implement new apis for SetAcquireParams, GetAcquireParams
 * and GetDefaultAcquireParams
 * 
 * Hydra_Software_Devel/44   1/24/12 11:16a atanugul
 * SW3461-132: Cleanup
 * 
 * Hydra_Software_Devel/43   1/24/12 10:16a atanugul
 * SW3461-92: Cleanup
 * 
 * Hydra_Software_Devel/42   1/23/12 3:03p atanugul
 * SW3461-92: Fix type in BTC2_3461_GetSelectiveAsyncStatus
 * 
 * Hydra_Software_Devel/41   1/23/12 2:24p atanugul
 * SW3461-136: Fix signalstrength units
 * 
 * Hydra_Software_Devel/40   1/20/12 5:36p atanugul
 * SW3461-92: Update  BTC2_GetSelectiveAsyncStatusReadyType; cleanup
 * 
 * Hydra_Software_Devel/39   1/20/12 10:33a atanugul
 * SW3461-92: Disable Legacy Status API from FW version 4.0
 * 
 * Hydra_Software_Devel/38   1/19/12 4:59p atanugul
 * SW3461-135: Add auto acquire on tune support for 3461
 * 
 * Hydra_Software_Devel/37   1/16/12 6:13p atanugul
 * SW3461-123: Update bandwidth filed in T2 status
 * 
 * Hydra_Software_Devel/36   1/13/12 3:25p atanugul
 * SW3461-133: Add SignalLevelPercent and SignalQualityPercent to THD and
 * TC2 PIs
 * 
 * Hydra_Software_Devel/35   1/13/12 12:30p atanugul
 * SW3461-132: Enable HAB CRC check
 * 
 * Hydra_Software_Devel/34   1/13/12 10:50a atanugul
 * SW3461-132: Enable HAB CRC check
 * 
 * Hydra_Software_Devel/33   1/12/12 5:54p atanugul
 * SW3461-132: Enable HAB CRC check
 * 
 * Hydra_Software_Devel/32   1/11/12 11:34a atanugul
 * SW3461-128: Update BHAB_GetApVersion() to handle all minor chip
 * revisions
 * 
 * Hydra_Software_Devel/31   12/21/11 6:02p atanugul
 * SW3461-92: Add TC2 Selective Status support for 3461 PI
 * 
 * Hydra_Software_Devel/30   12/1/11 3:54p atanugul
 * SW3461-77: Check for power status before sending HAB commands
 * 
 * Hydra_Software_Devel/29   11/30/11 3:54p atanugul
 * SW3461-77: Check for power status before sending HAB commands
 * 
 * Hydra_Software_Devel/28   11/16/11 3:36p atanugul
 * SW3461-79: Add No Signal to GetLockStatus()
 * 
 * Hydra_Software_Devel/27   11/2/11 4:43p atanugul
 * SW3461-3: Update GetVersion() based on BHAB_GetApVersion() update
 * 
 * Hydra_Software_Devel/26   10/18/11 5:27p atanugul
 * SW3461-67: Add 3461 B0 Support
 * 
 * Hydra_Software_Devel/25   9/22/11 10:33a atanugul
 * SW3461-3: Merge to main
 * 
 * Hydra_Software_Devel/24   9/19/11 6:02p atanugul
 * SW3461-24: Fix plpMode BTC2_3461_Acquire()
 * 
 * Hydra_Software_Devel/23   9/8/11 4:26p atanugul
 * SW3461-14: Fix signal Strength
 * 
 * Hydra_Software_Devel/22   9/8/11 1:08p atanugul
 * SW3461-36: Add DVB T2 status API
 * 
 * Hydra_Software_Devel/21   8/18/11 3:34p atanugul
 * SW3461-3: Fix plpId, snrEstimate
 * 
 * Hydra_Software_Devel/20   8/17/11 5:23p atanugul
 * SW3461-25: Update PLP mode in BTC2_Acquire()
 * 
 * Hydra_Software_Devel/19   8/17/11 3:31p atanugul
 * SW3461-36: Implement BTC2_GetLockStatus()
 * 
 * Hydra_Software_Devel/18   8/16/11 6:20p atanugul
 * SW3461-36: Update description; cleanup
 * 
 * Hydra_Software_Devel/17   8/16/11 3:44p atanugul
 * SW3461-36: Remove BTC2_StatusType
 * 
 * Hydra_Software_Devel/16   8/16/11 2:57p atanugul
 * SW3461-36: Cleanup
 * 
 * Hydra_Software_Devel/15   8/16/11 12:37p atanugul
 * SW3461-36: Expose DVB-T2 L1 Status at Magnum layer
 * 
 * Hydra_Software_Devel/14   8/12/11 4:17p atanugul
 * SW3461-5: Fix GetSoftDecision()
 * 
 * Hydra_Software_Devel/13   8/12/11 12:55p atanugul
 * SW3461-5: Update BTC2_3461_GetAsyncStatus() with FEC statistics
 * 
 * Hydra_Software_Devel/12   7/22/11 5:34p atanugul
 * SW3461-14: Add signal strength to T and T2 Status
 * 
 * Hydra_Software_Devel/11   7/21/11 4:48p atanugul
 * SW3461-18: Fix BTC2_3461_RequestAsyncStatus() and
 * BTC2_3461_GetAsyncStatus()
 * 
 * Hydra_Software_Devel/10   6/6/11 1:06p atanugul
 * SW3461-3:  remove untun(), getlockstatus(); fix resetStatus
 * 
 * Hydra_Software_Devel/9   6/3/11 5:22p atanugul
 * SW3461-3: Implemented hab command for rfinputmode, updated tc2 acquire
 * parameters, TNR_GetPowerSaver()
 * 
 * Hydra_Software_Devel/8   5/24/11 7:06p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip
 * 
 * Hydra_Software_Devel/7   5/5/11 11:03a atanugul
 * SW3461-3: Update BTC2_3461_Acquire()
 * 
 * Hydra_Software_Devel/6   5/3/11 11:54a vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 * Implement Tc2 power saver functions.
 * 
 * Hydra_Software_Devel/5   5/3/11 10:59a atanugul
 * sw3461-3: Update BTC2_3461_Acquire()
 * 
 * Hydra_Software_Devel/4   4/19/11 12:34p atanugul
 * SW3461-3: Implement BTNR_SetSettings(), Powermode for 3461 TNR and
 * update BHAB_3461_P_DecodeInterrupt()
 * 
 * Hydra_Software_Devel/3   3/31/11 11:25a atanugul
 * SW3461-3: Update GetStatus, DecodeInterrupt and code cleanup
 * 
 * Hydra_Software_Devel/2   3/17/11 1:14p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip
 * 
 * Hydra_Software_Devel/1   3/16/11 4:14p atanugul
 * SW3461-3: Add support for BCM3461
 * 
 *
 ***************************************************************************/
#include "bstd.h"
#include "btc2.h"
#include "bhab.h"
#include "btc2_priv.h"
#include "btc2_3461_priv.h"
#include "../../b0/bchp_leap_ctrl.h"

BDBG_MODULE(btc2_3461_priv);

#define CHK_RETCODE( rc, func )             \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)

#define MX_TC2_CHANNELS         (1)
#define DEV_MAGIC_ID            ((BERR_TC2_ID<<16) | 0xFACE)

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/
typedef struct BTC2_P_3461_Handle               *BTC2_3461_Handle;
typedef struct BTC2_P_3461_Handle
{
    uint32_t magicId;                   /* Used to check if structure is corrupt */
    BCHP_Handle hChip;
    BREG_Handle hRegister;
    BINT_Handle hInterrupt;
    BHAB_DevId devId;
    BHAB_Handle hHab;
    BTC2_Version verInfo;
    unsigned int mxChnNo;
    BTC2_ChannelHandle hTc2Chn[MX_TC2_CHANNELS];
} BTC2_P_3461_Handle;

typedef struct BTC2_P_3461_ChannelHandle            *BTC2_3461_ChannelHandle;
typedef struct BTC2_P_3461_ChannelHandle
{
    uint32_t magicId;                   /* Used to check if structure is corrupt */
    BTC2_Handle hTc2;
    unsigned int chnNo;
    BHAB_DevId devId;
    BHAB_Handle hHab;
    BTC2_CallbackFunc pCallback[BTC2_Callback_eLast];
    void *pCallbackParam[BTC2_Callback_eLast];
    BTC2_ChannelSettings settings;
    bool isLock;                        /* current lock status */
    BKNI_MutexHandle mutex;             /* mutex to protect lock status*/
    BHAB_InterruptType event;
    bool bPowerdown;
    bool bPowerOffToOn;
    BTC2_Version verInfo;
    BTC2_InbandParam previousAcquireParams;   
    uint8_t readBuf[HAB_MEM_SIZE];
} BTC2_P_3461_ChannelHandle;

/*******************************************************************************
*
*   Private Module Functions
*
*******************************************************************************/
BERR_Code BTC2_3461_P_EventCallback_isr(
    void * pParam1, int param2
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_ChannelHandle hChn = (BTC2_ChannelHandle) pParam1;
    BTC2_3461_ChannelHandle hImplChnDev;
    BHAB_InterruptType event = (BHAB_InterruptType) param2;

    BDBG_ENTER(BTC2_3461_ProcessNotification);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab);

    switch (event) {
        case BHAB_Interrupt_eLockChange:
            {
                if( hImplChnDev->pCallback[BTC2_Callback_eLockChange] != NULL )
                {
                    (hImplChnDev->pCallback[BTC2_Callback_eLockChange])(hImplChnDev->pCallbackParam[BTC2_Callback_eLockChange] );
                }
            }
            break;
        case BHAB_Interrupt_eUpdateGain:
            {
                if( hImplChnDev->pCallback[BTC2_Callback_eUpdateGain] != NULL )
                {
                    (hImplChnDev->pCallback[BTC2_Callback_eUpdateGain])(hImplChnDev->pCallbackParam[BTC2_Callback_eUpdateGain] );
                }
            }
            break;
        case BHAB_Interrupt_eNoSignal:
            {
                BDBG_WRN(("BTC2 No Signal"));
                if( hImplChnDev->pCallback[BTC2_Callback_eNoSignal] != NULL )
                {
                    (hImplChnDev->pCallback[BTC2_Callback_eNoSignal])(hImplChnDev->pCallbackParam[BTC2_Callback_eNoSignal] );
                }
            }
            break;
        case BHAB_Interrupt_eTc2AsyncStatusReady:
            {
                if( hImplChnDev->pCallback[BTC2_Callback_eAsyncStatusReady] != NULL )
                {
                    (hImplChnDev->pCallback[BTC2_Callback_eAsyncStatusReady])(hImplChnDev->pCallbackParam[BTC2_Callback_eAsyncStatusReady] );
                }
            }
            break;
        default:
            BDBG_WRN((" unknown event code from 3461"));
            break;
    }

    BDBG_LEAVE(BTC2_3461_P_EventCallback_isr);
    return retCode;
}

BERR_Code BTC2_3461_P_GetL1PreStatus(
    BTC2_ChannelHandle hChn,            /* [in] Device channel handle */
    BTC2_Status *pStatus                /* [out] Returns status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[33] = HAB_MSG_HDR(BTC2_eL1PreStatus, 0, BTC2_CORE_TYPE, BTC2_CORE_ID);    
    
    BDBG_ENTER(BTC2_3461_P_GetL1PreStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));

    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, hImplChnDev->readBuf, 33, false, true, 33));
    pStatus->l1PreStatus.streamType = hImplChnDev->readBuf[4];
    pStatus->l1PreStatus.bwtExt= hImplChnDev->readBuf[5] & 0x1;
    pStatus->l1PreStatus.s1= (hImplChnDev->readBuf[5] & 0xE) >> 1;
    pStatus->l1PreStatus.s2= (hImplChnDev->readBuf[5] & 0xF0) >> 4;
    pStatus->l1PreStatus.l1RepetitionFlag = hImplChnDev->readBuf[6] & 0x1;
    pStatus->l1PreStatus.guardInterval = (hImplChnDev->readBuf[6] & 0xE) >> 1;
    pStatus->l1PreStatus.papr = (hImplChnDev->readBuf[6] & 0xF0) >> 4;
    pStatus->l1PreStatus.l1Mod = hImplChnDev->readBuf[7] & 0xF;
    pStatus->l1PreStatus.l1Cod = (hImplChnDev->readBuf[7] & 0x30) >> 4;
    pStatus->l1PreStatus.l1FecType = (hImplChnDev->readBuf[7] & 0xC0) >> 6;
    pStatus->l1PreStatus.pilotPattern = hImplChnDev->readBuf[8] & 0xF;
    pStatus->l1PreStatus.regenFlag = (hImplChnDev->readBuf[8] & 0x70) >> 4;
    pStatus->l1PreStatus.l1PostExt = (hImplChnDev->readBuf[8] & 0x80) >> 6;
    pStatus->l1PreStatus.numRf= hImplChnDev->readBuf[9] & 0x7;
    pStatus->l1PreStatus.currentRfIndex = (hImplChnDev->readBuf[9] & 0x70) >> 4;
    pStatus->l1PreStatus.txIdAvailability = hImplChnDev->readBuf[10];
    pStatus->l1PreStatus.numT2Frames = hImplChnDev->readBuf[11];
    pStatus->l1PreStatus.numDataSymbols = (hImplChnDev->readBuf[12] << 8) | hImplChnDev->readBuf[13];
    pStatus->l1PreStatus.cellId = (hImplChnDev->readBuf[14] << 8) | hImplChnDev->readBuf[15];
    pStatus->l1PreStatus.networkId = (hImplChnDev->readBuf[16] << 8) | hImplChnDev->readBuf[17];
    pStatus->l1PreStatus.t2SystemId = (hImplChnDev->readBuf[18] << 8) | hImplChnDev->readBuf[19];
    pStatus->l1PreStatus.l1PostSize = (hImplChnDev->readBuf[24] << 24) | (hImplChnDev->readBuf[25] << 16) | (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27];
    pStatus->l1PreStatus.l1PostInfoSize = (hImplChnDev->readBuf[28] << 24) | (hImplChnDev->readBuf[29] << 16) | (hImplChnDev->readBuf[30] << 8) | hImplChnDev->readBuf[31];	

done:
    BDBG_LEAVE(BTC2_3461_P_GetL1PreStatus);
    return retCode;
}

BERR_Code BTC2_3461_P_GetL1PostConfigurableStatus(
    BTC2_ChannelHandle hChn,            /* [in] Device channel handle */
    BTC2_Status *pStatus                /* [out] Returns status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[52] = HAB_MSG_HDR(BTC2_eL1PostConfigurableStatus, 0, BTC2_CORE_TYPE, BTC2_CORE_ID);    
    
    BDBG_ENTER(BTC2_3461_P_GetL1PostConfigurableStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    
    BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));
    
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, hImplChnDev->readBuf, 53, false, true, 53));
    pStatus->l1PostCfgStatus.numPlp = hImplChnDev->readBuf[4];
    pStatus->l1PostCfgStatus.numAux = hImplChnDev->readBuf[5] & 0xF;
    pStatus->l1PostCfgStatus.auxConfigRFu = hImplChnDev->readBuf[6];
    pStatus->l1PostCfgStatus.rfIdx = hImplChnDev->readBuf[7] & 0x7;
    pStatus->l1PostCfgStatus.fefType = (hImplChnDev->readBuf[7] & 0xF0) >> 4;
    pStatus->l1PostCfgStatus.fefInterval = hImplChnDev->readBuf[8];
    pStatus->l1PostCfgStatus.subSlicesPerFrame = (hImplChnDev->readBuf[10] << 8) | hImplChnDev->readBuf[11];
    pStatus->l1PostCfgStatus.frequency= (hImplChnDev->readBuf[12] << 24) | (hImplChnDev->readBuf[13] << 16) | (hImplChnDev->readBuf[14] << 8) | hImplChnDev->readBuf[15];
    pStatus->l1PostCfgStatus.fefLength = (hImplChnDev->readBuf[16] << 24) | (hImplChnDev->readBuf[17] << 16) | (hImplChnDev->readBuf[18] << 8) | hImplChnDev->readBuf[19];
    pStatus->l1PostCfgStatus.plpA.plpId = hImplChnDev->readBuf[20];
    pStatus->l1PostCfgStatus.plpA.plpGroupId = hImplChnDev->readBuf[21];
    pStatus->l1PostCfgStatus.plpA.plpType = hImplChnDev->readBuf[22] & 0x7;
    pStatus->l1PostCfgStatus.plpA.plpPayloadType = (hImplChnDev->readBuf[22] & 0xF8) >> 3;
    pStatus->l1PostCfgStatus.plpA.ffFlag= hImplChnDev->readBuf[23] & 0x1;
    pStatus->l1PostCfgStatus.plpA.firstRfIdx = (hImplChnDev->readBuf[23] & 0xE) >> 1;
    pStatus->l1PostCfgStatus.plpA.plpFecType = (hImplChnDev->readBuf[23] & 0x30) >> 4;
    pStatus->l1PostCfgStatus.plpA.timeIlType = (hImplChnDev->readBuf[23] & 0x40) >> 6;
    pStatus->l1PostCfgStatus.plpA.inBandFlag = (hImplChnDev->readBuf[23] & 0x80) >> 7;
    pStatus->l1PostCfgStatus.plpA.plpCodeRate = hImplChnDev->readBuf[24] & 0x7;
    pStatus->l1PostCfgStatus.plpA.plpMod = (hImplChnDev->readBuf[24] & 0x38) >> 3;
    pStatus->l1PostCfgStatus.plpA.plpRotation = (hImplChnDev->readBuf[24] & 0x40) >> 6;
    pStatus->l1PostCfgStatus.plpA.firstFrameIdx = hImplChnDev->readBuf[25];
    pStatus->l1PostCfgStatus.plpA.frameInterval = hImplChnDev->readBuf[26];
    pStatus->l1PostCfgStatus.plpA.timeIlLength = hImplChnDev->readBuf[27];
    pStatus->l1PostCfgStatus.plpA.plpNumBlocksMax = (hImplChnDev->readBuf[28] << 8) | hImplChnDev->readBuf[29];
    pStatus->l1PostCfgStatus.plpB.plpId = hImplChnDev->readBuf[32];
    pStatus->l1PostCfgStatus.plpB.plpGroupId = hImplChnDev->readBuf[33];
    pStatus->l1PostCfgStatus.plpB.plpType = hImplChnDev->readBuf[34] & 0x7;
    pStatus->l1PostCfgStatus.plpB.plpPayloadType = (hImplChnDev->readBuf[34] & 0xF8) >> 3;
    pStatus->l1PostCfgStatus.plpB.ffFlag= hImplChnDev->readBuf[35] & 0x1;
    pStatus->l1PostCfgStatus.plpB.firstRfIdx = (hImplChnDev->readBuf[35] & 0xE) >> 1;
    pStatus->l1PostCfgStatus.plpB.plpFecType = (hImplChnDev->readBuf[35] & 0x30) >> 4;
    pStatus->l1PostCfgStatus.plpB.timeIlType = (hImplChnDev->readBuf[35] & 0x40) >> 6;
    pStatus->l1PostCfgStatus.plpB.inBandFlag = (hImplChnDev->readBuf[35] & 0x80) >> 7;
    pStatus->l1PostCfgStatus.plpB.plpCodeRate = hImplChnDev->readBuf[36] & 0x7;
    pStatus->l1PostCfgStatus.plpB.plpMod = (hImplChnDev->readBuf[36] & 0x38) >> 3;
    pStatus->l1PostCfgStatus.plpB.plpRotation = (hImplChnDev->readBuf[36] & 0x40) >> 6;
    pStatus->l1PostCfgStatus.plpB.firstFrameIdx = hImplChnDev->readBuf[37];
    pStatus->l1PostCfgStatus.plpB.frameInterval = hImplChnDev->readBuf[38];
    pStatus->l1PostCfgStatus.plpB.timeIlLength = hImplChnDev->readBuf[39];
    pStatus->l1PostCfgStatus.plpB.plpNumBlocksMax = (hImplChnDev->readBuf[40] << 8) | hImplChnDev->readBuf[41];
    pStatus->l1PostCfgStatus.auxStreamType = (hImplChnDev->readBuf[48] & 0xF0) >> 4;
    pStatus->l1PostCfgStatus.auxPrivateConf = ((hImplChnDev->readBuf[48] & 0xF) << 24) | (hImplChnDev->readBuf[49] << 16) | (hImplChnDev->readBuf[50] << 8) | hImplChnDev->readBuf[51];

done:
    BDBG_LEAVE(BTC2_3461_P_GetL1PostConfigurableStatus);
    return retCode;
}

BERR_Code BTC2_3461_P_GetL1PostDynamicStatus(
    BTC2_ChannelHandle hChn,            /* [in] Device channel handle */
    BTC2_Status *pStatus                /* [out] Returns status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[45] = HAB_MSG_HDR(BTC2_eL1PostDynamicStatus, 0, BTC2_CORE_TYPE, BTC2_CORE_ID);    
    
    BDBG_ENTER(BTC2_3461_P_GetL1PostDynamicStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    
    BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));
    
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, hImplChnDev->readBuf, 45, false, true, 45));
    pStatus->l1PostDynamicStatus.frameIdx = hImplChnDev->readBuf[4];
    pStatus->l1PostDynamicStatus.l1ChanlgeCounter = hImplChnDev->readBuf[5];
    pStatus->l1PostDynamicStatus.startRfIdx = hImplChnDev->readBuf[6] & 0x7;
    pStatus->l1PostDynamicStatus.subSliceInterval = (hImplChnDev->readBuf[12]<< 24) | (hImplChnDev->readBuf[13] << 16) | (hImplChnDev->readBuf[14] << 8) | hImplChnDev->readBuf[15];
    pStatus->l1PostDynamicStatus.type2Start = (hImplChnDev->readBuf[16]<< 24) | (hImplChnDev->readBuf[17] << 16) | (hImplChnDev->readBuf[18] << 8) | hImplChnDev->readBuf[19];
    pStatus->l1PostDynamicStatus.plpA.plpId = hImplChnDev->readBuf[20];
    pStatus->l1PostDynamicStatus.plpA.plpNumBlocks = (hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23];
    pStatus->l1PostDynamicStatus.plpA.plpStart = (hImplChnDev->readBuf[24]<< 24) | (hImplChnDev->readBuf[25] << 16) | (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27];
    pStatus->l1PostDynamicStatus.plpB.plpId = hImplChnDev->readBuf[28];
    pStatus->l1PostDynamicStatus.plpB.plpNumBlocks = (hImplChnDev->readBuf[30] << 8) | hImplChnDev->readBuf[31];
    pStatus->l1PostDynamicStatus.plpB.plpStart = (hImplChnDev->readBuf[32]<< 24) | (hImplChnDev->readBuf[33] << 16) | (hImplChnDev->readBuf[34] << 8) | hImplChnDev->readBuf[35];
    pStatus->l1PostDynamicStatus.auxPrivateDyn_47_32 = (hImplChnDev->readBuf[36] << 8) | hImplChnDev->readBuf[37];
    pStatus->l1PostDynamicStatus.auxPrivateDyn_31_0 = (hImplChnDev->readBuf[38]<< 24) | (hImplChnDev->readBuf[39] << 16) | (hImplChnDev->readBuf[40] << 8) | hImplChnDev->readBuf[41];

done:
    BDBG_LEAVE(BTC2_3461_P_GetL1PostDynamicStatus);
    return retCode;
}

BERR_Code BTC2_3461_P_GetL1PlpStatus(
    BTC2_ChannelHandle hChn,            /* [in] Device channel handle */
    BTC2_Status *pStatus                /* [out] Returns status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[BTC2_L1PLP_BUF_MAX] = HAB_MSG_HDR(BTC2_eL1PlpStatus, 0x4, BTC2_CORE_TYPE, BTC2_CORE_ID);    
    uint8_t mp, i;
    uint16_t plpCount = 0;
	
    BDBG_ENTER(BTC2_3461_P_GetL1PlpStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    
    BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));
    
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 9, hImplChnDev->readBuf, BTC2_L1PLP_BUF_MAX, false, true, BTC2_L1PLP_BUF_MAX));
    mp = hImplChnDev->readBuf[4] & 0x80;  
    pStatus->l1PlpStatus.numPlp = hImplChnDev->readBuf[8];     

    if(pStatus->l1PlpStatus.numPlp)
    {
        for(i = 0; i< BTC2_L1PLP_PKT_0_PLP_COUNT; i++)        
        {
            pStatus->l1PlpStatus.plp[plpCount].plpId=  hImplChnDev->readBuf[12+i*4];
            pStatus->l1PlpStatus.plp[plpCount].plpGroupId =  hImplChnDev->readBuf[13+i*4];
            pStatus->l1PlpStatus.plp[plpCount].plpPayloadType =  (hImplChnDev->readBuf[14+i*4] & 0xF8) >> 3;		
            pStatus->l1PlpStatus.plp[plpCount].plpType =  hImplChnDev->readBuf[14+i*4] & 0x7;	

            plpCount++;
            if(plpCount == pStatus->l1PlpStatus.numPlp)
                break;
        }

        while(mp)
        {      
            CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 9, hImplChnDev->readBuf, BTC2_L1PLP_BUF_MAX, false, true, BTC2_L1PLP_BUF_MAX));
            mp = hImplChnDev->readBuf[4] & 0x80;
        
            for(i = 0; i< BTC2_L1PLP_PKT_1_N_PLP_COUNT; i++)
            {
                pStatus->l1PlpStatus.plp[plpCount].plpId =  hImplChnDev->readBuf[8+i*4];
                pStatus->l1PlpStatus.plp[plpCount].plpGroupId =	hImplChnDev->readBuf[9+i*4];
                pStatus->l1PlpStatus.plp[plpCount].plpPayloadType =	(hImplChnDev->readBuf[10+i*4] & 0xF8) >> 3; 	
                pStatus->l1PlpStatus.plp[plpCount].plpType =  hImplChnDev->readBuf[10+i*4] & 0x7;
           
                plpCount++;                
                if((plpCount == pStatus->l1PlpStatus.numPlp) || (plpCount > sizeof(pStatus->l1PlpStatus.plp)/sizeof(BTC2_Plp)))
                {
                    mp = 0;
                    break;	
                }
            }
        }        
    }
	
done:
    BDBG_LEAVE(BTC2_3461_P_GetL1PlpStatus);
    return retCode;
}

/*******************************************************************************
*
*   Public Module Functions
*
*******************************************************************************/
BERR_Code BTC2_3461_Open(
    BTC2_Handle *pTc2,                          /* [out] Returns handle */
    BCHP_Handle hChip,                          /* [in] Chip handle */
    BREG_Handle hRegister,                      /* [in] Register handle */
    BINT_Handle hInterrupt,                     /* [in] Interrupt handle */
    const struct BTC2_Settings *pDefSettings    /* [in] Default settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_Handle hDev;
    unsigned int chnIdx;
    BTC2_3461_Handle hImplDev = NULL;

    BDBG_ENTER(BTC2_3461_Open);

    /* Alloc memory from the system heap */
    hDev = (BTC2_Handle) BKNI_Malloc( sizeof( BTC2_P_Handle ) );
    if( hDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BTC2_Open: BKNI_malloc() failed"));
        goto done;
    }
    
    BKNI_Memset( hDev, 0x00, sizeof( BTC2_P_Handle ) );
    hDev->magicId = DEV_MAGIC_ID;
    hDev->settings = *pDefSettings;

    hImplDev = (BTC2_3461_Handle) BKNI_Malloc( sizeof( BTC2_P_3461_Handle ) );
    if( hImplDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BTC2_Open: BKNI_malloc() failed, impl"));
        goto done;
    }

    BKNI_Memset( hImplDev, 0x00, sizeof( BTC2_P_3461_Handle ) );
    hImplDev->magicId = DEV_MAGIC_ID;
    hImplDev->hChip = hChip;
    hImplDev->hRegister = hRegister;
    hImplDev->hInterrupt = hInterrupt;
    hImplDev->hHab = (BHAB_Handle) pDefSettings->hGeneric;    /* For this device, we need the HAB handle */
    hImplDev->devId = BHAB_DevId_eTC20; /* Here the device id is always defaulted to channel 0. */
    hImplDev->mxChnNo = MX_TC2_CHANNELS;

    for( chnIdx = 0; chnIdx < MX_TC2_CHANNELS; chnIdx++ )
    {
        hImplDev->hTc2Chn[chnIdx] = NULL;
    }
    hDev->pImpl = hImplDev;
    *pTc2 = hDev;

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
        *pTc2 = NULL;
    }
    BDBG_LEAVE(BTC2_3461_Open);
    return retCode;
}

BERR_Code BTC2_3461_Close(
    BTC2_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTC2_3461_Close);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    BKNI_Free( (void *) hDev->pImpl );
    hDev->magicId = 0x00;       /* clear it to catch improper use */
    BKNI_Free( (void *) hDev );

    BDBG_LEAVE(BTC2_3461_Close);
    return retCode;
}

BERR_Code BTC2_3461_Init(
    BTC2_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_Handle hImplDev;
    uint32_t familyId, chipId;
    uint16_t chipVer;
    BFEC_SystemVersionInfo  versionInfo;    
    
    BDBG_ENTER(BTC2_3461_Init);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BTC2_3461_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );
    BDBG_ASSERT( hImplDev->hHab );

    retCode = BHAB_GetApVersion(hImplDev->hHab, &familyId, &chipId, &chipVer, &hImplDev->verInfo.apVer, &hImplDev->verInfo.minApVer);
   
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
    
    hImplDev->verInfo.minVer = chipVer & 0xFF;

    if((hImplDev->verInfo.apVer >= 6) || ((hImplDev->verInfo.apVer >= 5) & ( hImplDev->verInfo.minApVer>= 3)))
    {
        BHAB_GetVersionInfo(hImplDev->hHab, &versionInfo);
        BDBG_WRN(("LEAP FW version is %d.%d.%d.%d", versionInfo.firmware.majorVersion, versionInfo.firmware.minorVersion, versionInfo.firmware.buildType, versionInfo.firmware.buildId));
    }
    else
        BDBG_WRN(("LEAP FW version is %d.%d", hImplDev->verInfo.apVer, hImplDev->verInfo.minApVer));     
  
    BDBG_LEAVE(BTC2_3461_Init);
done:    
    return retCode;
}

BERR_Code BTC2_3461_GetVersion(
    BTC2_Handle hDev,                   /* [in] Device handle */
    BTC2_Version *pVersion              /* [out] Returns version */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_Handle hImplDev;

    BDBG_ENTER(BTC2_3461_GetVersion);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BTC2_3461_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );

    *pVersion = hImplDev->verInfo;      /* use saved data */

    BDBG_LEAVE(BTC2_3461_GetVersion);
    return retCode;
}

BERR_Code BTC2_3461_GetVersionInfo(
    BTC2_Handle hDev,              /* [in] Device handle */
    BFEC_VersionInfo *pVersionInfo /* [out] Returns version Info */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[29] = HAB_MSG_HDR(BTC2_eGetVersionInfo, 0, BTC2_CORE_TYPE, BTC2_CORE_ID);    
    BTC2_3461_Handle hImplDev;

    BDBG_ENTER(BTC2_3461_GetVersionInfo);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BTC2_3461_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );

    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplDev->hHab, buf, 5, buf, 29, false, true, 29 ));
    pVersionInfo->majorVersion = (buf[4] << 8) | buf[5];
    pVersionInfo->minorVersion = (buf[6] << 8) | buf[7];
    pVersionInfo->buildType = (buf[8] << 8) | buf[9];
    pVersionInfo->buildId = (buf[10] << 8) | buf[11];

    BDBG_LEAVE(BTC2_3461_GetVersionInfo);
    
done:    
    return retCode;
}    
    
BERR_Code BTC2_3461_GetTotalChannels(
    BTC2_Handle hDev,                   /* [in] Device handle */
    unsigned int *totalChannels         /* [out] Returns total number downstream channels supported */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_Handle hImplDev;

    BDBG_ENTER(BTC2_3461_GetTotalChannels);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BTC2_3461_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );

    *totalChannels = hImplDev->mxChnNo; /* use saved data */

    BDBG_LEAVE(BTC2_3461_GetTotalChannels);
    return retCode;
}

BERR_Code BTC2_3461_GetChannelDefaultSettings(
    BTC2_Handle hDev,                       /* [in] Device handle */
    unsigned int channelNo,                 /* [in] Channel number to default setting for */
    BTC2_ChannelSettings *pChnDefSettings   /* [out] Returns channel default setting */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_Handle hImplDev;
    BSTD_UNUSED(channelNo);
    BSTD_UNUSED(pChnDefSettings);
    
    BDBG_ENTER(BTC2_3461_GetChannelDefaultSettings);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BTC2_3461_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );
    BDBG_ASSERT( hImplDev->hHab );

    BDBG_LEAVE(BTC2_3461_GetChannelDefaultSettings);
    return retCode;
}

BERR_Code BTC2_3461_OpenChannel(
    BTC2_Handle hDev,                                   /* [in] Device handle */
    BTC2_ChannelHandle *phChn,                          /* [out] Returns channel handle */
    unsigned int channelNo,                             /* [in] Channel number to open */
    const struct BTC2_ChannelSettings *pChnDefSettings  /* [in] Channel default setting */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_Handle hImplDev;
    BTC2_3461_ChannelHandle hImplChnDev = NULL;
    BTC2_ChannelHandle hChnDev;
    unsigned int event=0;

    BDBG_ENTER(BTC2_3461_OpenChannel);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BTC2_3461_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );
    BDBG_ASSERT( hImplDev->hHab);
   
    hChnDev = NULL;
    if( channelNo < hImplDev->mxChnNo )
    {
        if( hImplDev->hTc2Chn[channelNo] == NULL )
        {
            /* Alloc memory from the system heap */
            hChnDev = (BTC2_ChannelHandle) BKNI_Malloc( sizeof( BTC2_P_ChannelHandle ) );
            if( hChnDev == NULL )
            {
                retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                BDBG_ERR(("BTC2_OpenChannel: BKNI_malloc() failed"));
                goto done;
            }
            
            BKNI_Memset( hChnDev, 0x00, sizeof( BTC2_P_ChannelHandle ) );
            hChnDev->magicId = DEV_MAGIC_ID;
            hChnDev->hTc2 = hDev;

            hImplChnDev = (BTC2_3461_ChannelHandle) BKNI_Malloc( sizeof( BTC2_P_3461_ChannelHandle ) );
            if( hImplChnDev == NULL )
            {
                retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                BDBG_ERR(("BTC2_OpenChannel: BKNI_malloc() failed, impl"));
                goto done;
            }

            BKNI_Memset( hImplChnDev, 0x00, sizeof( BTC2_P_3461_ChannelHandle ) );
            
            hImplChnDev->chnNo = channelNo;
            hImplChnDev->devId = (BHAB_DevId) BHAB_DevId_eTC20 + channelNo;
            /* set the bandwidth to an invalid value so that the acquire parameters get sent */
            hImplChnDev->previousAcquireParams.bandwidth = BTC2_Bandwidth_eLast;

            BHAB_InstallInterruptCallback( hImplDev->hHab,  hImplChnDev->devId, BTC2_3461_P_EventCallback_isr , (void *)hChnDev, event);
            
            if (pChnDefSettings) hImplChnDev->settings = *pChnDefSettings;
            hImplChnDev->hHab = hImplDev->hHab;
            hImplChnDev->verInfo = hImplDev->verInfo;
            CHK_RETCODE(retCode, BKNI_CreateMutex(&hImplChnDev->mutex));
            hImplDev->hTc2Chn[channelNo] = hChnDev;
            hImplChnDev->bPowerdown = true;          
            hImplChnDev->bPowerOffToOn = false;          
            hChnDev->pImpl = hImplChnDev;

            *phChn = hChnDev;
        }
        else
        {
            retCode = BERR_TRACE(BTC2_ERR_NOTAVAIL_CHN_NO);
        }
    }
    else
    {
        retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
    }

done:
    if( retCode != BERR_SUCCESS )
    {
        if( hChnDev != NULL )
        {
            BKNI_Free( hChnDev );
            hImplDev->hTc2Chn[channelNo] = NULL;
        }
        if( hImplChnDev != NULL )
        {
            BKNI_Free( hImplChnDev );
        }
        *phChn = NULL;
    }
    BDBG_LEAVE(BTC2_3461_OpenChannel);
    return retCode;
}

BERR_Code BTC2_3461_CloseChannel(
    BTC2_ChannelHandle hChn             /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_Handle hImplDev;
    BTC2_3461_ChannelHandle hImplChnDev;
    BTC2_Handle hTc2;
    unsigned int chnNo;
    
    BDBG_ENTER(BTC2_3461_CloseChannel);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    hTc2 = hChn->hTc2;
    hImplDev = (BTC2_3461_Handle) hTc2->pImpl;
    BDBG_ASSERT( hImplDev );
    
    BHAB_UnInstallInterruptCallback(hImplChnDev->hHab, hImplChnDev->devId);

    BKNI_DestroyMutex(hImplChnDev->mutex);
    chnNo = 0;
    hChn->magicId = 0x00;       /* clear it to catch inproper use */
    BKNI_Free( hChn->pImpl );
    BKNI_Free( hChn );
    hImplDev->hTc2Chn[chnNo] = NULL;

    BDBG_LEAVE(BTC2_3461_CloseChannel);
    return retCode;
}

BERR_Code BTC2_3461_GetDevice(
    BTC2_ChannelHandle hChn,            /* [in] Device channel handle */
    BTC2_Handle *phDev                  /* [out] Returns Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTC2_3461_GetDevice);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    *phDev = hChn->hTc2;

    BDBG_LEAVE(BTC2_3461_GetDevice);
    return retCode;
}

BERR_Code BTC2_3461_SetAcquireParams(
    BTC2_ChannelHandle hChn ,           /* [in] Device channel handle */
    BTC2_InbandParam *ibParams          /* [in] Inband Parameters to use */
    )    
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[9] = HAB_MSG_HDR(BTC2_eAcquireParamsWrite, 0x4, BTC2_CORE_TYPE, BTC2_CORE_ID);

    BDBG_ENTER(BTC2_3461_SetAcquireParams);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    BKNI_AcquireMutex(hImplChnDev->mutex);
    hImplChnDev->isLock = false;
    BKNI_ReleaseMutex(hImplChnDev->mutex);

    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("TC2 core Powered Off"));
        retCode = BTC2_ERR_POWER_DOWN;
    }
    else
    {
        if(!(!hImplChnDev->bPowerOffToOn &&
            (hImplChnDev->previousAcquireParams.acqMode == ibParams->acqMode) &&
            (hImplChnDev->previousAcquireParams.bandwidth == ibParams->bandwidth) &&
            (hImplChnDev->previousAcquireParams.tuneAcquire == ibParams->tuneAcquire) &&
            (hImplChnDev->previousAcquireParams.plpId == ibParams->plpId) &&
            (hImplChnDev->previousAcquireParams.plpMode == ibParams->plpMode)))
        {        
            /* set Acquire Parameters */             
            switch(ibParams->acqMode)
            {
                case BTC2_AcquireMode_eAuto:
                    buf[4] = 0x1;
                    break;
                case BTC2_AcquireMode_eManual:
                    buf[4] = 0;
                    break;            
                case BTC2_AcquireMode_eScan:
                    buf[4] = 0x2;
                    break;             
                default:
                    buf[4] = 0x1;            
            }  

            buf[4] |= (0x1 << 2);
            buf[4] |= (0x1 << 3);
            buf[4] |= ibParams->plpMode << 4;
            buf[4] |= (ibParams->bandwidth << 5);
     
            if(!ibParams->plpMode)
                buf[5] = ibParams->plpId;
            
            buf[7] = (ibParams->tuneAcquire << 7);         
            CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 9, buf, 0, false, true, 9 ));
            hImplChnDev->previousAcquireParams = *ibParams;
            hImplChnDev->bPowerOffToOn = false;            
        }
    }

done:
    BDBG_LEAVE(BTC2_3461_SetAcquireParams);
    return retCode;
}

BERR_Code BTC2_3461_GetAcquireParams(
    BTC2_ChannelHandle hChn ,           /* [in] Device channel handle */
    BTC2_InbandParam *ibParams          /* [out] Inband Parameters to use */
    )    
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[9] = HAB_MSG_HDR(BTC2_eAcquireParamsRead, 0, BTC2_CORE_TYPE, BTC2_CORE_ID);

    BDBG_ENTER(BTC2_3461_GetAcquireParams);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    BKNI_AcquireMutex(hImplChnDev->mutex);
    hImplChnDev->isLock = false;
    BKNI_ReleaseMutex(hImplChnDev->mutex);

    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("TC2 core Powered Off"));
        retCode = BTC2_ERR_POWER_DOWN;
    }
    else
    {
        BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));

        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, hImplChnDev->readBuf, 9, false, true, 9 ));          
        switch(hImplChnDev->readBuf[4] & 0x3)
        {
            case 0:
                ibParams->acqMode = BTC2_AcquireMode_eManual;
                break;
            case 1:
                ibParams->acqMode = BTC2_AcquireMode_eAuto;
                break;            
            case 2:
                ibParams->acqMode = BTC2_AcquireMode_eScan;
                break;             
            default:
                ibParams->acqMode = BTC2_AcquireMode_eAuto;            
        }  
        
        ibParams->plpMode = (hImplChnDev->readBuf[4] & 0x10) >> 4;          
        ibParams->bandwidth = (BTC2_Bandwidth)(hImplChnDev->readBuf[4] & 0xE0) >> 5;
        if(!ibParams->plpMode)
           ibParams->plpId = hImplChnDev->readBuf[5];
        ibParams->tuneAcquire = (hImplChnDev->readBuf[7] & 0x80) >> 7;
    }
 
done:
    BDBG_LEAVE(BTC2_3461_GetAcquireParams);
    return retCode;
}
    
BERR_Code BTC2_3461_Acquire(
    BTC2_ChannelHandle hChn,            /* [in] Device channel handle */
    BTC2_InbandParam *ibParams           /* [in] Inband Parameters to use */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[5] = HAB_MSG_HDR(BTC2_eAcquire, 0, BTC2_CORE_TYPE, BTC2_CORE_ID);

    BDBG_ENTER(BTC2_3461_Acquire);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    BSTD_UNUSED(ibParams);

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    BKNI_AcquireMutex(hImplChnDev->mutex);
    hImplChnDev->isLock = false;
    BKNI_ReleaseMutex(hImplChnDev->mutex);

    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("TC2 core Powered Off"));
        retCode = BTC2_ERR_POWER_DOWN;
    }
    else
    {  
        /* Acquire */
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, buf, 0, false, true, 5 ));
        CHK_RETCODE(retCode, BHAB_EnableLockInterrupt(hImplChnDev->hHab, hImplChnDev->devId, true));
    }

done:
    BDBG_LEAVE(BTC2_3461_Acquire);
    return retCode;
}


BERR_Code BTC2_3461_GetStatus(
    BTC2_ChannelHandle hChn,            /* [in] Device channel handle */
    BTC2_Status *pStatus                /* [out] Returns status */
    )
{
    BSTD_UNUSED(hChn);
    BSTD_UNUSED(pStatus);
    
    BDBG_ERR(("BTC2_GetStatus not supported on this platform, please use BTC2_GetAsyncStatus"));
    
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}


BERR_Code BTC2_3461_RequestAsyncStatus(
    BTC2_ChannelHandle hChn            /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[5] = HAB_MSG_HDR(BTC2_eRequestAsyncStatus, 0, BTC2_CORE_TYPE, BTC2_CORE_ID);
    
    BDBG_ENTER(BTC2_3461_RequestAsyncStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
 
    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("TC2 core Powered Off"));    
        retCode = BTC2_ERR_POWER_DOWN;
    }
    else
    {
        if(hImplChnDev->verInfo.apVer >= 4)
        {
            BDBG_ERR(("BTC2_3461_RequestAsyncStatus is not supported on version %d.%d Please use Selective Status API", hImplChnDev->verInfo.apVer, hImplChnDev->verInfo.minApVer));
            retCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        else
            CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, buf, 0, false, true, 5));
    }

done:
    BDBG_LEAVE(BTC2_3461_RequestAsyncStatus);
    return retCode;
}


BERR_Code BTC2_3461_GetAsyncStatus(
    BTC2_ChannelHandle hChn,            /* [in] Device channel handle */
    BTC2_Status *pStatus                /* [out] Returns status */   
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[10] = HAB_MSG_HDR(BTC2_eGetAsyncStatus, 0x4, BTC2_CORE_TYPE, BTC2_CORE_ID);
    uint8_t buf1[10] = HAB_MSG_HDR(BTC2_eGetAsyncStatus, 0x4, BTC2_CORE_TYPE, BTC2_CORE_ID);
    int16_t signalStrength;    
    
    BDBG_ENTER(BTC2_3461_GetAsyncStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("TC2 core Powered Off"));    
        retCode = BTC2_ERR_POWER_DOWN;
    }
    else
    {
        if(hImplChnDev->verInfo.apVer >= 4)
        {
            BDBG_ERR(("BTC2_3461_GetAsyncStatus is not supported on version %d.%d Please use Selective Status API", hImplChnDev->verInfo.apVer, hImplChnDev->verInfo.minApVer));
            retCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        else
        {
            BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));   

            CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 9, hImplChnDev->readBuf, 127, false, true, 127));
           
            pStatus->acquisitionMode = hImplChnDev->readBuf[8] & 0x1;
            pStatus->bandwidth = (hImplChnDev->readBuf[8] & 0x1c) >> 2;     
            pStatus->lock = hImplChnDev->readBuf[9] & 0x1;
            pStatus->isSpectrumInverted = (hImplChnDev->readBuf[9] & 0x8) >> 3;
            pStatus->reacqCount = ((hImplChnDev->readBuf[12] << 8) | hImplChnDev->readBuf[13]);
            pStatus->snrEstimate = ((hImplChnDev->readBuf[16] << 8) | hImplChnDev->readBuf[17]);
            pStatus->gainOffset = ((hImplChnDev->readBuf[18] << 8) | hImplChnDev->readBuf[19]);
            pStatus->carrierFreqOffset = ((hImplChnDev->readBuf[20] << 24) | (hImplChnDev->readBuf[21] << 16) | (hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23]);
            pStatus->timingOffset = ((hImplChnDev->readBuf[24] << 24) | (hImplChnDev->readBuf[25] << 16) | (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27]);
            pStatus->l1PreStatistics.lock = hImplChnDev->readBuf[28] & 0x1;
            pStatus->l1PreStatistics.snrData = ((hImplChnDev->readBuf[30] << 8) | hImplChnDev->readBuf[31]);
            pStatus->l1PreStatistics.ldpcAvgIter = ((hImplChnDev->readBuf[34] << 8) | hImplChnDev->readBuf[35]);
            pStatus->l1PreStatistics.ldpcTotIter = ((hImplChnDev->readBuf[36] << 24) | (hImplChnDev->readBuf[37] << 16) | (hImplChnDev->readBuf[38] << 8) | hImplChnDev->readBuf[39]);
            pStatus->l1PreStatistics.ldpcTotFrm = ((hImplChnDev->readBuf[40] << 24) | (hImplChnDev->readBuf[41] << 16) | (hImplChnDev->readBuf[42] << 8) | hImplChnDev->readBuf[43]);
            pStatus->l1PreStatistics.ldpcUncFrm = ((hImplChnDev->readBuf[44] << 24) | (hImplChnDev->readBuf[45] << 16) | (hImplChnDev->readBuf[46] << 8) | hImplChnDev->readBuf[47]);
            pStatus->l1PreStatistics.ldpcBER = ((hImplChnDev->readBuf[48] << 24) | (hImplChnDev->readBuf[49] << 16) | (hImplChnDev->readBuf[50] << 8) | hImplChnDev->readBuf[51]);
            pStatus->l1PreStatistics.bchCorBit = ((hImplChnDev->readBuf[52] << 24) | (hImplChnDev->readBuf[53] << 16) | (hImplChnDev->readBuf[54] << 8) | hImplChnDev->readBuf[55]);
            pStatus->l1PreStatistics.bchTotBlk = ((hImplChnDev->readBuf[56] << 24) | (hImplChnDev->readBuf[57] << 16) | (hImplChnDev->readBuf[58] << 8) | hImplChnDev->readBuf[59]);
            pStatus->l1PreStatistics.bchClnBlk = ((hImplChnDev->readBuf[60] << 24) | (hImplChnDev->readBuf[61] << 16) | (hImplChnDev->readBuf[62] << 8) | hImplChnDev->readBuf[63]);
            pStatus->l1PreStatistics.bchCorBlk = ((hImplChnDev->readBuf[64] << 24) | (hImplChnDev->readBuf[65] << 16) | (hImplChnDev->readBuf[66] << 8) | hImplChnDev->readBuf[67]);
            pStatus->l1PreStatistics.bchUncBlk = ((hImplChnDev->readBuf[68] << 24) | (hImplChnDev->readBuf[69] << 16) | (hImplChnDev->readBuf[70] << 8) | hImplChnDev->readBuf[71]);
            pStatus->l1PostStatistics.lock = hImplChnDev->readBuf[72] & 0x1;
            pStatus->l1PostStatistics.snrData = ((hImplChnDev->readBuf[74] << 8) | hImplChnDev->readBuf[75]);
            pStatus->l1PostStatistics.ldpcAvgIter = ((hImplChnDev->readBuf[78] << 8) | hImplChnDev->readBuf[79]);
            pStatus->l1PostStatistics.ldpcTotIter = ((hImplChnDev->readBuf[80] << 24) | (hImplChnDev->readBuf[81] << 16) | (hImplChnDev->readBuf[82] << 8) | hImplChnDev->readBuf[83]);
            pStatus->l1PostStatistics.ldpcTotFrm = ((hImplChnDev->readBuf[84] << 24) | (hImplChnDev->readBuf[85] << 16) | (hImplChnDev->readBuf[86] << 8) | hImplChnDev->readBuf[87]);
            pStatus->l1PostStatistics.ldpcUncFrm = ((hImplChnDev->readBuf[88] << 24) | (hImplChnDev->readBuf[89] << 16) | (hImplChnDev->readBuf[90] << 8) | hImplChnDev->readBuf[91]);
            pStatus->l1PostStatistics.ldpcBER = ((hImplChnDev->readBuf[92] << 24) | (hImplChnDev->readBuf[93] << 16) | (hImplChnDev->readBuf[94] << 8) | hImplChnDev->readBuf[95]);
            pStatus->l1PostStatistics.bchCorBit = ((hImplChnDev->readBuf[96] << 24) | (hImplChnDev->readBuf[97] << 16) | (hImplChnDev->readBuf[98] << 8) | hImplChnDev->readBuf[99]);
            pStatus->l1PostStatistics.bchTotBlk = ((hImplChnDev->readBuf[100] << 24) | (hImplChnDev->readBuf[101] << 16) | (hImplChnDev->readBuf[102] << 8) | hImplChnDev->readBuf[103]);
            pStatus->l1PostStatistics.bchClnBlk = ((hImplChnDev->readBuf[104] << 24) | (hImplChnDev->readBuf[105] << 16) | (hImplChnDev->readBuf[106] << 8) | hImplChnDev->readBuf[107]);
            pStatus->l1PostStatistics.bchCorBlk = ((hImplChnDev->readBuf[108] << 24) | (hImplChnDev->readBuf[109] << 16) | (hImplChnDev->readBuf[110] << 8) | hImplChnDev->readBuf[111]);
            pStatus->l1PostStatistics.bchUncBlk = ((hImplChnDev->readBuf[112] << 24) | (hImplChnDev->readBuf[113] << 16) | (hImplChnDev->readBuf[114] << 8) | hImplChnDev->readBuf[115]);    

            buf1[4] = 0;
            buf1[5] = 1;
            buf1[6] = 0;
            buf1[7] = 0;
            buf1[8] = 0;
            buf1[9] = 0;

            CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf1, 9, hImplChnDev->readBuf, 127, false, true, 127));
            pStatus->plpAStatistics.lock = hImplChnDev->readBuf[8] & 0x1;
            pStatus->plpAStatistics.snrData = ((hImplChnDev->readBuf[10] << 8) | hImplChnDev->readBuf[11]);
            pStatus->plpAStatistics.ldpcAvgIter = ((hImplChnDev->readBuf[14] << 8) | hImplChnDev->readBuf[15]);
            pStatus->plpAStatistics.ldpcTotIter = ((hImplChnDev->readBuf[16] << 24) | (hImplChnDev->readBuf[17] << 16) | (hImplChnDev->readBuf[18] << 8) | hImplChnDev->readBuf[19]);
            pStatus->plpAStatistics.ldpcTotFrm = ((hImplChnDev->readBuf[20] << 24) | (hImplChnDev->readBuf[21] << 16) | (hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23]);
            pStatus->plpAStatistics.ldpcUncFrm = ((hImplChnDev->readBuf[24] << 24) | (hImplChnDev->readBuf[25] << 16) | (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27]);
            pStatus->plpAStatistics.ldpcBER = ((hImplChnDev->readBuf[28] << 24) | (hImplChnDev->readBuf[29] << 16) | (hImplChnDev->readBuf[30] << 8) | hImplChnDev->readBuf[31]);
            pStatus->plpAStatistics.bchCorBit = ((hImplChnDev->readBuf[32] << 24) | (hImplChnDev->readBuf[33] << 16) | (hImplChnDev->readBuf[34] << 8) | hImplChnDev->readBuf[35]);
            pStatus->plpAStatistics.bchTotBlk = ((hImplChnDev->readBuf[36] << 24) | (hImplChnDev->readBuf[37] << 16) | (hImplChnDev->readBuf[38] << 8) | hImplChnDev->readBuf[39]);
            pStatus->plpAStatistics.bchClnBlk = ((hImplChnDev->readBuf[40] << 24) | (hImplChnDev->readBuf[41] << 16) | (hImplChnDev->readBuf[42] << 8) | hImplChnDev->readBuf[43]);
            pStatus->plpAStatistics.bchCorBlk = ((hImplChnDev->readBuf[44] << 24) | (hImplChnDev->readBuf[45] << 16) | (hImplChnDev->readBuf[46] << 8) | hImplChnDev->readBuf[47]);
            pStatus->plpAStatistics.bchUncBlk = ((hImplChnDev->readBuf[48] << 24) | (hImplChnDev->readBuf[49] << 16) | (hImplChnDev->readBuf[50] << 8) | hImplChnDev->readBuf[51]);
            pStatus->plpBStatistics.lock = hImplChnDev->readBuf[52] & 0x1;
            pStatus->plpBStatistics.snrData = ((hImplChnDev->readBuf[54] << 8) | hImplChnDev->readBuf[55]);
            pStatus->plpBStatistics.ldpcAvgIter = ((hImplChnDev->readBuf[58] << 8) | hImplChnDev->readBuf[59]);
            pStatus->plpBStatistics.ldpcTotIter = ((hImplChnDev->readBuf[60] << 24) | (hImplChnDev->readBuf[61] << 16) | (hImplChnDev->readBuf[62] << 8) | hImplChnDev->readBuf[63]);
            pStatus->plpBStatistics.ldpcTotFrm = ((hImplChnDev->readBuf[64] << 24) | (hImplChnDev->readBuf[65] << 16) | (hImplChnDev->readBuf[66] << 8) | hImplChnDev->readBuf[67]);
            pStatus->plpBStatistics.ldpcUncFrm = ((hImplChnDev->readBuf[68] << 24) | (hImplChnDev->readBuf[69] << 16) | (hImplChnDev->readBuf[70] << 8) | hImplChnDev->readBuf[71]);
            pStatus->plpBStatistics.ldpcBER = ((hImplChnDev->readBuf[72] << 24) | (hImplChnDev->readBuf[73] << 16) | (hImplChnDev->readBuf[74] << 8) | hImplChnDev->readBuf[75]);
            pStatus->plpBStatistics.bchCorBit = ((hImplChnDev->readBuf[76] << 24) | (hImplChnDev->readBuf[77] << 16) | (hImplChnDev->readBuf[78] << 8) | hImplChnDev->readBuf[79]);
            pStatus->plpBStatistics.bchTotBlk = ((hImplChnDev->readBuf[80] << 24) | (hImplChnDev->readBuf[81] << 16) | (hImplChnDev->readBuf[82] << 8) | hImplChnDev->readBuf[83]);
            pStatus->plpBStatistics.bchClnBlk = ((hImplChnDev->readBuf[84] << 24) | (hImplChnDev->readBuf[85] << 16) | (hImplChnDev->readBuf[86] << 8) | hImplChnDev->readBuf[87]);
            pStatus->plpBStatistics.bchCorBlk = ((hImplChnDev->readBuf[88] << 24) | (hImplChnDev->readBuf[89] << 16) | (hImplChnDev->readBuf[90] << 8) | hImplChnDev->readBuf[91]);
            pStatus->plpBStatistics.bchUncBlk = ((hImplChnDev->readBuf[92] << 24) | (hImplChnDev->readBuf[93] << 16) | (hImplChnDev->readBuf[94] << 8) | hImplChnDev->readBuf[95]);    
            signalStrength = ((hImplChnDev->readBuf[96] << 8) | (hImplChnDev->readBuf[97] & 0xFF));
            pStatus->signalStrength = signalStrength*100/256 + 4875;          
            CHK_RETCODE(retCode, BTC2_3461_P_GetL1PreStatus(hChn, pStatus));       
            CHK_RETCODE(retCode, BTC2_3461_P_GetL1PostConfigurableStatus(hChn, pStatus));
            CHK_RETCODE(retCode, BTC2_3461_P_GetL1PostDynamicStatus(hChn, pStatus));
            CHK_RETCODE(retCode, BTC2_3461_P_GetL1PlpStatus(hChn, pStatus));
        }
    }

done:
    BDBG_LEAVE(BTC2_3461_GetAsyncStatus);
    return retCode;
}


BERR_Code BTC2_3461_GetLockStatus(
    BTC2_ChannelHandle hChn,            /* [in] Device channel handle */
    BTC2_LockStatus *pLockStatus         /* [out] Returns lock status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;    
    BTC2_3461_ChannelHandle hImplChnDev;   
    uint32_t sb;
#if (BTC2_3461_VER != BCHP_VER_A0)    
    uint8_t status;
#endif    

    BDBG_ENTER(BTC2_3461_GetLockStatus);   
    BDBG_ASSERT( hChn );    
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );  
	
    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;   
    BDBG_ASSERT( hImplChnDev );    
    BDBG_ASSERT( hImplChnDev->hHab );       
	
#if (BTC2_3461_VER == BCHP_VER_A0)
    CHK_RETCODE(retCode, BHAB_ReadRegister(hImplChnDev->hHab, BCHP_LEAP_CTRL_SPARE, &sb));
    *pLockStatus = (sb >> 15) & 0x1;
#else    
    CHK_RETCODE(retCode, BHAB_ReadRegister(hImplChnDev->hHab, BCHP_LEAP_CTRL_SW_SPARE1, &sb));
    status = (sb & 0xF0) >> 4;
    
    switch(status)
    {
        case 1:
            *pLockStatus = BTC2_LockStatus_eLocked;
            break;
        case 2:
            *pLockStatus = BTC2_LockStatus_eUnlocked;
            break;
        case 3:
            *pLockStatus = BTC2_LockStatus_eNoSignal;
            break;
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }    
#endif

done:    
    BDBG_LEAVE(BTC2_3461_GetLockStatus);        
    return retCode;

}


BERR_Code BTC2_3461_GetSoftDecision(
    BTC2_ChannelHandle hChn,            /* [in] Device channel handle */
    int16_t nbrToGet,                   /* [in] Number values to get */
    int16_t *iVal,                      /* [out] Ptr to array to store output I soft decision */
    int16_t *qVal,                      /* [out] Ptr to array to store output Q soft decision */
    int16_t *nbrGotten                  /* [out] Number of values gotten/read */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t i;
    uint8_t buf[125] = HAB_MSG_HDR(BTC2_eGetConstellation, 0x4, BTC2_CORE_TYPE, BTC2_CORE_ID);    

    BDBG_ENTER(BTC2_3461_GetSoftDecision);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("TC2 core Powered Off"));    
        retCode = BTC2_ERR_POWER_DOWN;
    }
    else
    {
        BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));
    
        buf[4] = 0x13;
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 9, hImplChnDev->readBuf, 125, false, true, 125));
        
        for (i = 0; i < 30; i++)
        {
            iVal[i] = (hImplChnDev->readBuf[4+(4*i)] << 8) | hImplChnDev->readBuf[5+(4*i)];
            qVal[i] = (hImplChnDev->readBuf[6+(4*i)] << 8) | hImplChnDev->readBuf[7+(4*i)];
        }

        *nbrGotten = nbrToGet;
    }
done:   


    BDBG_LEAVE(BTC2_3461_GetSoftDecision);
    return retCode;
}


BERR_Code BTC2_3461_EnablePowerSaver(
    BTC2_ChannelHandle hChn             /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[5] = HAB_MSG_HDR(BTC2_ePowerCtrlOff, 0x0, BTC2_CORE_TYPE, BTC2_CORE_ID);   

    BDBG_ENTER(BTC2_3461_EnablePowerSaver);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    
    if(!hImplChnDev->bPowerdown)
    {
        buf[4] = hImplChnDev->chnNo; 
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, buf, 0, false, true, 5));
        hImplChnDev->bPowerdown = true;
    }
    
done:
    BDBG_LEAVE(BTC2_3461_EnablePowerSaver);
    return retCode;
}

BERR_Code BTC2_3461_DisablePowerSaver(
    BTC2_ChannelHandle hChn             /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[5] = HAB_MSG_HDR(BTC2_ePowerCtrlOn, 0x0, BTC2_CORE_TYPE, BTC2_CORE_ID);   
    uint8_t configParams[9] = HAB_MSG_HDR(BTC2_eConfigParamsWrite, 0x4, BTC2_CORE_TYPE, BTC2_CORE_ID);    

    BDBG_ENTER(BTC2_3461_DisablePowerSaver);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
   
    if(hImplChnDev->bPowerdown)
    {
        buf[4] = hImplChnDev->chnNo;        
        hImplChnDev->bPowerOffToOn = true;        
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 5, buf, 0, false, true, 5));
        hImplChnDev->bPowerdown = false;
    }

    if(hImplChnDev->verInfo.apVer >= 5) {
            configParams[4] = BTC2_CONFIG_PARAMS_BUF1;    
            configParams[5] = BTC2_CONFIG_PARAMS_BUF2;
            configParams[6] = BTC2_CONFIG_PARAMS_BUF3;  
            CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, configParams, 9, hImplChnDev->readBuf, 0, false, true, 9));
        }
    
done:
    BDBG_LEAVE(BTC2_3461_DisablePowerSaver);
    return retCode;    
}

BERR_Code BTC2_3461_ProcessNotification(
    BTC2_ChannelHandle hChn,                /* [in] Device channel handle */
    unsigned int event                      /* [in] Event code and event data*/
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(hChn);
    BSTD_UNUSED(event);

    BDBG_WRN(("Not supported for this frontend chip."));
    return retCode;
}

BERR_Code BTC2_3461_InstallCallback(
    BTC2_ChannelHandle hChn,            /* [in] Device channel handle */
    BTC2_Callback callbackType,         /* [in] Type of callback */
    BTC2_CallbackFunc pCallback,        /* [in] Function Ptr to callback */
    void *pParam                        /* [in] Generic parameter send on callback */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;

    BDBG_ENTER(BTC2_3461_InstallCallback);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );

    switch( callbackType )
    {
        case BTC2_Callback_eLockChange:
            hImplChnDev->pCallback[callbackType] = pCallback;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;
        case BTC2_Callback_eUpdateGain:
            hImplChnDev->pCallback[callbackType] = pCallback;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;
        case BTC2_Callback_eNoSignal:
            hImplChnDev->pCallback[callbackType] = pCallback;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;
        case BTC2_Callback_eAsyncStatusReady:
            hImplChnDev->pCallback[callbackType] = pCallback;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }

    BDBG_LEAVE(BTC2_3461_InstallCallback);
    return retCode;
}


BERR_Code BTC2_3461_ResetStatus(
    BTC2_ChannelHandle hChn     /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[9] = HAB_MSG_HDR(BTC2_eResetSelectiveAsyncStatus, 0x4, BTC2_CORE_TYPE, BTC2_CORE_ID); 
    
    BDBG_ENTER(BTC2_3461_ResetStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("TC2 core Powered Off"));
        retCode = BTC2_ERR_POWER_DOWN;
    }
    else 
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 9, buf, 0, false, true, 9));

done:
    BDBG_LEAVE(BTC2_3461_ResetStatus);
    return retCode;
}


BERR_Code BTC2_3461_WriteSlave(
    BTC2_ChannelHandle hChn,     /* [in] Device channel handle */
    uint8_t chipAddr,            /* [in] chip addr of the i2c slave device */
    uint32_t subAddr,            /* [in] sub addr of the register to read from the slave device */
    uint8_t subAddrLen,          /* [in] how many bytes is the sub addr? one to four*/
    uint32_t *data,              /* [in] ptr to the data that we will write to the slave device */
    uint8_t dataLen              /* [in] how many bytes are we going to write? one to four*/
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(hChn);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(subAddr);
    BSTD_UNUSED(subAddrLen);
    BSTD_UNUSED(data);
    BSTD_UNUSED(dataLen);

    return( retCode );
}

BERR_Code BTC2_3461_ReadSlave(
    BTC2_ChannelHandle hChn,     /* [in] Device channel handle */
    uint8_t chipAddr,            /* [in] chip addr of the i2c slave device */
    uint32_t subAddr,            /* [in] sub addr of the register to read from the slave device */
    uint8_t subAddrLen,          /* [in] how many bytes is the sub addr? one to four*/
    uint32_t *data,              /* [out] ptr to the data that we will read from the slave device */
    uint8_t dataLen              /* [in] how many bytes are we going to read? one to four*/
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(hChn);
    BSTD_UNUSED(chipAddr);
    BSTD_UNUSED(subAddr);
    BSTD_UNUSED(subAddrLen);
    BSTD_UNUSED(data);
    BSTD_UNUSED(dataLen);

    return( retCode );
}


BERR_Code BTC2_3461_RequestSelectiveAsyncStatus(
    BTC2_ChannelHandle hChn,
    BTC2_SelectiveAsyncStatusType type
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[9] = HAB_MSG_HDR(BTC2_eRequestSelectiveAsyncStatus, 0x4, BTC2_CORE_TYPE, BTC2_CORE_ID);
    
    BDBG_ENTER(BTC2_3461_RequestSelectiveAsyncStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("TC2 core Powered Off"));    
        retCode = BTC2_ERR_POWER_DOWN;
    }
    else
    {
        switch(type)
        {
            case BTC2_SelectiveAsyncStatusType_eFecStatisticsL1Pre:
                buf[5] = 6;
                break;
            case BTC2_SelectiveAsyncStatusType_eFecStatisticsL1Post:
                buf[5] = 7;
                break;
            case BTC2_SelectiveAsyncStatusType_eFecStatisticsPlpA:
                buf[5] = 8;
                break;
            case BTC2_SelectiveAsyncStatusType_eFecStatisticsPlpB:
                buf[5] = 9;
                break;
            case BTC2_SelectiveAsyncStatusType_eL1Pre:
                buf[5] = 1;
                break;    
            case BTC2_SelectiveAsyncStatusType_eL1PostConfigurable:
                buf[5] = 2;
                break;        
            case BTC2_SelectiveAsyncStatusType_eL1PostDynamic:
                buf[5] = 3;
                break; 
            case BTC2_SelectiveAsyncStatusType_eL1Plp:
                buf[5] = 5;
                break; 
            case BTC2_SelectiveAsyncStatusType_eShort:
                buf[5] = 10;
                break;                 
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                break;
        }
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 9, buf, 0, false, true, 9));
    }
done:
    BDBG_LEAVE(BTC2_3461_RequestSelectiveAsyncStatus);
    return retCode;    
}

BERR_Code BTC2_3461_GetSelectiveAsyncStatusReadyType(
    BTC2_ChannelHandle hChn,
    BTC2_SelectiveAsyncStatusReady *ready
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[9] = HAB_MSG_HDR(BTC2_eGetSelectiveAsyncStatusReadyType, 0, BTC2_CORE_TYPE, BTC2_CORE_ID);
    uint16_t statusReady = 0;
    uint8_t i;
    
    BDBG_ENTER(BTC2_3461_GetSelectiveAsyncStatusReadyType);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    BKNI_Memset( ready, 0x00, sizeof( BTC2_SelectiveAsyncStatusReady ) );
    
    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("TC2 core Powered Off"));    
        retCode = BTC2_ERR_POWER_DOWN;
    }
    else
    {
        BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));
    
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
                    ready->l1Pre = 1;
                    break;
                case 2:
                    ready->l1PostConfigurable = 1;
                    break;
                case 3:
                    ready->l1PostDynamic = 1;
                    break;
                case 5:
                    ready->l1Plp = 1;
                    break;
                case 6:
                    ready->fecStatisticsL1Pre = 1;
                    break;    
                case 7:
                    ready->fecStatisticsL1Post = 1;
                    break;        
                case 8:
                    ready->fecStatisticsPlpA = 1;
                    break; 
                case 9:
                    ready->fecStatisticsPlpB = 1;
                    break; 
                case 10:
                    ready->shortStatus = 1;
                    break;                 
                default:
                    retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                    break;
            }           
        }
    }

done:
    BDBG_LEAVE(BTC2_3461_GetSelectiveAsyncStatusReadyType);
    return retCode;
}    

BERR_Code BTC2_3461_GetSelectiveAsyncStatus(
    BTC2_ChannelHandle hChn,
    BTC2_SelectiveAsyncStatusType type,
    BTC2_SelectiveStatus *pStatus   /* [out] */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTC2_3461_ChannelHandle hImplChnDev;
    uint8_t buf[13] = HAB_MSG_HDR(BTC2_eGetSelectiveAsyncStatus, 0x8, BTC2_CORE_TYPE, BTC2_CORE_ID);
    uint8_t mp, i;
    uint16_t plpCount = 0, read_len = 0;
    int16_t signalStrength;

    BDBG_ENTER(BTC2_3461_GetSelectiveAsyncStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BTC2_3461_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    if(hImplChnDev->bPowerdown)
    {
        BDBG_ERR(("TC2 core Powered Off"));    
        retCode = BTC2_ERR_POWER_DOWN;
    }
    else
    {
        BKNI_Memset(hImplChnDev->readBuf, 0, sizeof(hImplChnDev->readBuf));   

        switch(type)
        {
            case BTC2_SelectiveAsyncStatusType_eFecStatisticsL1Pre:
                buf[9] = 6;
                read_len = 57;               
                break;
            case BTC2_SelectiveAsyncStatusType_eFecStatisticsL1Post:
                buf[9] = 7;
                read_len = 57;                
                break;
            case BTC2_SelectiveAsyncStatusType_eFecStatisticsPlpA:
                buf[9] = 8;
                read_len = 57;                
                break;
            case BTC2_SelectiveAsyncStatusType_eFecStatisticsPlpB:
                buf[9] = 9;
                read_len = 57;                
                break;
            case BTC2_SelectiveAsyncStatusType_eL1Pre:
                buf[9] = 1;
                read_len = 41;                
                break;    
            case BTC2_SelectiveAsyncStatusType_eL1PostConfigurable:
                buf[9] = 2;
                read_len = 61;                
                break;        
            case BTC2_SelectiveAsyncStatusType_eL1PostDynamic:
                buf[9] = 3;
                read_len = 53;                
                break; 
            case BTC2_SelectiveAsyncStatusType_eL1Plp:
                buf[9] = 5;
                read_len = BTC2_L1PLP_BUF_MAX;
                break; 
            case BTC2_SelectiveAsyncStatusType_eShort:
                buf[9] = 10;
                read_len = 37;                
                break;                 
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                break;
        }
        
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 13, hImplChnDev->readBuf, read_len, false, true, read_len));
     
        switch(hImplChnDev->readBuf[9])
        {
            case 1:
                pStatus->type = BTC2_SelectiveAsyncStatusType_eL1Pre;
                pStatus->status.l1PreStatus.streamType = hImplChnDev->readBuf[12];
                pStatus->status.l1PreStatus.bwtExt= hImplChnDev->readBuf[13] & 0x1;
                pStatus->status.l1PreStatus.s1= (hImplChnDev->readBuf[13] & 0xE) >> 1;
                pStatus->status.l1PreStatus.s2= (hImplChnDev->readBuf[13] & 0xF0) >> 4;
                pStatus->status.l1PreStatus.l1RepetitionFlag = hImplChnDev->readBuf[14] & 0x1;
                pStatus->status.l1PreStatus.guardInterval = (hImplChnDev->readBuf[14] & 0xE) >> 1;
                pStatus->status.l1PreStatus.papr = (hImplChnDev->readBuf[14] & 0xF0) >> 4;
                pStatus->status.l1PreStatus.l1Mod = hImplChnDev->readBuf[15] & 0xF;
                pStatus->status.l1PreStatus.l1Cod = (hImplChnDev->readBuf[15] & 0x30) >> 4;
                pStatus->status.l1PreStatus.l1FecType = (hImplChnDev->readBuf[15] & 0xC0) >> 6;
                pStatus->status.l1PreStatus.pilotPattern = hImplChnDev->readBuf[16] & 0xF;
                pStatus->status.l1PreStatus.regenFlag = (hImplChnDev->readBuf[16] & 0x70) >> 4;
                pStatus->status.l1PreStatus.l1PostExt = (hImplChnDev->readBuf[16] & 0x80) >> 6;
                pStatus->status.l1PreStatus.numRf= hImplChnDev->readBuf[17] & 0x7;
                pStatus->status.l1PreStatus.currentRfIndex = (hImplChnDev->readBuf[17] & 0x70) >> 4;
                pStatus->status.l1PreStatus.txIdAvailability = hImplChnDev->readBuf[18];
                pStatus->status.l1PreStatus.numT2Frames = hImplChnDev->readBuf[19];
                pStatus->status.l1PreStatus.numDataSymbols = (hImplChnDev->readBuf[20] << 8) | hImplChnDev->readBuf[21];
                pStatus->status.l1PreStatus.cellId = (hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23];
                pStatus->status.l1PreStatus.networkId = (hImplChnDev->readBuf[24] << 8) | hImplChnDev->readBuf[25];
                pStatus->status.l1PreStatus.t2SystemId = (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27];
                pStatus->status.l1PreStatus.l1PostSize = (hImplChnDev->readBuf[32] << 24) | (hImplChnDev->readBuf[33] << 16) | (hImplChnDev->readBuf[34] << 8) | hImplChnDev->readBuf[35];
                pStatus->status.l1PreStatus.l1PostInfoSize = (hImplChnDev->readBuf[36] << 24) | (hImplChnDev->readBuf[37] << 16) | (hImplChnDev->readBuf[38] << 8) | hImplChnDev->readBuf[39];            
                break;                
            case 2:
                pStatus->type = BTC2_SelectiveAsyncStatusType_eL1PostConfigurable;
                pStatus->status.l1PostConfigurableStatus.numPlp = hImplChnDev->readBuf[12];
                pStatus->status.l1PostConfigurableStatus.numAux = hImplChnDev->readBuf[13] & 0xF;
                pStatus->status.l1PostConfigurableStatus.auxConfigRFu = hImplChnDev->readBuf[14];
                pStatus->status.l1PostConfigurableStatus.rfIdx = hImplChnDev->readBuf[15] & 0x7;
                pStatus->status.l1PostConfigurableStatus.fefType = (hImplChnDev->readBuf[15] & 0xF0) >> 4;
                pStatus->status.l1PostConfigurableStatus.fefInterval = hImplChnDev->readBuf[16];
                pStatus->status.l1PostConfigurableStatus.subSlicesPerFrame = (hImplChnDev->readBuf[18] << 8) | hImplChnDev->readBuf[19];
                pStatus->status.l1PostConfigurableStatus.frequency= (hImplChnDev->readBuf[20] << 24) | (hImplChnDev->readBuf[21] << 16) | (hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23];
                pStatus->status.l1PostConfigurableStatus.fefLength = (hImplChnDev->readBuf[24] << 24) | (hImplChnDev->readBuf[25] << 16) | (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27];
                pStatus->status.l1PostConfigurableStatus.plpA.plpId = hImplChnDev->readBuf[28];
                pStatus->status.l1PostConfigurableStatus.plpA.plpGroupId = hImplChnDev->readBuf[29];
                pStatus->status.l1PostConfigurableStatus.plpA.plpType = hImplChnDev->readBuf[30] & 0x7;
                pStatus->status.l1PostConfigurableStatus.plpA.plpPayloadType = (hImplChnDev->readBuf[30] & 0xF8) >> 3;
                pStatus->status.l1PostConfigurableStatus.plpA.ffFlag= hImplChnDev->readBuf[31] & 0x1;
                pStatus->status.l1PostConfigurableStatus.plpA.firstRfIdx = (hImplChnDev->readBuf[31] & 0xE) >> 1;
                pStatus->status.l1PostConfigurableStatus.plpA.plpFecType = (hImplChnDev->readBuf[31] & 0x30) >> 4;
                pStatus->status.l1PostConfigurableStatus.plpA.timeIlType = (hImplChnDev->readBuf[31] & 0x40) >> 6;
                pStatus->status.l1PostConfigurableStatus.plpA.inBandFlag = (hImplChnDev->readBuf[31] & 0x80) >> 7;
                pStatus->status.l1PostConfigurableStatus.plpA.plpCodeRate = hImplChnDev->readBuf[32] & 0x7;
                pStatus->status.l1PostConfigurableStatus.plpA.plpMod = (hImplChnDev->readBuf[32] & 0x38) >> 3;
                pStatus->status.l1PostConfigurableStatus.plpA.plpRotation = (hImplChnDev->readBuf[32] & 0x40) >> 6;
                pStatus->status.l1PostConfigurableStatus.plpA.firstFrameIdx = hImplChnDev->readBuf[33];
                pStatus->status.l1PostConfigurableStatus.plpA.frameInterval = hImplChnDev->readBuf[34];
                pStatus->status.l1PostConfigurableStatus.plpA.timeIlLength = hImplChnDev->readBuf[35];
                pStatus->status.l1PostConfigurableStatus.plpA.plpNumBlocksMax = (hImplChnDev->readBuf[36] << 8) | hImplChnDev->readBuf[37];
                pStatus->status.l1PostConfigurableStatus.plpB.plpId = hImplChnDev->readBuf[40];
                pStatus->status.l1PostConfigurableStatus.plpB.plpGroupId = hImplChnDev->readBuf[41];
                pStatus->status.l1PostConfigurableStatus.plpB.plpType = hImplChnDev->readBuf[42] & 0x7;
                pStatus->status.l1PostConfigurableStatus.plpB.plpPayloadType = (hImplChnDev->readBuf[42] & 0xF8) >> 3;
                pStatus->status.l1PostConfigurableStatus.plpB.ffFlag= hImplChnDev->readBuf[43] & 0x1;
                pStatus->status.l1PostConfigurableStatus.plpB.firstRfIdx = (hImplChnDev->readBuf[43] & 0xE) >> 1;
                pStatus->status.l1PostConfigurableStatus.plpB.plpFecType = (hImplChnDev->readBuf[43] & 0x30) >> 4;
                pStatus->status.l1PostConfigurableStatus.plpB.timeIlType = (hImplChnDev->readBuf[43] & 0x40) >> 6;
                pStatus->status.l1PostConfigurableStatus.plpB.inBandFlag = (hImplChnDev->readBuf[43] & 0x80) >> 7;
                pStatus->status.l1PostConfigurableStatus.plpB.plpCodeRate = hImplChnDev->readBuf[44] & 0x7;
                pStatus->status.l1PostConfigurableStatus.plpB.plpMod = (hImplChnDev->readBuf[44] & 0x38) >> 3;
                pStatus->status.l1PostConfigurableStatus.plpB.plpRotation = (hImplChnDev->readBuf[44] & 0x40) >> 6;
                pStatus->status.l1PostConfigurableStatus.plpB.firstFrameIdx = hImplChnDev->readBuf[45];
                pStatus->status.l1PostConfigurableStatus.plpB.frameInterval = hImplChnDev->readBuf[46];
                pStatus->status.l1PostConfigurableStatus.plpB.timeIlLength = hImplChnDev->readBuf[47];
                pStatus->status.l1PostConfigurableStatus.plpB.plpNumBlocksMax = (hImplChnDev->readBuf[48] << 8) | hImplChnDev->readBuf[49];
                pStatus->status.l1PostConfigurableStatus.auxStreamType = (hImplChnDev->readBuf[56] & 0xF0) >> 4;
                pStatus->status.l1PostConfigurableStatus.auxPrivateConf = ((hImplChnDev->readBuf[56] & 0xF) << 24) | (hImplChnDev->readBuf[57] << 16) | (hImplChnDev->readBuf[58] << 8) | hImplChnDev->readBuf[59];
                break;                
            case 3:
                pStatus->type = BTC2_SelectiveAsyncStatusType_eL1PostDynamic;
                pStatus->status.l1PostDynamicStatus.frameIdx = hImplChnDev->readBuf[12];
                pStatus->status.l1PostDynamicStatus.l1ChanlgeCounter = hImplChnDev->readBuf[13];
                pStatus->status.l1PostDynamicStatus.startRfIdx = hImplChnDev->readBuf[14] & 0x7;
                pStatus->status.l1PostDynamicStatus.subSliceInterval = (hImplChnDev->readBuf[20]<< 24) | (hImplChnDev->readBuf[21] << 16) | (hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23];
                pStatus->status.l1PostDynamicStatus.type2Start = (hImplChnDev->readBuf[24]<< 24) | (hImplChnDev->readBuf[25] << 16) | (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27];
                pStatus->status.l1PostDynamicStatus.plpA.plpId = hImplChnDev->readBuf[28];
                pStatus->status.l1PostDynamicStatus.plpA.plpNumBlocks = (hImplChnDev->readBuf[30] << 8) | hImplChnDev->readBuf[31];
                pStatus->status.l1PostDynamicStatus.plpA.plpStart = (hImplChnDev->readBuf[32]<< 24) | (hImplChnDev->readBuf[33] << 16) | (hImplChnDev->readBuf[34] << 8) | hImplChnDev->readBuf[35];
                pStatus->status.l1PostDynamicStatus.plpB.plpId = hImplChnDev->readBuf[36];
                pStatus->status.l1PostDynamicStatus.plpB.plpNumBlocks = (hImplChnDev->readBuf[38] << 8) | hImplChnDev->readBuf[39];
                pStatus->status.l1PostDynamicStatus.plpB.plpStart = (hImplChnDev->readBuf[40]<< 24) | (hImplChnDev->readBuf[41] << 16) | (hImplChnDev->readBuf[42] << 8) | hImplChnDev->readBuf[43];
                pStatus->status.l1PostDynamicStatus.auxPrivateDyn_47_32 = (hImplChnDev->readBuf[44] << 8) | hImplChnDev->readBuf[45];
                pStatus->status.l1PostDynamicStatus.auxPrivateDyn_31_0 = (hImplChnDev->readBuf[46]<< 24) | (hImplChnDev->readBuf[47] << 16) | (hImplChnDev->readBuf[48] << 8) | hImplChnDev->readBuf[49];   
                break;
            case 5:
                pStatus->type = BTC2_SelectiveAsyncStatusType_eL1Plp;                
                mp = hImplChnDev->readBuf[4] & 0x80;      
                pStatus->status.l1PlpStatus.numPlp = hImplChnDev->readBuf[12];
                
                if(pStatus->status.l1PlpStatus.numPlp)
                {
                    for(i = 0; i < BTC2_SEL_L1PLP_PKT_0_PLP_COUNT; i++)        
                    {
                        pStatus->status.l1PlpStatus.plp[plpCount].plpId=  hImplChnDev->readBuf[16+i*4];
                        pStatus->status.l1PlpStatus.plp[plpCount].plpGroupId =  hImplChnDev->readBuf[17+i*4];
                        pStatus->status.l1PlpStatus.plp[plpCount].plpPayloadType =  (hImplChnDev->readBuf[18+i*4] & 0xF8) >> 3;		
                        pStatus->status.l1PlpStatus.plp[plpCount].plpType =  hImplChnDev->readBuf[18+i*4] & 0x7;	  
                        plpCount++;
                        if(plpCount == pStatus->status.l1PlpStatus.numPlp)
                            break;
                    }

                    while(mp)
                    {      
                        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, hImplChnDev->readBuf, 9, hImplChnDev->readBuf, BTC2_L1PLP_BUF_MAX, false, true, BTC2_L1PLP_BUF_MAX));
                        mp = hImplChnDev->readBuf[4] & 0x80;
                        for(i = 0; i < BTC2_SEL_L1PLP_PKT_1_N_PLP_COUNT; i++)
                        {
                            pStatus->status.l1PlpStatus.plp[plpCount].plpId =  hImplChnDev->readBuf[12+i*4];
                            pStatus->status.l1PlpStatus.plp[plpCount].plpGroupId =	hImplChnDev->readBuf[13+i*4];
                            pStatus->status.l1PlpStatus.plp[plpCount].plpPayloadType =	(hImplChnDev->readBuf[14+i*4] & 0xF8) >> 3; 	
                            pStatus->status.l1PlpStatus.plp[plpCount].plpType =  hImplChnDev->readBuf[14+i*4] & 0x7;                       
                            plpCount++;                
                            if((plpCount == pStatus->status.l1PlpStatus.numPlp) || (plpCount > sizeof(pStatus->status.l1PlpStatus.plp)/sizeof(BTC2_Plp)))
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
                pStatus->type = (BTC2_SelectiveAsyncStatusType)(hImplChnDev->readBuf[9] - 6);
                pStatus->status.fecStatistics.lock = hImplChnDev->readBuf[12] & 0x1;
                pStatus->status.fecStatistics.snrData = ((hImplChnDev->readBuf[14] << 8) | hImplChnDev->readBuf[15]);
                pStatus->status.fecStatistics.ldpcAvgIter = ((hImplChnDev->readBuf[18] << 8) | hImplChnDev->readBuf[19]);
                pStatus->status.fecStatistics.ldpcTotIter = ((hImplChnDev->readBuf[20] << 24) | (hImplChnDev->readBuf[21] << 16) | (hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23]);
                pStatus->status.fecStatistics.ldpcTotFrm = ((hImplChnDev->readBuf[24] << 24) | (hImplChnDev->readBuf[25] << 16) | (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27]);
                pStatus->status.fecStatistics.ldpcUncFrm = ((hImplChnDev->readBuf[28] << 24) | (hImplChnDev->readBuf[29] << 16) | (hImplChnDev->readBuf[30] << 8) | hImplChnDev->readBuf[31]);
                pStatus->status.fecStatistics.ldpcBER = ((hImplChnDev->readBuf[32] << 24) | (hImplChnDev->readBuf[33] << 16) | (hImplChnDev->readBuf[34] << 8) | hImplChnDev->readBuf[35]);
                pStatus->status.fecStatistics.bchCorBit = ((hImplChnDev->readBuf[36] << 24) | (hImplChnDev->readBuf[37] << 16) | (hImplChnDev->readBuf[38] << 8) | hImplChnDev->readBuf[39]);
                pStatus->status.fecStatistics.bchTotBlk = ((hImplChnDev->readBuf[40] << 24) | (hImplChnDev->readBuf[41] << 16) | (hImplChnDev->readBuf[42] << 8) | hImplChnDev->readBuf[43]);
                pStatus->status.fecStatistics.bchClnBlk = ((hImplChnDev->readBuf[44] << 24) | (hImplChnDev->readBuf[45] << 16) | (hImplChnDev->readBuf[46] << 8) | hImplChnDev->readBuf[47]);
                pStatus->status.fecStatistics.bchCorBlk = ((hImplChnDev->readBuf[48] << 24) | (hImplChnDev->readBuf[49] << 16) | (hImplChnDev->readBuf[50] << 8) | hImplChnDev->readBuf[51]);
                pStatus->status.fecStatistics.bchUncBlk = ((hImplChnDev->readBuf[52] << 24) | (hImplChnDev->readBuf[53] << 16) | (hImplChnDev->readBuf[54] << 8) | hImplChnDev->readBuf[55]);               
                break;
            case 10:
                pStatus->type = (BTC2_SelectiveAsyncStatusType)(hImplChnDev->readBuf[9] - 2);            
                pStatus->status.shortStatus.acquisitionMode = hImplChnDev->readBuf[12] & 0x1;
                pStatus->status.shortStatus.bandwidth = (hImplChnDev->readBuf[12] & 0x1c) >> 2;               
                pStatus->status.shortStatus.lock = hImplChnDev->readBuf[13] & 0x1;
                pStatus->status.shortStatus.isSpectrumInverted = (hImplChnDev->readBuf[13] & 0x8) >> 3;
                pStatus->status.shortStatus.reacqCount = ((hImplChnDev->readBuf[16] << 8) | hImplChnDev->readBuf[17]);
                pStatus->status.shortStatus.snrEstimate = ((hImplChnDev->readBuf[20] << 8) | hImplChnDev->readBuf[21]);
                pStatus->status.shortStatus.gainOffset = ((hImplChnDev->readBuf[22] << 8) | hImplChnDev->readBuf[23]);
                pStatus->status.shortStatus.carrierFreqOffset = ((hImplChnDev->readBuf[24] << 24) | (hImplChnDev->readBuf[25] << 16) | (hImplChnDev->readBuf[26] << 8) | hImplChnDev->readBuf[27]);
                pStatus->status.shortStatus.timingOffset = ((hImplChnDev->readBuf[28] << 24) | (hImplChnDev->readBuf[29] << 16) | (hImplChnDev->readBuf[30] << 8) | hImplChnDev->readBuf[31]);   
                signalStrength = ((hImplChnDev->readBuf[32] << 8) | (hImplChnDev->readBuf[33] & 0xFF));
                pStatus->status.shortStatus.signalStrength = signalStrength*100/256 + 4875;                 
                pStatus->status.shortStatus.signalLevelPercent = hImplChnDev->readBuf[34];           
                pStatus->status.shortStatus.signalQualityPercent = hImplChnDev->readBuf[35];           
                break;
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                break;        
        }        
    }

done:
    BDBG_LEAVE(BTC2_3461_GetSelectiveAsyncStatus);
    return retCode;
}
