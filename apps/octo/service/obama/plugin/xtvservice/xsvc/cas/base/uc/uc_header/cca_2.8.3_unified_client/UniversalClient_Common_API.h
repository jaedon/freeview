/*
 * Copyright 2009-2012 Irdeto B.V.
 *
 * This file and the information contained herein are the subject of copyright
 * and intellectual property rights under international convention. All rights
 * reserved. No part of this file may be reproduced, stored in a retrieval
 * system or transmitted in any form by any means, electronic, mechanical or
 * optical, in whole or in part, without the prior written permission of Irdeto
 * B.V.
 */

 /**
 * @file UniversalClient_Common_API.h
 * Cloaked CA Agent Common API. This file contains the API information to Start/Stop CA 
 * client and also operation APIs on a service. 
 */
#ifndef UNIVERSALCLIENT_COMMON_API_H__INCLUDED__
#define UNIVERSALCLIENT_COMMON_API_H__INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#include "UniversalClient_API.h"
#include "UniversalClient_SPI.h"

/** 
 * monitor message length.
 *
 * Only used when Unified Client is supported.
 */
#define UC_MONITOR_MSG_LENGTH 255


/** @defgroup universalclient_api Cloaked CA Agent APIs

 *  Cloaked CA Agent APIs
 *
 *  These API methods provide the necessary interfaces for the Application in client device.
 *
 *  @{
 */

/** @defgroup commonapi Cloaked CA Agent Common APIs
 * APIs related to service management and client management functionality
 *
 * These API methods provide the necessary functionality for managing ECM and EMM services.
 * 
 *  @{
 */


/**
 * Handle to a service object
 *
 * A service object is a logical grouping of EMM or ECM streams that are common
 * to the CA technology.
 * 
 * A service handle is opened when ::UniversalClient_OpenService
 * is called. A service handle must be closed by calling ::UniversalClient_CloseService.
 */
typedef uc_uintptr uc_service_handle;

/**
 * service message type enumeration.
 *
 * This type is used by the ::uc_service_message_proc callback provided by an application to
 * ::UniversalClient_OpenService. This indicates the type of 
 * message being sent to the application. The application may choose to ignore messages, or 
 * it may perform additional processing based on the type of the message.
 */
