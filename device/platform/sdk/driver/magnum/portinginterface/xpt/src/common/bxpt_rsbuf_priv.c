/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_rsbuf_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 4/18/12 11:38a $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/base2/bxpt_rsbuf_priv.c $
 * 
 * Hydra_Software_Devel/9   4/18/12 11:38a gmullen
 * SW7552-257: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7552-257/1   4/17/12 9:32a gmullen
 * SW7552-257: Clear overflow status after reading
 * 
 * Hydra_Software_Devel/8   4/2/12 2:17p gmullen
 * SW7435-81: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7435-81/1   4/2/12 1:38p gmullen
 * SW7435-81: Shared DRAM for unused XC and RS buffers
 * 
 * Hydra_Software_Devel/7   3/8/12 3:41p gmullen
 * SW7425-2497: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2497/1   3/5/12 4:30p gmullen
 * SW7425-2497: Checks for errors in data pipeline
 * 
 * Hydra_Software_Devel/6   12/20/11 1:29p gmullen
 * SW7358-176: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7358-176/1   12/20/11 1:05p gmullen
 * SW7358-175: Added per-client control of XC and RS buffer alloc
 * 
 * Hydra_Software_Devel/5   9/13/11 4:17p gmullen
 * SW7425-690: Fixed memory leak in MPOD RS buffer code
 * 
 * Hydra_Software_Devel/4   8/22/11 5:45p gmullen
 * SW7231-319: Merged to Hydra branch
 * 
 * Hydra_Software_Devel/SW7231-319/1   8/16/11 2:15p gmullen
 * SW7231-319: Initial support for B0
 * 
 * Hydra_Software_Devel/3   3/8/11 9:58a gmullen
 * SW7425-156: Fixed incorrect blockout calculations
 * 
 * Hydra_Software_Devel/2   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 * Hydra_Software_Devel/18   7/27/10 5:11p gmullen
 * SW3548-3013: Added error checking to BMEM allocs
 * 
 * Hydra_Software_Devel/17   4/16/09 7:20p piyushg
 * PR53728: Each of the XC buffer clients (Remux, RAVE, and the Mesg
 * filters) would get a separate API to set
 * their input bitrates. That API would take a client handle and the ID of
 * the parser band. The XPT PI would
 * then walk backwards through the pipeline, increasing only the bitrates
 * for the XC and RS buffers that it
 * needs to.
 * 
 * Hydra_Software_Devel/16   8/15/08 1:13p gmullen
 * PR45729: Fixed initial value for RsBufBO[] and XcBufBO[]
 * 
 * Hydra_Software_Devel/15   6/17/08 5:55p gmullen
 * PR43710: Fixed parser all-pass mode.
 * 
 * Hydra_Software_Devel/14   5/7/08 10:47a gmullen
 * PR42443: Removed call to change band rates when changing parser mode.
 * 
 * Hydra_Software_Devel/13   8/13/07 10:01a gmullen
 * PR33983: Changed band rate calc to 100kb resolution.
 * 
 * Hydra_Software_Devel/12   3/2/07 11:13a gmullen
 * PR28354: Workaround for the memory hole.
 * 
 * Hydra_Software_Devel/11   11/14/06 10:57a gmullen
 * PR25771: All buffers are 256 bytes, unless user specifies larger.
 * 
 * Hydra_Software_Devel/10   11/2/06 4:31p gmullen
 * PR25402: Added support for secure heap to RS and XC buffer code.
 * 
 * Hydra_Software_Devel/9   8/29/06 8:37a gmullen
 * PR23896: Fixed similar leak in rs buf code.,
 * 
 * Hydra_Software_Devel/8   7/24/06 6:37p gmullen
 * PR22836: If PB0 support is requested, force PB1 support.
 * 
 * Hydra_Software_Devel/7   7/21/06 11:05a gmullen
 * PR22601: Fixed memory deallocation bug in BXPT_P_RsBuf_Shutdown
 * 
 * Hydra_Software_Devel/6   7/20/06 5:30p katrep
 * PR22365: Added better mangement of XC buffer client BW for playback.XC
 * buf for PB is given max BW & BW throtle is done at PB engine.Spare BW
 * enabllag is used to provide more BW for PB if available.
 * 
 * Hydra_Software_Devel/5   7/12/06 5:54p gmullen
 * PR21042: XC and RS buffers can be customized to reduce total memory
 * needs.
 * 
 * Hydra_Software_Devel/4   6/16/06 11:47a katrep
 * PR20631: Added support for parser bands 5 and 6,init the RS bufffer
 * according to number of parser band not not according to the number if
 * input bands
 * 
 * Hydra_Software_Devel/3   4/13/06 4:02p gmullen
 * PR20914: Ported over Prashant's fix from 7401A0
 * 
 * Hydra_Software_Devel/2   2/1/06 10:19a gmullen
 * PR18998: Fixed overflow issue in RAVE ITB/CDB, added support for PB
 * channels.
 * 
 * Hydra_Software_Devel/4   8/26/05 2:01p gmullen
 * PR15309: Added DirecTV support, RAVE video support, cleaned up RS and
 * XC buffer code.
 * 
 * Hydra_Software_Devel/3   7/26/05 5:48p gmullen
 * PR15309: Fixed buffer size issue in bxpt_rave.c and added buffer
 * shutdown code to other blocks.
 * 
 * Hydra_Software_Devel/2   7/19/05 1:27p gmullen
 * PR15309: Fixed buffer alloc bugs.
 * 
 * Hydra_Software_Devel/1   7/15/05 9:04a gmullen
 * PR15309: Inita i
 * 
 *
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt.h"
#include "bkni.h"
#include "bint.h"
#include "bxpt_rsbuf_priv.h"
#include "bchp_xpt_rsbuff.h"

