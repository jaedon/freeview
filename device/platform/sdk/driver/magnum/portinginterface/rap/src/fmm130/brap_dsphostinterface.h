/***************************************************************************
*     Copyright (c) 2003-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_dsphostinterface.h $
* $brcm_Revision: Hydra_Software_Devel/34 $
* $brcm_Date: 11/10/09 4:39p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_dsphostinterface.h $
* 
* Hydra_Software_Devel/34   11/10/09 4:39p srajapur
* SW7403-766 : [7400,7401,7403] DRA decoder development on 74xx chip
* --------> Correct MAX downloadable FW modules numbers.
* 
* Hydra_Software_Devel/33   9/10/09 10:12a srajapur
* SW7403-766 : [7400] DRA decoder development on 7403 chip
* --------> Correct MAX downloadable FW modules numbers.
* 
* Hydra_Software_Devel/32   7/21/09 5:27p srajapur
* PR53240 : [7403] DRA decoder development on 7403 chip
* 
* Hydra_Software_Devel/31   4/6/09 4:50p srajapur
* PR 53700 : [7400,7401,7403,7118] Added Downloadable scheduler code for
* code memory optimization
* 
* Hydra_Software_Devel/30   9/16/08 6:07p bhanus
* PR 46417 : [7440] Merging DTS MA Support on main line.
* 
* Hydra_Software_Devel/29   9/16/08 3:16p bhanus
* PR 46417 : [7440] Merging DTS MA Support on main line
* 
* Hydra_Software_Devel/28   9/9/08 3:20p bhanus
* PR 46672 : [7440] Adding Ping Pong mechanism for User Mixing Coeff when
* passed to FW.
* 
* Hydra_Software_Devel/RAP_7440_DTSMA_DEVEL/1   9/12/08 7:03p bhanus
* PR 46417 : [7440] Adding support for Downloadable Scheduler.
* 
* Hydra_Software_Devel/27   8/18/08 3:48p gdata
* PR43971: [7601] Fixing compilation error for 7601
* 
* Hydra_Software_Devel/26   8/8/08 6:32p gdata
* PR43971: [7443] Merging DVD family for 7443 and onward chips
* 
* Hydra_Software_Devel/RAP_DVD_Family/2   7/17/08 9:07a gdata
* PR43971: [7440] Adding changes related to RDB
* 
* Hydra_Software_Devel/RAP_DVD_Family/1   6/23/08 8:08p gdata
* PR43971: [7440] Creating DVD family for 7443 and onward chips
* 
* Hydra_Software_Devel/24   12/20/07 11:32a gdata
* PR38296: [7440] Adding support for DTSHD SUB audio
* 
* Hydra_Software_Devel/23   11/20/07 8:45p sushmit
* PR 36877: [7335] Bringup PI
* 
* Hydra_Software_Devel/22   10/18/07 9:08p gdata
* PR36059: [7440] [HiDef-DVD PR6319] Merging the changes of
* RAP_7440_PR6319_Hidef
* Increasing the size of SIZE_OF_BAF_SPDIF_INFO from 8 to 12
* 
* Hydra_Software_Devel/21   10/1/07 10:29p sushmit
* PR35140: [7325] Develop 7325 Raptor PI for Bringup
* 
* Hydra_Software_Devel/20   9/30/07 2:00p sushmit
* PR35140: [7405] Develop 7325 Raptor PI for Bringup
* 
* Hydra_Software_Devel/19   8/10/07 4:41p speter
* PR 33403: [3563] AAC_HE LOAS/ADTS support merged to main line.
* 
* Hydra_Software_Devel/RAP_3563_AAC_HE_ADTS_LOAS_SUPPORT/1   8/2/07 4:55p speter
* PR 33403: [3563] Making the MIT array size for MAX downloadable modules
* as 30. This is to accomodate the new AAC HE Multichannel algorithms
* with IDs 0x16, 0x17 and 0x18
* 
* Hydra_Software_Devel/17   7/19/07 6:33p bselva
* PR 33176:[7400]Checkin the PI changes for phase 4.0 release
* 
* Hydra_Software_Devel/16   7/12/07 2:42p bhanus
* PR 32768 : [7440 B] Modified code for Secondary Metadata bring up.
* 
* Hydra_Software_Devel/15   6/7/07 7:29p bhanus
* PR 30069 : [7440B0] Added code for Secondary Metadata Processing.
* 
* Hydra_Software_Devel/14   6/7/07 3:26p sushmit
* PR 27646: NewMIT changes for 7405
* 
* Hydra_Software_Devel/13   5/31/07 6:05p bselva
* PR 26501: [7401 Family]Checking in the changes for AAC-HE multichannel
* support
* 
* Hydra_Software_Devel/12   5/11/07 9:01p sushmit
* PR 27646: Getting 7405 PI compiled
* 
* Hydra_Software_Devel/11   5/4/07 4:39p rjain
* PR 30436: [7401 family] First round of check ins for new mit. All
* changes for new MIT are within the macro BRAP_DSP_P_7401_NEWMIT. This
* macro is defined as 0 at present. It will be enabled once new MIT FW
* is also checked in.
* 
* Hydra_Software_Devel/10   3/15/07 2:26a bhanus
* PR 28040 : [7440 B0] Enabling new MIT for 7440 B0
* 
* Hydra_Software_Devel/9   3/7/07 6:52p kagrawal
* PR 27912: [7440A0] Enabling new MIT for 7440 A0
* 
* Hydra_Software_Devel/8   3/1/07 10:10a kagrawal
* PR 27912: New MIT changes
* - Added pEncode[]
* - Added BRAP_DSP_P_NEWMIT
* 
* Hydra_Software_Devel/7   2/9/07 2:55p bhanus
* PR 25607 : [7440] Modified MIT Array Size from 20 to 30
* 
* Hydra_Software_Devel/6   1/12/07 8:31p sushmit
* PR 26864:
* Added new MIT structure for 3563.
* 
* Hydra_Software_Devel/4   12/2/05 3:31p nitinb
* PR 18061: Raptor memory allocation must be less than 0.5M for MPEG/AC-3
* only
* 
* Hydra_Software_Devel/3   7/28/05 11:22p nitinb
* PR 16092: Merging new MIT code into main branch
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/2   6/16/05 10:00a nitinb
* Defined macro NUM_CHANNELS
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/1   6/16/05 9:32a nitinb
* Made changes to support DDP and AAC-HE firmware developement with new
* MIT changes
* 
* DSP_Raptor_Devel/23   6/7/05 3:29p ramanms
* PR 15608: Changing the size of the BAF_HOST_INFO
* 
* DSP_Raptor_Devel/22   5/27/05 10:21p ramanms
* PR 15608: Changing the MIT structure for new scheduler decoder
* interface
* 
* DSP_Raptor_Devel/21   7/1/04 12:00p ramanms
* Adding the Pass thru downloadable in the host info struct
* 
* DSP_Raptor_Devel/20   6/9/04 9:05a aram
* Added ContextInfo data structure
* 
* DSP_Raptor_Devel/19   6/7/04 4:14p ramanms
* SPDIF parser integration
* 
* DSP_Raptor_Devel/18   5/26/04 3:26p kwelch
* intermediate update
* 
* DSP_Raptor_Devel/17   5/14/04 1:24p kwelch
* Updating post processing support
* 
* DSP_Raptor_Devel/16   5/7/04 1:39p kwelch
* Changing Master Index Table
* 
* DSP_Raptor_Devel/15   3/31/04 5:33p kwelch
* intermediate update
* 
* DSP_Raptor_Devel/13   1/29/04 11:28a aram
* made the algorithm to a pointer and added more decode scratch
* 
* DSP_Raptor_Devel/12   1/6/04 10:35a aram
* This header file was reviewed and approved by SD, BLR and Irvine
* 
* DSP_Raptor_Devel/11   12/17/03 2:10p aram
* move the parser and added tables to the algorithm
* 
 Hydra_Software_Devel/1   4/9/03 5:32p vobadm
* merged from aramPrivateBranch
* 
* aramPrivateBranch/3   4/9/03 8:56a aram
* more meat was added
* 
* aramPrivateBranch/2   4/1/03 9:27a aram
* more files was added
* 
* aramPrivateBranch/1   3/24/03 12:29p aram
* genesis of files
***************************************************************************/
 
