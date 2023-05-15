/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/29 $
 * $brcm_Date: 8/2/12 4:31p $
 *
 * Private code for the data transport porting interface. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/src/core40nm/bxpt_priv.h $
 * 
 * Hydra_Software_Devel/29   8/2/12 4:31p gmullen
 * SW7425-3617: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-3617/1   8/1/12 4:04p gmullen
 * SW7425-3617: PI will access device RAM through cached addresses
 * 
 * Hydra_Software_Devel/28   6/21/12 10:07a gmullen
 * SW7429-115: Fixed bug in saturation code. Use last PHASE_ERROR if PCR
 * error is reported
 * 
 * Hydra_Software_Devel/27   5/8/12 10:15a gmullen
 * SW7429-115: Merged to Hydra
 * 
 * Hydra_Software_Devel/26   4/12/12 9:39a gmullen
 * SW7358-258: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7358-258/1   4/10/12 10:43a gmullen
 * SW7358-258: Ported SW35230-1645 to base2 tree
 * 
 * Hydra_Software_Devel/SW7429-115/1   4/6/12 2:04p gmullen
 * SW7429-115: Implemented workaround
 * 
 * Hydra_Software_Devel/25   4/2/12 2:17p gmullen
 * SW7435-81: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7435-81/1   4/2/12 1:38p gmullen
 * SW7435-81: Shared DRAM for unused XC and RS buffers
 * 
 * Hydra_Software_Devel/24   12/16/11 2:59p gmullen
 * SW7425-1838: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1838/1   12/6/11 5:01p gmullen
 * SW7425-1838: Ported dynamic splicing to base2 files
 * 
 * Hydra_Software_Devel/23   12/2/11 12:13p jtna
 * SW7425-1863: refactor xpt standby power management
 * 
 * Hydra_Software_Devel/22   11/10/11 12:25p jtna
 * SW7425-1672: merge to hydra
 * 
 * Hydra_Software_Devel/SW7425-1672/1   11/9/11 5:51p jtna
 * SW7425-1672: set FORCE_RESYNC flag on descriptors when packetizing ES
 * 
 * Hydra_Software_Devel/21   11/3/11 3:47p gmullen
 * SW7425-1323: Fixed B0/B1 binary compatability issue
 * 
 * Hydra_Software_Devel/20   10/3/11 10:01a gmullen
 * SW7425-1183: Missing WakeupArmed in xpt handle
 * 
 * Hydra_Software_Devel/19   9/30/11 3:10p gmullen
 * SW7425-1183: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1183/1   8/30/11 9:05a gmullen
 * SW7425-1183: Acquire the CLK216 resource when entering standby
 * 
 * Hydra_Software_Devel/18   9/30/11 12:14p jtna
 * SW7425-1266: merge BXPT_Rave_AllocCx() and related changes
 * 
 * Hydra_Software_Devel/17   9/28/11 11:47a gmullen
 * SW7425-1323: Cached RAVE settings in DRAM to reduce overhead
 * 
 * Hydra_Software_Devel/16   9/21/11 10:17a gmullen
 * SW7425-1323: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1323/1   9/21/11 10:02a gmullen
 * SW7425-1323: Potential workaround.
 * 
 * Hydra_Software_Devel/15   9/12/11 4:36p gmullen
 * SW7344-193: Merged changes to Hydra
 * 
 * Hydra_Software_Devel/SW7344-193/1   9/7/11 4:34p gmullen
 * SW7344-193: Added tri-state logic to jitter API
 * 
 * Hydra_Software_Devel/14   7/27/11 3:59p gmullen
 * SW7425-1016: Ported changes to 40nm chips
 * 
 * Hydra_Software_Devel/SW7425-1016/1   7/27/11 3:08p gmullen
 * SW7425-1016: Ported fix to 40nm chips
 * 
 * Hydra_Software_Devel/13   7/13/11 10:04a jtna
 * SW7231-296: initial non-sram pm work
 * 
 * Hydra_Software_Devel/12   6/27/11 2:39p gmullen
 * SW7231-186: Merged power management code to Hydra branch
 * 
 * Hydra_Software_Devel/SW7231-186/1   6/13/11 5:59p gmullen
 * SW7231-186: Implemented SRAM save and restore for XPT standby/resume
 * 
 * Hydra_Software_Devel/11   6/3/11 4:34p gmullen
 * SW7425-653: Merged changes to Hydra branch
 * 
 * Hydra_Software_Devel/SW7425-653/1   6/2/11 10:35a gmullen
 * SW7425-653: Added non-realtime transcoding support
 * 
 * Hydra_Software_Devel/10   5/16/11 5:08p gmullen
 * SW7408-284: Added support for jitter adjust in PCR hw to PI
 * 
 * Hydra_Software_Devel/9   5/12/11 4:59p gmullen
 * SW7231-128: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7231-128/1   5/10/11 1:40p gmohile
 * SW7231-128 : Add power management support
 * 
 * Hydra_Software_Devel/8   4/15/11 4:49p gmullen
 * SW7425-313: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7425-313/1   4/13/11 2:05p gmullen
 * SW7425-313: Added parser remapping support
 * 
 * Hydra_Software_Devel/7   4/11/11 9:15a gmullen
 * SW7346-119: Merged fix to mainline
 * 
 * Hydra_Software_Devel/SW7346-119/1   4/7/11 3:43p gmullen
 * SW7346-119: Disable CC checking when enabling all-pass mode. Restore
 * when exiting all-pass
 * 
 * Hydra_Software_Devel/6   3/11/11 1:07p gmullen
 * SW7346-112: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7346-112/1   3/11/11 9:39a gmullen
 * SW7346-112: Recalc blockout when packet length is changed
 * 
 * Hydra_Software_Devel/5   1/17/11 8:54p gmullen
 * SW7422-115: Removed SvcModeEn from BXPT_Rave_RecordSettings struct and
 * add SvcMvcEn to BXPT_Rave_IndexerSettings
 * 
 * Hydra_Software_Devel/SW7422-115/1   1/10/11 1:31p gmullen
 * SW7422-115: Removed SvcModeEn from BXPT_Rave_RecordSettings struct and
 * add  SvcMvcEn to BXPT_Rave_IndexerSettings.
 * 
 * Hydra_Software_Devel/4   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 * Hydra_Software_Devel/3   10/27/10 1:20p gmullen
 * SW7422-20: Ported to 7422
 * 
 * Hydra_Software_Devel/2   10/25/10 5:32p gmullen
 * SW7422-20: Changed _P_MAX defines to _NUM
 * 
 * Hydra_Software_Devel/1   10/25/10 2:09p gmullen
 * SW7425-15: Moved srcs to base2 folder
 * 
 * Hydra_Software_Devel/1   10/8/10 2:58p gmullen
 * SW7425-15: Added header files
 * 
 * Hydra_Software_Devel/57   7/7/10 11:37a gmullen
 * SW7401-4402: Fixed compilation error for 734x
 * 
 * Hydra_Software_Devel/56   7/4/10 5:33p gmullen
 * SW7630-81: Added support for QUICK
 * 
 * Hydra_Software_Devel/55   4/23/10 11:21a gmullen
 * SW7405-4254: Route PCR PID channel to RAVE only when stream PCRs are
 * used.
 * 
 * Hydra_Software_Devel/54   11/17/09 3:07p gmullen
 * SW7408-9: Finished adding 7408 support to XPT
 * 
 * Hydra_Software_Devel/53   11/2/09 10:53a gmullen
 * SW3548-2589: Use reference counting when deciding to disable PID
 * channels
 * 
 * Hydra_Software_Devel/52   10/1/09 11:12a gmullen
 * SW7405-3102: ResetPacing in Setting struct incorrectly set if pacing is
 * enabled
 * 
 * Hydra_Software_Devel/51   9/23/09 2:26p gmullen
 * SW7405-2994: Merged fix to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/SW7405-2994/1   9/8/09 6:56p gmullen
 * SW7405-2994: Stored MPEG mode setting for CONT_COUNT_IGNORE in module
 * handle
 * 
 * Hydra_Software_Devel/50   9/2/09 2:13p piyushg
 * SW3548-2363: restore SC_OR_MODE for cleaner scrambling detection.
 * Currently avaibale in 3549B1 and higher chips. Enable in others as
 * and when RDB is updated.
 * 
 * Hydra_Software_Devel/49   9/1/09 9:04a gmullen
 * SW3548-2368: Merged fix to mainline
 * 
 * Hydra_Software_Devel/SW3548-2368/1   8/23/09 4:09p gmullen
 * PRSW3548-2368: Created new context type for VCT handling
 * 
 * Hydra_Software_Devel/48   7/31/09 2:44p gmullen
 * PR54331: Added 35130 to XPT support.
 * 
 * Hydra_Software_Devel/47   7/14/09 10:22a piyushg
 * PR56771: Add XPT PI code for 7342.
 * 
 * Hydra_Software_Devel/46   7/13/09 10:01a gmullen
 * PR56760: Added dummy descriptor and wait until dummy desc is consumed.
 * 
 * Hydra_Software_Devel/45   6/17/09 6:46p gmullen
 * PR56110: Added support.xpt/7550/uif_image/a0
 * 
 * Hydra_Software_Devel/44   6/2/09 10:29a gmullen
 * PR51821: Merged workaround to mainline.
 * 
 * Hydra_Software_Devel/PR51821/1   5/26/09 8:02p gmullen
 * PR51821: Added workarounds and warning mesg.
 * 
 * Hydra_Software_Devel/43   4/7/09 5:25p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/42   1/27/09 1:10p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/41   1/9/09 3:41p gmohile
 * PR 50861 : Add detection of DivX sequence header in s/w rave
 * 
 * Hydra_Software_Devel/40   12/19/08 1:59p yili
 * PR42660:IPTV integration
 * 
 * Hydra_Software_Devel/39   12/15/08 2:36p gmullen
 * PR48908: Removed power management code from XPT PI.
 * 
 * Hydra_Software_Devel/38   12/4/08 7:40p vishk
 * PR 30494: Application does not remove P-skip frames from a few Divx avi
 * streams.
 * PR 44712: CBD data missing when ITB size is large (and when softrave is
 * used).
 * 
 * Hydra_Software_Devel/37   11/26/08 4:15p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/36   10/28/08 4:19p gmullen
 * PR46544: Added power management support.
 * 
 * Hydra_Software_Devel/35   8/18/08 2:57p gmullen
 * PR45362: Number of playback channels that support muxing should be
 * programmable.
 * 
 * Hydra_Software_Devel/34   7/16/08 3:44p gmullen
 * PR37867: Merged playback mux code to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/33   6/16/08 11:56a piyushg
 * PR39234: Change some comments and API names for setting up
 * multiple message buffers for same PID channel.
 * 
 * Hydra_Software_Devel/32   5/30/08 7:25p gmullen
 * PR43123: Fixed ITB alignment bug in soft RAVE config.
 * 
 * Hydra_Software_Devel/31   5/7/08 10:47a gmullen
 * PR42443: Removed call to change band rates when changing parser mode.
 * 
 * Hydra_Software_Devel/30   3/26/08 11:24a gmullen
 * PR38954: Added 3548 support to XPT PI.
 * 
 * Hydra_Software_Devel/29   3/17/08 1:03p gmullen
 * PR40675: Added support to route all mesg filter data on R-pipe.
 * 
 * Hydra_Software_Devel/28   3/13/08 6:43p qcheng
 * PR40402: Add ICAM support to BCM7325 A0
 * 
 * Hydra_Software_Devel/27   3/7/08 4:54p gmullen
 * PR38618: Added 7118C0 support
 * 
 * Hydra_Software_Devel/26   3/6/08 3:47p piyushg
 * PR39234: Initial checkin for API's to support PID Duplication.
 * 
 * Hydra_Software_Devel/25   2/28/08 11:24a gmullen
 * PR37893: Ported soft RAVE to XPT PI.
 * 
 * Hydra_Software_Devel/24   1/23/08 10:08a gmullen
 * PR38884: Updated for B0
 * 
 * Hydra_Software_Devel/23   12/19/07 9:01a gmullen
 * PR38274: Updated PI for final version of D0 RDB.
 * 
 * Hydra_Software_Devel/SanJose_CDI_Devel/2   12/17/07 6:43p shuang
 * PR37867:Merge Jethead patch in order to support DirecTV AM21 project
 * which ATSC data will input through USB interface.
 * Merge Magnum Phase 7.0.
 * 
 * Hydra_Software_Devel/22   11/29/07 3:57p gmullen
 * PR37062: Removed BAVC_ItbEsType_eDvdMpeg2Video support.
 * BXPT_Rave_AvSettings.VobMode is now used to indicate VOB streams for
 * 7401.
 * 
 * Hydra_Software_Devel/21   11/28/07 11:39a gmullen
 * PR36900: Added 7335 support
 * 
 * Hydra_Software_Devel/20   10/30/07 3:11p gmullen
 * PR35018: Added support for 7325
 * 
 * Hydra_Software_Devel/19   10/2/07 2:06p katrep
 * PR35544: Fixed SW RAVE memory leak in XPT.Now XPT tracks the memory
 * allocated by it.
 * 
 * Hydra_Software_Devel/18   7/19/07 5:06p gmullen
 * PR32771: Changed wraparound threshold computation for IP contexts.
 * 
 * Hydra_Software_Devel/17   7/13/07 4:09p piyushg
 * PR32218: PACING_START bit is reset whenever
 * TS_RANGE_ERROR interrupt occurs.
 * 
 * Hydra_Software_Devel/16   5/17/07 2:17p gmullen
 * PR30877: Added support for dedicated playback heap handle.
 * 
 * Hydra_Software_Devel/15   4/27/07 8:44a katrep
 * PR29959: Added support in RAVE to change the CDB context end pointer
 * required for DSS,MPEG,TimeStamp support
 * 
 * Hydra_Software_Devel/14   3/22/07 11:52a gmullen
 * PR28909: Added Transcoding bool to struct BXPT_Rave_AvSettings
 * 
 * Hydra_Software_Devel/13   2/14/07 1:10p gmullen
 * PR27642: Added support for 7405.
 * 
 * Hydra_Software_Devel/12   2/6/07 7:05p gmullen
 * PR26217: Added B0 support.
 * 
 * Hydra_Software_Devel/11   11/27/06 4:24p katrep
 * PR25431: Added code to detect the lost data ready interrupt and
 * generate sw callback
 * 
 * Hydra_Software_Devel/10   11/22/06 4:58p gmullen
 * PR26109: Updated for 7403.
 * 
 * Hydra_Software_Devel/9   11/2/06 4:31p gmullen
 * PR25402: Added support for secure heap to RS and XC buffer code.
 * 
 * Hydra_Software_Devel/8   8/17/06 6:01p katrep
 * PR23114: Added Support for 7440 chip
 * 
 * Hydra_Software_Devel/7   8/4/06 2:11a btan
 * PR22187: Move hsm out from xpt.
 * 
 * Hydra_Software_Devel/6   7/24/06 5:04p gmullen
 * PR20082: Added support for PIC counters.
 * 
 * Hydra_Software_Devel/5   7/13/06 11:37a katrep
 * PR20316: Added support to set/unset PSI setting per pid channel basis
 * as it's supported by new HW arch.PR20315:Also added support for 32
 * byte filters.Default filter size is 16 bytes.32 Bytes filters can be
 * enabled at compile time by -DBXPT_FILTER_32
 * 
 * Hydra_Software_Devel/4   3/31/06 11:30a gmullen
 * PR20565: Added UseHostPcrs to settings struct.
 * 
 * Hydra_Software_Devel/3   3/16/06 5:10p gmullen
 * PR20218: PES SYNC mode not initialized. Fixed and tested.
 * 
 * Hydra_Software_Devel/2   2/1/06 10:19a gmullen
 * PR18998: Fixed overflow issue in RAVE ITB/CDB, added support for PB
 * channels.
 * 
 * Hydra_Software_Devel/12   1/9/06 11:24a qcheng
 * PR18909: Add ICAM and keyladder support for 97401
 * 
 * Hydra_Software_Devel/11   1/6/06 2:22p gmullen
 * PR18489: Added PID destination workaround for hardware PR 18919.
 * 
 * Hydra_Software_Devel/9   10/17/05 10:35a gmullen
 * PR15309: Added TPIT support, BandHoldEn.
 * 
 * Hydra_Software_Devel/8   10/6/05 11:23a gmullen
 * PR15309: Fixed audio support.
 * 
 * Hydra_Software_Devel/7   9/21/05 2:17p gmullen
 * PR15309: Added support for AAC HE and AC3+ audio, fixed bug in playback
 * PI, modified RAVE PI to support channel change and reset.
 * 
 * Hydra_Software_Devel/6   8/18/05 9:59a gmullen
 * PR15309: Added more DirecTV support.
 * 
 * Hydra_Software_Devel/5   8/12/05 8:57a gmullen
 * PR15309: Added PCR, PCR Offset, DirecTV, RAVE video, and ITB/CDB
 * endianess support. Tested same.
 * 
 * Hydra_Software_Devel/4   8/4/05 8:14a gmullen
 * PR15309: Added PCR support.
 * 
 * Hydra_Software_Devel/3   7/27/05 6:33p gmullen
 * PR15309: Fixed bug in CheckBuffer() and UpdateReadOffet(). Also updated
 * register configs.
 * 
 * Hydra_Software_Devel/2   7/26/05 5:48p gmullen
 * PR15309: Fixed buffer size issue in bxpt_rave.c and added buffer
 * shutdown code to other blocks.
 * 
 * Hydra_Software_Devel/1   7/15/05 9:03a gmullen
 * PR15309: Inita i
 * 
 ***************************************************************************/

