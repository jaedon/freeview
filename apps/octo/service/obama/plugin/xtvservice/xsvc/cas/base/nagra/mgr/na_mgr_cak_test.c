/**************************************************************************************************/
#define ________CA_CAK_TEST_Private_Include______________________________________________
/**************************************************************************************************/
#include <na_glob.h>


/**************************************************************************************************/
#define ________CA_CAK_TEST_Golbal_Value________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_TEST_Private_Define______________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_TEST_Private_Type_Define___________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_TEST_Private_Type_Define___________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_TEST_Private_Static_Value___________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_TEST_Private_Static_Prototype________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_TEST_Private_Static_Prototype_Body____________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_TEST_Public_Functions_Body__________________________________________
/**************************************************************************************************/

#if (CONFIG_MW_CAS_NAGRA_TEST == 1) || (CONFIG_FOR_DAL_TEST_DEF == 1)
#if (CONFIG_FOR_DAL_TEST_DEF == 0)
TCaStatus caInitialization
(
  TPairingDataImportation	xPairingImportation,
  TUtcTimeImportation		xUtcTimeImportation,
  TUnsignedInt8				xMaxNumberOfIcc
)
{
	return CA_NO_ERROR;
}

TCaStatus caStartUp
(
  void
)
{
	return CA_NO_ERROR;
}

TCaStatus caRegisterAlarmExportation
(
  TAlarmExportation       xAlarmExportation,
  TCaRegistrationId*	pxRegistrationId
)
{

	return CA_NO_ERROR;
}

TCaStatus caRegisterIrdCmdExportation
(
  TIrdCmdExportation        xIrdCmdExportation,
  TCaRegistrationId*	  pxRegistrationId
)
{
	return CA_NO_ERROR;
}

TCaStatus caCreditRequest
(
  TCreditCallback     	xCreditCallbackd
)
{
	return CA_NO_ERROR;
}

TCaStatus caRegisterCreditNotification
(
  TCreditNotification 	xCreditNotification,
  TCaRegistrationId*	pxRegistrationId
)
{
	return CA_NO_ERROR;
}

TCaStatus caEventInformationRequest
(
  TEventInfoCallback  xEventInfoCallback,
  void*              pxPrivateData,
  TCalendarTime*	xStartTime,
  TSize			    xSizeOfEventSpecificDescriptors,
  TUnsignedInt8*     xEventSpecificDescriptorBlock,
  TSize         	xSizeOfServiceDescriptors,
  TUnsignedInt8*     xServiceDescriptorBlock,
  TSize         	xSizeOfSystemWideDescriptors,
  TUnsignedInt8*	xSystemWideDescriptorBlock
)
{
	return CA_NO_ERROR;
}

TCaStatus caRegisterAccessNotification
(
  TAccessNotificationCallback  	xAccessNotification,
  TCaRegistrationId*		  	pxRegistrationId
)
{
	return CA_NO_ERROR;
}

TCaStatus caEventPurchaseListRequest
(
  TEventPurchaseListCallback      xPpvListCallback
)
{
	return CA_NO_ERROR;
}

TCaStatus caRegisterEventPurchaseListNotification
(
  TEventPurchaseListNotification  xPpvListNotification,
  TCaRegistrationId*		 pxRegistrationId
)
{
	return CA_NO_ERROR;
}

TCaStatus caLocationInformationRequest
(
  TLocationInfoCallback        xLocInfoCallback
)
{
	return CA_NO_ERROR;
}

TCaStatus caRegisterLocationInformationNotification
(
  TLocationInfoNotification    xLocInfoNotification,
  TCaRegistrationId*		pxRegistrationId
)
{
	return CA_NO_ERROR;
}

TCaStatus caSystemInformationRequest
(
  TSystemInfoCallback     	xSystemInfoCallback
)
{
	return CA_NO_ERROR;
}

TCaStatus caRegisterSystemInfoNotification
(
  TSystemInfoNotification	xSystemInfoNotification,
  TCaRegistrationId*		pxRegistrationId
)
{
	return CA_NO_ERROR;
}

