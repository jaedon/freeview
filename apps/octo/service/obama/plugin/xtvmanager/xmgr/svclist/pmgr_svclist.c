#include <octo_common.h>

//#include <pmgr_svclist.h>

#include "./local_include/_xmgr_svclist.h"



/*
	SVC_Attr_t�� ���� �⺻������ memset 0���� ���õȴ�.
	SVC_Attr_t�� �ʵ带 ���캸��, 0�� All �Ӽ��� �ǹ��Ѵ�.
	�ϴ� DB_SVC_InitSvcAttribute()�� ���ؼ� �⺻���� SVC_Attr_t�� �����ϰ�,
	�� �⺰�� �޶��� �� �ִ� �⺻������ �������ش�.
	���⼭ ������ �⺻ �˻� �Ӽ���, chlist, edit channel, epg, zapping �� ap ���ݿ� ���� ����ȴ�.
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

