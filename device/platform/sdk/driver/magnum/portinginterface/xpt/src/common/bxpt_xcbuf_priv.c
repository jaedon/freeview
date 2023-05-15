/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_xcbuf_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 4/18/12 11:38a $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/base2/bxpt_xcbuf_priv.c $
 * 
 * Hydra_Software_Devel/13   4/18/12 11:38a gmullen
 * SW7552-257: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7552-257/1   4/17/12 9:32a gmullen
 * SW7552-257: Clear overflow status after reading
 * 
 * Hydra_Software_Devel/12   4/2/12 2:17p gmullen
 * SW7435-81: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7435-81/1   4/2/12 1:38p gmullen
 * SW7435-81: Shared DRAM for unused XC and RS buffers
 * 
 * Hydra_Software_Devel/11   3/8/12 3:41p gmullen
 * SW7425-2497: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2497/1   3/5/12 4:30p gmullen
 * SW7425-2497: Checks for errors in data pipeline
 * 
 * Hydra_Software_Devel/10   2/28/12 10:49a gmullen
 * SW7425-2403: Back were we started from...
 * 
 * Hydra_Software_Devel/9   2/16/12 9:55a gmullen
 * SW7425-2403: Corrected blockout count and change warning mesg
 * 
 * Hydra_Software_Devel/8   12/20/11 1:29p gmullen
 * SW7358-176: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7358-176/1   12/20/11 1:05p gmullen
 * SW7358-175: Added per-client control of XC and RS buffer alloc
 * 
 * Hydra_Software_Devel/7   4/13/11 8:34a gmullen
 * SW7344-33: Merged fix to mainline
 * 
 * Hydra_Software_Devel/SW7344-33/1   3/8/11 1:36p gmullen
 * SW7425-153: Incorrect param passed to ComputeBlockout()
 * 
 * Hydra_Software_Devel/6   4/11/11 9:15a gmullen
 * SW7346-119: Merged fix to mainline
 * 
 * Hydra_Software_Devel/SW7346-119/1   4/7/11 3:43p gmullen
 * SW7346-119: Disable CC checking when enabling all-pass mode. Restore
 * when exiting all-pass
 * 
 * Hydra_Software_Devel/5   3/23/11 5:12p gmullen
 * SW7346-124: Do not allocate XC buffers for unused remux blocks
 * 
 * Hydra_Software_Devel/4   3/8/11 9:58a gmullen
 * SW7425-156: Fixed incorrect blockout calculations
 * 
 * Hydra_Software_Devel/3   12/6/10 10:01a gmullen
 * SW7422-117: Disable pausing from XC buffer to flush data on playback
 * stopping
 * 
 * Hydra_Software_Devel/2   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt.h"
#include "bkni.h"
#include "bxpt_xcbuf_priv.h"
#include "bchp_xpt_xcbuff.h"

#if( BDBG_DEBUG_BUILD == 1 )
BDBG_MODULE( xpt_xcbuf_priv );
#endif

#define INPUT_BAND_BUF_SIZE         (200* 1024)
#define PLAYBACK_BUF_SIZE           (8 * 1024)
#define BUFFER_PTR_REG_STEPSIZE     (6 * 4)
#define MAX_BITRATE                 ( 108000000 )
#define BLOCKOUT_REG_STEPSIZE       4
#define DEFAULT_PACKET_SIZE         (188)

