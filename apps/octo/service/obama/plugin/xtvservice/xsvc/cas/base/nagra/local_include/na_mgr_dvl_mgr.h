/*
@file				na_mgr_cak_ext.h
@brief				na_mgr_cak_ext.h (Nagra CAK Service)

Description:  \n
				\n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2008 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef _CA_DVL_MGR_H
#define _CA_DVL_MGR_H
#include "na_glob.h"


/********************************************************************
  Structure and Type Definition
 ********************************************************************/



/********************************************************************
  Extern Function Declaration
 ********************************************************************/
HERROR		CA_NA_DvlInitialze(void);
HERROR		CA_NA_GetDvlVersion(TUnsignedInt8** ppxVersion);
HERROR 		CA_NA_GetDvlChipsetType(TUnsignedInt8** ppxChipsetType);

HERROR		CA_NA_DvlResetPersistentMemory(void);
void 		CA_NA_dvlTerminate(void);
TDvlStatus CA_NA_dvlSystemInformationRequest(TDvlVersionString xVersionString, TUnsignedInt32* pxIrdSerialNumberValue, TDvlUniqueIdString xIrdSerialNumberString,
														TUnsignedInt32*    pxNuidValue, TDvlUniqueIdString	xNuidString);
TDvlStatus CA_NA_dvlChipsetInformationRequest(TDvlChipsetInfo** ppxChipsetInformation);
TDvlStatus CA_NA_dvlHomeNetworkInformationRequest(TUnsignedInt32* pxHomeNetworkIdValue, TDvlUniqueIdString xHomeNetworkIdString, TUnsignedInt8* pxHomeNetworkVersion,
																TUnsignedInt8* pxNumberOfNeighbourhoodInfoItems, const TDvlNeighbourhoodInfo** ppxNeighbourhoodInfoItems);
TDvlStatus CA_NA_dvlCreateRecordSessionParameters(TDvlRecordSessionParameters** ppxRecordParameters);
TDvlStatus CA_NA_dvlStartRecordEx(TDvlRecordSessionParameters* pxRecordParameters, TSize* pxOutputCredentialsSize,const TUnsignedInt8** ppxOutputCredentials,
										TDvlRecordStatus* pxRecordingStatus, TDvlHandle** ppxRecordHandle);
TDvlStatus CA_NA_dvlStartRecord(TTransportSessionId xTSid, TSize xProfileSize,  const TUnsignedInt8* pxProfile, TSize  xSpecificMetadataSize,  const TUnsignedInt8* pxSpecificMetadata,
        						TUnsignedInt32        xCollectionId, TSize xInputCredentialsSize, const TUnsignedInt8* pxInputCredentials, TDvlRecordErrorCB xRecordErrorCB, TSize* pxOutputCredentialsSize,
	 						const TUnsignedInt8** ppxOutputCredentials, TDvlRecordStatus* pxRecordingStatus, TSize* pxGenericMetadataSize, const TUnsignedInt8** ppxGenericMetadata, TDvlHandle** ppxRecordHandle);
TDvlStatus CA_NA_dvlStopRecord(TDvlHandle* pxRecordHandle);
TDvlStatus CA_NA_dvlReleaseBuffer(const TUnsignedInt8* pxBuffer);
TDvlStatus CA_NA_dvlCredentialsInformationRequest(TSize xCredentialsSize, const TUnsignedInt8* pxCredentials, TCredentialsType* pxCredentialsType, const TCredentialsUnion** ppxCredentialsUnion,
											TSize* pxGenericMetadataSize, const TUnsignedInt8** ppxGenericMetadata, TSize* pxSpecificMetadataSize, const TUnsignedInt8** ppxSpecificMetadata);
TDvlStatus CA_NA_dvlStartPlayback(TTransportSessionId xTSid, TSize xInputCredentialsSize, const TUnsignedInt8* pxInputCredentials, TDvlPlaybackErrorCB xPlaybackErrorCB, TSize* pxOutputCredentialsSize,
								const TUnsignedInt8** ppxOutputCredentials, TDvlAccessStatus* pxPlaybackStatus, TSize* pxGenericMetadataSize, const TUnsignedInt8** ppxGenericMetadata,
								TSize* pxSpecificMetadataSize, const TUnsignedInt8** ppxSpecificMetadata, TDvlHandle** ppxPlaybackHandle);
TDvlStatus CA_NA_dvlStopPlayback(TDvlHandle* pxPlaybackHandle);
#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)
TDvlStatus CA_NA_dvlPlaybackOpenSession(TTransportSessionId xTSid, TDvlHandle** ppxPlaybackHandle);
TDvlStatus CA_NA_dvlPlaybackSetEntitlements(TDvlHandle* pxPlaybackHandle, TDvlEntitlementsParameters* pxEntitlementsParameters, TDvlAccessStatus* pxPlaybackStatus, TSize* pxOutputCredentialsSize,
													const TUnsignedInt8** ppxOutputCredentials, TSize* pxKeyIdSize, const TUnsignedInt8** ppxKeyId);
TDvlStatus CA_NA_dvlPlaybackCloseSession(TDvlHandle* pxPlaybackHandle);
TDvlStatus CA_NA_dvlPostPrmManagementMessage(TSize xPrmManagementMessageSize,const TUnsignedInt8* pxPrmManagementMessage);
TDvlStatus CA_NA_dvlGetAuthenticatedMessage(TSize xCipheredMessageSize, const TUnsignedInt8* pxCipheredMessage, TSize* pxAuthenticatedDataSize, const TUnsignedInt8** ppxAuthenticatedData);
TDvlStatus CA_NA_dvlOpaqueDataMessageCreate(TOdmHandle** ppxOpaqueDataMessageHandle);
TDvlStatus CA_NA_dvlOpaqueDataMessageRelease(TOdmHandle* pxOpaqueDataMessageHandle);
TDvlStatus CA_NA_dvlOpaqueDataMessageSetApplicationData(TOdmHandle* pxOpaqueDataMessageHandle,TSize xApplicationDataSize, const TUnsignedInt8* pxApplicationData);
TDvlStatus CA_NA_dvlOpaqueDataMessageSetPrmSignalization(TOdmHandle* pxOpaqueDataMessageHandle, TSize xPrmSignalizationSize, const TUnsignedInt8* pxPrmSignalization);
TDvlStatus CA_NA_dvlOpaqueDataMessageGet(TOdmHandle* pxOpaqueDataMessageHandle,TSize* pxOpaqueDataMessageSize, const TUnsignedInt8** ppxOpaqueDataMessage);
#endif
TDvlStatus CA_NA_dvlUpdateSpecificLcmMetadata(TSize xInLcmCredentialsSize, const TUnsignedInt8* pxInLcmCredentials, TSize xSpecificMetadataSize,
											const TUnsignedInt8* pxSpecificMetadata, TSize* pxOutLcmCredentialsSize, const TUnsignedInt8** ppxOutLcmCredentials);
TDvlStatus CA_NA_dvlUpdateGenericLcmMetadata(TSize xInLcmCredentialsSize, const TUnsignedInt8* pxInLcmCredentials, TSize xGenericMetadataSize, const TUnsignedInt8* pxGenericMetadata,
												TSize* pxOutLcmCredentialsSize, const TUnsignedInt8**	  ppxOutLcmCredentials);
TDvlStatus CA_NA_dvlCreateLegacyLcm(TSize xSpecificMetadataSize, const TUnsignedInt8* pxSpecificMetadata, TSize* pxOutputCredentialsSize, const TUnsignedInt8** ppxOutputCredentials);



#endif /* _CA_DVL_MGR_H */