#ifndef AUDIO_DSP_HOST_INTERFACE_H__
#define AUDIO_DSP_HOST_INTERFACE_H__

#include "brap_dsphostcommon.h"

#if((BCHP_CHIP == 7405)||(BCHP_CHIP == 7325) || (BCHP_CHIP == 7335))
#define BRAP_7405_FAMILY    1
#endif

#if((BCHP_CHIP == 7440) || (BCHP_CHIP == 7601))
#define BRAP_DVD_FAMILY         1
#endif

#if (BCHP_CHIP == 3563)
#define BRAP_DSP_P_3563_NEWMIT  1
#define BRAP_DSP_P_NEWMIT       0
#define BRAP_DSP_P_NEWMIT_RDB   0
#elif ((BRAP_DVD_FAMILY == 1)||(BRAP_7405_FAMILY == 1))
#define BRAP_DSP_P_3563_NEWMIT  0     
#define BRAP_DSP_P_NEWMIT       1
#define BRAP_DSP_P_NEWMIT_RDB   0
#else
#define BRAP_DSP_P_3563_NEWMIT  0     
#define BRAP_DSP_P_NEWMIT       0
#define BRAP_DSP_P_NEWMIT_RDB   0
#endif






typedef  uint32_t  BAF_HostDramAddress;

