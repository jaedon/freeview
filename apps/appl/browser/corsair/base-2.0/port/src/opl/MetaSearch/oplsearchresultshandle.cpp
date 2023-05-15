#include "oplsearchresultshandle.h"

#include <hlib.h>
#include <oplfilemgr.h>
#include <apk.h>

OplSearchResultsHandle::OplSearchResultsHandle(OPL_SearchTarget_e target, OPL_HANDLE context) {
    this->target = target;
    this->context = context;
    this->items = NULL;
	this->length = 0;
	this->totalSize = 0;
}

OplSearchResultsHandle::~OplSearchResultsHandle() {
    if (this->items) {
        OPL_Collection_Delete(this->items);
        this->items = NULL;
    }

    if (this->context) {
        APK_META_QUERY_Delete((ApkMetaQHandle_t)this->context);
    }
}

void OplSearchResultsHandle::abort() {
    HxLOG_Trace();

    (void) APK_META_QUERY_Abort((ApkMetaQHandle_t)this->context);

    if (this->items) {
        OPL_Collection_Delete(this->items);
        this->items = NULL;
    }

    this->offset = 0;
    this->length = 0;
    this->totalSize = 0;
    this->count = 0;
}

void OplSearchResultsHandle::release() {
    HxLOG_Trace();
}

OPL_BOOL OplSearchResultsHandle::search() {
    HERROR err;

    HxLOG_Trace();

    if (this->context == NULL) {
        return OPL_FALSE;
    }

    err = APK_META_QUERY_Search((ApkMetaQHandle_t)this->context, FALSE);

    if (err != ERR_OK) {
        HxLOG_Error("[%s:%d] APK_META_QUERY_Search() return error!\n", __HxFILE__, __HxLINE__);
        return OPL_FALSE;
    }

    return OPL_TRUE;
}

OPL_BOOL OplSearchResultsHandle::update() {
    HxVector_t *list;
    HINT32 i, n;
    OPL_ProgrammeHandle	programme;
#if defined( CONFIG_OP_JAPAN)
	HUINT32 	ulCurTime = 0;
	DxEvent_t 	*event = NULL;
	ulCurTime = HLIB_STD_GetSystemTime();
#endif

    this->length = 0;
    this->totalSize = 0;

    HxLOG_Trace();

    if (this->context == NULL) {
        return OPL_FALSE;
    }

    list = APK_META_QUERY_Result((ApkMetaQHandle_t)this->context);

    if (list == NULL) {
        HxLOG_Debug("[%s:%d] has no result!!!\n", __HxFILE__, __HxLINE__);
        return OPL_FALSE;
    }

    if (items) {
        OPL_Collection_Delete(this->items);
    }

    this->items = OPL_Collection_NewEx((void (*)(void *))OPL_Programme_Delete, (void *(*)(void *))OPL_Programme_Clone);

    if (this->items == NULL) {
        HxLOG_Critical("[%s:%d] out of memory!!!\n", __HxFILE__, __HxLINE__);
        return OPL_FALSE;
    }

    n = HLIB_VECTOR_Length(list);

    for (i = 0 ; i < n ; i++) {
        if( (i < this->offset) || ( i >= (this->offset + this->count))) {
            continue;
        }

#if defined( CONFIG_OP_JAPAN)
		/* 미정 처리 - 무조건 1개만 올라가게 해주는데 최신 미정이간다. 물론 미래 미정도 필요 없음 */
		if( ( i + 1 )< n)
		{
			/* 이렇게 가능한건 mw정렬시 미정이 무조건 제일 앞에가게 했다.*/
            event = (DxEvent_t *)HLIB_VECTOR_ItemAt(list, (i+1));
            if( event && ( event->duration == 0 ) && ( event->startTime <= ulCurTime ))
			{/* 현재 시간 전에 시작한 마지막 미정이벤트가 최신이다. */
				continue;
            }
        }

		event = (DxEvent_t *)HLIB_VECTOR_ItemAt(list, i);
		if( event && ( event->duration == 0 ) && ( event->startTime > ulCurTime ))
		{/* 미래시간 미정은 제거한다. */
			continue;
		}
#endif
        programme = OPL_Programme_New("o", APK_EVENT_Incref((DxEvent_t*)HLIB_VECTOR_ItemAt(list, i)));

        if (programme) {
            OPL_Collection_Add(this->items, (void*)programme);
        }
    }

    HLIB_VECTOR_Delete(list);

    this->length = OPL_Collection_Length(this->items);
    this->totalSize = this->length;

    return OPL_TRUE;
}