#define INPUT_BAND_BUF_SIZE         (200* 1024)
#define PLAYBACK_BUF_SIZE           (8 * 1024)
#define MINIMUM_BUF_SIZE            (256)
#define BUFFER_PTR_REG_STEPSIZE     (BCHP_XPT_RSBUFF_BASE_POINTER_IBP1 - BCHP_XPT_RSBUFF_BASE_POINTER_IBP0)
#define MAX_BITRATE                 ( 108000000 )
#define BLOCKOUT_REG_STEPSIZE       (BCHP_XPT_RSBUFF_BO_IBP1 - BCHP_XPT_RSBUFF_BO_IBP0)
#define DEFAULT_PACKET_LEN          (188)

/* Threshold for pause generation when XC Buffer for a corresponding band is almost full */
#define DEFAULT_PACKET_PAUSE_LEVEL  ( 12 )

#if( BDBG_DEBUG_BUILD == 1 )
BDBG_MODULE( xpt_rsbuf_priv );
#endif

static void SetupBufferRegs(
    BXPT_Handle hXpt,               
    unsigned BaseRegAddr,       /* [in] Which client buffer we are dealing with */
    unsigned WhichInstance,
    size_t Size,
    uint32_t Offset          /* [in] Size in bytes. Must be a multiple of 256. */
    )
{
    BaseRegAddr = BaseRegAddr + WhichInstance * BUFFER_PTR_REG_STEPSIZE;

    BREG_Write32( hXpt->hRegister, BaseRegAddr, Offset );                   /* Set BASE */
    BREG_Write32( hXpt->hRegister, BaseRegAddr + 4, Offset + Size - 1 );    /* Set END */
    BREG_Write32( hXpt->hRegister, BaseRegAddr + 8, Offset - 1 );           /* Set WRITE */
    BREG_Write32( hXpt->hRegister, BaseRegAddr + 12, Offset - 1 );          /* Set VALID */
    BREG_Write32( hXpt->hRegister, BaseRegAddr + 16, Offset - 1 );          /* Set READ */
    BREG_Write32( hXpt->hRegister, BaseRegAddr + 20, 0 );                   /* Set WATERMARK */
}