#ifndef BXPT_PRIV_H__
#define BXPT_PRIV_H__

#include "breg_mem.h"
#include "bint.h"
#include "bmem.h"
#include "bavc.h"
#include "bchp.h"
#include "berr_ids.h"
#include "bxpt.h"
#include "bavc.h"
#include "bxpt_rave.h"
#include "bxpt_pcr.h"

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
    #ifdef BCHP_PWR_RESOURCE_XPT_SRAM
    #include "bchp_xpt_fe.h"
    #include "bchp_xpt_msg.h"
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
/* Address delta (in bytes) between elements in a register array. */
#define PARSER_REG_STEPSIZE		( BCHP_XPT_FE_MINI_PID_PARSER1_CTRL1 - BCHP_XPT_FE_MINI_PID_PARSER0_CTRL1 )
#define IB_REG_STEPSIZE 		( BCHP_XPT_FE_IB1_CTRL - BCHP_XPT_FE_IB0_CTRL )
#define BP_TABLE_STEP                   ( 4 )
#define RP_TABLE_STEP                   ( 4 )
#define PID_TABLE_STEP                  ( 4 )
#define PID_CTRL1_TABLE_STEP            ( 4 )
#define PID_CTRL2_TABLE_STEP            ( 4 )
#define FILTER_WORD_STEP                ( 32 )
#define GEN_FILT_EN_STEP                ( 4 )
#define PID2BUF_TABLE_STEP              ( 4 )
#define XPT_REG_SIZE_BITS               ( 32 )
#define XPT_REG_SIZE_BYTES              ( 4 )

