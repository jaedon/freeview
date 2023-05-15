/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_interrupt.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 8/4/11 11:07a $
 *
 * Porting interface code for the data transport interrupt handlers. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/base2/bxpt_interrupt.c $
 * 
 * Hydra_Software_Devel/3   8/4/11 11:07a gmullen
 * SW7346-399: Fixed coverity warnings
 * 
 * Hydra_Software_Devel/2   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 * Hydra_Software_Devel/2   11/27/06 4:24p katrep
 * PR25431: Added code to detect the lost data ready interrupt and
 * generate sw callback
 * 
 * Hydra_Software_Devel/2   8/26/05 2:01p gmullen
 * PR15309: Added DirecTV support, RAVE video support, cleaned up RS and
 * XC buffer code.
 * 
 * Hydra_Software_Devel/1   7/15/05 9:03a gmullen
 * PR15309: Inita i
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxpt.h"
#include "bxpt_interrupt.h"
#include "bkni.h"
#include "bxpt_priv.h"

#include "bchp_xpt_msg.h"

#define INTERRUPT_BITS_PER_REGISTER		( 32 )
#define REGISTER_SIZE					( 4 )	

BDBG_MODULE( xpt_interrupt );


static void MapBitAndAddress( 
	int MessageBufferNum, 
	uint32_t InAddress,
	uint32_t *BitNum,
	uint32_t *OutAddress 
	);

static void SetBit( 
	BXPT_Handle hXpt, 	   					/* [in] Handle for this transport */
	uint32_t Addr,
	int BitNum
	);

static void ClearBit( 
	BXPT_Handle hXpt, 	   					/* [in] Handle for this transport */
	uint32_t Addr,
	int BitNum
	);

BERR_Code BXPT_Interrupt_EnableMessageInt( 
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	int MessageBufferNum,		/* [in] Which message buffer to watch. */
	BINT_CallbackFunc Callback,		/* [in] Handler for this interrupt. */
	void *Parm1,					/* [in] First arg to be passed to callback */
	int Parm2						/* [in] Second arg to be passed to callback */
	)
{
	BERR_Code ExitCode;

	BKNI_EnterCriticalSection();
	ExitCode = BXPT_Interrupt_EnableMessageInt_isr( hXpt, MessageBufferNum, Callback, Parm1, Parm2 );
	BKNI_LeaveCriticalSection();

	return ExitCode;
}

BERR_Code BXPT_Interrupt_DisableMessageInt( 
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	int MessageBufferNum		/* [in] Message interrupt to disable. */
	)
{
	BERR_Code ExitCode;

	BKNI_EnterCriticalSection();
	ExitCode = BXPT_Interrupt_DisableMessageInt_isr( hXpt, MessageBufferNum );
	BKNI_LeaveCriticalSection();

	return ExitCode;
}

