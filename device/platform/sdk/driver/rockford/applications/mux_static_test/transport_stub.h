/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: transport_stub.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 10/25/11 3:39p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/applications/mux_static_test/transport_stub.h $
 * 
 * Hydra_Software_Devel/7   10/25/11 3:39p nilesh
 * SW7425-1236: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-1580/1   10/25/11 3:20p nilesh
 * SW7425-1236: Update mux static test to compile/run
 * 
 * Hydra_Software_Devel/6   12/21/10 4:37p nilesh
 * SW7425-38: Fix Pacing
 * 
 * Hydra_Software_Devel/5   12/9/10 2:11p nilesh
 * SW7425-38: Added PCR/System Data Muxing Support
 * 
 * Hydra_Software_Devel/4   11/11/10 3:37p nilesh
 * SW7425-38: Update to use actual XPT PI
 * 
 * Hydra_Software_Devel/3   10/5/10 12:40p nilesh
 * SW7425-38: Updated to use transport device/channel
 * 
 * Hydra_Software_Devel/2   10/4/10 2:21p nilesh
 * SW7425-38: Added pacing
 * 
 * Hydra_Software_Devel/1   9/28/10 4:53p nilesh
 * SW7425-38: Added transport stub
 *
 ***************************************************************************/

#ifndef TRANSPORT_STUB_H_
#define TRANSPORT_STUB_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

#define MAX_TRANSPORT_CHANNELS 3
/* #define BTST_P_USE_XPT_MUX_STUB 1 */

/* Transport Stub */
typedef struct TransportStubDeviceContext* TransportDeviceHandle;

typedef struct TransportDeviceSettings
{
      BCHP_Handle hChp;
      BREG_Handle hReg;
      BINT_Handle hInt;
      BMEM_Handle hMem;
      char *szFriendlyName;
} TransportDeviceSettings;

typedef struct TransportChannelSettings
{
      uint32_t uiTransportIndex;

      uint32_t uiTransportDepth;
      char *szFriendlyName;
} TransportChannelSettings;

typedef struct TransportStubContext
{
      bool bInUse;
      TransportDeviceHandle hDeviceTransport;

      TransportChannelSettings stSettings;

      uint32_t uiTransportDescriptorsReceived;

#ifdef BTST_P_USE_XPT_MUX_STUB
      const BMUXlib_TS_TransportDescriptor **astTransportDescriptors;
#else
      BXPT_Playback_Handle hXptPb;
      BXPT_PvrDescriptor8 *astPvrDescriptors;
#endif

      uint32_t uiQueueSize;
      uint32_t uiTransportDescriptorsRead;
      uint32_t uiTransportDescriptorsWrite;
} TransportStubContext;

typedef struct TransportStubDeviceContext
{
      TransportDeviceSettings stSettings;

#ifndef BTST_P_USE_XPT_MUX_STUB
      BXPT_Handle hXpt;
      BXPT_PcrOffset_Handle hXptPcrOffset;
      BXPT_TsMux_Handle hXptTsMux;
#endif

      FILE *hFile;
      uint32_t uiCurrentESCR;
      uint32_t uiCurrentPacket2PacketDelta;

      BMUXlib_TS_TransportStatus stStatus;

      TransportStubContext ahChannel[MAX_TRANSPORT_CHANNELS];
} TransportStubDeviceContext;

BERR_Code
app_OpenTransportDevice(
         TransportDeviceHandle *phDeviceTransport,
         const TransportDeviceSettings* pstSettings
         );

BERR_Code
app_CloseTransportDevice(
         TransportDeviceHandle hDeviceTransport
         );

BERR_Code
app_TransportIncrementTime(
         TransportDeviceHandle hDeviceTransport,
         unsigned uiMilliseconds
         );

BERR_Code
app_GetTransportSettings(
         TransportDeviceHandle hDeviceTransport,
         BMUXlib_TS_TransportSettings *pstTransportSettings
         );

BERR_Code
app_SetTransportSettings(
         TransportDeviceHandle hDeviceTransport,
         const BMUXlib_TS_TransportSettings *pstTransportSettings
         );


BERR_Code
app_GetTransportStatus(
         TransportDeviceHandle hDeviceTransport,
         BMUXlib_TS_TransportStatus *pstTransportStatus
         );

/*********************/
/* Transport Channel */
/*********************/
typedef struct TransportStubContext* TransportChannelHandle;

BERR_Code
app_OpenTransportChannel(
         TransportDeviceHandle hDeviceTransport,
         TransportChannelHandle *phChannelTransport,
         const TransportChannelSettings* pstSettings
         );

BERR_Code
app_CloseTransportChannel(
         TransportChannelHandle hChannelTransport
         );

BERR_Code
app_AddTransportDescriptors(
         TransportChannelHandle hChannelTransport,
         const BMUXlib_TS_TransportDescriptor *astTransportDescriptors, /* Array of pointers to transport descriptors */
         size_t uiCount, /* Count of descriptors in array */
         size_t *puiQueuedCount /* Count of descriptors queued (*puiQueuedCount <= uiCount) */
         );

BERR_Code
app_GetCompletedTransportDescriptors(
         TransportChannelHandle hChannelTransport,
         size_t *puiCompletedCount /* Count of descriptors completed */
         );

#ifdef __cplusplus
}
#endif

#endif /* TRANSPORT_STUB_H_ */
