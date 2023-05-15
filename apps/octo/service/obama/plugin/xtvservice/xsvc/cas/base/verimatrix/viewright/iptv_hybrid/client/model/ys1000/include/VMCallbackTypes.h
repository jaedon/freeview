#ifndef _VMCALLBACKTYPES_H_
#define _VMCALLBACKTYPES_H_

typedef enum {
    eVcasVersion_31x = 0,
    eVcasVersion_320,
    eVcasVersion_321,
    eVcasVersion_330,
    eVcasVersion_331,
    eVcasVersion_340
} eVcasVersion_t;
#define MAX_VCAS_PROTO eVcasVersion_340

typedef enum {
   eVmDecodeInit = -1,                   // Initial state to detect first decode
   eVmDecodeOk = 0,                      // Decode proceeding
   eVmDecodeFailurePinRequired = 100,    // program Rating Level exceeds STB's
   eVmDecodeFailureKeyNotAvailable,      // Could be not authorized or network error getting key
   eVmDecodeFailureBlackout,             // Service Class >= 0xFF0000 matched
   eVmDecodeFailureOther,                // TBD
   eVmDecodeFailureTA,                //  decryption fail due to trustzone applet error
} eVmDecodeResult;
// the following enum was deprecated because it was too specific 
// for the general failure being reported. This define is here to
// support any existing code
#define   eVmDecodeFailureNoServiceClass    eVmDecodeFailureKeyNotAvailable

typedef struct keydata_s {
    int control_flag;       /* non-zero means this key is valid */
    unsigned char key[32];
    unsigned char iv[32];
} keydata_t;

typedef enum {
    eEncAlg_null = 0,
    eEncAlg_RC4 = 1, 
    eEncAlg_AESECBT = 2,
    eEncAlg_proprietary1 = 3,
    eEncAlg_AESECBL = 4,
    eEncAlg_DVBCSA = 5,
    eEncAlg_proprietary2 = 6,
    eEncAlg_AESCBC1 = 7
} eEncryptionAlgorithm_t;

#define nMaxNumEncAlg 5    /* max number of simultaneous hw algorithms supported */

typedef struct descramble_param_s {
    eEncryptionAlgorithm_t encAlg;
    int    keylength;
    struct keydata_s odd;
    struct keydata_s even;        
    int    iCwp_data_length;
    void  *pCwp_data;  // NULL means clear CW, otherwise target specific structure

} descramble_param_t;


#ifdef __cplusplus
  extern "C" {
#endif

#ifndef WIN32
  #define __cdecl
#endif
      
typedef void (__cdecl *RenderFunc)(void *, unsigned char *, unsigned long);

typedef int  (__cdecl *DataWriteFunc)(void *, char *, int  );
typedef int  (__cdecl *DataReadFunc )(void *, char *, unsigned int *);

typedef int  (__cdecl *PasswordSaveFunc)(void * pArg, char * pSubjKeyId, char * pEncryptedPwd, int iPwdLen);
typedef int  (__cdecl *PasswordRetrieveFunc)(void * pArg, char * pSubjKeyId, char * pEncryptedPwd, int iMaxLen, int * pPwdLen);

typedef void (__cdecl *StreamChangeCallback)(void * pArg, unsigned long dwStreamId);

typedef void (__cdecl *LoggingCallbackFunc)(void *, char *);

typedef void (__cdecl *tDecodeFailureCallback) (void *pUserData,          // for caller's use
                                                unsigned long lChannel,   // channel decode failed on
                                                eVmDecodeResult eReason); // failure code

typedef void (__cdecl *tUserDataCallback) (void *pUserParam,       // for caller's use
                                           int   userdataLength,   // userdata length
                                           void *pUserDataValue);  // userdata pointer

typedef int (__cdecl *tControlWordCallback) (void *pUserParam, descramble_param_t *dscParam); 

typedef void (__cdecl *tFingerPrintingCallback) (void *pUserParam,	// for caller's use
						 int	FingerPrintingdataLength, 	// fingerPrinting data length
						 void	*pFingerPrintingData,				// fingerPrinting data pointer
						 int	localdataLength,						// local data pointer
						 void	*pLocalData);								// local data pointer

typedef void (__cdecl *tCopyControlCallback) (void *pUserParam,		// for caller's use
												 int  dataLength,	// data length
												 void *pData);		// data pointer

typedef void (*tOsdMessageCallback) (void *pUserParam, unsigned int nDataSize, void *pOsdMessage );

typedef void (*tIrdOpaqueDataCallback) (void*pUserParam, unsigned int nDataSize, void *pIrdOpaqueData );

#ifdef __cplusplus
  }
#endif


#endif // _VMCALLBACKTYPES_H_