#define BAF_HOST_MAX_NAME_SIZE   4
#define BAF_MAX_DRAM_POINTERS   10 /* max number of tables used by an algorithm, decode or post processing */
#if (BCHP_CHIP == 7401)||(BCHP_CHIP == 7403)||(BCHP_CHIP == 7118) || (BRAP_7405_FAMILY == 1) || ( BCHP_CHIP == 7400 ) || (BCHP_CHIP == 3563)
    #if (BCHP_CHIP == 7401)||(BCHP_CHIP == 7403)||(BCHP_CHIP == 7118) || ( BCHP_CHIP == 7400 )
    #define BAF_HOST_MAX_DL_MODULE  40 /* Max number of downloadable modules for frame sync, decode or post processing */
    #else
    #define BAF_HOST_MAX_DL_MODULE  30 /* Max number of downloadable modules for frame sync, decode or post processing */
    #endif
#elif (BRAP_DVD_FAMILY == 1)
    #define BAF_HOST_MAX_DL_MODULE  40 /* Max number of downloadable modules for frame sync, decode or post processing */
#else
    #define BAF_HOST_MAX_DL_MODULE  20 /* Max number of downloadable modules for frame sync, decode or post processing */
#endif
#define BAF_MAX_SCRATCH_BUF     16 /* max number of scratch buffers available to an algorithm ( NUM_STAGE_BFR * 2) */
#define NUM_CHANNELS			3

#if ((BRAP_DSP_P_3563_NEWMIT == 0) && (BRAP_DSP_P_NEWMIT == 0) && (BRAP_DSP_P_7401_NEWMIT == 0) )
typedef struct BAF_HostBufInfo
{
    uint32_t                 id;     /* One of AUDIO_DSP_DECODER_xxx or AUDIO_DSP_PP_xxx */
    uint32_t                 size;   /* size */    
    BAF_HostDramAddress      data;   /* data pointer to DRAM address */
}BAF_HostBufInfo;
#define SIZE_OF_BAF_HOST_BUF_INFO (4 + 4 + 4)

typedef struct BAF_Download
{
    BAF_HostBufInfo          sFirmware;                         /* Firmware */
    BAF_HostBufInfo          sTables;  /* Data tables for given algorithm only one pointer to all the tables */    
    BAF_HostBufInfo          sInterStageBuf;  /* Pointer to the interstage buffer that would be used by decode */    
    BAF_HostBufInfo          sInterStageInterfaceBuf;  /* Pointer to Interstage interface that is sued by decoders to pass on config info */
    BAF_HostBufInfo          sDecodeScratchBuf;  /* Pointer to Decoder scratch buffer */    
    
}BAF_Download;
#define SIZE_OF_BAF_DOWNLOAD (   SIZE_OF_BAF_HOST_BUF_INFO * 5 )

typedef struct BAF_HostInfo
{
    uint32_t          id;                /* BAF_HOST_AL_TABLE_ID */
    uint32_t          version;           /* BAF_HOST_AL_VERSION */
    BAF_HostBufInfo   apmScratchBuf;     /* scratch buffer for APM */
    BAF_HostBufInfo	 InterFrameBuf[ NUM_CHANNELS ]; /* Inter frame buffers ( context dependant ) */
    BAF_HostBufInfo   *pFrameSync   [ BAF_HOST_MAX_DL_MODULE ]; /* frame sync module i/f */
    BAF_Download      *pDecode      [ BAF_HOST_MAX_DL_MODULE ]; /* decode module i/f */
    BAF_Download      *pPostProcess [ BAF_HOST_MAX_DL_MODULE ]; /* post process module i/f */
	BAF_Download      *pPassThru    [ BAF_HOST_MAX_DL_MODULE ]; /* Pass Thru i/f */
}BAF_HostInfo;

