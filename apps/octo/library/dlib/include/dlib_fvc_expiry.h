/* $Header: $ */

#ifndef DLIB_FVC_EXPIRY_H__
#define DLIB_FVC_EXPIRY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

#define FVC_EXPIRY_URL_LENGTH		(256)
#define FVC_EXPIRY_DATE_LENGTH		(64)
#define	DxFVC_UID(onid, startTime)	(HUID)(((HUID)onid) << 32 | startTime)

#define DxFVC_SERVICE_UID 		(-2)	// -1은 DAMA_TABLE_RESET_UID와 걸리므로..?

typedef enum
{
	eDxFvcExpiry_Type_Schedule			= 0,
	eDxFvcExpiry_Type_Service			= 1,
	eDxFvcExpiry_Type_ApplicationList	= 2,
	eDxFvcExpiry_Type_Browse			= 3,
	eDxFvcExpiry_Type_EndOfCase
} DxFvcExpiry_Type_e;

typedef struct
{
	HUID				uid;
	HUINT32				onid;
	HUINT32				startTime;
	HUINT32				expiryTime;
	HCHAR				lastModifiedTime[FVC_EXPIRY_DATE_LENGTH];
	HUINT32				statusCode;			// HTTP status (200 OK ...)
	HUINT32				queryStampTime;		// HUMAX Server와 sync가 안 맞을 시에 사용
	DxFvcExpiry_Type_e	type;
} DxFvcExpiry_t;


#ifdef __cplusplus
}
#endif

#endif