typedef enum _uc_service_message_type
{
    /** (0x00000000) ECM status message type for the ::uc_service_message_proc callback
     *
     * ECM status messages are provided to the callback when an ECM has arrived from 
     * one of the streams associated with a ECM service. ECM status messages provide 
     * information about the CA stream that the ECM arrived on, the elementary stream 
     * information, such as audio and video components, that are affected by the ECM,
     * and the status of the ECM processing. 
     *
     * For messages with this type, the \a lpVoid argument is a pointer to 
     * a structure of type ::uc_ecm_status_st. 
     *
     */
    UC_ECM_STATUS = 0x00000000,

    /** (0x00000001) EMM status message type for the ::uc_service_message_proc callback
     *
     * EMM status messages are provided to the callback when an EMM has arrived from 
     * the stream associated with a EMM service. EMM status messages provide information 
     * about the CA stream  that the EMM arrived on, and the status of the ECM processing. 
     *
     * For messages with this type, the \a lpVoid argument is a pointer to 
     * a structure of type ::uc_emm_status_st. 
     *
     */
    UC_EMM_STATUS = 0x00000001,

    /** (0x00000002) Service status message type for the ::uc_service_message_proc callback
     *
     * Service status messages are provided to the callback when the service status has changed.
     * Service status messages provide information about the status of the service being processed.
     *
     * For messages with this type, the \a lpVoid argument is a pointer to 
     * a structure of type ::uc_service_status_st. 
     *
     */
    UC_SERVICE_STATUS = 0x00000002,

    
    /**
     * (0x00000003) Status in reply to a PVR record request.
     *
     * This message is used to notify the status in reply to a PVR record request 
     * via ::UniversalClient_ConfigService or ::UniversalClient_ConfigConnection.
     *
     * For messages with this type, the \a lpVoid argument points to a ::uc_pvr_service_status_st
     * structure. The ::uc_pvr_service_status_st::pMessage points to a buffer containing a 
     * PVR_Record_Request_Reply_Message.
     *
     * The format of the PVR_Record_Request_Reply_Message is as follows
     *
     \code
      PVR_Record_Request_Reply_Message()
      {
          status_message                          string
      }
     \endcode

        Explanation of the fields:
        - \b status_message: See \ref status_messages "Status Messages" for a list of 
             possible values and what they mean.
     *
     */
    UC_SERVICE_PVR_RECORD_STATUS_REPLY = 0x00000003,

    /**
     * (0x00000004) Status in reply to a PVR playback request.
     * 
     * This message is used to notify the status in reply to a PVR playback request 
     * via ::UniversalClient_SubmitPVRSessionMetadata.
     * 
     * For messages with this type, the \a lpVoid argument points to a ::uc_pvr_service_status_st
     * structure. The ::uc_pvr_service_status_st::pMessage points to a buffer containing 
     * a PVR_Playback_Request_Reply_Message.
     *
     * The format of the PVR_Record_Request_Reply_Message is as follows
     *
     \code
      PVR_Playback_Request_Reply_Message()
      {
          status_message                          string
      }
     \endcode

        Explanation of the fields:
        - \b status_message: See \ref status_messages "Status Messages" for a list of 
             possible values and what they mean.
     *
     */
    UC_SERVICE_PVR_PLAYBACK_STATUS_REPLY = 0x00000004,

    /**
     * (0x00000005) Message to send the PVR Session Metadata in reply to a PVR record request.
     *
     * This message is used to notify the PVR Session Metadata in reply to a PVR record request 
     * via ::UniversalClient_ConfigService or ::UniversalClient_ConfigConnection.
     *
     * For messages with this type, the \a lpVoid argument points to a ::uc_pvr_service_status_st
     * structure. The uc_pvr_service_status_st::pMessage points to a ::uc_buffer_st structure. 
     * The content in it is a PVR_Session_Metadata_Reply_Message. The application should save the 
     * PVR Session Metadata associated with the recorded content. At playback, the PVR Session 
     * Metadata should be submitted to Cloaked CA Agent via ::UniversalClient_SubmitPVRSessionMetadata.
     *
     * The format of the PVR_Session_Metadata_Reply_Message is as follows
     *
     \code
      PVR_Session_Metadata_Reply_Message()
      {
          Field Name               Field Length(bytes)          Field Description

          Reserved                 28                           Reserved data
          TLV section length       2                            N, length of all TLVs (Network order)               
          TLV section              N                            Some TLVs
          Reserved                 X                            Reserved data
      }
     \endcode

     In TLV section, there are a serial of TLVs, the format of a TLV is:
     \code
      PVR_TLV_Data()
      {
         Field Name                Field Length(bytes)          Field Description

         Tag                       1                            Type of the TLV
         Length                    2                            L, Length of the TLV payload(network order)
         Payload                   L                            The TLV data. 
      }
     \endcode

     
     The format of Copy Control information TLV is:
     \code
     Copy_Control_Information_TLV()
     {
        Field Name                 Field Length(bytes)          Field Value                  

        Tag                        1                            0x04                         
        Length                     2                            0x00,0x01
        Payload                    1                            Copy Control information.
     }
     In the payload, the Analog CCI bits are carried in bit3 and bit2, the Digital CCI bits are
     carried in bit1 and bit0 (The most significant bit is bit7):
     bit7   bit6   bit5   bit4   bit3   bit2   bit1   bit0
                                 |<--C1--->|   |<---C2--->| 
     Let the value of the paylod be V, the Analog CCI C1 is ((V & 0xC)>>2), the Digital CCI C2 is (V & 0x03).
     \endcode
     

     If content expiry is enabled, there maybe an Expiry Time TLV. It specifies when the recorded 
     content will be expired. If there is no such TLV, the content will never be expired. The format of 
     the Expiry Time TLV is:
     \code
     Expiry_Time_TLV()
     {
        Field Name                 Field Length(bytes)          Field Value                  

        Tag                        1                            0x06                         
        Length                     2                            0x00,0x04
        Payload                    4                            The expiry time of the content, 
                                                                as seconds from 1 January 2000 00:00.                            
     }
     \endcode

     \note The application should use the expiry time to perform expiry check on recorded content. 
     The whole PVR Metadata should never be changed.


     The format of Non PVR enable Flag TLV is:
     \code
     Non_PVR_Enable_Flag_TLV()
     {
        Field Name                 Field Length(bytes)          Field Value                  

        Tag                        1                            0x11                         
        Length                     2                            0x00,0x01
        Payload                    1                            Non PVR enable Flag                             
     }
     \endcode
     
     *
     */
    UC_SERVICE_PVR_SESSION_METADATA_REPLY = 0x00000005,

    /** (0x00000006) ECM monitor status message type for the ::uc_service_message_proc callback
     *
     * This message is used to notify an ECM monitor status string when an ECM has been processed.
     * This message is activated only after the Application has configured a TLV item ::UC_TLV_TAG_FOR_MONITOR_SWITCH to enable this message.
     * By default, the message will not be sent to the Application in order to reduce system cost.
     * 
     *
     * For messages with this type, the \a lpVoid argument is a pointer to 
     * a structure of type ::uc_service_monitor_status_st. 
     *
     */
    UC_SERVICE_ECM_MONITOR_STATUS = 0x00000006,

    /** (0x00000007) EMM monitor status message type for the ::uc_service_message_proc callback
     *
     * This message is used to notify an EMM monitor status string when an EMM has been processed.
     * This message is activated only after the Application has configured a TLV item ::UC_TLV_TAG_FOR_MONITOR_SWITCH to enable this message.
     * By default, the message will not be sent to the Application in order to reduce system cost.
     * 
     *
     * For messages with this type, the \a lpVoid argument is a pointer to 
     * a structure of type ::uc_service_monitor_status_st. 
     *
     */
    UC_SERVICE_EMM_MONITOR_STATUS = 0x00000007,

    /** (0x00000008) ECM monitor status message type for the ::uc_service_message_proc callback
     * 
     * When Unified Client is integrated, we use this message to notify the ECM monitor status to the application 
     * instead of ::UC_SERVICE_ECM_MONITOR_STATUS message.
     *
     * This message is used to notify an ECM monitor status string when an ECM has been processed.
     * This message is activated only after the application has configured a TLV item ::UC_TLV_TAG_FOR_MONITOR_SWITCH to enable this message.
     * By default, the message will not be sent to the application in order to reduce system cost.
     * 
     *
     * For messages with this type, the \a lpVoid argument is a pointer to 
     * a structure of type ::uc_convergent_client_service_monitor_status_st. 
     *
     */
    UC_SERVICE_ECM_MONITOR_STATUS_FOR_CONVERGENT_CLIENT = 0x00000008,

    /** (0x00000009) EMM monitor status message type for the ::uc_service_message_proc callback
     *
     * When Unified Client is integrated, we use this message to notify the EMM monitor status to the application 
     * instead of ::UC_SERVICE_EMM_MONITOR_STATUS message.
     *
     * This message is used to notify an EMM monitor status string when an EMM has been processed.
     * This message is activated only after the Application has configured a TLV item ::UC_TLV_TAG_FOR_MONITOR_SWITCH to enable this message.
     * By default, the message will not be sent to the Application in order to reduce system cost.
     * 
     *
     * For messages with this type, the \a lpVoid argument is a pointer to 
     * a structure of type ::uc_convergent_client_service_monitor_status_st. 
     *
     */
    UC_SERVICE_EMM_MONITOR_STATUS_FOR_CONVERGENT_CLIENT = 0x00000009,

    /** (0x0000000a) Service status message type for the ::uc_service_message_proc callback
     *
     * When Unified Client is integrated, this message is used to notify service status to the application 
     * instead of ::UC_SERVICE_STATUS message.
     *
     * Service status messages are provided to the callback when the service status is changed.
     * Service status messages provide information about the status of the service being processed.
     *
     * For messages with this type, the \a lpVoid argument is a pointer to 
     * a structure of type ::uc_convergent_client_service_status_st. 
     *
     */
    UC_SERVICE_STATUS_FOR_CONVERGENT_CLIENT = 0x0000000a,

    /** (0x0000000b) SoftCell preview service message type for the ::uc_service_message_proc callback
     *
     * When Unified Client is integrated, this message is used to notify SoftCell preview service message.
     *
     * Service information messages are provided to the callback when the Softcell Preview information is changed.
     *
     * For messages with this type, the \a lpVoid argument is a pointer to 
     * a structure of type ::uc_service_info_for_softcell_preview_st. 
     *
     */
    UC_SERVICE_INFO_FOR_SOFTCELL_PREVIEW = 0x0000000b

} uc_service_message_type;


