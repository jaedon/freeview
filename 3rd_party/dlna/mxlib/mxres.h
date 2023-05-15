#ifndef MXDLNA_APP_RESOURCE_EXTERN__H_
#define MXDLNA_APP_RESOURCE_EXTERN__H_


#include "mxapi_define.h"


#ifdef __cplusplus
extern "C" 
{
#endif



#include "mxres_define.h"
#include "mxres_ref.h"
#include "mxtypes.h"
#include "mxdlna.h"


// 20101006 - whiterub
// custom 에 있던 항목들을 callback 타입으로 변경
typedef MX_INT32 (*FN_ISAVAILABLEPLAY)(
	MX_INT32 media_format,
	char *mimeString
);

typedef MXRES_PRE_TABLE * (*FN_GETPRECMDTABLE)(
	MX_INT32 media_type
);

typedef MXPN_PRI_MAP * (*FN_GETPROFILEPRITABLE)(
	MX_INT32 media_type
);

typedef struct _MXRES_CALLBACK
{
	FN_ISAVAILABLEPLAY fnIsAvailablePlay;
	FN_GETPRECMDTABLE fn_GetPreCmdTable;
	FN_GETPROFILEPRITABLE fn_GetProfilePriTable;
}MXRES_CALLBACK;




// 20101006 mxres에서 사용하는 
// mxMedia_isPlayable , mxRes_getPreCmdTable, mxRes_getProfilePriTable 함수는 callback 타입으로 변경

// 리소스 선택관련 초기화 작업	
// 우선순위 테이블 초기화
__MXAPI 
MX_INT32 mxRes_init( MXRES_CALLBACK * pMxResCallback );








// 최적의 리소스 찾아 주는 함수.
__MXAPI 
MXDLNA_CDSRESOURCE* mxRes_findBestResource( 
	MXDLNA_CDSOBJECT * obj , 
	MX_INT32 media_type
);



// 우선순위 PN값 변경하게 해주는 함수
__MXAPI 
MX_INT32 mxRes_chgPnPriority( 
	MX_INT32 media_type, 
	MX_INT8 * pnStr, 
	MX_INT32 changePriValue 
);



// util

// protocol info를 파싱하면서 얻은 정보를 비트별로 채운다	
// ----------------------------------------------------------------------
// |   R    |    R   |   R  |   R   |Direct|        Profile ID	        |	// Photo
// ----------------------------------------------------------------------
// |        |channels|timeS | byteS |Direct|        Profile ID			|	// Music
// ----------------------------------------------------------------------
// |		|channels|timeS | byteS |Direct|        Profile ID			|	// Video
// ----------------------------------------------------------------------
__MXAPI 
MX_INT32 mxRes_getCmdProtocolInfo ( 
	MXRES_INFO_CMD cmd ,            // 어떤 정보를 가져올지 정한다                    
	MX_UINT32 source ,              // 가져올 비트배열
	MX_UINT32 * pOutValue          // 결과값
);

__MXAPI 
MX_INT32 mxRes_setCmdProtocolInfo( 
	MXRES_INFO_CMD cmd ,            // 어떤 정보를 저장할지 정한다
	MX_UINT32 source ,              // 저장할 비트배열
	MX_INT32 inpValue ,             // 저장할 입력 값
	MX_UINT32 * pOutValue          // 저장된 결과 값
);


// protocolInfo를 넣으면 비트 배열로 값을 설정해줌.
__MXAPI 
MX_UINT32 mxRes_saveProtocolInfo(
	MX_INT8 * inpProtocolInfo , 
	MX_INT32 media_type 
);


__MXAPI 
MX_INT32 mxRes_majorClassToMediaType( 
	MX_INT32 majorClass 
);







#ifdef __cplusplus
}
#endif



#endif
