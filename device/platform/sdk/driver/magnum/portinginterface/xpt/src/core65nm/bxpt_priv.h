/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/68 $
 * $brcm_Date: 9/30/11 2:24p $
 *
 * Private code for the data transport porting interface. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/7400/bxpt_priv.h $
 * 
 * Hydra_Software_Devel/68   9/30/11 2:24p jtna
 * SW7425-1266: back-port BXPT_Rave_AllocCx() and related changes to non-
 * 40nm platforms
 * 
 * Hydra_Software_Devel/67   9/13/11 8:53a gmullen
 * SW7408-284: Fixed compilation bug
 * 
 * Hydra_Software_Devel/66   9/12/11 5:03p gmullen
 * SW7408-284: Merged changes to Hydra
 * 
 * Hydra_Software_Devel/SW7408-284/2   9/7/11 4:35p gmullen
 * SW7408-284: Changed jitter settings to an array
 * 
 * Hydra_Software_Devel/SW7408-284/1   9/7/11 1:51p gmullen
 * SW7408-284: Changed jitter adjust API to 'tri-date'
 * 
 * Hydra_Software_Devel/65   7/27/11 3:06p gmullen
 * SW7335-1285: Merged XPT side of the changes to Hydra
 * 
 * Hydra_Software_Devel/SW7335-1285/1   7/12/11 2:19p gmullen
 * SW7335-1285: First pass implementation on XPT/RAVE side
 * 
 * Hydra_Software_Devel/64   5/16/11 4:52p gmullen
 * SW7408-284: Added support for jitter adjust in PCR hw to PI
 * 
 * Hydra_Software_Devel/63   4/26/11 5:07p gmullen
 * SW7335-1181: Merged latest changes on this branch to mainline
 * 
 * Hydra_Software_Devel/62   2/17/11 11:38a jtna
 * SW7420-1320: add BXPT_Standby/BXPT_Resume
 * 
 * Hydra_Software_Devel/61   2/15/11 10:27p jtna
 * SW7420-1320: merge XPT power management
 * 
 * Hydra_Software_Devel/60   1/18/11 11:36a rjlewis
 * SW7420-1383: Added support for BDBG_OBJECT interface
 * 
 * Hydra_Software_Devel/59   12/29/10 4:25p gmullen
 * SW7401-4376: Merged branched file to mainline
 * 
 * Hydra_Software_Devel/SW7401-4376/3   11/23/10 1:04p gmullen
 * SW7401-4376: Added support for programmable PTS tolerance
 * 
 * Hydra_Software_Devel/SW7401-4376/2   3/22/10 5:59p gmullen
 * SW7401-4376: Added audio splicing
 * 
 * Hydra_Software_Devel/SW7401-4376/1   3/16/10 10:37a gmullen
 * SW7401-4376: Ported Asif's implementation
 * 
 * Hydra_Software_Devel/58   10/11/10 5:17p gmohile
 * SW35230-1645 : Soft rave should look for 2 consecutive frames of
 * possible sequence header data
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
#include "bdbg.h"
#include "bxpt_pcr.h"

BDBG_OBJECT_ID_DECLARE(bxpt_t); /* this is the object identifier to validate an XPT handle */
/* Note: it is not necessary to have a different one for each type of handle because the typechecking will ensure
** that the parameter is of the right type (unless they typecast incorrectly).
*/

