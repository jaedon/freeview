/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_mesg.c $
 * $brcm_Revision: Hydra_Software_Devel/76 $
 * $brcm_Date: 8/21/12 10:26a $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/src/core65nm/bxpt_mesg.c $
 * 
 * Hydra_Software_Devel/76   8/21/12 10:26a gmullen
 * SW7425-3785: Don't automatically enable PID channels during mesg
 * handling
 * 
 * Hydra_Software_Devel/75   5/21/12 2:47p gmullen
 * SW7401-3748: Fixed compilation error, old macro being used
 * 
 * Hydra_Software_Devel/74   12/20/11 5:04p gmullen
 * SW7425-1868: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1868/1   12/20/11 5:00p gmullen
 * SW7425-1868: Added filter offset support
 * 
 * Hydra_Software_Devel/73   3/25/11 10:40a gmullen
 * SWDTV-5976: Added 35233 support
 * 
 * Hydra_Software_Devel/72   1/18/11 11:36a rjlewis
 * SW7420-1383: Added support for BDBG_OBJECT interface
 * 
 * Hydra_Software_Devel/71   12/17/10 4:04p gmullen
 * SW7405-5066: Set PID2BUF mapping when starting message capture and PID
 * channel recording
 * 
 * Hydra_Software_Devel/70   9/3/10 3:58p gmullen
 * SW35230-1172: Fixed message size calcs in BXPT_CheckBufferWithWrap()
 * 
 * Hydra_Software_Devel/69   8/30/10 5:24p gmullen
 * SW7403-924: Protected access to the PID and SPID table from other
 * modules in the PI
 * 
 * Hydra_Software_Devel/68   8/27/10 3:46p gmullen
 * SW35230-1172: Merged to mainline
 * 
 * Hydra_Software_Devel/SW35230-1172/2   8/27/10 3:35p gmullen
 * SW35230-1172: Removed old implementation of BXPT_CheckBuffer()
 * 
 * Hydra_Software_Devel/SW35230-1172/1   8/27/10 2:16p gmullen
 * SW35230-1172: Added BXPT_CheckBufferWithWrap()
 * 
 * Hydra_Software_Devel/67   5/25/10 8:27a gmullen
 * SW3548-2589: BXPT_Mesg_StartPidChannelRecord now checks reference count
 * before enabling PID channel
 * 
 * Hydra_Software_Devel/66   4/2/10 5:11p gmullen
 * SW7405-4165: Fixed inconsistent API descriptions
 * 
 * Hydra_Software_Devel/65   11/2/09 10:53a gmullen
 * SW3548-2589: Use reference counting when deciding to disable PID
 * channels
 * 
 * Hydra_Software_Devel/64   10/27/09 9:07a gmullen
 * SW7405-3301: Changed index from per-PID channel to per-message buffer
 * 
 * Hydra_Software_Devel/63   9/24/09 5:27p gmullen
 * SWDEPRECATED-3717: Added BXPT_GetDefaultPsiSettings() and
 * BXPT_GetDefaultPidChannelRecordSettings()
 * 
 * Hydra_Software_Devel/62   9/24/09 1:22p gmullen
 * SW7325-574: Backed out vers 57 change.
 * 
 * Hydra_Software_Devel/61   9/22/09 4:27p gmullen
 * SWDEPRECATED-3717: BXPT_Mesg_SetPid2Buff() needs to check for
 * transitions between enable and disable
 * 
 * Hydra_Software_Devel/60   9/16/09 2:55p piyushg
 * SW3548-2479: Added new API BXPT_GetBuffer_isr which directly calls
 * BXPT_UpdateReadOffset_isr() to avoid entering into critical section.
 * 
 * Hydra_Software_Devel/59   9/14/09 11:58a piyushg
 * SW3548-2470: Enable PID2BUF mapping API's for 3548.
 * 
 * Hydra_Software_Devel/58   9/2/09 2:07p piyushg
 * SW7325-582: Added API in bxpt_mesg.c to disable the pid channel
 * to message buffer association that is created after calling the
 * ConfigPid2BufferMap.
 * 
 * Hydra_Software_Devel/57   8/25/09 3:18p piyushg
 * SW7325-574: MessageSize calculation changed for wrap
 * around condition.
 * 
 * Hydra_Software_Devel/56   8/11/09 10:39a piyushg
 * PR55216: Added initial 7340 XPT support.
 * 
 * Hydra_Software_Devel/55   8/5/09 4:52p piyushg
 * PR55545: Add 7125 XPT PI support
 * Added file element "bxpt_rave_ihex.c".
 * Added file element "bxpt_rave_ihex.h".
 * 
 * Hydra_Software_Devel/54   7/31/09 2:44p gmullen
 * PR54331: Added 35130 to XPT support.
 * 
 * Hydra_Software_Devel/53   7/14/09 10:21a piyushg
 * PR56771: Add XPT PI code for 7342.
 * 
 * Hydra_Software_Devel/52   6/8/09 3:41p gmullen
 * PR55813: Fixed message buffer index usage.
 * 
 * Hydra_Software_Devel/51   6/5/09 3:43p gmullen
 * PR55761: Added BufferSize param check to BXPT_SetPidChannelBuffer()
 * 
 * Hydra_Software_Devel/50   4/29/09 1:51p gmullen
 * PR54710: Added requested features to debug mesg.
 * 
 * Hydra_Software_Devel/49   1/27/09 1:09p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/48   1/5/09 10:03a gmullen
 * PR50811: Added _isr version of BXPT_UpdateReadOffset().
 * 
 * Hydra_Software_Devel/47   10/28/08 8:21a gmullen
 * PR48291: Checked return value.
 * 
 * Hydra_Software_Devel/46   9/30/08 4:45p gmullen
 * PR34473: Don't drop messages that the CDI driver doesn't have room for.
 * 
 * Hydra_Software_Devel/45   9/30/08 11:29a gmullen
 * PR34473: Removed patch from mainline code.
 * 
 * Hydra_Software_Devel/44   9/19/08 3:50p gmullen
 * PR47147: Wrapped _isr() calls with critical sections.
 * 
 * Hydra_Software_Devel/43   7/17/08 4:47p gmullen
 * PR44849: Fixed compilation errors for 7335B0
 * 
 * Hydra_Software_Devel/42   7/16/08 3:43p gmullen
 * PR37867: Merged playback mux code to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/41   7/10/08 7:49p gmullen
 * PR44734: Fixed compilation warnings.
 * 
 * Hydra_Software_Devel/40   6/18/08 4:10p piyushg
 * PR39234: Change some comments and API names for setting up
 * multiple message buffers for same PID channel.
 * 
 * Hydra_Software_Devel/39   6/16/08 11:55a piyushg
 * PR39234: Change some comments and API names for setting up
 * multiple message buffers for same PID channel.
 * 
 * Hydra_Software_Devel/38   5/15/08 2:49p gmullen
 * PR42718: Changed bounds check.
 * 
 * Hydra_Software_Devel/37   5/6/08 1:46p gmullen
 * PR42471: Check that PID channel has been configured AND is mapped to
 * the same parser.
 * 
 * Hydra_Software_Devel/36   4/1/08 6:55p gmullen
 * PR40496: Added bounds check for ParserNum index.
 * 
 * Hydra_Software_Devel/35   3/17/08 1:03p gmullen
 * PR40675: Added support to route all mesg filter data on R-pipe.
 * 
 * Hydra_Software_Devel/34   3/11/08 3:02p mward
 * PR38618: Fix typo to fix 7118[A0|B0] support.
 * 
 * Hydra_Software_Devel/33   3/7/08 4:54p gmullen
 * PR38618: Added 7118C0 support
 * 
 * Hydra_Software_Devel/32   3/7/08 11:00a piyushg
 * PR39234: Fix compilation error for Chips that do not support
 * PID Duplication.
 * 
 * Hydra_Software_Devel/31   3/6/08 3:47p piyushg
 * PR39234: Initial checkin for API's to support PID Duplication.
 * 
 * Hydra_Software_Devel/30   2/19/08 2:58p gmullen
 * PR39609: Fixed writes of DATA_OUTPUT_MODE to wrong register.
 * 
 * Hydra_Software_Devel/29   10/1/07 4:30p gmullen
 * PR35528: GetPidChannelByFilter() iterates over number of mesg buffers,
 * not PID channels.
 * 
 * Hydra_Software_Devel/28   9/19/07 9:20a gmullen
 * PR35049: Passed return value from BXPT_UpdateReadOffset() to caller.
 * 
 * Hydra_Software_Devel/27   9/13/07 1:36p gmullen
 * PR32868: Changes to support bmsglib
 * 
 * Hydra_Software_Devel/26   5/17/07 2:39p katrep
 * PR29057: 29057
 * 
 * Hydra_Software_Devel/25   4/27/07 8:31a katrep
 * PR25533: Changed the warning message to debug message
 * 
 * Hydra_Software_Devel/24   3/16/07 11:59a gmullen
 * PR28828: Added check for bad BytesRead to BXPT_UpdateReadOffset
 * 
 * Hydra_Software_Devel/23   2/20/07 5:30p gmullen
 * PR27998: BXPT_P_ApplyParserPsiSettings needs to clear PB flag from
 * parser number before checking.
 * 
 * Hydra_Software_Devel/22   12/20/06 10:34a katrep
 * PR25431: Downgraded the error message to debug message when the missed
 * data ready interrupt condition is hit.
 * 
 * Hydra_Software_Devel/21   12/6/06 3:22p katrep
 * PR26127: Also covers PR 26358,Fixed BXPT_RemoveFilterFromGroup Api
 * 
 * Hydra_Software_Devel/20   12/5/06 3:00p gmullen
 * PR26325: Added support for CAP filtering.
 * 
 * Hydra_Software_Devel/19   12/5/06 2:04p gmullen
 * PR26278: Allow user to configure byte alignment of data in message
 * buffer.
 * 
 * Hydra_Software_Devel/18   12/4/06 6:33p katrep
 * PR25533: Fixed broken psi scanning due to read of buffer when the
 * packet received is incomplete.
 * 
 * Hydra_Software_Devel/17   12/1/06 3:26p katrep
 * PR25533: Due to HW bug data ready flag is not reliable. Use the Read
 * and valid pointers to determine if the data is available.
 * 
 * Hydra_Software_Devel/16   11/27/06 4:24p katrep
 * PR25431: Added code to detect the lost data ready interrupt and
 * generate sw callback
 * 
 * Hydra_Software_Devel/15   11/2/06 3:06p katrep
 * PR25278: Added PidChannelNum argument to BXPT_AddFilterToGroup API.
 * 
 * Hydra_Software_Devel/14   11/1/06 3:34p gmullen
 * PR23189: Added support for 3563
 * 
 * Hydra_Software_Devel/13   9/25/06 12:40p katrep
 * PR24466: Diabled Sanity check for parser band argument.
 * 
 * Hydra_Software_Devel/12   9/21/06 6:31p katrep
 * PR24466: Fixed the bug in BXPT_AddFilterToGroup API.
 * 
 * Hydra_Software_Devel/11   8/18/06 5:20p gmullen
 * PR23679: Removed workaround. Its only needed on 7401A0.
 * 
 * Hydra_Software_Devel/10   8/7/06 2:02p gmullen
 * PR18998: BXPT_StopPidChannelRecord() will disable PID channel
 * destination fieldm , not the channel itself.
 * 
 * Hydra_Software_Devel/9   7/27/06 10:59a gmullen
 * PR15309: BXPT_ConfigurePidChannel() now checks for duplicated PID
 * channels
 * 
 * Hydra_Software_Devel/8   7/21/06 12:17p katrep
 * PR21525: Added support for 128,16 byte filters across 4 banks or
 * 64,32byte filters across 2 banks.
 * 
 * Hydra_Software_Devel/7   7/17/06 5:19p gmullen
 * PR20624: Added code for 7118.
 * 
 * Hydra_Software_Devel/6   7/13/06 11:37a katrep
 * PR20316: Added support to set/unset PSI setting per pid channel basis
 * as it's supported by new HW arch.PR20315:Also added support for 32
 * byte filters.Default filter size is 16 bytes.32 Bytes filters can be
 * enabled at compile time by -DBXPT_FILTER_32
 * 
 * Hydra_Software_Devel/5   7/10/06 9:33a gmullen
 * PR18998: Fixed void * dereference warning.
 * 
 * Hydra_Software_Devel/4   7/5/06 12:13p katrep
 * PR22320: Fixed some logical errors. Enabled Filetr Bank 1
 * 
 * Hydra_Software_Devel/3   4/24/06 5:48p gmullen
 * PR18998: Set GEN_OFFSET to 3 for AUX messages, 0 for all others.
 * 
 * Hydra_Software_Devel/2   3/31/06 4:11p gmullen
 * PR 20314: 7400 PI also had this problem
 * 
 * Hydra_Software_Devel/12   1/9/06 9:40a gmullen
 * PR15039: Removed static define for DisableFilter() and PauseFilter()
 * 
 * Hydra_Software_Devel/11   1/6/06 4:41p gmullen
 * PR18489: Changes to other modules to support fix for PR 18489.
 * 
 * Hydra_Software_Devel/10   1/4/06 9:57a gmullen
 * PR15309: Filter mode is now Generic OR Special Filter when capturing
 * DirecTV messages.
 * 
 * Hydra_Software_Devel/9   1/3/06 10:22a gmullen
 * PR18796: Fixed un-initialized variable "Reg" .
 * 
 * Hydra_Software_Devel/8   12/16/05 10:06a gmullen
 * PR18228: Added this API to the 7401.
 * 
 * Hydra_Software_Devel/7   9/26/05 2:22p gmullen
 * PR17303: Feature to use byte 2 of PSI message in filter match was
 * enabled. This must be disabled for backward compatability.
 * 
 * Hydra_Software_Devel/6   9/21/05 2:17p gmullen
 * PR15309: Added support for AAC HE and AC3+ audio, fixed bug in playback
 * PI, modified RAVE PI to support channel change and reset.
 * 
 * Hydra_Software_Devel/5   8/31/05 10:58a gmullen
 * PR15309: Fixed bugs in PSI extraction and MPEG audio support.
 * 
 * Hydra_Software_Devel/4   8/26/05 2:01p gmullen
 * PR15309: Added DirecTV support, RAVE video support, cleaned up RS and
 * XC buffer code.
 * 
 * Hydra_Software_Devel/3   8/18/05 9:59a gmullen
 * PR15309: Added more DirecTV support.
 * 
 * Hydra_Software_Devel/2   7/19/05 1:27p gmullen
 * PR15309: Fixed buffer alloc bugs.
 * 
 * Hydra_Software_Devel/1   7/15/05 9:03a gmullen
 * PR15309: Inita i
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt.h"
#include "bkni.h"
#include "bmem.h"