/**
 * Indicate the type of stream.
 *
 * This type is used in ::uc_ca_stream and ::uc_component_stream to 
 * determine what type of stream implementation should be used.
 */
typedef enum _uc_stream_type
{
    /**
     * DVB Stream Type
     *
     * DVB uses PIDs (Packet IDs) to uniquely identify a stream.
     */
    UC_STREAM_DVB = 0
} uc_stream_type;



/**
 * ECM/EMM stream definition structure.
 *
 * This structure is used as part of ::uc_stream_open_params, ::uc_ecm_status_st,
 * and ::uc_emm_status_st structures to uniquely identify a stream carrying 
 * CA messages. 
 */
typedef struct _uc_ca_stream
{
    /**
     * Indicates the type of stream.
     *
     * This determines what other members are the structure are valid.
     */
    uc_stream_type protocolType;

    /**
     * \a protocolType == UC_STREAM_DVB: The PID of the stream carrying ECMs or EMMs.
     *
     * This is the PID (Packet ID) of the ECM or EMM stream identified by this
     * structure.
     *
     * \note This is \b not the same as the PID of the elementary stream defined
     *     in uc_component_stream::pid. 
     */
    uc_uint16 pid;
} uc_ca_stream;

/**
 * Elementary stream definition structure.
 *
 * To save component stream information.
 * CA client uses it to identify a component stream, i.e. audio,video,data.
 * This structure can be extended to support other clients.
 */
typedef struct _uc_component_stream
{
    /**
     * Indicates the type of stream.
     *
     * This determines what other members are the structure are valid.
     */
    uc_stream_type protocolType;

    /**
     * \a protocolType == UC_STREAM_DVB: The PID of the stream carrying audio or video.
     *
     * This is the PID (Packet ID) of the audio or video
     * data that is to be descrambled by descrambling keys sent to the stream 
     * implementation via ::UniversalClientSPI_Stream_SetDescramblingKey. 
     *
     * \note This is \b not the same as the PID of the ECM/EMM stream defined
     *     in uc_ca_stream::pid. 
     */
    uc_uint16 pid;
} uc_component_stream;


/**
 * ECM Status structure
 *
 * This is the structure that is passed as the \a lpVoid argument of 
 * ::uc_service_message_proc when the \a message argument is ::UC_ECM_STATUS.
 */
typedef struct _uc_ecm_status_st
{
    /**
     * The service handle for this ECM
     *
     * This handle is identical to the service handle previously opened by a call to ::UniversalClient_OpenService. 
     * But in Unified Client, this handle is used internally and can be ignored by the application.
     */
    uc_service_handle serviceHandle;

    /**
     * Status message for this ECM
     *
     * This is the message that indicates the status of the processing
     * of the ECM.
     *
     * See \ref status_messages "Status Messages" for a list of 
     * possible values and what they mean.
     */
    uc_string statusMessage;

    /**
     * CA stream information.
     *
     * This matches the CA stream information that was passed to 
     * ::UniversalClientSPI_Stream_Open in uc_stream_open_params::caStream.
     */
    uc_ca_stream caStream; 
} uc_ecm_status_st;

/**
 * Stream open parameters for UniversalClientSPI_Stream_Open
 *
 * This is the structure that is passed to ::UniversalClientSPI_Stream_Open to create each 
 * stream instance when ::UniversalClient_DVB_NotifyPMT or ::UniversalClient_DVB_NotifyCAT
 * is called.
 */
typedef struct _uc_stream_open_params
{
    /**
     * The original \a pServiceContext value passed to ::UniversalClient_OpenService.
     * This can be used to tie application instance data to the instance of the stream. 
     */
    void *pServiceContext;

    /**
     * CA stream information.
     *
     * The information in this structure is used to identify the source of incoming
     * CA messages, such as ECMs or EMMs. This structure is also 
     * passed back to the ::uc_service_message_proc callback function when events
     * occur related to the stream information. 
     * See the uc_ecm_status_st::caStream and uc_emm_status_st::caStream members. 
     */
    uc_ca_stream caStream; 
} uc_stream_open_params;

/**
 * Elementary Stream Component structure
 *
 * This is the structure that is passed to ::UniversalClientSPI_Stream_AddComponent and ::UniversalClientSPI_Stream_RemoveComponent.
 */
typedef struct _uc_elementary_component
{
    /**
     * Base structure
     *
     * This is the base structure that a component 'inherits' from.
     * This is used internally and can be ignored by the implementation.
     */
    uc_component base;

    /**
     * Elementary stream of the component
     *
     * Elementary stream associated with this component. This is the PID of the audio or video
     * data that is being descrambled by descrambling keys sent to the stream implementation via
     * ::UniversalClientSPI_Stream_SetDescramblingKey. 
     */
    uc_component_stream componentStream;
} uc_elementary_component;

/**
 * EMM Status structure
 *
 * This is the structure that is passed as the \a lpVoid argument of 
 * ::uc_service_message_proc when the \a message argument is ::UC_EMM_STATUS.
 */
typedef struct _uc_emm_status_st
{
    /**
     * The service handle for this EMM
     *
     * This handle is identical to the service handle previously opened by a call to ::UniversalClient_OpenService. 
     * But in Unified Client, this handle is used internally and can be ignored by the application.
     */
    uc_service_handle serviceHandle;

    /**
     * Status message for this EMM
     *
     * This is the message that indicates the status of the processing
     * of the EMM.
     *
     * See \ref status_messages "Status Messages" for a list of 
     * possible values and what they mean.
     */
    uc_string statusMessage;

    /**
     * CA stream information.
     */
    uc_ca_stream caStream; 
} uc_emm_status_st;


/**
 * Service Status structure
 *
 * This is the structure that is passed as the \a lpVoid argument of 
 * ::uc_service_message_proc when the \a message argument is ::UC_SERVICE_STATUS.
 */
typedef struct _uc_service_status_st
{
    /**
     * Status message for this service
     *
     * This is the message that indicates the status of the service being processed.
     *
     * See \ref status_messages "Status Messages" for a list of 
     * possible values.
     */
    uc_string statusMessage;

    /**
     * Service handle
     *
     * Service handle previously opened by a call to ::UniversalClient_OpenService.
     *
     */
    uc_service_handle serviceHandle;

    /**
     * Service context parameter
     *
     * Its value is from the application via the call to ::UniversalClient_OpenService.
     *
     */
    void *pServiceContext;

    /**
     * Non PVR enable flag
     *
     * When the current service is a descrambling service, this nonPVREnableFlag is the Non PVR enable flag.
     * For other kinds of service, this nonPVREnableFlag can be ignored.
     *
     */
    uc_byte nonPVREnableFlag;
    
} uc_service_status_st;

