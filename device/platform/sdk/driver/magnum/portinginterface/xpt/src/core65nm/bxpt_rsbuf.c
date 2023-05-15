/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_rsbuf.c $
 * $brcm_Revision: Hydra_Software_Devel/22 $
 * $brcm_Date: 4/24/12 8:21a $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/7400/bxpt_rsbuf.c $
 * 
 * Hydra_Software_Devel/22   4/24/12 8:21a gmullen
 * SW7420-2127: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7420-2127/1   4/10/12 4:18p gmullen
 * SW7420-2127: Stored user bandrate in BXPT_XcBuf_SetBandDataRate() and
 * BXPT_RsBuf_SetBandDataRate()
 * 
 * Hydra_Software_Devel/21   10/12/11 9:50a gmullen
 * SW7342-241: Removed duplicated code and update RS blockout calc
 * 
 * Hydra_Software_Devel/20   1/18/11 11:36a rjlewis
 * SW7420-1383: Added support for BDBG_OBJECT interface
 * 
 * Hydra_Software_Devel/19   11/1/10 5:52p gmullen
 * SW7342-241: Ported fix to LATEST. Define SW7432_241_WORKAROUND to
 * enable it
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
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt.h"
#include "bkni.h"
#include "bxpt_rsbuf.h"

#include "bchp_xpt_rsbuff.h"

BDBG_MODULE( xpt_rsbuf );

 
#define MAX_BITRATE					( 108000000 )

#ifdef SW7342_241_WORKAROUND
    #define DEFAULT_PEAK_RATE  			30000000
#else
    #define DEFAULT_PEAK_RATE  			25000000
#endif

#define DEFAULT_BUF_SIZE   			( 200 * 1024 )
#define BLOCKOUT_REG_STEPSIZE		( BCHP_XPT_RSBUFF_BO_IBP1 - BCHP_XPT_RSBUFF_BO_IBP0 )
#define BUFFER_PTR_REG_STEPSIZE		24
  
static BERR_Code DisableBuffer( BXPT_Handle hXpt, unsigned BandNum );
static BERR_Code EnableBuffer( BXPT_Handle hXpt, unsigned BandNum );
static BERR_Code DeleteBuffer( BXPT_Handle hXpt, uint32_t RegAddr );
static BERR_Code AllocateBuffer( BXPT_Handle hXpt, uint32_t RegAddr, unsigned long BufferSize );
static BERR_Code DeletePbBuffer( BXPT_Handle hXpt, unsigned BandNum );
static BERR_Code AllocatePbBuffer( BXPT_Handle hXpt, unsigned BandNum, unsigned long BufferSize );
static BERR_Code SetBandDataRate(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum,				/* [in] Which input band parser to configure */
	unsigned long PeakRate,			/* [in] Max data rate (in bps) the band will handle. */
    unsigned PacketLen             /* [in] Packet size ,130 for dss and 188 for mpeg */
	);
static BERR_Code SetBlockout(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum,             /* [in] Packet size ,130 for dss and 188 for mpeg */
    unsigned long NewBO
	);

BERR_Code BXPT_RsBuf_SetBandDataRate(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum,				/* [in] Which input band parser to configure */
	unsigned long PeakRate,			/* [in] Max data rate (in bps) the band will handle. */
    unsigned PacketLen             /* [in] Packet size ,130 for dss and 188 for mpeg */
	)
{                                               
#ifdef SW7342_241_WORKAROUND
    BSTD_UNUSED( hXpt );
    BSTD_UNUSED( BandNum );
    BSTD_UNUSED( PeakRate );
    BSTD_UNUSED( PacketLen );
    BDBG_ERR(( "BXPT_RsBuf_SetBandDataRate() ignored" ));
    return BERR_SUCCESS;
#else
    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    hXpt->RsBufBO[ BandNum ] = BXPT_P_RsBuf_ComputeBlockOut( PeakRate, PacketLen );
    return SetBandDataRate( hXpt, BandNum, PeakRate, PacketLen );
#endif
}