TCaStatus caIrdCmdAcknowledgement(TUnsignedInt8* pxIrdCmd)
{
	return CA_NO_ERROR;
}

TCaStatus caSystemInformationAcknowledgement(
	TIrdInfo* 			pxData,
	TSmartcardInfoItem*	pxSmcInfoArray

)
{
	return CA_NO_ERROR;
}

TCaStatus caLocationInformationAcknowledgement(TLocationInfoItem* pxLocInfo)
{
	return CA_NO_ERROR;
}

TCaStatus caCreditAcknowledgement(
	TCreditItem*	pxCreditoArray

)
{
	return CA_NO_ERROR;
}

TCaStatus caEventPurchaseListAcknowledgement(
	TEventPurchaseListItem*	pxEventPurchaseListArray

)
{
	return CA_NO_ERROR;
}

TCaObjectStatus caPurchaseGetProduct
(
	const TCaPurchase*pxPurchase,
	const TCaProduct**ppxProduct
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caProductGetId
(
	const TCaProduct*pxProduct,
	TCaProductId*pxProductId
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caProductGetName
(
	const TCaProduct*pxProduct,
	TSize*pxSize,
	const TUnsignedInt8**ppxName
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caPurchaseGetExpirationDate
(
	const TCaPurchase*   pxPurchase,
	TCalendarTime* pxExpirationDate
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caPurchaseGetDate
(
	const TCaPurchase*pxPurchase,
	TCalendarTime*pxDate
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caPurchaseGetCost
(
  	const TCaPurchase*pxPurchase,
       TCreditAmount*pxCost
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caPurchaseGetFlags
(
  	const TCaPurchase*pxPurchase,
       TPurchaseFlags*pxFlags
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caEventGetAccess
(
  	const TCaEvent*pxEvent,
       TCaAccess*pxAccess
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caEventGetPreviewTime
(
  	const TCaEvent*pxEvent,
       TEventPreviewTime*pxPreviewTime
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caProductGetFlags
(
  	const TCaProduct*pxProduct,
       TProductFlags*pxFlags
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caProductGetCost
(
  	const TCaProduct*pxProduct,
       TCreditAmount*pxCost
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaRequestStatus caRequestSetStartTime
(
       TCaRequest*pxRequest,
  	const TCalendarTime*pxStartTime
)
{
	return CA_REQUEST_NO_ERROR;
}

TCaRequestStatus caRequestSetProduct
(
	TCaRequest*pxRequest,
	const TCaProduct*pxProduct
)
{
	return CA_REQUEST_NO_ERROR;
}

TCaStatus caCheckPinCode
(
  TPinCodeStatusCallback	 xPinCodeStatusCallback,
  TPinIndex                xPinIndex,
  TPinCode                 xPinCode,
  void*                   pxPrivateData
)
{
	return CA_NO_ERROR;
}

TCaStatus caChangePinCode
(
  TPinCodeStatusCallback   xPinCodeStatusCallback,
  TPinIndex                xPinIndex,
  TPinCode                 xOldPinCode,
  TPinCode                 xNewPinCode,
  void*                   pxPrivateData
)
{
	return CA_NO_ERROR;
}
#else		// (CONFIG_FOR_DAL_TEST_DEF == 0)
#if	defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO) || defined(CONFIG_PROD_HGS1000S) || defined(CONFIG_PROD_HD9000I)
#if (0)
TScrStatus scrOpenEncryptionSession
(
  TScrEncSessionHandler**   ppxScrEncSessionHandler,
  TTransportSessionId         xTransportSessionId
)
{
	return SCR_NO_ERROR;
}

TScrStatus scrCloseEncryptionSession
(
  TScrEncSessionHandler*   pxScrEncSessionHandler
)
{
	return SCR_NO_ERROR;
}

TScrStatus scrSetEncryptionEmi
(
  TScrEncSessionHandler*  pxScrEncSessionHandler,
  TUnsignedInt16           xEncryptionEmi
)
{
	return SCR_NO_ERROR;
}

TScrStatus scrSetClearTextEncryptionKey
(
        TScrEncSessionHandler*  pxScrEncSessionHandler,
        TSize                    xScrEncryptionKeySize,
  const TUnsignedInt8*          pxScrEncryptionKey
)
{
	return SCR_NO_ERROR;
}

TScrStatus scrOpenDecryptionSession
(
  TScrDecSessionHandler** ppxScrDecSessionHandler,
  TTransportSessionId       xTransportSessionId
)
{
	return SCR_NO_ERROR;
}

TScrStatus scrCloseDecryptionSession
(
  TScrDecSessionHandler* pxScrDecSessionHandler
)
{
	return SCR_NO_ERROR;
}

TScrStatus scrSetDecryptionEmi
(
  TScrDecSessionHandler*  pxScrDecSessionHandler,
  TUnsignedInt16           xDecryptionEmi
)
{
	return SCR_NO_ERROR;
}

TScrStatus scrSetClearTextDecryptionKey
(
        TScrDecSessionHandler*  pxScrDecSessionHandler,
        TSize                    xScrDecryptionKeySize,
  const TUnsignedInt8*          pxScrDecryptionKey
)
{
	return SCR_NO_ERROR;
}

TScrStatus scrSet1LevelProtectedDecryptionKey
(
        TScrDecSessionHandler*  pxScrDecSessionHandler,
        TSize                    xL1ProtectingKeySize,
  const TUnsignedInt8*          pxL1ProtectingKey,
        TSize                    xProtectedDecryptionKeySize,
  const TUnsignedInt8*          pxProtectedDecryptionKey
)
{
	return SCR_NO_ERROR;
}

TScrStatus scrSetLegacyDecryptionMode
(
  TScrDecSessionHandler*  pxScrDecSessionHandler
)
{
	return SCR_NO_ERROR;
}
#endif
#else		// (CONFIG_PROD_DIGIPLUSCHD)
TCaRequestStatus caRequestCreate
(
	TCaRequestType xType,
	TCaRequest**ppxRequest
)
{
	return CA_REQUEST_NO_ERROR;
}

TCaRequestStatus caRequestDispose
(
	TCaRequest* pxRequest
)
{
	return CA_REQUEST_NO_ERROR;
}

TCaRequestStatus caRequestGetProcessingStatus
(
	const TCaRequest* pxRequest,
	TCaProcessingStatus* pxProcessingStatus
)
{
	return CA_REQUEST_NO_ERROR;
}

TCaRequestStatus caRequestGetObjects
(
	const TCaRequest*pxRequest,
	TUnsignedInt32*pxNumberOfObjects,
	void***pppxObjectArray
)
{
	return CA_REQUEST_NO_ERROR;
}

TCaObjectStatus caSmartcardGetExpirationDate
(
	const TCaSmartcard*pxSmartcard,
	TCalendarTime*pxExpirationDate
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaRequestStatus caRequestSetAsynchronousResponse
(
	TCaRequest*pxRequest,
	TCaRequestCallback xCallback
)
{
	return CA_REQUEST_NO_ERROR;
}

TCaRequestStatus caRequestSend
(
	TCaRequest*pxRequest
)
{
	return CA_REQUEST_NO_ERROR;
}

TCaRequestStatus caRequestSetDescriptors
(
        TCaRequest*pxRequest,
        TSize xSizeOfDescriptors,
  	const TUnsignedInt8*pxDescriptorBlock
)
{
	return CA_REQUEST_NO_ERROR;
}

TCaObjectStatus caEventGetRelatedProducts
(
  	const TCaEvent*pxEvent,
       TUnsignedInt32*pxNumberOfProducts,
  	const TCaProduct***pppxProductArray
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caProductGetRemainingRentalTime
(
  const TCaProduct*  pxProduct,
        TCaDuration* pxRemainingRentalTime
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caSmartcardGetState
(
  const TCaSmartcard*	pxSmartcard,
        TSmartcardState*	pxState
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caProgramGetAccess
(
  const TCaProgram* pxProgram,
        TCaAccess*  pxAccess
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caProgramGetNumber
(
  const TCaProgram*     pxProgram,
        TProgramNumber* pxNumber
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caProgramGetTransportSessionId
(
  const TCaProgram*          pxProgram,
        TTransportSessionId* pxTransportSessionId
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaObjectStatus caProgramGetMetadata
(
  const TCaProgram*      pxProgram,
        TUnsignedInt8** ppxMetaDataBuffer,
        TSize*           pxMetaDataLength
)
{
	return CA_OBJECT_NO_ERROR;
}

TCaListenerStatus caListenerCreate
(
  TCaListenerType         xType,
  TCaListenerCallback     xCallback,
  TCaListener**         ppxListener
)
{
	return CA_LISTENER_NO_ERROR;
}

TCaListenerStatus caListenerRegister
(
  TCaListener*       pxListener
)
{
	return CA_LISTENER_NO_ERROR;
}

TCaListenerStatus caListenerDispose
(
  TCaListener*       pxListener
)
{
	return CA_LISTENER_NO_ERROR;
}
#endif		// (CONFIG_PROD_DIGIPLUSCHD)

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
void dvlTerminate(void)
{
	return;
}

TDvlStatus dvlSystemInformationRequest(TDvlVersionString xVersionString, TUnsignedInt32* pxIrdSerialNumberValue, TDvlUniqueIdString xIrdSerialNumberString,
														TUnsignedInt32*    pxNuidValue, TDvlUniqueIdString  xNuidString)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlChipsetInformationRequest(TDvlChipsetInfo** ppxChipsetInformation)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlHomeNetworkInformationRequest(TUnsignedInt32* pxHomeNetworkIdValue, TDvlUniqueIdString xHomeNetworkIdString, TUnsignedInt8* pxHomeNetworkVersion,
																TUnsignedInt8* pxNumberOfNeighbourhoodInfoItems, const TDvlNeighbourhoodInfo** ppxNeighbourhoodInfoItems)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlCreateRecordSessionParameters(TDvlRecordSessionParameters** ppxRecordParameters)
{
	return DVL_NO_ERROR;

}

TDvlStatus dvlStartRecordEx(TDvlRecordSessionParameters* pxRecordParameters, TSize* pxOutputCredentialsSize,const TUnsignedInt8** ppxOutputCredentials,
										TDvlRecordStatus* pxRecordingStatus, TDvlHandle** ppxRecordHandle)
{
  	return DVL_NO_ERROR;
}

TDvlStatus dvlStartRecord(TTransportSessionId xTSid, TSize xProfileSize,  const TUnsignedInt8* pxProfile, TSize  xSpecificMetadataSize,  const TUnsignedInt8* pxSpecificMetadata,
        						TUnsignedInt32        xCollectionId, TSize xInputCredentialsSize, const TUnsignedInt8* pxInputCredentials, TDvlRecordErrorCB xRecordErrorCB, TSize* pxOutputCredentialsSize,
	 						const TUnsignedInt8** ppxOutputCredentials, TDvlRecordStatus* pxRecordingStatus, TSize* pxGenericMetadataSize, const TUnsignedInt8** ppxGenericMetadata, TDvlHandle** ppxRecordHandle)
{
  	return DVL_NO_ERROR;
}

TDvlStatus dvlStopRecord(TDvlHandle* pxRecordHandle)
{
	return DVL_NO_ERROR;
}

NAGRA_CA_API void dvlDebugControlRole(TDvlDebugModule xModule, TDvlDebugVerbosity xVerbosity, TDvlDebugClass32 xClass, TDvlDebugRole xRole)
{
	return;
}

TDvlStatus dvlReleaseBuffer(const TUnsignedInt8* pxBuffer)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlCredentialsInformationRequest(TSize xCredentialsSize, const TUnsignedInt8* pxCredentials, TCredentialsType* pxCredentialsType, const TCredentialsUnion** ppxCredentialsUnion,
											TSize* pxGenericMetadataSize, const TUnsignedInt8** ppxGenericMetadata, TSize* pxSpecificMetadataSize, const TUnsignedInt8** ppxSpecificMetadata)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlStartPlayback(TTransportSessionId xTSid, TSize xInputCredentialsSize, const TUnsignedInt8* pxInputCredentials, TDvlPlaybackErrorCB xPlaybackErrorCB, TSize* pxOutputCredentialsSize,
								const TUnsignedInt8** ppxOutputCredentials, TDvlAccessStatus* pxPlaybackStatus, TSize* pxGenericMetadataSize, const TUnsignedInt8** ppxGenericMetadata,
								TSize* pxSpecificMetadataSize, const TUnsignedInt8** ppxSpecificMetadata, TDvlHandle** ppxPlaybackHandle)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlStopPlayback(TDvlHandle* pxPlaybackHandle)
{
	return DVL_NO_ERROR;
}

#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)
TDvlStatus dvlPlaybackOpenSession(TTransportSessionId xTSid, TDvlHandle** ppxPlaybackHandle)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlPlaybackSetEntitlements(TDvlHandle* pxPlaybackHandle, TDvlEntitlementsParameters* pxEntitlementsParameters, TDvlAccessStatus* pxPlaybackStatus, TSize* pxOutputCredentialsSize,
													const TUnsignedInt8** ppxOutputCredentials, TSize* pxKeyIdSize, const TUnsignedInt8** ppxKeyId)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlPlaybackCloseSession(TDvlHandle* pxPlaybackHandle)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlPostPrmManagementMessage(TSize xPrmManagementMessageSize,const TUnsignedInt8* pxPrmManagementMessage)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlGetAuthenticatedMessage(TSize xCipheredMessageSize, const TUnsignedInt8* pxCipheredMessage, TSize* pxAuthenticatedDataSize, const TUnsignedInt8** ppxAuthenticatedData)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlOpaqueDataMessageCreate(TOdmHandle** ppxOpaqueDataMessageHandle)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlOpaqueDataMessageRelease(TOdmHandle* pxOpaqueDataMessageHandle)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlOpaqueDataMessageSetApplicationData(TOdmHandle* pxOpaqueDataMessageHandle,TSize xApplicationDataSize, const TUnsignedInt8* pxApplicationData)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlOpaqueDataMessageSetPrmSignalization(TOdmHandle* pxOpaqueDataMessageHandle, TSize xPrmSignalizationSize, const TUnsignedInt8* pxPrmSignalization)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlOpaqueDataMessageGet(TOdmHandle* pxOpaqueDataMessageHandle,TSize* pxOpaqueDataMessageSize, const TUnsignedInt8** ppxOpaqueDataMessage)
{
	return DVL_NO_ERROR;
}
#endif

TDvlStatus dvlUpdateSpecificLcmMetadata(TSize xInLcmCredentialsSize, const TUnsignedInt8* pxInLcmCredentials, TSize xSpecificMetadataSize,
											const TUnsignedInt8* pxSpecificMetadata, TSize* pxOutLcmCredentialsSize, const TUnsignedInt8** ppxOutLcmCredentials)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlUpdateGenericLcmMetadata(TSize xInLcmCredentialsSize, const TUnsignedInt8* pxInLcmCredentials, TSize xGenericMetadataSize, const TUnsignedInt8* pxGenericMetadata,
												TSize* pxOutLcmCredentialsSize, const TUnsignedInt8**     ppxOutLcmCredentials)
{
	return DVL_NO_ERROR;
}

TDvlStatus dvlCreateLegacyLcm(TSize xSpecificMetadataSize, const TUnsignedInt8* pxSpecificMetadata, TSize* pxOutputCredentialsSize, const TUnsignedInt8** ppxOutputCredentials)
{
	return DVL_NO_ERROR;
}
#endif

#endif		// (CONFIG_FOR_DAL_TEST_DEF == 0)
#endif		// (CONFIG_MW_CAS_NAGRA_TEST == 1) || (CONFIG_FOR_DAL_TEST_DEF == 1)