/**
 * Indicate CA client type.
 *
 * When Unified Client is integrated, both Cloaked CA Agent and Softcell are valid. This
 * type can be used to seperate these two CA clients.
 *
 */
typedef enum _uc_client_type
{
    /**
     * Softcell
     *
     */
    UC_SOFTCELL = 0,
        
    /**
     * Cloaked CA Agent
     *
     */
    UC_CLOAKEDCA = 1
}uc_client_type;

/**
 * service status information of one CA client(Cloaked CA Agent or Softcell)
 */
typedef struct _uc_client_service_status
{
    /**
     * Whether this status information is valid.
     * If this is set to be invalid, members in this structure shall be ignored.
     */
    uc_bool isValid;

    /**
     * Internal service handle, this handle is only locally valid inside SoftCell or Cloaked CA. 
     *
     * \note This handle is only used by Cloaked CA Agent and Softcell internally. Application shall only
     * display it in GUI dialogs without performing any service specific actions on it.
     */
    uc_service_handle internalServiceHandle;

    /**
     * Status message
     *
     * This is the message that indicates the status of the service being processed.
     *
     * See \ref status_messages "Status Messages" for a list of 
     * possible values.
     */
    uc_byte statusMessage[UC_STATUS_LENGTH];
}uc_client_service_status;

/**
 * Service Status structure for Unified Client
 *
 * This is the structure that is passed as the \a lpVoid argument of 
 * ::uc_service_message_proc when the \a message argument is ::UC_SERVICE_STATUS_FOR_CONVERGENT_CLIENT.
 * This struct is only used when Unified Client is integrated.
 *
 * Application shall check cloakedCAStatus.isValid and softcellStatus.isValid to determine which CA client is active
 * on the service.
 *
 * For EMM service, both cloakedCAStatus and softcellStatus in this struct may be valid.
 * For descrambling service, only one member (either cloakedCAStatus or softcellStatus) may be valid as only one CA
 * client is used for descrambling.
 */
typedef struct _uc_convergent_client_service_status_st
{
    /**
     * Service handle.
     *
     * Service handle previously opened by a call to ::UniversalClient_OpenService.
     *
     */
    uc_service_handle serviceHandle;

   /**
    * Status of Cloaked CA Agent.
    * Application shall check cloakedCAStatus.isValid before using it.
    */
    uc_client_service_status cloakedCAStatus;

   /**
    * Status of Softcell.
    * Application shall check softcellStatus.isValid before using it.    
    */
    uc_client_service_status softcellStatus;

}uc_convergent_client_service_status_st;

/**
 * Structure for Softcell preview feature.
 *
 * This is the structure that is passed as the \a lpVoid argument of 
 * ::uc_service_message_proc when the \a message argument is ::UC_SERVICE_INFO_FOR_SOFTCELL_PREVIEW.
 * This struct is only used when Unified Client is integrated.
 */
typedef struct _uc_service_info_for_softcell_preview_st
{
    /**
     * Service handle.
     *
     * Service handle previously opened by a call to ::UniversalClient_OpenService.
     *
     */
    uc_service_handle serviceHandle;

    /**
     * The time left of the preview package and it's defined as minutes left.
     */
    uc_byte leftTime;
}uc_service_info_for_softcell_preview_st;

/**
 * Indicate service type.
 *
 */
typedef enum _uc_service_type
{
    /**
     * Descrambling Service
     *
     */
    UC_SERVICE_DESCRAMBLE = 0,
    
    /**
     * EMM Service
     *
     */
    UC_SERVICE_EMM = 1
}uc_service_type;

/**
 * PVR service status structure
 *
 * This is the structure that is passed as the \a lpVoid argument of 
 * ::uc_service_message_proc when the \a message argument is ::UC_SERVICE_PVR_RECORD_STATUS_REPLY, 
 * ::UC_SERVICE_PVR_PLAYBACK_STATUS_REPLY, or ::UC_SERVICE_PVR_SESSION_METADATA_REPLY.
 */
typedef struct _uc_pvr_service_status_st
{
    /**
     * Service handle
     *
     * Service handle previously opened by a call to ::UniversalClient_OpenService.
     *
     */
    uc_service_handle serviceHandle;

    /**
     * The message
     *
     * The format of pMessage is different for different PVR service message type.
     * When \a message argument is ::UC_SERVICE_PVR_RECORD_STATUS_REPLY or ::UC_SERVICE_PVR_PLAYBACK_STATUS_REPLY, 
     * the format of pMessage is:
     \code
          PVR_Service_Status_Message()
          {
              status_message                          uc_string
          }
     \endcode
     * When \a message argument is ::UC_SERVICE_PVR_SESSION_METADATA_REPLY, the format of pMessage is:
     \code
          PVR_Session_Metadata()
          {
              pvr_metadata                          pointer to uc_buffer_st
          }
     \endcode
     *
     */
    void *pMessage;
} uc_pvr_service_status_st;

/**
 * Stream Status structure
 *
 * This is the structure used by ::UniversalClient_GetStreamStatus.
 */
typedef struct _uc_service_stream_status_st
{
    /**
     * Is ECM or not.
     */
    uc_bool isEcm;

    /**
     * Count of ECM or EMM sections that have been processed
     */
    uc_uint32 count;

    /**
     * Status message for this stream
     *
     * This is the message that indicates the status of the stream.
     */
    uc_byte streamStatusMessage[UC_CONNECTION_STATUS_STRING_SIZE];    

    /**
     * CA stream information.
     */
    uc_ca_stream caStream;

    /**
     * Whether the service can be recorded
     */
    uc_bool canRecord;

    /**
     * Count of component streams associated with this stream. It also identifies the number of 
     * uc_component_stream data structures in componentStreamArray.
     */
    uc_uint32 componentCount;

    /**
     * An array of component stream. The size of this array is componentCount*sizeof(uc_component_stream).
     * Application can access all of the component stream structures like this:
     * \code
     * uc_uint16 thePid = 0;
     * for( uc_uint32 i=0; i<componentCount; i++ )
     * {
     *     //Get the PID of the component stream.
     *     thePid = componentStreamArray[i].pid;
     * }
     * \endcode     
     * 
     */
    uc_component_stream *componentStreamArray;

} uc_service_stream_status_st;