/***************************************************************************
Summary:
Used to define the flags inside a linked list descriptor. 
****************************************************************************/
#define TRANS_DESC_INT_FLAG             ( 1ul << 31 )
#define TRANS_DESC_FORCE_RESYNC_FLAG    ( 1ul << 30 )
#define TRANS_DESC_LAST_DESCR_IND       ( 1ul << 0 )

/***************************************************************************
Summary:
Used to maintain a list of which message buffers have been allocated by
porting interface, rather than the caller. 
****************************************************************************/

typedef struct 
{
    bool IsAllocated;   /* true if PI allocated the memory. */
    uint32_t Address;   /* address of the buffer */
    unsigned OutputMode;    /* Type of filtering this buffer is doing */
}
MessageBufferEntry; 

/***************************************************************************
Summary:
Used to maintain a list of which PID channels are allocated and what parser
band and PID are assigned to that channel.
****************************************************************************/

typedef struct
{
    bool IsAllocated;               /* true if allocated. */
    bool IsPidChannelConfigured;    /* has the Pid and Band values been set. */
    bool HasMessageBuffer;          /* true if hardware message buffer is associated with this channel. */
    unsigned int Pid;       
    unsigned int Band;

    bool EnableRequested;           /* Has a request to enable this PID channel been recieved? */
#ifdef ENABLE_PLAYBACK_MUX
	bool MuxEnable;
	bool HasDestination;
#endif /*ENABLE_PLAYBACK_MUX*/

    uint32_t MessageBuffercount;

#if BXPT_SW7425_1323_WORKAROUND
	bool IsPb;
	bool IsEnabled;
#endif
}
PidChannelTableEntry;

 
/***************************************************************************
Summary:
Used to keep track of which PSI filters have been allocated, and to what
PID.
****************************************************************************/
typedef struct
{ 
    bool IsAllocated;       /* Is someone using this? */
}
FilterTableEntry;

