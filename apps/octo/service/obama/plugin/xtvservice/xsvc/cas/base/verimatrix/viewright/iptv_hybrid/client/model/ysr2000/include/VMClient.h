#ifndef WIN32
#define __cdecl
#endif

#define LIB_API

#include <stdint.h>
#include "VMCallbackTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------- Common Functions ----------
LIB_API int  __cdecl VMConfig( const char * pStorePath, const char *pIpAddress, 
        unsigned short nPort, unsigned int nTimeout, unsigned int nRetries, 
        unsigned short nIpVersionPreferred );

LIB_API int __cdecl VMConfigOffline( const char *pStoreFilePath );

LIB_API void * __cdecl VMCreateContext( const char * pINIFileName
                                           , int bUseIniFile
                                           );
LIB_API int   __cdecl VMResetStream(void *pContext);
LIB_API int   __cdecl VMDecryptStreamData( void *pContext
                                              , unsigned char *pBuffer
                                              , int iBufferLength
                                              , int * piBytesProcessed
                                              );
LIB_API void  __cdecl VMLogVersion(void);

#define VMGetCASystemID(x)   0x5601

LIB_API int __cdecl VMSetECM( void * pContext, 
                              int isSection, 
							  int pid,
                              unsigned char *pBuffer, 
                              int iBufferLength);

LIB_API int   __cdecl VMUpdateChannelKeys(void *pContext);
LIB_API int   __cdecl VMSetCallback( void *pContext
                                        , RenderFunc fnCallback
                                        , unsigned long size
                                        , void * pArg
                                        );
LIB_API void __cdecl VMRegisterDecodeFailureCallback( void *pContext
                         , tDecodeFailureCallback pDecodeFailureCallback
                         , void * pUserParam
                         );
LIB_API void  __cdecl VMFlushBuffer( void *pContext, int bTriggerCallback );

LIB_API int __cdecl VMDestroyClient(void); // 원래는 LIB_API int __cdecl VMDestroyClient() -> LIB_API int __cdecl VMDestroyClient(void) 로 변경함. 컴파일 waring으로 인해서.

// ---------- Special Functions ----------
LIB_API const char * __cdecl VMGetVersionString(void);
LIB_API int   __cdecl VMRequestVODKey(void *pContext,  int iMovieID );
LIB_API int   __cdecl VMRemoveVodKey( void * pContext, int iMovieID );
LIB_API int   __cdecl VMPurgeVodKeyCache( void * pContext );
LIB_API int   __cdecl VMPurgeLastKeyCache( void * pContext );
LIB_API int   __cdecl VMPurgePvrKeyCache( void * pContext );
LIB_API int   __cdecl VMGetKeyUpdateWindow(void *pContext
                                               , int *piMinTime
                                               , int *piMaxTime
                                               , int *piSuggestedTime
                                               );
LIB_API void  __cdecl VMSetStoreReadCallback( DataReadFunc fnReader
                                                 , void * pArg 
                                                 );
LIB_API void  __cdecl VMSetStoreWriteCallback( DataWriteFunc fnWriter
                                                  , void * pArg 
                                                  );
LIB_API void  __cdecl VMSetRatingLevel( void *pContext
                                      , unsigned char ucRatingLevel );

LIB_API  unsigned char  __cdecl VMOverrideRatingLevel( void *pContext); // huamdks TODO_VMX : void->nsigned char 로 변경함. 컴파일 문제로인해.

LIB_API unsigned char __cdecl VMGetRatingLevel( void *pContext 
	                                           , unsigned char *pbOverride 
											   , unsigned char *pucRatingLevel);

LIB_API void  __cdecl VMRegisterUserdataCallback ( void *pContext
                                        , tUserDataCallback pUserDataFunction
                                        , void *pUserParam
                                        );

LIB_API int   __cdecl VMLogMessage( int          iLevel
                                       , const char * pMsg 
                                       );
LIB_API void   __cdecl VMDestroyContext(void *pContext);

