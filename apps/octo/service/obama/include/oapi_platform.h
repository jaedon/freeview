
#ifndef	__OAPI_PLATFORM_H__
#define	__OAPI_PLATFORM_H__


#include <hlib.h>

#ifdef  __cplusplus
extern "C" {
#endif

#if	0		//	빠른 진행시 SemLock걸림 , DaMa와 같은 이슈.
/**
 * size of data block
 */
extern HINT32	OAPI_PLATFORM_DATA_CryptBufferSize(void);

/**
 * Encrypt data block
 */
extern HINT32	OAPI_PLATFORM_DATA_Encrypt(HUINT32 ulBlockNo, HUINT32 ulSize, HUINT8 *pucData, HxReturnFunc_t ret, void *userdata);

/**
 * Decrypt data block
 */
extern HINT32	OAPI_PLATFORM_DATA_Decrypt(HUINT32 ulBlockNo, HUINT32 ulSize, HUINT8 *pucData, HxReturnFunc_t ret, void *userdata);
#endif

/**
 * Extract Thumbnail from TS
 */
extern void	OAPI_PLATFORM_VIDEO_ExtractFrame(HCHAR *pszUrl, HUINT32 ulTime, HINT32 nWidth, HINT32 nHeight, HUINT16 usVideoPid, HUINT32 ulVideoCodec, HBOOL bEncrypt, HCHAR *pszOutFile);

extern HINT32	OAPI_PLATFORM_DATA_Decrypt(HUINT32 ulBlockNo, HUINT8 *pucData,HUINT32 ulSize , HxRPC_Response_t pFunc,void *userdata);
extern void	OAPI_PLATFORM_KeyInit(void);
extern HERROR	OAPI_PLATFORM_GetKey(HUINT8 *pucKey);

#ifdef  __cplusplus
}
#endif

#endif /* __OAPI_PLATFORM_H__ */