#ifdef __cplusplus
extern "C" {
#endif

/* Address delta (in bytes) between elements in a register array. */
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

#define BXPT_P_NUM_PIC_COUNTERS         ( 6 )

#define BXPT_P_MAX_XC_BUFFERS			( 60 )
#define BXPT_P_DEFAULT_PEAK_RATE  		( 25000000 )


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
    BDBG_OBJECT(bxpt_t) /* used to check if structure is valid */

    BCHP_Handle hChip;              /* [Input] Handle to used chip. */
    BREG_Handle hRegister;          /* [Input] Handle to access regiters. */
    BMEM_Handle hMemory;            /* [Input] Handle to memory heap to use. */
    void *vhXpt;                    /* Pointer to parent transport handle. Requires casting. */

    uint8_t ChannelNo;      /* This channels number. */
    uint32_t BaseAddr;  /* Address of the first register in this block. */
    uint32_t LastDescriptor;    /* Address of the last descriptor on the linked list. */
    bool Opened;
    bool Running;
    unsigned int SyncMode;
    BINT_CallbackHandle hPbIntCallback; /* Callback Handle to service the ISR */
	bool AlwaysResumeFromLastDescriptor;

	BXPT_PvrDescriptor *DummyDescriptor;
	uint8_t *DummyBuffer;
    bool ResetPacingOnTsRangeError;
}
BXPT_P_PbHandle;

/***************************************************************************
Summary:
Handle for accessing the remux API via a channel. Users should not directly
access the contents of the structure.
****************************************************************************/

typedef struct BXPT_P_RemuxHandle
{
    BDBG_OBJECT(bxpt_t) /* used to check if structure is valid */

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

#if BXPT_HAS_REMUX_PID_REMAPPING
    bool PidMapEntryAllocated[ BXPT_P_MAX_REMUX_PID_MAPS ];    
#endif

}
BXPT_P_RemuxHandle;

/***************************************************************************
Summary:
Handle for accessing the packet substution API via a channel. Users should 
not directly access the contents of the structure.
****************************************************************************/

typedef struct BXPT_P_PacketSubHandle
{
    BDBG_OBJECT(bxpt_t) /* used to check if structure is valid */

    BCHP_Handle hChip;              /* [Input] Handle to used chip. */
    BREG_Handle hRegister;          /* [Input] Handle to access regiters. */
    BMEM_Handle hMemory;            /* [Input] Handle to memory heap to use. */
    void *vhXpt;

    uint8_t ChannelNo;      /* This channels number. */
    uint32_t BaseAddr;      /* Address of the first register in this block. */
    uint32_t LastDescriptor;    /* Address of the last descriptor on the linked list. */
    bool Opened;
    bool Running;
}
BXPT_P_PacketSubHandle;

