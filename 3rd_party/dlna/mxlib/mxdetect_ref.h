#ifndef MX_DETECT_REF__H_
#define MX_DETECT_REF__H_


#include "mxapi_define.h"


#ifdef __cplusplus
extern "C" 
{
#endif


#include "mxtypes.h"





// Resource Selection의 우선 순위..
typedef struct _MXPN_MAP
{  
    MX_INT32 media_format;    
	MX_INT32 label;
	MX_INT32 profile_value;

}MXPN_MAP;




//-----------------------------------------------------------------------------------------//
//Media Profiles
//-----------------------------------------------------------------------------------------//
// Detection & Resource Selection에서 사용
//		ProfileID			MIMEType		container		TimeStamp		codec1		profile		
//		Resolution		Region		audio-codec		MediaProfileDefine		
typedef struct _MXVIDEO_MAP
{
    MX_INT8 *name;
	
	MX_UINT16 media_type;
	MX_UINT8 container;
	MX_UINT8 TimeStamp;
	MX_UINT8 codec;
	MX_UINT8 profile_type;
	MX_UINT8 resolution;
	MX_UINT8 region;	
	MX_UINT8 audio_codec;	
	MX_UINT32 profile_value; 		

}MXVIDEO_MAP;





__MXAPI MX_INT32 mxMedia_isAvailablePlay( MX_INT32 format , char *mime);






	
#ifdef __cplusplus
}
#endif




#endif