/***************************************************************************
Summary:
Handle for accessing the record API via a channel. Users should not directly
access the contents of the structure.
****************************************************************************/

typedef struct BXPT_P_PbHandle
{
    BCHP_Handle hChip;              /* [Input] Handle to used chip. */
    BREG_Handle hRegister;          /* [Input] Handle to access regiters. */
    BMEM_Handle hMemory;            /* [Input] Handle to memory heap to use. */
    void *vhXpt;                    /* Pointer to parent transport handle. Requires casting. */

    uint8_t ChannelNo;      /* This channels number. */
    uint32_t BaseAddr;  /* Address of the first register in this block. */
    uint32_t LastDescriptor_Cached;    /* Address of the last descriptor on the linked list. */
    bool Opened;
    bool Running;
    unsigned int SyncMode;
    BINT_CallbackHandle hPbIntCallback; /* Callback Handle to service the ISR */
    bool AlwaysResumeFromLastDescriptor;
    bool ForceResync;
    
    BXPT_PvrDescriptor *DummyDescriptor;
    uint8_t *DummyBuffer;
    bool ResetPacingOnTsRangeError;
    uint32_t BitRate;

    BXPT_PidChannel_CC_Config CcConfigBeforeAllPass;
    bool IsParserInAllPass;

#if BXPT_HAS_TSMUX
    unsigned PacingLoadMap;
    unsigned PacingCount;
    bool PacingLoadNeeded;
#endif
}
BXPT_P_PbHandle;