/**
 * Service Monitor Status structure
 *
 * This is the structure that is passed as the \a lpVoid argument of 
 * ::uc_service_message_proc when the \a message argument is ::UC_SERVICE_ECM_MONITOR_STATUS
 * or ::UC_SERVICE_EMM_MONITOR_STATUS.
 */
typedef struct _uc_service_monitor_status_st
{
    /**
     * Service handle
     *
     * Service handle previously opened by a call to ::UniversalClient_OpenService.
     *
     */
    uc_service_handle serviceHandle;

    /**
     * The service monitor status string
     *
     * The service monitor status string may be an ECM service monitor status string or an EMM service monitor status string. 
     *
     * If the notify message is ::UC_SERVICE_ECM_MONITOR_STATUS, the string is an ECM service monitor status string.
     * Each ECM stream will be in the format below:\n
     *        "(%u) %04x %02x %02x" \n
     * The following is an example monitor status string with 3 ECM streams:\n
     *        (005) 0213 01 00 (005) 0214 01 00 (005) 0215 01 00
     * 
     *   \note
     *        ECM Count:         "%u"\n
     *        ECM PID:           "%04x"\n
     *        Page Number:       "%02x"\n
     *        Version Number:    "%02x"
     *
     * If the notify message is ::UC_SERVICE_EMM_MONITOR_STATUS, the string is an EMM service monitor status string.
     * Each EMM stream will be in the format below:\n
     *        "(%u) %04x %02x" \n
     * The following is an example monitor status string with 1 EMM stream:\n
     *        (001) 0500 03 
     * 
     *   \note
     *        EMM Count:         "%u"\n
     *        EMM PID:           "%04x"\n
     *        Address Control:   "%02x"
     */
    uc_string pMessage;
} uc_service_monitor_status_st;

/**
 * Monitor info of a CA client.
 *
 */
typedef struct _convergent_service_monitor_info
{
    /**
     * Whether this monitor info is valid.
     * This struct is only valid if valid is UC_TRUE.
     */
     uc_bool valid;

    /**
     * Internal service handle maintained by Cloaked CA Agent or SoftCell.
     * 
     * \note This handle is used by Cloaked CA Agent and Softcell internally. The application
     * shall not perform any service specific actions on it.
     */
     uc_service_handle internalServiceHandle;

    /**
     * Content of the monitor message string.
     * This message may come from Cloaked CA Agent or Softcell. The format is a bit different.\n 
     * If the monitor message is for Cloaked CA Agent, the format of this is the same as the message format in ::uc_service_monitor_status_st::pMessage.\n
     * If the monitor message is for Softcell and it is ECM monitor message, the format is:\n
     * 
     * "(%u) %04x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x"\n
     * \note
     *     ECM Count                             "%u"\n                            
     *     ECM PID                               "%04x"\n
     *     Page Number                           "%02x"\n    
     *     ECM Version                           "%02x"\n
     *     Status from the smartcard byte 1      "%02x"\n
     *     Status from the smartcard byte 2      "%02x"\n
     *     Reply from the smartcard byte 1       "%02x"\n
     *     Reply from the smartcard byte 2       "%02x"\n
     *     Reply from the smartcard byte 3       "%02x"\n
     *     Reply from the smartcard byte 4       "%02x"\n
     *     Reply from the smartcard byte 5       "%02x"\n
     *     Reply from the smartcard byte 6       "%02x"\n
     *     Algorithm                             "%02x"\n 
     *     Odd or Even                           "%02x"\n
     *     Next Key                              "%02x"\n
     *
     * If the monitor message is for Softcell and it is EMM monitor message, the format is:\n
     *
     * "(%u) %04x %02x %02x %02x %02x %02x %02x"\n
     * \note
     *     EMM Count                             "%u"\n                 
     *     EMM PID                               "%04x"\n 
     *     Address Control                       "%02x"\n
     *     Status from the smartcard byte 1      "%02x"\n
     *     Status from the smartcard byte 2      "%02x"\n
     *     Reply from the smartcard byte 1       "%02x"\n
     *     Reply from the smartcard byte 2       "%02x"\n
     *     Reply from the smartcard byte 3       "%02x"\n
     *
     */
    uc_char message[UC_MONITOR_MSG_LENGTH];
    
} convergent_service_monitor_info;

/**
 * Service Monitor Status structure
 *
 * This structure is only used when Unified Client is integrated.
 *
 * This is the structure that is passed as the \a lpVoid argument of 
 * ::uc_service_message_proc when the \a message argument is ::UC_SERVICE_EMM_MONITOR_STATUS_FOR_CONVERGENT_CLIENT
 * or ::UC_SERVICE_ECM_MONITOR_STATUS_FOR_CONVERGENT_CLIENT.
 */
typedef struct _uc_convergent_client_service_monitor_status_st
{
    /**
     * Service handle of Unified Client
     *
     * Service handle previously opened by a call to ::UniversalClient_OpenService.
     *
     */
    uc_service_handle serviceHandle;

    /**
     * Service type, descrambling service or EMM service.
     */
     uc_service_type serviceType;

    /**
     * The monitor info from Cloaked CA Agent.
     * Application shall check cloakedCAMonitorInfo.valid before using it.
     */
    convergent_service_monitor_info cloakedCAMonitorInfo;

    /**
     * The monitor info from SoftCell.
     * Application shall check softcellMonitorInfo.valid before using it.
     */
    convergent_service_monitor_info softcellMonitorInfo;

} uc_convergent_client_service_monitor_status_st;


/**
 * Indicate the source client
 *
 * This structure is only used when Unified Client is integrated.
 *
 * This type lets Unified Client know which client is active for the specific features.
 * For instance, normally, Unified Client can deliver IRD messages to application both from Softcell and Cloaked CA,
 * but in some secnarios, the application wants to filter the IRD messages by source dynamically. For supportting the above behaviors conveniently,
 * the application can set ::UC_SOURCE_CLOAKEDCA to Unified Client for delivering IRD messages source Cloaked CA only,
 * set ::UC_SOURCE_SOFTCELL just for Softcell's IRD messages and set ::UC_SOURCE_BOTH for delivering the IRD message both from Softcell and Cloaked CA
 * through ::UniversalClient_ConfigClient.
 */
typedef enum _uc_source_client
{
    /**
     * Source Cloaked CA
     *
     */
    UC_SOURCE_CLOAKEDCA = 0,

    /**
     * Source SoftCell
     *
     */
    UC_SOURCE_SOFTCELL = 1,

    /**
     * Source both SoftCell and Cloaked CA
     *
     */
    UC_SOURCE_BOTH = 2
}uc_source_client;

