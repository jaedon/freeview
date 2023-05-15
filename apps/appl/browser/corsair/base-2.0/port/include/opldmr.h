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

#ifndef __OPLDMR_H__
#define __OPLDMR_H__

#include	"bpltype.h"

//1 #### DMP 에서 올라가는 타입과 동일해야함. 
// TODO: 아래 수정시, opldmp.h 에도 동일하게 수정해 주세요.
#define OPL_DMR_MAX_TITLE			(128)
#define OPL_DMR_MAX_UDN				(48)
#define OPL_DMR_MAX_CID				(512)
#define OPL_DMR_MAX_IP4ADDR			(16)	
#define OPL_DMR_MAX_CLASS			(128)	
#define OPL_DMR_MAX_OBJECTTYPE		(16)	
#define OPL_DMR_MAX_URI				(1024)	
#define OPL_DMR_MAX_DATE			(16)	
#define OPL_DMR_MAX_DURATION		(16)	
#define OPL_DMR_MAX_RESOLUTION		(16)	
#define OPL_DMR_MAX_DIGITALCOPLY	(16)	
#define OPL_DMR_MAX_GENRE			(32)	

typedef enum
{
	eOPL_DMR_MODE_WEB_BASE,
	eOPL_DMR_MODE_OCTO_BASE,
} Opl_DLNA_DmrMode;
	
typedef enum
{
	//eOPL_DMR_REQ_SETURI,
	eOPL_DMR_REQ_PLAY,
	eOPL_DMR_REQ_STOP,
	eOPL_DMR_REQ_PAUSE,
	eOPL_DMR_REQ_SEEK,
	eOPL_DMR_REQ_SETVOLUME,
	eOPL_DMR_REQ_SETMUTE,
	eOPL_DMR_REQ_BRINGIN,
} OplMRCP_Request_e;

//1 #### DMP에서 올라가는 타입과 동일해야함. 
// TODO: 아래 수정시, opldmp.h 에도 동일하게 수정해 주세요.
typedef struct
{
	HCHAR	szAlbumTitle[OPL_DMR_MAX_TITLE];
	HCHAR	szArtist[OPL_DMR_MAX_TITLE];
} Opl_DLNA_DmrContentMeta_t;

typedef	struct
{
	HCHAR	szTitle[OPL_DMR_MAX_TITLE];
	HCHAR   szUDN[OPL_DMR_MAX_UDN];
	HCHAR	szCID[OPL_DMR_MAX_CID];
	HCHAR	szParentId[OPL_DMR_MAX_CID];
	HCHAR	szClass[OPL_DMR_MAX_CLASS];
	HCHAR	szObjectType[OPL_DMR_MAX_OBJECTTYPE];
	HCHAR	szImportURI[OPL_DMR_MAX_URI];
	HCHAR	szThumbnailURI[OPL_DMR_MAX_URI];
	HCHAR	szDate[OPL_DMR_MAX_DATE];
	HCHAR	szDuration[OPL_DMR_MAX_DURATION];
	HCHAR	szResolution[OPL_DMR_MAX_RESOLUTION];
	HCHAR	szDigitalCopyControl[OPL_DMR_MAX_DIGITALCOPLY];
	HCHAR	szGenre[OPL_DMR_MAX_GENRE];
	Opl_DLNA_DmrContentMeta_t	stMetaData;

	HBOOL	bByteSeekAble;
	HBOOL	bTimeSeekAble;
	
	HUINT32 ulChildCount;
	HBOOL	bRestricted;

} Opl_DLNA_DmrContent_t;


/******************************************************************
	Typedef
******************************************************************/
typedef	void	(*OPL_DMR_EventCallback)(unsigned int event, unsigned int size, void* cdsItem);

#ifdef __cplusplus
extern "C" {
#endif

BPLStatus	OPL_DMR_Start( void );
BPLStatus	OPL_DMR_Stop( void );

BPLStatus	OPL_DMR_GetActionStatus(HBOOL *status );
BPLStatus 	OPL_DMR_IsActivated(HBOOL *bEnable);
BPLStatus 	OPL_DMR_GetDmrFriendlyName(HCHAR *friendlyName, int nLen);
BPLStatus 	OPL_DMR_SetDmrFriendlyName(const HCHAR *friendlyName);
BPLStatus 	OPL_DMR_SetBringInUri(const HCHAR *pUrl);
BPLStatus 	OPL_DMR_SetOperationMode(const HINT32 nMode);


BPLStatus	OPL_DMR_RegisterListener( OPL_DMR_EventCallback listener );
BPLStatus	OPL_DMR_UnRegisterListener( void );


#ifdef __cplusplus
};

/******************************************************************
	class 
******************************************************************/



#endif // __cplusplus
#endif // __OPLDMP_H__