/***************************************************************************
Summary:
Handle for accessing the remux API via a channel. Users should not directly
access the contents of the structure.
****************************************************************************/

typedef struct BXPT_P_RemuxHandle
{
    void *vhXpt;            /* Pointer to parent transport handle. Requires casting. */

    BCHP_Handle hChip;              /* [Input] Handle to used chip. */
    BREG_Handle hRegister;          /* [Input] Handle to access regiters. */
    BMEM_Handle hMemory;            /* [Input] Handle to memory heap to use. */
    
    bool Opened;
    bool Running;

    uint8_t ChannelNo;      /* This channels number. */
    uint32_t BaseAddr;  /* Address of the first register in this block. */
    bool BufferIsAllocated;    
    void *BufferAddress;
}
BXPT_P_RemuxHandle;

/***************************************************************************
Summary:
Handle for accessing the packet substution API via a channel. Users should 
not directly access the contents of the structure.
****************************************************************************/

typedef struct BXPT_P_PacketSubHandle
{
    BCHP_Handle hChip;              /* [Input] Handle to used chip. */
    BREG_Handle hRegister;          /* [Input] Handle to access regiters. */
    BMEM_Handle hMemory;            /* [Input] Handle to memory heap to use. */
    void *vhXpt;

    uint8_t ChannelNo;      /* This channels number. */
    uint32_t BaseAddr;      /* Address of the first register in this block. */
    void *LastDescriptor_Cached;    /* Address of the last descriptor on the linked list. */
    bool Opened;
    bool Running;
}
BXPT_P_PacketSubHandle;

typedef struct BXPT_P_PcrHandle_Impl
{
    BCHP_Handle hChip;              /* [Input] Handle to used chip. */
    BREG_Handle hRegister;          /* [Input] Handle to access regiters. */
    void *vhXpt;

    uint8_t ChannelNo;              /* This channels number. */
    uint32_t RegOffset;            /* reg Offset of the given module. */
    bool DMode;
#if BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND
	uint8_t FiltB;			/* loop gain. */	
	uint8_t FiltC;			/* direct path gain. */	
	uint8_t TrackRange;
	int64_t Accum;
	bool InErrState;
    unsigned PcrCount;
    int64_t PcrThreshold;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	bool ChangingToNonPcrSource;
	long PhaseErrorDelay;
	unsigned PcrInc;
#endif
#endif
}
BXPT_P_PcrHandle_Impl;

typedef struct BXPT_P_InterruptCallbackArgs
{
    BINT_CallbackFunc   Callback;   /* Callback for this interrupt. */
    void *Parm1;                    /* User arg for the callback. */
    int Parm2;                      /* User arg for the callback. */
}
BXPT_P_InterruptCallbackArgs;

typedef struct ParserConfig
{
    bool SaveShortPsiMsg;       
    bool SaveLongPsiMsg;        
    bool PsfCrcDis;             
    BXPT_PsiMesgModModes PsiMsgMod;
}
ParserConfig;

/***************************************************************************
Summary:
Handles for accessing and controlling the RAVE and it's contexts.
****************************************************************************/

typedef struct BXPT_P_IndexerHandle 
{
    BXPT_RaveIdx ChannelType;
    void *hChannel;
    bool Allocated;
    unsigned NumChannels;
    void *vhCtx;
    bool SvcMvcMode;
}
BXPT_P_IndexerHandle;

typedef struct StartCodeIndexer
{
    BREG_Handle hReg;           
    BMEM_Handle hMem;           
    BINT_Handle hInt;
    uint32_t BaseAddr;
    unsigned ChannelNo;
    bool Allocated;
}
StartCodeIndexer;

typedef enum brave_itb_types {
    brave_itb_video_start_code =     0x00,   
    brave_itb_base_address =         0x20,
    brave_itb_pts_dts =              0x21,
    brave_itb_pcr_offset =           0x22,  
    brave_itb_btp =                  0x23,
    brave_itb_private_hdr =          0x24,
    brave_itb_rts =                  0x25,
    brave_itb_pcr =                  0x26,
    brave_itb_ip_stream_out =        0x30,
    brave_itb_termination =          0x70
} brave_itb_types;

