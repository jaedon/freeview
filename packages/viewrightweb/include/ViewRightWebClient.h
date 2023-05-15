#pragma once

#ifndef WIN32
#define __cdecl
#endif

#define COMPANY_NAME_MAX_LENGTH 256

typedef void (__cdecl *KeyRetrCallback)(void *, const char *, unsigned long);

class VCASCommunicationHandler;

class ViewRightWebClient
{
public:
	enum VRWebClientError_t
	{
		Success = 0,
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
	};

	typedef struct VRWebClientStatus_Struct
	{
		bool				bIsProvisioned;
		bool				bIsCertExpired;
		unsigned long long	certExpirationTime;
		char				companyName[COMPANY_NAME_MAX_LENGTH];

	} VRWebClientStatus_t;

	virtual ~ViewRightWebClient();

	static ViewRightWebClient * GetInstance(); 

	// Initialization

	VRWebClientError_t SetVCASCommunicationHandlerSettings(const char * VCASBootAddress, const char * storePath, const char * companyName = "");
	VRWebClientError_t InitializeSSL(void);

	// Provisioning, unique identifier and Key Retrieval
	VRWebClientError_t SetUniqueIdentifier(const char * uniqueID, const char * validationString);

	VRWebClientError_t IsDeviceProvisioned(void);
	VRWebClientError_t ConnectAndProvisionDevice(void);
	VRWebClientError_t CheckVCASConnection(void);
	VRWebClientError_t RetrieveKeyFile(const char * url, char * key);
	VRWebClientError_t GetUniqueIdentifier(char * * uniqueID, int * length);

	void SetKeyRetrievalStatusCallback(void * userData, KeyRetrCallback func);

	// Status

	VRWebClientError_t GetClientStatus(VRWebClientStatus_t * clientStatus);

	// Logging

	void SetLogging(bool OnOff);

	// Version Info

	const char * GetVersion(void);

private:

	bool 			m_bIsProvisioned;
	bool			m_bHaveSettings;
	KeyRetrCallback 	m_callbackFunction;
	void * 			m_userData;
	char			m_companyName[COMPANY_NAME_MAX_LENGTH];

	static ViewRightWebClient * m_pInstance;
	VCASCommunicationHandler * m_pVCASCommunicationHandler;
	bool m_bSSLInitialized;

	ViewRightWebClient();
};
