#include <octo_common.h>

//#include <pmgr_svclist.h>

#include "./local_include/_xmgr_svclist.h"



/*
	SVC_Attr_t의 값은 기본적으로 memset 0으로 세팅된다.
	SVC_Attr_t의 필드를 살펴보면, 0은 All 속성을 의미한다.
	일단 DB_SVC_InitSvcAttribute()를 통해서 기본적인 SVC_Attr_t을 설정하고,
	각 향별로 달라질 수 있는 기본조건을 세팅해준다.
	여기서 설정된 기본 검색 속성은, chlist, edit channel, epg, zapping 등 ap 전반에 걸쳐 적용된다.
*/
void pmgr_svclist_InitSvcAttribute(DbSvc_Attr_t * pstAttr)
{
	if(pstAttr == NULL)
		return;

	xmgr_svclist_InitSvcAttribute_Base(pstAttr);
}


HERROR	pmgr_svclist_GetSvcDbParamsFromGroup (ChListUiGroup_t eSvcListGroup, DbSvc_Group_e *peSvcGroup, DbSvc_Attr_t *pstAttr)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == peSvcGroup)			return ERR_FAIL;
	if (NULL == pstAttr)			return ERR_FAIL;

	hErr = xmgr_svclist_GetSvcDbParamsFromGroup_Base (eSvcListGroup, peSvcGroup, pstAttr);

	return hErr;
}

HERROR	pmgr_svclist_CheckValidGroup (ChListUiGroup_t eSvcListroup)
{
	return xmgr_svclist_CheckValidGroup_Base (eSvcListroup);
}

DxSvcType_e	pmgr_svclist_GetSvcCountCheckType(void)
{
	return SERVICE_TYPE_TV_RADIO;
}