typedef enum eDynamic_splice_btp_marker_commands
{
    brave_itb_splice_pts_marker = 0x0B , 
    brave_itb_splice_start_marker = 0x12,
    brave_itb_splice_stop_marker = 0x13,
    brave_itb_splice_transition_marker = 0x14,
    brave_itb_splice_pcr_offset_marker = 0x15
}
eDynamic_splice_btp_marker_command;

typedef enum eDynamic_splice_state
{
	SoftRave_SpliceState_Copy = 0,
	SoftRave_SpliceState_Discard = 1
}
eDynamic_splice_state;

typedef struct SoftRaveData
{
    size_t ItbSize;
    uint32_t SrcBaseAddr;
    unsigned mode;
    uint32_t last_base_address;
    uint8_t *src_itb_mem, *dest_itb_mem;
    uint32_t last_src_itb_valid, last_dst_itb_valid;
    uint32_t src_itb_base, dest_itb_base;
	
    uint32_t last_pts_dts;
    uint32_t last_sc;

	
    bool b_frame_found;
    uint32_t last_dest_valid, flush_cnt;
    uint32_t* P_frame_pts;
    brave_itb_types last_entry_type;
	
    bool adjust_pts;
    bool insufficient_itb_info;
    bool sequence_hdr_found;
    bool prev_sequence_hdr_found;
	
    bool discard_till_next_gop;
    bool discarding;

    uint32_t splice_start_PTS;
    uint32_t splice_start_PTS_tolerance;		
    uint32_t splice_stop_PTS;
    uint32_t splice_stop_PTS_tolerance;
    uint32_t splice_monitor_PTS;
    uint32_t splice_monitor_PTS_tolerance;
    uint32_t splice_pcr_offset;
    uint32_t splice_pts_offset;
    uint32_t splice_last_pcr_offset;
    bool  SpliceStartPTSFlag;
    bool  SpliceStopPTSFlag;
    bool  SpliceMonitorPTSFlag;
    bool  SpliceModifyPCROffsetFlag;
    bool  SpliceModifyPTSFlag;
    eDynamic_splice_state splice_state;
    uint32_t Splice_Ctx_num;
    void (*SpliceStopPTSCB) ( void *, uint32_t pts);
    void (*SpliceStartPTSCB) ( void *,uint32_t pts);
    void (*SpliceMonitorPTSCB) ( void *, uint32_t pts);
    void * SpliceStopPTSCBParam;
    void * SpliceStartPTSCBParam;
    void * SpliceMonitorPTSCBParam;
    bool InsertStartPts;
    bool InsertStopPts;
    bool StartMarkerInserted;
    bool StopMarkerInserted;

    unsigned SrcContextIndex;
    bool SrcIsHeld;
}
SoftRaveData;

typedef struct BXPT_P_ContextHandle
{
    BREG_Handle hReg;   
    BCHP_Handle hChp;
    BMEM_Handle hMem;           
    BINT_Handle hInt;
    BMEM_Handle hRHeap;
    void *vhRave;

    unsigned Type;
    uint32_t BaseAddr;
    bool Allocated;
    unsigned Index; 
    bool CdbReset;  
    bool ItbReset;  
    bool CdbReadReset;  
    bool ItbReadReset;  

    bool HaveSpliceQueue;
    unsigned SpliceQueueIdx;

    BAVC_StreamType InputFormat;
    BAVC_ItbEsType ItbFormat;
    StartCodeIndexer *hAvScd;
    int PicCounter;     /* Offset into table of Pic Counter registers */
    bool Transcoding;
    uint32_t allocatedCdbBufferSize; /* Total CDB buffer allocated by AllocContext */ 
    uint32_t usedCdbBufferSize;      /* actual CDB buffer used */ 
    void *CdbBufferAddr;             /* save the cdb allocated bufffer adddress */ 
    void *ItbBufferAddr;             /* save the itb allocated buffer address allocated */ 
    bool externalCdbAlloc;
    bool externalItbAlloc;

    /* For streaming IP support only. Don't use in AV or REC processing */
    bool IsMpegTs;      /* True if context is handling MPEG TS, false if DirecTV */

	/* For 7401 only */
	bool VobMode;

	bool IsSoftContext;
	SoftRaveData SoftRave;	

    StartCodeIndexer *hVctScd;
	void *VctNeighbor;

	/* PR57627 :
    ** SC_OR_MODE is used to select the way scramble control bits are reported.
    ** 0 = Disable OR-ing of current and previous scramble control bits (Default).
    ** 1 = Enable OR-ing of current and previous scramble control bits. This is to 
	** support streams which have mixture of scrambled and unscrambled packets within 
	** the same PID. In such case, these PIDs will be treated as scramble PIDs. 
	** By default this is disabled.
	*/
	bool ScOrMode; 

#if BXPT_SW7425_1323_WORKAROUND
	bool BandHoldEn;
	unsigned UpperThreshold;
#endif
}
BXPT_P_ContextHandle;

typedef struct CtxIntData
{
    BLST_S_ENTRY( CtxIntData ) Link;

    BINT_CallbackFunc   Callback;   /* Callback for this interrupt. */
    void *Parm1;                    /* User arg for the callback. */
    int Parm2;                      /* User arg for the callback. */
    uint32_t EnableRegAddr;
    uint32_t StatusRegAddr;
}
CtxIntData;