static BERR_Code AllocateBuffer(
    BXPT_Handle hXpt,               
    unsigned BaseRegAddr,       /* [in] Which client buffer we are dealing with */
    unsigned WhichInstance,
    unsigned long Size          /* [in] Size in bytes. Must be a multiple of 256. */
    )
{
    uint32_t Offset;
    void *Buffer;

    BERR_Code ExitCode = BERR_SUCCESS;

    /* If there is a secure heap defined, use it. */
    BMEM_Handle hMem = hXpt->hRHeap ? hXpt->hRHeap : hXpt->hMemory;

    /* Size must be a multiple of 256. */
    Size = Size - ( Size % 256 );

    Buffer = BMEM_AllocAligned( hMem, Size, 8, 0 ); 
    if( !Buffer )
    {
        BDBG_ERR(( "XC buffer alloc failed!" ));
        ExitCode = BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
        goto Done;
    }
    BMEM_ConvertAddressToOffset( hMem, Buffer, &Offset );
    SetupBufferRegs( hXpt, BaseRegAddr, WhichInstance, Size, Offset );
    Done:
    return( ExitCode );
}

static BERR_Code DeleteBuffer(
    BXPT_Handle hXpt, 
    unsigned BaseRegAddr,       /* [in] Which client buffer we are dealing with */
    unsigned WhichInstance
    )
{
    uint32_t Offset;
    void *Buffer;

    BERR_Code ExitCode = BERR_SUCCESS;

    /* If there is a secure heap defined, use it. */
    BMEM_Handle hMem = hXpt->hRHeap ? hXpt->hRHeap : hXpt->hMemory;

    BaseRegAddr = BaseRegAddr + WhichInstance * BUFFER_PTR_REG_STEPSIZE;

    Offset = BREG_Read32( hXpt->hRegister, BaseRegAddr );
    BMEM_ConvertOffsetToAddress( hMem, Offset, ( void ** ) &Buffer );
    BMEM_Free( hMem, Buffer );
    
    return( ExitCode );
}

static BERR_Code SetBufferEnable( 
    BXPT_Handle hXpt, 
    unsigned EnableRegAddr,
    unsigned Index,
    bool EnableIt
    )
{
    uint32_t EnReg;

    BERR_Code ExitCode = BERR_SUCCESS;

    EnReg = BREG_Read32( hXpt->hRegister, EnableRegAddr );
    if( EnableIt )
    {
        EnReg |= ( 1ul << Index );
    }
    else
    {
        EnReg &= ~( 1ul << Index );
    }
    BREG_Write32( hXpt->hRegister, EnableRegAddr, EnReg );  
    
    return( ExitCode );
}

static bool IsBufferEnabled( 
    BXPT_Handle hXpt, 
    unsigned EnableRegAddr,
    unsigned Index
    )
{
    uint32_t EnReg = BREG_Read32( hXpt->hRegister, EnableRegAddr );
    if( EnReg & ( 1ul << Index ) )
        return true;
    return false;
}

static unsigned long ComputeBlockOut( 
	unsigned long PeakRate,			/* [in] Max data rate (in bps) the band will handle. */
    unsigned PacketLen              /* [in] Packet size ,130 for dss and 188 for mpeg */
    )
{
    if( PeakRate < BXPT_MIN_PARSER_RATE )
    {
        BDBG_WRN(( "Minimum buffer rate is %u bps. PeakRate will be clamped to this value", BXPT_MIN_PARSER_RATE ));
        PeakRate = BXPT_MIN_PARSER_RATE;
    }
    else if( PeakRate > BXPT_MAX_PARSER_RATE )
    {
        BDBG_WRN(( "Maximum buffer rate is %u bps. PeakRate will be clamped to this value", BXPT_MAX_PARSER_RATE ));
        PeakRate = BXPT_MAX_PARSER_RATE;
    }

    return (10800 * PacketLen * 8) / ( PeakRate / 10000 );
}

