/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
** This file is a Windows specific example implementation of an ND-Receiver.
*/

#include <drmwindowsenv.h>      /* for the Sleep() function */

#include <drmcommon.h>
#include <ndtnetmsg.h>
#include <Wire.h>
#include <drmndr.h>

ENTER_PK_NAMESPACE_CODE;

#define WMDRMNET_PROXIMITY_DETECTION_INTERVAL   30 /* milliseconds */


/*****************************************************************************
** Function:    _CreateProximityContext
**
** Synopsis:    Allocates a proximity context.  The proximity context holds all
**              state describing a receiver's progress in the proximity detection
**              algorithm.  Such state is held in a context to allow the algorithm
**              to be executed simulataneously with multiple transmitters.
**
** Arguments:   None.
**
** Returns:     A pointer to the proximity context, 
**              or NULL if memory allocation fails
**
** Notes:       None.
**
******************************************************************************/
static DRM_ND_PROXIMITY_CONTEXT *_CreateProximityContext( void )
{
    DRM_ND_PROXIMITY_CONTEXT *pProximityContext = NULL;

    /*
    ** Allocate the context
    */
    pProximityContext = (DRM_ND_PROXIMITY_CONTEXT *)Oem_MemAlloc( SIZEOF( DRM_ND_PROXIMITY_CONTEXT ) );

    if ( pProximityContext != NULL )
    {
        /*
        ** Initialize the context to a known state
        */
        MEMSET( pProximityContext, 0, SIZEOF( DRM_ND_PROXIMITY_CONTEXT ) );
        pProximityContext->State = NDR_InitialState;
    }

    return pProximityContext;
}



/*****************************************************************************
** Function:    _DeleteProximityContext
**
** Synopsis:    Frees a proximity context.
**
** Arguments:   [f_pProximityContext] : The context to free; may be NULL
**
** Returns:     void
**
** Notes:       None.
**
******************************************************************************/
static DRM_VOID _DeleteProximityContext(
    __in_opt DRM_ND_PROXIMITY_CONTEXT *f_pProximityContext
)
{
    /*
    ** Free the address field, proximity content encryption key, and the context itself
    */
    SAFE_OEM_FREE( f_pProximityContext->Address );
    SAFE_OEM_FREE( f_pProximityContext->ProximityContentEncryptionKey );
    SAFE_OEM_FREE( f_pProximityContext );
}