static BERR_Code SetBandDataRate(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum,				/* [in] Which input band parser to configure */
	unsigned long PeakRate,			/* [in] Max data rate (in bps) the band will handle. */
    unsigned PacketLen             /* [in] Packet size ,130 for dss and 188 for mpeg */
	)
{
	uint32_t NewBO;

	BERR_Code ExitCode = BERR_SUCCESS;

    NewBO = BXPT_P_RsBuf_ComputeBlockOut( PeakRate, PacketLen );
    ExitCode = SetBlockout( hXpt, BandNum, NewBO );
	return( ExitCode );
}

BERR_Code BXPT_RsBuf_SetPlaybackDataRate(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned PbNum,					/* [in] Which playback band parser to configure */
	unsigned long PeakRate			/* [in] Max data rate (in bps) the band will handle. */
	)
{
	uint32_t Reg, RegAddr, NewBO;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    NewBO = BXPT_P_RsBuf_ComputeBlockOut( PeakRate, 188 );

	RegAddr = BCHP_XPT_RSBUFF_BO_PBP0 + ( PbNum * BLOCKOUT_REG_STEPSIZE );
	Reg = BREG_Read32( hXpt->hRegister, RegAddr );
	Reg &= ~BCHP_MASK( XPT_RSBUFF_BO_PBP0, BO_COUNT );
	Reg |= BCHP_FIELD_DATA( XPT_RSBUFF_BO_PBP0, BO_COUNT, NewBO );
	BREG_Write32( hXpt->hRegister, RegAddr, Reg );

	return( ExitCode );
}


uint32_t BXPT_P_RsBuf_ComputeBlockOut( 
	unsigned long PeakRate,			/* [in] Max data rate (in bps) the band will handle. */
    unsigned PacketLen             /* [in] Packet size ,130 for dss and 188 for mpeg */
    )
{
    uint32_t NewBO;

    NewBO = MAX_BITRATE / 1000000;
    PeakRate = PeakRate / 1000000;
    NewBO = ( NewBO * PacketLen * 8 ) / PeakRate;  /* default ,set for mpeg ts */    
    return NewBO;                
}