static BERR_Code SetBlockout(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned BufferTypeBlockoutAddr,
    unsigned WhichInstance,
    unsigned long NewB0        
    )
{
    uint32_t Reg, RegAddr;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );

    RegAddr = BufferTypeBlockoutAddr + WhichInstance * BLOCKOUT_REG_STEPSIZE;
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    Reg &= ~BCHP_MASK( XPT_RSBUFF_BO_IBP0, BO_COUNT );
    Reg |= BCHP_FIELD_DATA( XPT_RSBUFF_BO_IBP0, BO_COUNT, NewB0 );
    BREG_Write32( hXpt->hRegister, RegAddr, Reg );

    return( ExitCode );
}

/*
** These functions are called internally from BXPT_Open() and BXPT_Close(). 
** Users should NOT uses these functions directly.
*/

BERR_Code BXPT_P_RsBuf_Init(
	BXPT_Handle hXpt, 	   	   	/* [in] Handle for this transport */
    const BXPT_BandWidthConfig *BandwidthConfig
	)
{
    unsigned ii;

    BERR_Code ExitCode = BERR_SUCCESS;
    unsigned totalAllocated = 0;

    BDBG_ASSERT( hXpt );
    BDBG_ASSERT( BandwidthConfig );

    #if BXPT_HAS_IB_PID_PARSERS
    BREG_Write32( hXpt->hRegister, BCHP_XPT_RSBUFF_IBP_BUFFER_ENABLE, 0 );    /* Default to OFF */
    for( ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++ )
    {
        if( BandwidthConfig->MaxInputRate[ ii ] )
        {
            BDBG_MSG(( "Alloc RS for IB parser %u, %u bps", ii, BandwidthConfig->MaxInputRate[ ii ] ));
            AllocateBuffer( hXpt, BCHP_XPT_RSBUFF_BASE_POINTER_IBP0, ii, INPUT_BAND_BUF_SIZE );
            SetBlockout( hXpt, BCHP_XPT_RSBUFF_BO_IBP0, ii, 
               ComputeBlockOut( BandwidthConfig->MaxInputRate[ ii ], DEFAULT_PACKET_LEN ) );
            SetBufferEnable( hXpt, BCHP_XPT_RSBUFF_IBP_BUFFER_ENABLE, ii, true );
            totalAllocated += INPUT_BAND_BUF_SIZE;
        }
        else
        {
            if( !hXpt->SharedXcRsBufferOffset )
            {
                BXPT_P_AllocSharedXcRsBuffer( hXpt );
                totalAllocated += BXPT_P_MINIMUM_BUF_SIZE;
            }
            SetupBufferRegs( hXpt, BCHP_XPT_RSBUFF_BASE_POINTER_IBP0, ii, BXPT_P_MINIMUM_BUF_SIZE, hXpt->SharedXcRsBufferOffset );
        }
    }
    #endif

    #if BXPT_HAS_PLAYBACK_PARSERS
    BREG_Write32( hXpt->hRegister, BCHP_XPT_RSBUFF_PBP_BUFFER_ENABLE, 0 );    /* Default to OFF */
    for( ii = 0; ii < BXPT_NUM_PLAYBACKS; ii++ )
    {
        if( BandwidthConfig->MaxPlaybackRate[ ii ] )
        {
            BDBG_MSG(( "Alloc RS for PB parser %u, %u bps", ii, BandwidthConfig->MaxPlaybackRate[ ii ] ));
            AllocateBuffer( hXpt, BCHP_XPT_RSBUFF_BASE_POINTER_PBP0, ii, PLAYBACK_BUF_SIZE );
            SetBlockout( hXpt, BCHP_XPT_RSBUFF_BO_PBP0, ii, 
               ComputeBlockOut( 2 * BandwidthConfig->MaxPlaybackRate[ ii ], DEFAULT_PACKET_LEN ) );
            SetBufferEnable( hXpt, BCHP_XPT_RSBUFF_PBP_BUFFER_ENABLE, ii, true );
            totalAllocated += INPUT_BAND_BUF_SIZE;
        }
        else
        {
            if( !hXpt->SharedXcRsBufferOffset )
            {
                BXPT_P_AllocSharedXcRsBuffer( hXpt );
                totalAllocated += BXPT_P_MINIMUM_BUF_SIZE;
            }
            SetupBufferRegs( hXpt, BCHP_XPT_RSBUFF_BASE_POINTER_PBP0, ii, BXPT_P_MINIMUM_BUF_SIZE, hXpt->SharedXcRsBufferOffset );
        }
    }
    #endif

    #if BXPT_HAS_MPOD_RSBUF
    BREG_Write32( hXpt->hRegister, BCHP_XPT_RSBUFF_MPOD_IBP_BUFFER_ENABLE, 0 );    /* Default to OFF */
    for( ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++ )
    {
        if( BandwidthConfig->MaxInputRate[ ii ] )
        {
            BDBG_MSG(( "Alloc RS for IB MPOD parser %u, %u bps", ii, BandwidthConfig->MaxInputRate[ ii ] ));
            AllocateBuffer( hXpt, BCHP_XPT_RSBUFF_BASE_POINTER_MPOD_IBP0, ii, INPUT_BAND_BUF_SIZE );
            SetBlockout( hXpt, BCHP_XPT_RSBUFF_BO_MPOD_IBP0, ii, 
               ComputeBlockOut( BandwidthConfig->MaxInputRate[ ii ], DEFAULT_PACKET_LEN ) );
            SetBufferEnable( hXpt, BCHP_XPT_RSBUFF_MPOD_IBP_BUFFER_ENABLE, ii, true );
            totalAllocated += INPUT_BAND_BUF_SIZE;
        }
        else
        {
            if( !hXpt->SharedXcRsBufferOffset )
            {
                BXPT_P_AllocSharedXcRsBuffer( hXpt );
                totalAllocated += BXPT_P_MINIMUM_BUF_SIZE;
            }
            SetupBufferRegs( hXpt, BCHP_XPT_RSBUFF_BASE_POINTER_MPOD_IBP0, ii, BXPT_P_MINIMUM_BUF_SIZE, hXpt->SharedXcRsBufferOffset );
        }
    }
    #endif

    BDBG_MSG(( "RS totalAllocated: %u bytes", totalAllocated ));
    return ExitCode;
}