#define SIZE_OF_BAF_HOST_INFO ( (4 + 4) + ( SIZE_OF_BAF_HOST_BUF_INFO * 1 ) + ( SIZE_OF_BAF_HOST_BUF_INFO * NUM_CHANNELS ) + \
(4 * (BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE) ) )

#else
/* for new mit for 7401, 7440 and 3563 */


#if ((BRAP_DSP_P_7401_NEWMIT==1)||(BRAP_7405_FAMILY == 1))
#define BAF_MAX_BRANCHES    1   
#define BAF_MAX_POSTPROCESS_STAGES  3 
#define BAF_MAX_CONTEXTS    3 
#else
#define BAF_MAX_CONTEXTS    3 /*4*/ /* Changed to 3 as ZSP doesn't have enough memory */
#define BAF_MAX_BRANCHES    3
#define BAF_MAX_POSTPROCESS_STAGES  7
#endif

typedef struct BAF_HostBufInfo
{
    uint32_t                 id;     /* One of AUDIO_DSP_DECODER_xxx or AUDIO_DSP_PP_xxx */
    uint32_t                 size;   /* size */    
    BAF_HostDramAddress      data;   /* data pointer to DRAM address */
}BAF_HostBufInfo;
#define SIZE_OF_BAF_HOST_BUF_INFO (4 + 4 + 4)

typedef struct BAF_Download
{
    BAF_HostBufInfo          sFirmware;                         /* Firmware */
    BAF_HostBufInfo          sTables;  /* Data tables for given algorithm only one pointer to all the tables */    
    BAF_HostBufInfo          sDecodeScratchBuf;  /* Pointer to Decoder scratch buffer */    
}BAF_Download;
#define SIZE_OF_BAF_DOWNLOAD (   SIZE_OF_BAF_HOST_BUF_INFO * 3 )

typedef struct BAF_HostInfo
{
    uint32_t          id;                /* BAF_HOST_AL_TABLE_ID */
    uint32_t          version;           /* BAF_HOST_AL_VERSION */
#if (BRAP_DVD_FAMILY == 1)||(BRAP_DSP_P_7401_NEWMIT==1)
    BAF_HostDramAddress DnldSchedular;   /* Downaloadable Scheduler Address */
    uint32_t            DnldSchedularSize; /* Downaloadable Scheduler Size */
#endif
    BAF_HostBufInfo   apmScratchBuf;     /* scratch buffer for APM */
    BAF_HostBufInfo	  InterFrameBuf[BAF_MAX_CONTEXTS][BAF_MAX_BRANCHES]
                                    [BAF_MAX_POSTPROCESS_STAGES + 1]; 
    /* Inter frame buffers ( context, branch & stage dependant ) */
    BAF_HostBufInfo	  InterStageInputBuf[BAF_MAX_CONTEXTS][BAF_MAX_BRANCHES]
                                        [BAF_MAX_POSTPROCESS_STAGES + 1]; 
    /* Inter Stage Input buffers ( context, branch & stage dependant ) */
    BAF_HostBufInfo	  InterStageOutputBuf[BAF_MAX_CONTEXTS][BAF_MAX_BRANCHES]
                                        [BAF_MAX_POSTPROCESS_STAGES + 1]; 
    /* Inter Stage Output buffers ( context, branch & stage dependant ) */
    BAF_HostBufInfo	  InterStageInterfaceInputBuf[BAF_MAX_CONTEXTS][BAF_MAX_BRANCHES]
                                                [BAF_MAX_POSTPROCESS_STAGES + 1]; 
    /* Inter Stage Interface Input buffers ( context, branch & stage dependant ) */
    BAF_HostBufInfo	  InterStageInterfaceOutputBuf[BAF_MAX_CONTEXTS][BAF_MAX_BRANCHES]
                                                [BAF_MAX_POSTPROCESS_STAGES + 1]; 
    /* Inter Stage Interface Output buffers ( context, branch & stage dependant ) */
    BAF_HostBufInfo   *pFrameSync   [ BAF_HOST_MAX_DL_MODULE ]; /* frame sync module i/f */
    BAF_Download      *pDecode      [ BAF_HOST_MAX_DL_MODULE ]; /* decode module i/f */
    BAF_Download      *pPostProcess [ BAF_HOST_MAX_DL_MODULE ]; /* post process module i/f */
	BAF_Download      *pPassThru    [ BAF_HOST_MAX_DL_MODULE ]; /* Pass Thru i/f */
#if (BRAP_DSP_P_NEWMIT == 1) ||  (BRAP_DSP_P_7401_NEWMIT==1)
    BAF_Download      *pEncode      [ BAF_HOST_MAX_DL_MODULE ]; /* encode module i/f */    
#endif
}BAF_HostInfo;