/*****************************************************************************
** Function:    _AllocateStack
**
** Synopsis:    Allocates 4K of memory for a stack allocator context.
**
** Arguments:   [f_pStack] : Pointer to a stack allocator context
**
** Returns:     DRM_SUCCESS       - on success
**              DRM_E_OUTOFMEMORY - if memory allocation fails
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _AllocateStack( 
    __in       DRM_STACK_ALLOCATOR_CONTEXT    *f_pStack
)
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_DWORD  cbMaxStack = 4096;
    DRM_BYTE  *pbMemory   = NULL;

    ChkMem( pbMemory = (DRM_BYTE *)Oem_MemAlloc( cbMaxStack ) );

    f_pStack->nStackTop = 0;
    f_pStack->pbStack   = pbMemory;
    f_pStack->cbStack   = cbMaxStack;
    
    ZEROMEM( pbMemory, cbMaxStack );

 ErrorExit:
    return dr;
}



/*****************************************************************************
** Function:    NDR_ReceiverMain
**
** Synopsis:    Drives the entire receiver side process; it demonstrates an example
**              state change sequence for a Digital Media Receiver (DMR).
**
**              The DMR starts off in an initial state, registers with the transmitter, 
**              performs proximity detection and then requests for a License. Note 
**              that it is not necessary for a DMR to go through this sequence in all
**              cases: this state change is only illustrative of how a DMR performs 
**              certain actions like registration, proximity detection etc. For example,
**              in a real world scenario, a DMR that has already registered and
**              validated itself could make multiple requests for policies for different
**              content.
**
** Arguments:   None.
**
** Returns:     DRM_SUCCESS - on success
**              or other return code from a function called within
**
** Notes:       None.
**
******************************************************************************/
DRM_RESULT DRM_CALL NDR_ReceiverMain( void )
{
    DRM_RESULT dr;

    DRM_ND_PROXIMITY_CONTEXT *pProximityContext = NULL;
    DRM_CRYPTO_CONTEXT       *pCryptoCtx        = NULL;

    DRM_BYTE  bMessageType      = 0;
    DRM_BYTE *pbMessageToSend   = NULL;
    DRM_DWORD cbMessageToSend   = 0;
    DRM_BYTE *pbMessageReceived = NULL;
    DRM_DWORD cbMessageReceived = 0;

    DRM_VOID *hWire                  = NULL;
    DRM_BOOL  fFirstProximityAttempt = TRUE;
    DRM_WORD  wResult                = 0;

    /*
    ** Allocate memory for crypto context.
    */      
    ChkMem( pCryptoCtx = (DRM_CRYPTO_CONTEXT *)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pCryptoCtx, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    /*
    ** Establish a context for proximity detection
    */
    ChkMem( pProximityContext = _CreateProximityContext() );

    /*
    ** Open a connnection to the transmitter
    */
    dr = WireOpen( &hWire );

    if ( dr != DRM_SUCCESS )
    {
        TRACE(( "Receiver: Cannot open connection to transmitter. %lx\n", dr ));
        goto ErrorExit;
    }

    /*
    ** Loop processing messages
    */
    while ( pProximityContext->State != NDR_HaveLicenseState )
    {
        /*
        ** Free any buffer from the previous iteration
        */
        SAFE_OEM_FREE( pbMessageReceived );
        SAFE_OEM_FREE( pbMessageToSend );

        /*
        ** Build a message to send based on the current state
        ** Initially, send a registration message
        */
        switch ( pProximityContext->State )
        {
        case NDR_InitialState:
            dr = Drm_Ndr_Registration_GenerateChallenge( NULL, 
                                                         &pbMessageToSend, 
                                                         &cbMessageToSend );

            if ( dr != DRM_SUCCESS )
            {
                TRACE(( "Receiver: Cannot build device registration message. %lx\n", dr ));
                goto ErrorExit;
            }
            TRACE(( "Receiver: Built Registration message.\n" ));
            break;

        /*
        ** Handle the registration state
        */
        case NDR_RegisteredState:

            /*
            ** Wait before sending the next proximity start message
            */
            if( !fFirstProximityAttempt )
            {
                Sleep( WMDRMNET_PROXIMITY_DETECTION_INTERVAL );
            }

            /*
            ** Build the Proximity Start message
            */
            ChkDR( Drm_Ndr_Proximity_GenerateStart( NULL, 
                                                    pProximityContext, 
                                                    &pbMessageToSend, 
                                                    &cbMessageToSend ) );

            TRACE(( "Receiver: Built Proximity Start message.\n" ));

            fFirstProximityAttempt = FALSE;
            
            break;

        /*
        ** Handle the challenged state
        */
        case NDR_ChallengedState:

            /*
            ** Build the challenge response message
            */
            ChkDR( Drm_Ndr_Proximity_GenerateResponse( NULL, 
                                                       pProximityContext, 
                                                       &pbMessageToSend, 
                                                       &cbMessageToSend ) );

            TRACE(( "Receiver: Built Proximity Response message.\n" ));

            /* 
            ** Go back to the registered state 
            */
            pProximityContext->State = NDR_RegisteredState;
            break;

        /*
        ** Handle the proximity detected state
        */
        case NDR_ProximityDetectedState:

            /*
            ** Build the License request message
            */
            dr = Drm_Ndr_License_GenerateChallenge( NULL,
                                                    &pProximityContext->RightsId,
                                                    &pbMessageToSend,
                                                    &cbMessageToSend );

            if ( dr != DRM_SUCCESS )
            {
                TRACE(( "Receiver: Cannot build License request message. %lx\n", dr ));
                goto ErrorExit;
            }

            TRACE(( "Receiver: Built License request message.\n" ));

            break;

        default:
            TRACE(( "Receiver: Invalid State.\n" ));
            dr = DRM_E_FAIL;
            goto ErrorExit;
        }

        /*
        ** Send the message to the transmitter and get a response. 
        ** Proximity detection messages are sent to a specific address.
        */
        if( pProximityContext->State == NDR_RegisteredState )
        {
            dr = WireSendReceiveMessageTo( hWire,
                                           pProximityContext->Address,
                                           pProximityContext->AddressSize,
                                           pbMessageToSend,
                                           cbMessageToSend,
                                           &pbMessageReceived,
                                           &cbMessageReceived );

        } else
        {
            dr = WireSendReceiveMessage( hWire,
                                         pbMessageToSend,
                                         cbMessageToSend,
                                         &pbMessageReceived,
                                         &cbMessageReceived );
        }

        if ( dr != DRM_SUCCESS )
        {
            TRACE(( "Receiver: Cannot send/receive message to/from transmitter. %lx\n", dr ));
            goto ErrorExit;
        }

        /*
        ** Process the response we got back
        */
        bMessageType = Drm_Ndr_GetMessageType( pbMessageReceived, cbMessageReceived );
        TRACE(( "Receiver: Got response from transmitter. %ld\n", bMessageType ));

        switch ( bMessageType )
        {
        /*
        ** Process a registration response message
        */
        case WMDRMNET_REGISTRATION_RESPONSE_MESSAGE_TYPE:

            /*
            ** Message is only processed when in the initial state
            */
            DRMASSERT( pProximityContext->State == NDR_InitialState );
            if( pProximityContext->State != NDR_InitialState )
            {
                dr = DRM_E_FAIL;
                TRACE(( "Receiver: Cannot process the registration response message. Receiver not in correct state. %lx\n", dr ));
                goto ErrorExit;
            }

            dr = Drm_Ndr_Registration_ProcessResponse( NULL,
                                                       pProximityContext, 
                                                       pbMessageReceived, 
                                                       cbMessageReceived,
                                                       pCryptoCtx );

            if ( dr != DRM_SUCCESS )
            {
                TRACE(( "Receiver: Failed to process the registration response message. %lx\n", dr ));
                goto ErrorExit;
            }
            TRACE(( "Receiver: Processed RegistrationResponse message.\n" ));

            pProximityContext->State = NDR_RegisteredState;

            break;

        /*
        ** Process a Proximity Challenge Message
        */
        case WMDRMNET_PROXIMITY_CHALLENGE_MESSAGE_TYPE:

            /*
            ** Message is only processed when in the registered state
            */
            DRMASSERT( pProximityContext->State == NDR_RegisteredState );
            if( pProximityContext->State != NDR_RegisteredState )
            {
                dr = DRM_E_FAIL;
                TRACE(( "Receiver: Cannot process the proximity challenge message. Receiver not in correct state. %lx\n", dr ));
                goto ErrorExit;
            }

            /*
            ** Process the Proximity Challenge message
            */
            ChkDR( Drm_Ndr_Proximity_ProcessChallenge( NULL, 
                                                       pProximityContext, 
                                                       pbMessageReceived, 
                                                       cbMessageReceived ) );

            TRACE(( "Receiver: Processed proximity challenge message.\n" ));
            pProximityContext->State = NDR_ChallengedState;
            break;

        /*
        ** Process a Proximity Result Message
        */
        case WMDRMNET_PROXIMITY_RESULT_MESSAGE_TYPE:

            /*
            ** Message is only processed when in the registered state
            */
            DRMASSERT( pProximityContext->State == NDR_RegisteredState );
            if( pProximityContext->State != NDR_RegisteredState )
            {
                dr = DRM_E_FAIL;
                TRACE(( "Receiver: Cannot process the proximity result message. Receiver not in correct state. %lx\n", dr ));
                goto ErrorExit;
            }

            /*
            ** Process the Proximity Result message
            */
            ChkDR( Drm_Ndr_Proximity_ProcessResult( NULL, 
                                                    pProximityContext, 
                                                    pbMessageReceived, 
                                                    cbMessageReceived, 
                                                    &wResult ) );
            
            /*
            ** Handle a failure result
            **
            ** A real receiver should handle the various results differently
            */
            if ( wResult != 0 )
            {
                TRACE(( "Receiver: Proximity challenge result says failure %ld.\n", wResult ));
                /* The result may have been garbled on the wire.  Just continue. */
                break;
            }

            pProximityContext->State = NDR_ProximityDetectedState;
            TRACE(( "Receiver: Processed proximity result message.\n" ));
            break;

        /*
        ** Process a License Response Message
        */
        case WMDRMNET_LICENSE_RESPONSE_MESSAGE_TYPE:
        {
            DRM_STACK_ALLOCATOR_CONTEXT    Stack = {0};

            /*
            ** Message is only processed when in the ProximityDetectedState state
            */
            DRMASSERT( pProximityContext->State == NDR_ProximityDetectedState );
            if( pProximityContext->State != NDR_ProximityDetectedState )
            {
                dr = DRM_E_FAIL;
                TRACE(( "Receiver: Cannot process the License Response message. Receiver not in correct state. %lx\n", dr ));
                goto ErrorExit;
            }

            /*
            ** Process a License Response message
            */
            _AllocateStack( &Stack );
            dr = Drm_Ndr_License_ProcessResponse( NULL,
                                                  pProximityContext, 
                                                  pbMessageReceived, 
                                                  cbMessageReceived, 
                                                  &Stack );

            if ( dr != DRM_SUCCESS )
            {
                TRACE(( "Receiver: Cannot process the License response message. %lx\n", dr ));
                break;
            }

            TRACE(( "Receiver: Processed License Response message.\n" ));

            pProximityContext->State = NDR_HaveLicenseState;

            break;
        }
        /*
        ** Ignore messages we don't expect
        */
        default:
            TRACE(( "Receiver: Invalid message type received. %ld\n", bMessageType ));
            break;
        }
    }

    dr = DRM_SUCCESS;

ErrorExit:
    if ( hWire != NULL)
    {
        WireClose( hWire );
    }
    
    SAFE_OEM_FREE( pbMessageReceived );
    SAFE_OEM_FREE( pbMessageToSend );
    SAFE_OEM_FREE( pCryptoCtx );
        
    if ( pProximityContext != NULL )
    {
        _DeleteProximityContext( pProximityContext );
    }

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

