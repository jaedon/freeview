/*************************************************************************************************************
	File            : pl_userdata.h
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#define	________________Header_Files_______________________________
#include <string.h>
#include <hlib.h>

#include "mxdlna_struct.h"
#include "mxres_ref.h"
#include "mxutil.h"
#include "mxmedia.h"
#include "mxres.h"
#include "mxdlna_mscp.h"
#include "mxdlna_define.h"


#include "hlib.h"
#include "vkernel.h"

#include "common.h"

#define ________________Defines____________________________________
#define DLNA_RES_SIZE       				12
#define MEDIA_URL_MIN_SIZE                  (512)

#define PAL_DLNA_MAX_RESOLUTION				( 3300*2500 )
#define PAL_DLNA_MAX_WIDTH					( 8000 )
#define PAL_DLNA_MAX_HEIGHT					( 8000 )

#define CxDLNA_isContainer(x) ((x == MXDLNA_CDSOBJECT_CONTAINER) ? 1 : 0)
#define CxDLNA_isItem(x) (((x == MXDLNA_CDSOBJECT_ITEM ) ? 1 : 0)

#define SUPPORT_CHECK_THUMBNAIL_RESOLUTION	( 0 )

#define ________________Internal_Functions_________________________
HINT32 __is_folder_type( HINT32 majorClass , HINT32 minorClass )
{
	HINT32 retCode = -1;

	switch ( majorClass )
	{

		case MXDLNA_CDSOBJECT_MAJOR_GENRE :
		case MXDLNA_CDSOBJECT_MAJOR_ALBUM:
		case MXDLNA_CDSOBJECT_MAJOR_AUDIOITEM :
			retCode = FolderType_Music;
			goto endFunction;

		case MXDLNA_CDSOBJECT_MAJOR_VIDEOITEM :
			retCode = FolderType_Video;
			goto endFunction;

		case MXDLNA_CDSOBJECT_MAJOR_IMAGEITEM :
			retCode = FolderType_Image;
			goto endFunction;
	}

	switch ( minorClass )
	{
		// audio item
		case MXDLNA_CDSOBJECT_MINOR_MUSICTRACK :
		case MXDLNA_CDSOBJECT_MINOR_AUDIOBROADCAST :
		case MXDLNA_CDSOBJECT_MINOR_AUDIOBOOK :
			retCode = FolderType_Music;
			goto endFunction;

			// photo item
		case MXDLNA_CDSOBJECT_MINOR_PHOTO :
			retCode = FolderType_Video;
			goto endFunction;

			// video item
		case MXDLNA_CDSOBJECT_MINOR_MOVIE :
		case MXDLNA_CDSOBJECT_MINOR_VIDEOBROADCAST :
		case MXDLNA_CDSOBJECT_MINOR_MUSICVIDEOCLIP :
			retCode = FolderType_Image;
			goto endFunction;
	}

	retCode = FolderType_Unknown;

endFunction :

	return retCode;
}

HCHAR *__find_media_thumbnail( MXDLNA_CDSRESOURCE *pRes )
{
	HCHAR *candidate = "";
	HINT32 cnt = 0 ;
	MX_INT32 mf;

	while ( pRes )
	{
		HCHAR *v = mxMedia_getProtocolValue( (MX_INT8*)pRes->ProtocolInfo  , "DLNA.ORG_PN=" );

		if ( v && HxSTD_StrCmp( v , "JPEG_TN" ) == 0 )
		{
			return (HCHAR*)pRes->Value;
		}

		/*  thumbnail 이 없을때 Image 를 바로 주나?  */
		mxMedia_getExactMediaformat( pRes->ProtocolInfo , MF_IMAGE , &mf , 0 );

		if ( mf == MF_IMAGE_JPG && cnt > 0 )
			candidate = (HCHAR*)pRes->Value;
		cnt ++;

		pRes = pRes->Next;
	}

	return candidate;
}