BERR_Code BXPT_RsBuf_SetBufferSize(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum,				/* [in] Which input band parser to configure */
	unsigned long BufferSize		/* [in] Buffer size in bytes */
	)
{
	uint32_t RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    if( BXPT_P_RsBuf_IsBufferEnabled( hXpt, BandNum ) )
    {
	    RegAddr = BCHP_XPT_RSBUFF_BASE_POINTER_IBP0 + ( BandNum * BUFFER_PTR_REG_STEPSIZE ); 

	    DisableBuffer( hXpt, BandNum );
	    DeleteBuffer( hXpt, RegAddr );
	    AllocateBuffer( hXpt, RegAddr, BufferSize );
	    EnableBuffer( hXpt, BandNum );
	}
    else
    {
        /* BXPT_DramBufferCfg in BXPT_DefaultSettings didn't request a buffer. */
		BDBG_ERR(( "Buffer BandNum %lu not used!", BandNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }

	return( ExitCode );
}

BERR_Code BXPT_RsBuf_SetPlaybackBufferSize(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum,				/* [in] Which input band parser to configure */
	unsigned long BufferSize		/* [in] Buffer size in bytes */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    if( BXPT_P_RsBuf_IsBufferEnabled( hXpt, BandNum + BXPT_P_PLAYBACK_ENABLE_BIT_OFFSET ) )
    {
	    DisableBuffer( hXpt, BandNum + BXPT_P_PLAYBACK_ENABLE_BIT_OFFSET );
	    DeletePbBuffer( hXpt, BandNum );
	    AllocatePbBuffer( hXpt, BandNum, BufferSize );
	    EnableBuffer( hXpt, BandNum + BXPT_P_PLAYBACK_ENABLE_BIT_OFFSET );
	}
    else
    {
        /* BXPT_DramBufferCfg in BXPT_DefaultSettings didn't request a buffer. */
		BDBG_ERR(( "Buffer BandNum %lu not used!", BandNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
	
	return( ExitCode );
}

BERR_Code BXPT_P_RsBuf_Init(
	BXPT_Handle hXpt, 	   	   	/* [in] Handle for this transport */
    const BXPT_DramBufferCfg *Cfg
	)
{
	unsigned BandNum;
    unsigned BufferSize;
	uint32_t RegAddr;
    unsigned ii;
    uint32_t InitialBlockOut;

	BERR_Code ExitCode = BERR_SUCCESS;
    unsigned long TotalRsMemory = 0;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
	BDBG_ASSERT( Cfg );

    InitialBlockOut = BXPT_P_RsBuf_ComputeBlockOut( BXPT_P_DEFAULT_PEAK_RATE, 188 );
    for( ii = 0; ii < BXPT_P_MAX_PID_PARSERS; ii++ )
        hXpt->RsBufBO[ ii ] = InitialBlockOut;
    
	/* Set up RS buffers for the parser bands (not the input bands.) */
	for( BandNum = 0; BandNum < hXpt->MaxPidParsers; BandNum++ )
	{
        /* PR 25771: Need a buffer of at least 256 bytes. */
        if( Cfg->IbParserRsSize[ BandNum ] )
            BufferSize = Cfg->IbParserRsSize[ BandNum ] * 1024;
        else
            BufferSize = 256;

        SetBandDataRate( hXpt, BandNum, DEFAULT_PEAK_RATE,188 );

        RegAddr = BCHP_XPT_RSBUFF_BASE_POINTER_IBP0 + ( BandNum * BUFFER_PTR_REG_STEPSIZE );
        AllocateBuffer( hXpt, RegAddr, BufferSize );
        EnableBuffer( hXpt, BandNum );

        TotalRsMemory += BufferSize;
        BDBG_MSG(( "Enabling RS buffer for IB parser %lu", BandNum ));
	}

	/* Set up RS buffers for the playback channels. */
	for( BandNum = 0; BandNum < hXpt->MaxPlaybacks; BandNum++ )
	{
        /* PR 25771: Need a buffer of at least 256 bytes. */
        if( Cfg->PbParserRsSize[ BandNum ] )
            BufferSize = Cfg->PbParserRsSize[ BandNum ] * 1024;
        else
            BufferSize = 256;

        BXPT_RsBuf_SetPlaybackDataRate( hXpt, BandNum, DEFAULT_PEAK_RATE );

        AllocatePbBuffer( hXpt, BandNum, BufferSize );
        EnableBuffer( hXpt, BandNum + BXPT_P_PLAYBACK_ENABLE_BIT_OFFSET );

        TotalRsMemory += BufferSize;
        BDBG_MSG(( "Enabling RS buffer for PB parser %lu", BandNum ));
	}

    /*
    ** WORKAROUND. PR 22836: If PB0 support is enabled, PB1 support must be also. But 
    ** check that PB1 support isn't already enabled..
    ** NOTE: PR 22836 is covered by the workaround for PR 25771.
    */

    BDBG_MSG(( "Total RS memory usage is %lu bytes", TotalRsMemory ));

    /* Store the initial BlockOut values we've set above */
    for( ii = 0; ii < BXPT_P_MAX_PID_PARSERS; ii++ )
        hXpt->RsBufBO[ ii ] = BXPT_P_RsBuf_GetBlockout( hXpt, ii );

	return( ExitCode );
}

BERR_Code BXPT_P_RsBuf_Shutdown(
	BXPT_Handle hXpt 	   	   	/* [in] Handle for this transport */
	)
{
	unsigned BandNum;
	uint32_t RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	/* Delete the input band buffers */
	for( BandNum = 0; BandNum < hXpt->MaxPidParsers; BandNum++ )
	{
        if( BXPT_P_RsBuf_IsBufferEnabled( hXpt, BandNum ) )
        {
		    RegAddr = BCHP_XPT_RSBUFF_BASE_POINTER_IBP0 + ( BandNum * BUFFER_PTR_REG_STEPSIZE );
		    DisableBuffer( hXpt, BandNum );
		    DeleteBuffer( hXpt, RegAddr );
        }
	}

	/* Delete the parser band buffers */
	for( BandNum = 0; BandNum < hXpt->MaxPlaybacks; BandNum++ )
	{
        if( BXPT_P_RsBuf_IsBufferEnabled( hXpt, BandNum + BXPT_P_PLAYBACK_ENABLE_BIT_OFFSET ) )
        {
		    DisableBuffer( hXpt, BandNum + BXPT_P_PLAYBACK_ENABLE_BIT_OFFSET );
		    DeletePbBuffer( hXpt, BandNum );
        }
	}

	return( ExitCode );
}

static BERR_Code DisableBuffer( 
	BXPT_Handle hXpt, 
	unsigned BandNum
	)
{
	uint32_t EnReg;

	BERR_Code ExitCode = BERR_SUCCESS;

	EnReg = BREG_Read32( hXpt->hRegister, BCHP_XPT_RSBUFF_BUFFER_ENABLE );
   	EnReg &= ~( 1ul << BandNum );
	BREG_Write32( hXpt->hRegister, BCHP_XPT_RSBUFF_BUFFER_ENABLE, EnReg );	
	
	return( ExitCode );
}

static BERR_Code EnableBuffer( 
	BXPT_Handle hXpt, 
	unsigned BandNum
	)
{
	uint32_t EnReg;

	BERR_Code ExitCode = BERR_SUCCESS;

	EnReg = BREG_Read32( hXpt->hRegister, BCHP_XPT_RSBUFF_BUFFER_ENABLE );
	EnReg |= ( 1ul << BandNum );
	BREG_Write32( hXpt->hRegister, BCHP_XPT_RSBUFF_BUFFER_ENABLE, EnReg );	
	
	return( ExitCode );
}

static BERR_Code DeleteBuffer(
	BXPT_Handle hXpt,                                                
	uint32_t RegAddr
	)
{
	uint32_t Offset;
	void *Buffer;

	BERR_Code ExitCode = BERR_SUCCESS;

    /* If there is a secure heap defined, use it. */
    BMEM_Handle hMem = hXpt->hRHeap ? hXpt->hRHeap : hXpt->hMemory;

	Offset = BREG_Read32( hXpt->hRegister, RegAddr );
	BMEM_ConvertOffsetToAddress( hMem, Offset, &Buffer );
	BMEM_Free( hMem, Buffer );
	
	return( ExitCode );
}

static BERR_Code AllocateBuffer(
	BXPT_Handle hXpt, 	   			
	uint32_t RegAddr,
	unsigned long BufferSize		
	)
{
	uint32_t Offset;
    void *Buffer;

	BERR_Code ExitCode = BERR_SUCCESS;

    /* If there is a secure heap defined, use it. */
    BMEM_Handle hMem = hXpt->hRHeap ? hXpt->hRHeap : hXpt->hMemory;

	/* Size must be a multiple of 256 */
	BufferSize = BufferSize - ( BufferSize % 256 );

	Buffer = BMEM_AllocAligned( hMem, BufferSize, 8, 0 ); 
    if( !Buffer )
    {
        BDBG_ERR(( "RS front-end buffer alloc failed!" ));
        ExitCode = BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
        goto Done;
    }

	BMEM_ConvertAddressToOffset( hMem, Buffer, &Offset );

	BREG_Write32( hXpt->hRegister, RegAddr, Offset );						/* Set BASE */
	BREG_Write32( hXpt->hRegister, RegAddr + 4, Offset + BufferSize - 1 );	/* Set END */
	BREG_Write32( hXpt->hRegister, RegAddr + 8, Offset - 1 );				/* Set WRITE */
	BREG_Write32( hXpt->hRegister, RegAddr + 12, Offset - 1 );				/* Set VALID */
	BREG_Write32( hXpt->hRegister, RegAddr + 16, Offset - 1 );				/* Set READ */
	BREG_Write32( hXpt->hRegister, RegAddr + 20, 0 );						/* Set WATERMARK */
	
    Done:
	return( ExitCode );
}

bool BXPT_P_RsBuf_IsBufferEnabled( 
	BXPT_Handle hXpt, 
    unsigned BandNum
    )
{
	uint32_t EnReg;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	EnReg = BREG_Read32( hXpt->hRegister, BCHP_XPT_RSBUFF_BUFFER_ENABLE );
    return EnReg & ( 1ul << BandNum ) ? true : false;
}

/*
** On some chips, there is a memory hole between BCHP_XPT_RSBUFF_WRITE_POINTER_PBP1 and
** BCHP_XPT_RSBUFF_VALID_POINTER_PBP1: The address delta between these two registers is
** 8 bytes, not 4. So, here we define to handle that hole. Note that the define also
** handles the possibility that the hole gets fixed. 
*/

#ifdef BCHP_XPT_RSBUFF_BASE_POINTER_PBP1
    #define WRITE_VALID_REG_DELTA ( BCHP_XPT_RSBUFF_VALID_POINTER_PBP1 - BCHP_XPT_RSBUFF_WRITE_POINTER_PBP1 - 4 )
#else
    #define WRITE_VALID_REG_DELTA ( 0 )
#endif

static uint32_t GetPbBaseAddr( 
    unsigned BandNum
    )
{
    uint32_t RegAddr = BCHP_XPT_RSBUFF_BASE_POINTER_PBP0 + ( BandNum * BUFFER_PTR_REG_STEPSIZE );

    /* Handle the memory hole */
    switch( BandNum )
    {
        case 0:     /* Easy, do nothing: The hole is above the BASE register */
        case 1:     
        break;

        default:    /* Everybody else has an address shifted up */
        RegAddr += WRITE_VALID_REG_DELTA;
        break;
    }

    return RegAddr;
}

static BERR_Code DeletePbBuffer(
	BXPT_Handle hXpt,                                                
	unsigned BandNum
	)
{
	uint32_t Offset, RegAddr;
	void *Buffer;

	BERR_Code ExitCode = BERR_SUCCESS;

    /* If there is a secure heap defined, use it. */
    BMEM_Handle hMem = hXpt->hRHeap ? hXpt->hRHeap : hXpt->hMemory;

    RegAddr = GetPbBaseAddr( BandNum );

	Offset = BREG_Read32( hXpt->hRegister, RegAddr );
	BMEM_ConvertOffsetToAddress( hMem, Offset, &Buffer );
	BMEM_Free( hMem, Buffer );
	
	return( ExitCode );
}

static BERR_Code AllocatePbBuffer(
	BXPT_Handle hXpt, 	   			
	unsigned BandNum,
	unsigned long BufferSize		
	)
{
	uint32_t Offset, RegAddr;
    void *Buffer;

	BERR_Code ExitCode = BERR_SUCCESS;

    /* If there is a secure heap defined, use it. */
    BMEM_Handle hMem = hXpt->hRHeap ? hXpt->hRHeap : hXpt->hMemory;

	/* Size must be a multiple of 256 */
	BufferSize = BufferSize - ( BufferSize % 256 );

	Buffer = BMEM_AllocAligned( hMem, BufferSize, 8, 0 ); 

    if( !Buffer )
    {
        BDBG_ERR(( "RS playback buffer alloc failed!" ));
        ExitCode = BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
        goto Done;
   }

	BMEM_ConvertAddressToOffset( hMem, Buffer, &Offset );

    RegAddr = GetPbBaseAddr( BandNum );
    
    /* Handle the memory hole (again) */
    switch( BandNum )
    {
        case 1:     /* Take care of the step between WRITE and VALID. */
	    BREG_Write32( hXpt->hRegister, RegAddr, Offset );						/* Set BASE */
	    BREG_Write32( hXpt->hRegister, RegAddr + 4, Offset + BufferSize - 1 );	/* Set END */
	    BREG_Write32( hXpt->hRegister, RegAddr + 8, Offset - 1 );				/* Set WRITE */
	    BREG_Write32( hXpt->hRegister, RegAddr + 12 + WRITE_VALID_REG_DELTA, Offset - 1 );	/* Set VALID */
	    BREG_Write32( hXpt->hRegister, RegAddr + 16 + WRITE_VALID_REG_DELTA, Offset - 1 );	/* Set READ */
	    BREG_Write32( hXpt->hRegister, RegAddr + 20 + WRITE_VALID_REG_DELTA, 0 );			/* Set WATERMARK */
        break;

        default:    /* All other registers are okay */
	    BREG_Write32( hXpt->hRegister, RegAddr, Offset );						/* Set BASE */
	    BREG_Write32( hXpt->hRegister, RegAddr + 4, Offset + BufferSize - 1 );	/* Set END */
	    BREG_Write32( hXpt->hRegister, RegAddr + 8, Offset - 1 );				/* Set WRITE */
	    BREG_Write32( hXpt->hRegister, RegAddr + 12, Offset - 1 );				/* Set VALID */
	    BREG_Write32( hXpt->hRegister, RegAddr + 16, Offset - 1 );				/* Set READ */
	    BREG_Write32( hXpt->hRegister, RegAddr + 20, 0 );						/* Set WATERMARK */
        break;
    }
    
    Done:	
	return( ExitCode );
}

unsigned long BXPT_P_RsBuf_GetBlockout(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum				/* [in] Which input band parser to configure */
	)
{
	uint32_t Reg, RegAddr;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	RegAddr = BCHP_XPT_RSBUFF_BO_IBP0 + ( BandNum * BLOCKOUT_REG_STEPSIZE );
	Reg = BREG_Read32( hXpt->hRegister, RegAddr );
	return( BCHP_GET_FIELD_DATA( Reg, XPT_RSBUFF_BO_IBP0, BO_COUNT ) );
}

BERR_Code BXPT_P_RsBuf_SetBlockout(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum,             /* [in] Packet size ,130 for dss and 188 for mpeg */
    unsigned long NewBO
	)
{
#ifdef SW7342_241_WORKAROUND
    BSTD_UNUSED( hXpt );
    BSTD_UNUSED( BandNum );
    BSTD_UNUSED( NewBO );
    BDBG_ERR(( "BXPT_P_RsBuf_SetBlockout() ignored" ));
    return BERR_SUCCESS;
#else
    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    return SetBlockout( hXpt, BandNum, NewBO );
#endif
}

static BERR_Code SetBlockout(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum,             /* [in] Packet size ,130 for dss and 188 for mpeg */
    unsigned long NewBO
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

	RegAddr = BCHP_XPT_RSBUFF_BO_IBP0 + ( BandNum * BLOCKOUT_REG_STEPSIZE );
	Reg = BREG_Read32( hXpt->hRegister, RegAddr );
	Reg &= ~BCHP_MASK( XPT_RSBUFF_BO_IBP0, BO_COUNT );
	Reg |= BCHP_FIELD_DATA( XPT_RSBUFF_BO_IBP0, BO_COUNT, NewBO );
	BREG_Write32( hXpt->hRegister, RegAddr, Reg );

	return( ExitCode );
}

BERR_Code BXPT_P_RsBuf_PlaybackSetBlockout(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum,             /* [in] Packet size ,130 for dss and 188 for mpeg */
    unsigned long NewBO
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	RegAddr = BCHP_XPT_RSBUFF_BO_PBP0 + ( BandNum * BLOCKOUT_REG_STEPSIZE );
	Reg = BREG_Read32( hXpt->hRegister, RegAddr );
	Reg &= ~BCHP_MASK( XPT_RSBUFF_BO_PBP0, BO_COUNT );
	Reg |= BCHP_FIELD_DATA( XPT_RSBUFF_BO_PBP0, BO_COUNT, NewBO );
	BREG_Write32( hXpt->hRegister, RegAddr, Reg );

	return( ExitCode );
}

