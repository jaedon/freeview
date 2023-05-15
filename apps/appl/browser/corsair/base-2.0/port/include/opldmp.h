/**************************************************************************************/
/**
 * @file opldlna.h
 *
 * dlna/dmp Interfaces
 *
 * @author  Munho Park(mhpar3@humaxdigital.com)
 * @date    2011/12/08
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLDMP_H__
#define __OPLDMP_H__

#include	"bpltype.h"
#include "oplbookmark.h"

#if defined(CONFIG_OP_JAPAN)
#include	"opltype.h"
#include    "oplquery.h"
#include    "oplprogramme.h"
#endif

// TODO: ¾Æ·¡ ¼öÁ¤½Ã, opldmr.h ¿¡µµ µ¿ÀÏÇÏ°Ô ¼öÁ¤ÇØ ÁÖ¼¼¿ä.
#define OPL_DMP_MAX_TITLE		(128)
#define OPL_DMP_MAX_UDN			(48)
#define OPL_DMP_MAX_CID			(512)
#define OPL_DMP_MAX_IP4ADDR		(16)	
#define OPL_DMP_MAX_CLASS		(128)	
#define OPL_DMP_MAX_OBJECTTYPE	(16)	
#define OPL_DMP_MAX_URI			(1024)	
#define OPL_DMP_MAX_DATE		(16)	
#define OPL_DMP_MAX_DURATION		(16)	
#define OPL_DMP_MAX_RESOLUTION		(16)	
#define OPL_DMP_MAX_DIGITALCOPLY		(16)	
#define OPL_DMP_MAX_GENRE		(32)	


#if defined(CONFIG_OP_JAPAN)
#define OPL_DMP_MAX_DESC			(256)
#define OPL_DMP_MAX_LONGDESC		(256* 8)
#define OPL_DMP_MAX_CHANNELNAME     (64)
#define OPL_DMP_MAX_CHANNELNR       (16)
#define OPL_DMP_MAX_RADIOBAND       (128)
#define OPL_DMP_MAX_ANALOGCOPLY     (16)
#define OPL_DMP_MAX_CAP				(16)
#endif

/******************************************************************
	Typedef
******************************************************************/
typedef	void	(OPL_DMP_EventCallback)(int event, int size, void *data);
typedef	void	(OPL_CONTENT_EventCallback)(int event, char *udn, int reqId, int size, void **data, int total);
typedef	void	(OPL_REMOVE_EventCallback)(char *strRemove, int ret);

typedef void * OplDMSDeviceHandle;
typedef void ** OplDLNAContentHandle;

typedef enum
{
	eDMP_SERVER_REMOVE = 0,
	eDMP_SERVER_ADD,
	eDMP_SERVER_SCAN_STARTED,
	eDMP_SERVER_SCAN_COMPLETE,
} OplDMP_SERVER_Update_e;


typedef enum
{
	eDMP_CONTENT_END = 0,
	eDMP_CONTENT_ADD,

	eDMP_RECORD_DESTINATIONS,
	eDMP_RECORD_DISKINFO,

	eDMP_CONTENT_SEARCH_DONE,
	eDMP_CONTENT_SEARCH_ERROR,

	eDMP_CONTENT_BROWSE_ERROR,
} OplDMP_CONTENT_Update_e;

