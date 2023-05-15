#ifndef CDS_BROWSINGLIST_H
#define CDS_BROWSINGLIST_H

#ifdef __cplusplus
extern "C"{
#endif

#include "mxapi_define.h"


#ifndef USER_TYPE_STRING
#define USER_TYPE_STRING char
#endif

#include "mxdlna_struct.h"
#include "mxdlna_mscp.h"

typedef struct _MXDMC_CDS_DS_ITEM* (*MXDLNA_DMC_BROWSE_USER_MAKE_DS) (char *UDN, char *ObjectID, void *UserObject, struct _MXDMC_CDS_DS_ITEM  *pPrev, int depth);
typedef struct _MXDMC_CDS_BS_ITEM* (*MXDLNA_DMC_BROWSE_USER_MAKE_BS) (struct _MXDMC_CDS_DS_HANDLE *DsHandle, struct mxdlna_cdsobject* result, int idx);
typedef void (*MXDLNA_DMC_BROWSE_USER_CALLBACK) (struct _MXDMC_CDS_DS_HANDLE *DsHandle, struct _MXDMC_CDS_BS_HANDLE *BsHandle, int errorCode);

typedef struct _MXDMC_CDS_BS_ITEM
{
	int depth;
	int IsSearch;
	int isAvailable;

	USER_TYPE_STRING *Title;
	char *UDN;
	char *ObjectID;

	char *media_uri;
	char *media_thumbnail;

	char *SubtitleUri;
	int	 media_format;
	char *PN;

	MX_INT64 Size;
	
	char *metaData;
	char *protocolInfo;
	char *extension;
	void *path;

	struct _MXDMC_CDS_BS_ITEM *pPrev;
	struct _MXDMC_CDS_BS_ITEM *pNext;

	struct _MXDMC_CDS_BS_ITEM *pParent;
	struct _MXDMC_CDS_BS_ITEM *pChild;

} MXDMC_CDS_BS_ITEM;


typedef struct _MXDMC_CDS_BS_HANDLE {
	int IsProcess;

	int	Idx;
	int	Cnt;
	int	ItemIdx;
	int	ItemCnt;

	MX_INT64 TotalSize;

	MXDMC_CDS_BS_ITEM *Start;
	MXDMC_CDS_BS_ITEM *State;
	MXDMC_CDS_BS_ITEM *End;
} MXDMC_CDS_BS_HANDLE;


typedef struct _MXDMC_CDS_DS_ITEM 
{
	int depth;
	int IsSearch;
	int isAvailable;

	USER_TYPE_STRING *Title;
	int TitleLength;
	char *UDN;
	char *ObjectID;
	char *media_uri;
	char *media_thumbnail;
	char *SubtitleUri;
	int	 media_format;
	char *PN;

	MX_INT64 Size;

	char *metaData;
	char *protocolInfo;
	char *extension;

	struct _MXDMC_CDS_DS_ITEM *pNext;
} MXDMC_CDS_DS_ITEM;


typedef struct _MXDMC_CDS_DS_HANDLE 
{
	void *DmcHandle;
	MXDMC_CDS_DS_ITEM *Start;							// 시작위치
	MXDMC_CDS_DS_ITEM *Location;						// 만들고 있는 위치
	MXDMC_CDS_DS_ITEM *State;							// 사용하고 있는 위치
	
	MXDLNA_DMC_BROWSE_USER_MAKE_DS		DsBack;			// UserMake가 들어가지만
	MXDLNA_DMC_BROWSE_USER_MAKE_BS		BsBack;			// USerMake가 들어가지만
	MXDLNA_MSCP_BROWSE_RESULT_CUSTOM	Callback;		// CDSList가 돌아오는 함수.
	MXDLNA_DMC_BROWSE_USER_CALLBACK		Userback;		// 최종 결과를 유저에게 반납하는 함수.

	MXDMC_CDS_BS_HANDLE *BsHandle;
} MXDMC_CDS_DS_HANDLE;

////////////////////////////////////////////////////////////////////////////
// @  탐색될 리스트를 작성합니다.
////////////////////////////////////////////////////////////////////////////

// 디바이스 리스트를 전달하면 해당 디바이스의 “0” Object를 대상으로 탐색할 목록을 만듭니다.
//int mxDmc_deviceList2DsList(void *hDmc, MXDMC_CDS_DS_HANDLE *DsHandle, MXDLNA_DEVICE_TINY *DeviceList, MXDLNA_MSCP_BROWSE_RESULT_CUSTOM CallBack, MXDLNA_DMC_BROWSE_USER_CALLBACK UserCallBack);
__MXAPI int mxDmc_deviceList2DsList(void *hDmc, MXDMC_CDS_DS_HANDLE *DsHandle, 
							MXDLNA_DEVICE_TINY *DeviceList, 
							MXDLNA_DMC_BROWSE_USER_MAKE_DS DsBack,
							MXDLNA_DMC_BROWSE_USER_MAKE_BS BsBack,
							MXDLNA_MSCP_BROWSE_RESULT_CUSTOM CallBack, 
							MXDLNA_DMC_BROWSE_USER_CALLBACK UserCallBack);


// 탐색할 목록을 초기화 
//및 최초 아이템을 추가합니다. -- mxDmc_addDsList로 통일
//int mxDmc_initDsList(void *hDmc, MXDMC_CDS_DS_HANDLE *DsHandle, char *UDN, char *ObjectID, MXDLNA_CDSOBJECT* CdsObject, MXDLNA_MSCP_BROWSE_RESULT_CUSTOM CallBack, MXDLNA_DMC_BROWSE_USER_CALLBACK UserBack);
// 검색 리스트에 최초에 더미 노드가 존재합니다.
//int mxDmc_initDsList(void *hDmc, MXDMC_CDS_DS_HANDLE *DsHandle, MXDLNA_MSCP_BROWSE_RESULT_CUSTOM CallBack, MXDLNA_DMC_BROWSE_USER_CALLBACK UserBack);
__MXAPI int mxDmc_initDsList(void *hDmc, MXDMC_CDS_DS_HANDLE *DsHandle,
					 MXDLNA_DMC_BROWSE_USER_MAKE_DS	DsBack,
					 MXDLNA_DMC_BROWSE_USER_MAKE_BS BsBack,
					 MXDLNA_MSCP_BROWSE_RESULT_CUSTOM CallBack,
					 MXDLNA_DMC_BROWSE_USER_CALLBACK UserBack
					 );


// mxDmc_initDsLis을 호출 한 이후에 탐색할 목록에 아이템을 추가하기 위해서 사용합니다. 
__MXAPI int mxDmc_addDsList(MXDMC_CDS_DS_HANDLE *DsHandle, char *UDN, char *ObjectID, void* CdsObject, int depth) ;
int mxDmc_delDsList(MXDMC_CDS_DS_HANDLE *DsHandle, char *UDN, char *ObjectID);

// 탐색할 아이템 목록을 제거합니다.  탐색할 아이템 목록만 제거됩니다.
__MXAPI int mxDmc_deinitBrowsingList(MXDMC_CDS_DS_HANDLE *DsHandle);

// 탐색한 결과를 가지는 핸들을 만듭니다.
__MXAPI int mxDmc_makeBsHandle(MXDMC_CDS_DS_HANDLE *DsHandle);

// 현재 탐색 중인 아이템을 가져옵니다.
__MXAPI MXDMC_CDS_DS_ITEM *mxDmc_getDsListItem(MXDMC_CDS_DS_HANDLE *DsHandle);

// 탐색 중인 아이템 다음 아이템을 가져옵니다.
__MXAPI MXDMC_CDS_DS_ITEM *mxDmc_nextDsListItem(MXDMC_CDS_DS_HANDLE *DsHandle);

////////////////////////////////////////////////////////////////////////////
// @ 탐색을 시작하고 탐색된 결과를 저장합니다.
////////////////////////////////////////////////////////////////////////////
// CDSObjectData를 가지고 탐색한 결과가 되는 아이템을 생성합니다. 정보는 최소 단위로 한정되어있습니다.
//MXDMC_CDS_BS_ITEM *mxDmc_defaultMakeCds2Bs(MXDMC_CDS_DS_HANDLE *DsHandle, MXDLNA_CDSOBJECT* result, int idx);

// 생성된 아이템을 탐색결과 리스트에 추가합니다.
__MXAPI MXDMC_CDS_BS_ITEM *mxDmc_insertBsItem(MXDMC_CDS_DS_ITEM *DsItem, MXDMC_CDS_BS_HANDLE *BsHandle, MXDMC_CDS_BS_ITEM *end);

// 탐색을 시작 합니다.
__MXAPI int mxDmc_startDsList(MXDMC_CDS_DS_HANDLE *DsHandle);

// 모든 탐색을 종료하고 해당 데이터를 정리합니다.
__MXAPI void mxDmc_endDsList(MXDMC_CDS_DS_HANDLE *DsHandle);

// 탐색된 아이템 리스트의 처음 아이템을 가져옵니다.
__MXAPI MXDMC_CDS_BS_ITEM *mxDmc_getBsStartItem(MXDMC_CDS_DS_HANDLE *DsHandle);

// 탐색된 아이템이 현재 재탐색 대기중인 위치를 반환합니다.
__MXAPI MXDMC_CDS_BS_ITEM *mxDmc_getBsStateItem(MXDMC_CDS_DS_HANDLE *DsHandle);

// 아이템의 위치를 이동합니다.
__MXAPI MXDMC_CDS_BS_ITEM *mxDmc_setBSItemIdx(MXDMC_CDS_BS_HANDLE *BsHandle, int idx);
__MXAPI MXDMC_CDS_BS_ITEM *mxDmc_getBSItemInfo(MXDMC_CDS_BS_HANDLE *BsHandle);
__MXAPI MXDMC_CDS_BS_ITEM *mxDmc_getNextBSItemInfo(MXDMC_CDS_BS_HANDLE *BsHandle);
__MXAPI MXDMC_CDS_BS_ITEM *mxDmc_getPrevBSItemInfo(MXDMC_CDS_BS_HANDLE *BsHandle);

// 현재 파일 크기를 전달합니다.
__MXAPI MX_INT64 mxDmc_getBSItemInfoFileSize(MXDMC_CDS_BS_HANDLE *BsHandle);
__MXAPI void mxDmc_setBSItemInfoFileSize(MXDMC_CDS_BS_HANDLE *BsHandle, MX_INT64 fileSize);
__MXAPI void mxDmc_setBSItemInfoTotalFileSize(MXDMC_CDS_BS_HANDLE *BsHandle, MX_INT64 fileSize);

#ifdef __cplusplus
}
#endif

#endif /* CDS_BROWSINGLIST_H */