/**
 * Configuration structure for Unified Client
 *
 * When Unified Client is integrated, this type is used by ::UniversalClient_ConfigClient to configure the parameters for Unified Client.
 * If the application wants to update the configuration parameters, all the configuration parameters listed in this structure must be filled by valid value
 * even there is just one item changed.
 */
typedef struct _uc_unified_client_config
{
    /**
     * Configure the client's priority
     *
     * It is to specify which CA client has higher priority to view the simulcrypt services.
     * The default is UC_FALSE to indicate SoftCell with higher priority.
     * UC_TRUE indicates Cloaked CA with higher priority.
     *
     * \note The latest setting doesn't take effect until changing channel. 
     */
    uc_bool isCloakedCAWithHighPriority;

    /**
     * Auto client switching
     *
     * It is to enable or disable auto switching to use the other client to view the service when the service is a simulcrypt service and can not be viewed by the current client.
     * The default is UC_FALSE to indicate no switching.
     * UC_TRUE indicates auto switching service on. 
     *
     * \note The latest setting doesn't take effect until changing channel. 
     */
    uc_bool isAutoClientSwitching;

    /**
     * Configure the source of IRD message
     *
     * It is to specify which client can deliver IRD message to the application.
     * The default is ::UC_SOURCE_CLOAKEDCA to indicate just the IRD message for Cloaked CA will be notified to application.
     *
     * \note The latest setting will take effect immediately. 
     */
    uc_source_client irdMessageSource;
}uc_unified_client_config;

/**
 * Indicate the configuration type
 *
 * When CA client is integrated, the application may need to configure some information to CA client for the specific requirements.
 * This enumeration lists all types that can be configured by applicaton through the API ::UniversalClient_ConfigClient.
 */
typedef enum _uc_config_type
{
    /**
     * Configure for Unified Client
     *
     */
    UC_CONFIG_TYPE_FOR_UNIFIED_CLIENT = 0
}uc_config_type;


/**
 * CA client configuration structure
 *
 * This is the structure containing the configuration information for CA client. 
 * This structure is used by ::UniversalClient_ConfigClient.
 */
typedef struct _uc_ca_client_config
{
    /**
     * Configuration type
     *
     */
    uc_config_type configType;

    /**
     * A pointer that points to the specific configuration data according to configType
     *
     * For example, if configType is ::UC_CONFIG_TYPE_FOR_UNIFIED_CLIENT, pConfigData points the parameter with structure ::uc_unified_client_config.
     */
    void* pConfigData;
}uc_ca_client_config;

/**
 * Callback called to notify the application of an event related to a specific service. 
 *
 * An application can implement a callback function of this type and pass it to
 * ::UniversalClient_OpenService.  
 * It will then receive notifications from the client when events occur 
 * related to any of the connections associated with the service. 
 * 
 * Because this callback may occur in the context of a client thread, 
 * an application should take care to queue this message for later processing and return immediately
 * from the function. In particular, attempting to call back into any UniversalClient_XXX methods
 * from the same thread as the callback may result in undefined behavior. 
 *
 * @param[in] pMessageProcData User-defined value of the \a pMessageProcData 
 *     parameter originally passed to the 
 *     call to ::UniversalClient_OpenService. 
 * @param[in] message Message ID for this message. See the definition of 
 *     ::uc_service_message_type for a description of the different types. 
 * @param[in] lpVoid Message defined pointer. The value of this depends on the message type. 
 *     See ::uc_service_message_type for details.
 */
typedef void (*uc_service_message_proc)(
    void *pMessageProcData,
    uc_service_message_type message,
    void *pVoid);