#include "bchp_xpt_msg.h"
#include "bchp_xpt_fe.h"

#if BXPT_HAS_PACKETSUB
#include "bchp_xpt_psub.h"
#include "bxpt_packetsub.h"
#endif

/* Message buffers sizes must be multiples of BXPT_P_MESSAGE_BUFFER_BLOCK_SIZE */
#define BXPT_P_MESSAGE_BUFFER_BLOCK_SIZE 1024
#define SPID_CHNL_STEPSIZE      ( 4 )

BDBG_MODULE( xpt_mesg );

/* On some chips, such at the 35233, the reset register was renamed */
#ifdef BCHP_XPT_MSG_DMA_BUFFER_INIT
#define BCHP_XPT_MSG_DMA_BUFFER_RESET BCHP_XPT_MSG_DMA_BUFFER_INIT
#endif

static BERR_Code ConfigMessageBufferRegs( BXPT_Handle hXpt, unsigned int PidChannelNum, uint32_t CpuAddr, BXPT_MessageBufferSize BufferSize); 
static void GetFilterAddrs( unsigned int Bank, unsigned int FilterNum, uint32_t *CoefAddr, uint32_t *MaskAddr, uint32_t *ExclAddr );
static BERR_Code ChangeFilterByte( BXPT_Handle hXpt, uint32_t FilterBaseAddr, unsigned int FilterNum, unsigned int ByteOffset, uint8_t FilterByte );
static unsigned GetPidChannelByFilter( BXPT_Handle hXpt, unsigned int Bank, unsigned int FilterNum );
static BERR_Code EnableFilter( BXPT_Handle hXpt, unsigned int FilterNum, unsigned int PidChannelNum );
static int AdjustChanAndAddr( BXPT_Handle hXpt, uint32_t PidChannelNum, uint32_t FlagBaseRegister, uint32_t *AdjustedPidChannel, uint32_t *AdjustedRegisterAddr );
static int GetPidChannelFlag( BXPT_Handle hXpt, int PidChannelNum, int FlagBaseRegister );
static void GetBufferPointers( BXPT_Handle hXpt, unsigned int PidChannelNum, uint32_t *ReadPtr, uint32_t *ValidPtr, size_t *MesgSize, size_t *BufferSizeInBytes );
static uint32_t GetRegArrayAddr( BXPT_Handle hXpt, uint32_t PidChannelNum, uint32_t BaseRegister );
static void CopyDmaDataToUser( BXPT_Handle hXpt, unsigned int PidChannelNum, uint32_t ReadPtr, uint8_t *UserBufferAddr, size_t NumBytesToCopy, size_t DmaBufferSize );

#if BXPT_HAS_PID2BUF_MAPPING
static void ResetPid2BuffMap(BXPT_Handle hXpt);
static void SetPid2BuffMap(BXPT_Handle hXpt);
static void ConfigPid2BufferMap( BXPT_Handle hXpt, unsigned int PidChannelNum, unsigned int BufferNumber, bool enableIt);
#endif

#if BXPT_HAS_PID2BUF_MAPPING
void BXPT_Mesg_SetPid2Buff(
        BXPT_Handle hXpt,
        bool SetPid2Buff
    )
{
    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	if (SetPid2Buff && !hXpt->Pid2BuffMappingOn)
    {
		ResetPid2BuffMap(hXpt);
    }
	else if (!SetPid2Buff && hXpt->Pid2BuffMappingOn)
    {
		SetPid2BuffMap(hXpt);
    }

    hXpt->Pid2BuffMappingOn = SetPid2Buff;

    return;
}

static void ResetPid2BuffMap(
	BXPT_Handle hXpt
	)
{
    uint32_t RegAddr;
	int i;

	RegAddr = BCHP_XPT_MSG_PID2BUF_MAP_i_ARRAY_BASE;

	for (i=0; i<(BXPT_P_MAX_MESG_BUFFERS * 4); i++) 
		BREG_Write32( hXpt->hRegister, RegAddr + (i*4), 0x00000000 );

	return;
}

static void SetPid2BuffMap(
	BXPT_Handle hXpt
	)
{
    uint32_t Reg, RegAddr;
    uint32_t tempReg;
	int i;

	ResetPid2BuffMap(hXpt);

	for (i=0; i<BXPT_P_MAX_MESG_BUFFERS; i++) 
	{
		RegAddr = BCHP_XPT_MSG_PID2BUF_MAP_i_ARRAY_BASE + ((i * 4) + (int)(i/32)) * PID2BUF_TABLE_STEP;
		Reg = BREG_Read32( hXpt->hRegister, RegAddr + i * 4 * PID2BUF_TABLE_STEP );
		tempReg = 0x00000001;

		if ( i < 32 ) {
			tempReg <<= i;
		}
		else if ( i < 64 ) {
			tempReg <<= (i - 32);
		}
		else if ( i < 96 ) {
			tempReg <<= (i - 64);
		}
		else if ( i < 128 ) {
			tempReg <<= (i - 96);
		}
		Reg |= tempReg;
		BREG_Write32( hXpt->hRegister, RegAddr, Reg );
	}

	return;
}