OPL_BOOL OplSearchResultsHandle::update(const char *typeName, HxVector_t *list) {
//    void* item;
	OPL_ProgrammeHandle programme;
	OPL_Recording_t* recording;
	OPL_Channel_t channel;

    if (this->items) {
        OPL_Collection_Delete(this->items);
    }

	if (list == NULL) {
	    this->length = 0;
	    this->totalSize = 0;
        return OPL_FALSE;
	}

    if (strcmp(DxNAMEOF(eDxEPG_TYPE_PF), typeName) == 0 ||
		strcmp(DxNAMEOF(eDxEPG_TYPE_SC), typeName) == 0 ||
		strcmp(DxNAMEOF(eDxEPG_TYPE_IP), typeName) == 0) {
        DxEvent_t *event;
        this->items = OPL_Collection_NewEx((void (*)(void *))OPL_Programme_Delete, (void *(*)(void *))OPL_Programme_Clone);

		if (this->items == NULL) {
			HxLOG_Critical("[%s:%d] out of memory!!!\n", __HxFILE__, __HxLINE__);
			return OPL_FALSE;
		}

        HxVECTOR_EACH(list, DxEvent_t*, event, {
            programme = OPL_Programme_New("o", APK_EVENT_Incref(event));
            if (programme) {
                OPL_Collection_Add(this->items, (void*)programme);
            }
        });
    }
    else if (strcmp(DxNAMEOF(DxService_t), typeName) == 0) {
        DxService_t *service;
        this->items = OPL_Collection_NewEx(NULL, NULL);
        HxVECTOR_EACH(list, DxService_t*, service, {
            // item = (DxService_t*)HLIB_STD_MemAlloc(sizeof(DxService_t));
            // HxSTD_MemCopy(item, service, sizeof(DxService_t));
			// channel = (void *)OPL_Channel_Clone((OPL_Channel_t)service);
			channel = OPL_Channel_FindChannelByUID(OPL_Channel_GetManager(), service->uid);
			if (channel && OPL_Channel_IsHidden(channel) == FALSE) {
            	OPL_Collection_Add(this->items, channel);
			}
        });
    }
    else if (strcmp(DxNAMEOF(DxFileInfo_t), typeName) == 0) {
        DxFileInfo_t *fileInfo;
        this->items = OPL_Collection_NewEx(NULL, NULL);
        HxVECTOR_EACH(list, DxFileInfo_t*, fileInfo, {
            OPL_Collection_Add(this->items, new OplBrowsedContent((OplBrowsedContentHandle)fileInfo));
        });
    }
    else if (strcmp(DxNAMEOF(DxRecListData_t), typeName) == 0) {
		DxRecListData_t *recData;
	    this->items = OPL_Collection_NewEx((void (*)(void *))OPL_Recording_Delete, (void *(*)(void *))OPL_Recording_Clone);
		HxVECTOR_EACH(list, DxRecListData_t*, recData, {
			recording = OPL_Recording_New("o", recData);
			if (recording) {
				OPL_Collection_Add(this->items, recording);
			}
		});
	}
    else {
        HxLOG_Debug("[%s:%d] invalid target!!!\n", __HxFILE__, __HxLINE__);
        return OPL_FALSE;
    }

    HLIB_VECTOR_Delete(list);
    this->length = OPL_Collection_Length(this->items);
    this->totalSize = this->length;

    return OPL_TRUE;
}

int OplSearchResultsHandle::getTotalSize() {
    HxVector_t *list = NULL;
    HINT32 i = 0, n = 0;
    this->totalSize = 0;

    switch(this->target) {
    case eOPL_SEARCH_TARGET_SCHEDULED:
        list = APK_META_QUERY_Result((ApkMetaQHandle_t)this->context);

        if (list == NULL) {
            HxLOG_Debug("[%s:%d] has no result!!!\n", __HxFILE__, __HxLINE__);
        }
        else {
            n = HLIB_VECTOR_Length(list);
            for (i = 0 ; i < n ; i++)
            {
                if((i < this->offset) || (i >= (this->offset + this->count))) {
                    continue;
                }

#if defined(CONFIG_OP_JAPAN)
                if((i + 1) < n) {
                    DxEvent_t *event = (DxEvent_t *)HLIB_VECTOR_ItemAt(list, (i+1));
                    if( event && event->duration == 0 ) {
                        continue;
                    }
                }
#endif
                this->totalSize++;
            }

            HLIB_VECTOR_Delete(list);
        }
        break;

        case eOPL_SEARCH_TARGET_ONDEMAND:
            HxLOG_Debug("[%s:%d] eOPL_SEARCH_TARGET_ONDEMAND!!\n", __HxFILE__, __HxLINE__);
            break;

        default:
            HxLOG_Debug("[%s:%d] Only invalid target!!\n", __HxFILE__, __HxLINE__);
            break;
    }

    return this->totalSize;
}
