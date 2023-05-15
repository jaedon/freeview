/*
    COPYRIGHT VIACCESS S.A.
    date        : 30/06/2006
    file        : va_opentv_Otype.h
    Description : It is the Viaccess'API of CAS functionalities
*/

#ifndef _VA_OPENTV_OTYPE_H_
#define _VA_OPENTV_OTYPE_H_

#include "va_types.h"
#include "va_setup.h"
#include "va_ui.h"

/* Action to apply on message type */
typedef enum 
{
    eO_VA_UI_ADD_ACTION,     /* Register for messages     */
    eO_VA_UI_REMOVE_ACTION,  /* Unregister for messages   */
    eO_VA_UI_NEXT_ACTION     /* Only used to know the end */

}   tO_VA_UI_Action;

/* Categories of events the application may receive */
#define kO_VA_UI_SUB_EVENT      (WORD) (0x00E0) /* Register for subscription messages      */
#define kO_VA_UI_CARD_EVENT     (WORD) (0x0100) /* Register for smartcard messages         */
#define kO_VA_UI_SYSTEM_EVENT   (WORD) (0x0200) /* Register for Viaccess sytem messages    */
#define kO_VA_UI_PVR_EVENT      (WORD) (0x0400) /* Register for PVR messages               */
#define kO_VA_UI_NETWORK_EVENT  (WORD) (0x0800) /* Register for IP-Network messages        */
#define kO_VA_UI_TERMINAL_EVENT (WORD) (0x1000) /* Register for Cardless Terminal messages */

#define kO_VA_UI_ALL_EVENT  (WORD) ( kO_VA_UI_SUB_EVENT     | kO_VA_UI_CARD_EVENT |      \
                                     kO_VA_UI_SYSTEM_EVENT  | kO_VA_UI_PVR_EVENT  |      \
                                     kO_VA_UI_NETWORK_EVENT | kO_VA_UI_TERMINAL_EVENT )


/* The request structure*/
typedef struct 
{
    tVA_UI_Request * pRequest;
    DWORD            dwReserved;

}   tO_VA_UI_RequestMessage;

/*the event structure*/
typedef struct 
{
    BYTE    byScSlot;
    BYTE    byPipeId;
    BYTE    byScrambledType;
    UINT8   uiStreamsNumber;
    DWORD   dwStbStreamHandle;

}   tO_VA_UI_EventMessage;

typedef struct 
{
    UINT16  uiClass;
    UINT16  uiMessageType;
    union 
    {
        tO_VA_UI_RequestMessage request;
        tO_VA_UI_EventMessage   event;

    }   info;

}   tO_VA_UI_OpenTvMessage;


/* OpenTv Return codes */
#define kO_VA_OK                           kVA_OK

#define kO_VA_ILLEGAL_HANDLE               kVA_ILLEGAL_HANDLE
#define kO_VA_ERROR                        kVA_ERROR
#define kO_VA_INVALID_PARAMETER            kVA_INVALID_PARAMETER
#define kO_VA_ALREADY_DONE                 kVA_ALREADY_DONE
#define kO_VA_RESOURCE_BUSY                kVA_RESOURCE_BUSY

#define kO_VA_NOT_IMPLEMENTED              kVA_NOT_IMPLEMENTED

#define kO_VA_NOT_USED (BYTE) (0xFF) /* when the paramater is not used */
#define kO_VA_INVALID_PIPE_ID			   kVA_SETUP_NBMAX_ACS
#define kO_VA_INVALID_PIPE_TAG			   0

/* Accessor macros */
#define O_VA_UI_GetTheMessageType(_msg)    (((tO_VA_UI_OpenTvMessage*)(_msg))->uiMessageType)

#define O_VA_UI_GetTheRequestPtr(_msg)     (((tO_VA_UI_OpenTvMessage*)(_msg))->info.request.pRequest)

#define O_VA_UI_GetTheSlotId(_msg)         (((tO_VA_UI_OpenTvMessage*)(_msg))->info.event.byScSlot)
#define O_VA_UI_GetThePipeId(_msg)         (((tO_VA_UI_OpenTvMessage*)(_msg))->info.event.byPipeId)
#define O_VA_UI_GetTheScrambledType(_msg)  (((tO_VA_UI_OpenTvMessage*)(_msg))->info.event.byScrambledType)
#define O_VA_UI_GetTheStreamHandle(_msg)   (((tO_VA_UI_OpenTvMessage*)(_msg))->info.event.dwStbStreamHandle)


#endif /* _VA_OPENTV_OTYPE_H_ */
