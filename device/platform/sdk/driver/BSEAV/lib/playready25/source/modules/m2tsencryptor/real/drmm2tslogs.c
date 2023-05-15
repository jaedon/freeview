/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMM2TSLOGS_C
#include <drmm2tslogs.h>
#include <drmm2tsmacros.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
**
** Function:    DRM_M2TS_TranslateError
**
** Synopsis:    Translate the error into either DRM_E_M2TS_DROP_PACKET
**              or DRM_E_M2TS_DROP_PES if the result indicates a packet
**              or PES should be dropped.
**
** Arguments:
**
** [f_drResult]             -- The result to be translated
**
** Returns:                 DRM_E_M2TS_DROP_PACKET if the result is one of the
**                              results listed for DRM_E_M2TS_DROP_PACKET.
**                          DRM_E_M2TS_DROP_PES  if the result is one of the
**                              results listed for DRM_E_M2TS_DROP_PES.
**                          The original result if it is not listed for 
**                              DRM_E_M2TS_DROP_PACKET and DRM_E_M2TS_DROP_PES.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_TranslateError(
    __in    const DRM_RESULT    f_drResult )
{
    /* 
    ** Below errors are for internal state and logging purpose onlye.  
    ** None of the functions should return these error to their caller. 
    */
    DRMASSERT( f_drResult != DRM_E_M2TS_INCOMPLETE_SECTION_HEADER );
    DRMASSERT( f_drResult != DRM_E_M2TS_PES_PACKET_LENGTH_NOT_SPECIFIED );
    DRMASSERT( f_drResult != DRM_E_M2TS_DROP_PES );
    DRMASSERT( f_drResult != DRM_E_M2TS_DROP_PACKET );
    DRMASSERT( f_drResult != DRM_E_M2TS_STREAM_OR_PACKET_TYPE_CHANGED );

    if( f_drResult == DRM_E_M2TS_TABLE_ID_INVALID
     || f_drResult == DRM_E_M2TS_PACKET_SYNC_BYTE_INVALID
     || f_drResult == DRM_E_M2TS_ADAPTATION_LENGTH_INVALID
     || f_drResult == DRM_E_M2TS_PAT_HEADER_INVALID
     || f_drResult == DRM_E_M2TS_PMT_HEADER_INVALID
     || f_drResult == DRM_E_M2TS_PES_HEADER_INVALID
     || f_drResult == DRM_E_M2TS_CA_DESCRIPTOR_LENGTH_INVALID
     || f_drResult == DRM_E_M2TS_CRC_FIELD_INVALID
     || f_drResult == DRM_E_M2TS_UNKNOWN_PACKET
     || f_drResult == DRM_E_M2TS_PTS_NOT_EXIST 
     || f_drResult == DRM_E_M2TS_PES_START_CODE_NOT_FOUND
     || f_drResult == DRM_E_M2TS_NOT_UNIT_START_PACKET
     || f_drResult == DRM_E_M2TS_PROGRAM_INFO_LENGTH_INVALID
     || f_drResult == DRM_E_M2TS_SECTION_LENGTH_INVALID
     || f_drResult == DRM_E_M2TS_PAT_PID_IS_NOT_ZERO )
    {
        return DRM_E_M2TS_DROP_PACKET;
    }
    else if( f_drResult == DRM_E_M2TS_DDPLUS_FORMAT_INVALID
          || f_drResult == DRM_E_M2TS_ADTS_FORMAT_INVALID
          || f_drResult == DRM_E_M2TS_MPEGA_FORMAT_INVALID
          || f_drResult == DRM_E_M2TS_TOO_MANY_SUBSAMPLES
          || f_drResult == DRM_E_M2TS_INCOMPLETE_PES )
    {
        return DRM_E_M2TS_DROP_PES;
    }
    else
    {
        return f_drResult;
    }
}