#ifdef __cplusplus
extern "C" {
#endif

BPLStatus	OPL_DMP_Start( void );
BPLStatus	OPL_DMP_Stop( void );
BPLStatus	OPL_DMP_RefreshDms( const HCHAR *pUdn );

BPLStatus   OPL_DMP_GetListOfDMS( unsigned int *nCount, OplDMSDeviceHandle *dmpDevice);
BPLStatus	OPL_DMP_GetCurrentDMS( OplDMSDeviceHandle *identifier );

BPLStatus	OPL_DMP_RegisterListener( OPL_DMP_EventCallback callback );
BPLStatus	OPL_DMP_UnRegisterListener( void );


#if defined(CONFIG_OP_JAPAN)
typedef struct
{
	OPL_HANDLE          search;
	int                 length;
	int                 offset;
	int                 totalSize;
	OPL_Collection_t    *items;
} OPL_JlabsContentSearchResults_t;

typedef struct
{
    HCHAR	szName[OPL_DMP_MAX_TITLE];
    HUINT8  ucParentalRating;
} Opl_ParentalRating_t;

typedef struct
{
	HCHAR   szAlbumTitle[OPL_DMP_MAX_TITLE];
	HCHAR   szArtist[OPL_DMP_MAX_TITLE];
	HCHAR   szAuthor[OPL_DMP_MAX_TITLE];
	HCHAR   szDescription[OPL_DMP_MAX_DESC];
	HCHAR   szLongDescription[OPL_DMP_MAX_LONGDESC];
} Opl_CDSMeta_t;

typedef struct
{
	HUINT32 ullength;
	HUINT32 uloffset;
	HUINT32 ultotalsize;
} Opl_ContentResult_t;

typedef struct 
{
	HCHAR	szId[128];
	HCHAR	szName[128];
} Opl_RecordDestination_t;
typedef void ** OplDLNADestinationHandle;

typedef struct
{
    HUINT64 Total; /* Byte */
    HUINT64 Available; /* Byte */
	HCHAR	*pDstId;
} Opl_DmsStorage_t;

typedef struct
{
	HCHAR   szTitle[OPL_DMP_MAX_TITLE];
	HCHAR	szParentId[OPL_DMP_MAX_CID];
	HCHAR   szUDN[OPL_DMP_MAX_UDN];
	HCHAR   szCID[OPL_DMP_MAX_CID];
	HCHAR   szClass[OPL_DMP_MAX_CLASS];
	HCHAR   szObjectType[OPL_DMP_MAX_OBJECTTYPE];
	HCHAR   szImportURI[OPL_DMP_MAX_URI];
	HCHAR   szThumbnailURI[OPL_DMP_MAX_URI];
	HCHAR   szDate[OPL_DMP_MAX_TITLE];
	HCHAR   szDuration[OPL_DMP_MAX_DURATION];
	HCHAR   szResolution[OPL_DMP_MAX_RESOLUTION];
	HCHAR   szDigitalCopyControl[OPL_DMP_MAX_TITLE];
	HCHAR   szGenre[OPL_DMP_MAX_GENRE];
	HCHAR   szProfile[OPL_DMP_MAX_TITLE];

	HCHAR   szChannelName[OPL_DMP_MAX_TITLE];
	HCHAR   szChannelNr[OPL_DMP_MAX_TITLE];
	HCHAR   szRadioBand[OPL_DMP_MAX_TITLE];
	HCHAR   szAnalogCopyControl[OPL_DMP_MAX_TITLE];

	HBOOL	bByteSeekAble;
	HBOOL	bTimeSeekAble;
	
	HUINT32 ulChildCount;
	HUINT32 ulPRNum;
	HBOOL   bRestricted;

	Opl_CDSMeta_t   	stMetaData;
	OPL_Collection_t*   bookmarks;
	Opl_ParentalRating_t stParentalRating[3];

	HUINT32 ulTotalMatches;
} Opl_CDSRecording_t;

typedef Opl_CDSRecording_t  OplDLNAContent;
typedef Opl_CDSMeta_t       OplDLNAContentMeta;
typedef Opl_ContentResult_t OplDLNAContentResult;
typedef Opl_RecordDestination_t	OplDLNADestination;
typedef Opl_ParentalRating_t	OplDLNAParentalRating;
typedef Opl_DmsStorage_t		OplDLNADmsStorage;

OPL_Bookmark_t*     OPL_CDSRecording_Bookmark_New (unsigned long   time, const char *name);
void                OPL_CDSRecording_Bookmark_Delete (OPL_Bookmark_t *self);
OPL_Bookmark_t *    OPL_CDSRecording_Bookmark_Clone (const OPL_Bookmark_t *self);
BPLStatus			OPL_REMOVE_RegisterListener( OPL_REMOVE_EventCallback callback );
BPLStatus			OPL_REMOVE_UnRegisterListener( void );
void				OPL_CDSRecording_Remove (Opl_CDSRecording_t *item, char **retVal);

#else

// TODO: ¾Æ·¡ ¼öÁ¤½Ã, opldmr.h ¿¡µµ µ¿ÀÏÇÏ°Ô ¼öÁ¤ÇØ ÁÖ¼¼¿ä.
typedef struct
{
	HCHAR	szAlbumTitle[OPL_DMP_MAX_TITLE];
	HCHAR	szArtist[OPL_DMP_MAX_TITLE];
} Opl_DLNAContentMeta_t;

typedef	struct
{
	HCHAR	szTitle[OPL_DMP_MAX_TITLE];
	HCHAR   szUDN[OPL_DMP_MAX_UDN];
	HCHAR	szCID[OPL_DMP_MAX_CID];
	HCHAR	szParentId[OPL_DMP_MAX_CID];
	HCHAR	szClass[OPL_DMP_MAX_CLASS];
	HCHAR	szObjectType[OPL_DMP_MAX_OBJECTTYPE];
	HCHAR	szImportURI[OPL_DMP_MAX_URI];
	HCHAR	szThumbnailURI[OPL_DMP_MAX_URI];
	HCHAR	szDate[OPL_DMP_MAX_DATE];
	HCHAR	szDuration[OPL_DMP_MAX_DURATION];
	HCHAR	szResolution[OPL_DMP_MAX_RESOLUTION];
	HCHAR	szDigitalCopyControl[OPL_DMP_MAX_DIGITALCOPLY];
	HCHAR	szGenre[OPL_DMP_MAX_GENRE];
	Opl_DLNAContentMeta_t	stMetaData;

	HBOOL	bByteSeekAble;
	HBOOL	bTimeSeekAble;
	
	HUINT32 ulChildCount;
	HBOOL	bRestricted;

} Opl_DLNAContent_t;

typedef Opl_DLNAContent_t OplDLNAContent;
typedef Opl_DLNAContentMeta_t OplDLNAContentMeta;
#endif

#ifdef __cplusplus
};