BERR_Code BXPT_Interrupt_EnableMessageInt_isr( 
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	int MessageBufferNum,		/* [in] Which message buffer to watch. */
	BINT_CallbackFunc Callback,		/* [in] Handler for this interrupt. */
	void *Parm1,					/* [in] First arg to be passed to callback */
	int Parm2						/* [in] Second arg to be passed to callback */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( Callback );
											   
	if( MessageBufferNum >= BXPT_NUM_MESG_BUFFERS )
	{
		/* Bad interrupt number. Complain. */
		BDBG_ERR(( "MessageBufferNum %lu is out of range!", ( unsigned long ) MessageBufferNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		uint32_t BitNum = 0;
		uint32_t RegAddr = 0;

		/* Load the callback into the jump table. */
		hXpt->MesgIntrCallbacks[ MessageBufferNum ].Callback = Callback;
		hXpt->MesgIntrCallbacks[ MessageBufferNum ].Parm1 = Parm1;
		hXpt->MesgIntrCallbacks[ MessageBufferNum ].Parm2 = Parm2;

		MapBitAndAddress( MessageBufferNum, BCHP_XPT_MSG_BUF_DAT_RDY_INTR_EN_00_31, &BitNum, &RegAddr );
		SetBit( hXpt, RegAddr, BitNum );
	}

	return( ExitCode );
}


BERR_Code BXPT_Interrupt_DisableMessageInt_isr( 
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	int MessageBufferNum		/* [in] Message interrupt to disable. */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	
	if( MessageBufferNum >= BXPT_NUM_MESG_BUFFERS )
	{
		/* Bad interrupt number. Complain. */
		BDBG_ERR(( "MessageBufferNum %lu is out of range!", ( unsigned long ) MessageBufferNum ));
		ExitCode = BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	else
	{				 
		uint32_t BitNum = 0;
		uint32_t RegAddr = 0;
	 
		MapBitAndAddress( MessageBufferNum, BCHP_XPT_MSG_BUF_DAT_RDY_INTR_EN_00_31, &BitNum, &RegAddr );
		ClearBit( hXpt, RegAddr, BitNum );

		/* Clear the callback entry the jump table. */
		hXpt->MesgIntrCallbacks[ MessageBufferNum ].Callback = ( BINT_CallbackFunc ) NULL;
 	}

	return( ExitCode );
}


BERR_Code BXPT_Interrupt_EnableMessageOverflowInt( 
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	int MessageBufferNum,		/* [in] Which message buffer to watch. */
	BINT_CallbackFunc Callback,		/* [in] Handler for this interrupt. */
	void *Parm1,					/* [in] First arg to be passed to callback */
	int Parm2						/* [in] Second arg to be passed to callback */
	)
{
	BERR_Code ExitCode;

	BKNI_EnterCriticalSection();
	ExitCode = BXPT_Interrupt_EnableMessageOverflowInt_isr( hXpt, MessageBufferNum, Callback, Parm1, Parm2 );
	BKNI_LeaveCriticalSection();

	return ExitCode;
}


BERR_Code BXPT_Interrupt_DisableMessageOverflowInt( 
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	int MessageBufferNum		/* [in] Message interrupt to disable. */
	)
{
	BERR_Code ExitCode;

	BKNI_EnterCriticalSection();
	ExitCode = BXPT_Interrupt_DisableMessageOverflowInt_isr( hXpt, MessageBufferNum );
	BKNI_LeaveCriticalSection();

	return ExitCode;
}


BERR_Code BXPT_Interrupt_EnableMessageOverflowInt_isr( 
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	int MessageBufferNum,		/* [in] Which message buffer to watch. */
	BINT_CallbackFunc Callback,		/* [in] Handler for this interrupt. */
	void *Parm1,					/* [in] First arg to be passed to callback */
	int Parm2						/* [in] Second arg to be passed to callback */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( Callback );
											   
	if( MessageBufferNum >= BXPT_NUM_MESG_BUFFERS )
	{
		/* Bad interrupt number. Complain. */
		BDBG_ERR(( "MessageBufferNum %lu is out of range!", ( unsigned long ) MessageBufferNum ));
		ExitCode = BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	else
	{
		uint32_t BitNum = 0;
		uint32_t RegAddr = 0;

		/* Load the callback into the jump table. */
		hXpt->OverflowIntrCallbacks[ MessageBufferNum ].Callback = Callback;
		hXpt->OverflowIntrCallbacks[ MessageBufferNum ].Parm1 = Parm1;
		hXpt->OverflowIntrCallbacks[ MessageBufferNum ].Parm2 = Parm2;

		MapBitAndAddress( MessageBufferNum, BCHP_XPT_MSG_BUF_OVFL_INTR_EN_00_31, &BitNum, &RegAddr );
	   	SetBit( hXpt, RegAddr, BitNum );
	}

	return( ExitCode );
}


BERR_Code BXPT_Interrupt_DisableMessageOverflowInt_isr( 
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	int MessageBufferNum		/* [in] Message interrupt to disable. */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	
	if( MessageBufferNum >= BXPT_NUM_MESG_BUFFERS )
	{
		/* Bad interrupt number. Complain. */
		BDBG_ERR(( "MessageBufferNum %lu is out of range!", ( unsigned long ) MessageBufferNum ));
		ExitCode = BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	else
	{
		uint32_t BitNum = 0;
		uint32_t RegAddr = 0;

		MapBitAndAddress( MessageBufferNum, BCHP_XPT_MSG_BUF_OVFL_INTR_EN_00_31, &BitNum, &RegAddr );
		ClearBit( hXpt, RegAddr, BitNum );
	
		MapBitAndAddress( MessageBufferNum, BCHP_XPT_MSG_BUF_OVFL_INTR_00_31, &BitNum, &RegAddr );
		ClearBit( hXpt, RegAddr, BitNum );

		/* Clear the callback entry the jump table. */
		hXpt->OverflowIntrCallbacks[ MessageBufferNum ].Callback = ( BINT_CallbackFunc ) NULL;
 	}

	return( ExitCode );
}

void BXPT_P_Interrupt_MsgVector_isr( 
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	int L1Shift 		/* [in] Dummy arg. Not used by this interface. */
	)
{
	unsigned i, j;
	uint32_t Status, Mask, StatusAddr, MaskAddr;

	BDBG_ASSERT( hXpt );
	BSTD_UNUSED( L1Shift ); 

	for( i = 0; i < BXPT_NUM_MESG_BUFFERS; i += INTERRUPT_BITS_PER_REGISTER )
	{
		StatusAddr = BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31 + ( REGISTER_SIZE * ( i / INTERRUPT_BITS_PER_REGISTER ));
		Status = BREG_Read32( hXpt->hRegister, StatusAddr );

		MaskAddr = BCHP_XPT_MSG_BUF_DAT_RDY_INTR_EN_00_31 + ( REGISTER_SIZE * ( i / INTERRUPT_BITS_PER_REGISTER ));
		Mask = BREG_Read32( hXpt->hRegister, MaskAddr );

		if( Status & Mask )
		{
			for( j = 0; j < INTERRUPT_BITS_PER_REGISTER; j++ )
			{
				if( ( Status & Mask ) & ( 1ul << j ) )
				{
					BXPT_P_InterruptCallbackArgs *Cb = &hXpt->MesgIntrCallbacks[ i + j ];
					if( Cb->Callback != ( BINT_CallbackFunc ) NULL )
						(*( Cb->Callback )) ( Cb->Parm1, Cb->Parm2 );
				}
			}		
		}
	}
}

void BXPT_P_Interrupt_MsgSw_isr(
    BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
    int MessageBufferNum		    /* [in] Message Buffer */
    )
{
    BXPT_P_InterruptCallbackArgs *Cb = &hXpt->MesgIntrCallbacks[ MessageBufferNum ];
    if( Cb->Callback != ( BINT_CallbackFunc ) NULL )
			(*( Cb->Callback )) ( Cb->Parm1, Cb->Parm2 );
}

void BXPT_P_Interrupt_MsgOverflowVector_isr( 
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	int L1Shift 		/* [in] Dummy arg. Not used by this interface. */
	)
{
	unsigned i, j;
	uint32_t Status, Mask, StatusAddr, MaskAddr;

	BDBG_ASSERT( hXpt );
	BSTD_UNUSED( L1Shift ); 

	for( i = 0; i < BXPT_NUM_MESG_BUFFERS; i += INTERRUPT_BITS_PER_REGISTER )
	{
		StatusAddr = BCHP_XPT_MSG_BUF_OVFL_INTR_00_31 + ( REGISTER_SIZE * ( i / INTERRUPT_BITS_PER_REGISTER ));
		Status = BREG_Read32( hXpt->hRegister, StatusAddr );

		MaskAddr = BCHP_XPT_MSG_BUF_OVFL_INTR_EN_00_31 + ( REGISTER_SIZE * ( i / INTERRUPT_BITS_PER_REGISTER ));
		Mask = BREG_Read32( hXpt->hRegister, MaskAddr );

		if( Status & Mask )
		{
			for( j = 0; j < INTERRUPT_BITS_PER_REGISTER; j++ )
			{
				if( ( Status & Mask ) & ( 1ul << j ) )
				{
					BXPT_P_InterruptCallbackArgs *Cb = &hXpt->OverflowIntrCallbacks[ i + j ];

					if( Cb->Callback != ( BINT_CallbackFunc ) NULL )
						(*( Cb->Callback )) ( Cb->Parm1, Cb->Parm2 );
				}
			}		
		}
	}
}
 
static void MapBitAndAddress( 
	int MessageBufferNum, 
	uint32_t InAddress,
	uint32_t *BitNum,
	uint32_t *OutAddress 
	)
{
	*OutAddress = InAddress + ( REGISTER_SIZE * ( MessageBufferNum / INTERRUPT_BITS_PER_REGISTER ) );
	*BitNum = MessageBufferNum % INTERRUPT_BITS_PER_REGISTER;
}

static void SetBit( 
	BXPT_Handle hXpt, 	   					/* [in] Handle for this transport */
	uint32_t Addr,
	int BitNum
	)
{
	uint32_t EnReg = BREG_Read32( hXpt->hRegister, Addr );

	EnReg |= ( 1ul << BitNum );
	BREG_Write32( hXpt->hRegister, Addr, EnReg );
}
	

static void ClearBit( 
	BXPT_Handle hXpt, 	   					/* [in] Handle for this transport */
	uint32_t Addr,
	int BitNum
	)
{
	uint32_t Reg = BREG_Read32( hXpt->hRegister, Addr );
	
	Reg &= ~( 1ul << BitNum );
	BREG_Write32( hXpt->hRegister, Addr, Reg );
}


/* end of file */