/**********************************************************************
**
** Function:    DRM_M2TS_LogEventWithPacket
**
** Synopsis:    Log an event about a packet
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_eCategory]            -- The category of the log.
** [f_fDropped]             -- Flag to indicate whether the packet is dropped.
** [f_drResult]             -- The result that triggered the log.
** [f_pPacket]              -- The packet that triggered the log.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_LogEventWithPacket(
    __in      const DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in      const DRM_M2TS_LOG_CATEGORY        f_eCategory,
    __in      const DRM_BOOL                     f_fDropped,
    __in      const DRM_RESULT                   f_drResult,
    __in      const DRM_M2TS_PACKET             *f_pPacket )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pEncContext != NULL );

    if( f_pEncContext->pfnLoggingCallback != NULL )
    {
        DRM_M2TS_LOG oLog;

        ChkArg( f_pPacket != NULL );
        ChkArg( f_eCategory == eDRM_M2TS_LOG_CATEGORY_ERROR
             || f_eCategory == eDRM_M2TS_LOG_CATEGORY_WARNING
             || f_eCategory == eDRM_M2TS_LOG_CATEGORY_INFORMATION );

        MEMSET( &oLog, 0, SIZEOF( DRM_M2TS_LOG ) );

        oLog.eCategory                      = f_eCategory;
        oLog.drResult                       = f_drResult;
        oLog.eDetailsType                   = eDRM_M2TS_LOG_DETAILS_TYPE_PACKET;
        oLog.Details.Packet.fDropped        = f_fDropped;
        oLog.Details.Packet.wPID            = DRM_M2TS_GET_PID( f_pPacket->rgbData );
        oLog.Details.Packet.dwPacketNumber  = f_pPacket->dwPacketNumber;
        MEMCPY( oLog.Details.Packet.rgbPacketData, f_pPacket->rgbData, DRM_M2TS_PACKET_SIZE );

        f_pEncContext->pfnLoggingCallback( f_pEncContext->pLoggingCallbackContext, &oLog );
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_LogEventWithPES
**
** Synopsis:    Log an event about a PES packet
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_eCategory]            -- The category of the log.
** [f_fDropped]             -- Flag to indicate whether the PES packet is dropped.
** [f_drResult]             -- The result that triggered the log.
** [f_pPES]                 -- The PES packet that triggered the log.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_LogEventWithPES(
    __in      const DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in      const DRM_M2TS_LOG_CATEGORY        f_eCategory,
    __in      const DRM_BOOL                     f_fDropped,
    __in      const DRM_RESULT                   f_drResult,
    __in      const DRM_M2TS_PES                *f_pPES )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pEncContext != NULL );

    if( f_pEncContext->pfnLoggingCallback != NULL )
    {
        DRM_M2TS_LOG oLog;

        ChkArg( f_pPES != NULL );
        ChkArg( f_eCategory == eDRM_M2TS_LOG_CATEGORY_ERROR
             || f_eCategory == eDRM_M2TS_LOG_CATEGORY_WARNING
             || f_eCategory == eDRM_M2TS_LOG_CATEGORY_INFORMATION );

        MEMSET( &oLog, 0, SIZEOF( DRM_M2TS_LOG ) );

        oLog.eCategory                          = f_eCategory;
        oLog.drResult                           = f_drResult;
        oLog.eDetailsType                       = eDRM_M2TS_LOG_DETAILS_TYPE_PES;
        oLog.Details.PES.fDropped               = f_fDropped;
        oLog.Details.PES.wPID                   = f_pPES->wPID;
        oLog.Details.PES.qwPTS                  = f_pPES->qwPTS;
        oLog.Details.PES.bDataAligned           = f_pPES->bDataAlignmentFlag;
        oLog.Details.PES.bStreamType            = f_pPES->bStreamType;
        oLog.Details.PES.cPackets               = f_pPES->oPacketList.cItems;
        oLog.Details.PES.dwFirstPacketNumber    = f_pPES->oPacketList.pHead != NULL ? f_pPES->oPacketList.pHead->dwPacketNumber : 0;
        oLog.Details.PES.dwLastPacketNumber     = f_pPES->oPacketList.pTail != NULL ? f_pPES->oPacketList.pTail->dwPacketNumber : 0;

        f_pEncContext->pfnLoggingCallback( f_pEncContext->pLoggingCallbackContext, &oLog );
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_LogEventWithKeyRotation
**
** Synopsis:    Log an event about key rotation
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_qwPTS]                -- The PTS of the last PES that triggered key rotation.
** [f_qwAggregatedDuration] -- The aggregated duration since beginning or last key
**                             rotation.
** [f_qwCurrentKeyDuration] -- The duration for the last key data set by the caller
**                             via Drm_M2ts_SetData.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_LogEventWithKeyRotation(
    __in      const DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in      const DRM_UINT64                   f_qwPTS,
    __in      const DRM_UINT64                   f_qwAggregatedDuration,
    __in      const DRM_UINT64                   f_qwCurrentKeyDuration )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pEncContext != NULL );

    if( f_pEncContext->pfnLoggingCallback != NULL )
    {
        DRM_M2TS_LOG oLog;

        MEMSET( &oLog, 0, SIZEOF( DRM_M2TS_LOG ) );

        oLog.eCategory                                  = eDRM_M2TS_LOG_CATEGORY_INFORMATION;
        oLog.drResult                                   = DRM_E_M2TS_NEED_KEY_DATA;
        oLog.eDetailsType                               = eDRM_M2TS_LOG_DETAILS_TYPE_KEY_ROTATION;
        oLog.Details.KeyRotation.qwPTS                  = f_qwPTS;
        oLog.Details.KeyRotation.qwAggregatedDuration   = f_qwAggregatedDuration;
        oLog.Details.KeyRotation.qwCurrentKeyDuration   = f_qwCurrentKeyDuration;

        f_pEncContext->pfnLoggingCallback( f_pEncContext->pLoggingCallbackContext, &oLog );
    }

ErrorExit:
    return dr;

}

EXIT_PK_NAMESPACE_CODE;