/* DMS가 Image의 Resolution을 전달하지 않는다면 직접 구해 보자. */
void __get_image_resolution( HCHAR *pUrl, HINT32 *pResX, HINT32 *pResY )
{
	MXFILE_DESC stFileDesc = { 0, };
	MXMEDIA_INFO stMediaInfo = { 0, };
	MXCODEC_INFO stCodecInfo = { 0, };
	MX_INT32	nRet = 0;

	if( pUrl == NULL || pResX == NULL || pResX == NULL )
		return;

	stFileDesc.url = pUrl;
	stFileDesc.linear_access	= 0;
	stFileDesc.cancel_op		= NULL;

	nRet = mxMedia_getInfoByHttp( &stFileDesc, &stMediaInfo );
	if( nRet > 0 )
	{
		if( stMediaInfo.media_format == MF_IMAGE_JPG || stMediaInfo.media_format == MF_IMAGE_PNG )
		{
			nRet = mxMedia_getCodecInfo( &stFileDesc, &stMediaInfo, NULL, MXDLNA_DETECT_CMD_ACCESS, &stCodecInfo );
			if( nRet > 0 )
			{
				*pResX = stCodecInfo.media.image.width;
				*pResY = stCodecInfo.media.image.height;

				HxLOG_Print( "[DLNA_MakeUserdata] Find Image Resolution( %d x %d )\n", *pResX, *pResY );
			}
			else
			{
				HxLOG_Print( "[DLNA_MakeUserdata] fail to mxMedia_getCodecInfo() ret = %d\n", nRet );
			}
		}
		else
		{
			HxLOG_Print( "[DLNA_MakeUserdata] invalid media format( %d )\n", stMediaInfo.media_format );
		}
	}
	else
	{
		HxLOG_Print( "[DLNA_MakeUserdata] fail to mxMedia_getInfoByHttp() ret = %d\n", nRet );
	}
}