BERR_Code BXPT_Mesg_SetPidChannelBuffer( 
    BXPT_Handle hXpt,                   /* [in] Handle for this transport */
    unsigned int PidChannelNum,             /* [in] Which PID channel buffer we want. */
    unsigned int MesgBufferNum,             /* [in] Which Buffer number we want . */
    void *CpuAddr,                  /* [in] Caller allocated memory, or NULL. */
    BXPT_MessageBufferSize BufferSize   /* [in] Size, in bytes, of the buffer. */
    )
{

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    /* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )
    {
        /* Bad PID channel number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
	else if ( MesgBufferNum >= BXPT_P_MAX_MESG_BUFFERS ) {
        /* Bad Message Buffer number. Complain. */
        BDBG_ERR(( "Message Buffer Number %lu is out of range!", ( unsigned long ) MesgBufferNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
    else if ( hXpt->Pid2BuffMappingOn == true) 
    {
        /* Is the caller allocating memory, or are we? */
        if( CpuAddr == NULL )
        {   
            /* We are. */
            uint32_t Reg, RegAddr;
            BXPT_MessageBufferSize OldBufferSize;
             
            MessageBufferEntry *Buffer = &hXpt->MessageBufferTable[ MesgBufferNum ];

            RegAddr = BCHP_XPT_MSG_DMA_BP_TABLE_i_ARRAY_BASE + ( MesgBufferNum * BP_TABLE_STEP );

            Reg = BREG_Read32( hXpt->hRegister, RegAddr );
            OldBufferSize = ( BXPT_MessageBufferSize ) BCHP_GET_FIELD_DATA( Reg, XPT_MSG_DMA_BP_TABLE_i, BP_BUFFER_SIZE );

            /* Allocate under 2 cases: The buffer size is changing, or if no buffer was ever allocated */
            if( ( Buffer->IsAllocated == true && BufferSize != OldBufferSize ) 
            || Buffer->IsAllocated == false )
            {
                unsigned int i;

                unsigned long SizeInBytes = BXPT_P_MESSAGE_BUFFER_BLOCK_SIZE;

                /* Free the old buffer, if there is one. */
                if( Buffer->IsAllocated == true )
                    BMEM_Free( hXpt->hMemory, ( void * ) Buffer->Address );

                /* Convert from the enum to a byte count. */
                for( i = 0; i < ( unsigned int ) BufferSize; i++ )
                    SizeInBytes *= 2;

                /*
                ** Alloc some memory. Must use the Memory Manager, since this buffer
                ** must be visible to both the transport hardware and the CPU. We also
                ** have some alignment requirements. 
                */
                Buffer->Address = ( uint32_t ) BMEM_AllocAligned( hXpt->hMemory, SizeInBytes, 10, 0 ); 
                if( Buffer->Address )
                {
                    Buffer->IsAllocated = true;

					ConfigPid2BufferMap( hXpt, PidChannelNum, MesgBufferNum, true);
                    /* Load the buffer address and size into the transport registers. */
                    ExitCode = ConfigMessageBufferRegs( hXpt, MesgBufferNum, Buffer->Address, BufferSize );
                }
                else
                {
                    BDBG_ERR(( "Message buffer alloc failed! MesgBufferNum %lu", ( unsigned long ) MesgBufferNum ));
                    ExitCode = BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
                }
            }
        }
        else
        {
            /* We are not, the caller is. */

			ConfigPid2BufferMap( hXpt, PidChannelNum, MesgBufferNum, true);

            /* Load the buffer address and size into the transport registers. */
            ExitCode = ConfigMessageBufferRegs( hXpt, MesgBufferNum, ( uint32_t ) CpuAddr, BufferSize );
        }

        /* Note that the buffer is now configured. */
        hXpt->MesgBufferIsInitialized[ MesgBufferNum ] = true;
    }
	else
	{
        /* Wrong function call. Complain. */
	BDBG_ERR(( "Pid2BuffMapping is OFF. Please use BXPT_SetPidChannelBuffer instead of BXPT_Mesg_SetPidChannelBuffer. " ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}

    return( ExitCode );
}

static void ConfigPid2BufferMap(
    BXPT_Handle hXpt,
    unsigned int PidChannelNum,
    unsigned int BufferNumber,
	bool		 enableIt		
    )
{
    uint32_t Reg, RegAddr;
    uint32_t tempReg;

	#if 0
    /*Reset the PID to BUFF mapping entry for PID=BUFF
    By default PID 0 is mapped to BUFF 0 ... PID 127 is mapped
    to BUFF 127 */
    RegAddr = BCHP_XPT_MSG_PID2BUF_MAP_i_ARRAY_BASE + (BufferNumber * 4 + (int)(BufferNumber/32)) * PID2BUF_TABLE_STEP;
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    tempReg = 0x00000001;

    if ( BufferNumber < 32 ) {
        tempReg <<= BufferNumber;
    }
    else if ( BufferNumber < 64 ) {
        tempReg <<= (BufferNumber - 32);
    }
    else if ( BufferNumber < 96 ) {
        tempReg <<= (BufferNumber - 64);
    }
    else if ( BufferNumber < 128 ) {
        tempReg <<= (BufferNumber - 96);
    }
    Reg &= (~tempReg);
    BREG_Write32( hXpt->hRegister, RegAddr, Reg );
	#endif

    /* Set the PID to BUFF map for new PID channel*/
    RegAddr = BCHP_XPT_MSG_PID2BUF_MAP_i_ARRAY_BASE + (PidChannelNum * 4 + (int)(BufferNumber/32)) * PID2BUF_TABLE_STEP;
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    tempReg = 0x00000001;

    if ( BufferNumber < 32 ) {
        tempReg <<= BufferNumber;
    }
    else if ( BufferNumber < 64 ) {
        tempReg <<= (BufferNumber - 32);
    }
    else if ( BufferNumber < 96 ) {
        tempReg <<= (BufferNumber - 64);
    }
    else if ( BufferNumber < 128 ) {
        tempReg <<= (BufferNumber - 96);
    }

	if (enableIt)
        Reg |= tempReg;
	else
		Reg &= (~tempReg);
    BREG_Write32( hXpt->hRegister, RegAddr, Reg );
}
#endif

BERR_Code BXPT_SetPidChannelBuffer( 
    BXPT_Handle hXpt,                   /* [in] Handle for this transport */
    unsigned int PidChannelNum,             /* [in] Which PID channel buffer we want. */
    void *CpuAddr,                  /* [in] Caller allocated memory, or NULL. */
    BXPT_MessageBufferSize BufferSize   /* [in] Size, in bytes, of the buffer. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	if (hXpt->Pid2BuffMappingOn == true )
	{
        /* Wrong function call. Complain. */
	BDBG_ERR(( "Pid2BuffMapping is ON. Please use BXPT_Mesg_SetPidChannelBuffer instead of BXPT_SetPidChannelBuffer. " ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
		return (ExitCode);
	}

    /* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )
    {
        /* Bad PID channel number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else if( BufferSize > BXPT_MessageBufferSize_e512kB )
    {
        /* Buffer size is bad. Complain. */
        BDBG_ERR(( "BufferSize %lu is out of range!", ( unsigned long ) BufferSize ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        /* Is the caller allocating memory, or are we? */
        if( CpuAddr == NULL )
        {   
            /* We are. */
            uint32_t Reg, RegAddr;
            BXPT_MessageBufferSize OldBufferSize;
             
            MessageBufferEntry *Buffer = &hXpt->MessageBufferTable[ PidChannelNum ];

            RegAddr = BCHP_XPT_MSG_DMA_BP_TABLE_i_ARRAY_BASE + ( PidChannelNum * BP_TABLE_STEP );

            Reg = BREG_Read32( hXpt->hRegister, RegAddr );
            OldBufferSize = ( BXPT_MessageBufferSize ) BCHP_GET_FIELD_DATA( Reg, XPT_MSG_DMA_BP_TABLE_i, BP_BUFFER_SIZE );

            /* Allocate under 2 cases: The buffer size is changing, or if no buffer was ever allocated */
            if( ( Buffer->IsAllocated == true && BufferSize != OldBufferSize ) 
            || Buffer->IsAllocated == false )
            {
                unsigned int i;

                unsigned long SizeInBytes = BXPT_P_MESSAGE_BUFFER_BLOCK_SIZE;

                /* Free the old buffer, if there is one. */
                if( Buffer->IsAllocated == true )
                    BMEM_Free( hXpt->hMemory, ( void * ) Buffer->Address );

                /* Convert from the enum to a byte count. */
                for( i = 0; i < ( unsigned int ) BufferSize; i++ )
                    SizeInBytes *= 2;

                /*
                ** Alloc some memory. Must use the Memory Manager, since this buffer
                ** must be visible to both the transport hardware and the CPU. We also
                ** have some alignment requirements. 
                */
                Buffer->Address = ( uint32_t ) BMEM_AllocAligned( hXpt->hMemory, SizeInBytes, 10, 0 ); 
                if( Buffer->Address )
                {
                    Buffer->IsAllocated = true;

                    /* Load the buffer address and size into the transport registers. */
                    ExitCode = ConfigMessageBufferRegs( hXpt, PidChannelNum, Buffer->Address, BufferSize );
                }
                else
                {
                    BDBG_ERR(( "Message buffer alloc failed! PidChannelNum %lu", ( unsigned long ) PidChannelNum ));
                    ExitCode = BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
                }
            }
        }
        else
        {
            /* We are not, the caller is. */

            /* Load the buffer address and size into the transport registers. */
            ExitCode = ConfigMessageBufferRegs( hXpt, PidChannelNum, ( uint32_t ) CpuAddr, BufferSize );
        }

        /* Note that the buffer is now configured. */
        hXpt->MesgBufferIsInitialized[ PidChannelNum ] = true;
    }

    return( ExitCode );
}

BERR_Code BXPT_AllocPSIFilter( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int Bank,          /* [in] Which bank to allocate from. */
    unsigned int *FilterNum     /* [out] Number for the allocated filter. */
    )
{
    unsigned int i;

    BERR_Code ExitCode = BXPT_ERR_NO_AVAILABLE_RESOURCES;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    
    /* Sanity check on the arguments. */
    if( Bank >= hXpt->MaxFilterBanks )     
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "Bank %lu is out of range!", ( unsigned long ) Bank ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        for( i = 0; i < BXPT_P_MAX_FILTERS_PER_BANK; i++ )
        {
            if( hXpt->FilterTable[ Bank ][ i ].IsAllocated == false )
            {
                hXpt->FilterTable[ Bank ][ i ].IsAllocated = true;
                *FilterNum = i;
                ExitCode = BERR_SUCCESS;
                break;  
            }
        }
    }

    return( ExitCode );
}

BERR_Code BXPT_FreePSIFilter( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int Bank,          /* [in] Which bank to free the filter from. */
    unsigned int FilterNum      /* [in] Which filter to free up. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    
    /* Sanity check on the arguments. */
    if( Bank >= hXpt->MaxFilterBanks )     
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "Bank %lu is out of range!", ( unsigned long ) Bank ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else if( FilterNum >= BXPT_P_FILTER_TABLE_SIZE )       
    {
        /* Bad filter number. Complain. */
        BDBG_ERR(( "FilterNum %lu is out of range!", ( unsigned long ) FilterNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        hXpt->FilterTable[ Bank ][ FilterNum ].IsAllocated = false;
    }

    return( ExitCode );
}

BERR_Code BXPT_GetFilter( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int Bank,              /* [in] Which bank the filter belongs to. */
    unsigned int FilterNum,         /* [in] Which filter to get data from. */
    BXPT_Filter *Filter         /* [out] Filter data */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( Filter );

    /* Sanity check on the arguments. */
    if( Bank >= hXpt->MaxFilterBanks )     
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "Bank %lu is out of range!", ( unsigned long ) Bank ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else if( FilterNum >= BXPT_P_FILTER_TABLE_SIZE )       
    {
        /* Bad filter number. Complain. */
        BDBG_ERR(( "FilterNum %lu is out of range!", ( unsigned long ) FilterNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        unsigned int i, GroupSel;

        uint32_t CoefRegAddr = 0, MaskRegAddr = 0, ExclRegAddr = 0;
        
        if (BXPT_FILTER_SIZE == 32)
        {
            /* 
            ** Logical bank 0 maps to filter group 8B0. 
            ** Logical bank 1 maps to filter grouo 4B8.
            */
            GroupSel = Bank * 256;
        }
        else
        {
            /* 
            ** Logical bank 0 maps to filter group 4B0. 
            ** Logical bank 1 maps to filter grouo 4B4.
            */
            GroupSel = Bank * 128;
        }
        for( i = 0; i < BXPT_FILTER_SIZE; i += 4 )
        {
            uint32_t Reg;

            GetFilterAddrs( GroupSel, FilterNum, &CoefRegAddr, &MaskRegAddr, &ExclRegAddr );
            GroupSel += 32; /* Step to the next bank in the cascade */

            Reg = BREG_Read32( hXpt->hRegister, CoefRegAddr );
            Filter->Coeficient[ i ] = ( uint8_t ) (( Reg >> 24 ) & 0xFF );                  
            Filter->Coeficient[ i + 1 ] = ( uint8_t ) (( Reg >> 16 ) & 0xFF );                  
            Filter->Coeficient[ i + 2 ] = ( uint8_t ) (( Reg >> 8 ) & 0xFF );                   
            Filter->Coeficient[ i + 3 ] = ( uint8_t ) ( Reg & 0xFF );                   

            Reg = BREG_Read32( hXpt->hRegister, MaskRegAddr );
            Filter->Mask[ i ] = ( uint8_t ) (( Reg >> 24 ) & 0xFF );                    
            Filter->Mask[ i + 1 ] = ( uint8_t ) (( Reg >> 16 ) & 0xFF );                    
            Filter->Mask[ i + 2 ] = ( uint8_t ) (( Reg >> 8 ) & 0xFF );                 
            Filter->Mask[ i + 3 ] = ( uint8_t ) ( Reg & 0xFF );                 

            Reg = BREG_Read32( hXpt->hRegister, ExclRegAddr );
            Filter->Exclusion[ i ] = ( uint8_t ) (( Reg >> 24 ) & 0xFF );                   
            Filter->Exclusion[ i + 1 ] = ( uint8_t ) (( Reg >> 16 ) & 0xFF );                   
            Filter->Exclusion[ i + 2 ] = ( uint8_t ) (( Reg >> 8 ) & 0xFF );                    
            Filter->Exclusion[ i + 3 ] = ( uint8_t ) ( Reg & 0xFF );                    
        }
    }

    return( ExitCode );
}

BERR_Code BXPT_SetFilter( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int Bank,              /* [in] Which bank the filter belongs to. */
    unsigned int FilterNum,         /* [in] Which filter to get data from. */
    const BXPT_Filter *Filter   /* [in] Filter data to be loaded */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( Filter );

    /* Sanity check on the arguments. */
    if( Bank >= hXpt->MaxFilterBanks )     
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "Bank %lu is out of range!", ( unsigned long ) Bank ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else if( FilterNum >= BXPT_P_FILTER_TABLE_SIZE )       
    {
        /* Bad filter number. Complain. */
        BDBG_ERR(( "FilterNum %lu is out of range!", ( unsigned long ) FilterNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        unsigned int i, GroupSel;

        uint32_t CoefRegAddr = 0, MaskRegAddr = 0, ExclRegAddr = 0;
        
        if (BXPT_FILTER_SIZE == 32)
        {
            /* 
            ** Logical bank 0 maps to filter group 8B0. 
            ** Logical bank 1 maps to filter grouo 4B8.
            */
            GroupSel = Bank * 256;
        }
        else
        {
            /* 
            ** Logical bank 0 maps to filter group 4B0. 
            ** Logical bank 1 maps to filter grouo 4B4.
            */
            GroupSel = Bank * 128;
        }

        for( i = 0; i < BXPT_FILTER_SIZE; i += 4 )
        {
            uint32_t Reg;

            GetFilterAddrs( GroupSel, FilterNum, &CoefRegAddr, &MaskRegAddr, &ExclRegAddr );
            GroupSel += 32; /* Step to the next bank in the cascade */

            Reg = ( Filter->Coeficient[ i ] << 24 );
            Reg |= ( Filter->Coeficient[ i + 1 ] << 16 );
            Reg |= ( Filter->Coeficient[ i + 2 ] << 8 );
            Reg |= Filter->Coeficient[ i + 3 ];
            BREG_Write32( hXpt->hRegister, CoefRegAddr, Reg );

            Reg = ( Filter->Mask[ i ] << 24 );
            Reg |= ( Filter->Mask[ i + 1 ] << 16 );
            Reg |= ( Filter->Mask[ i + 2 ] << 8 );
            Reg |= Filter->Mask[ i + 3 ];
            BREG_Write32( hXpt->hRegister, MaskRegAddr, Reg );

            Reg = ( Filter->Exclusion[ i ] << 24 );
            Reg |= ( Filter->Exclusion[ i + 1 ] << 16 );
            Reg |= ( Filter->Exclusion[ i + 2 ] << 8 );
            Reg |= Filter->Exclusion[ i + 3 ];
            BREG_Write32( hXpt->hRegister, ExclRegAddr, Reg );
        }
    }

    return( ExitCode );
}

BERR_Code BXPT_ChangeFilterCoefficientByte( 
    BXPT_Handle hXpt,       /* [in] Handle for this transport */
    unsigned int Bank,          /* [in] Which bank the filter belongs to. */
    unsigned int FilterNum,     /* [in] Which filter to change. */
    unsigned int ByteOffset,        /* [in] Which byte in the array to change */
    uint8_t FilterByte      /* [in] New filter byte to be written. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    /* Sanity check on the arguments. */
    if( Bank >= hXpt->MaxFilterBanks )     
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "Bank %lu is out of range!", ( unsigned long ) Bank ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        unsigned int GroupSel;

        uint32_t RegAddr = 0;
        unsigned PidChnlNum = GetPidChannelByFilter( hXpt, Bank, FilterNum );

        if (BXPT_FILTER_SIZE == 32)
        {
            /* 
            ** Logical bank 0 maps to filter group 8B0. 
            ** Logical bank 1 maps to filter grouo 4B8.
            */
            GroupSel = Bank * 256;
        }
        else
        {
            /* 
            ** Logical bank 0 maps to filter group 4B0. 
            ** Logical bank 1 maps to filter grouo 4B4.
            */
            GroupSel = Bank * 128;
        }
    
        if( PidChnlNum != BXPT_P_MAX_PID_CHANNELS )
            BXPT_P_DisableFilter( hXpt, FilterNum, PidChnlNum );

        GetFilterAddrs( GroupSel, FilterNum, &RegAddr, NULL, NULL );
        ExitCode = BERR_TRACE( ChangeFilterByte( hXpt, RegAddr, FilterNum, ByteOffset, FilterByte ) );

        if( PidChnlNum != BXPT_P_MAX_PID_CHANNELS )
            EnableFilter( hXpt, FilterNum, PidChnlNum );
    }
        
    return( ExitCode );
}

BERR_Code BXPT_ChangeFilterMaskByte( 
    BXPT_Handle hXpt,       /* [in] Handle for this transport */
    unsigned int Bank,          /* [in] Which bank the filter belongs to. */
    unsigned int FilterNum,     /* [in] Which filter to change. */
    unsigned int ByteOffset,        /* [in] Which byte in the array to change */
    uint8_t MaskByte        /* [in] New mask byte to be written. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    /* Sanity check on the arguments. */
    if( Bank >= hXpt->MaxFilterBanks )     
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "Bank %lu is out of range!", ( unsigned long ) Bank ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        unsigned int GroupSel;

        uint32_t RegAddr = 0;
        unsigned PidChnlNum = GetPidChannelByFilter( hXpt, Bank,  FilterNum );

        if (BXPT_FILTER_SIZE == 32)
        {
            /* 
            ** Logical bank 0 maps to filter group 8B0. 
            ** Logical bank 1 maps to filter grouo 4B8.
            */
            GroupSel = Bank * 256;
        }
        else
        {
            /* 
            ** Logical bank 0 maps to filter group 4B0. 
            ** Logical bank 1 maps to filter grouo 4B4.
            */
            GroupSel = Bank * 128;
        }
        if( PidChnlNum != BXPT_P_MAX_PID_CHANNELS )
            BXPT_P_DisableFilter( hXpt, FilterNum, PidChnlNum );

        GetFilterAddrs( GroupSel, FilterNum, NULL, &RegAddr, NULL );
        ExitCode = BERR_TRACE( ChangeFilterByte( hXpt, RegAddr, FilterNum, ByteOffset, MaskByte ) );

        if( PidChnlNum != BXPT_P_MAX_PID_CHANNELS )
            EnableFilter( hXpt, FilterNum, PidChnlNum );
    }
        
    return( ExitCode );
}

BERR_Code BXPT_ChangeFilterExclusionByte( 
    BXPT_Handle hXpt,       /* [in] Handle for this transport */
    unsigned int Bank,          /* [in] Which bank the filter belongs to. */
    unsigned int FilterNum,     /* [in] Which filter to change. */
    unsigned int ByteOffset,        /* [in] Which byte in the array to change */
    uint8_t ExclusionByte       /* [in] New exclusion byte to be written. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    /* Sanity check on the arguments. */
    if( Bank >= hXpt->MaxFilterBanks )     
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "Bank %lu is out of range!", ( unsigned long ) Bank ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        unsigned int GroupSel;

        uint32_t RegAddr = 0;
        unsigned PidChnlNum = GetPidChannelByFilter( hXpt, Bank,  FilterNum );

        if (BXPT_FILTER_SIZE == 32)
        {
            /* 
            ** Logical bank 0 maps to filter group 8B0. 
            ** Logical bank 1 maps to filter grouo 4B8.
            */
            GroupSel = Bank * 256;
        }
        else
        {
            /* 
            ** Logical bank 0 maps to filter group 4B0. 
            ** Logical bank 1 maps to filter grouo 4B4.
            */
            GroupSel = Bank * 128;
        }
    
        if( PidChnlNum != BXPT_P_MAX_PID_CHANNELS )
            BXPT_P_DisableFilter( hXpt, FilterNum, PidChnlNum );

        GetFilterAddrs( GroupSel, FilterNum, NULL, NULL, &RegAddr );
        ExitCode = BERR_TRACE( ChangeFilterByte( hXpt, RegAddr, FilterNum, ByteOffset, ExclusionByte ) );

        if( PidChnlNum != BXPT_P_MAX_PID_CHANNELS )
            EnableFilter( hXpt, FilterNum, PidChnlNum );
    }
        
    return( ExitCode );
}

#if BXPT_HAS_PID2BUF_MAPPING
BERR_Code BXPT_Mesg_AddFilterToGroup(
    BXPT_Handle hXpt,                   /* [in] Handle for this transport */
    unsigned int PidChannelNum,         /* [in] Pid channel number */
    unsigned int MesgBufferNum,         /* [in] Message Buffer number */
    unsigned int FilterNum,             /* [in] Which filter to add. */
    BXPT_PsiMessageSettings *Settings   /* [in] Filter group to add to. */
    )
{
    uint32_t RegAddr;
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( Settings );

    /* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )
    {
        /* Bad PID channel number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
	else if ( MesgBufferNum >= BXPT_P_MAX_MESG_BUFFERS ) {
        /* Bad Message Buffer number. Complain. */
        BDBG_ERR(( "Message Buffer Number %lu is out of range!", ( unsigned long ) MesgBufferNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
    else if ( hXpt->Pid2BuffMappingOn == true) 
    {
        PidChannelTableEntry *Entry = &hXpt->PidChannelTable[ PidChannelNum ];

        if( Entry->IsAllocated == true )
        {
            Settings->FilterEnableMask |= ( 1ul << FilterNum );

            RegAddr = BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE + ( MesgBufferNum * GEN_FILT_EN_STEP );

            BXPT_P_DisableFilter( hXpt, FilterNum, MesgBufferNum );

            BREG_Write32( hXpt->hRegister, RegAddr, Settings->FilterEnableMask );

            /* EnableFilter( hXpt, FilterNum, PidChannelNum ); */
        }
        else
        {
            ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
            BDBG_ERR(("PidChannel %u not allocated\n", PidChannelNum ));
        }
    }
	else
	{
        /* Wrong function call. Complain. */
	BDBG_ERR(( "Pid2BuffMapping is OFF. Please use BXPT_AddFilterToGroup instead of BXPT_Mesg_AddFilterToGroup. " ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}

    return( ExitCode );
}
#endif

BERR_Code BXPT_AddFilterToGroup(
    BXPT_Handle hXpt,                   /* [in] Handle for this transport */
    unsigned int PidChannelNum,         /* [in] Pid channel number */
    unsigned int FilterNum,             /* [in] Which filter to add. */
    BXPT_PsiMessageSettings *Settings   /* [in] Filter group to add to. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;
    uint32_t RegAddr;
    
    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( Settings );

	if (hXpt->Pid2BuffMappingOn == true )
	{
        /* Wrong function call. Complain. */
	BDBG_ERR(( "Pid2BuffMapping is ON. Please use BXPT_Mesg_AddFilterToGroup instead of BXPT_AddFilterToGroup. " ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
		return (ExitCode);
	}

    if( PidChannelNum < BXPT_P_MAX_PID_CHANNELS )
    {
        PidChannelTableEntry *Entry = &hXpt->PidChannelTable[ PidChannelNum ];

        if( Entry->IsAllocated == true )
        {
            Settings->FilterEnableMask |= ( 1ul << FilterNum );

            RegAddr = BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE + ( PidChannelNum * GEN_FILT_EN_STEP );

            BXPT_P_DisableFilter( hXpt, FilterNum, PidChannelNum );

            BREG_Write32( hXpt->hRegister, RegAddr, Settings->FilterEnableMask );

            /* EnableFilter( hXpt, FilterNum, PidChannelNum ); */
        }
        else
        {
            ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
            BDBG_ERR(("PidChannel %u not allocated\n", PidChannelNum ));
        }
        
    }
    else
    {
        BDBG_ERR(("Invalid PidChannelNumber (%d : max = %d)\n", PidChannelNum, BXPT_P_MAX_PID_CHANNELS));   
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    return( ExitCode );

}

BERR_Code BXPT_RemoveFilterFromGroup(
    BXPT_Handle hXpt,               /* [in] Handle for this transport */
    unsigned int FilterNum,                 /* [in] Which filter to remove. */
    BXPT_PsiMessageSettings *Settings   /* [in] Filter group to add to. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    uint32_t RegAddr;
    unsigned int PidChannelNum;
    
    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( Settings );

    PidChannelNum = GetPidChannelByFilter( hXpt, Settings->Bank, FilterNum );

    if( PidChannelNum < BXPT_P_MAX_PID_CHANNELS )
    {
        Settings->FilterEnableMask &= ~( 1ul << FilterNum );
        RegAddr = BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE + ( PidChannelNum * GEN_FILT_EN_STEP );

        BXPT_P_DisableFilter( hXpt, FilterNum, PidChannelNum );

        BREG_Write32( hXpt->hRegister, RegAddr, Settings->FilterEnableMask );
    }
    else
    {
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
        BDBG_ERR(("Incorrect PidChannelNumber (%d)\n", PidChannelNum ));
    }

    return( ExitCode );
}

#if BXPT_HAS_PID2BUF_MAPPING
BERR_Code BXPT_Mesg_StartPidChannelRecord( 
    BXPT_Handle hXpt,                               /* [in] Handle for this transport */
    unsigned int PidChannelNum,                         /* [in] Which PID channel. */
    unsigned int MesgBufferNum,                         /* [in] Which Buffer Number. */
    const BXPT_PidChannelRecordSettings *Settings   /* [in] Record settings. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( Settings );

    /* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )
    {
        /* Bad PID channel number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
	else if ( MesgBufferNum >= BXPT_P_MAX_MESG_BUFFERS ) {
        /* Bad Message Buffer number. Complain. */
        BDBG_ERR(( "Message Buffer Number %lu is out of range!", ( unsigned long ) MesgBufferNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else if( hXpt->MesgBufferIsInitialized[ MesgBufferNum ] == false )
	{
		BDBG_ERR(( "Message buffer for this channel not configured!" ));
		ExitCode = BERR_TRACE( BXPT_ERR_MESG_BUFFER_NOT_CONFIGURED );
	}
    else if ( hXpt->Pid2BuffMappingOn == true) 
    {
		uint32_t Reg;

		uint32_t DataOutputMode = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_NO_OUTPUT;
		uint32_t RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( MesgBufferNum * PID_CTRL1_TABLE_STEP );

		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		Reg &= ~(
			BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, SPECIAL_SEL ) |
			BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, ALIGN_MODE ) |
			BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE ) 
		);

		/* Disable the PSI filters for this PID channel. Older chip didn't allow PSI filtering of PES or PacketSaveAll data. */
		BREG_Write32( hXpt->hRegister, BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE + ( MesgBufferNum * GEN_FILT_EN_STEP ), 0 );       

		/* Configure the message buffer for the type of recording. */
		switch( Settings->RecordType )
		{
			case BXPT_SingleChannelRecordType_ePes:   
			DataOutputMode = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PES;
			break;

			case BXPT_SingleChannelRecordType_ePacketSaveAll:
			DataOutputMode = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_TS;
			break;

			case BXPT_SingleChannelRecordType_ePesSaveAll:
			DataOutputMode = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PAYLOAD;
			break;

			default:
			BDBG_ERR(( "Unsupported/illegal RecordType: %u!", ( unsigned ) Settings->RecordType ));
			goto Done;
		}

		Reg |= (
			BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, ALIGN_MODE, Settings->ByteAlign == true ? 0 : 1 ) |
			BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE, DataOutputMode ) 
		);
		BREG_Write32( hXpt->hRegister, RegAddr, Reg );
  		if(hXpt->PidChannelTable[ PidChannelNum ].MessageBuffercount == 0)
		{
		    /* Enable data from the PID channel to the mesg block. */
		    BXPT_P_SetPidChannelDestination( hXpt, PidChannelNum, hXpt->MesgDataOnRPipe == true ? 7 : 6, true );

		    /* Set this PID channel as allocated, in case they forced the channel assignment. */
		    hXpt->PidChannelTable[ PidChannelNum ].IsAllocated = true;

		    /* Configure the PID channel.  */
		    ExitCode = BXPT_ConfigurePidChannel( hXpt, PidChannelNum, Settings->Pid, Settings->Band );
		    if( ExitCode != BERR_SUCCESS )
			    goto Done;

            /* Enable the PID channel. */ 
#if BXPT_MESG_DONT_AUTO_ENABLE_PID_CHANNEL
        /* do nothing. must be explicitly enabled. */
#else
            ExitCode = BERR_TRACE( BXPT_EnablePidChannel( hXpt, PidChannelNum ) );
#endif
   		}

        ConfigPid2BufferMap( hXpt, PidChannelNum, MesgBufferNum, true);
		hXpt->PidChannelTable[ PidChannelNum ].MessageBuffercount++;
    }
	else
	{
        /* Wrong function call. Complain. */
	BDBG_ERR(( "Pid2BuffMapping is OFF. Please use BXPT_StartPidChannelRecord instead of BXPT_Mesg_StartPidChannelRecord. " ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}

    Done:
    return( ExitCode );
}
#endif

BERR_Code BXPT_StartPidChannelRecord( 
    BXPT_Handle hXpt,                               /* [in] Handle for this transport */
    unsigned int PidChannelNum,                         /* [in] Which PID channel. */
    const BXPT_PidChannelRecordSettings *Settings   /* [in] Record settings. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( Settings );


	if (hXpt->Pid2BuffMappingOn == true )
	{
        /* Wrong function call. Complain. */
        BDBG_ERR(( "Pid2BuffMapping is ON. You might need to call BXPT_SetPid2Buff() to turn it OFF" ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
		return (ExitCode);
	}

    /* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )    
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else if( hXpt->MesgBufferIsInitialized[ PidChannelNum ] == false )
    {
        BDBG_ERR(( "Message buffer for this channel not configured!" ));
        ExitCode = BERR_TRACE( BXPT_ERR_MESG_BUFFER_NOT_CONFIGURED );
    }
    else
    {
        uint32_t Reg;
        
        uint32_t DataOutputMode = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_NO_OUTPUT;
        uint32_t RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CTRL1_TABLE_STEP );

        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        Reg &= ~(
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, SPECIAL_SEL ) |
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, ALIGN_MODE ) |
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE ) 
        );

        /* Disable the PSI filters for this PID channel. Older chip didn't allow PSI filtering of PES or PacketSaveAll data. */
        BREG_Write32( hXpt->hRegister, BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE + ( PidChannelNum * GEN_FILT_EN_STEP ), 0 );       

        /* Configure the message buffer for the type of recording. */
        switch( Settings->RecordType )
        {
            case BXPT_SingleChannelRecordType_ePes:   
            DataOutputMode = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PES;
            break;

            case BXPT_SingleChannelRecordType_ePacketSaveAll:
            DataOutputMode = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_TS;
            break;

            case BXPT_SingleChannelRecordType_ePesSaveAll:
            DataOutputMode = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PAYLOAD;
            break;

            default:
            BDBG_ERR(( "Unsupported/illegal RecordType: %u!", ( unsigned ) Settings->RecordType ));
            goto Done;
        }

        Reg |= (
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, ALIGN_MODE, Settings->ByteAlign == true ? 0 : 1 ) |
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE, DataOutputMode ) 
        );
        BREG_Write32( hXpt->hRegister, RegAddr, Reg );

        /* Enable data from the PID channel to the mesg block. */
		BXPT_P_SetPidChannelDestination( hXpt, PidChannelNum, hXpt->MesgDataOnRPipe == true ? 7 : 6, true );

        /* Set this PID channel as allocated, in case they forced the channel assignment. */
        hXpt->PidChannelTable[ PidChannelNum ].IsAllocated = true;
    
        /* Configure the PID channel.  */
        ExitCode = BXPT_ConfigurePidChannel( hXpt, PidChannelNum, Settings->Pid, Settings->Band );
        if( ExitCode != BERR_SUCCESS )
            goto Done;

        /* Enable the PID channel. */ 
#if BXPT_MESG_DONT_AUTO_ENABLE_PID_CHANNEL
        /* do nothing. must be explicitly enabled. */
#else
        ExitCode = BERR_TRACE( BXPT_EnablePidChannel( hXpt, PidChannelNum ) );
#endif
    }

    Done:
    return( ExitCode );
}

#if BXPT_HAS_PID2BUF_MAPPING
BERR_Code BXPT_Mesg_StopPidChannelRecord( 
    BXPT_Handle hXpt,                               /* [in] Handle for this transport */
    unsigned int PidChannelNum,                      /* [in] Which PID channel. */
    unsigned int MesgBufferNum                      /* [in] Which Buffer Number. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    /* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )
    {
        /* Bad PID channel number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
	else if ( MesgBufferNum >= BXPT_P_MAX_MESG_BUFFERS ) {
        /* Bad Message Buffer number. Complain. */
        BDBG_ERR(( "Message Buffer Number %lu is out of range!", ( unsigned long ) MesgBufferNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
    else if ( hXpt->Pid2BuffMappingOn == true) 
    {

        uint32_t Reg, RegAddr;

        /* Disable data from the PID channel to the mesg block. */
	hXpt->PidChannelTable[ PidChannelNum ].MessageBuffercount--;
	if(hXpt->PidChannelTable[ PidChannelNum ].MessageBuffercount == 0)
	{
		BXPT_P_SetPidChannelDestination( hXpt, PidChannelNum, hXpt->MesgDataOnRPipe == true ? 7 : 6, false );
	}

        /* Disable message buffer output */
        RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( MesgBufferNum * PID_CTRL1_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        Reg &= ~(
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE ) 
        );
        Reg |= (
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE, BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_NO_OUTPUT )
        );
        BREG_Write32( hXpt->hRegister, RegAddr, Reg );

        RegAddr = BCHP_XPT_MSG_PID_CTRL2_TABLE_i_ARRAY_BASE + ( MesgBufferNum * PID_CTRL2_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        Reg &= ~(
            BCHP_MASK( XPT_MSG_PID_CTRL2_TABLE_i, FILTER_MODE ) 
        );
        Reg |= (
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL2_TABLE_i, FILTER_MODE, 0 )
        );
        BREG_Write32( hXpt->hRegister, RegAddr, Reg );
    
        /* Write the PID channel number to the buffer reset register. Writes trigger the reset. */
        BREG_Write32( hXpt->hRegister, BCHP_XPT_MSG_DMA_BUFFER_RESET, MesgBufferNum );

        /** Clear the read pointer. **/
        RegAddr = BCHP_XPT_MSG_DMA_RP_TABLE_i_ARRAY_BASE + ( MesgBufferNum * RP_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        Reg &= ~BCHP_MASK( XPT_MSG_DMA_RP_TABLE_i, READ_POINTER ); 
        BREG_Write32( hXpt->hRegister, RegAddr, Reg );

		/* Clear the pid2buf mapping */
		ConfigPid2BufferMap(hXpt, PidChannelNum, MesgBufferNum, false);
    }
	else
	{
        /* Wrong function call. Complain. */
	BDBG_ERR(( "Pid2BuffMapping is OFF. Please use BXPT_StopPidChannelRecord instead of BXPT_Mesg_StopPidChannelRecord. " ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}

    return( ExitCode );
}
#endif

BERR_Code BXPT_StopPidChannelRecord( 
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    unsigned int PidChannelNum                      /* [in] Which PID channel. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	if (hXpt->Pid2BuffMappingOn == true )
	{
        /* Wrong function call. Complain. */
        BDBG_ERR(( "Pid2BuffMapping is ON. You might need to call BXPT_SetPid2Buff() to turn it OFF" ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
		return (ExitCode);
	}

	/* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )    
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        uint32_t Reg, RegAddr;

        /* Disable data from the PID channel to the mesg block. */
		BXPT_P_SetPidChannelDestination( hXpt, PidChannelNum, hXpt->MesgDataOnRPipe == true ? 7 : 6, false );

        /* Disable message buffer output */
        RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CTRL1_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        Reg &= ~(
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE ) 
        );
        Reg |= (
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE, BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_NO_OUTPUT )
        );
        BREG_Write32( hXpt->hRegister, RegAddr, Reg );

        RegAddr = BCHP_XPT_MSG_PID_CTRL2_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CTRL2_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        Reg &= ~(
            BCHP_MASK( XPT_MSG_PID_CTRL2_TABLE_i, FILTER_MODE ) 
        );
        Reg |= (
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL2_TABLE_i, FILTER_MODE, 0 )
        );
        BREG_Write32( hXpt->hRegister, RegAddr, Reg );
    
        /* Write the PID channel number to the buffer reset register. Writes trigger the reset. */
        BREG_Write32( hXpt->hRegister, BCHP_XPT_MSG_DMA_BUFFER_RESET, PidChannelNum );

        /** Clear the read pointer. **/
        RegAddr = BCHP_XPT_MSG_DMA_RP_TABLE_i_ARRAY_BASE + ( PidChannelNum * RP_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        Reg &= ~BCHP_MASK( XPT_MSG_DMA_RP_TABLE_i, READ_POINTER ); 
        BREG_Write32( hXpt->hRegister, RegAddr, Reg );
    }

    return( ExitCode );
}

#if BXPT_HAS_PID2BUF_MAPPING
BERR_Code BXPT_Mesg_StartPsiMessageCapture( 
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    unsigned int PidChannelNum,                     /* [in] Which PID channel. */
    unsigned int MesgBufferNum,                     /* [in] Which Message Buffer. */
    const BXPT_PsiMessageSettings *Settings     /* [in] PID, band, and filters to use. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( Settings );

    /* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )
    {
        /* Bad PID channel number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else if( hXpt->MesgBufferIsInitialized[ MesgBufferNum ] == false )
    {
        BDBG_ERR(( "Message buffer for this channel not configured!" ));
        ExitCode = BERR_TRACE( BXPT_ERR_MESG_BUFFER_NOT_CONFIGURED );
    }
	else if ( MesgBufferNum >= BXPT_P_MAX_MESG_BUFFERS ) {
        /* Bad Message Buffer number. Complain. */
        BDBG_ERR(( "Message Buffer Number %lu is out of range!", ( unsigned long ) MesgBufferNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else if ( Settings->StartingOffset > 255 ) 
    {
        BDBG_ERR(( "StartingOffset %lu is out of range!", ( unsigned long ) Settings->StartingOffset ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
    else if ( hXpt->Pid2BuffMappingOn == true) 
    {
        uint32_t Reg, RegAddr;
        unsigned int GroupSel = BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G8_B0;

        ExitCode = BXPT_P_GetGroupSelect( Settings->Bank, &GroupSel );
        if( ExitCode != BERR_SUCCESS )
            goto Done;

        /* Select the bank the filters are in. */
        RegAddr = BCHP_XPT_MSG_PID_CTRL2_TABLE_i_ARRAY_BASE + ( MesgBufferNum * PID_CTRL2_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        Reg &= ~(
            BCHP_MASK( XPT_MSG_PID_CTRL2_TABLE_i, SPECIAL_TYPE ) |
            BCHP_MASK( XPT_MSG_PID_CTRL2_TABLE_i, GEN_OFFSET ) |
            BCHP_MASK( XPT_MSG_PID_CTRL2_TABLE_i, SKIP_BYTE2 ) |
            BCHP_MASK( XPT_MSG_PID_CTRL2_TABLE_i, GEN_GRP_SEL ) 
        );
        Reg |= (
            BCHP_FIELD_DATA( XPT_MSG_BUF_CTRL2_TABLE_i, GEN_OFFSET, Settings->StartingOffset ) |
            BCHP_FIELD_DATA( XPT_MSG_BUF_CTRL2_TABLE_i, SKIP_BYTE2, Settings->SkipByte2 == true ? 1 : 0 ) |
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL2_TABLE_i, GEN_GRP_SEL, GroupSel )
        );
        BREG_Write32( hXpt->hRegister, RegAddr, Reg );

        /* Enable the filters for this PID channel. */
        BREG_Write32( hXpt->hRegister, BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE + ( MesgBufferNum * GEN_FILT_EN_STEP ), Settings->FilterEnableMask );      

        RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( MesgBufferNum * PID_CTRL1_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        Reg &= ~(
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, ERR_CK_MODE ) |
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, ERR_CK_DIS ) |
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, SPECIAL_SEL ) |
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, ALIGN_MODE ) |
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE ) 
        );
    
        /* Configure the message buffer for capturing PSI messages. */
        Reg |= (
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, ALIGN_MODE, Settings->ByteAlign == true ? 0 : 1 ) |
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE, BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PSI ) 
        );

        /* CRC checks can be disabled on a per-PID channel basis. Do this here if requested in the settings struct. */
        Reg |= (
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, ERR_CK_MODE, 0 ) |
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, ERR_CK_DIS, Settings->CrcDisable == true ? 1 : 0 ) 
        );
        BREG_Write32( hXpt->hRegister, RegAddr, Reg );

        /* Enable data from the PID channel to the mesg block. */
	if(hXpt->PidChannelTable[ PidChannelNum ].MessageBuffercount == 0)
   	{
		BXPT_P_SetPidChannelDestination( hXpt, PidChannelNum, hXpt->MesgDataOnRPipe == true ? 7 : 6, true );
    
        /* Set this PID channel as allocated, in case they forced the channel assignment. */
        hXpt->PidChannelTable[ PidChannelNum ].IsAllocated = true;

        /* Configure the PID channel. */
        ExitCode = BXPT_ConfigurePidChannel( hXpt, PidChannelNum, Settings->Pid, Settings->Band );
        if( ExitCode != BERR_SUCCESS )
            goto Done;
   	}
        /* 
        ** Some filtering configs were done in the parser band on older chips. Now, those
        ** configs are done per-PID channel. 
        */
        BXPT_P_ApplyParserPsiSettings( hXpt, Settings->Band, false );   /* Check IB parsers */

        BXPT_P_ApplyParserPsiSettings( hXpt, Settings->Band, true );    /* Check playback parsers */

        ConfigPid2BufferMap( hXpt, PidChannelNum, MesgBufferNum, true);

        /* Enable the PID channel. */ 
#if BXPT_MESG_DONT_AUTO_ENABLE_PID_CHANNEL
        /* do nothing. must be explicitly enabled. */
#else
	    if(hXpt->PidChannelTable[ PidChannelNum ].MessageBuffercount == 0)
	    {
            ExitCode = BERR_TRACE( BXPT_EnablePidChannel( hXpt, PidChannelNum ) );
	    }
#endif
	    hXpt->PidChannelTable[ PidChannelNum ].MessageBuffercount++;
    }
	else
	{
        /* Wrong function call. Complain. */
	BDBG_ERR(( "Pid2BuffMapping is OFF. Please use BXPT_StartPsiMessageCapture instead of BXPT_Mesg_StartPsiMessageCapture. " ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}

	Done:
    return( ExitCode );
}
#endif

BERR_Code BXPT_StartPsiMessageCapture( 
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    unsigned int PidChannelNum,                     /* [in] Which PID channel. */
    const BXPT_PsiMessageSettings *Settings     /* [in] PID, band, and filters to use. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( Settings );

	if (hXpt->Pid2BuffMappingOn == true )
	{
        /* Wrong function call. Complain. */
	BDBG_ERR(( "Pid2BuffMapping is ON. Please use BXPT_Mesg_StartPsiMessageCapture instead of BXPT_StartPsiMessageCapture. " ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
		return (ExitCode);
	}

	/* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )    
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else if( hXpt->MesgBufferIsInitialized[ PidChannelNum ] == false )
    {
        BDBG_ERR(( "Message buffer for this channel not configured!" ));
        ExitCode = BERR_TRACE( BXPT_ERR_MESG_BUFFER_NOT_CONFIGURED );
    }
	else if ( Settings->StartingOffset > 255 ) 
    {
        BDBG_ERR(( "StartingOffset %lu is out of range!", ( unsigned long ) Settings->StartingOffset ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
    else
    {
        uint32_t Reg, RegAddr;

        unsigned int GroupSel = BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G8_B0;

        ExitCode = BXPT_P_GetGroupSelect( Settings->Bank, &GroupSel );
        if( ExitCode != BERR_SUCCESS )
            goto Done;

        /* Select the bank the filters are in. */
        RegAddr = BCHP_XPT_MSG_PID_CTRL2_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CTRL2_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        Reg &= ~(
            BCHP_MASK( XPT_MSG_PID_CTRL2_TABLE_i, SPECIAL_TYPE ) |
            BCHP_MASK( XPT_MSG_PID_CTRL2_TABLE_i, GEN_OFFSET ) |
            BCHP_MASK( XPT_MSG_PID_CTRL2_TABLE_i, SKIP_BYTE2 ) |
            BCHP_MASK( XPT_MSG_PID_CTRL2_TABLE_i, GEN_GRP_SEL ) 
        );
        Reg |= (
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL2_TABLE_i, GEN_OFFSET, Settings->StartingOffset ) |
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL2_TABLE_i, SKIP_BYTE2, 1 ) |
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL2_TABLE_i, GEN_GRP_SEL, GroupSel )
        );
        BREG_Write32( hXpt->hRegister, RegAddr, Reg );

        /* Enable the filters for this PID channel. */
        BREG_Write32( hXpt->hRegister, BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE + ( PidChannelNum * GEN_FILT_EN_STEP ), Settings->FilterEnableMask );      

        RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CTRL1_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        Reg &= ~(
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, ERR_CK_MODE ) |
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, ERR_CK_DIS ) |
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, SPECIAL_SEL ) |
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, ALIGN_MODE ) |
            BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE ) 
        );
    
        /* Configure the message buffer for capturing PSI messages. */
        Reg |= (
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, ALIGN_MODE, Settings->ByteAlign == true ? 0 : 1 ) |
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE, BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PSI ) 
        );

        /* CRC checks can be disabled on a per-PID channel basis. Do this here if requested in the settings struct. */
        Reg |= (
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, ERR_CK_MODE, 0 ) |
            BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, ERR_CK_DIS, Settings->CrcDisable == true ? 1 : 0 ) 
        );
        BREG_Write32( hXpt->hRegister, RegAddr, Reg );

        /* Enable data from the PID channel to the mesg block. */
		BXPT_P_SetPidChannelDestination( hXpt, PidChannelNum, hXpt->MesgDataOnRPipe == true ? 7 : 6, true );
    
        /* Set this PID channel as allocated, in case they forced the channel assignment. */
        hXpt->PidChannelTable[ PidChannelNum ].IsAllocated = true;

        /* Configure the PID channel. */
        ExitCode = BXPT_ConfigurePidChannel( hXpt, PidChannelNum, Settings->Pid, Settings->Band );
        if( ExitCode != BERR_SUCCESS )
            goto Done;

        /* 
        ** Some filtering configs were done in the parser band on older chips. Now, those
        ** configs are done per-PID channel. 
        */
        BXPT_P_ApplyParserPsiSettings( hXpt, Settings->Band, false );   /* Check IB parsers */

        BXPT_P_ApplyParserPsiSettings( hXpt, Settings->Band, true );    /* Check playback parsers */

        /* Enable the PID channel. */ 
#if BXPT_MESG_DONT_AUTO_ENABLE_PID_CHANNEL
        /* do nothing. must be explicitly enabled. */
#else
        ExitCode = BERR_TRACE( BXPT_EnablePidChannel( hXpt, PidChannelNum ) );
#endif
    }

    Done:
    return( ExitCode );
}

