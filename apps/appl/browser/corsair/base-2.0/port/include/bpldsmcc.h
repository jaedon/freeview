/**************************************************************************************/
/**
 * @file bpldsmcc.h
 *
 * DSM-CC Interfaces
 *
 **************************************************************************************
 **/

#ifndef __BPL_DSMCC_H__
#define __BPL_DSMCC_H__

#include "bpltype.h"

/******************************************************************
	Define
******************************************************************/
#define BPL_DSMCC_STREAMEVENT_CAROUSELID	0x00000000

/******************************************************************
	Enum
 ******************************************************************/
typedef enum {
	/** dsmcc information(ex PMT) was updated */
	BPLDSMCC_EVENT_SESSIONID_UPDATED = 0,

	/** dsmcc information(ex PMT) was updated */
	BPLDSMCC_EVENT_SESSIONDATA_UPDATED,

	/** you get the PID value with this event */
	BPLDSMCC_EVENT_DOWNLOADED,

	/** you get the PID value with this event */
	BPLDSMCC_EVENT_DOWNLOAD_FAIL,

	/** the version of downloaded dsmcc is updated */
	BPLDSMCC_EVENT_VERSION_CHANGED,

	/** you get the BplDsmccStreamEvent_t with this event */
	BPLDSMCC_EVENT_STREAM_EVENT_MESSAGE_UPDATED,
	
	/** you get stream event descriptor from dsmcc */
	BPLDSMCC_EVENT_STREAM_EVENT_DESCRIPTOR_UPDATED,

} eBplDsmccDownloadEvent;

/******************************************************************
	Typedef
******************************************************************/
typedef void BplDsmcc_t;

typedef struct _BplDsmccStreamEvent_t {
	unsigned int    ulPid;
	char            *szUrl;
	unsigned int    ulDescLen;
	char            *szDescription;
	char            *szEventName;
	unsigned int    ulObjInfoByteLen;
	char            *pucObjInfoByte;
	unsigned short  nEventId;
	unsigned short  usTag;
} BplDsmccStreamEvent_t;

typedef struct _BplDsmccES_t
{
	/* component tag of this  pid */
	unsigned char	ucComponentTag;
	/* indicate the Objectcarousel Stream type */
	unsigned int	ulStreamType;
}BplDsmccES_t;

typedef struct _BplDsmccObjectCarousel_t
{
	/* component tag of this  pid */
	BplDsmccES_t	*Es;

	/* count of component tag */
	unsigned int	uiEsCount;

	/* Carousel ID of this pid */
	unsigned int	ulCarouselId;
} BplDsmccObjectCarousel_t;

typedef struct _BplDsmccObjectCarouselList_t
{
	unsigned int				ulCarouselCount;
	BplDsmccObjectCarousel_t	*pstItem;
} BplDsmccObjectCarouselList_t;

typedef struct _BplDsmccService_t{
	unsigned short	usOnID;
	unsigned short	usTSID;
	unsigned short	usSID;
	void	*chHandle;
}BplDsmccService_t;

typedef void (*BplDsmccListener_t)(eBplDsmccDownloadEvent eEvent, void *data);

/******************************************************************
	Interfaces
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

void BPL_Dsmcc_Init(void);

void BPL_Dsmcc_Uninit(void);

BPLStatus BPL_Dsmcc_SetListener(BplDsmccListener_t listener);

/* Listener 등록 후 필요한 Event를 받을 수 있다. */
BPLStatus BPL_Dsmcc_Open(unsigned int ulSessionId);

/* Download 중인 Carousel Stop, Event Cache 삭제, Event도 발생시키지 않는다.. */
BPLStatus BPL_Dsmcc_Close(void);

/* Down 시작을 위한 Caoursel Id와 caching path를 지정 */
BPLStatus BPL_Dsmcc_Down( unsigned int uiCarouselId, char *szCachePath );

/* Down 정지 */
BPLStatus BPL_Dsmcc_Cancel( unsigned int uiCarouselId );

/* 특정 Carousel Down 재시작 */
BPLStatus BPL_Dsmcc_Restart( unsigned int uiCarouselId );

/* Stream Event 시작 */
BPLStatus BPL_Dsmcc_RequestStreamEventStart(void);

/* Stream Event 종료 */
BPLStatus BPL_Dsmcc_RequestStreamEventStop(void);

/* default path를 얻기위한 함수, 모듈 초기화 시 한번만 호출 */
BPLStatus BPL_Dsmcc_GetCachePath( char *pszPath, unsigned int uiBufferLength );

/* PMT의 Carousel정볼르 얻기위한 함수 */
BPLStatus BPL_Dsmcc_GetCarouselList(BplDsmccObjectCarousel_t **pastList, unsigned int *pulCount);

BPLStatus BPL_Dsmcc_FreeCarouselList(BplDsmccObjectCarousel_t *astList, unsigned int ulCount);

BPLStatus BPL_Dsmcc_GetServiceInfo( BplDsmccService_t *pService );
#ifdef __cplusplus
};

/******************************************************************
	class BplDsmcc
******************************************************************/
class BplDsmcc {
public:
	BplDsmcc() {}
	virtual ~BplDsmcc() {}

	static void init()
	{ BPL_Dsmcc_Init(); }

	static void close()
	{ BPL_Dsmcc_Uninit(); }

	static bool setListener( BplDsmccListener_t listener )
	{ return BPL_Dsmcc_SetListener(listener) == BPL_STATUS_OK; }

	static bool open_session( unsigned int ulSessionId )
	{ return BPL_Dsmcc_Open(ulSessionId) == BPL_STATUS_OK; }

	static bool close_session()
	{ return BPL_Dsmcc_Close() == BPL_STATUS_OK; }

	static bool down( unsigned int uiCarouselId, char *szPathToStore )
	{ return BPL_Dsmcc_Down( uiCarouselId, szPathToStore ) == BPL_STATUS_OK; }

	static bool cancel( unsigned int uiCarouselId )
	{ return BPL_Dsmcc_Cancel( uiCarouselId ) == BPL_STATUS_OK; }

	static bool restart( unsigned int uiCarouselId )
	{ return BPL_Dsmcc_Restart( uiCarouselId ) == BPL_STATUS_OK; }

	static bool requestEvent()
	{ return BPL_Dsmcc_RequestStreamEventStart() == BPL_STATUS_OK; }

	static bool getCachePath( char *pszPath, unsigned int uiBufferLength )
	{ return BPL_Dsmcc_GetCachePath( pszPath, uiBufferLength ) == BPL_STATUS_OK; }

	bool getCarouselList( BplDsmccObjectCarousel_t **pptList, unsigned int *pulCount )
	{ return BPL_Dsmcc_GetCarouselList( pptList, pulCount ) == BPL_STATUS_OK; }
};
#endif

#endif
