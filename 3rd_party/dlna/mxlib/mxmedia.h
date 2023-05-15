#ifndef MX_MEDIA__H_
#define MX_MEDIA__H_

#include "mxapi_define.h"

#ifdef __cplusplus
extern "C" 
{
#endif

#include "mxmedia_define.h"
#include "mxmedia_profile_define.h"
#include "mxdetect_define.h"

#include "mxfile_http.h"
#include "mxfile.h"	
#include "mxdetect_ref.h"

//*************************************************************************
// 기본 디텍션에 관련 함수 및 유틸..
//*************************************************************************
// 정확한 mime type 얻어오는 함수  : protocolInfo와 media_format을 인자로 넘긴다.
__MXAPI MX_INT32 mxMedia_getExactMediaformat( 
	const MX_INT8	 *	protocolInfo, 
	MX_INT32	media_type, 
	MX_INT32 *	media_format,
	MX_INT32 *	media_info 
);

// 각종 타입의 변환
__MXAPI MX_INT32 mxMedia_getExtensionByMediaformat( 
	MX_INT32 *mf , 
	MXMULTI_RET * ext 
);


__MXAPI MX_INT32 mxMedia_getMediaformatByOrgPN( 
	MX_INT8 * org_pn , 
	MX_INT32  media_type, 
	MX_INT32 * media_format 
);

// protocol 정보를 파싱하는 함수.
__MXAPI MX_INT8 * mxMedia_getProtocolValue( 
	MX_INT8 *s , 
	MX_INT8 *find 
);

// Mime Type 으로 미디어 포멧얻어오기
__MXAPI MX_INT8 * mxMedia_getMimetypeByExtension( 
	MX_INT8 *ext 
);


// mime으로 extension 얻기
__MXAPI MX_INT8 * mxMedia_getExtensionByMimetype( 
	MX_INT8 *mimeType 
);



// protocolInfo로 MimeType얻어오기.
__MXAPI MX_INT8 * mxMedia_getMimeTypeByProtocolInfo( 
	MX_INT8 * protocolInfo 
);


// 미디어 포멧으로 mimeType얻어오기
__MXAPI MX_INT8 * mxMedia_getMimetypeByMediaFormat( 
	int media_format
);


		// Custom에서 정의한 함수들.
//MXPN_PRI_MAP * mxRes_getProfilePriTable( MX_INT32 media_format )
//MX_INT32 mxMedia_isAvailablePlay(
//	MX_INT32 format ,
//	char * mime
//);

//*************************************************************************




	
//*************************************************************************
// media Detect funcions..
//*************************************************************************
// mandatory
__MXAPI MX_INT32 mxMedia_getInfoByMime(						// mediaformat , mediainfo		 
	MX_INT8 * pMimeStr , MXMEDIA_INFO * pDetect 
);

__MXAPI MX_INT32 mxMedia_getInfoByHttp(						// mediaformat , mediainfo
	MXFILE_DESC * pDesc, MXMEDIA_INFO * pDetect 
);

__MXAPI MX_INT32 mxMedia_getInfoByProtocolInfo(				// mediaformat , mediainfo
	MX_INT8 * protocolInfo  , MXMEDIA_INFO * pDetect 
);


__MXAPI MX_INT32 mxMedia_getInfoByExtension(				// 특별한 코덱의 경우 정확도가 떨어진다.
	MX_INT8 * pExtension , MXMEDIA_INFO * pDetect 
);


// optional
__MXAPI MX_INT32 mxMedia_getInfoByDuration( 
	MX_INT32 sec , MXMEDIA_INFO * pDetect 
);
//*************************************************************************






	
#define MXDLNA_DETECT_CMD_DLNA			(0xa1)
#define MXDLNA_DETECT_CMD_ACCESS		(0xa2)





//*************************************************************************
// Codec Detect funcions..
//*************************************************************************
//----------------------------------------------------------------------//
__MXAPI MX_INT32 mxMedia_getCodecInfo
//----------------------------------------------------------------------//
			(
				MXFILE_DESC		 *	pAccDesc ,	// Access Descriptor
				MXMEDIA_INFO	 *	pMediaInfo,	// Media Detect's Output
				void			 *	pOption,	// reserved parameter .. Not Yet Use..
				MX_INT32			command,	// command
				MXCODEC_INFO	 *	pCodecInfo	// this function's Output parameter..
			);




//*************************************************************************
// Meta Detect funcions..
//*************************************************************************
//----------------------------------------------------------------------//
__MXAPI MX_INT32 mxMedia_getMetaInfo
//----------------------------------------------------------------------//
			(
				MXFILE_DESC		 *	pAccDesc ,	// Access Descriptor
				MXMEDIA_INFO	 *	pMediaInfo,	// Media Detect's Output				
				void			 *	pOption,	// reserved parameter .. Not Yet Use..
				MX_INT32			command,	// command
				MXMETA_INFO		 *	pMetaInfo	// this function's Output parameter..
			);

__MXAPI MX_INT32 mxMedia_freeMetaInfo( MXMETA_INFO * pMetaInfo );

__MXAPI MX_INT8 * mxDetect_getTitleStringByCmd( MX_INT32 media_format , MX_INT32 titleCmd );
//MX_INT32 mxMedia_initMetaInfo( MXMETA_INFO * pMetaInfo );


//*************************************************************************
// PN Detect funcions..
//*************************************************************************
#define MXDLNA_DETECT_CMD_MIME			(0xa1)
//----------------------------------------------------------------------//
__MXAPI MX_INT32 mxMedia_getPNInfo
//----------------------------------------------------------------------//
	(
			MXMEDIA_INFO	*	pMediaInfo,	// Media Detect's Output value
			MXCODEC_INFO	*	pCodecInfo,	// this function's input parameter..
			void			*	pOption,	// reserved parameter .. Not Yet use..
			MX_INT32			command,	// command .... Not Yet use...
			MX_INT32		*	pPNValue	// this function's output parameter..
	);


//----------------------------------------------------------------------//
__MXAPI MX_INT8 * mxMedia_getPNStr
//----------------------------------------------------------------------//
	( 
		MX_INT32 media_format , 
		MX_INT32 pnValue 
	);


// 20101220 More Detect	
void mxMedia_printCodecInfo(char * filename, MXCODEC_INFO * pCodecInfo );


typedef MX_UINT32 (*FN_SAVE_PROTOCOL_INFO_CALLBACK) ( MX_INT8 * inpProtocolInfo , MX_INT32 media_type );
typedef void *(*FN_GET_PROFILE_PRITABLE_CALLBACK)(MX_INT32 media_type );
void mxMedia_setDetectResourceCallback(FN_SAVE_PROTOCOL_INFO_CALLBACK fn, FN_GET_PROFILE_PRITABLE_CALLBACK fn2);

	
#ifdef __cplusplus
}
#endif

#endif