#if BXPT_HAS_PID2BUF_MAPPING
BERR_Code BXPT_Mesg_StopPsiMessageCapture( 
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    unsigned int PidChannelNum,                  /* [in] Which PID channel. */
    unsigned int MesgBufferNum                  /* [in] Which PID channel. */
    )
{
    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    /* Turns out this is really easy.. */
    return( BERR_TRACE( BXPT_Mesg_StopPidChannelRecord( hXpt, PidChannelNum, MesgBufferNum )));
}
#endif

BERR_Code BXPT_StopPsiMessageCapture( 
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    unsigned int PidChannelNum                  /* [in] Which PID channel. */
    )
{
    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
	/* Turns out this is really easy.. */
    return( BERR_TRACE( BXPT_StopPidChannelRecord( hXpt, PidChannelNum )));
}

BERR_Code BXPT_CheckBufferWithWrap( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int MesgBufferNum, /* [in] buffer to check.*/
    uint8_t **BufferAddr,       /* [out] Address of the buffer. */
    size_t *MessageSizeBeforeWrap,        /* [out] Total size of new messages before the buffer wrapped. */
    uint8_t **WrapBufferAddr,    /* [out] Address of the wraparound data. 0 if no wrap */
    size_t *MessageSizeAfterWrap    /* Total size of message starting at the base. NULL if no wrap */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( BufferAddr );
    BDBG_ASSERT( MessageSizeBeforeWrap );
    BDBG_ASSERT( WrapBufferAddr );
    BDBG_ASSERT( MessageSizeAfterWrap );

    *BufferAddr = *WrapBufferAddr = ( uint8_t * ) NULL;
    *MessageSizeBeforeWrap = *MessageSizeAfterWrap = 0;

	if ( MesgBufferNum >= BXPT_P_MAX_MESG_BUFFERS ) {
        /* Bad Message Buffer number. Complain. */
        BDBG_ERR(( "Message Buffer Number %lu is out of range!", ( unsigned long ) MesgBufferNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
    else
    {
        uint32_t DataReady;

        uint32_t RegAddr = 0;
        uint32_t BitShift = 0;

        /* Interrupt registers are 32 bits wide, so step to the next one if needed. */
        AdjustChanAndAddr( hXpt, MesgBufferNum, BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31, &BitShift, &RegAddr ); 
        DataReady = BREG_Read32( hXpt->hRegister, RegAddr );
        DataReady &= 1ul << BitShift;

        /* Is there any new data in the buffer? */
        if ( DataReady )
        {
            uint32_t Reg, ReadPtr, ValidPtr, BufferStart, BufferSizeInBytes;
            size_t Dummy;

            /* Yes, there is data */

            GetBufferPointers( hXpt, MesgBufferNum, &ReadPtr, &ValidPtr, &Dummy, &BufferSizeInBytes );
            Reg = BREG_Read32( hXpt->hRegister, GetRegArrayAddr( hXpt, MesgBufferNum, BCHP_XPT_MSG_DMA_BP_TABLE_i_ARRAY_BASE ) );
            BufferStart = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_DMA_BP_TABLE_i, BP_BUFFER_BASE_ADDR );
            BufferStart *= BXPT_P_MESSAGE_BUFFER_BLOCK_SIZE;

            /* On the new chip arch Read==Valid is always empty */
            if ( ReadPtr == ValidPtr )
            {
                BDBG_MSG(( "(PR25533)Data Ready set,but buffer empty" ));
                Dummy = 0;   /* Placate the compiler. We've already set the pointers to NULL above. */
            }

            /* Did it wrap around? */
            else if ( ValidPtr < ReadPtr )
            {
                /* It wrapped, but its not full. */
                *MessageSizeBeforeWrap = (size_t) BufferSizeInBytes - ReadPtr;
                BERR_TRACE( BMEM_ConvertOffsetToAddress( hXpt->hMemory, BufferStart + ReadPtr, ( void ** ) BufferAddr ));

                *MessageSizeAfterWrap = ( size_t ) ValidPtr;
                if( *MessageSizeAfterWrap )
                {
                    BERR_TRACE( BMEM_ConvertOffsetToAddress( hXpt->hMemory, BufferStart, ( void ** ) WrapBufferAddr ));
                }
            }

            /* Not full or wrapped. */
            else
            {
                /* No wrap and not full. */
                *MessageSizeBeforeWrap =  ( size_t ) ( ValidPtr - ReadPtr );
                BERR_TRACE( BMEM_ConvertOffsetToAddress( hXpt->hMemory, BufferStart + ReadPtr, ( void ** ) BufferAddr ));
            }
        }
    }

    return( ExitCode );
}

BERR_Code BXPT_CheckBuffer( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int MesgBufferNum, /* [in] buffer to check.*/
    uint8_t **BufferAddr,       /* [out] Address of the buffer. */
    size_t *MessageSize,        /* [out] Total size of new messages. */
    bool *MoreDataAvailable     /* [out] TRUE if the buffer wrapped. */
    )
{
    uint8_t *WrapBufferAddr = NULL;
    size_t MessageSizeAfterWrap = 0;
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( BufferAddr );
    BDBG_ASSERT( MessageSize );
    BDBG_ASSERT( MoreDataAvailable );

    ExitCode = BXPT_CheckBufferWithWrap( hXpt, MesgBufferNum, BufferAddr, MessageSize, &WrapBufferAddr, &MessageSizeAfterWrap );
    if( WrapBufferAddr )
        *MoreDataAvailable = true;
    else
        *MoreDataAvailable = false;

    return ExitCode;
}

BERR_Code BXPT_GetBuffer( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int MesgBuffNum, /* [in] Message buffer to check. */
    uint8_t *BufferAddr,        /* [out] Address of the buffer. */
    size_t *UserBufferSize      /* [in,out] Size of message buffer (on input), size of new messages (on putput). */
    )
{
    uint32_t ReadPtr, ValidPtr;

    BERR_Code ExitCode = BERR_SUCCESS;
    size_t MessageSize = 0;     /* Init to no messages. */
    size_t DmaBufferSize = 0;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( BufferAddr );
    BDBG_ASSERT( UserBufferSize );
    
    GetBufferPointers( hXpt, MesgBuffNum, &ReadPtr, &ValidPtr, &MessageSize, &DmaBufferSize );
    
    /* Copy over as much data as will fit in the buffer.  */
    *UserBufferSize = ( *UserBufferSize < MessageSize ) ? *UserBufferSize : MessageSize;    
    CopyDmaDataToUser( hXpt, MesgBuffNum, ReadPtr, BufferAddr, *UserBufferSize, DmaBufferSize );

#if 0
    ExitCode = BXPT_UpdateReadOffset( hXpt, MesgBuffNum, MessageSize );
#else
    /* 
    ** The CDI driver uses buffers that are too small for the messages they want to capture, so don't update  
    ** the READ pointer past the end of the data they can take. They'll have to call this API again. See
    ** PR 34473 for details 
    */
    ExitCode = BXPT_UpdateReadOffset( hXpt, MesgBuffNum, *UserBufferSize ); 
#endif

    return( ExitCode );
}

BERR_Code BXPT_GetBuffer_isr(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int MesgBuffNum, /* [in] Message buffer to check. */
    uint8_t *BufferAddr,        /* [out] Address of the buffer. */
    size_t *UserBufferSize      /* [in,out] Size of message buffer (on input), size of new messages (on putput). */
    )
{
    uint32_t ReadPtr, ValidPtr;

    BERR_Code ExitCode = BERR_SUCCESS;
    size_t MessageSize = 0;     /* Init to no messages. */
    size_t DmaBufferSize = 0;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( BufferAddr );
    BDBG_ASSERT( UserBufferSize );

    GetBufferPointers( hXpt, MesgBuffNum, &ReadPtr, &ValidPtr, &MessageSize, &DmaBufferSize );

    /* Copy over as much data as will fit in the buffer.  */
    *UserBufferSize = ( *UserBufferSize < MessageSize ) ? *UserBufferSize : MessageSize;
    CopyDmaDataToUser( hXpt, MesgBuffNum, ReadPtr, BufferAddr, *UserBufferSize, DmaBufferSize );

    /*
    ** The CDI driver uses buffers that are too small for the messages they want to capture, so don't update
    ** the READ pointer past the end of the data they can take. They'll have to call this API again. See
    ** PR 34473 for details
    */
    ExitCode = BXPT_UpdateReadOffset_isr( hXpt, MesgBuffNum, *UserBufferSize );

    return( ExitCode );
}

BERR_Code BXPT_UpdateReadOffset( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int MesgBuffNum,     /* [in] Which mesg buffer to update. */
    size_t BytesRead            /* [in] Number of bytes read. */
    )
{
    BERR_Code ErrCode;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    BKNI_EnterCriticalSection();
    ErrCode = BXPT_UpdateReadOffset_isr( hXpt, MesgBuffNum, BytesRead ) ;
    BKNI_LeaveCriticalSection();
    return ErrCode;
}

BERR_Code BXPT_UpdateReadOffset_isr( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int MesgBuffNum,     /* [in] Which mesg buffer to update. */
    size_t BytesRead            /* [in] Number of bytes read. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    /* Sanity check on the arguments. */
	if ( MesgBuffNum >= BXPT_P_MAX_MESG_BUFFERS ) {
        /* Bad Message Buffer number. Complain. */
        BDBG_ERR(( "Message Buffer Number %lu is out of range!", ( unsigned long ) MesgBuffNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
    else
    {
        uint32_t Reg, ReadPtr, SizeField, i;

        uint32_t BufferSizeInBytes = 1024;
        uint32_t RpRegAddr = GetRegArrayAddr( hXpt, MesgBuffNum, BCHP_XPT_MSG_DMA_RP_TABLE_i_ARRAY_BASE );

        /* Get the read pointer. */
        Reg = BREG_Read32( hXpt->hRegister, RpRegAddr );
        ReadPtr = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_DMA_RP_TABLE_i, READ_POINTER );

        /* Compute the size of the buffer, in bytes. */
        Reg = BREG_Read32( hXpt->hRegister, GetRegArrayAddr( hXpt, MesgBuffNum, BCHP_XPT_MSG_DMA_BP_TABLE_i_ARRAY_BASE ) );
        SizeField = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_DMA_BP_TABLE_i, BP_BUFFER_SIZE );
        for( i = 0; i < SizeField; i++ )
            BufferSizeInBytes *= 2;
            
        if( BytesRead > BufferSizeInBytes )
        {
            BDBG_ERR(( "BytesRead %d is larger than the buffer %d for MesgBuffNum %lu!", BytesRead, BufferSizeInBytes, ( unsigned long ) MesgBuffNum )); 
            ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
        }
                        
        /* Add number of bytes we read to the pointer. */
        ReadPtr += BytesRead;

        /* Now check if that caused a wrap. */
        if( ReadPtr >= BufferSizeInBytes )
        {
            /* It did, so adjust the ReadPtr to be an offset from the start of the buffer. */
            ReadPtr -= BufferSizeInBytes;
        }
        
        /* Now write out the new ReadPtr. */
        Reg = BREG_Read32( hXpt->hRegister, RpRegAddr );
        Reg &= ~( BCHP_MASK( XPT_MSG_DMA_RP_TABLE_i, READ_POINTER ) );
        Reg |= ( BCHP_FIELD_DATA( XPT_MSG_DMA_RP_TABLE_i, READ_POINTER, ReadPtr ) );
        BREG_Write32( hXpt->hRegister, RpRegAddr, Reg );


        /* Due to current HW limitation the data ready interrupt data ready irpts are missed 
           if the valid pointer is updated within one clock cycle of the data read  
           pointer update. We need to check for this condition and force the cb */
        {
            uint32_t ReadPtr, ValidPtr;
            size_t BytesInBuffer, BufferSizeInBytes;
            bool DataReady = GetPidChannelFlag( hXpt, MesgBuffNum, BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31 );
            GetBufferPointers( hXpt, MesgBuffNum, &ReadPtr, &ValidPtr, &BytesInBuffer, &BufferSizeInBytes );
            if( !DataReady && BytesInBuffer)
            {
                /* we have the hw error, trigger the interrupt */
                BDBG_MSG(( "Data ready irpt lost,generating irpt cb in sw" ));
                BXPT_P_Interrupt_MsgSw_isr(hXpt,MesgBuffNum);
            }
        }
    }

    return( ExitCode );
}

BERR_Code BXPT_PausePesRecord(
    BXPT_Handle hXpt,                       /* [in] Handle for this transport */
    unsigned int MesgBuffNum,     /* [in] Which message buffer. */
    bool Pause                                /* [in] Enable or disable pause */
    )
{
    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    return( BXPT_P_PauseFilters( hXpt, MesgBuffNum, BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PES, Pause ));
}

BERR_Code BXPT_PausePsiCapture(
    BXPT_Handle hXpt,                       /* [in] Handle for this transport */
    unsigned int MesgBuffNum,             /* [in] Which message buffer. */
    bool Pause                                /* [in] Enable or disable pause */
    )
{
    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    return( BXPT_P_PauseFilters( hXpt, MesgBuffNum, BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_MPEG_PSI, Pause ));
}

#if BXPT_HAS_PID2BUF_MAPPING
BERR_Code BXPT_Mesg_SetPidChannelPsiSettings(
        BXPT_Handle hXpt,                  /* [In] Handle for this transport */
        unsigned int PidChannelNum,        /* [In] The pid channel to configure. */
        unsigned int MesgBufferNum,        /* [In] The Mesg Buffer NUmber to configure. */
        bool OverrideParserSettings,       /* [In] If set true the PSI settings for 
                                                   this pid channel will be changed to Config
                                                   else restored to the paser band to which 
                                                   this pid channel is allocated */
        const BXPT_PidPsiConfig *Config    /* [In] Message mode setting for pid channel. */
        )                            
{

    uint32_t Reg, RegAddr, PlaybackFeSel, ChnlParserNum;
    ParserConfig *PcPtr;
    BERR_Code ExitCode = BERR_SUCCESS;

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
	BXPT_P_PacketSubCfg PsubCfg;
#endif

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( Config );

    /* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )
    {
        /* Bad PID channel number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
	else if ( MesgBufferNum >= BXPT_P_MAX_MESG_BUFFERS ) {
        /* Bad Message Buffer number. Complain. */
        BDBG_ERR(( "Message Buffer Number %lu is out of range!", ( unsigned long ) MesgBufferNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
    else if ( hXpt->Pid2BuffMappingOn == true) 
    {
        RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_TABLE_STEP );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
		BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
		BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

        PlaybackFeSel = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL );
        ChnlParserNum = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT );

        if( PlaybackFeSel )
            PcPtr = &( hXpt->PbParserTable[ ChnlParserNum ] );
        else
            PcPtr = &( hXpt->IbParserTable[ ChnlParserNum ] );

        /* Yes, update the PSI config members in the PID channel. */                        
        RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( MesgBufferNum * PID_CTRL1_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );

        Reg &= ~(
                 BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_SHORT_PSI_EN ) |
                 BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_LONG_PSI_EN ) |
                 BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, PSF_CRC_DIS ) |
                 BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i,  MSG_MOD_MODE )
        );

        if(OverrideParserSettings)
        {
            /* reserve this pid channel to have PSI setting different from the
               pid parser */
            hXpt->PidChannelParserConfigOverride[MesgBufferNum] = true;
            Reg |= (
              BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_SHORT_PSI_EN, Config->SaveShortPsiMsg == true ? 1 : 0 )  |
              BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_LONG_PSI_EN, Config->SaveLongPsiMsg == true ? 1 : 0 ) |
              BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, PSF_CRC_DIS, Config->PsfCrcDis == true ? 1 : 0 )  |
              BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, MSG_MOD_MODE, Config->PsiMsgMod ) 
            );
        }
        else
        {
            /* release the pid channel to the pool */
            hXpt->PidChannelParserConfigOverride[MesgBufferNum] = false;
            Reg |= (
              BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_SHORT_PSI_EN, PcPtr->SaveShortPsiMsg == true ? 1 : 0 )  |
              BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_LONG_PSI_EN, PcPtr->SaveLongPsiMsg == true ? 1 : 0 ) |
              BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, PSF_CRC_DIS, PcPtr->PsfCrcDis == true ? 1 : 0 )  |
             BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, MSG_MOD_MODE, PcPtr->PsiMsgMod ) 
            );
        }
        BREG_Write32( hXpt->hRegister, RegAddr, Reg ); 
    }
	else
	{
        /* Wrong function call. Complain. */
	BDBG_ERR(( "Pid2BuffMapping is OFF. Please use BXPT_SetPidChannelPsiSettings instead of BXPT_Mesg_SetPidChannelPsiSettings. " ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}

    return( ExitCode );
}
#endif

BERR_Code BXPT_SetPidChannelPsiSettings(
        BXPT_Handle hXpt,                  /* [In] Handle for this transport */
        unsigned int PidChannelNum,        /* [In] The pid channel to configure. */
        bool OverrideParserSettings,       /* [In] If set true the PSI settings for 
                                                   this pid channel will be changed to Config
                                                   else restored to the paser band to which 
                                                   this pid channel is allocated */
        const BXPT_PidPsiConfig *Config    /* [In] Message mode setting for pid channel. */
        )                            
{

    uint32_t Reg, RegAddr, PlaybackFeSel, ChnlParserNum;
    ParserConfig *PcPtr;

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
	BXPT_P_PacketSubCfg PsubCfg;
#endif

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( Config );

	if (hXpt->Pid2BuffMappingOn == true )
	{
        /* Wrong function call. Complain. */
	BDBG_ERR(( "Pid2BuffMapping is ON. Please use BXPT_Mesg_SetPidChannelPsiSettings instead of BXPT_SetPidChannelPsiSettings. " ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
		return (ExitCode);
	}
    /* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )    
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_TABLE_STEP );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
		BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
		BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

        PlaybackFeSel = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL );
        ChnlParserNum = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT );

        if( PlaybackFeSel )
            PcPtr = &( hXpt->PbParserTable[ ChnlParserNum ] );
        else
            PcPtr = &( hXpt->IbParserTable[ ChnlParserNum ] );

        /* Yes, update the PSI config members in the PID channel. */                        
        RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CTRL1_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );

        Reg &= ~(
                 BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_SHORT_PSI_EN ) |
                 BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_LONG_PSI_EN ) |
                 BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, PSF_CRC_DIS ) |
                 BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i,  MSG_MOD_MODE )
        );

        if(OverrideParserSettings)
        {
            /* reserve this pid channel to have PSI setting different from the
               pid parser */
            hXpt->PidChannelParserConfigOverride[PidChannelNum] = true;
            Reg |= (
              BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_SHORT_PSI_EN, Config->SaveShortPsiMsg == true ? 1 : 0 )  |
              BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_LONG_PSI_EN, Config->SaveLongPsiMsg == true ? 1 : 0 ) |
              BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, PSF_CRC_DIS, Config->PsfCrcDis == true ? 1 : 0 )  |
              BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, MSG_MOD_MODE, Config->PsiMsgMod ) 
            );
        }
        else
        {
            /* release the pid channel to the pool */
            hXpt->PidChannelParserConfigOverride[PidChannelNum] = false;
            Reg |= (
              BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_SHORT_PSI_EN, PcPtr->SaveShortPsiMsg == true ? 1 : 0 )  |
              BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_LONG_PSI_EN, PcPtr->SaveLongPsiMsg == true ? 1 : 0 ) |
              BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, PSF_CRC_DIS, PcPtr->PsfCrcDis == true ? 1 : 0 )  |
             BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, MSG_MOD_MODE, PcPtr->PsiMsgMod ) 
            );
        }
        BREG_Write32( hXpt->hRegister, RegAddr, Reg ); 
     
    }
    return ( ExitCode ); 
}


