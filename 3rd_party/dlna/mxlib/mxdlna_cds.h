
#ifndef _MXDLNA_CDS_H__
#define _MXDLNA_CDS_H__


#include "mxapi_define.h"

#ifdef __cplusplus
extern "C"{
#endif



// **********************************************
// include
// **********************************************
#include "mxtypes.h"
#include "mxdlna_struct.h"



//------------------------------------------------------
/**	\brief	mxDlnaCds_classStringToClassType 
\brief	class문자열을 정수 값으로 변경하는 함수이다.

\param	classString	a parameter of type char *
\return	int	
*/
//------------------------------------------------------
__MXAPI MX_INT32 mxDlnaCds_classStringToClassType(
//------------------------------------------------------
	char * classString
);


//------------------------------------------------------
__MXAPI char * mxDlnaCds_ClassTypeToClassString( 
//------------------------------------------------------
	MX_INT32 classType
);




//  아래 함수들에서 값을 설정하지 않으려면 이 값을 매개변수로 넣는다.
#define MXDLNA_CDS_NOTSET	-10


// Container
__MXAPI
void mxDlnaCds_setContainerProperty(
	MXDLNA_CDSOBJECT * pObj,
	int dlnaContainerType,
	int searchClass,
	int createClass,
	int childCount, 
	int isRestricted,
	int isSearchable
);

// Item
__MXAPI
void mxDlnaCds_setItemProperty(
	MXDLNA_CDSOBJECT * pObj,
	char * refID,
	char * bookmarkID
);


// Image
__MXAPI
void mxDlnaCds_resSetImageProperty(
   MXDLNA_CDSRESOURCE *pRes, 
   int resX, int resY, 
   int colorDepth, 
   MX_INT64 size
);

// Video
__MXAPI
void mxDlnaCds_resSetVideoProperty(
   MXDLNA_CDSRESOURCE *pRes , 
   int resX, int resY, 
   int duration, 
   int bitrate, 
   int colorDepth, 
   MX_INT64 size
);

// Audio
__MXAPI
void mxDlnaCds_resSetAudioProperty(
	MXDLNA_CDSRESOURCE *pRes, 
	int duration, 
	int bitrate,
	int bitsPerSample, 	
	int sampleFrequency, 
	int channelNum, 
	MX_INT64 size
);

__MXAPI
void mxDlnaCds_resSetEtcProperty(
	MXDLNA_CDSRESOURCE *pRes,
	char * importURI,
	char * ifoFileURI,
	char * importIfoFileURI,
	char * userData,
	char * protection,
	MX_INT64 size
);

#ifdef __cplusplus
}
#endif

#endif //_MXDLNA_CDS_H__