typedef struct TpitIndexer
{
    BREG_Handle hReg;           
    BMEM_Handle hMem;           
    BINT_Handle hInt;
    uint32_t BaseAddr;
    uint32_t PidTableBaseAddr;
    uint32_t ParseTableBaseAddr;
    unsigned ChannelNo;
    bool Allocated;
}
TpitIndexer;

typedef struct BXPT_P_RaveHandle    
{
    BCHP_Handle hChip;              
    BREG_Handle hReg;           
    BMEM_Handle hMem;           
    BINT_Handle hInt;
    void *lvXpt;
    unsigned ChannelNo;             

    BXPT_P_ContextHandle ContextTbl[ BXPT_NUM_RAVE_CONTEXTS ];
    bool SpliceQueueAllocated[ BXPT_P_NUM_SPLICING_QUEUES ];
    StartCodeIndexer ScdTable[ BXPT_NUM_SCD ];
    TpitIndexer TpitTable[ BXPT_NUM_TPIT ];
    BXPT_P_IndexerHandle IndexerHandles[ BXPT_NUM_SCD + BXPT_NUM_TPIT ];
    RaveChannelOpenCB chanOpenCB;

#if BXPT_SW7425_1323_WORKAROUND
	signed PidChanToContextMap[ BXPT_NUM_PID_CHANNELS ];
	unsigned WatermarkGranularity;
    bool DoWorkaround;
#endif

}
BXPT_P_RaveHandle;

typedef struct RaveChannel
{
    void *Handle;
    bool Allocated;
}
RaveChannel;

typedef struct BXPT_P_PcrOffset_Impl
{
    BCHP_Handle hChip;              
    BREG_Handle hReg;           
    BMEM_Handle hMem;           
    unsigned int ChannelNo;
    uint32_t BaseAddr;
    void *lvXpt;
    uint32_t CurrentTimeBase;
    unsigned PidChannelNum;
    unsigned WhichStc;
    bool UseHostPcrs;
}
BXPT_P_PcrOffset_Impl;

typedef struct PcrOffsetData
{
    void *Handle;
    bool Allocated;
}
PcrOffsetData;

/***************************************************************************
Summary:
The handle for the transport module. Users should not directly access the 
contents.
****************************************************************************/

#define BXPT_P_MINIMUM_BUF_SIZE            (256)

typedef struct BXPT_P_TransportData
{
    BCHP_Handle hChip;              /* [Input] Handle to used chip. */
    BREG_Handle hRegister;          /* [Input] Handle to access regiters. */
    BMEM_Handle hMemory;            /* [Input] Handle to memory heap to use. */
    BINT_Handle hInt;               /* [Input] Handle to interrupt interface to use. */

    /* Handle for secure memory allocations. */
    BMEM_Handle hRHeap;             

    BINT_CallbackHandle hMsgCb;     /* Callback handle for message interrupts */
    BINT_CallbackHandle hMsgOverflowCb; /* Callback handle for message overflow interrupts */

    unsigned int MaxPlaybacks;          /* Number of playback blocks we support. */
    unsigned int MaxPidChannels;        /* Number of PID channels we support. */
    unsigned int MaxPidParsers;         /* Number of PID parsers we support. */
    unsigned int MaxInputBands;         /* Max number of input bands we support. */
    unsigned int MaxFilterBanks;        /* Max number of filter banks */
    unsigned int MaxFiltersPerBank;     /* Number of filters in each bank. */
    unsigned int MaxPcrs;               /* Number of PCR blocks */
    unsigned int MaxPacketSubs;         /* Max number of packet substitution channels. */
    unsigned int MaxTpitPids;           /* Max number of PIDs handled by each TPIT parser */
    unsigned int MaxRaveContexts;       /* Max number of RAVE contexts, AV plus record */
    unsigned int MaxRaveChannels;       /* Max number of RAVE channels (or instances of the RAVE core in XPT). */

#ifdef ENABLE_PLAYBACK_MUX
    unsigned int NumPlaybackMuxes;      /* Number of playback blocks to be used for muxing. */
#endif /* ENABLE_PLAYBACK_MUX */
                                                 
#if BXPT_HAS_IB_PID_PARSERS
    ParserConfig IbParserTable[ BXPT_NUM_PID_PARSERS ];
    bool IsParserInAllPass[ BXPT_NUM_PID_PARSERS ];
#endif

    ParserConfig PbParserTable[ BXPT_NUM_PLAYBACKS ];

#if BXPT_HAS_MESG_BUFFERS
    bool PidChannelParserConfigOverride[BXPT_NUM_MESG_BUFFERS]; /* if set the true then PSI setting for this pid
                                                                     channel can different from the the PSI settings
                                                                     for the PARSER to which this pid channel belongs */

    MessageBufferEntry MessageBufferTable[ BXPT_NUM_MESG_BUFFERS ];   /* Table of PI allocated buffers */
    bool MesgBufferIsInitialized[ BXPT_NUM_MESG_BUFFERS ];            /* true if buffer has been configured. */

    FilterTableEntry FilterTable[ BXPT_NUM_FILTER_BANKS ][ BXPT_P_FILTER_TABLE_SIZE ];

    /* Pointers to the interrupt handlers. */
    BXPT_P_InterruptCallbackArgs MesgIntrCallbacks[ BXPT_NUM_MESG_BUFFERS ];
    BXPT_P_InterruptCallbackArgs OverflowIntrCallbacks[ BXPT_NUM_MESG_BUFFERS ];
#endif

#if BXPT_HAS_REMUX                      
    BXPT_P_RemuxHandle RemuxHandles[ BXPT_NUM_REMULTIPLEXORS ];
#endif

#if BXPT_HAS_PACKETSUB
    BXPT_P_PacketSubHandle PacketSubHandles[ BXPT_NUM_PACKETSUBS ]; 
#endif

    PidChannelTableEntry PidChannelTable[ BXPT_NUM_PID_CHANNELS ];    /* Table of PI allocated PID channels. */

    BXPT_P_PbHandle PlaybackHandles[ BXPT_NUM_PLAYBACKS ];

#if BXPT_HAS_DPCRS
    BXPT_P_PcrHandle_Impl  *PcrHandles[ BXPT_NUM_PCRS ];
#endif

    RaveChannel RaveChannels[ BXPT_NUM_RAVE_CHANNELS ];
    PcrOffsetData PcrOffsets[ BXPT_NUM_PCR_OFFSET_CHANNELS ];

    bool IsLittleEndian;
	bool Pid2BuffMappingOn;
	bool MesgDataOnRPipe;

    BMEM_Handle hPbHeap;

    BXPT_PidChannel_CC_Config CcConfigBeforeAllPass[ BXPT_NUM_PID_CHANNELS ];

#if BXPT_HAS_PARSER_REMAPPING
    BXPT_ParserBandMapping BandMap;
#endif

    /* TODO: consolidate power-related variables into a struct */
    bool bStandby; /* true if in standby */
    bool bS3Standby;
    bool WakeupEnabled; /* Wakeup packet support enabled */

#ifdef BCHP_PWR_RESOURCE_XPT_SRAM
    void *SRAM_Backup;
    void *Reg_Backup;
    void *vhRave;
#endif

#ifdef BCHP_PWR_RESOURCE_XPT
    bool WakeupArmed;
#endif

    unsigned DpcrRefCount;  /* Number of DPCR channels that have been openned */

    BXPT_PCR_JitterTimestampMode JitterTimestamp[ BXPT_NUM_PCRS ]; 
    BXPT_PCR_JitterCorrection PbJitterDisable[ BXPT_NUM_PCRS ];
    BXPT_PCR_JitterCorrection LiveJitterDisable[ BXPT_NUM_PCRS ];

#if BXPT_SW7425_1323_WORKAROUND
    bool DoWorkaround;
#endif

	uint32_t SharedXcRsBufferOffset;
}
BXPT_P_TransportData;