#if BXPT_HAS_PID2BUF_MAPPING
BERR_Code BXPT_Mesg_GetPidChannelPsiSettings(
        BXPT_Handle hXpt,                  /* [In] Handle for this transport */
        unsigned int PidChannelNum,        /* [In] The pid channel to configure. */
        unsigned int MesgBufferNum,        /* [In] The pid channel to configure. */
        bool *OverrideParserSettings,       /* [Out]  */
        BXPT_PidPsiConfig *Config    /* [Out] Message mode setting for pid channel. */
        )                           
{

    uint32_t Reg, RegAddr;
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( Config );

    /* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )
    {
        /* Bad PID channel number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
	else if ( MesgBufferNum >= BXPT_P_MAX_MESG_BUFFERS ) {
        /* Bad Message Buffer number. Complain. */
        BDBG_ERR(( "Message Buffer Number %lu is out of range!", ( unsigned long ) MesgBufferNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
    else if ( hXpt->Pid2BuffMappingOn == true) 
    {
        *OverrideParserSettings = hXpt->PidChannelParserConfigOverride[MesgBufferNum];
        RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( MesgBufferNum * PID_CTRL1_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );

        Config->PsfCrcDis = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_PID_CTRL1_TABLE_i, PSF_CRC_DIS ) ? true : false;
        Config->SaveLongPsiMsg = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_PID_CTRL1_TABLE_i, SAVE_LONG_PSI_EN ) ? true : false; 
        Config->SaveShortPsiMsg = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_PID_CTRL1_TABLE_i, SAVE_SHORT_PSI_EN ) ? true : false; 
        Config->PsiMsgMod =  BCHP_GET_FIELD_DATA( Reg, XPT_MSG_PID_CTRL1_TABLE_i, MSG_MOD_MODE );
    }
	else
	{
        /* Wrong function call. Complain. */
	BDBG_ERR(( "Pid2BuffMapping is OFF. Please use BXPT_GetPidChannelPsiSettings instead of BXPT_Mesg_GetPidChannelPsiSettings. " ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}

    return( ExitCode );
}
#endif

BERR_Code BXPT_GetPidChannelPsiSettings(
        BXPT_Handle hXpt,                  /* [In] Handle for this transport */
        unsigned int PidChannelNum,        /* [In] The pid channel to configure. */
        bool *OverrideParserSettings,       /* [Out]  */
        BXPT_PidPsiConfig *Config    /* [Out] Message mode setting for pid channel. */
        )                           
{

    uint32_t Reg, RegAddr;
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( Config );

	if (hXpt->Pid2BuffMappingOn == true )
	{
        /* Wrong function call. Complain. */
	BDBG_ERR(( "Pid2BuffMapping is ON. Please use BXPT_Mesg_GetPidChannelPsiSettings instead of BXPT_GetPidChannelPsiSettings. " ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
		return (ExitCode);
	}

    /* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )    
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        *OverrideParserSettings = hXpt->PidChannelParserConfigOverride[PidChannelNum];
        RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CTRL1_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );

        Config->PsfCrcDis = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_PID_CTRL1_TABLE_i, PSF_CRC_DIS ) ? true : false;
        Config->SaveLongPsiMsg = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_PID_CTRL1_TABLE_i, SAVE_LONG_PSI_EN ) ? true : false; 
        Config->SaveShortPsiMsg = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_PID_CTRL1_TABLE_i, SAVE_SHORT_PSI_EN ) ? true : false; 
        Config->PsiMsgMod =  BCHP_GET_FIELD_DATA( Reg, XPT_MSG_PID_CTRL1_TABLE_i, MSG_MOD_MODE );
    }
    
    return ( ExitCode );
}


BERR_Code BXPT_P_ApplyParserPsiSettings(
    BXPT_Handle hXpt,                     
    unsigned int ParserNum,
    bool IsPbParser             
    )
{
    unsigned PidChannelNum;

    BERR_Code ExitCode = BERR_SUCCESS;

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
	BXPT_P_PacketSubCfg PsubCfg;
#endif

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BSTD_UNUSED( IsPbParser );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
	BXPT_PacketSub_P_SaveCfg( hXpt, 0, &PsubCfg );
#endif
                    
    for( PidChannelNum = 0; PidChannelNum < BXPT_P_MAX_MESG_BUFFERS/*(BXPT_P_MAX_PID_CHANNELS/2)*/; PidChannelNum++ )
    {
        uint32_t Reg, RegAddr, PlaybackFeSel, ChnlParserNum;
        ParserConfig *PcPtr;

        /* if this pid channel is reserved for per pid psi configuration 
           leave it alone */ 
        if(hXpt->PidChannelParserConfigOverride[PidChannelNum])
            continue; 

        RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_TABLE_STEP );
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        PlaybackFeSel = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL );
        ChnlParserNum = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT );

        /* Is this PID channel mapped to the parser code who called it? */
        /* The ParserNum passed in my have the MSB set, which is our internal indication that the band is a playback parser */
        if( BXPT_P_CLEAR_PB_FLAG( ParserNum ) == ChnlParserNum && hXpt->PidChannelTable[ PidChannelNum ].IsPidChannelConfigured )
        {
            if( PlaybackFeSel )
            {
                if( ParserNum >= BXPT_P_MAX_PLAYBACKS )
                {
                    BDBG_ERR(( "Playback ParserNum %u out of range", ParserNum ));
                    ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
                    goto Done;
                }
       
                PcPtr = &( hXpt->PbParserTable[ ParserNum ] );
            }
            else
            {
                if( ParserNum >= BXPT_P_MAX_PID_PARSERS )
                {
                    BDBG_ERR(( "Input ParserNum %u out of range", ParserNum ));
                    ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
                    goto Done;
                }
       
                PcPtr = &( hXpt->IbParserTable[ ParserNum ] );
            }

            /* Yes, update the PSI config members in the PID channel. */                        
            RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CTRL1_TABLE_STEP );
            Reg = BREG_Read32( hXpt->hRegister, RegAddr );

            Reg &= ~(
                BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_SHORT_PSI_EN ) |
                BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_LONG_PSI_EN ) |
                BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, PSF_CRC_DIS ) |
                BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i,  MSG_MOD_MODE )
            );

            Reg |= (
                BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_SHORT_PSI_EN, PcPtr->SaveShortPsiMsg == true ? 1 : 0 )  |
                BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, SAVE_LONG_PSI_EN, PcPtr->SaveLongPsiMsg == true ? 1 : 0 ) |
                BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, PSF_CRC_DIS, PcPtr->PsfCrcDis == true ? 1 : 0 )  |
                BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, MSG_MOD_MODE, PcPtr->PsiMsgMod ) 
            );
            BREG_Write32( hXpt->hRegister, RegAddr, Reg ); 
        }
    }

    Done:

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
	BXPT_PacketSub_P_RestoreCfg( hXpt, 0, &PsubCfg );