#if (BRAP_DSP_P_NEWMIT == 1)
#if (BRAP_DVD_FAMILY == 1)
#define SIZE_OF_BAF_HOST_INFO ( (4 + 4 ) + ( 4 + 4) + ( SIZE_OF_BAF_HOST_BUF_INFO * 1 ) + \
    ( SIZE_OF_BAF_HOST_BUF_INFO * 5 * (BAF_MAX_CONTEXTS * BAF_MAX_BRANCHES * (BAF_MAX_POSTPROCESS_STAGES + 1)) ) + \
    (4 * (BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE) ) )

#else
#define SIZE_OF_BAF_HOST_INFO ( (4 + 4 ) + ( SIZE_OF_BAF_HOST_BUF_INFO * 1 ) + \
    ( SIZE_OF_BAF_HOST_BUF_INFO * 5 * (BAF_MAX_CONTEXTS * BAF_MAX_BRANCHES * (BAF_MAX_POSTPROCESS_STAGES + 1)) ) + \
    (4 * (BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE) ) )

#endif
#else
#define SIZE_OF_BAF_HOST_INFO ( (4 + 4 ) + ( 4 + 4) + ( SIZE_OF_BAF_HOST_BUF_INFO * 1 ) + \
    ( SIZE_OF_BAF_HOST_BUF_INFO * 5 * (BAF_MAX_CONTEXTS * BAF_MAX_BRANCHES * (BAF_MAX_POSTPROCESS_STAGES + 1)) ) + \
    (4 * (BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE + BAF_HOST_MAX_DL_MODULE) ) )
#endif
#endif

/* SPDIF information per context */
typedef struct
{
	 uint16_t professional_mode_flag;
	 uint16_t software_copyright_asserted;
	 uint16_t category_code;
	 uint16_t clock_accuracy;  
  
}BAF_SPDIFInfo;
#define  SIZE_OF_BAF_SPDIF_INFO		(12)


/* 
   This data struture define DSSM (Dynamic System Shared Memory) 
   for a DSP context. This data structure can be changed dynamically 
   before/during/after decode.
 */

typedef struct
{

#define 	BAF_UPDATE_SPDIF			0x00000001
	uint32_t				  dirtyBits;
	BAF_SPDIFInfo		  	  sSPDIFParams;
 
}BAF_ContextInfo;

#define SIZE_OF_BAF_CONTEXT_INFO ( 4 + SIZE_OF_SPDIF_INFO )

#if (BRAP_SEC_METADATA_SUPPORT == 1)

typedef struct BRAP_SecMetadataInfo
{
    uint32_t ui32UserCoeff[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS][2][BRAP_RM_P_MAX_OP_CHANNELS]; 
    uint32_t ui32BfRbusAddress[2]; 
    uint32_t ui32DpRbusAddress[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS][2][BRAP_RM_P_MAX_OP_CHANNELS];
    uint32_t ui32NumPrimaryCh;
 
}BRAP_SecMetadataInfo;

#define BRAP_METADATA_INFO_SIZE ( \
    (4 * BRAP_RM_P_MAX_OP_CHANNEL_PAIRS * 2 * BRAP_RM_P_MAX_OP_CHANNELS) + \
    (4 * BRAP_RM_P_MAX_OP_CHANNEL_PAIRS * 2 * BRAP_RM_P_MAX_OP_CHANNELS) + \
    (4 * BRAP_RM_P_MAX_OP_CHANNELS) + \
    (4 * BRAP_RM_P_MAX_OP_CHANNEL_PAIRS * 2 * BRAP_RM_P_MAX_OP_CHANNELS) \
    )
#endif

#endif









