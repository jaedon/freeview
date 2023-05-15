/***************************************************************************
*     Copyright (c) 2006-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_fwproc_priv.c $
* $brcm_Revision: Hydra_Software_Devel/51 $
* $brcm_Date: 3/7/11 6:05p $
*
* Module Description:
*	This file contains PI-FW Interface.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_fwproc_priv.c $
* 
* Hydra_Software_Devel/51   3/7/11 6:05p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/7550_SCM_DEVEL/1   1/6/11 3:39p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/50   10/6/10 4:33p srajapur
* SW7468-323:[7468] sReferenceBuffParams added to
* BRAP_VF_P_sVDecodeBuffCfg struct for supporting new design of video
* decoder on ZSP
* 
* Hydra_Software_Devel/49   10/5/10 4:16p srajapur
* SW7468-323:[7468] sReferenceBuffParams added to
* BRAP_VF_P_sVDecodeBuffCfg struct for supporting new design of video
* decoder on ZSP
* 
* Hydra_Software_Devel/48   9/16/10 12:19a srajapur
* SW7468-298 :[7468]Merging Real Video Code to main latest
* 
* Hydra_Software_Devel/47   9/6/10 5:15p gautamk
* SW7550-459: [7550] Merging GFX support in mainline
* 
* Hydra_Software_Devel/46   9/6/10 3:43p gautamk
* SW7550-547: [7550] Adding support for System task downloadable code.
* Merging to mainline
* 
* Hydra_Software_Devel/RAP_7550_TASK_DOWNLOAD_BRANCH/1   8/31/10 3:36p gautamk
* SW7550-547: [7550] Adding support for System task downloadable code
* 
* Hydra_Software_Devel/45   8/13/10 5:23p gautamk
* SW7405-4713:[7405] Merging changes for RDB indepedence in Main line
* 
* Hydra_Software_Devel/RAP_RDB_INDEP_FW/1   8/3/10 1:38p gautamk
* SW7405-3359: [7405] Adding support for RDB indepdence for FW
* 
* Hydra_Software_Devel/44   11/17/09 6:27p sgadara
* SW3556-832 : [3548,3556] Adding the "dynamic port switching" feature
* support.
* --> Adding PI-CIT interface for FMM Port(Destination) reconfiguration
* 
* Hydra_Software_Devel/43   6/17/09 5:18p gautamk
* PR56011: [7405] Implementing code review comments
* 
* Hydra_Software_Devel/42   6/12/09 3:41p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/41   4/16/09 2:16p gautamk
* PR52383: [7405] Increasing the Interstage size to (MAX branch +1)
* 
* Hydra_Software_Devel/40   4/7/09 4:50p gautamk
* PR53393: [7405]
* PR53764: [7405] Memory OPtimization
* 
* Hydra_Software_Devel/39   4/7/09 2:21p gautamk
* PR53393: [7405]
* PR53764: [7405]
* Memory optimization
* 
* Hydra_Software_Devel/38   4/6/09 5:06p gautamk
* PR53764: [7405]
* PR53393: [7405] Memory optimization in RAP PI
* 
* Hydra_Software_Devel/37   4/1/09 4:39p gautamk
* PR53764: [7405]
* PR53393: [7405] Memory optimization in RAP PI
* 
* Hydra_Software_Devel/36   2/2/09 4:28p gautamk
* PR51652: [7405]
* PR51618: [7405] Changing the global variables as per following rule.
* 1) make them static
* 2) if you cannot make them static because they must be shared between
* multiple .c files in your module, you must rename them to include the
* BXXX_ magnum prefix for your module.
* 
* Hydra_Software_Devel/35   9/22/08 9:08a speter
* PR 43454: [7405]Adding correction for IOBuffer memory allocation
* 
* Hydra_Software_Devel/34   7/25/08 4:39p gautamk
* PR43493: [7405] TSM configuration can be  set asynchronously for
* different tasks, So using a single buffer for ChangeCfgCommand was
* causing overwrite of the memory. Using seperate memory buffer for each
* task for updating TSm config param.
* 
* Hydra_Software_Devel/33   7/18/08 4:20p gautamk
* PR44827: [7405] Increasing the async fifo size.
* 
* Hydra_Software_Devel/32   7/4/08 4:29p gautamk
* PR43454: [7405] Implementation for Adaptive rate ctrl (2ms lipsync)
* 
* Hydra_Software_Devel/31   5/29/08 3:30p gautamk
* PR41726: [7405] Adding function to insert/destroy stages internally in
* Task network.
* 
* Hydra_Software_Devel/30   5/15/08 3:37p gautamk
* PR42664: [7405] setting hTask->bStopped to false when watchdog recovery
* is called so that no command gets called till start of channel
* 
* Hydra_Software_Devel/29   5/3/08 4:28a gautamk
* PR41726: [7405] changes for Fw verification
* 
* Hydra_Software_Devel/28   4/23/08 8:03p gautamk
* PR42092: [7405] Fixing the check for return value of BRap_Allocaligned.
* 
* Hydra_Software_Devel/27   4/14/08 11:37a sushmit
* PR 34648: [7405] Reducing Stack Usage
* 
* Hydra_Software_Devel/26   3/14/08 1:53p tthomas
* PR34648: [7405] Changing Macro names. (On behalf of Gautam)
* 
* Hydra_Software_Devel/25   3/14/08 11:54a gautamk
* PR34648: [7405] Merging the branched code to mainline
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_TESTBRANCH/1   3/12/08 2:54p gautamk
* PR34648: [7405] Fixing bug in generate citinput and moving dspchn_start
* from opendecchannel to getchannelpath
* 
* Hydra_Software_Devel/24   2/29/08 9:10p gautamk
* PR34648: [7405] Fixing segmentaion fault at time of quitting rap_test,
* due to freeing the memory which is already freed.
* 
* Hydra_Software_Devel/23   2/29/08 4:25p gautamk
* PR34648: [7405] Changing file names includes
* 
* Hydra_Software_Devel/22   2/29/08 3:41p gautamk
* PR34648: [7405] Adding on the fly change for spdifstatuparam
* 
* Hydra_Software_Devel/21   2/20/08 7:06p nitinb
* PR34648: [7405] Fixed compilation warning
* 
* Hydra_Software_Devel/20   2/15/08 8:45p gautamk
* PR34648: [7405]Adding User Config for Passthru
* Adding spdif CBit buffer.
* 
* Hydra_Software_Devel/19   2/4/08 8:18p gautamk
* PR34648: [7405] Changes for CHG_CFG command
* 
* Hydra_Software_Devel/18   1/31/08 5:24p gautamk
* PR34648: [7405] Removing BRAP_DSPCHN_DecodeMode_eSimulMode Enum
* 
* Hydra_Software_Devel/17   1/30/08 3:07p gdata
* PR34648: [7405] Adding support for status buffers
* 
* Hydra_Software_Devel/16   1/30/08 1:45p gdata
* PR34648: [7405] Removing the hard coding of DSP fifo for response and
* command
* 
* Hydra_Software_Devel/15   1/29/08 7:09p gdata
* PR34648: [7405] Adding support for close calls
* 
* Hydra_Software_Devel/14   1/18/08 3:53p gautamk
* PR34648: [7405] Changes for statr time download
* 
* Hydra_Software_Devel/13   1/17/08 10:18p sushmit
* PR 34648: [7405] Memory for Start Time Download Code Checkin
* 
* Hydra_Software_Devel/12   1/17/08 2:51p sushmit
* PR 34648: [7405] More memory for CIT
* 
* Hydra_Software_Devel/11   1/16/08 4:50p nitinb
* PR34648: [7405] Added support for programming configuration parameters
* 
* Hydra_Software_Devel/10   1/16/08 12:18p sushmit
* PR34648: [7405] Syncing up extern structures w.r.t. fw_algo_init.c
* 
* Hydra_Software_Devel/9   1/16/08 11:57a sushmit
* PR34648: [7405] Downloading CIT to DRAM
* 
* Hydra_Software_Devel/8   1/15/08 2:30p sushmit
* PR34648: [7405] Moving CIT Output malloc to DRAM
* 
* Hydra_Software_Devel/7   1/15/08 1:37p sushmit
* PR34648: [7405] Checkin in updated code
* 
* Hydra_Software_Devel/6   1/11/08 6:40p gautamk
* PR34648: [7405] Files for Mapping Processing network to CIT input
* 
* Hydra_Software_Devel/5   1/10/08 7:03p sushmit
* PR34648: [7405] Fixing start time memory allocation
* 
* Hydra_Software_Devel/4   1/8/08 1:45p sushmit
* PR34648: [7405] Updating Memory allocation to allocate one big chunk
* for Iframe & Cfgbuf instead of separate buffers per stage
* 
***************************************************************************/

#include "brap.h"
#include "brap_types.h"
#include "brap_priv.h"
#include "brap_af_priv.h"
#include "brap_dspchn_priv.h"
#ifdef RAP_SCM_SUPPORT
#include "brap_scm.h"
#endif

BDBG_MODULE(rap_fwproc);

