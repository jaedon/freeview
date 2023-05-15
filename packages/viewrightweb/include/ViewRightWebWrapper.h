#ifndef __VIEWRIGHTWEB_WRAPPER_H__
#define __VIEWRIGHTWEB_WRAPPER_H__

#ifndef WIN32
#define __cdecl
#endif

#ifdef __cplusplus
	extern "C"
	{
#endif

#define COMPANY_NAME_MAX_LENGTH 256

typedef void (__cdecl *KeyRetrCallback)(void *, const char *, unsigned long);

typedef enum VRWebClientError_e
{
	VR_Success = 0,
	NoConnect,
	GeneralError,
	BadMemoryAlloc,
	BadRandNumGen,
	BadURL,
	BadReply,
	BadReplyMoved,
	BadVerifyCertificateChain,
	BadCreateKeyPair,
	NotEntitled,
	BadCreateStore,
	BadWriteStore,
	BadReadStore,
	BadStoreIntegrity,
	BadStoreFileNoExist,
	BadCert,
	BadINI,
	BadPrivateKey,
	BadConvertPEMToX509,
	BadPublicEncrypt,
	BadAddX509Entry,
	BadAddX509Subject,
	BadX509Sign,
	CantRetrieveBoot,
	CantProvision,
	BadArguments,
	BadKeyGeneration,
	NotProvisioned,
	CommunicationObjectNotInitialized,
	NoOTTSignature,
	BadOTTSignature,
	KeyFileNotEntitled,
	CertificateExpired,
	IntegrityCheckFailed,
	SecurityError,
	FeatureUnavailable,
	NoUniqueIdentifier,
} VRWebClientError_t;

typedef struct VRWebClientStatus_Struct
{
	unsigned int		bIsProvisioned;
	unsigned int		bIsCertExpired;
	unsigned long long	certExpirationTime;
	char				companyName[COMPANY_NAME_MAX_LENGTH];

} VRWebClientStatus_t;


	/* C TypeÀÇ Function ¼±¾ð */
	//int test_function(int a);

	// Initialization

VRWebClientError_t 	WebClient_ViewRight_InitializeSSL(void);

VRWebClientError_t 	WebClient_ViewRight_SetVCASCommunicationHandlerSettings(const char * VCASBootAddress, const char * storePath, const char * companyName );

VRWebClientError_t WebClient_ViewRight_SetUniqueIdentifier(const char * uniqueID, const char * validationString);

	// Provisioning, unique identifier and Key Retrieval

VRWebClientError_t 	WebClient_ViewRight_IsDeviceProvisioned(void);
VRWebClientError_t 	WebClient_ViewRight_ConnectAndProvisionDevice(void);
VRWebClientError_t 	WebClient_ViewRight_CheckVCASConnection(void);
VRWebClientError_t 	WebClient_ViewRight_RetrieveKeyFile(const char * url, char * key);
VRWebClientError_t 	WebClient_ViewRight_GetUniqueIdentifier(char * * uniqueID, int * length);

void 			WebClient_ViewRight_SetKeyRetrievalStatusCallback(void * userData, KeyRetrCallback func);

	// Status

VRWebClientError_t 	WebClient_ViewRight_GetClientStatus(VRWebClientStatus_t * clientStatus);

	// Logging

void 			WebClient_ViewRight_SetLogging(unsigned int OnOff);

	// Version Info

const char * 	WebClient_ViewRight_GetVersion(void);


#ifdef __cplusplus
	}
#endif

#endif //__VIEWRIGHTWEB_WRAPPER_H__