uint32_t BXPT_P_GetParserCtrlRegAddr(
	BXPT_Handle hXpt,				/* [in] Handle for the transport to access. */
    unsigned ParserNum,
    unsigned Reg0
    );

void BXPT_P_ResetTransport( 
    BREG_Handle hReg         
    );

void BXPT_P_Interrupt_MsgVector_isr( 
    BXPT_Handle hXpt,               /* [in] Handle for this transport */
    int L1Shift         /* [in] Dummy arg. Not used by this interface. */
    );

void BXPT_P_Interrupt_MsgOverflowVector_isr( 
    BXPT_Handle hXpt,               /* [in] Handle for this transport */
    int L1Shift         /* [in] Dummy arg. Not used by this interface. */
    );

void BXPT_P_Interrupt_MsgSw_isr(
    BXPT_Handle hXpt,               /* [in] Handle for this transport */
    int MessageBufferNum            /* [in] Message Buffer */
    );

BERR_Code BXPT_P_GetGroupSelect( unsigned int Bank, unsigned int *GenGrpSel );

#if BXPT_HAS_PID_CHANNEL_PES_FILTERING

typedef enum
{
    BXPT_Spid_eChannelFilterMode_Disable = 0,           /* Disable filter mode */
    BXPT_Spid_eChannelFilterMode_StreamId = 8,          /* PB packetizer filters data on stream id */
    BXPT_Spid_eChannelFilterMode_StreamIdRange = 10,    /* PB packetizer filters on hi to lo stream id range */
    BXPT_Spid_eChannelFilterMode_StreamIdExtension = 12,/* PB packetizer filters on stream id and stream id extension */
    BXPT_Spid_eChannelFilterMode_SubStreamId = 14       /* PB packetizer filters on substream id */

}BXPT_Spid_eChannelFilterMode;

typedef struct BXPT_Spid_eChannelFilter 
{
    BXPT_Spid_eChannelFilterMode Mode;  

    union
    {
        unsigned char StreamId;
        struct
        {
            unsigned char Hi;
            unsigned char Lo;
        }StreamIdRange;

        struct
        {
            unsigned char Id;
            unsigned char Extension;
        }StreamIdAndExtension;

        struct
        {
            unsigned char Id;
            unsigned char SubStreamId;
        }StreamIdAndSubStreamId;
    }FilterConfig;
    
}BXPT_Spid_eChannelFilter;

/***************************************************************************
Set the configuration for the given secondary PID channel stream filter
based on various stream id combinations. Used during DVD playback mode. 
****************************************************************************/
BERR_Code BXPT_Spid_P_ConfigureChannelFilter(BXPT_Handle hXpt,unsigned int ChannelNum,BXPT_Spid_eChannelFilter Filter);

/* Enable PID2BUFF support. On by default in this sw base. */
void SetPid2BuffMap(BXPT_Handle hXpt);


#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BXPT_PRIV_H__ */

/* end of file */