extern const BRAP_AF_P_sNODE_INFO BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMax];
extern const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eMax][BRAP_DSPCHN_DecodeMode_eMax];
extern const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eMax][BRAP_DSPCHN_EncodeMode_eSimulMode];
extern const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eMax][BRAP_DSPCHN_ProcessingMode_eMax];
extern const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sGfxDecodeExecInfo[BRAP_DSPCHN_GfxType_eMax];
#ifdef RAP_SCM_SUPPORT
extern const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sScmDecodeExecInfo[BRAP_DSPCHN_ScmType_eMax];
#endif

BERR_Code BRAP_Fwif_P_CalcMemPoolReq(BRAP_Handle hRap)
{
    /*
        Call BRAP_DSP_DownloadMemReq to get download memory requiremnts.
        --
        For (All Processing Algos Supported)
        NOTE: This can be further optimized by allocing combined sizes of 
        BRAP_MAX_FW_PROCESSING_SUPPORTED & providing memory from there but 
        that will enforce that all PAlgos are downloaded at open time only. 
        Same logic applies for IFrameBufs & ConfigParams 
        {
            Find max Processing Algo size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32PAlgo
        --
        For(BRAP_MAX_FW_TSK_SUPPORTED)  Based on usage mode 
            For (All Codecs Supported)
            {
                Find max codec scratch size required
            }
            Put in TempScratchSize
            For (All Processing Algos Supported)
            {
                Add up Scratch Size required
            }
            Add that to TempScratchSize
            Fill psDspMem->sDspMemSizeReq[uiDspId].ui32Scratch with TempScratchSize
        --
        For(BRAP_MAX_FW_TSK_SUPPORTED)  Based on usage mode 
            For (All Codecs Supported)
            {
                Find max codec IStage size required
            }
            Put in TempIStageSize
            For (All Processing Algos Supported)
            {
                Add up IStage Size required
            }
            Add that to TempIStageSize
            Fill psDspMem->sDspMemSizeReq[uiDspId].ui32IStage with TempIStageSize
        --
        For (All Codecs Supported)
        {
            Find max codec Iframe size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32IFrameCommon
        --
        For (All Processing Algos Supported)
        {
            Find max Processing Algo Iframe size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32IFramePAlgo
        --
        For (All Codecs Supported)
        {
            Find max codec ConfigParams size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32CfgBufCommon
        --
        For (All Processing Algos Supported)
        {
            Find max Processing Algo ConfigParams size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32CfgBufPAlgo
        
    */
    BERR_Code err = BERR_SUCCESS;
    uint32_t ui32OpenTimeDownloadSize = 0;
    uint32_t ui32DecScratch = 0, ui32DecIs = 0, ui32DecIsIf = 0, ui32DecIf = 0, ui32DecCfgBuf = 0, ui32DecStatusBuf = 0;
    uint32_t ui32EncScratch = 0, ui32EncIs = 0, ui32EncIsIf = 0, ui32EncIf = 0, ui32EncCfgBuf = 0, ui32EncStatusbuf = 0;
    uint32_t ui32PaScratch = 0, ui32PaIs = 0, ui32PaIsIf = 0, ui32PaIf = 0, ui32PaCfgBuf = 0, ui32PaStatusBuf = 0;
    uint32_t ui32TempScratch = 0, ui32TempIs = 0, ui32TempIsIf = 0, ui32TempIf = 0, ui32TempCfgBuf = 0, ui32TempStatusBuf =0,ui32NumCh=0;
    unsigned int i = 0, j = 0, uiNumNodes = 0;

    BDBG_ENTER(BRAP_Fwif_P_CalcMemPoolReq);

    /* Get FW Exec download memory requirements */
    err = BRAP_DSP_GetDownloadMemRequirement(hRap, &(hRap->sMemReq.sDwnldMemInfo), &ui32OpenTimeDownloadSize);
    if(BERR_SUCCESS != err)
    {
        BDBG_ERR(("BRAP_Fwif_P_CalcMemPoolReq: Error getting memory download requirements!"));
        return BERR_TRACE(err);
    }
    if(true == hRap->sSettings.bOpenTimeFwDownload)
    {
        hRap->sMemReq.ui32OpenTimeDownloadSize = ui32OpenTimeDownloadSize;
    }

    /* Calculate scratch & interstage & interstage interface memory requirements */
    /* For Decoders */
#ifdef RAP_VIDEOONDSP_SUPPORT
    for(i=0; i<BRAP_DSPCHN_VideoType_eMax; i++)
#else
    for(i=0; i<BRAP_DSPCHN_AudioType_eMax; i++)
#endif
    {
	 if(!(BRAP_FWDWNLD_P_IsAudCodecSupported(i)))
	 	continue;
        uiNumNodes = BRAP_sDecAlgoExecInfo[i][0].NumNodes; 
        ui32TempScratch = 0;
        ui32TempIs = 0;
        ui32TempIsIf = 0;
        ui32TempIf = 0;
        ui32TempCfgBuf = 0;
        ui32TempStatusBuf =0;
        for(j=0; (j<uiNumNodes)&&(j < BRAP_AF_P_MAX_NUM_NODES_IN_ALGO) ; j++)
        {
        	if( BRAP_sDecAlgoExecInfo[i][0].eAlgoIds[j] < BRAP_AF_P_AlgoId_eMax )
        {
            ui32TempScratch += BRAP_sNodeInfo[BRAP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32ScratchBuffSize;

            if(ui32TempIs < BRAP_sNodeInfo[BRAP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan)
                ui32TempIs = BRAP_sNodeInfo[BRAP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan;
            if(ui32NumCh < BRAP_sNodeInfo[BRAP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported)
                ui32NumCh = BRAP_sNodeInfo[BRAP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported;            
            
            ui32TempIsIf += BRAP_sNodeInfo[BRAP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterStgGenericBuffSize;
            ui32TempIf += hRap->sImgIdMappingArrays.ui32InterframeSize[BRAP_sDecAlgoExecInfo[i][0].eAlgoIds[j]];
/*                BRAP_sNodeInfo[BRAP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterFrmBuffSize;*/
            ui32TempCfgBuf += BRAP_sNodeInfo[BRAP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32UserCfgBuffSize;
            ui32TempStatusBuf += BRAP_sNodeInfo[BRAP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32StatusBuffSize;
        }
        }
        if(ui32TempScratch >= ui32DecScratch)
            ui32DecScratch = ui32TempScratch;
        if(ui32TempIs >= ui32DecIs)
            ui32DecIs = ui32TempIs;
        if(ui32TempIsIf >= ui32DecIsIf)
            ui32DecIsIf = ui32TempIsIf;
        if(ui32TempIf >= ui32DecIf)
            ui32DecIf = ui32TempIf;
        if(ui32TempCfgBuf >= ui32DecCfgBuf)
            ui32DecCfgBuf = ui32TempCfgBuf;
        if(ui32TempStatusBuf >= ui32DecStatusBuf)
            ui32DecStatusBuf = ui32TempStatusBuf;
    }
#ifdef 	RAP_GFX_SUPPORT    
    for(i=0; i<BRAP_DSPCHN_GfxType_eMax; i++)
    {
	 if(!(BRAP_FWDWNLD_P_IsGfxSupported(i)))
	 	continue;
        uiNumNodes = BRAP_sDecAlgoExecInfo[i][0].NumNodes; 
        ui32TempScratch = 0;
        ui32TempIs = 0;
        ui32TempIsIf = 0;
        ui32TempIf = 0;
        ui32TempCfgBuf = 0;
        ui32TempStatusBuf =0;
        for(j=0; (j<uiNumNodes)&&(j < BRAP_AF_P_MAX_NUM_NODES_IN_ALGO) ; j++)
        {
        	if( BRAP_sGfxDecodeExecInfo[i].eAlgoIds[j] < BRAP_AF_P_AlgoId_eMax )
            {
                ui32TempScratch += BRAP_sNodeInfo[BRAP_sGfxDecodeExecInfo[i].eAlgoIds[j]].ui32ScratchBuffSize;

                if(ui32TempIs < BRAP_sNodeInfo[BRAP_sGfxDecodeExecInfo[i].eAlgoIds[j]].ui32MaxSizePerChan)
                    ui32TempIs = BRAP_sNodeInfo[BRAP_sGfxDecodeExecInfo[i].eAlgoIds[j]].ui32MaxSizePerChan;
                if(ui32NumCh < BRAP_sNodeInfo[BRAP_sGfxDecodeExecInfo[i].eAlgoIds[j]].ui32MaxNumChansSupported)
                    ui32NumCh = BRAP_sNodeInfo[BRAP_sGfxDecodeExecInfo[i].eAlgoIds[j]].ui32MaxNumChansSupported;            
                
                ui32TempIsIf += BRAP_sNodeInfo[BRAP_sGfxDecodeExecInfo[i].eAlgoIds[j]].ui32InterStgGenericBuffSize;
                ui32TempIf += hRap->sImgIdMappingArrays.ui32InterframeSize[BRAP_sGfxDecodeExecInfo[i].eAlgoIds[j]];
                ui32TempCfgBuf += BRAP_sNodeInfo[BRAP_sGfxDecodeExecInfo[i].eAlgoIds[j]].ui32UserCfgBuffSize;
                ui32TempStatusBuf += BRAP_sNodeInfo[BRAP_sGfxDecodeExecInfo[i].eAlgoIds[j]].ui32StatusBuffSize;
            }
        }
        if(ui32TempScratch >= ui32DecScratch)
            ui32DecScratch = ui32TempScratch;
        if(ui32TempIs >= ui32DecIs)
            ui32DecIs = ui32TempIs;
        if(ui32TempIsIf >= ui32DecIsIf)
            ui32DecIsIf = ui32TempIsIf;
        if(ui32TempIf >= ui32DecIf)
            ui32DecIf = ui32TempIf;
        if(ui32TempCfgBuf >= ui32DecCfgBuf)
            ui32DecCfgBuf = ui32TempCfgBuf;
        if(ui32TempStatusBuf >= ui32DecStatusBuf)
            ui32DecStatusBuf = ui32TempStatusBuf;
    }
#endif
#ifdef 	RAP_SCM_SUPPORT    
		for(i=0; i<BRAP_DSPCHN_ScmType_eMax; i++)
		{
		 if(!(BRAP_FWDWNLD_P_IsScmSupported(i)))
			continue;
			uiNumNodes = BRAP_sDecAlgoExecInfo[i][0].NumNodes; 
			ui32TempScratch = 0;
			ui32TempIs = 0;
			ui32TempIsIf = 0;
			ui32TempIf = 0;
			ui32TempCfgBuf = 0;
			ui32TempStatusBuf =0;
			for(j=0; (j<uiNumNodes)&&(j < BRAP_AF_P_MAX_NUM_NODES_IN_ALGO) ; j++)
			{
				if( BRAP_sScmDecodeExecInfo[i].eAlgoIds[j] < BRAP_AF_P_AlgoId_eMax )
				{
					ui32TempScratch += BRAP_sNodeInfo[BRAP_sScmDecodeExecInfo[i].eAlgoIds[j]].ui32ScratchBuffSize;
	
					if(ui32TempIs < BRAP_sNodeInfo[BRAP_sScmDecodeExecInfo[i].eAlgoIds[j]].ui32MaxSizePerChan)
						ui32TempIs = BRAP_sNodeInfo[BRAP_sScmDecodeExecInfo[i].eAlgoIds[j]].ui32MaxSizePerChan;
					if(ui32NumCh < BRAP_sNodeInfo[BRAP_sScmDecodeExecInfo[i].eAlgoIds[j]].ui32MaxNumChansSupported)
						ui32NumCh = BRAP_sNodeInfo[BRAP_sScmDecodeExecInfo[i].eAlgoIds[j]].ui32MaxNumChansSupported;			
					
					ui32TempIsIf += BRAP_sNodeInfo[BRAP_sScmDecodeExecInfo[i].eAlgoIds[j]].ui32InterStgGenericBuffSize;
					ui32TempIf += hRap->sImgIdMappingArrays.ui32InterframeSize[BRAP_sScmDecodeExecInfo[i].eAlgoIds[j]];
					ui32TempCfgBuf += BRAP_sNodeInfo[BRAP_sScmDecodeExecInfo[i].eAlgoIds[j]].ui32UserCfgBuffSize;
					ui32TempStatusBuf += BRAP_sNodeInfo[BRAP_sScmDecodeExecInfo[i].eAlgoIds[j]].ui32StatusBuffSize;
				}
			}
			if(ui32TempScratch >= ui32DecScratch)
				ui32DecScratch = ui32TempScratch;
			if(ui32TempIs >= ui32DecIs)
				ui32DecIs = ui32TempIs;
			if(ui32TempIsIf >= ui32DecIsIf)
				ui32DecIsIf = ui32TempIsIf;
			if(ui32TempIf >= ui32DecIf)
				ui32DecIf = ui32TempIf;
			if(ui32TempCfgBuf >= ui32DecCfgBuf)
				ui32DecCfgBuf = ui32TempCfgBuf;
			if(ui32TempStatusBuf >= ui32DecStatusBuf)
				ui32DecStatusBuf = ui32TempStatusBuf;
		}
#endif

    /* For Encoders */
    for(i=0; i<BRAP_CIT_P_EncAudioType_eMax; i++)
    {
	 if(!(BRAP_FWDWNLD_P_IsEncodeSupported(i)))
	 	continue;    
        uiNumNodes = BRAP_sEncAlgoExecInfo[i][0].NumNodes; 
        ui32TempScratch = 0;
        ui32TempIs = 0;
        ui32TempIsIf = 0;
        ui32TempIf = 0;
        ui32TempCfgBuf = 0;
        ui32TempStatusBuf = 0;
        for(j=0; j<uiNumNodes; j++)
        {
             if( BRAP_sEncAlgoExecInfo[i][0].eAlgoIds[j] < BRAP_AF_P_AlgoId_eMax )
            {
            ui32TempScratch += BRAP_sNodeInfo[BRAP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32ScratchBuffSize;

            if(ui32TempIs < BRAP_sNodeInfo[BRAP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan)
                ui32TempIs = BRAP_sNodeInfo[BRAP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan;
            if(ui32NumCh < BRAP_sNodeInfo[BRAP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported)
                ui32NumCh = BRAP_sNodeInfo[BRAP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported;

            ui32TempIsIf += BRAP_sNodeInfo[BRAP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterStgGenericBuffSize;
            ui32TempIf += hRap->sImgIdMappingArrays.ui32InterframeSize[BRAP_sEncAlgoExecInfo[i][0].eAlgoIds[j]];
/*                BRAP_sNodeInfo[BRAP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterFrmBuffSize;*/
            ui32TempCfgBuf += BRAP_sNodeInfo[BRAP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32UserCfgBuffSize;
            ui32TempStatusBuf += BRAP_sNodeInfo[BRAP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32StatusBuffSize;
        }
        }
        if(ui32TempScratch >= ui32EncScratch)
            ui32EncScratch = ui32TempScratch;
        if(ui32TempIs >= ui32EncIs)
            ui32EncIs = ui32TempIs;
        if(ui32TempIsIf >= ui32EncIsIf)
            ui32EncIsIf = ui32TempIsIf;
        if(ui32TempIf >= ui32EncIf)
            ui32EncIf = ui32TempIf;
        if(ui32TempCfgBuf >= ui32EncCfgBuf)
            ui32EncCfgBuf = ui32TempCfgBuf;
        if(ui32TempStatusBuf >= ui32EncStatusbuf)
            ui32EncStatusbuf = ui32TempStatusBuf;
    }
    /* For Processing Algos */
    for(i=0; i<BRAP_CIT_P_ProcessingType_eMax; i++)
    {
	 if(!(BRAP_FWDWNLD_P_IsAudProcSupported(i)))
	 	continue;        
        uiNumNodes = BRAP_sPpAlgoExecInfo[i][0].NumNodes; 
        ui32TempScratch = 0;
        ui32TempIs = 0;
        ui32TempIsIf = 0;
        ui32TempIf = 0;
        ui32TempCfgBuf = 0;
        ui32TempStatusBuf = 0;
        for(j=0; j<uiNumNodes; j++)
        {
             if( BRAP_sPpAlgoExecInfo[i][0].eAlgoIds[j] < BRAP_AF_P_AlgoId_eMax )
            {        
            ui32TempScratch += BRAP_sNodeInfo[BRAP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32ScratchBuffSize;
            if(ui32TempIs < BRAP_sNodeInfo[BRAP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan)
                ui32TempIs = BRAP_sNodeInfo[BRAP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan;
            
            if(ui32NumCh < BRAP_sNodeInfo[BRAP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported)
                ui32NumCh = BRAP_sNodeInfo[BRAP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported;

            ui32TempIsIf += BRAP_sNodeInfo[BRAP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterStgGenericBuffSize;
            ui32TempIf += hRap->sImgIdMappingArrays.ui32InterframeSize[BRAP_sPpAlgoExecInfo[i][0].eAlgoIds[j]];
             /*   BRAP_sNodeInfo[BRAP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterFrmBuffSize;*/
            ui32TempCfgBuf += BRAP_sNodeInfo[BRAP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32UserCfgBuffSize;
            ui32TempStatusBuf += BRAP_sNodeInfo[BRAP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32StatusBuffSize;
            }
        }
        if(ui32TempScratch >= ui32PaScratch)
            ui32PaScratch = ui32TempScratch;
        if(ui32TempIs >= ui32PaIs)
            ui32PaIs = ui32TempIs;
        if(ui32TempIsIf >= ui32PaIsIf)
            ui32PaIsIf = ui32TempIsIf;
        if(ui32TempIf >= ui32PaIf)
            ui32PaIf = ui32TempIf;
        if(ui32TempCfgBuf >= ui32PaCfgBuf)
            ui32PaCfgBuf = ui32TempCfgBuf;
        if(ui32TempStatusBuf >= ui32PaStatusBuf)
            ui32PaStatusBuf = ui32TempStatusBuf;
    }
    ui32TempScratch = ui32DecScratch;
    if(ui32EncScratch >= ui32TempScratch)
        ui32TempScratch = ui32EncScratch;
    if(ui32PaScratch >= ui32TempScratch)
        ui32TempScratch = ui32PaScratch;    
    ui32TempIs = ui32DecIs;
    if(ui32EncIs >= ui32TempIs)
        ui32TempIs = ui32EncIs;
    if(ui32PaIs >= ui32TempIs)
        ui32TempIs = ui32PaIs;

    ui32TempIs = ui32TempIs*ui32NumCh*(BRAP_MAX_PP_BRANCH_SUPPORTED +1);
    ui32TempIsIf = ui32DecIsIf;
    if(ui32EncIsIf >= ui32TempIsIf)
        ui32TempIsIf = ui32EncIsIf;
    
    if(ui32PaIsIf >= ui32TempIsIf)
        ui32TempIsIf = ui32PaIsIf;    

    ui32TempIf = ui32DecIf + ui32EncIf;
    ui32TempCfgBuf = ui32DecCfgBuf + ui32EncCfgBuf;


    ui32TempStatusBuf = ui32DecStatusBuf + ui32EncStatusbuf;

    for(i=0; i<BRAP_MAX_DSP_SUPPORTED; i++)
    {
        BDBG_MSG(("ui32TempScratch = %d , ui32TempIs = %d ui32TempIsIf =%d",ui32TempScratch,ui32TempIs,ui32TempIsIf));
        hRap->sMemReq.ui32DspScratchMemReq[i] = (ui32TempScratch + ui32TempIs + ui32TempIsIf);
        hRap->sMemReq.sTskMemReq[i].ui32IframeCommon = ui32TempIf;
        hRap->sMemReq.sTskMemReq[i].ui32CfgBufCommon = ui32TempCfgBuf;
        hRap->sMemReq.sTskMemReq[i].ui32StatusBufCommon = ui32TempStatusBuf;        
        hRap->sMemReq.sTskMemReq[i].ui32IFramePAlgo = ui32PaIf;
        hRap->sMemReq.sTskMemReq[i].ui32CfgBufPAlgo = ui32PaCfgBuf;
        hRap->sMemReq.sTskMemReq[i].ui32StatusBufPAlgo = ui32PaStatusBuf;
        hRap->sMemReq.sTskMemReq[i].ui32CBitBuf = BRAP_CIT_P_TASK_SPDIF_USER_CFG_MEM_SIZE;
        hRap->sMemReq.sTskMemReq[i].ui32ExtraBuf = BRAP_CIT_P_TOTAL_TASK_CFG_MEMORY  /*BRAP_CIT_P_TASK_SWAP_BUFFER_SIZE_INBYTES + BRAP_CIT_P_TASK_PORT_CONFIG_MEM_SIZE + BRAP_CIT_P_TASK_FMM_GATE_OPEN_CONFIG*/;
    }

    if(ui32TempCfgBuf< sizeof(BRAP_AF_P_sFMM_DEST_CFG))
    {
        hRap->sMemReq.ui32ConfigBufReq = sizeof(BRAP_AF_P_sFMM_DEST_CFG);        
    }
    else
    {
        hRap->sMemReq.ui32ConfigBufReq = ui32TempCfgBuf;
    }

    BDBG_LEAVE(BRAP_Fwif_P_CalcMemPoolReq);
    return err;
}

BERR_Code BRAP_DSP_AllocMem(BRAP_Handle hRap)
{
    BERR_Code err = BERR_SUCCESS;
    uint32_t ui32BaseAddr = 0,ui32UsedSize = 0, ui32TempAddr = 0;
#ifdef RAP_VIDEOONDSP_SUPPORT    
    unsigned int uiTempSize=0;
    BXDM_Picture *hXDMPicture = NULL;
#endif
    unsigned int i = 0, j = 0;
#ifdef RAP_GFX_SUPPORT    
    unsigned int    size=0;
#endif
    bool    bOpenTimeAudProcDownload=false;

    BDBG_ENTER(BRAP_DSP_AllocMem);

    BDBG_ASSERT(hRap);

    err = BRAP_Fwif_P_CalcMemPoolReq(hRap);
    if(err != BERR_SUCCESS)
    {
        BDBG_ERR(("BRAP_DSP_AllocMem: Unable to get memory requirements for DSP!"));
        return BERR_TRACE(err);
    }
    if(true == hRap->sSettings.bOpenTimeFwDownload)
    {
        ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, hRap->sMemReq.ui32OpenTimeDownloadSize,5, 0);	/* 32 bit aligned*/
        if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for open time download!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        BKNI_Memset((void *)ui32BaseAddr,0,hRap->sMemReq.ui32OpenTimeDownloadSize);
        hRap->sMemInfo.sOpenTimeMemInfo.ui32BaseAddr = ui32BaseAddr;
        hRap->sMemInfo.sOpenTimeMemInfo.ui32Size = hRap->sMemReq.ui32OpenTimeDownloadSize;

        /* Fill up pointers */
        ui32TempAddr = ui32BaseAddr;
        ui32UsedSize = 0;
		BKNI_Memcpy((void *)(&(hRap->sMemInfo.sDwnldMemInfo)),&(hRap->sMemReq.sDwnldMemInfo),sizeof(BRAP_DSP_DwnldMemInfo));		
        hRap->sMemInfo.sDwnldMemInfo.ui32SystemIbootCodePtr = ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.ui32SystemIbootCodeSize;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.ui32SystemIbootCodeSize;
        hRap->sMemInfo.sDwnldMemInfo.ui32SystemCodePtr = ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.ui32SystemCodeSize;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.ui32SystemCodeSize;
        hRap->sMemInfo.sDwnldMemInfo.ui32SystemDataPtr = ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.ui32SystemDataSize;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.ui32SystemDataSize;
        hRap->sMemInfo.sDwnldMemInfo.ui32SystemRdbVariablesPtr= ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.ui32SystemRdbVariablesSize;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.ui32SystemRdbVariablesSize;        
#ifdef RAP_SCM_SUPPORT		
        hRap->sMemInfo.sDwnldMemInfo.ui32SystemTableOfHashesPtr = ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.ui32SystemTableOfHashesSize;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.ui32SystemTableOfHashesSize;
#endif		
        hRap->sMemInfo.sDwnldMemInfo.ui32TsmCodePtr = ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.ui32TsmCodeSize;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.ui32TsmCodeSize;

        hRap->sMemInfo.sDwnldMemInfo.ui32SystemTaskCodePtr= ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.ui32SystemTaskCodeSize;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.ui32SystemTaskCodeSize;        

        hRap->sMemInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32BaseAddr = ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size;
        hRap->sMemInfo.sDwnldMemInfo.sFsBufInfo[0].ui32BaseAddr = ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.sFsBufInfo[0].ui32Size;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.sFsBufInfo[0].ui32Size;
        hRap->sMemInfo.sDwnldMemInfo.sDecCodecBufInfo[0].ui32BaseAddr = ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size;
        hRap->sMemInfo.sDwnldMemInfo.sPtCodecBufInfo[0].ui32BaseAddr = ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size;
        hRap->sMemInfo.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32BaseAddr = ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size;

#ifdef RAP_VIDEOONDSP_SUPPORT
        hRap->sMemInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32BaseAddr = ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32Size;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32Size;        
#endif

#ifdef RAP_GFX_SUPPORT
        hRap->sMemInfo.sDwnldMemInfo.sGfxBufInfo[0].ui32BaseAddr = ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.sGfxBufInfo[0].ui32Size;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.sGfxBufInfo[0].ui32Size;        
#endif        
#ifdef RAP_SCM_SUPPORT 
        hRap->sMemInfo.sDwnldMemInfo.sScmBufInfo[0].ui32BaseAddr = ui32TempAddr;
        ui32TempAddr += hRap->sMemInfo.sDwnldMemInfo.sScmBufInfo[0].ui32Size;
        ui32UsedSize += hRap->sMemInfo.sDwnldMemInfo.sScmBufInfo[0].ui32Size;        
#endif		

		BKNI_Memcpy((void *)(&(hRap->sDwnldMemInfo)),&(hRap->sMemInfo.sDwnldMemInfo),sizeof(BRAP_DSP_DwnldMemInfo));		
    }
    else
    {
		BKNI_Memcpy((void *)(&(hRap->sMemInfo.sDwnldMemInfo)),&(hRap->sMemReq.sDwnldMemInfo),sizeof(BRAP_DSP_DwnldMemInfo));		
        /* Iboot */
        ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, hRap->sMemReq.sDwnldMemInfo.ui32SystemIbootCodeSize,2, 0);	/* 32 bit aligned*/
        if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Iboot!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        hRap->sMemInfo.sDwnldMemInfo.ui32SystemIbootCodePtr = ui32BaseAddr;
        /* SystemCode */
        ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, hRap->sMemReq.sDwnldMemInfo.ui32SystemCodeSize,2, 0);	/* 32 bit aligned*/
        if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for SystemCode!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        hRap->sMemInfo.sDwnldMemInfo.ui32SystemCodePtr = ui32BaseAddr;
        /* SystemData */
        ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, hRap->sMemReq.sDwnldMemInfo.ui32SystemDataSize,2, 0);	/* 32 bit aligned*/
        if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for SystemData!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        hRap->sMemInfo.sDwnldMemInfo.ui32SystemDataPtr = ui32BaseAddr;

        /* System RDB */
        ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, hRap->sMemReq.sDwnldMemInfo.ui32SystemRdbVariablesSize,2, 0);	/* 32 bit aligned*/
        if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for SystemData!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        hRap->sMemInfo.sDwnldMemInfo.ui32SystemRdbVariablesPtr  = ui32BaseAddr;
#ifdef RAP_SCM_SUPPORT
        /* System Table Of Hashes */
        ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, hRap->sMemReq.sDwnldMemInfo.ui32SystemTableOfHashesSize,2, 0);	/* 32 bit aligned*/
        if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for SystemTableOfHashes!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        hRap->sMemInfo.sDwnldMemInfo.ui32SystemTableOfHashesPtr  = ui32BaseAddr;
#endif		

        
        /* TsmCode */
        ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, hRap->sMemReq.sDwnldMemInfo.ui32TsmCodeSize,2, 0);	/* 32 bit aligned*/
        if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for TsmCode!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        hRap->sMemInfo.sDwnldMemInfo.ui32TsmCodePtr = ui32BaseAddr;

        /* System Task Code */
        ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, hRap->sMemReq.sDwnldMemInfo.ui32SystemTaskCodeSize,2, 0);	/* 32 bit aligned*/
        if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for System Task Code!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        hRap->sMemInfo.sDwnldMemInfo.ui32SystemTaskCodePtr= ui32BaseAddr;        

    BRAP_FWDWNLD_P_IfOpenTimeProcDownload(&bOpenTimeAudProcDownload);

        if(true == bOpenTimeAudProcDownload)
        {	
            if(hRap->sMemReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size)
            {
                ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, 
                    hRap->sMemReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size,2, 0);   /* 32 bit aligned*/
                if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
                {
                    BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Processing Algo!"));
                    return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                }
                hRap->sMemInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32BaseAddr = ui32BaseAddr ;
                hRap->sMemInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size = 
					hRap->sMemReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size;				
            }        
        }
        else
        {
            if(hRap->sMemReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size)
            {
                ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, 
                    hRap->sMemReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size *
                    BRAP_DSP_P_MAX_AUD_PROCESSING_CTX,2, 0);   /* 32 bit aligned*/
                if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
                {
                    BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Processing Algo!"));
                    return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                }
                for(i=0;i<BRAP_DSP_P_MAX_AUD_PROCESSING_CTX;i++)
                {
                    hRap->sMemInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[i].ui32BaseAddr = 
                        ui32BaseAddr + i*hRap->sMemReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size;
                    hRap->sMemInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[i].ui32Size = 
                        hRap->sMemReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size;
    			hRap->sMemInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[i].numUser = -1;
    			hRap->sMemInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[i].uExecAlgoType.ePAlgoType = BRAP_ProcessingType_eMax;
    		
                }
            }
        }
        if(hRap->sMemReq.sDwnldMemInfo.sFsBufInfo[0].ui32Size)
        {
            ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, 
                hRap->sMemReq.sDwnldMemInfo.sFsBufInfo[0].ui32Size *
                BRAP_DSP_P_MAX_FS_CTXT,2, 0);   /* 32 bit aligned*/
            if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
            {
                BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for FrameSync!"));
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
            for(i=0;i<BRAP_DSP_P_MAX_FS_CTXT;i++)
            {
                hRap->sMemInfo.sDwnldMemInfo.sFsBufInfo[i].ui32BaseAddr = 
                    ui32BaseAddr + i*hRap->sMemReq.sDwnldMemInfo.sFsBufInfo[0].ui32Size;
                hRap->sMemInfo.sDwnldMemInfo.sFsBufInfo[i].ui32Size = 
                    hRap->sMemReq.sDwnldMemInfo.sFsBufInfo[0].ui32Size;
			hRap->sMemInfo.sDwnldMemInfo.sFsBufInfo[i].numUser = -1;
			hRap->sMemInfo.sDwnldMemInfo.sFsBufInfo[i].uExecAlgoType.eAlgoType = BRAP_DSPCHN_AudioType_eMax;				
            }
        }
        if(hRap->sMemReq.sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size)
        {
            ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, 
                hRap->sMemReq.sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size *
                BRAP_DSP_P_MAX_DECODE_CTXT,2, 0); /* 32 bit aligned*/
            if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
            {
                BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Decode!"));
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
            for(i=0;i<BRAP_DSP_P_MAX_DECODE_CTXT;i++)
            {
                hRap->sMemInfo.sDwnldMemInfo.sDecCodecBufInfo[i].ui32BaseAddr = 
                    ui32BaseAddr + i*hRap->sMemReq.sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size;
                hRap->sMemInfo.sDwnldMemInfo.sDecCodecBufInfo[i].ui32Size = 
                    hRap->sMemReq.sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size;
			hRap->sMemInfo.sDwnldMemInfo.sDecCodecBufInfo[i].numUser = -1;
			hRap->sMemInfo.sDwnldMemInfo.sDecCodecBufInfo[i].uExecAlgoType.eAlgoType = BRAP_DSPCHN_AudioType_eMax;					
            }
        }
        if(hRap->sMemReq.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size)
        {
            ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, 
                hRap->sMemReq.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size *
                BRAP_DSP_P_MAX_PASSTHRU_CTXT,2, 0);  /* 32 bit aligned*/
            if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
            {
                BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Passthru!"));
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
            for(i=0;i<BRAP_DSP_P_MAX_PASSTHRU_CTXT;i++)
            {
                hRap->sMemInfo.sDwnldMemInfo.sPtCodecBufInfo[i].ui32BaseAddr = 
                    ui32BaseAddr + i*hRap->sMemReq.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size;
                hRap->sMemInfo.sDwnldMemInfo.sPtCodecBufInfo[i].ui32Size = 
                    hRap->sMemReq.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size;
			hRap->sMemInfo.sDwnldMemInfo.sPtCodecBufInfo[i].numUser = -1;
			hRap->sMemInfo.sDwnldMemInfo.sPtCodecBufInfo[i].uExecAlgoType.ePAlgoType = BRAP_ProcessingType_eMax;					
				
            }
        }
        if( hRap->sMemReq.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size *BRAP_DSP_P_MAX_ENCODE_CTXT)
        {
            ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, 
                hRap->sMemReq.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size *
                BRAP_DSP_P_MAX_ENCODE_CTXT,2, 0);  /* 32 bit aligned*/
            if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
            {
                BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Processing Algo!"));
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
            for(i=0;i<BRAP_DSP_P_MAX_ENCODE_CTXT;i++)
            {
                hRap->sMemInfo.sDwnldMemInfo.sEncodeCodecBufInfo[i].ui32BaseAddr = 
                    ui32BaseAddr + i*hRap->sMemReq.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size;
                hRap->sMemInfo.sDwnldMemInfo.sEncodeCodecBufInfo[i].ui32Size = 
                    hRap->sMemReq.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size;
			hRap->sMemInfo.sDwnldMemInfo.sEncodeCodecBufInfo[i].numUser = -1;
			hRap->sMemInfo.sDwnldMemInfo.sEncodeCodecBufInfo[i].uExecAlgoType.ePAlgoType = BRAP_ProcessingType_eMax;					
            }
        }
#ifdef RAP_VIDEOONDSP_SUPPORT
        if(hRap->sMemReq.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32Size)
        {
            ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, 
                hRap->sMemReq.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32Size *
                BRAP_DSP_P_MAX_VIDEODECODE_CTXT,2, 0); /* 32 bit aligned*/
            if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
            {
                BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Decode!"));
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
            for(i=0;i<BRAP_DSP_P_MAX_VIDEODECODE_CTXT;i++)
            {
                hRap->sMemInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[i].ui32BaseAddr = 
                    ui32BaseAddr + i*hRap->sMemReq.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32Size;
                hRap->sMemInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[i].ui32Size = 
                    hRap->sMemReq.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32Size;
			hRap->sMemInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[i].numUser = -1;
			hRap->sMemInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[i].uExecAlgoType.eVideoAlgoType = BRAP_DSPCHN_VideoType_eMax;					
            }
        }        
#endif

#ifdef RAP_GFX_SUPPORT
        if(hRap->sMemReq.sDwnldMemInfo.sGfxBufInfo[0].ui32Size)
        {
            ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, 
                hRap->sMemReq.sDwnldMemInfo.sGfxBufInfo[0].ui32Size *
                BRAP_DSP_P_MAX_GFX_CTXT,2, 0); /* 32 bit aligned*/
            if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
            {
                BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Decode!"));
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
            for(i=0;i<BRAP_DSP_P_MAX_GFX_CTXT;i++)
            {
                hRap->sMemInfo.sDwnldMemInfo.sGfxBufInfo[i].ui32BaseAddr = 
                    ui32BaseAddr + i*hRap->sMemReq.sDwnldMemInfo.sGfxBufInfo[0].ui32Size;
                hRap->sMemInfo.sDwnldMemInfo.sGfxBufInfo[i].ui32Size = 
                    hRap->sMemReq.sDwnldMemInfo.sGfxBufInfo[0].ui32Size;
			hRap->sMemInfo.sDwnldMemInfo.sGfxBufInfo[i].numUser = -1;
			hRap->sMemInfo.sDwnldMemInfo.sGfxBufInfo[i].uExecAlgoType.eGfxAlgoType = BRAP_DSPCHN_GfxType_eMax;					
            }
        }        
#endif        
#ifdef RAP_SCM_SUPPORT 
        if(hRap->sMemReq.sDwnldMemInfo.sScmBufInfo[0].ui32Size)
        {
            ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, 
                hRap->sMemReq.sDwnldMemInfo.sScmBufInfo[0].ui32Size *
                BRAP_DSP_P_MAX_SCM_CTXT,2, 0); /* 32 bit aligned*/
            if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
            {
                BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for SCM!"));
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
            for(i=0;i<BRAP_DSP_P_MAX_SCM_CTXT;i++)
            {
                hRap->sMemInfo.sDwnldMemInfo.sScmBufInfo[i].ui32BaseAddr = 
                    ui32BaseAddr + i*hRap->sMemReq.sDwnldMemInfo.sScmBufInfo[0].ui32Size;
                hRap->sMemInfo.sDwnldMemInfo.sScmBufInfo[i].ui32Size = 
                    hRap->sMemReq.sDwnldMemInfo.sScmBufInfo[0].ui32Size;
			hRap->sMemInfo.sDwnldMemInfo.sScmBufInfo[i].numUser = -1;
			hRap->sMemInfo.sDwnldMemInfo.sScmBufInfo[i].uExecAlgoType.eScmAlgoType = BRAP_DSPCHN_ScmType_eMax;					
            }
        }
#endif		

		BKNI_Memcpy((void *)(&(hRap->sDwnldMemInfo)),&(hRap->sMemInfo.sDwnldMemInfo),sizeof(BRAP_DSP_DwnldMemInfo));		
    }
    
        for(i=0;i<BRAP_MAX_DSP_SUPPORTED;i++)
        {
            /* Allocate Task Memory */
            /* Allocate Scratch */
            ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, hRap->sMemReq.ui32DspScratchMemReq[i],2, 0);	/* 32 bit aligned*/
            if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
            {
                BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for scratch!"));
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
            hRap->sMemInfo.sDspScratchInfo[i].ui32BaseAddr = ui32BaseAddr;
            hRap->sMemInfo.sDspScratchInfo[i].ui32Size = hRap->sMemReq.ui32DspScratchMemReq[i];
            /* Add up memory requirements to create a whole big chunk */
            /* Add up memory requirement to create a whole big chunk for status buffer */
            ui32UsedSize = hRap->sMemReq.sTskMemReq[i].ui32IframeCommon +
                hRap->sMemReq.sTskMemReq[i].ui32CfgBufCommon +
                hRap->sMemReq.sTskMemReq[i].ui32IFramePAlgo * 
                    BRAP_MAX_PP_BRANCH_SUPPORTED * (BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED -1) +
                hRap->sMemReq.sTskMemReq[i].ui32CfgBufPAlgo * 
                    BRAP_MAX_PP_BRANCH_SUPPORTED * (BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED -1) + 
                BRAP_MAX_PP_BRANCH_SUPPORTED * BRAP_CIT_P_SIZEOF_IOBUFFCFG *
                    BRAP_AF_P_MAX_IP_FORKS+
                hRap->sMemReq.sTskMemReq[i].ui32StatusBufCommon +
               hRap->sMemReq.sTskMemReq[i].ui32StatusBufPAlgo*
                   BRAP_MAX_PP_BRANCH_SUPPORTED * (BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED -1)
                   +  hRap->sMemReq.sTskMemReq[i].ui32CBitBuf
                   +  hRap->sMemReq.sTskMemReq[i].ui32ExtraBuf;
            for(j=0;j<BRAP_MAX_FW_TSK_SUPPORTED;j++)
            {
                ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, ui32UsedSize,2, 0);	/* 32 bit aligned*/
                if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
                {
                    BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for IFrameCfgBuf!"));
                    return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                }
                else
                {
                    hRap->sMemInfo.sTskMemInfo[i].sTskIFrameCfgBufInfo[j].ui32BaseAddr = ui32BaseAddr;
                    hRap->sMemInfo.sTskMemInfo[i].sTskIFrameCfgBufInfo[j].ui32Size = ui32UsedSize;
                }
            }
    }

    /* Allocate Task Cmd/Resp Queues */
    for(i=0;i<BRAP_MAX_DSP_SUPPORTED;i++)
    {
        /* Allocate Command queue */
        ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, ((BRAP_MAX_MSGS_PER_QUEUE *BRAP_MAX_FW_TSK_SUPPORTED + BRAP_ADDITIONAL_MSGS_FOR_GFX)* sizeof(BRAP_FWIF_P_Command)),2, 0);
        if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for cmd queue!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        hRap->sMemInfo.sCmdQueue[i].uiBaseAddr = ui32BaseAddr;
        hRap->sMemInfo.sCmdQueue[i].uiMsgQueueSize = ((BRAP_MAX_MSGS_PER_QUEUE *BRAP_MAX_FW_TSK_SUPPORTED + BRAP_ADDITIONAL_MSGS_FOR_GFX)* sizeof(BRAP_FWIF_P_Command));
        /*TODO : Correct the calculation for DSP1*/
        hRap->sMemInfo.sCmdQueue[i].ui32FifoId = (2*i) + BRAP_DSP_P_FIFO_CMD;

        /* Allocate Generic Response queue */
        ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, (BRAP_MAX_MSGS_PER_QUEUE * sizeof(BRAP_FWIF_P_Response)),2, 0);
        if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for generic response queue!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        hRap->sMemInfo.sGenRspQueue[i].uiBaseAddr = ui32BaseAddr;
        hRap->sMemInfo.sGenRspQueue[i].uiMsgQueueSize = BRAP_MAX_MSGS_PER_QUEUE * sizeof(BRAP_FWIF_P_Response);
        /*TODO : Correct the calculation for DSP1*/
        hRap->sMemInfo.sGenRspQueue[i].ui32FifoId = (2*i) + BRAP_DSP_P_FIFO_GENERIC_RSP;

#ifdef RAP_VIDEOONDSP_SUPPORT
        /* Allocate Picture delivery queue */
        ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap,BRAP_REALVIDEO_MAX_MSGS_PER_QUEUE*4,2, 0);
        if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for PDQ!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        hRap->sMemInfo.sPDQueue[i].uiBaseAddr = ui32BaseAddr;
        hRap->sMemInfo.sPDQueue[i].uiMsgQueueSize = BRAP_REALVIDEO_MAX_MSGS_PER_QUEUE*4;
        /*TODO : Correct the calculation for DSP1*/
        hRap->sMemInfo.sPDQueue[i].ui32FifoId = (2*i) + BRAP_REALVIDEO_DSP_P_FIFO_PDQ;

        /* Allocate Picture release queue */
        ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap,BRAP_REALVIDEO_MAX_MSGS_PER_QUEUE*4,2, 0);
        if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for PRQ!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        hRap->sMemInfo.sPRQueue[i].uiBaseAddr = ui32BaseAddr;
        hRap->sMemInfo.sPRQueue[i].uiMsgQueueSize = BRAP_REALVIDEO_MAX_MSGS_PER_QUEUE*4;
        /*TODO : Correct the calculation for DSP1*/
        hRap->sMemInfo.sPRQueue[i].ui32FifoId = (2*i) + BRAP_REALVIDEO_DSP_P_FIFO_PRQ;

        /* Allocate Display queue */
        ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap,BRAP_REALVIDEO_MAX_MSGS_PER_QUEUE*4,2, 0);
        if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for DSQ!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        hRap->sMemInfo.sDSQueue[i].uiBaseAddr = ui32BaseAddr;
        hRap->sMemInfo.sDSQueue[i].uiMsgQueueSize = BRAP_REALVIDEO_MAX_MSGS_PER_QUEUE*4;
        /*TODO : Correct the calculation for DSP1*/
        hRap->sMemInfo.sDSQueue[i].ui32FifoId = (2*i) + BRAP_REALVIDEO_DSP_P_FIFO_DSQ;
        
#endif
        /* Allocate sync & async task queues */
        for(j=0;j<BRAP_MAX_FW_TSK_SUPPORTED;j++)
        {
            /* Sync Queue */
            ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, (BRAP_MAX_MSGS_PER_QUEUE * sizeof(BRAP_FWIF_P_Response)),2, 0);
            if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
            {
                BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for sync task queue!"));
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
            hRap->sMemInfo.sTskMemInfo[i].sTskQueue[j].sTskSyncQueue.uiBaseAddr = ui32BaseAddr;
            hRap->sMemInfo.sTskMemInfo[i].sTskQueue[j].sTskSyncQueue.uiMsgQueueSize = BRAP_MAX_MSGS_PER_QUEUE * sizeof(BRAP_FWIF_P_Response);
            hRap->sMemInfo.sTskMemInfo[i].sTskQueue[j].sTskSyncQueue.ui32FifoId = BRAP_DSP_P_FIFO_INVALID;

            /* Async Queue */
            ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, (BRAP_MAX_ASYNC_MSGS_PER_QUEUE * sizeof(BRAP_FWIF_P_AsynEventMsg)),2, 0);
            if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
            {
                BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for async task queue!"));
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
            hRap->sMemInfo.sTskMemInfo[i].sTskQueue[j].sTskAsyncQueue.uiBaseAddr = ui32BaseAddr;
            hRap->sMemInfo.sTskMemInfo[i].sTskQueue[j].sTskAsyncQueue.uiMsgQueueSize = BRAP_MAX_ASYNC_MSGS_PER_QUEUE * sizeof(BRAP_FWIF_P_AsynEventMsg);
            hRap->sMemInfo.sTskMemInfo[i].sTskQueue[j].sTskAsyncQueue.ui32FifoId = BRAP_DSP_P_FIFO_INVALID;

            /* Async MSG Buf Memory */
            ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, (BRAP_MAX_ASYNC_MSGS_PER_QUEUE * sizeof(BRAP_FWIF_P_AsynEventMsg)),2, 0);
            if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
            {
                BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for async msgs!"));
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
            hRap->sMemInfo.sTskMemInfo[i].sTskQueue[j].sAsyncMsgBufmem.ui32BaseAddr = ui32BaseAddr;
            hRap->sMemInfo.sTskMemInfo[i].sTskQueue[j].sAsyncMsgBufmem.ui32Size = BRAP_MAX_ASYNC_MSGS_PER_QUEUE * sizeof(BRAP_FWIF_P_AsynEventMsg);

            /* Initialize task id to invalid */
            hRap->sMemInfo.sTskMemInfo[i].sTskQueue[j].ui32TaskId = BRAP_FWIF_P_INVALID_TSK_ID;

            /* Start Task memory */
            ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, sizeof(BRAP_FWIF_P_TaskParamInfo),2, 0);
            if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
            {
                BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for start task params!"));
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
            hRap->sMemInfo.sTskMemInfo[i].sTskInfo[j].ui32BaseAddr = ui32BaseAddr;
            hRap->sMemInfo.sTskMemInfo[i].sTskInfo[j].ui32Size = sizeof(BRAP_FWIF_P_TaskParamInfo);

            /* Cit memory */
            ui32BaseAddr = (uint32_t)BRAP_P_AllocAligned(hRap, sizeof(BRAP_AF_P_sTASK_CONFIG),2, 0);
            if((void *) BRAP_P_INVALID_DRAM_ADDRESS== (void *)ui32BaseAddr)
            {
                BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for cit!"));
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
            hRap->sMemInfo.sTskMemInfo[i].sCitStruct[j].ui32BaseAddr = ui32BaseAddr;
            hRap->sMemInfo.sTskMemInfo[i].sCitStruct[j].ui32Size = sizeof(BRAP_AF_P_sTASK_CONFIG);
        }
    }
    
    /* Allocate buffer required for on-the-fly programming of configuration parameters */
    ui32BaseAddr = (uint32_t) BRAP_P_AllocAligned(hRap, hRap->sMemReq.ui32ConfigBufReq, 2, 0);
    if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
    {
        BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for extra config buffer!"));
        return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    }
    
    hRap->sMemInfo.sConfigParamBufInfo.ui32BaseAddr = ui32BaseAddr;
    hRap->sMemInfo.sConfigParamBufInfo.ui32Size = hRap->sMemReq.ui32ConfigBufReq;

    /* Allocate buffer required for on-the-fly programming of TSM configuration parameters for each task*/
    /*This memory is allocated per task for TSM configuration, because TSM configuration can be
    set asynchronously for different tasks, So using a single buffer was overwriting the memory*/
    ui32BaseAddr = (uint32_t) BRAP_P_AllocAligned(hRap, sizeof(BRAP_FWIF_P_TsmConfigParams)*BRAP_MAX_FW_TSK_SUPPORTED, 2, 0);
    if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
    {
        BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for extra TSM config buffer!"));
        return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    }

    for(i=0; i<BRAP_MAX_FW_TSK_SUPPORTED ; i++)
    {
        hRap->sMemInfo.sTsmConfigParamBufInfo[i].ui32BaseAddr = ui32BaseAddr +  sizeof(BRAP_FWIF_P_TsmConfigParams)*i;
        hRap->sMemInfo.sTsmConfigParamBufInfo[i].ui32Size = sizeof(BRAP_FWIF_P_TsmConfigParams);
    }

    /* Allocate buffer required for on-the-fly programming of SPDIF  Channel status parameters */
    ui32BaseAddr = (uint32_t) BRAP_P_AllocAligned(hRap, sizeof(BRAP_AF_P_sSPDIF_USER_CFG), 2, 0);
    if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
    {
        BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for SPDIF Channel status config buffer!"));
        return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    }
    
    hRap->sMemInfo.sSpdifStatusBitBufInfo.ui32BaseAddr = ui32BaseAddr;
    hRap->sMemInfo.sSpdifStatusBitBufInfo.ui32Size = sizeof(BRAP_AF_P_sSPDIF_USER_CFG);

#ifdef RAP_VIDEOONDSP_SUPPORT
#ifdef RAP_REALVIDEO_SUPPORT    

/* Real Video Memory allocation*/

    uiTempSize = (BRAP_REALVIDEO_DECODE_LUMAFRAME_BUF_SIZE+BRAP_REALVIDEO_DECODE_CHROMAFRAME_BUF_SIZE)
                    *BRAP_FWMAX_VIDEO_BUFF_AVAIL                     
                    +(BRAP_REALVIDEO_DRAM_MB_BUF_SIZE*BRAP_FWMAX_MB_INFO_AVAIL)
                    +(BRAP_REALVIDEO_DECODE_REF_LUMAFRAME_BUF_SIZE+BRAP_REALVIDEO_DECODE_REF_CHROMAFRAME_BUF_SIZE)
                    *BRAP_FWMAX_VIDEO_REF_BUFF_AVAIL;

    ui32BaseAddr = (uint32_t) BRAP_P_AllocAligned(hRap,uiTempSize, 2, 0);
    if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
    {
        BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for SPDIF Channel status config buffer!"));
        return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    }

    ui32UsedSize =0;
    /*sLumaFrameBuffParams and sChromaFrameBuffParams*/    
    for(i=0; i<BRAP_FWMAX_VIDEO_BUFF_AVAIL ; i++)
    {
        /* Allocate the UPB handle */
        hXDMPicture = (BXDM_Picture *)BRAP_P_AllocAligned(hRap,(sizeof(BXDM_Picture)), 2, 0);
        if(hXDMPicture == NULL)
        {
        	return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }

        /* Reset the entire structure */
        BKNI_Memset(hXDMPicture, 0, sizeof(BXDM_Picture));

        hXDMPicture->stBufferInfo.hHeap = hRap->hHeap;
        hXDMPicture->stBufferInfo.pLumaBufferVirtualAddress = (void *)(ui32BaseAddr + ui32UsedSize);        
        hRap->sMemInfo.sRealVideoBufferInfo.sLumaFrameBuffParams[i].ui32BaseAddr = (uint32_t)(ui32BaseAddr + ui32UsedSize);
        hRap->sMemInfo.sRealVideoBufferInfo.sLumaFrameBuffParams[i].ui32Size  = BRAP_REALVIDEO_DECODE_LUMAFRAME_BUF_SIZE;        
        ui32UsedSize += BRAP_REALVIDEO_DECODE_LUMAFRAME_BUF_SIZE;

        hXDMPicture->stBufferInfo.pChromaBufferVirtualAddress = (void *)(ui32BaseAddr + ui32UsedSize);
        hRap->sMemInfo.sRealVideoBufferInfo.sChromaFrameBuffParams[i].ui32BaseAddr = (uint32_t)(ui32BaseAddr + ui32UsedSize);    
        hRap->sMemInfo.sRealVideoBufferInfo.sChromaFrameBuffParams[i].ui32Size  = BRAP_REALVIDEO_DECODE_CHROMAFRAME_BUF_SIZE;        
        ui32UsedSize += BRAP_REALVIDEO_DECODE_CHROMAFRAME_BUF_SIZE;

        hRap->sMemInfo.sRealVideoBufferInfo.sUPBs[i].ui32BaseAddr = (uint32_t)hXDMPicture;    
        hRap->sMemInfo.sRealVideoBufferInfo.sUPBs[i].ui32Size  = sizeof(BXDM_Picture);

    }

    for(i=0; i<BRAP_FWMAX_VIDEO_REF_BUFF_AVAIL ; i++)
    {
        hRap->sMemInfo.sRealVideoBufferInfo.sLumaReferenceBuffParams[i].ui32BaseAddr = (uint32_t)(ui32BaseAddr + ui32UsedSize);
        hRap->sMemInfo.sRealVideoBufferInfo.sLumaReferenceBuffParams[i].ui32Size  = BRAP_REALVIDEO_DECODE_REF_LUMAFRAME_BUF_SIZE;        
        ui32UsedSize += BRAP_REALVIDEO_DECODE_REF_LUMAFRAME_BUF_SIZE;

        hRap->sMemInfo.sRealVideoBufferInfo.sChromaReferenceBuffParams[i].ui32BaseAddr = (uint32_t)(ui32BaseAddr + ui32UsedSize);    
        hRap->sMemInfo.sRealVideoBufferInfo.sChromaReferenceBuffParams[i].ui32Size  = BRAP_REALVIDEO_DECODE_REF_CHROMAFRAME_BUF_SIZE;        
        ui32UsedSize += BRAP_REALVIDEO_DECODE_REF_CHROMAFRAME_BUF_SIZE;
    }

    /*sDRAMMBInfoStartAdr*/
    for(i=0; i<BRAP_FWMAX_MB_INFO_AVAIL ; i++)
    {
        hRap->sMemInfo.sRealVideoBufferInfo.sDRAMMBInfoStartAdr[i].ui32BaseAddr = ui32BaseAddr + ui32UsedSize;    
        hRap->sMemInfo.sRealVideoBufferInfo.sDRAMMBInfoStartAdr[i].ui32Size  = BRAP_REALVIDEO_DRAM_MB_BUF_SIZE;
        ui32UsedSize+= hRap->sMemInfo.sRealVideoBufferInfo.sDRAMMBInfoStartAdr[i].ui32Size;
    }  

#endif    
#endif   

#ifdef RAP_GFX_SUPPORT
    /* Allocate buffer required for on-the-fly programming of GFX */
    size = sizeof(BRAP_GFX_P_Fill);
    if(size  < sizeof(BRAP_GFX_P_Blit))
        size  = sizeof(BRAP_GFX_P_Blit);
    ui32BaseAddr = (uint32_t) BRAP_P_AllocAligned(hRap, size *BRAP_MAX_GFX_OP_IN_QUEUE, 2, 0);
    if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
    {
        BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Fill/Blit Temp buffer!"));
        return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    }

    for(i=0; i<BRAP_MAX_GFX_OP_IN_QUEUE ; i++)
    {
        hRap->sMemInfo.sGfxfInfo.sGfxBuffer[i].ui32BaseAddr = ui32BaseAddr +  size*i;
        hRap->sMemInfo.sGfxfInfo.sGfxBuffer[i].ui32Size = size;
        hRap->sMemInfo.sGfxfInfo.bFree[i] = true;        
        hRap->sMemInfo.sGfxfInfo.uiGfxOpId[i] = 0;                
    }
#endif
    
    BDBG_LEAVE(BRAP_DSP_AllocMem);
    return err;
}

BERR_Code BRAP_DSP_FreeMem(BRAP_Handle hRap)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int    i=0, j=0;
    BDBG_ENTER(BRAP_DSP_FreeMem);
    BDBG_ASSERT(hRap);

    if(true == hRap->sSettings.bOpenTimeFwDownload)
    {
        BRAP_P_Free(hRap, (void *)hRap->sMemInfo.sOpenTimeMemInfo.ui32BaseAddr);
    }
    else
    {
        BRAP_P_Free(hRap, (void *)hRap->sMemInfo.sDwnldMemInfo.ui32SystemIbootCodePtr);
        BRAP_P_Free(hRap, (void *)hRap->sMemInfo.sDwnldMemInfo.ui32SystemCodePtr);        
        BRAP_P_Free(hRap, (void *)hRap->sMemInfo.sDwnldMemInfo.ui32SystemDataPtr);
        BRAP_P_Free(hRap, (void *)hRap->sMemInfo.sDwnldMemInfo.ui32SystemRdbVariablesPtr);        
#ifdef RAP_SCM_SUPPORT		
        BRAP_P_Free(hRap, (void *)hRap->sMemInfo.sDwnldMemInfo.ui32SystemTableOfHashesPtr);        		
#endif
        BRAP_P_Free(hRap, (void *)hRap->sMemInfo.sDwnldMemInfo.ui32TsmCodePtr);
        BRAP_P_Free(hRap, (void *)hRap->sMemInfo.sDwnldMemInfo.ui32SystemTaskCodePtr);        
        if(hRap->sMemReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size)
        {
            BRAP_P_Free(hRap, (void *)hRap->sMemInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32BaseAddr);
    /*        for(i=0;i<BRAP_DSP_P_MAX_AUD_PROCESSING_CTX;i++)
            {
                BRAP_P_Free(hRap, (void *)hRap->sMemInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[i].ui32BaseAddr);
            }*/
        }
        if(hRap->sMemReq.sDwnldMemInfo.sFsBufInfo[0].ui32Size)
        {
            BRAP_P_Free(hRap, (void *)hRap->sMemInfo.sDwnldMemInfo.sFsBufInfo[0].ui32BaseAddr);        
/*            for(i=0;i<BRAP_DSP_P_MAX_FS_CTXT;i++)
            {
                BRAP_P_Free(hRap, (void *)hRap->sMemInfo.sDwnldMemInfo.sFsBufInfo[i].ui32BaseAddr);
            }*/
        }
        if(hRap->sMemReq.sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size)
        {
            BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sDwnldMemInfo.sDecCodecBufInfo[0].ui32BaseAddr);        
/*            for(i=0;i<BRAP_DSP_P_MAX_DECODE_CTXT;i++)
            {
                BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sDwnldMemInfo.sDecCodecBufInfo[i].ui32BaseAddr);
            }*/
        }
        if(hRap->sMemReq.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size)
        {
            BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sDwnldMemInfo.sPtCodecBufInfo[0].ui32BaseAddr);        
/*            for(i=0;i<BRAP_DSP_P_MAX_PASSTHRU_CTXT;i++)
            {
                BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sDwnldMemInfo.sPtCodecBufInfo[i].ui32BaseAddr);
            }*/
        }
        if(hRap->sMemReq.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size)
        {
            BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32BaseAddr);        
/*            for(i=0;i<BRAP_DSP_P_MAX_ENCODE_CTXT;i++)
            {
                BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sDwnldMemInfo.sEncodeCodecBufInfo[i].ui32BaseAddr);
            }*/
        }
#ifdef RAP_GFX_SUPPORT        
        if(hRap->sMemReq.sDwnldMemInfo.sGfxBufInfo[0].ui32Size)
        {
                BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sDwnldMemInfo.sGfxBufInfo[0].ui32BaseAddr);        
        }
#endif        
#ifdef RAP_SCM_SUPPORT        
	if(hRap->sMemReq.sDwnldMemInfo.sScmBufInfo[0].ui32Size)
	{
                BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sDwnldMemInfo.sScmBufInfo[0].ui32BaseAddr); 	   
	}
#endif        

    }
    /*Free scratch buffer, config buffer */
    for(i=0;i<BRAP_MAX_DSP_SUPPORTED;i++)
    {
        BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sDspScratchInfo[i].ui32BaseAddr);
        BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sCmdQueue[i].uiBaseAddr);
        BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sGenRspQueue[i].uiBaseAddr);
        for(j=0;j<BRAP_MAX_FW_TSK_SUPPORTED;j++)
        {
            BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sTskMemInfo[i].sTskQueue[j].sTskSyncQueue.uiBaseAddr);
            BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sTskMemInfo[i].sTskQueue[j].sTskAsyncQueue.uiBaseAddr);
            BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sTskMemInfo[i].sTskQueue[j].sAsyncMsgBufmem.ui32BaseAddr);
            BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sTskMemInfo[i].sTskInfo[j].ui32BaseAddr);
            BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sTskMemInfo[i].sCitStruct[j].ui32BaseAddr);
            BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sTskMemInfo[i].sTskIFrameCfgBufInfo[j].ui32BaseAddr);            
        }
    }
    
    BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sConfigParamBufInfo.ui32BaseAddr);
    BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sTsmConfigParamBufInfo[0].ui32BaseAddr);    
    BRAP_P_Free(hRap,(void *)    hRap->sMemInfo.sSpdifStatusBitBufInfo.ui32BaseAddr);

#ifdef RAP_VIDEOONDSP_SUPPORT
#ifdef RAP_REALVIDEO_SUPPORT
    /* Real video memory free*/
    for(i=0;i<BRAP_FWMAX_VIDEO_BUFF_AVAIL;i++)
    {

        BRAP_P_Free(hRap,(void *)    hRap->sMemInfo.sRealVideoBufferInfo.sLumaFrameBuffParams[i].ui32BaseAddr);
        BRAP_P_Free(hRap,(void *)    hRap->sMemInfo.sRealVideoBufferInfo.sChromaFrameBuffParams[i].ui32BaseAddr);
    }
#endif
#endif

#ifdef RAP_GFX_SUPPORT    
        BRAP_P_Free(hRap,(void *)hRap->sMemInfo.sGfxfInfo.sGfxBuffer[0].ui32BaseAddr);    
#endif    
    BDBG_LEAVE(BRAP_DSP_FreeMem);        
    return err;
}