typedef struct BXPT_P_PcrHandle_Impl
{
    BDBG_OBJECT(bxpt_t) /* used to check if structure is valid */

    BCHP_Handle hChip;              /* [Input] Handle to used chip. */
    BREG_Handle hRegister;          /* [Input] Handle to access regiters. */
    void *vhXpt;

    uint8_t ChannelNo;              /* This channels number. */
    uint32_t RegOffset;            /* reg Offset of the given module. */
    bool DMode;
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
    BDBG_OBJECT(bxpt_t) /* used to check if structure is valid */

    BXPT_RaveIdx ChannelType;
    void *hChannel;
    bool Allocated;
    unsigned NumChannels;
    void *vhCtx;
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
    BDBG_OBJECT(bxpt_t) /* used to check if structure is valid */
    BCHP_Handle hChp;
    BREG_Handle hReg;           
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

#define BXPT_P_MAX_PIC_COUNTER  ( 5 )
        
typedef struct BXPT_P_RaveHandle    
{
    BDBG_OBJECT(bxpt_t) /* used to check if structure is valid */

    BCHP_Handle hChip;              
    BREG_Handle hReg;           
    BMEM_Handle hMem;           
    BINT_Handle hInt;
    void *lvXpt;
    unsigned ChannelNo;             

    BXPT_P_ContextHandle ContextTbl[ BXPT_P_MAX_RAVE_CONTEXTS ];
    bool SpliceQueueAllocated[ BXPT_P_NUM_SPLICING_QUEUES ];
    StartCodeIndexer ScdTable[ BXPT_P_MAX_SCD ];
    TpitIndexer TpitTable[ BXPT_P_MAX_TPIT ];
    BXPT_P_IndexerHandle IndexerHandles[ BXPT_P_MAX_SCD + BXPT_P_MAX_TPIT ];
    bool PicCounterUsed[ BXPT_P_MAX_PIC_COUNTER ];
    RaveChannelOpenCB chanOpenCB;
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
    BDBG_OBJECT(bxpt_t) /* used to check if structure is valid */

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
typedef struct BXPT_P_TransportData
{
    BDBG_OBJECT(bxpt_t) /* used to check if structure is valid */

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
    ParserConfig IbParserTable[ BXPT_P_MAX_PID_PARSERS ];
	bool InputParserContCountIgnore[ BXPT_P_MAX_PID_PARSERS ];
#endif

    ParserConfig PbParserTable[ BXPT_P_MAX_PLAYBACKS ];

#if BXPT_HAS_MESG_BUFFERS
    bool PidChannelParserConfigOverride[BXPT_P_MAX_MESG_BUFFERS]; /* if set the true then PSI setting for this pid
                                                                     channel can different from the the PSI settings
                                                                     for the PARSER to which this pid channel belongs */

    MessageBufferEntry MessageBufferTable[ BXPT_P_MAX_MESG_BUFFERS ];   /* Table of PI allocated buffers */
    bool MesgBufferIsInitialized[ BXPT_P_MAX_MESG_BUFFERS ];            /* true if buffer has been configured. */

    FilterTableEntry FilterTable[ BXPT_P_MAX_FILTER_BANKS ][ BXPT_P_FILTER_TABLE_SIZE ];

    /* Pointers to the interrupt handlers. */
    BXPT_P_InterruptCallbackArgs MesgIntrCallbacks[ BXPT_P_MAX_MESG_BUFFERS ];
    BXPT_P_InterruptCallbackArgs OverflowIntrCallbacks[ BXPT_P_MAX_MESG_BUFFERS ];
#endif

#if BXPT_HAS_REMUX                      
    BXPT_P_RemuxHandle RemuxHandles[ BXPT_P_MAX_REMULTIPLEXORS ];
#endif

#if BXPT_HAS_PACKETSUB
    BXPT_P_PacketSubHandle PacketSubHandles[ BXPT_P_MAX_PACKETSUBS ]; 
#endif

    PidChannelTableEntry PidChannelTable[ BXPT_P_MAX_PID_CHANNELS ];    /* Table of PI allocated PID channels. */

    BXPT_P_PbHandle PlaybackHandles[ BXPT_P_MAX_PLAYBACKS ];

#if BXPT_HAS_DPCRS
    BXPT_P_PcrHandle_Impl  *PcrHandles[ BXPT_P_MAX_PCRS ];
#endif

    RaveChannel RaveChannels[ BXPT_P_MAX_RAVE_CHANNELS ];
    PcrOffsetData PcrOffsets[ BXPT_P_MAX_PCR_OFFSET_CHANNELS ];

    bool IsLittleEndian;
	bool Pid2BuffMappingOn;
	bool MesgDataOnRPipe;

    BMEM_Handle hPbHeap;

#if BXPT_HAS_RSBUF						
	unsigned long RsBufBO[ BXPT_P_MAX_PID_PARSERS ];
#endif
#if BXPT_HAS_XCBUF						
    unsigned long XcBufBO[ BXPT_P_MAX_XC_BUFFERS ];
#endif
    bool bStandby; /* true if in standby */

    unsigned DpcrRefCount;  /* Number of DPCR channels that have been openned */

#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
    BXPT_PCR_JitterTimestampMode JitterTimestamp[ BXPT_P_MAX_PCRS ]; 
    BXPT_PCR_JitterCorrection PbJitterDisable[ BXPT_P_MAX_PCRS ];
    BXPT_PCR_JitterCorrection LiveJitterDisable[ BXPT_P_MAX_PCRS ];
#endif
}
BXPT_P_TransportData;

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

#endif

#if BXPT_HAS_PID2BUF_MAPPING

    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE                           BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ARRAY_BASE
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DIRECTV_SAVE_FLAGS_MASK              BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DIRECTV_SAVE_FLAGS_MASK
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DIRECTV_SAVE_FLAGS_SHIFT             BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DIRECTV_SAVE_FLAGS_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_OFFSET_MASK						 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_GEN_OFFSET_MASK
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_SPECIAL_TYPE_MASK					 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_SPECIAL_TYPE_MASK
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_SKIP_BYTE2_MASK						 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_SKIP_BYTE2_MASK
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_MASK						 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_GEN_GRP_SEL_MASK
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_FILTER_MODE_MASK						 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_FILTER_MODE_MASK
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_OFFSET_SHIFT						 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_GEN_OFFSET_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_SPECIAL_TYPE_SHIFT					 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_SPECIAL_TYPE_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_SKIP_BYTE2_SHIFT						 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_SKIP_BYTE2_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_SHIFT					 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_GEN_GRP_SEL_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_FILTER_MODE_SHIFT					 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_FILTER_MODE_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ERR_CK_MODE_MASK						 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ERR_CK_MODE_MASK
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ERR_CK_DIS_MASK						 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ERR_CK_DIS_MASK
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SPECIAL_SEL_MASK						 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SPECIAL_SEL_MASK
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ALIGN_MODE_MASK						 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ALIGN_MODE_MASK
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MASK				 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MASK
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SPECIAL_SEL_SHIFT					 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SPECIAL_SEL_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ALIGN_MODE_SHIFT						 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ALIGN_MODE_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DIRECTV_SAVE_FLAGS_FIRST				 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DIRECTV_SAVE_FLAGS_FIRST
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_SHIFT				 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DATA_OUTPUT_MODE_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ERR_CK_MODE_SHIFT					 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ERR_CK_MODE_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ERR_CK_DIS_SHIFT						 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ERR_CK_DIS_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_NO_OUTPUT			 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DATA_OUTPUT_MODE_NO_OUTPUT
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_ARRAY_BASE							 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_ARRAY_BASE
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_SPECIAL_TYPE_MASK					 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_SPECIAL_TYPE_MASK
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_FILTER_MODE_MASK						 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_FILTER_MODE_MASK
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_SPECIAL_TYPE_SHIFT					 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_SPECIAL_TYPE_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_FILTER_MODE_SHIFT					 BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_FILTER_MODE_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SPECIAL_SEL_MASK						 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SPECIAL_SEL_MASK
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ALIGN_MODE_MASK						 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ALIGN_MODE_MASK
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DIRECTV_PES_HD_FILT_MODE_MASK		 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DIRECTV_PES_HD_FILT_MODE_MASK
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MASK				 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MASK
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ALIGN_MODE_SHIFT						 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ALIGN_MODE_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DIRECTV_PES_HD_FILT_MODE_SHIFT		 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DIRECTV_PES_HD_FILT_MODE_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_SHIFT				 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DATA_OUTPUT_MODE_SHIFT
    #define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_RAW				 BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DATA_OUTPUT_MODE_RAW

	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PES	  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PES	 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_TS		  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_TS		 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PAYLOAD  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PAYLOAD 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PSI	  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PSI	 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PES	  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PES	 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PSI	  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PSI	 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SAVE_SHORT_PSI_EN_MASK		  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SAVE_SHORT_PSI_EN_MASK		 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SAVE_LONG_PSI_EN_MASK		  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SAVE_LONG_PSI_EN_MASK		 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_PSF_CRC_DIS_MASK				  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_PSF_CRC_DIS_MASK				 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_MSG_MOD_MODE_MASK			  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_MSG_MOD_MODE_MASK			 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SAVE_SHORT_PSI_EN_SHIFT		  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SAVE_SHORT_PSI_EN_SHIFT		 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SAVE_LONG_PSI_EN_SHIFT		  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SAVE_LONG_PSI_EN_SHIFT		 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_PSF_CRC_DIS_SHIFT			  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_PSF_CRC_DIS_SHIFT			 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_MSG_MOD_MODE_SHIFT			  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_MSG_MOD_MODE_SHIFT			 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_PSF_CRC_DIS_MASK				  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_PSF_CRC_DIS_MASK				 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_PSF_CRC_DIS_SHIFT			  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_PSF_CRC_DIS_SHIFT			 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SAVE_LONG_PSI_EN_MASK		  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SAVE_LONG_PSI_EN_MASK		 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SAVE_LONG_PSI_EN_SHIFT		  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SAVE_LONG_PSI_EN_SHIFT		 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SAVE_SHORT_PSI_EN_MASK		  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SAVE_SHORT_PSI_EN_MASK		 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SAVE_SHORT_PSI_EN_SHIFT		  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SAVE_SHORT_PSI_EN_SHIFT		 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_MSG_MOD_MODE_MASK			  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_MSG_MOD_MODE_MASK			 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_MSG_MOD_MODE_SHIFT			  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_MSG_MOD_MODE_SHIFT			 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SAVE_SHORT_PSI_EN_MASK		  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SAVE_SHORT_PSI_EN_MASK		 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SAVE_LONG_PSI_EN_MASK		  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SAVE_LONG_PSI_EN_MASK		 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_PSF_CRC_DIS_MASK				  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_PSF_CRC_DIS_MASK				 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_MSG_MOD_MODE_MASK			  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_MSG_MOD_MODE_MASK			 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SAVE_SHORT_PSI_EN_SHIFT		  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SAVE_SHORT_PSI_EN_SHIFT		 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_SAVE_LONG_PSI_EN_SHIFT		  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_SAVE_LONG_PSI_EN_SHIFT		 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_PSF_CRC_DIS_SHIFT			  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_PSF_CRC_DIS_SHIFT			 
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_MSG_MOD_MODE_SHIFT			  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_MSG_MOD_MODE_SHIFT			 
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G8_B0			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_GEN_GRP_SEL_G8_B0			 
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G8_B8			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_GEN_GRP_SEL_G8_B8			 
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G4_B0			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_GEN_GRP_SEL_G4_B0			 
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G4_B4			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_GEN_GRP_SEL_G4_B4			 
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G4_B8			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_GEN_GRP_SEL_G4_B8			 
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G4_B12			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_GEN_GRP_SEL_G4_B12			 
	#define BCHP_XPT_MSG_PID_ERR_00_31									  BCHP_XPT_MSG_BUF_ERR_00_31									 

	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_PHY_EN_MASK			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_PHY_EN_MASK
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_NET_EN_MASK			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_NET_EN_MASK
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_M40_EN_MASK			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_M40_EN_MASK
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_M24_EN_MASK			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_M24_EN_MASK
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_M16_EN_MASK			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_M16_EN_MASK
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_ID_REJECT_EN_MASK	  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_ID_REJECT_EN_MASK
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_SPECIAL_NOT_MASK		  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_SPECIAL_NOT_MASK
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_PHY_EN_SHIFT			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_PHY_EN_SHIFT
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_NET_EN_SHIFT			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_NET_EN_SHIFT
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_M40_EN_SHIFT			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_M40_EN_SHIFT
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_M24_EN_SHIFT			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_M24_EN_SHIFT
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_M16_EN_SHIFT			  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_M16_EN_SHIFT
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_ID_REJECT_EN_SHIFT	  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_ID_REJECT_EN_SHIFT
	#define BCHP_XPT_MSG_PID_CTRL2_TABLE_i_SPECIAL_NOT_SHIFT	  BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_SPECIAL_NOT_SHIFT

	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DIRECTV_SAVE_FLAGS_NONE BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DIRECTV_SAVE_FLAGS_NONE
	#define BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DIRECTV_SAVE_FLAGS_ALL  BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_DIRECTV_SAVE_FLAGS_ALL


#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BXPT_PRIV_H__ */

/* end of file */