/* Threshold for pause generation when XC Buffer for a corresponding band is almost full */
#define DEFAULT_PACKET_PAUSE_LEVEL  ( 12 )

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
    unsigned PacketLen,             /* [in] Packet size ,130 for dss and 188 for mpeg */
    char *BufferName,
    unsigned BufferIndex
    )
{
    /* Need to double the peak rate in order to support simultaneous R and G pipe traffic. */
    PeakRate *= 2;

    /* Error reporting is done in the RS buffer setup only. The RS rates really determine the
    bandwidth needs. This code just needs to make sure we don't overflow the bitfield. */
    if( PeakRate < BXPT_MIN_PARSER_RATE )
    {
#if 0
        BDBG_WRN(( "Requested rate for %s [%u] is %u bps, but supported minimum is %u bps. Minimum rate will be used.", 
            BufferName, BufferIndex, PeakRate, BXPT_MIN_PARSER_RATE ));
#else
    BSTD_UNUSED( BufferName );
    BSTD_UNUSED( BufferIndex );
#endif

        PeakRate = BXPT_MIN_PARSER_RATE;
    }
    else if( PeakRate > BXPT_MAX_PARSER_RATE )
    {
#if 0
        BDBG_WRN(( "Requested rate for %s [%u] is %u bps, but supported maximum is %u bps. Maximum rate will be used.", 
            BufferName, BufferIndex, PeakRate, BXPT_MAX_PARSER_RATE ));
#else
    BSTD_UNUSED( BufferName );
    BSTD_UNUSED( BufferIndex );
#endif
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
    Reg &= ~BCHP_MASK( XPT_XCBUFF_BO_RAVE_IBP0, BO_COUNT );
    Reg |= BCHP_FIELD_DATA( XPT_XCBUFF_BO_RAVE_IBP0, BO_COUNT, NewB0 );
    BREG_Write32( hXpt->hRegister, RegAddr, Reg );

    return( ExitCode );
}

BERR_Code BXPT_P_XcBuf_Init(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    const BXPT_BandWidthConfig *BandwidthConfig
    )
{
    uint32_t Reg;
    unsigned ii;

    BERR_Code ExitCode = BERR_SUCCESS;
    unsigned totalAllocated = 0;

    BDBG_ASSERT( hXpt );

    /* Set Pause to 12 packets */
    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_PAUSE_THRESHOLD );
    Reg &= ~( BCHP_MASK( XPT_XCBUFF_PAUSE_THRESHOLD, PACKETS ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XCBUFF_PAUSE_THRESHOLD, PACKETS, DEFAULT_PACKET_PAUSE_LEVEL ) );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_PAUSE_THRESHOLD, Reg );
   
    /* 
    ** XC buffer bitrates should be set to 2 times the expected input rate, to handle watch and 
    ** record. That usage requires both the R-pipe and G-pipe to be enabled.  
    */
    #if BXPT_HAS_IB_PID_PARSERS && BXPT_HAS_RAVE
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_RAVE_CTRL_BUFFER_EN_IBP, 0 );    /* Default to OFF */
    for( ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++ )
    {
        if( BandwidthConfig->MaxInputRate[ ii ] && BandwidthConfig->IbParserClients[ ii ].ToRave )
        {
            BDBG_MSG(( "Alloc XC for IB parser %u to RAVE, %u bps", ii, BandwidthConfig->MaxInputRate[ ii ] ));
            AllocateBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RAVE_IBP0, ii, INPUT_BAND_BUF_SIZE );
            SetBlockout( hXpt, BCHP_XPT_XCBUFF_BO_RAVE_IBP0, ii, 
               ComputeBlockOut( BandwidthConfig->MaxInputRate[ ii ], DEFAULT_PACKET_SIZE, "MaxInputRate", ii ) );

            SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_RAVE_CTRL_BUFFER_EN_IBP, ii, true );
            totalAllocated += INPUT_BAND_BUF_SIZE;
        }
        else
        {
            if( !hXpt->SharedXcRsBufferOffset )
            {
                BXPT_P_AllocSharedXcRsBuffer( hXpt );
                totalAllocated += BXPT_P_MINIMUM_BUF_SIZE;
            }
            SetupBufferRegs( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RAVE_IBP0, ii, BXPT_P_MINIMUM_BUF_SIZE, hXpt->SharedXcRsBufferOffset );
        }
    }
    #endif

    #if BXPT_HAS_PLAYBACK_PARSERS && BXPT_HAS_RAVE
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_RAVE_CTRL_BUFFER_EN_PBP, 0 );    /* Default to OFF */
    for( ii = 0; ii < BXPT_NUM_PLAYBACKS; ii++ )
    {
        if( BandwidthConfig->MaxPlaybackRate[ ii ] && BandwidthConfig->PlaybackParserClients[ ii ].ToRave )
        {
            BDBG_MSG(( "Alloc XC for PB parser %u to RAVE, %u bps", ii, BandwidthConfig->MaxPlaybackRate[ ii ] ));
            AllocateBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RAVE_PBP0, ii, PLAYBACK_BUF_SIZE );
            SetBlockout( hXpt, BCHP_XPT_XCBUFF_BO_RAVE_PBP0, ii, 
               ComputeBlockOut( BandwidthConfig->MaxPlaybackRate[ ii ], DEFAULT_PACKET_SIZE, "MaxPlaybackRate", ii ) );
            SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_RAVE_CTRL_BUFFER_EN_PBP, ii, true );
            totalAllocated += PLAYBACK_BUF_SIZE;
        }
        else
        {
            if( !hXpt->SharedXcRsBufferOffset )
            {
                BXPT_P_AllocSharedXcRsBuffer( hXpt );
                totalAllocated += BXPT_P_MINIMUM_BUF_SIZE;
            }
            SetupBufferRegs( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RAVE_PBP0, ii, BXPT_P_MINIMUM_BUF_SIZE, hXpt->SharedXcRsBufferOffset );
        }
    }
    #endif

    #if BXPT_HAS_IB_PID_PARSERS && BXPT_HAS_MESG_BUFFERS
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_MSG_CTRL_BUFFER_EN_IBP, 0 );    /* Default to OFF */
    for( ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++ )
    {
        if( BandwidthConfig->MaxInputRate[ ii ] && BandwidthConfig->IbParserClients[ ii ].ToMsg )
        {
            BDBG_MSG(( "Alloc XC for IB parser %u to MSG, %u bps", ii, BandwidthConfig->MaxInputRate[ ii ] ));
            AllocateBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_MSG_IBP0, ii, INPUT_BAND_BUF_SIZE );
            SetBlockout( hXpt, BCHP_XPT_XCBUFF_BO_MSG_IBP0, ii, 
               ComputeBlockOut( BandwidthConfig->MaxInputRate[ ii ], DEFAULT_PACKET_SIZE, "MaxInputRate", ii ) );
            SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_MSG_CTRL_BUFFER_EN_IBP, ii, true );
            totalAllocated += INPUT_BAND_BUF_SIZE;
        }
        else
        {
            if( !hXpt->SharedXcRsBufferOffset )
            {
                BXPT_P_AllocSharedXcRsBuffer( hXpt );
                totalAllocated += BXPT_P_MINIMUM_BUF_SIZE;
            }
            SetupBufferRegs( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_MSG_IBP0, ii, BXPT_P_MINIMUM_BUF_SIZE, hXpt->SharedXcRsBufferOffset );
        }
    }
    #endif

    #if BXPT_HAS_PLAYBACK_PARSERS && BXPT_HAS_MESG_BUFFERS
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_MSG_CTRL_BUFFER_EN_PBP, 0 );    /* Default to OFF */
    for( ii = 0; ii < BXPT_NUM_PLAYBACKS; ii++ )
    {
        if( BandwidthConfig->MaxPlaybackRate[ ii ] && BandwidthConfig->PlaybackParserClients[ ii ].ToMsg )
        {
            BDBG_MSG(( "Alloc XC for PB parser %u to MSG, %u bps", ii, BandwidthConfig->MaxPlaybackRate[ ii ] ));
            AllocateBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_MSG_PBP0, ii, PLAYBACK_BUF_SIZE );
            SetBlockout( hXpt, BCHP_XPT_XCBUFF_BO_MSG_PBP0, ii, 
               ComputeBlockOut( BandwidthConfig->MaxPlaybackRate[ ii ], DEFAULT_PACKET_SIZE, "MaxPlaybackRate", ii ) );
            SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_MSG_CTRL_BUFFER_EN_PBP, ii, true );
            totalAllocated += PLAYBACK_BUF_SIZE;
        }
        else
        {
            if( !hXpt->SharedXcRsBufferOffset )
            {
                BXPT_P_AllocSharedXcRsBuffer( hXpt );
                totalAllocated += BXPT_P_MINIMUM_BUF_SIZE;
            }
            SetupBufferRegs( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_MSG_PBP0, ii, BXPT_P_MINIMUM_BUF_SIZE, hXpt->SharedXcRsBufferOffset );
        }
    }
    #endif

    #if BXPT_HAS_IB_PID_PARSERS && BXPT_HAS_REMUX
    /* We have at least RMX0 */
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX0_CTRL_BUFFER_EN_IBP, 0 );    /* Default to OFF */
    for( ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++ )
    {
        if( BandwidthConfig->MaxInputRate[ ii ] && BandwidthConfig->RemuxUsed[ 0 ] && BandwidthConfig->IbParserClients[ ii ].ToRmx[ 0 ] )
        {
            BDBG_MSG(( "Alloc XC for IB parser %u to RMX0, %u bps", ii, BandwidthConfig->MaxInputRate[ ii ] ));
            AllocateBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RMX0_IBP0, ii, INPUT_BAND_BUF_SIZE );
            SetBlockout( hXpt, BCHP_XPT_XCBUFF_BO_RMX0_IBP0, ii, 
               ComputeBlockOut( BandwidthConfig->MaxInputRate[ ii ], DEFAULT_PACKET_SIZE, "MaxInputRate", ii ) );
            SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_RMX0_CTRL_BUFFER_EN_IBP, ii, true );
            totalAllocated += INPUT_BAND_BUF_SIZE;
        }
        else
        {
            if( !hXpt->SharedXcRsBufferOffset )
            {
                BXPT_P_AllocSharedXcRsBuffer( hXpt );
                totalAllocated += BXPT_P_MINIMUM_BUF_SIZE;
            }
            SetupBufferRegs( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RMX0_IBP0, ii, BXPT_P_MINIMUM_BUF_SIZE, hXpt->SharedXcRsBufferOffset );
        }
    }
    
    #if BXPT_NUM_REMULTIPLEXORS > 0
    /* We've got at least RMX1 */
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX1_CTRL_BUFFER_EN_IBP, 0 );    /* Default to OFF */
    for( ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++ )
    {
        if( BandwidthConfig->MaxInputRate[ ii ] && BandwidthConfig->RemuxUsed[ 1 ] && BandwidthConfig->IbParserClients[ ii ].ToRmx[ 1 ])
        {
            BDBG_MSG(( "Alloc XC for IB parser %u to RMX1, %u bps", ii, BandwidthConfig->MaxInputRate[ ii ] ));
            AllocateBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RMX1_IBP0, ii, INPUT_BAND_BUF_SIZE );
            SetBlockout( hXpt, BCHP_XPT_XCBUFF_BO_RMX1_IBP0, ii, 
               ComputeBlockOut( BandwidthConfig->MaxInputRate[ ii ], DEFAULT_PACKET_SIZE, "MaxInputRate", ii ) );
            SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_RMX1_CTRL_BUFFER_EN_IBP, ii, true );
            totalAllocated += INPUT_BAND_BUF_SIZE;
        }
        else
        {
            if( !hXpt->SharedXcRsBufferOffset )
            {
                BXPT_P_AllocSharedXcRsBuffer( hXpt );
                totalAllocated += BXPT_P_MINIMUM_BUF_SIZE;
            }
            SetupBufferRegs( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RMX1_IBP0, ii, BXPT_P_MINIMUM_BUF_SIZE, hXpt->SharedXcRsBufferOffset );
        }
    }
    #endif

    #if BXPT_NUM_REMULTIPLEXORS > 2
    #error "Add support for remux2 and higher, input bands"
    #endif

    #endif

    #if BXPT_HAS_PLAYBACK_PARSERS && BXPT_HAS_REMUX
    /* We have at least RMX0 */
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX0_CTRL_BUFFER_EN_PBP, 0 );    /* Default to OFF */
    for( ii = 0; ii < BXPT_NUM_PLAYBACKS; ii++ )
    {
        if( BandwidthConfig->MaxPlaybackRate[ ii ] && BandwidthConfig->RemuxUsed[ 0 ] && BandwidthConfig->PlaybackParserClients[ ii ].ToRmx[ 0 ] )
        {
            BDBG_MSG(( "Alloc XC for PB parser %u to RMX0, %u bps", ii, BandwidthConfig->MaxPlaybackRate[ ii ] ));
            AllocateBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RMX0_PBP0, ii, PLAYBACK_BUF_SIZE );
            SetBlockout( hXpt, BCHP_XPT_XCBUFF_BO_RMX0_PBP0, ii, 
               ComputeBlockOut( BandwidthConfig->MaxPlaybackRate[ ii ], DEFAULT_PACKET_SIZE, "MaxPlaybackRate", ii ) );
            SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_RMX0_CTRL_BUFFER_EN_PBP, ii, true );
            totalAllocated += PLAYBACK_BUF_SIZE;
        }
        else
        {
            if( !hXpt->SharedXcRsBufferOffset )
            {
                BXPT_P_AllocSharedXcRsBuffer( hXpt );
                totalAllocated += BXPT_P_MINIMUM_BUF_SIZE;
            }
            SetupBufferRegs( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RMX0_PBP0, ii, BXPT_P_MINIMUM_BUF_SIZE, hXpt->SharedXcRsBufferOffset );
        }
    }

    #if BXPT_NUM_REMULTIPLEXORS > 0
    /* We've got at least RMX1 */
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX1_CTRL_BUFFER_EN_PBP, 0 );    /* Default to OFF */
    for( ii = 0; ii < BXPT_NUM_PLAYBACKS; ii++ )
    {
        if( BandwidthConfig->MaxPlaybackRate[ ii ] && BandwidthConfig->RemuxUsed[ 1 ] && BandwidthConfig->PlaybackParserClients[ ii ].ToRmx[ 1 ])
        {
            BDBG_MSG(( "Alloc XC for PB parser %u to RMX1, %u bps", ii, BandwidthConfig->MaxPlaybackRate[ ii ] ));
            AllocateBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RMX1_PBP0, ii, PLAYBACK_BUF_SIZE );
            SetBlockout( hXpt, BCHP_XPT_XCBUFF_BO_RMX1_PBP0, ii, 
               ComputeBlockOut( BandwidthConfig->MaxPlaybackRate[ ii ], DEFAULT_PACKET_SIZE, "MaxPlaybackRate", ii ) );
            SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_RMX1_CTRL_BUFFER_EN_PBP, ii, true );
            totalAllocated += PLAYBACK_BUF_SIZE;
        }
        else
        {
            if( !hXpt->SharedXcRsBufferOffset )
            {
                BXPT_P_AllocSharedXcRsBuffer( hXpt );
                totalAllocated += BXPT_P_MINIMUM_BUF_SIZE;
            }
            SetupBufferRegs( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RMX1_PBP0, ii, BXPT_P_MINIMUM_BUF_SIZE, hXpt->SharedXcRsBufferOffset );
        }
    }
    #endif

    #if BXPT_NUM_REMULTIPLEXORS > 2
    #error "Add support for remux2 and higher, playback channels"
    #endif

    #endif

    BDBG_MSG(( "XC totalAllocated: %u bytes", totalAllocated ));
    return( ExitCode );
}