#endif

    return( ExitCode );
}

static BERR_Code ConfigMessageBufferRegs( 
    BXPT_Handle hXpt,                   /* [Input] Handle for this transport */
    unsigned int PidChannelNum,             /* [Input] Which PID channel buffer we want. */
    uint32_t CpuAddr,                   /* [Input] Caller allocated memory, or NULL. */
    BXPT_MessageBufferSize BufferSize   /* [Input] Size, in bytes, of the buffer. */
    )
{                   
    uint32_t Reg, RegAddr, Offset, OutputMode;

    BERR_Code ExitCode = BERR_SUCCESS;

    /** Reset the Valid Pointer. **/
    /* Set the PID channel message output mode to NO_OUTPUT ( == 0 ) first. */
    RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CTRL1_TABLE_STEP );
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    OutputMode = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE );
    Reg &= ~BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE );
    BREG_Write32( hXpt->hRegister, RegAddr, Reg );

    /* Write the PID channel number to the buffer reset register. Writes trigger the reset. */
    BREG_Write32( hXpt->hRegister, BCHP_XPT_MSG_DMA_BUFFER_RESET, PidChannelNum );

    /** Clear the read pointer. **/
    RegAddr = BCHP_XPT_MSG_DMA_RP_TABLE_i_ARRAY_BASE + ( PidChannelNum * RP_TABLE_STEP );
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    Reg &= ~BCHP_MASK( XPT_MSG_DMA_RP_TABLE_i, READ_POINTER ); 
    BREG_Write32( hXpt->hRegister, RegAddr, Reg );

    BMEM_ConvertAddressToOffset( hXpt->hMemory, ( void * ) CpuAddr, &Offset );

    /* 
    ** The buffer address passed in is still a byte-based address. 
    ** Convert this to an address based on blocks the size of
    ** BXPT_P_MESSAGE_BUFFER_BLOCK_SIZE .
    */
    Offset /= BXPT_P_MESSAGE_BUFFER_BLOCK_SIZE;

    /** Set the buffer address and size. **/
    RegAddr = BCHP_XPT_MSG_DMA_BP_TABLE_i_ARRAY_BASE + ( PidChannelNum * BP_TABLE_STEP );
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );

    Reg &= ~( 
        BCHP_MASK( XPT_MSG_DMA_BP_TABLE_i, BP_BUFFER_SIZE ) |
        BCHP_MASK( XPT_MSG_DMA_BP_TABLE_i, BP_BUFFER_BASE_ADDR )
        );

    Reg |= ( 
        BCHP_FIELD_DATA( XPT_MSG_DMA_BP_TABLE_i, BP_BUFFER_SIZE, ( uint32_t ) BufferSize ) |
        BCHP_FIELD_DATA( XPT_MSG_DMA_BP_TABLE_i, BP_BUFFER_BASE_ADDR, Offset )
        );

    BREG_Write32( hXpt->hRegister, RegAddr, Reg );

    /* Restore the old output mode */
    RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CTRL1_TABLE_STEP );
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    Reg |= BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE, OutputMode );
    BREG_Write32( hXpt->hRegister, RegAddr, Reg );
    
    return( ExitCode );
}