LIB_API int	__cdecl VMRegisterFingerPrintingCallback ( void *pContext
                                        , tFingerPrintingCallback pFingerPrintingFunction
                                        , void *pUserParam
                                        );
LIB_API int	__cdecl VMRegisterCopyControlCallback ( void *pContext
                                        , tCopyControlCallback pCopyControlFunction
                                        , void *pUserParam
                                        );
LIB_API int	__cdecl VMRegisterOsdMessageCallback ( tOsdMessageCallback pCallbackFunction, void *pUserParam );

LIB_API int __cdecl VMSetEMM( unsigned char **pTSPacketBuffer, unsigned int *nNumTSPackets );

LIB_API int __cdecl VMRegisterIrdOpaqueCallback( tIrdOpaqueDataCallback pCallbackFunction, void *pUserParam );


// ---------- INI Parameter Functions ----------
LIB_API void  __cdecl VMSetErrorLevel(void *pContext, int iErrLevel);

LIB_API int __cdecl VMSetMiddlewareAuthenticator( const unsigned char * pData
                                , unsigned int nLength 
                                );

LIB_API int VMSetVcasCommunicationProtocol( eVcasVersion_t eVersion );


LIB_API int __cdecl VMRegisterControlWordCallback( void *pContext
                                    , tControlWordCallback pControlWordCallback
                                    , void *pUserParam
                                    , int nAlgorithmCount
                                    , int *nAlgorithmList);

//-------------------------------------------------------------------
// Function:    GetUniqueIdentifier
//
// Purpose:     Returns the ID string used by the client to identify
//              itself to the VCAS server.
//
// Parameters:  buffer = buffer into which to copy ID
//              maxlen = size of the buffer
//              len    = used to return length of the ID
//
// Returns:     0 on Success.
//              VMCLIENT_ERR_BADARG on NULL len.
//              VMCLIENT_ERR_SIZE   if provided buffer is too small or NULL.
//                                  Required size returned in len.
//              VMCLIENT_ERR_NOBUF  if failed to get ID.
//-------------------------------------------------------------------
LIB_API int __cdecl GetUniqueIdentifier( char * buffer
                                       , unsigned int maxlen
                                       , unsigned int * len );


#define VMX_PUBAUTH_MAXIDLEN         128
#define VMX_PUBAUTH_AUTHENTICATORLEN 256

LIB_API int  VMServiceStart(void *pContext, uint8_t bServiceIdx, uint8_t bPidCount, unsigned short *pawStreamPid);
LIB_API void  VMServiceStop(void *pContext, uint8_t bServiceIdx);

LIB_API int  __cdecl VMSetTZKeyIndex ( int iKeyIndex1, int iKeyIndex2 );

#define VMCLIENT_ERR_NOCONTEXT       -1
#define VMCLIENT_ERR_CATCH           -2
#define VMCLIENT_ERR_FEATUREDISABLED -3
#define VMCLIENT_ERR_SIZE            -4
#define VMCLIENT_ERR_BADARG          -5
#define VMCLIENT_ERR_NOBUF           -6
#define VMCLIENT_ERR_UNSUPPORTED     -7

#define VMFMT_VCASDEFAULT        0
#define VMFMT_VCAS1153SB109      1
#define VMFMT_VCAS1153           1
#define VMFMT_VCAS1154           2
#define VMFMT_VCAS1155           3
#define VMFMT_VCAS1156           4
#define VMFMT_VCAS1157           5

// Values for the iInfoRequested parameter of the VMGetInfo function
#define REQINFO_NOTHING             0
#define REQINFO_VERSIONSTRING       1
#define REQINFO_INIFILENAME         2
#define REQINFO_OPERATORID          3
#define REQINFO_AUTHORIZEDSERVICES  4
#define REQINFO_ISSERVICEAUTHORIZED 5
#define REQINFO_RATINGLEVEL         6
#define REQINFO_CURRENTSTREAM       7
#define REQINFO_KEYUPDATEWINDOW     8


#ifdef __cplusplus
}
#endif



