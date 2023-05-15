

#ifndef MXDTCP_0404042
#define MXDTCP_0404042

#ifdef __cplusplus
extern "C"{
#endif


#include "config.h"

int mxDtcp_Start( void );
int mxDtcp_SetSecureData( char *data, int dataSize );
int mxDtcp_Listen(const char *aSourceIp, unsigned short aSourcePort);
int mxDtcp_CloseListen( void );
const char *mxDtcp_GetSourceIp( void );
unsigned short mxDtcp_GetSourcePort( void );


#ifdef DTCP_MOVE_SUPPORT

int mxDtcp_WaitAKE( long ip );
int mxDtcp_WaitStopAKE( long ip );
int mxDtcp_IsMoveMode( long ip );
int mxDtcp_WaitAKECommit( long ip );

#endif


typedef enum
{
    dtcpEmiCopyNever = 12,           ///< Copy Never
    dtcpEmiCopyOneGenFormatCog=10,   ///< Copy One Generation - Format cognizant recording only
    dtcpEmiCopyOneGenFormatNonCog=8, ///< Copy One Generation - Format non cognizant recording permitted
    dtcpEmiMove=6,                   ///< Move
    dtcpEmiNoMoreCopies=4,           ///< No More Copies
    dtcpEmiCopyFreeEpnAsserted=2,    ///< Copy Free with EPN asserted
    dtcpEmiCopyFree=0                ///< Copy Free
} DtcpEmi;

#ifdef DTCP_HARDENING_PRVKEY_SECUREDATA
typedef void (*FN_DTCP_EC_DSA)	(unsigned char *pSignature,   unsigned char *pBuffer,  unsigned int bufferLengthBytes);
#endif
typedef void (*FN_DTCP_MEMFREE)( void *pMemory );
typedef void * (*FN_DTCP_MEMALLOC)( unsigned int nSize );
typedef int (*FN_DTCP_ENCRYPT)( unsigned char *pMainKey, unsigned char *pSharedKey, unsigned int ulKeySize, unsigned char *pSrcBuffer, unsigned char *pDstBuffer, unsigned int ulBuffSize );
typedef int (*FN_DTCP_DECRYPT)( unsigned char *pMainKey, unsigned char *pSharedKey, unsigned int ulKeySize, unsigned char *pSrcBuffer, unsigned char *pDstBuffer, unsigned int ulBuffSize );

typedef int (*FN_DTCP_ENCRYPT_AES_KEY)(char *pcOriginal_key, char *pcScramble_key, int nKey_size);


typedef struct _MXDTCP_CALLBACK
{
#ifdef DTCP_HARDENING_PRVKEY_SECUREDATA
			 FN_DTCP_EC_DSA fnEcDda;
#endif
             FN_DTCP_MEMFREE fnMemFree;
             FN_DTCP_MEMALLOC fnMemAlloc;
             FN_DTCP_ENCRYPT fnEncrypt;
             FN_DTCP_DECRYPT fnDecrypt;
			 FN_DTCP_ENCRYPT_AES_KEY	fnEncryptAesKey;
} MXDTCP_CALLBACK;

void mxDtcp_SetCallback( MXDTCP_CALLBACK *callback );
#ifdef DTCP_HARDENING_PRVKEY_SECUREDATA
void mxDtcp_SetEcDsaCallback(FN_DTCP_EC_DSA callback );
int mxDtcp_EcDsa(unsigned char *pSignature,   unsigned char *pBuffer,  unsigned int bufferLengthBytes);
int mxDtcp_SetDeviceCert( char *data, int dataSize );
#endif
int mxDtcp_SetSrmPath(unsigned char *pPath);


#define MXDTCP_IS_SUCCESS(ReturnCode) (0 <= (ReturnCode))
#define MXDTCP_IS_FAILURE(ReturnCode) (0 >  (ReturnCode))

#define MXDTCP_SUCCESS                             0   ///< Function succeeded
#define MXDTCP_SUCCESS_FALSE                       1

#define MXDTCP_FAILURE                            -1   ///< General failure
#define MXDTCP_INVALID_ARGUMENT                   -2   ///< Invalid function argument
#define MXDTCP_LOGIC_ERROR                        -3   ///< Functional logic error
#define MXDTCP_FILE_NOT_FOUND                     -4   ///< File not found




void mxlib_SHA1(
	 unsigned char *digest,           /* OUT: 160 bit digest */
	 unsigned char *pBuffer,           /* IN: Buffer pointer */
	 unsigned int bufferLengthBytes  /* IN: Buffer length */
	 );


#ifdef __cplusplus
}
#endif


#endif