/**
 * Initialize the Cloaked CA Agent.
 *
 * This function initializes the Cloaked CA Agent for use with an application.
 *
 * An application must call this function before calling any other Cloaked CA Agent API methods.
 * This function is responsible for creating any internal data structures, threads, global data, and other resources.
 * 
 * Calling this function more than once will cause the error ::UC_ERROR_ALREADY_STARTED to be returned.
 *
 * When this function is called, ::UniversalClientSPI_GetVersion is called to check the version of
 * the SPI implementation being used. If the version is not compatible, ::UC_ERROR_INCOMPATIBLE_SPI_VERSION
 * is returned.
 *
 * After this, ::UniversalClientSPI_GetImplementation is called to fill a structure with
 * function pointers to the implementation of SPI methods. This structure is checked
 * to make sure that the minimum required SPI methods are present. If not, ::UC_ERROR_MISSING_SPI_METHOD
 * is returned. 
 *
 * @param[in] MessageProc Callback function to be used for notifying the application of 
 *     global events. For example, ::UC_GLOBAL_RAW_IRD_MESSAGE is used to notify IRD messages(text, mail, etc) to the application.
 *     For more details on IRD messages, please refer to 705410_CCP_IRD_Messages,_a.k.a._Decoder_messages.doc provided by Irdeto.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *      ::UC_ERROR_ALREADY_STARTED
 * @retval
 *      ::UC_ERROR_MISSING_SPI_METHOD
 * @retval
 *      ::UC_ERROR_INCOMPATIBLE_SPI_VERSION
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result  UniversalClient_StartCaClient(uc_global_message_proc MessageProc);


/**
 * Shut down the Cloaked CA Agent.
 *
 * This function shuts down the Cloaked CA Agent, freeing all internal data structures, threads, global data, and other resources. 
 * An application that has called ::UniversalClient_Stop must call this function before exiting to ensure proper cleanup. 
 *
 * This function will close all the services that are sill running.
 *
 * Calling this function more than once will cause the error ::UC_ERROR_ALREADY_STOPPED to be returned.
 *
 * After this function returns, all internal threads are terminated, and it is guaranteed
 * that any \a MessageProc callback function passed to ::UniversalClient_StartCaClient will no longer
 * be called.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_ALREADY_STOPPED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result  UniversalClient_StopCaClient(void);

/**
 * Open a new service
 *
 * A 'service' is a collection of connections associated with each of the ECM
 * streams OR EMM streams(In DVB, referenced by a DVB PMT or DVB CAT).
 *
 * This function sets up the service handle and informational callbacks. After opening the service,
 * the application should call ::UniversalClient_DVB_NotifyPMT or ::UniversalClient_DVB_NotifyCAT in order
 * to begin processing ECMs or EMMs on those streams. 
 *
 * When a service is opened for special purpose, such as PPV VOD or Push VOD, the service handle can't be used by normal live descrambling service.
 * In other words, the service handle can't be reused among different kinds of services. If the service type is unchanged, the service handle can
 * be reused. Suppose there are two services running in parallel, one is VOD service and the other is live descrambling service, the suggested
 * behaviors are as below: \n
 *   1. For VOD service, open one service and get the service handle A, then use service handle A all the time for VOD service.\n
 *   2. For live descrambling service, open a second service and get service handle B, then use service handle B all the time for live descrambling service.
 * At channel switch, the application can just notify the new PMT to service handle B.\n
 *
 * Mixing-up service handles among different kinds of services may cause unexpected behavior inside Cloaked CA Agent.
 *
 * Service status can be retrieved via ::UniversalClient_GetServiceStatus once it has been opened.
 * Some advanced features can be achieved via ::UniversalClient_ConfigService.
 * A service can be closed via ::UniversalClient_CloseService.
 *
 * Generally, Digital TV Application can open one descramble service and a EMM service at startup,
 * then notify the whole PMT or CAT to this service via ::UniversalClient_DVB_NotifyPMT or ::UniversalClient_DVB_NotifyCAT.
 *
 * @param[in] pServiceContext Parameter for this service. 
 *     This parameter will be passed to ::UniversalClientSPI_Stream_Open, so that Stream SPI Implementation may know the operation source.
 *     In case of multiple tunners, this parameter can be used to indicate tunner info.
 *     For those devices that integrated SoftCell3 before,this parameter can be used to carry resource ID.
 * @param[in] messageProc Callback function to receive notification of events related to
 *     the service. Whenever the status of a service is changed,  this callback function will be called 
 *     with message type #UC_SERVICE_STATUS.
 * @param[in] pMessageProcData Callback-specific value to pass to the function specified
 *     by \a messageProc when events related to the service are sent. 
 * @param[out] pServiceHandle Receives a handle to the newly opened service. When the service is
 *     no longer needed, the caller should close it using ::UniversalClient_CloseService.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_OpenService(
    void *pServiceContext,
    uc_service_message_proc messageProc,
    void *pMessageProcData,
    uc_service_handle *pServiceHandle);

/**
 * Configure a service
 * 
 * An application can call this method to send a list of parameters to the service.  
 * 
 * This function will make a copy of the TLV parameters, post it to the internal task of Cloaked CA Agent,
 * and then return.
 *
 * This function is used to achieve some advanced features like PVR. No need to invoke this function
 * if relevant features are not used.
 *
 *     \note 
 *     Refer to \ref basicpvr_record "Basic PVR Record Flow" and \ref pvrrecord "PVR Record" 
 *     to get an overview on how to config a PVR service. 
 *
 * @param[in] serviceHandle Service handle previously opened by a call to ::UniversalClient_OpenService.
 *
 * @param[in] TLVLength Must be set to the number of the \a pTLV member. 
 *
 * @param[in] pTLV Buffer containing a list of parameters in TLV format.
 *     Please refer to \ref servicetlvtags "Cloaked CA Agent TLV Tags for Service" 
 *     for a list of TVL tags and its values.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *    ::UC_ERROR_NULL_PARAM
  * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
*/
uc_result UniversalClient_ConfigService(
    uc_service_handle serviceHandle,
    uc_uint32 TLVLength,
    const void *pTLV);

/**
 * Get the status of a service.
 *
 * This function retrieves the service status.
 *
 * The method is similar with UniversalClient_GetConvergentServiceStatus.
 * when the client is configured to support Unified Client, use ::UniversalClient_GetConvergentServiceStatus, 
 * otherwise use ::UniversalClient_GetServiceStatus.
 * 
 * Once a service is opened, Digital TV Application can query the status of the service
 * via this function.
 *
 * Refer to \ref descramblingservicestatus_dvb "Descramblig Service Status Transition" to get an 
 * overview on the service status transition.
 *
 * @param[in] serviceHandle Service handle previously opened by a call to ::UniversalClient_OpenService.
 * @param[out] serviceStatus CA client will fill this input buffer with service status string.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetServiceStatus(
    uc_service_handle serviceHandle,
    uc_byte serviceStatus[UC_CONNECTION_STATUS_STRING_SIZE]);

/**
 * Close a service handle
 *
 * This function closes a service handle previously opened by a call to 
 * ::UniversalClient_OpenService. 
 *
 * @param[in,out] pServiceHandle On input, points to the service handle to be closed.
 *     When the function returns, this will be set to INVALID_SERVICE_HANDLE. 
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INVALID_HANDLE
 * @retval
 *    ::UC_ERROR_WRONG_HANDLE_TYPE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_CloseService(
    uc_service_handle *pServiceHandle);


/**
 * Configure client
 * 
 * An application can call this method to configure parameters to client for some special requirement,
 * for example, the application can set the client's priority through this method.
 *
 * @param[in] pConfigInfo Structure containing configuration parameters.
 * When filling in the parameter pConfigInfo, the content of pointer pConfigData must be filled according to the parameter configType.
 * For instance, if application wants to enable auto switching service, the value of configType must be set to ::UC_CONFIG_TYPE_FOR_UNIFIED_CLIENT,
 * the pConfigInfo must point to a ::uc_unified_client_config parameter, set the value of isAutoClientSwitching to ::UC_TRUE, 
 * and the rest of the members in ::uc_unified_client_config must also be set to the valid value.
 *
 * \note After calling this method to configure client's priority or auto switching service, the newly setting will not take effect immediately
 * until the user changes the channel. For more details for integration code, please refer to \ref configclient "Config Client".
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
  * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
*/
uc_result UniversalClient_ConfigClient(
    uc_ca_client_config* pConfigInfo);


/**
 * Obtain the status of all ECM/EMM streams belonged to the service.
 *
 * This function can be used to obtain a list of ECM/EMM stream status belonged to the service. 
 * The service can be an EMM service or descramble service.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in] serviceHandle Service handle previously opened by a call to ::UniversalClient_OpenService.
 * @param[out] pStreamCount If the function succeeds, this parameter receives the number of 
 *     elements returned in the ppStreamStatusList array.
 * @param[out] ppStreamStatusList If the function succeeds, this parameter receives a pointer to a buffer containing a
 *     array of ::uc_service_stream_status_st structures. Cloaked CA Agent will allocate the memory, and if the client device application
 *     no longer needs the information in the array, it must call ::UniversalClient_FreeStreamStatus to free the buffer.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".  
 */