/******************************************************************
	class OplDMSDevice
******************************************************************/
class OplDMSDevice
{
private :

public :
	OplDMSDevice(OplDMSDeviceHandle Handle);
	OplDMSDevice( const OplDMSDevice &dms );
	virtual ~OplDMSDevice();
	
	static	OplDMSDevice *m_sCurrentDMS; 	
	static	char 		 *m_sCurrentCID; 	
	static  int	m_nReqIdCnt;

#if defined(CONFIG_OP_JAPAN)
	static 	char 	*m_szCurrentDestinationId;
#endif
	static	void onEventNotifier( HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 );
	static	void	registerEventListener( OPL_CONTENT_EventCallback callback );
	static	void	unRegisterEventListener( void );
	BPLStatus	browsingChildItem( const char *cid, const char *sort, int nStart , int nRequest);
	int		searchingItem( const char *cid, const char *category, const char *keyword,  const char *sort, int nStart , int nRequest);

	HCHAR	*getUDN()			{ 	return m_szUDN; 			}
	HCHAR	*getFriendlyName()	{ 	return m_szFriendlyName;	}
	HCHAR	*getIpAddress()		{ 	return m_szIpAddr;			}

#if defined(CONFIG_OP_JAPAN)
	HUINT32 getDeviceHandle()   {   return ulDeviceHandle;      }
	HCHAR   *getMACAddress()    {   return m_szMAC;             }
	HCHAR   *getTotalSize()     {   return m_szTotalSize;       }

	HCHAR   **getSortCAP(int *p)	{   *p = nSortCap; 		return m_szSortCAP;         }
	HCHAR   **getSearchCAP(int *p)	{   *p = nSearchCap; 	return m_szSearchCAP;		}
	HCHAR   **getDlnaCAP(int *p)	{   *p = nDlanCap; 		return m_szDlnaCAP;         }
	HCHAR   **getJlabsCAP(int *p)	{   *p = nJlabsCap; 	return m_szJlabsCAP;        }

	BPLStatus	getRecordDestination( const char *udn);
	BPLStatus	getAvailableStorageSize( const char *dst );
#endif

	// Event Callback Listë¡?ë§Œë“¤?´ì„œ ê´€ë¦¬í•´?¼í•¨.
	static  OPL_CONTENT_EventCallback	*s_pfn_contentSearch_callback;

	char	*m_szUDN;
	char	*m_szFriendlyName;
	char	*m_szIpAddr;

#if defined(CONFIG_OP_JAPAN)
	HBOOL			bIsSupportSrs;
	unsigned int    ulDeviceHandle;
	char    *m_szMAC;
	char    *m_szTotalSize;

	char    *m_szSortCAP[OPL_DMP_MAX_CAP];
	char    *m_szSearchCAP[OPL_DMP_MAX_CAP];
	char    *m_szDlnaCAP[OPL_DMP_MAX_CAP];
	char    *m_szJlabsCAP[OPL_DMP_MAX_CAP];

	int     nSortCap;
	int     nSearchCap;
	int     nDlanCap;
	int     nJlabsCap;
#endif
};


#endif // __cplusplus
#endif // __OPLDMP_H__