BERR_Code BXPT_P_RsBuf_Shutdown(
	BXPT_Handle hXpt 	   	   	/* [in] Handle for this transport */
	)
{
    unsigned ii;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );

    #if BXPT_HAS_IB_PID_PARSERS 
    for( ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++ )
    {
        if( IsBufferEnabled( hXpt, BCHP_XPT_RSBUFF_IBP_BUFFER_ENABLE, ii ) )
        { 
            ExitCode |= SetBufferEnable( hXpt, BCHP_XPT_RSBUFF_IBP_BUFFER_ENABLE, ii, false );
            ExitCode |= DeleteBuffer( hXpt, BCHP_XPT_RSBUFF_BASE_POINTER_IBP0, ii );
            if( ExitCode != BERR_SUCCESS )
            {
                BDBG_ERR(( "Disable/Delete of RS Buffer %d failed", ii ));
                goto Done;
            }
        }
    }
    #endif

    #if BXPT_HAS_PLAYBACK_PARSERS
    for( ii = 0; ii < BXPT_NUM_PLAYBACKS; ii++ )
    {
        if( IsBufferEnabled( hXpt, BCHP_XPT_RSBUFF_PBP_BUFFER_ENABLE, ii ) )
        { 
            ExitCode |= SetBufferEnable( hXpt, BCHP_XPT_RSBUFF_PBP_BUFFER_ENABLE, ii, false );
            ExitCode |= DeleteBuffer( hXpt, BCHP_XPT_RSBUFF_BASE_POINTER_PBP0, ii );
            if( ExitCode != BERR_SUCCESS )
            {
                BDBG_ERR(( "Disable/Delete of RS Buffer %d failed", ii ));
                goto Done;
            }
        }
    }
    #endif

    #if BXPT_HAS_MPOD_RSBUF
    for( ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++ )
    {
        if( IsBufferEnabled( hXpt, BCHP_XPT_RSBUFF_MPOD_IBP_BUFFER_ENABLE, ii ) )
        { 
            ExitCode |= SetBufferEnable( hXpt, BCHP_XPT_RSBUFF_MPOD_IBP_BUFFER_ENABLE, ii, false );
            ExitCode |= DeleteBuffer( hXpt, BCHP_XPT_RSBUFF_BASE_POINTER_MPOD_IBP0, ii );
            if( ExitCode != BERR_SUCCESS )
            {
                BDBG_ERR(( "Disable/Delete of RS MPOD Buffer %d failed", ii ));
                goto Done;
            }
        }
    }
    #endif

    Done:
        return ExitCode;
}