uc_result UniversalClient_GetStreamStatus(
    uc_service_handle serviceHandle,
    uc_uint32 *pStreamCount, 
    uc_service_stream_status_st **ppStreamStatusList);
    
/**
 * Free buffer allocated in ::UniversalClient_GetStreamStatus.
 *
 * This function will free the memory allocated by a previous call to ::UniversalClient_GetStreamStatus.
 *
 * @param[in,out] ppStreamStatusList On input, this is the pointer used in the previous call to ::UniversalClient_GetStreamStatus.
 *       On output, the function sets this to NULL. 
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".  
 */
uc_result UniversalClient_FreeStreamStatus(uc_service_stream_status_st **ppStreamStatusList);
    

/** @} */ /* End of groups */

/** @defgroup pvrapis Cloaked CA Agent PVR APIs
 *  Cloaked CA Agent PVR APIs
 *
 *  These API methods provide the necessary functionality for \ref glossary_pvr "PVR".
 *
 *  Irdeto provides CA based solution on content protection and management for \ref glossary_pvr "PVR".
 *  In this solution, the content is first descrambled, before re-scrambling 
 *  with the same or different algorithm to disk. Playback is achieved by retrieving 
 *  the session information to create the keys to decrypt the file on disk.
 *
 *  In order to use PVR, \ref pvrspi "Cloaked CA Agent PVR SPIs" must be implemented.
 *
 *  Refer to \ref basicpvr_record "Basic PVR Record Flow" to get an 
 *  overview on how to record a service.
 *
 *  Refer to \ref basicpvr_playback "Basic PVR Playback Flow" to get an 
 *  overview on how to playback a service.
 *

 *  On recording, the application shall configure a service or connection to Cloaked CA Agent via ::UniversalClient_ConfigService or :: UniversalClient_ConfigConnection.
 *  The status of record request will be notified to the application via service message uc_service_message_type::UC_SERVICE_PVR_RECORD_STATUS_REPLY.
 *  If the client is entitled to record the content, at least one PVR Session Metadata will be sent to the application via service message uc_service_message_type::UC_SERVICE_PVR_SESSION_METADATA_REPLY,
 *  and a PVR Session Key will be sent to SPI ::UniversalClientSPI_PVR_SetSessionKey. The application should use the session key to encrypt the descrambled content. When the record finishs, the application
 *  should call ::UniversalClient_ConfigService or ::UniversalClient_ConfigConnection to stop the recording.
 *
 *  
 *  On playback, the application shall open a service for playback and send the PVR Session Metadata to this service. Cloaked CA Agent will parse the metadata. Then If the client is entitled to playback the content, 
 *  a PVR Session Key will be sent to SPI ::UniversalClientSPI_PVR_SetSessionKey. The application should use the session key to decrypt the encrypted content. The status of playback request will be notified to 
 *  the application via service message uc_service_message_type::UC_SERVICE_PVR_PLAYBACK_STATUS_REPLY. If there are more than one PVR Session Metadata associated with the content, the application should submit
 *  these data in time and order that match the time and order when these data are generated. For each submitted PVR Session Metadata there will be a service message to tell application the status. The application
 *  shall call ::UniversalClient_CloseService to stop playback.
 *
 *  \note Cloaked CA Agent only provides \ref glossary_pvr "PVR" feature for platform with secure chipset support.
 *
 *  @{
 */

/**
 * Submit a PVR Session Metadata.
 * 
 * An application can call this method to submit PVR Session Metadata to Cloaked CA Agent.
 * A PVR Session Metadata is generated by Cloaked CA Agent in response to a PVR record request via
 * ::UniversalClient_ConfigService or ::UniversalClient_ConfigConnection with a tag ::UC_TLV_TAG_FOR_PVR_RECORD.
 * 
 * When the application wants to playback a recorded content, it should open a service and submit the PVR Session Metadata
 * to the service.
 *
 * Cloaked CA Agent will parse the PVR Session Metadata and check the entitlement. If the client is entitled to
 * playback the content, the session key for the content will be sent out via ::UniversalClientSPI_PVR_SetSessionKey.
 *
 * Cloaked CA Agent replies the result of processing the PVR Session Metadata via uc_service_message_type::UC_SERVICE_PVR_PLAYBACK_STATUS_REPLY.
 *
 * @param[in] playbackServiceHandle service handle of the playback service.
 *
 * @param[in] pPVRSessionMetadata Buffer containing the PVR Session Metadata.
 *     The uc_buffer_st::bytes member must point to a buffer containing the PVR Session Metadata, and
 *     the uc_buffer_st::length must be set to the number of bytes in the buffer.
 * 
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_SubmitPVRSessionMetadata(
    uc_service_handle playbackServiceHandle,
    const uc_buffer_st *pPVRSessionMetadata );

/** @} */ /* End of pvrapis groups */

/** @} */ /* End of Univeral Client APIs */


/** @defgroup convergentclient_api Unified Client Specific APIs

 *  Unified Client APIs
 *
 *  These API methods provide the necessary interfaces for the Unified Client device. When
 *  Unified Client is integrated, the following APIs shall be used in replace of corresponding
 *  Universal Client APIs that are only for Cloaked CA Agent. However, not all APIs have its Unified
 *  Client specific version. Some of them can be used without caring whether Cloaked CA Agent or Unified Client is integrated.
 *  , such as ::UniversalClient_OpenService.
 *
 *  @{
 */


/**
 * Get the status of a service.
 *
 * This function retrieves the service status.
 *
 * The method is similar with UniversalClient_GetServiceStatus.
 * When Unified Client is supported, use ::UniversalClient_GetConvergentServiceStatus, 
 * otherwise use ::UniversalClient_GetServiceStatus.
 * 
 * Once a service is opened, Digital TV Application can query the status of the service
 * via this function.
 *
 * Refer to \ref descramblingservicestatus_dvb "Descramblig Service Status Transition" to get an 
 * overview on the service status transition.
 *
 * @param[in] serviceHandle Service handle previously opened by a call to ::UniversalClient_OpenService.
 * @param[out] pServiceStatusInfo Unified Client will fill this service status infomation.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetConvergentServiceStatus(
    uc_service_handle serviceHandle,
    uc_convergent_client_service_status_st* pServiceStatusInfo);

/** @} */ /* End of Unified Client APIs */


#ifdef __cplusplus
}
#endif

#endif /* !UNIVERSALCLIENT_COMMON_API_H__INCLUDED__ */