#define ________________Public_Functions___________________________
//Description : DLNA의 Cache에 저장될 사용자 데이터를 만드는 callback 함수입니다
MX_INT32 PL_USERDATA_Make(MXDLNA_CDSOBJECT *object, void *appData )
{
	MXDLNA_CDSRESOURCE  *Res = NULL;
	CO_MediaInstence_t	*item = (CO_MediaInstence_t *) appData;
	HLONG majorClass , minorClass , rootClass , mediaClass;
	HLONG mf = MF_UNKNOWN;
	HLONG mi = MF_UNKNOWN;
	HLONG media_type = 0;
	HCHAR *thumbnail = NULL;
	const MX_INT8 *p = NULL;

	rootClass  = mxDlnaMscp_getRootClass( object );
	majorClass = mxDlnaMscp_getMajorClass( object );
	minorClass = mxDlnaMscp_getMinorClass( object );

	mediaClass = (rootClass << 24) | (majorClass << 16) | (minorClass << 8 );

	HxLOG_Print("[DLNA_MakeUserdata] mediaClass (0x%x)\n", __FUNCTION__, mediaClass);

	// 초기화
	item->eFolderType = item->ChildCount = item->thumbnailType = 0;

	item->resolution[0] = 0;
	item->Bitrate = 0;
	item->Duration = 0;
	item->FileSize = 0;

	//MXDLNA_CDSOBJECT

	/* FolderType */
	if ( CxDLNA_isContainer( rootClass ) )
	{
		item->eFolderType = __is_folder_type( majorClass , minorClass );
	}

	/* Object ID */
	HxSTD_snprintf(item->ID , sizeof(item->ID), "%s", (HCHAR*)object->ID );

	/* Title metadata */
	HxSTD_snprintf ( item->Title , sizeof(item->Title), "%s", (HCHAR*)object->Title);

	/* genre for this content */
	p = mxDlnaMscp_getStrProperty( object , MXDLNA_SP_GENRE , 0 );
	if ( p ) HxSTD_snprintf(item->Genre, sizeof(item->Genre), "%s", (HCHAR *)p); p = NULL;

	/* Artist */
	p = mxDlnaMscp_getStrProperty( object , MXDLNA_SP_ARTIST , 0 );
	if ( p ) HxSTD_snprintf(item->Artist, sizeof(item->Artist), "%s",  (HCHAR*)p); p = NULL;

	p = mxDlnaMscp_getStrProperty( object , MXDLNA_SP_ALBUM , 0 );
	if ( p ) HxSTD_snprintf(item->AlbumTitle , sizeof(item->AlbumTitle ), "%s",  (HCHAR*)p); p = NULL;

	//albumArtUri
	p = mxDlnaMscp_getStrProperty( object , MXDLNA_GSP_ALBUMARTURI , 0 );
	if ( p )
	{
		thumbnail =  (HCHAR*)p;
		HxLOG_Print( "[DLNA_MakeUserdata] AlbumArtURI = %s \n" , thumbnail );

		item->thumbnailType = ThumbnailType_Album;
		/* media thumbnail uri */
		if(thumbnail != NULL)
		{
#if 0
			HINT32	nResX = 0, nResY = 0;

			if ( (Res != NULL) && (Res->ResolutionX > 0 && Res->ResolutionY > 0) )
			{
				nResX = Res->ResolutionX;
				nResY = Res->ResolutionY;
			}
			else
			{
				/* DMS가 Resolution 정보를 주지 않는 경우 직접 구함 */
				__get_image_resolution( thumbnail, &nResX, &nResY );
				HxLOG_Print( "[DLNA_MakeUserdata] Thumbnail Resolution ( %d x %d )\n", nResX, nResY );
			}

			if( nResX > 0 && nResY > 0 )
#endif
			{
#if SUPPORT_CHECK_THUMBNAIL_RESOLUTION
				/* Image의 경우 Resolution 제약 사항에 걸리는 경우, URI를 지워서 올려줌. Decording시 Crash 방지 */
				if( ( nResX * nResY ) <  PAL_DLNA_MAX_RESOLUTION &&
						nResX < PAL_DLNA_MAX_WIDTH &&
						nResY < PAL_DLNA_MAX_HEIGHT )
#endif
				HxSTD_snprintf(item->ThumbnailURI, sizeof(item->ThumbnailURI), "%s", thumbnail);
			}

		}
		else
			HxSTD_MemSet(item->ThumbnailURI, 0x0, MEDIA_URL_MIN_SIZE);
	}

	/* ChildCount - 가공해서 사용*/
	if ( CxDLNA_isContainer( rootClass ) )
		item->ChildCount = mxDlnaMscp_getIntProperty(object , MXDLNA_IP_CHILDCOUNT, 0 );

	// 1. 초기화
	if ( !CxDLNA_isContainer( rootClass ) )
		media_type = mxRes_majorClassToMediaType( majorClass );

	/*-----------------------------------------------------------------------------
	 *  Multi-Resource 에 대해서 고민해야함.
	 *-----------------------------------------------------------------------------*/

	// 2. 최적의 리소스 선택하기
	if ( media_type != MF_UNKNOWN )
		Res = mxRes_findBestResource( object , media_type );

	// 선택된 리소스에서 데이터 채우기.
	if ( Res )
	{
		item->SampleFrequency = Res->SampleFrequency;
		item->NrAudioChannels = Res->NrAudioChannels;

		if(Res->ProtocolInfo != NULL)
		{
			char *token, *pPtr;
			HCHAR *pcdtcp_port = NULL;
			HCHAR *pcdtcp_host = NULL;
			MXDLNA_PROTOCOLINFO *proinfo = NULL;

			mxMedia_getExactMediaformat((const MX_INT8 *)Res->ProtocolInfo , media_type , &mf ,&mi);
			if ((media_type == 256))
			{
				if((HxSTD_StrStr(Res->ProtocolInfo, "Video/vnd.dlna.mpeg-tts") != NULL) ||
						(HxSTD_StrStr(Res->ProtocolInfo, "video/vnd.dlna.mpeg-tts") != NULL) ||
						(HxSTD_StrStr(Res->ProtocolInfo, "Video/mpeg") != NULL) ||
						(HxSTD_StrStr(Res->ProtocolInfo, "video/mpeg") != NULL))
				{
					mf  = MF_VIDEO_MPG;
				}
			}

			if( item->eFolderType == 0 )
			{ /* Content Type == Item */
				switch( MASK_MEDIATYPE( mf ) )
				{
					case MF_VIDEO:
						item->eItemType = MediaType_Video;
						break;

					case MF_AUDIO:
						item->eItemType = MediaType_Music;
						break;

					case MF_IMAGE:
						item->eItemType = MediaType_Photo;
						break;

					default:
						item->eItemType = MediaType_Unknown;
						break;
				}
			}
			else
			{ /* Content Type == Container */
				item->eItemType = MediaType_Container;
			}

			proinfo = mxDlna_protocolinfoParser( (HCHAR*)Res->ProtocolInfo , HxSTD_StrLen(Res->ProtocolInfo) );
			if(proinfo != NULL)
			{
				item->ByteSeekable = ( proinfo->SupportsByteBasedSeek == 1 ) ? TRUE : FALSE;
				item->TimeSeekable = ( proinfo->SupportsTimeBasedSeek == 1 ) ? TRUE : FALSE;

				// Fixed CTT DMP 7.3.41.1
				if(proinfo->SenderPaced == TRUE)
				{
					HxLOG_Print("[DLNA_MakeUserdata] proinfo->SenderPaced == TRUE RangeAvailable is FALSE !!!!!!!!!!!!!!!\n");
					item->ByteSeekable = FALSE;
				}

				mxDlna_protocolinfoDestruct( proinfo );
			}

			HxSTD_snprintf(item->Protocol, sizeof(item->Protocol), "%s", Res->ProtocolInfo);

			pcdtcp_host = HxSTD_StrStr((HCHAR*)Res->ProtocolInfo,"DTCP1HOST=");
			pcdtcp_port = HxSTD_StrStr((HCHAR*)Res->ProtocolInfo,"DTCP1PORT=");

			if(pcdtcp_host && pcdtcp_port)
				item->dtcpenable = TRUE;

			if(pcdtcp_host != NULL)
			{
				token = strtok_r(pcdtcp_host, ";", &pPtr);
				if(token)
					HxSTD_snprintf(item->dtcp_ipaddress, sizeof(item->dtcp_ipaddress), "%s", token+HxSTD_StrLen("DTCP1HOST="));
				token = NULL;
			}

			if(pcdtcp_port != NULL)
			{
				token = strtok_r(pcdtcp_port, ";", &pPtr);
				if(token)
					HxSTD_snprintf(item->dtcp_portnumber, sizeof(item->dtcp_portnumber), "%s", token+HxSTD_StrLen("DTCP1PORT="));
			}

			/* Fixed [redmine #61555: DMP object listing error with Synology NAS */
			/* This is workaround for DLNA stack problem with undefined item type */
			/* TODO: Match with detailed mime-type, not pre-fixed string(but, too many mime-types) */
			if (item->eItemType == MediaType_Unknown)
			{
				/* RTSP protocolInfo? */
				if (HxSTD_StrStr (item->Protocol, "http-get:*:video") != NULL)
					item->eItemType = MediaType_Video;

				/* Real Media rm file */
				else if (HxSTD_StrStr (item->Protocol, "http-get:*:application/vnd.rn-realmedia") != NULL)
					item->eItemType = MediaType_Video;

				else if (HxSTD_StrStr (item->Protocol, "http-get:*:audio") != NULL)
					item->eItemType = MediaType_Music;

				else if (HxSTD_StrStr (item->Protocol, "http-get:*:image") != NULL)
					item->eItemType = MediaType_Photo;
				else
					item->eItemType = MediaType_Unknown;
			}
		}

		/* media uri */
		if ( Res->Value )
			HxSTD_snprintf(item->URI, sizeof(item->URI), "%s", Res->Value);

		/* duration */
		if (Res->Duration == (-1))
			item->Duration = 0;
		else
			item->Duration = Res->Duration ;

		/* Bitrate */
		if (Res->Bitrate == (-1))
			item->Bitrate = 0;
		else
			item->Bitrate = Res->Bitrate;

		/* Size */
		item->FileSize = Res->Size;

		//Thumbnail을 얻어오고
		thumbnail = __find_media_thumbnail( Res );
		if ( thumbnail && item->ThumbnailURI[0] == 0)
		{
			HxSTD_snprintf(item->ThumbnailURI, sizeof(item->ThumbnailURI), "%s", thumbnail);
			item->thumbnailType = ThumbnailType_Media;
		}
	}

	/* resolution */
	if( majorClass == MXDLNA_CDSOBJECT_MAJOR_IMAGEITEM )
	{
		HINT32	nResX = 0, nResY = 0;

		if ((Res != NULL) &&  (Res->ResolutionX > 0 && Res->ResolutionY > 0) )
		{
			nResX = Res->ResolutionX;
			nResY = Res->ResolutionY;

			HxLOG_Print( "[DLNA_MakeUserdata:%d] Photo[%s]  Image Resolution From DMS( %d x %d )\n",
			   	__LINE__, item->Title,  Res->ResolutionX, Res->ResolutionY );

		}
		else
		{
			/* DMS가 Resolution 정보를 주지 않는 경우 직접 구함 */
			if(Res)
			{
				HxLOG_Print( "[DLNA_MakeUserdata] Invalid Image Resolution From DMS( %d x %d )\n", Res->ResolutionX, Res->ResolutionY );
			}
			__get_image_resolution( item->URI, &nResX, &nResY );
		}

		if( nResX > 0 && nResY > 0 )
		{
#if SUPPORT_CHECK_THUMBNAIL_RESOLUTION
			/* Image의 경우 Resolution 제약 사항에 걸리는 경우, URI를 지워서 올려줌. Decording시 Crash 방지 */
			/*  WEB 단에서 resolution 를 보고 재생 유무 결정  */
			if( ( nResX * nResY ) <  PAL_DLNA_MAX_RESOLUTION &&
				nResX < PAL_DLNA_MAX_WIDTH &&
				nResY < PAL_DLNA_MAX_HEIGHT )
#endif
				HxSTD_snprintf( item->resolution , DLNA_RES_SIZE, "%d x %d" , nResX, nResY );
		}
	}

	return 0;
}