BERR_Code BXPT_GetMesgBufferDepth(
    BXPT_Handle hXpt,
    unsigned int PidChannelNum,
    size_t *BufferDepth
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( BufferDepth );

    /* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )    
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        *BufferDepth = 0;
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        uint32_t Read, Valid;
        size_t BufferSize;

        GetBufferPointers( hXpt, PidChannelNum, &Read, &Valid, BufferDepth, &BufferSize );
    }

    return ExitCode;
}

static void GetFilterAddrs(
    unsigned int GroupSel,          
    unsigned int FilterNum,     
    uint32_t *CoefAddr,
    uint32_t *MaskAddr,
    uint32_t *ExclAddr
    )
{
    FilterNum *= 4; /* Since each filter register is 4 bytes wide. */
    GroupSel *= 4;

    if( CoefAddr )
        *CoefAddr = BCHP_XPT_MSG_GEN_FILT_COEF_i_ARRAY_BASE + GroupSel + FilterNum;
    if( MaskAddr )
        *MaskAddr = BCHP_XPT_MSG_GEN_FILT_MASK_i_ARRAY_BASE + GroupSel + FilterNum;
    if( ExclAddr )
        *ExclAddr = BCHP_XPT_MSG_GEN_FILT_EXCL_i_ARRAY_BASE + GroupSel + FilterNum;
}

static BERR_Code ChangeFilterByte( 
    BXPT_Handle hXpt,       
    uint32_t FilterBaseAddr,    
    unsigned int FilterNum,     
    unsigned int ByteOffset,        
    uint8_t FilterByte      
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    /* Sanity check on the arguments. */
    if( FilterNum >= BXPT_P_FILTER_TABLE_SIZE )    
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "FilterNum %lu is out of range!", ( unsigned long ) FilterNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else if( ByteOffset >= BXPT_FILTER_SIZE )      
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "ByteOffset %lu is out of range!", ( unsigned long ) ByteOffset ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        uint32_t Reg, ShiftVal;

        /*
        ** The filter bytes are organized as 1 or more 4-byte registers. The registers
        ** themselves are NOT located sequentially in memory: Bytes 0-3 of the filter
        ** are in a register at address X, but bytes 4-7 are in a register at address
        ** X + 32. The rest of the register addresses increment in the same way.
        */
        FilterBaseAddr += ( ByteOffset / 4 ) * FILTER_WORD_STEP;
        ShiftVal = 3 - ( ByteOffset % 4 );      /* ShiftVal expressed as bytes */
        ShiftVal *= 8;                          /* ShiftVal expressed as bits */
        
        Reg = BREG_Read32( hXpt->hRegister, FilterBaseAddr );
        Reg &= ~( 0xFF << ShiftVal );
        Reg |= ( FilterByte << ShiftVal );
        BREG_Write32( hXpt->hRegister, FilterBaseAddr, Reg );
    }

    return( ExitCode );
}

BERR_Code BXPT_P_GetGroupSelect(
                               unsigned int Bank,
                               unsigned int *GenGrpSel
                               )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    if(BXPT_FILTER_SIZE == 32)
    {
        switch ( Bank )
        {
        case 0:
            *GenGrpSel = BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G8_B0;
            break;

#if ( BCHP_CHIP == 7118 ) || ( BCHP_CHIP == 3563 )
            /* These chips have only a subset of the full filter groups */
#else
        case 1:
            *GenGrpSel = BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G8_B8;
            break;
#endif

        default:
            BDBG_ERR(( "Bank %lu is out of range!", ( unsigned long ) Bank ));
        }
    }
    else
    {
        switch ( Bank)
        {
        case 0:
            *GenGrpSel = BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G4_B0;
            break;

        case 1:
            *GenGrpSel = BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G4_B4;
            break;

#if ( BCHP_CHIP == 7118 ) || ( BCHP_CHIP == 3563 )
            /* These chips have only a subset of the full filter groups */
#else
        case 2:
            *GenGrpSel = BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G4_B8;
            break;
        case 3:
            *GenGrpSel = BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G4_B12;
            break;
#endif

        default:
            BDBG_ERR(( "Bank %lu is out of range!", ( unsigned long ) Bank ));
        }
        
    }
    return ExitCode;
}