BERR_Code BXPT_P_XcBuf_Shutdown(
    BXPT_Handle hXpt            /* [in] Handle for this transport */
    )
{
    unsigned ii;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );

    #if BXPT_HAS_IB_PID_PARSERS && BXPT_HAS_RAVE
    for( ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++ )
    {
        if( IsBufferEnabled( hXpt, BCHP_XPT_XCBUFF_RAVE_CTRL_BUFFER_EN_IBP, ii ) )
        { 
            ExitCode |= SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_RAVE_CTRL_BUFFER_EN_IBP, ii, false );
            ExitCode |= DeleteBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RAVE_IBP0, ii );
            if( ExitCode != BERR_SUCCESS )
            {
                BDBG_ERR(( "Disable/Delete of XC RAVE Buffer %d failed", ii ));
                goto Done;
            }
        }
    }
    #endif

    #if BXPT_HAS_PLAYBACK_PARSERS && BXPT_HAS_RAVE
    for( ii = 0; ii < BXPT_NUM_PLAYBACKS; ii++ )
    {
        if( IsBufferEnabled( hXpt, BCHP_XPT_XCBUFF_RAVE_CTRL_BUFFER_EN_PBP, ii ) )
        { 
            ExitCode |= SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_RAVE_CTRL_BUFFER_EN_PBP, ii, false );
            ExitCode |= DeleteBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RAVE_PBP0, ii );
            if( ExitCode != BERR_SUCCESS )
            {
                BDBG_ERR(( "Disable/Delete of XC RAVE Buffer %d failed", ii ));
                goto Done;
            }
        }
    }
    #endif

    #if BXPT_HAS_IB_PID_PARSERS && BXPT_HAS_MESG_BUFFERS
    for( ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++ )
    {
        if( IsBufferEnabled( hXpt, BCHP_XPT_XCBUFF_MSG_CTRL_BUFFER_EN_IBP, ii ) )
        { 
            ExitCode |= SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_MSG_CTRL_BUFFER_EN_IBP, ii, false );
            ExitCode |= DeleteBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_MSG_IBP0, ii );
            if( ExitCode != BERR_SUCCESS )
            {
                BDBG_ERR(( "Disable/Delete of XC Msg Input Buffer %d failed", ii ));
                goto Done;
            }
        }
    }
    #endif

    #if BXPT_HAS_PLAYBACK_PARSERS && BXPT_HAS_MESG_BUFFERS
    for( ii = 0; ii < BXPT_NUM_PLAYBACKS; ii++ )
    {
        if( IsBufferEnabled( hXpt, BCHP_XPT_XCBUFF_MSG_CTRL_BUFFER_EN_PBP, ii ) )
        { 
            ExitCode |= SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_MSG_CTRL_BUFFER_EN_PBP, ii, false );
            ExitCode |= DeleteBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_MSG_PBP0, ii );
            if( ExitCode != BERR_SUCCESS )
            {
                BDBG_ERR(( "Disable/Delete of XC Msg Playback Buffer %d failed", ii ));
                goto Done;
            }
        }
    }
    #endif

    #if BXPT_HAS_IB_PID_PARSERS && BXPT_HAS_REMUX
    /* We have at least RMX0 */
    for( ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++ )
    {
        if( IsBufferEnabled( hXpt, BCHP_XPT_XCBUFF_RMX0_CTRL_BUFFER_EN_IBP, ii ) )
        { 
            ExitCode |= SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_RMX0_CTRL_BUFFER_EN_IBP, ii, false );
            ExitCode |= DeleteBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RMX0_IBP0, ii );
            if( ExitCode != BERR_SUCCESS )
            {
                BDBG_ERR(( "Disable/Delete of XC RMX0 Input Buffer %d failed", ii ));
                goto Done;
            }
        }
    }
    
    #if BXPT_NUM_REMULTIPLEXORS > 0
    /* We've got at least RMX1 */
    for( ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++ )
    {
        if( IsBufferEnabled( hXpt, BCHP_XPT_XCBUFF_RMX1_CTRL_BUFFER_EN_IBP, ii ) )
        { 
            ExitCode |= SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_RMX1_CTRL_BUFFER_EN_IBP, ii, false );
            ExitCode |= DeleteBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RMX1_IBP0, ii );
            if( ExitCode != BERR_SUCCESS )
            {
                BDBG_ERR(( "Disable/Delete of XC RMX1 Input Buffer %d failed", ii ));
                goto Done;
            }
        }
    }
    #endif

    #if BXPT_NUM_REMULTIPLEXORS > 2
    #error "Add support for remux2 and higher, input bands"
    #endif

    #endif

    #if BXPT_HAS_PLAYBACK_PARSERS && BXPT_HAS_REMUX
    /* We have at least RMX0 */
    for( ii = 0; ii < BXPT_NUM_PLAYBACKS; ii++ )
    {
        if( IsBufferEnabled( hXpt, BCHP_XPT_XCBUFF_RMX0_CTRL_BUFFER_EN_PBP, ii ) )
        { 
            ExitCode |= SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_RMX0_CTRL_BUFFER_EN_PBP, ii, false );
            ExitCode |= DeleteBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RMX0_PBP0, ii );
            if( ExitCode != BERR_SUCCESS )
            {
                BDBG_ERR(( "Disable/Delete of XC RMX0 Playback Buffer %d failed", ii ));
                goto Done;
            }
        }
    }

    #if BXPT_NUM_REMULTIPLEXORS > 0
    /* We've got at least RMX1 */
    for( ii = 0; ii < BXPT_NUM_PLAYBACKS; ii++ )
    {
        if( IsBufferEnabled( hXpt, BCHP_XPT_XCBUFF_RMX1_CTRL_BUFFER_EN_PBP, ii ) )
        { 
            ExitCode |= SetBufferEnable( hXpt, BCHP_XPT_XCBUFF_RMX1_CTRL_BUFFER_EN_PBP, ii, false );
            ExitCode |= DeleteBuffer( hXpt, BCHP_XPT_XCBUFF_BASE_POINTER_RMX1_PBP0, ii );
            if( ExitCode != BERR_SUCCESS )
            {
                BDBG_ERR(( "Disable/Delete of XC RMX1 Playback Buffer %d failed", ii ));
                goto Done;
            }
        }
    }
    #endif

    #if BXPT_NUM_REMULTIPLEXORS > 2
    #error "Add support for remux2 and higher, playback channels"
    #endif

    #endif

    Done:
    return( ExitCode );
}