//Description : DLNA가 부르는 callback 함수로서 Cache에 들어 있는 User Data중
//				동적으로 할당된 메모리를 해제 하는 역할을 수행합니다
MX_INT32 PL_USERDATA_Free( void* appData )
{
	CO_MediaInstence_t *item = (CO_MediaInstence_t *) appData;

	if( item == NULL )
		return 0;

	return 0;
}

void PL_USERDATA_Cds_Dump(CO_MediaInstence_t *pItem)
{
	if( pItem == NULL )
		return;

	HxLOG_Print( "[ COMMON CDS ]++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" );
	HxLOG_Print( "[ COMMON CDS ] eItemType = %d\n", pItem->eItemType );
	HxLOG_Print( "[ COMMON CDS ] ID = %s\n", pItem->ID );
	HxLOG_Print( "[ COMMON CDS ] nNumOfChildItems = %d\n", pItem->ChildCount);
	HxLOG_Print( "[ COMMON CDS ] IDofUpper = %s\n", pItem->IDofUpper );
	HxLOG_Print( "[ COMMON CDS ] title = %s\n", pItem->Title );
	HxLOG_Print( "[ COMMON CDS ] URI = %s\n", pItem->URI );
	HxLOG_Print( "[ COMMON CDS ] thumbnailURI = %s\n", pItem->ThumbnailURI );
	HxLOG_Print( "[ COMMON CDS ] ByteSeekAble = %d\n", pItem->ByteSeekable);
	HxLOG_Print( "[ COMMON CDS ] TimeSeekAble = %d\n", pItem->TimeSeekable);
	HxLOG_Print( "[ COMMON CDS ] filesize = %d\n", pItem->FileSize );
	HxLOG_Print( "[ COMMON CDS ] protocol = %s\n", pItem->Protocol );
	HxLOG_Print( "[ COMMON CDS ] bitrate = %d\n", pItem->Bitrate );
	HxLOG_Print( "[ COMMON CDS ] duration = %d\n", pItem->Duration );
	HxLOG_Print( "[ COMMON CDS ] nrAudioChannels = %d\n", pItem->NrAudioChannels );
	HxLOG_Print( "[ COMMON CDS ] sampleRateFreq = %d\n", pItem->SampleFrequency);
	HxLOG_Print( "[ COMMON CDS ] AlbumTitle = %s\n", pItem->AlbumTitle );
	HxLOG_Print( "[ COMMON CDS ] szGenre = %s\n", pItem->Genre );
	HxLOG_Print( "[ COMMON CDS ] Artist = %s\n", pItem->Artist );
	HxLOG_Print( "[ COMMON CDS ] resolution = %s\n", pItem->resolution );
	HxLOG_Print( "[ COMMON CDS ] ParentID = %s\n", pItem->IDofUpper);
	HxLOG_Print( "[ COMMON CDS ]++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" );
}