static unsigned GetPidChannelByFilter( 
    BXPT_Handle hXpt, 
    unsigned int Bank, 
    unsigned int FilterNum 
    )
{
    unsigned GenGrpSel = BCHP_XPT_MSG_PID_CTRL2_TABLE_i_GEN_GRP_SEL_G8_B0;
    unsigned Index = BXPT_P_MAX_PID_CHANNELS;

    if( BXPT_P_GetGroupSelect( Bank, &GenGrpSel ) != BERR_SUCCESS )
        goto Done;

    for( Index = 0; Index < BXPT_P_MAX_MESG_BUFFERS; Index++ )
    {
        uint32_t Reg, ShiftVal;

        /* Is this PID channel using the bank we're worried about? */
        Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_MSG_PID_CTRL2_TABLE_i_ARRAY_BASE + ( Index * PID_CTRL2_TABLE_STEP ) );
        if( BCHP_GET_FIELD_DATA( Reg, XPT_MSG_PID_CTRL2_TABLE_i, GEN_GRP_SEL ) != GenGrpSel )
            continue;       /* This channel isn't using the filter bank we're after. */

        ShiftVal = 1ul << FilterNum;
        Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE + ( Index * GEN_FILT_EN_STEP ) );
        if( Reg & ShiftVal )
            break;          /* Found it. */ 
    }

     if(Index == BXPT_P_MAX_MESG_BUFFERS)
	Index = BXPT_P_MAX_PID_CHANNELS;
     
    Done:
    return Index;
}

BERR_Code BXPT_P_DisableFilter( 
    BXPT_Handle hXpt,       
    unsigned int FilterNum,     
    unsigned int PidChannelNum
    )
{
    uint32_t Reg, RegAddr, OutputMode;

    BERR_Code ExitCode = BERR_SUCCESS;
    
    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    /* PR 16986 requires a workaround for modifying a PSI filter. */
#if 1   
    /* Old method. */
    BSTD_UNUSED( OutputMode );
    RegAddr = BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE + ( PidChannelNum * GEN_FILT_EN_STEP );
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    Reg &= ~( 1ul << FilterNum );
    BREG_Write32( hXpt->hRegister, RegAddr, Reg );
#else
    /* Workaround */
    BSTD_UNUSED( FilterNum );
    RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CTRL1_TABLE_STEP );
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    OutputMode = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE );

    hXpt->MessageBufferTable[ PidChannelNum ].OutputMode = OutputMode;
    BXPT_P_PauseFilters( hXpt, PidChannelNum, 0, true );
#endif
    return( ExitCode );
}


BERR_Code BXPT_GetMesgBufferErrorStatus(
    BXPT_Handle hXpt,                       /* [in]  Handle for this transport */
    BXPT_PidChannelRange PidChannelRange,   /* [in]  Range of pid channel numbers */
    unsigned int *ErrorStatus               /* [out] Bitwise error status,single bit 
                                                    is dedicated to each pid channel
                                                    indicated by the rage parameter above   */
    )
{
    uint32_t RegAddr;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    RegAddr = BCHP_XPT_MSG_PID_ERR_00_31 + (4 * PidChannelRange );
    *ErrorStatus = BREG_Read32( hXpt->hRegister, RegAddr ); 
    return BERR_SUCCESS; 
}



#if 0
bool GetPidChannelByPid( 
    BXPT_Handle hXpt,           /* [Input] Handle for this transport */
    unsigned int Pid,               /* [Input] PID to search for. */
    unsigned int Band,              /* [Input] The parser band to search in. */
    unsigned int *PidChannelNum     /* [Output] Where to put the channel num. */
    )
{
    unsigned int i;

    bool FoundChannel = false;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    /* Sanity check on the arguments. */
    if( Pid >= 0x2000 )
    {
        /* Bad PID. Complain. */
        BDBG_ERR(( "Pid %lu is out of range!", ( unsigned long ) Pid ));
    }
    else
    {
        for( i= 0; i < hXpt->MaxPidChannels; i++ )
        {
            PidChannelTableEntry *Entry = &hXpt->PidChannelTable[ i ];

            if( Entry->IsAllocated == true
            && Entry->Pid == Pid
            && Entry->Band == Band )
            {
                *PidChannelNum = i;
                FoundChannel = true;
                break;
            }
        }
    }

    return( FoundChannel );
}
#endif

static BERR_Code EnableFilter( 
    BXPT_Handle hXpt,       
    unsigned int FilterNum,     
    unsigned int PidChannelNum
    )
{
    uint32_t Reg, RegAddr;

    BERR_Code ExitCode = BERR_SUCCESS;
    
    /* PR 16986 requires a workaround for modifying a PSI filter. */
#if 1   
    /* Old method. */
    RegAddr = BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE + ( PidChannelNum * GEN_FILT_EN_STEP );
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    Reg |= ( 1ul << FilterNum );
    BREG_Write32( hXpt->hRegister, RegAddr, Reg );
#else
    /* Workaround */
    BSTD_UNUSED( Reg );
    BSTD_UNUSED( RegAddr );
    BSTD_UNUSED( FilterNum );
    BXPT_P_PauseFilters( hXpt, PidChannelNum, hXpt->MessageBufferTable[ PidChannelNum ].OutputMode, false );
#endif

    return( ExitCode );
}

static int AdjustChanAndAddr( 
    BXPT_Handle hXpt,           /* [Input] Handle for this transport */
    uint32_t PidChannelNum,
    uint32_t FlagBaseRegister,
    uint32_t *AdjustedPidChannel,
    uint32_t *AdjustedRegisterAddr
    )
{           
    BSTD_UNUSED( hXpt );

    *AdjustedPidChannel = PidChannelNum % XPT_REG_SIZE_BITS;
    *AdjustedRegisterAddr = FlagBaseRegister + (( PidChannelNum / XPT_REG_SIZE_BITS ) * XPT_REG_SIZE_BYTES );
    return 0;
}

static void GetBufferPointers( 
    BXPT_Handle hXpt,
    unsigned int PidChannelNum,
    uint32_t *ReadPtr,
    uint32_t *ValidPtr,
    size_t *MesgSize,
    size_t *BufferSizeInBytes
    )
{
    uint32_t Reg, SizeField, i;

    uint32_t BufferSize = 1024;
    /*bool DataReady = GetPidChannelFlag( hXpt, PidChannelNum, BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31 );*/
    
    /* Get the read and valid pointers, coverting to byte-addressing in the process. */
    Reg = BREG_Read32( hXpt->hRegister, GetRegArrayAddr( hXpt, PidChannelNum, BCHP_XPT_MSG_DMA_RP_TABLE_i_ARRAY_BASE ) );
    *ReadPtr = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_DMA_RP_TABLE_i, READ_POINTER );

    Reg = BREG_Read32( hXpt->hRegister, GetRegArrayAddr( hXpt, PidChannelNum, BCHP_XPT_MSG_DMA_VP_TABLE_i_ARRAY_BASE ) );
    *ValidPtr = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_DMA_VP_TABLE_i, VALID_POINTER );

    /* Compute the size of the buffer, in bytes. */
    Reg = BREG_Read32( hXpt->hRegister, GetRegArrayAddr( hXpt, PidChannelNum, BCHP_XPT_MSG_DMA_BP_TABLE_i_ARRAY_BASE) );
    SizeField = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_DMA_BP_TABLE_i, BP_BUFFER_SIZE );
    for( i = 0; i < SizeField; i++ )
        BufferSize *= 2;
    *BufferSizeInBytes = BufferSize;

    /* How much data is in this buffer? */
    if( *ReadPtr == *ValidPtr )
    {
        *MesgSize = 0;
    }
    else if( *ValidPtr < *ReadPtr )
    {
        /* It wrapped, but its not full. */
        *MesgSize = BufferSize - *ReadPtr + *ValidPtr;
    }
    else
    {
        /* No wrap and not full. */
        *MesgSize = *ValidPtr - *ReadPtr;
    }
}

static uint32_t GetRegArrayAddr( 
    BXPT_Handle hXpt,           /* [Input] Handle for this transport */
    uint32_t PidChannelNum,
    uint32_t BaseRegister
    )
{
    BSTD_UNUSED( hXpt );
    return BaseRegister + ( XPT_REG_SIZE_BYTES * PidChannelNum );
}

static int GetPidChannelFlag(
    BXPT_Handle hXpt,           /* [Input] Handle for this transport */
    int PidChannelNum,
    int FlagBaseRegister
    )
{
    uint32_t Reg;
    uint32_t BitShift = 0;
    uint32_t RegAddr = 0;

    AdjustChanAndAddr( hXpt, PidChannelNum, FlagBaseRegister, &BitShift, &RegAddr );

    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    Reg &= ( 0x01 << BitShift );
    Reg >>= BitShift;
    return Reg;
}

static void CopyDmaDataToUser(
    BXPT_Handle hXpt,
    unsigned int PidChannelNum,
    uint32_t ReadPtr,
    uint8_t *UserBufferAddr,
    size_t NumBytesToCopy,
    size_t DmaBufferSize
    )
{
    uint32_t Reg, DmaBaseOffset;
    void *DmaBaseAddr;
    size_t SpaceUntilEndOfBuffer, ByteCount;

    /* Just in case... */
    if( NumBytesToCopy > DmaBufferSize )
    {
        BDBG_ERR(( "CopyDmaDataToUser(): NumBytesToCopy out of range!\n" ));
        NumBytesToCopy = DmaBufferSize;
    }

    Reg = BREG_Read32( hXpt->hRegister, GetRegArrayAddr( hXpt, PidChannelNum, BCHP_XPT_MSG_DMA_BP_TABLE_i_ARRAY_BASE ) );
    DmaBaseOffset = BCHP_GET_FIELD_DATA( Reg, XPT_MSG_DMA_BP_TABLE_i, BP_BUFFER_BASE_ADDR );
    DmaBaseOffset *= BXPT_P_MESSAGE_BUFFER_BLOCK_SIZE;

    /* Number of bytes between ReadPtr and the end of the DMA buffer. */
    SpaceUntilEndOfBuffer = DmaBufferSize - ReadPtr;

    /* The number of bytes we are asked to copy may be less than the ByteCount above. */
    ByteCount = NumBytesToCopy < SpaceUntilEndOfBuffer ? NumBytesToCopy : SpaceUntilEndOfBuffer;

    BMEM_ConvertOffsetToAddress( hXpt->hMemory, ( DmaBaseOffset + ReadPtr ), ( void ** ) &DmaBaseAddr );
    BKNI_Memcpy( ( void * ) UserBufferAddr, ( void * ) DmaBaseAddr, ByteCount );

    NumBytesToCopy -= ByteCount;
    UserBufferAddr += ByteCount;
    
    /* If there's still bytes left to copy, the data must have wrapped around the end of the buffer. */
    if( NumBytesToCopy )
    {
        BMEM_ConvertOffsetToAddress( hXpt->hMemory, ( DmaBaseOffset ), ( void ** ) &DmaBaseAddr );
        BKNI_Memcpy( ( void * ) UserBufferAddr, ( void * ) DmaBaseAddr, NumBytesToCopy );
    }
}

BERR_Code BXPT_P_PauseFilters(
    BXPT_Handle hXpt,                       
    unsigned int PidChannelNum,     
    unsigned FilteringOp,     
    bool Pause                                
    )
{
    uint32_t Reg, RegAddr;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    /* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )    
    {
        /* Bad parser band number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        if( Pause == true )
        {
            /* Just turn off the filter. */
            RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CTRL1_TABLE_STEP );
            Reg = BREG_Read32( hXpt->hRegister, RegAddr );
            Reg &= ~(
                BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE ) 
            );
            Reg |= (
                BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE, BCHP_XPT_MSG_PID_CTRL1_TABLE_i_DATA_OUTPUT_MODE_NO_OUTPUT ) 
            );
            BREG_Write32( hXpt->hRegister, RegAddr, Reg );
        }
        else
        {
            /* Write the PID channel number to the buffer reset register. Writes trigger the reset. */
            BREG_Write32( hXpt->hRegister, BCHP_XPT_MSG_DMA_BUFFER_RESET, PidChannelNum );

            /** Clear the read pointer. **/
            RegAddr = BCHP_XPT_MSG_DMA_RP_TABLE_i_ARRAY_BASE + ( PidChannelNum * RP_TABLE_STEP );
            Reg = BREG_Read32( hXpt->hRegister, RegAddr );
            Reg &= ~BCHP_MASK( XPT_MSG_DMA_RP_TABLE_i, READ_POINTER ); 
            BREG_Write32( hXpt->hRegister, RegAddr, Reg );

            /* Turn the filter back on. */
            RegAddr = BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CTRL1_TABLE_STEP );
            Reg = BREG_Read32( hXpt->hRegister, RegAddr );
            Reg &= ~(
                BCHP_MASK( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE ) 
            );
            Reg |= (
                BCHP_FIELD_DATA( XPT_MSG_PID_CTRL1_TABLE_i, DATA_OUTPUT_MODE, FilteringOp ) 
            );
            BREG_Write32( hXpt->hRegister, RegAddr, Reg );
        }
    }

    return( ExitCode );
}


#if BXPT_HAS_PID2BUF_MAPPING
BERR_Code BXPT_Mesg_ClearPidChannelBuffer( 
    BXPT_Handle hXpt,                   /* [in] Handle for this transport */
    unsigned int PidChannelNum,             /* [in] Which PID channel buffer we want. */
    unsigned int MesgBufferNum             /* [in] Which Buffer number we want . */
	)
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    /* Sanity check on the arguments. */
    if( PidChannelNum >= hXpt->MaxPidChannels )
    {
        /* Bad PID channel number. Complain. */
        BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
	else if ( MesgBufferNum >= BXPT_P_MAX_MESG_BUFFERS ) {
        /* Bad Message Buffer number. Complain. */
        BDBG_ERR(( "Message Buffer Number %lu is out of range!", ( unsigned long ) MesgBufferNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		ConfigPid2BufferMap(hXpt, PidChannelNum, MesgBufferNum, false);
	}
	return ExitCode;
}
#endif

void BXPT_GetDefaultPsiSettings( 
    BXPT_PsiMessageSettings *Settings     /*  */
    )
{
    BKNI_Memset( (void *) Settings, 0, sizeof( BXPT_PsiMessageSettings ) );
}

void BXPT_GetDefaultPidChannelRecordSettings( 
    BXPT_PidChannelRecordSettings *Settings     /*  */
    )
{
    BKNI_Memset( (void *) Settings, 0, sizeof( BXPT_PsiMessageSettings ) );
}