void BXPT_XcBuf_P_EnablePlaybackPausing( 
    BXPT_Handle hXpt, 
    unsigned PbChannelNum,
    bool PauseEn
    )
{
    uint32_t Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_RAVE_CTRL_PAUSE_EN_PBP );

    if( PauseEn) 
    {
        BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_RAVE_CTRL_PAUSE_EN_PBP, Reg | ( 1 << PbChannelNum ) );
    }
    else
    {
        BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_RAVE_CTRL_PAUSE_EN_PBP, Reg & ~( 1 << PbChannelNum ) );
    }
}

BERR_Code BXPT_P_XcBuf_ReportOverflows( 
    BXPT_Handle hXpt 
    )
{
    uint32_t Overflow;
    unsigned BufferNum;

    BERR_Code Status = 0;

    #if BXPT_HAS_IB_PID_PARSERS && BXPT_HAS_RAVE
    Overflow = BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_RAVE_CTRL_BUFFER_EN_IBP ) & 
        BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_RAVE_OVERFLOW_STATUS_IBP );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_RAVE_OVERFLOW_STATUS_IBP, 0 );
    Status |= Overflow;
    for( BufferNum = 0; BufferNum < BXPT_NUM_PID_PARSERS; BufferNum++ )
    {
        if (Overflow & 1 << BufferNum)
            BDBG_ERR(( "XC buffer for IB parser %u to RAVE has overflowed. Consider increasing BXPT_BandWidthConfig.MaxInputRate[%u] or enable RAVE overflow interrupts", BufferNum ));
    }
    #endif

    #if BXPT_HAS_PLAYBACK_PARSERS && BXPT_HAS_RAVE
    Overflow = BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_RAVE_CTRL_BUFFER_EN_PBP ) & 
        BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_RAVE_OVERFLOW_STATUS_PBP );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_RAVE_OVERFLOW_STATUS_PBP, 0 );
    Status |= Overflow;
    for( BufferNum = 0; BufferNum < BXPT_NUM_PLAYBACKS; BufferNum++ )
    {
        if (Overflow & 1 << BufferNum)
            BDBG_ERR(( "XC buffer for PB parser %u to RAVE has overflowed. Consider increasing BXPT_BandWidthConfig.MaxPlaybackRate[%u] or enable RAVE overflow interrupts", BufferNum ));
    }
    #endif

    #if BXPT_HAS_IB_PID_PARSERS && BXPT_HAS_MESG_BUFFERS
    Overflow = BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_MSG_CTRL_BUFFER_EN_IBP ) & 
        BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_MSG_OVERFLOW_STATUS_IBP );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_MSG_OVERFLOW_STATUS_IBP, 0 );
    Status |= Overflow;
    for( BufferNum = 0; BufferNum < BXPT_NUM_PID_PARSERS; BufferNum++ )
    {
        if (Overflow & 1 << BufferNum)
            BDBG_ERR(( "XC buffer for IB parser %u to Mesg filter has overflowed. Consider increasing BXPT_BandWidthConfig.MaxInputRate[%u] or enable Mesg overflow interrupts", BufferNum ));
    }
    #endif

    #if BXPT_HAS_PLAYBACK_PARSERS && BXPT_HAS_MESG_BUFFERS
    Overflow = BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_MSG_CTRL_BUFFER_EN_PBP ) & 
        BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_MSG_OVERFLOW_STATUS_PBP );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_MSG_OVERFLOW_STATUS_PBP, 0 );
    Status |= Overflow;
    for( BufferNum = 0; BufferNum < BXPT_NUM_PLAYBACKS; BufferNum++ )
    {
        if (Overflow & 1 << BufferNum)
            BDBG_ERR(( "XC buffer for PB parser %u to Mesg filter has overflowed. Consider increasing BXPT_BandWidthConfig.MaxPlaybackRate[%u] or enable Mesg overflow interrupts", BufferNum ));
    }
    #endif

    #if BXPT_HAS_IB_PID_PARSERS && BXPT_HAS_REMUX
    Overflow = BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX0_CTRL_BUFFER_EN_IBP ) & 
        BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX0_OVERFLOW_STATUS_IBP );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX0_OVERFLOW_STATUS_IBP, 0 );
    Status |= Overflow;
    for( BufferNum = 0; BufferNum < BXPT_NUM_PID_PARSERS; BufferNum++ )
    {
        if (Overflow & 1 << BufferNum)
            BDBG_ERR(( "XC buffer for IB parser %u to Remux0 has overflowed. Consider increasing BXPT_BandWidthConfig.MaxInputRate[%u]", BufferNum ));
    }

    #if BXPT_NUM_REMULTIPLEXORS > 0
    /* We've got at least RMX1 */
    Overflow = BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX1_CTRL_BUFFER_EN_IBP ) & 
        BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX1_OVERFLOW_STATUS_IBP );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX1_OVERFLOW_STATUS_IBP, 0 );
    Status |= Overflow;
    for( BufferNum = 0; BufferNum < BXPT_NUM_PID_PARSERS; BufferNum++ )
    {
        if (Overflow & 1 << BufferNum)
            BDBG_ERR(( "XC buffer for IB parser %u to Remux0 has overflowed. Consider increasing BXPT_BandWidthConfig.MaxInputRate[%u]", BufferNum ));
    }
    #endif

    #if BXPT_NUM_REMULTIPLEXORS > 2
    #error "Add support for remux2 and higher, input bands"
    #endif

    #endif

    #if BXPT_HAS_PLAYBACK_PARSERS && BXPT_HAS_REMUX
    Overflow = BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX0_CTRL_BUFFER_EN_PBP ) & 
        BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX0_OVERFLOW_STATUS_PBP );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX0_OVERFLOW_STATUS_PBP, 0 );
    Status |= Overflow;
    for( BufferNum = 0; BufferNum < BXPT_NUM_PLAYBACKS; BufferNum++ )
    {
        if (Overflow & 1 << BufferNum)
            BDBG_ERR(( "XC buffer for PB parser %u to Remux0 has overflowed. Consider increasing BXPT_BandWidthConfig.MaxPlaybackRate[%u]", BufferNum ));
    }

    #if BXPT_NUM_REMULTIPLEXORS > 0
    /* We've got at least RMX1 */
    Overflow = BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX1_CTRL_BUFFER_EN_PBP ) & 
        BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX1_OVERFLOW_STATUS_PBP );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_XCBUFF_RMX1_OVERFLOW_STATUS_PBP, 0 );
    Status |= Overflow;
    for( BufferNum = 0; BufferNum < BXPT_NUM_PLAYBACKS; BufferNum++ )
    {
        if (Overflow & 1 << BufferNum)
            BDBG_ERR(( "XC buffer for PB parser %u to Remux0 has overflowed. Consider increasing BXPT_BandWidthConfig.MaxPlaybackRate[%u]", BufferNum ));
    }
    #endif

    #if BXPT_NUM_REMULTIPLEXORS > 2
    #error "Add support for remux2 and higher, input bands"
    #endif

    #endif

    return Status;
}