BERR_Code BXPT_P_RsBuf_ReportOverflows( 
    BXPT_Handle hXpt 
    )
{
    uint32_t Overflow;
    unsigned BufferNum;

    BERR_Code Status = 0;

#if BXPT_HAS_MPOD_RSBUF
    Overflow = BREG_Read32( hXpt->hRegister, BCHP_XPT_RSBUFF_MPOD_IBP_BUFFER_ENABLE ) & 
        BREG_Read32( hXpt->hRegister, BCHP_XPT_RSBUFF_MPOD_IBP_OVERFLOW_STATUS );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_RSBUFF_MPOD_IBP_OVERFLOW_STATUS, 0 );
    Status |= Overflow;
    for( BufferNum = 0; BufferNum < BXPT_NUM_PID_PARSERS; BufferNum++ )
    {
        if (Overflow & 1 << BufferNum)
            BDBG_ERR(( "RS MPOD buffer has overflowed. Consider increasing BXPT_BandWidthConfig.MaxInputRate[%u]", BufferNum ));
    }
#endif

    Overflow = BREG_Read32( hXpt->hRegister, BCHP_XPT_RSBUFF_IBP_BUFFER_ENABLE ) & 
        BREG_Read32( hXpt->hRegister, BCHP_XPT_RSBUFF_IBP_OVERFLOW_STATUS );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_RSBUFF_IBP_OVERFLOW_STATUS, 0 );
    Status |= Overflow;
    for( BufferNum = 0; BufferNum < BXPT_NUM_PID_PARSERS; BufferNum++ )
    {
        if (Overflow & 1 << BufferNum)
            BDBG_ERR(( "RS buffer has overflowed. Consider increasing BXPT_BandWidthConfig.MaxInputRate[%u]", BufferNum ));
    }

    Overflow = BREG_Read32( hXpt->hRegister, BCHP_XPT_RSBUFF_PBP_BUFFER_ENABLE ) & 
        BREG_Read32( hXpt->hRegister, BCHP_XPT_RSBUFF_PBP_OVERFLOW_STATUS );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_RSBUFF_PBP_OVERFLOW_STATUS, 0 );
    Status |= Overflow;
    for( BufferNum = 0; BufferNum < BXPT_NUM_PLAYBACKS; BufferNum++ )
    {
        if (Overflow & 1 << BufferNum)
            BDBG_ERR(( "RS buffer has overflowed. Consider increasing BXPT_BandWidthConfig.MaxPlaybackRate[%u]", BufferNum ));
    }

    return Status;
}

