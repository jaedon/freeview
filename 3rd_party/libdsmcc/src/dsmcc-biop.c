#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <syslog.h>

#include "dsmcc-descriptor.h"
#include "dsmcc-cache.h"
#include "dsmcc-biop.h"
#include "dsmcc-receiver.h"
#include "libdsmcc.h"

#define CALCULATE_SVC_CTXT

/*
struct biop_stream *
	dsmcc_biop_process_strream() { ; }

struct biop_streamevent *
	dsmcc_biop_process_event() { ; }

*/


/*******************************************************************/
/********************	   Extern function Prototype	 ********************/
/*******************************************************************/
extern void dsmcc_cache_dir_info(Cache_All_t *pstCacheAll, unsigned short module_id, unsigned int objkey_len, char *objkey, BIOP_Binding_t *pstBiopBinding, unsigned int comp_idx);
extern void dsmcc_cache_file_info(Cache_All_t *pstCacheAll, unsigned short mod_id, unsigned int key_len, char *key, BIOP_Binding_t *pstBiopBinding, unsigned int comp_idx);
extern void dsmcc_cache_event_info(Cache_All_t *pstCacheAll, unsigned short mod_id, unsigned int key_len, char *key, BIOP_Binding_t *pstBiopBinding, unsigned int comp_idx);
extern void dsmcc_cache_file(Cache_All_t *pstCacheAll, BIOP_Message_t *pstBiopMsg, CacheModuleData_t *pstCacheModuleData);
extern void dsmcc_cache_event (Cache_All_t *pstCacheAll, BIOP_Message_t *pstBiopMsg, CacheModuleData_t *pstCacheModuleData);

/*******************************************************************/
/********************	   Static function Prototype	 ********************/
/*******************************************************************/
#define _____STATIC_FUCTIONS_PROTOTYPE_____
static int dsmcc_biop_process_name_comp(BIOP_NameComponent_t*pstBiopNameComponent,unsigned char *pucRawData);
static int dsmcc_biop_process_name(BIOP_Name_t *pstBiopName, unsigned char *pucRawData) ;
static int dsmcc_biop_process_binding(BIOP_Binding_t *pstBiopBinding, unsigned char *pucRawData) ;
static void dsmcc_biop_free_binding (BIOP_Binding_t *pstBiopBinding);
static int dsmcc_biop_process_ConnBinder(BIOP_DsmConnbinder_t *pstConnBinder, unsigned char *pucRawData);
static int dsmcc_biop_process_ObjectLocation(BIOP_ObjLocation_t *pstObjLocation, unsigned char *pucRawData) ;
static int dsmcc_biop_process_BIOPProfileBody(BIOP_ProfileBody_t *pstProfileBody, unsigned char *pucRawData) ;
static int dsmcc_biop_process_MessageHeader(BIOP_Message_t *pstBiopMsg, CacheModuleData_t *pstCacheModuleData);
static void dsmcc_biop_process_MessageDirectory (BIOP_Message_t *pstBiopMsg, CacheModuleData_t*pstCacheModuleData, Cache_All_t *pstCacheAll);
static void dsmcc_biop_process_MessageFile(BIOP_Message_t *pstBiopMsg,CacheModuleData_t *pstCacheModuleData, Cache_All_t *pstCacheAll);
static int dsmcc_biop_process_MessageServiceGateWay(BIOP_Message_t *pstBiopMsg, CacheModuleData_t *pstCacheModuleData, Cache_All_t *pstCacheAll);
static void dsmcc_biop_process_MessageStreamEvent(BIOP_Message_t *pstBiopMsg,CacheModuleData_t *pstCacheModuleData, Cache_All_t *pstCacheAll);


/*******************************************************************/
/********************      Static Functions   ********************/
/*******************************************************************/

#define _____STATIC_FUCTIONS_____

static int dsmcc_biop_process_name_comp(BIOP_NameComponent_t*pstBiopNameComponent,unsigned char *pucRawData)
{
	int offset = 0;

	pstBiopNameComponent->id_len = pucRawData[offset++];

	if(pstBiopNameComponent->id_len >0)
	{
		pstBiopNameComponent->id = (char *)malloc(pstBiopNameComponent->id_len + 1);
		if(pstBiopNameComponent->id != NULL)
		{
			memcpy(pstBiopNameComponent->id, pucRawData+offset, pstBiopNameComponent->id_len);
			pstBiopNameComponent->id[pstBiopNameComponent->id_len] = '\0';
		}
	}

	offset+=pstBiopNameComponent->id_len;

	pstBiopNameComponent->kind_len = pucRawData[offset++];

	if(pstBiopNameComponent->kind_len >0)
	{
		pstBiopNameComponent->kind = (char *)malloc(pstBiopNameComponent->kind_len + 1);
		if(pstBiopNameComponent->kind  != NULL)
		{
			memcpy(pstBiopNameComponent->kind, pucRawData+offset, pstBiopNameComponent->kind_len);
			pstBiopNameComponent->kind[pstBiopNameComponent->kind_len] = '\0';
		}
	}
	offset+= pstBiopNameComponent->kind_len;

	return offset;
}

static int dsmcc_biop_process_name(BIOP_Name_t *pstBiopName, unsigned char *pucRawData)
{
	int i, offset = 0, ret;

	pstBiopName->component_count = pucRawData[0];
	offset++;

	pstBiopName->comps = (BIOP_NameComponent_t*) malloc(sizeof(BIOP_NameComponent_t) * pstBiopName->component_count);
	if(pstBiopName->comps == NULL)
	{
		return offset;
	}

	for(i = 0; i < pstBiopName->component_count; i++)
	{
		ret = dsmcc_biop_process_name_comp(&pstBiopName->comps[i], pucRawData+offset);
		if(ret > 0) { offset += ret; } else { /* TODO error */ }
	}

	return offset;
}

static int dsmcc_biop_process_binding(BIOP_Binding_t *pstBiopBinding, unsigned char *pucRawData)
{
	int offset = 0, ret;

	ret = dsmcc_biop_process_name(&pstBiopBinding->stBiopName, pucRawData);
	if(ret > 0)
	{
		offset += ret;
	}
	else
	{
		return -1;
	}

	pstBiopBinding->binding_type = pucRawData[offset++];

	ret = dsmcc_biop_process_Ior(&pstBiopBinding->stBiopIor, pucRawData+offset);
	if(ret > 0)
	{
		offset += ret;
	}
	else
	{
		return -1;
	}

	pstBiopBinding->objinfo_len = (pucRawData[offset] << 8) | pucRawData[offset+1];

	offset+=2;

	if(pstBiopBinding->objinfo_len > 0)
	{
		pstBiopBinding->objinfo = (char *)malloc(pstBiopBinding->objinfo_len);
		if(pstBiopBinding->objinfo != NULL)
		{
			memcpy(pstBiopBinding->objinfo, pucRawData+offset, pstBiopBinding->objinfo_len);
		}
	}
	else
	{
		pstBiopBinding->objinfo = NULL;
	}

	offset+= pstBiopBinding->objinfo_len;

	return offset;
}

static void dsmcc_biop_free_binding (BIOP_Binding_t *pstBiopBinding)
{
	int i;

	for(i = 0; i < pstBiopBinding->stBiopName.component_count; i++)
	{
		if(pstBiopBinding->stBiopName.comps[i].id != NULL)
		{
			free(pstBiopBinding->stBiopName.comps[i].id);
			pstBiopBinding->stBiopName.comps[i].id = NULL;
		}
		if(pstBiopBinding->stBiopName.comps[i].kind != NULL)
		{
			free(pstBiopBinding->stBiopName.comps[i].kind);
			pstBiopBinding->stBiopName.comps[i].kind = NULL;
		}
	}

	if(pstBiopBinding->stBiopName.comps != NULL)
	{
		free(pstBiopBinding->stBiopName.comps);
		pstBiopBinding->stBiopName.comps = NULL;
		pstBiopBinding->stBiopName.component_count = 0;
	}

	if(pstBiopBinding->stBiopIor.type_id != NULL)
	{
		free(pstBiopBinding->stBiopIor.type_id);
		pstBiopBinding->stBiopIor.type_id = NULL;
		pstBiopBinding->stBiopIor.type_id_length = 0;
	}

	if(pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey != NULL)
	{
		free(pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey);
		pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey = NULL;
		pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey_len = 0;
	}

	if(pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopDsmConnbiner.stBiopTap.selector_data != NULL)
	{
		free(pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopDsmConnbiner.stBiopTap.selector_data);
		pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopDsmConnbiner.stBiopTap.selector_data = NULL;
		pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopDsmConnbiner.stBiopTap.selector_len = 0;
	}

	if(pstBiopBinding->objinfo != NULL)
	{
		free(pstBiopBinding->objinfo);
		pstBiopBinding->objinfo = NULL;
		pstBiopBinding->objinfo_len = 0;
	}
}

// Figure F-2. The structure of the IOR with BIOP ProfileBody inside
int dsmcc_biop_process_tap(BIOP_Tap_t *pstBiopTap, unsigned char *pucRawData)
{
	int offset=0;

	// id
	pstBiopTap->id = (pucRawData[0] << 8) | pucRawData[1];
	offset+=2;

	// use
	pstBiopTap->use = (pucRawData[offset] << 8) | pucRawData[offset+1];
	offset+=2;

	// assocTag
	pstBiopTap->assoc_tag = (pucRawData[offset] << 8) | pucRawData[offset+1];
	offset+=2;

	// selector length
	pstBiopTap->selector_len = pucRawData[offset++];

	if(pstBiopTap->selector_len > 0)
	{
		pstBiopTap->selector_data = (char *)malloc(pstBiopTap->selector_len);
		if(pstBiopTap->selector_data == NULL)
		{
			offset+=pstBiopTap->selector_len;
			return offset;
		}

		memcpy(pstBiopTap->selector_data, pucRawData+offset, pstBiopTap->selector_len);
	}
	else
	{
		pstBiopTap->selector_data = NULL;
	}

	offset+=pstBiopTap->selector_len;

	return offset;
}

// Figure F-2. The structure of the IOR with BIOP Profile Body inside
int dsmcc_biop_process_ConnBinder(BIOP_DsmConnbinder_t *pstConnBinder, unsigned char *pucRawData)
{
	int offset = 0, ret;

	// ConnBinder component tag (0x49534f40)
	pstConnBinder->component_tag = ((pucRawData[0] << 24) | (pucRawData[1] << 16) | (pucRawData[2] << 8)  | pucRawData[3]);
	offset+=4;

	if(pstConnBinder->component_tag != 0x49534f40)
	{
		return -1;
	}

	// Connbinder component data length
	pstConnBinder->component_data_len = pucRawData[offset];
	offset +=1;

	// tap count in Connbinder Component Tag
	pstConnBinder->taps_count = pucRawData[offset];
	offset +=1;

	ret = dsmcc_biop_process_tap(&pstConnBinder->stBiopTap, pucRawData+offset);
	if(ret > 0)
	{
		offset += ret;
	}
	else
	{
		/* TODO error */
	}

	return offset;
}

// Figure F-2. The structure of the IOR with BIOP ProfileBody inside
int dsmcc_biop_process_ObjectLocation(BIOP_ObjLocation_t *pstObjLocation, unsigned char *pucRawData)
{
	int offset = 0;

	// objectLocation component Tag (0x49534f50)
	pstObjLocation->component_tag = (pucRawData[0] << 24) | (pucRawData[1] << 16) | (pucRawData[2] << 8)  | pucRawData[3];
	offset+=4;

	if(pstObjLocation->component_tag != 0x49534F50)
	{
		return -1;
	}

	pstObjLocation->component_data_len = pucRawData[offset];
	offset +=1;

	pstObjLocation->carousel_id = (pucRawData[offset] << 24) | (pucRawData[offset+1] << 16) |  (pucRawData[offset+2] << 8)  | pucRawData[offset+3];
	offset+=4;

	pstObjLocation->module_id = (pucRawData[offset] << 8) | pucRawData[offset+1];
	offset+=2;

	pstObjLocation->version_major = pucRawData[offset];
	offset+=1;

	pstObjLocation->version_minor = pucRawData[offset];
	offset+=1;

	pstObjLocation->objkey_len = pucRawData[offset];
	offset+=1;

	if(pstObjLocation->objkey_len > 0)
	{
		pstObjLocation->objkey = (char *)malloc(pstObjLocation->objkey_len);
		if(pstObjLocation->objkey == NULL)
		{
			offset+=pstObjLocation->objkey_len;
			return offset;
		}

		memcpy(pstObjLocation->objkey, pucRawData+offset, pstObjLocation->objkey_len);
	}
	else
	{
		pstObjLocation->objkey = NULL;
	}

	offset+=pstObjLocation->objkey_len;

	return offset;
}

/* 11.3 Broadcast inter ORB Protocol
*  	-> 11.3.1 Profile Body Definition. 13818-6
* Profile Body = BIOP::ObjectLocation + DSM::ConnBinder = DSM::LiteComponent
* A BIOP Profile Body shall contain exactly one component of type BIOP::ObjectLocation.
* A BIOP Profile Body shall contain exactly one component of type DSM::ConnBinder.
*/
int dsmcc_biop_process_BIOPProfileBody(BIOP_ProfileBody_t *pstProfileBody, unsigned char *pucRawData)
{
	int offset = 0, ret;

	// profile_data_byte_order:8bit
	offset+=1;

	// lite component profile count in body profile profile: 8
	pstProfileBody->lite_components_count = pucRawData[offset];
	offset+=1;

	// BIOP::ObjectLocation
	ret = dsmcc_biop_process_ObjectLocation(&pstProfileBody->stBiopObjLocation, pucRawData+offset);
	if(ret < 0)
	{
		return -1;
	}
	offset += ret;

	// DSM::Connbinder
	ret = dsmcc_biop_process_ConnBinder(&pstProfileBody->stBiopDsmConnbiner, pucRawData+offset);
	if(ret < 0)
	{
		return -1;
	}
	offset += ret;

	return offset;
}

static int dsmcc_biop_process_MessageHeader(BIOP_Message_t *pstBiopMsg, CacheModuleData_t *pstCacheModuleData)
{
	BIOP_Message_Header_t *pstBiopMsgHeader = &pstBiopMsg->stBiopMsgHeader;
	unsigned char *pucModuleData = pstCacheModuleData->pucModuleData + pstCacheModuleData->ulReadPosition;
	int offset = 0;

	if(pucModuleData == NULL)
	{
		return -1;
	}

	if(pucModuleData[0] !='B' || pucModuleData[1] !='I' || pucModuleData[2] !='O' || pucModuleData[3] !='P')
	{
		return -2;
	}

	offset+=4;/* skip magic */

	pstBiopMsgHeader->version_major = pucModuleData[offset++];
	pstBiopMsgHeader->version_minor = pucModuleData[offset++];

	offset+=2; /* skip byte order & message type */

	pstBiopMsgHeader->message_size  = (pucModuleData[offset] << 24) | (pucModuleData[offset+1] << 16) | (pucModuleData[offset+2] << 8)  | pucModuleData[offset+3];
	offset+=4;

	pstBiopMsgHeader->objkey_len = pucModuleData[offset++];

	if(pstBiopMsgHeader->objkey_len >0 )
	{
		pstBiopMsgHeader->objkey = (char *)malloc(pstBiopMsgHeader->objkey_len);
		if(pstBiopMsgHeader->objkey != NULL)
		{
			memcpy(pstBiopMsgHeader->objkey, pucModuleData+offset, pstBiopMsgHeader->objkey_len);
		}
	}

	offset+= pstBiopMsgHeader->objkey_len;

	pstBiopMsgHeader->objkind_len = ((pucModuleData[offset] << 24) | (pucModuleData[offset+1] << 16) | (pucModuleData[offset+2] << 8) | pucModuleData[offset+3]);
	offset+=4;

	if(pstBiopMsgHeader->objkind_len >0 )
	{
		pstBiopMsgHeader->objkind = (char *)malloc(pstBiopMsgHeader->objkind_len);
		if(pstBiopMsgHeader->objkind != NULL)
		{
			memcpy(pstBiopMsgHeader->objkind, pucModuleData+offset, pstBiopMsgHeader->objkind_len);
		}
	}

	offset+= pstBiopMsgHeader->objkind_len;

	pstBiopMsgHeader->objinfo_len = pucModuleData[offset] << 8 | pucModuleData[offset+1];
	offset+=2;

	if(pstBiopMsgHeader->objinfo_len >0 )
	{
		pstBiopMsgHeader->objinfo = (char *)malloc(pstBiopMsgHeader->objinfo_len);
		if(pstBiopMsgHeader->objinfo != NULL)
		{
			memcpy(pstBiopMsgHeader->objinfo, pucModuleData+offset, pstBiopMsgHeader->objinfo_len);
		}
	}

	offset+= pstBiopMsgHeader->objinfo_len;

	pstCacheModuleData->ulNextPosition = pstCacheModuleData->ulReadPosition + pstBiopMsgHeader->message_size + 12;
	pstCacheModuleData->ulReadPosition += offset;

	if (pstCacheModuleData->ulNextPosition <= pstCacheModuleData->ulReadPosition)
	{
		return -1;
	}

	return 0;
}

static void dsmcc_biop_process_MessageDirectory (BIOP_Message_t *pstBiopMsg, CacheModuleData_t*pstCacheModuleData, Cache_All_t *pstCacheAll)
{
	unsigned int i, j;
	int nSvcCntxCnt;
	int offset = 0, ret;

	unsigned char *pucModuleData = pstCacheModuleData->pucModuleData + pstCacheModuleData->ulReadPosition;
	BIOP_BodyDriectory_t	*pBiopBodyDir = &(pstBiopMsg->body.stBiopBodyDirectory) ;
	BIOP_Message_Header_t *pstBiopMsgHeader = &pstBiopMsg->stBiopMsgHeader;

	/* skip service contect count */
	nSvcCntxCnt = (int)(pucModuleData[offset]);
#if defined(CALCULATE_SVC_CTXT)
{
	int		 nIdx;

	offset++;
	/* skip service Context List */
	for (nIdx = 0; nIdx < nSvcCntxCnt; nIdx++)
	{
		int 		 nContextDataLength;

		// context_id
		offset += 4;

		// context_data_length
		nContextDataLength = pucModuleData[offset] << 8 | pucModuleData[offset+1];
		offset += 2;

		// context_data_byte
		offset += nContextDataLength;
	}
}
#else
	offset += (nSvcCntxCnt + 1);
#endif

	pBiopBodyDir->msgbody_len = (pucModuleData[offset] << 24) | (pucModuleData[offset+1] << 16) | (pucModuleData[offset+2] << 8) | pucModuleData[offset+3];
	offset+=4;

	pBiopBodyDir->bindings_count = pucModuleData[offset] << 8 | pucModuleData[offset+1];
	offset+=2;

	for(i = 0; i < pBiopBodyDir->bindings_count; i++)
	{
		ret = dsmcc_biop_process_binding(&pBiopBodyDir->stBiopBinding, pucModuleData+offset);
		if(ret > 0)
		{
		 	offset+= ret;
		}
		else
		{
		 	pstCacheModuleData->ulReadPosition = pstCacheModuleData->ulNextPosition;
			pstCacheAll->num_dirs--;
			return;
		}

		for (j = 0; j < (unsigned int)pBiopBodyDir->stBiopBinding.stBiopName.component_count; j++)
		{
			if (!strcmp("dir",pBiopBodyDir->stBiopBinding.stBiopName.comps[j].kind))
			{
				dsmcc_cache_dir_info(pstCacheAll, pstCacheModuleData->module_id, pstBiopMsgHeader->objkey_len, pstBiopMsgHeader->objkey, &pBiopBodyDir->stBiopBinding, j);
			}
			else if (!strcmp("fil", pBiopBodyDir->stBiopBinding.stBiopName.comps[j].kind))
			{
				dsmcc_cache_file_info(pstCacheAll, pstCacheModuleData->module_id, pstBiopMsgHeader->objkey_len, pstBiopMsgHeader->objkey, &pBiopBodyDir->stBiopBinding, j);
			}
			else if (!strcmp("ste", pBiopBodyDir->stBiopBinding.stBiopName.comps[j].kind))
			{
				dsmcc_cache_event_info(pstCacheAll, pstCacheModuleData->module_id, pstBiopMsgHeader->objkey_len, pstBiopMsgHeader->objkey, &pBiopBodyDir->stBiopBinding, j);
			}
		}

		dsmcc_biop_free_binding(&pBiopBodyDir->stBiopBinding);
	}

	pstCacheModuleData->ulReadPosition += offset;

	pstCacheAll->num_dirs--;
}

static void dsmcc_biop_process_MessageFile(BIOP_Message_t *pstBiopMsg,CacheModuleData_t *pstCacheModuleData, Cache_All_t *pstCacheAll)
{
	int offset = 0;
	int nSvcCntxCnt;
	unsigned char 		*pucModuleData = pstCacheModuleData->pucModuleData+pstCacheModuleData->ulReadPosition;
	BIOP_BodyFile_t	*pBiopBodyFile = &(pstBiopMsg->body.stBiopBodyFile) ;

	/* skip service contect count */
	nSvcCntxCnt = (int)(pucModuleData[offset]);
#if defined(CALCULATE_SVC_CTXT)
	{
		int 	 nIdx;

		offset++;
		/* skip service Context List */
		for (nIdx = 0; nIdx < nSvcCntxCnt; nIdx++)
		{
			int 		 nContextDataLength;

			// context_id
			offset += 4;

			// context_data_length
			nContextDataLength = pucModuleData[offset] << 8 | pucModuleData[offset+1];
			offset += 2;

			// context_data_byte
			offset += nContextDataLength;
		}
	}
#else
	offset += (nSvcCntxCnt + 1);
#endif

	pBiopBodyFile->msgbody_len = (pucModuleData[offset] << 24) | (pucModuleData[offset+1] << 16) | (pucModuleData[offset+2] << 8) | pucModuleData[offset+3];
	offset+=4;

	pBiopBodyFile->content_len = (pucModuleData[offset] << 24) | (pucModuleData[offset+1] << 16) | (pucModuleData[offset+2] << 8)  | pucModuleData[offset+3];
	offset+=4;

	pstCacheModuleData->ulReadPosition += offset;

	dsmcc_cache_file(pstCacheAll, pstBiopMsg, pstCacheModuleData);

	pstCacheModuleData->ulReadPosition += pBiopBodyFile->content_len;
}

static int dsmcc_biop_process_MessageServiceGateWay(BIOP_Message_t *pstBiopMsg, CacheModuleData_t *pstCacheModuleData, Cache_All_t *pstCacheAll)
{
	unsigned int i, j;
	int offset = 0, ret;
	int nSvcCntxCnt;
	unsigned char *pucModuleData = pstCacheModuleData->pucModuleData + pstCacheModuleData->ulReadPosition;
	BIOP_BodyServerGateWay_t	*pBiopBodyServerGateWay = &(pstBiopMsg->body.stBiopBodyServerGateWay) ;

	/* skip service contect count */
	nSvcCntxCnt = (int)(pucModuleData[offset]);
#if defined(CALCULATE_SVC_CTXT)
	{
		int 	 nIdx;

		offset++;
		/* skip service Context List */
		for (nIdx = 0; nIdx < nSvcCntxCnt; nIdx++)
		{
			int 		 nContextDataLength;

			// context_id
			offset += 4;

			// context_data_length
			nContextDataLength = pucModuleData[offset] << 8 | pucModuleData[offset+1];
			offset += 2;

			// context_data_byte
			offset += nContextDataLength;
		}
	}
#else
	offset += (nSvcCntxCnt + 1);
#endif

	pBiopBodyServerGateWay->msgbody_len = (pucModuleData[offset] << 24) | (pucModuleData[offset+1] << 16) | (pucModuleData[offset+2] << 8) | pucModuleData[offset+3];
	offset+=4;

	pBiopBodyServerGateWay->bindings_count = pucModuleData[offset] << 8 | pucModuleData[offset+1];
	offset+=2;

	for (i = 0; i < pBiopBodyServerGateWay->bindings_count; i++)
	{
		ret = dsmcc_biop_process_binding(&pBiopBodyServerGateWay->stBiopBinding, pucModuleData+offset);
		if(ret > 0)
		{
			offset += ret;
		}
		else
		{
		 	pstCacheModuleData->ulReadPosition = pstCacheModuleData->ulNextPosition;
			return -1;
		}

		if ((pBiopBodyServerGateWay->stBiopBinding.stBiopName.component_count > 0) && (NULL != pBiopBodyServerGateWay->stBiopBinding.stBiopName.comps))
		{
			//for (j = 0; j < (unsigned int)bm->body.dir.binding.name.comp_count; j++)

			j = 0;

			if ((pBiopBodyServerGateWay->stBiopBinding.stBiopName.comps[j].kind_len > 0) && (NULL != pBiopBodyServerGateWay->stBiopBinding.stBiopName.comps[j].kind))
			{
				//printf("[dsmcc_biop_process_MessageServiceGateWay] ==== (%s) \n", pBiopBodyServerGateWay->stBiopBinding.stBiopName.comps[j].kind);

				{
					if (!strcmp("dir", pBiopBodyServerGateWay->stBiopBinding.stBiopName.comps[j].kind))
					{
						dsmcc_cache_dir_info (pstCacheAll, 0,0,NULL,&pBiopBodyServerGateWay->stBiopBinding, j);
					}
					else if (!strcmp("fil",pBiopBodyServerGateWay->stBiopBinding.stBiopName.comps[j].kind))
					{
						dsmcc_cache_file_info (pstCacheAll, 0,0,NULL,&pBiopBodyServerGateWay->stBiopBinding, j);
					}
					else if (!strcmp("ste",pBiopBodyServerGateWay->stBiopBinding.stBiopName.comps[j].kind))
					{
						dsmcc_cache_event_info (pstCacheAll, 0,0,NULL,&pBiopBodyServerGateWay->stBiopBinding, j);
					}
				 }
			}
		}


	  	dsmcc_biop_free_binding(&pBiopBodyServerGateWay->stBiopBinding);
	}

	pstCacheModuleData->ulReadPosition += offset;

	return 0;
}

static void dsmcc_biop_process_MessageStreamEvent(BIOP_Message_t *pstBiopMsg,CacheModuleData_t *pstCacheModuleData, Cache_All_t *pstCacheAll)
{
	int offset = 0;
	int nSvcCntxCnt;
	unsigned char *pucModuleData = pstCacheModuleData->pucModuleData+pstCacheModuleData->ulReadPosition;
	BIOP_BodyStreamEvent_t	*pBiopBodyStreamEvt = &(pstBiopMsg->body.stBiopBodyStreamEvent) ;

	/* skip service contect count */
	nSvcCntxCnt = (int)(pucModuleData[offset]);
#if defined(CALCULATE_SVC_CTXT)
	{
		int 	 nIdx;

		offset++;
		/* skip service Context List */
		for (nIdx = 0; nIdx < nSvcCntxCnt; nIdx++)
		{
			int 		 nContextDataLength;

			// context_id
			offset += 4;

			// context_data_length
			nContextDataLength = pucModuleData[offset] << 8 | pucModuleData[offset+1];
			offset += 2;

			// context_data_byte
			offset += nContextDataLength;
		}
	}
#else
	offset += (nSvcCntxCnt + 1);
#endif

	pBiopBodyStreamEvt->msgbody_len = (pucModuleData[offset] << 24) | (pucModuleData[offset+1] << 16) | (pucModuleData[offset+2] << 8) | pucModuleData[offset+3];
	offset+=4;

	pBiopBodyStreamEvt->taps_count = (unsigned int)pucModuleData[offset];
	offset++;

	dsmcc_cache_event(pstCacheAll, pstBiopMsg, pstCacheModuleData);

	pstCacheModuleData->ulReadPosition = pstCacheModuleData->ulNextPosition;
}

/*******************************************************************/
/********************      Global Functions     ********************/
/*******************************************************************/
#define _____GLOBAL_FUCTIONS_____

//13818-6 , 11.3.3.2.Module Delivery Parameters
int dsmcc_biop_process_module_info(BIOP_ModuleInfo_t *pstBiopModuleInfo, unsigned char *pucRawData)
{
	int offset, ret;
	unsigned int	ulIndex;

	// module Time out (4byte)
	pstBiopModuleInfo->mod_timeout	= (pucRawData[0] << 24) | (pucRawData[1] << 16) | (pucRawData[2] << 8) | pucRawData[3];

	// block Time Out(4byt4)
	pstBiopModuleInfo->block_timeout	= (pucRawData[4] << 24) | (pucRawData[5] << 16) | (pucRawData[6] << 8) | pucRawData[7];

	// min Block Time (4byte)
	pstBiopModuleInfo->min_blocktime	= (pucRawData[8] << 24) | (pucRawData[9] << 16) | (pucRawData[10] << 8) | pucRawData[11];

	// Tap Count(1byte)
	pstBiopModuleInfo->taps_count	= pucRawData[12];
	offset = 13;

	if(pstBiopModuleInfo->taps_count > 0)
	{
		for(ulIndex= 0; ulIndex<pstBiopModuleInfo->taps_count; ulIndex++)
		{
			/* only 1 allowed TODO - may not be first though ? */
			ret = dsmcc_biop_process_tap(&pstBiopModuleInfo->stBiopTap, pucRawData+offset);
			if(ret > 0)
			{
				offset += ret;
			}
			else
			{
				/* TODO error */
			}
		}
	}

	pstBiopModuleInfo->user_info_len = pucRawData[offset++];
	if(pstBiopModuleInfo->user_info_len > 0)
	{
		pstBiopModuleInfo->descriptors = dsmcc_desc_process(pucRawData+offset,pstBiopModuleInfo->user_info_len,&offset);
	}
	else
	{
		pstBiopModuleInfo->descriptors = NULL;
	}

	return offset;
}

void dsmcc_biop_process_data(Cache_All_t *pstCacheAll, CacheModuleData_t *pstCacheModuleData)
{
	BIOP_Message_t stBiopMsg;
	DsmccDes_t 	*pstDsmccDesc = NULL;
	int ret;
	unsigned int len;

	for(pstDsmccDesc = pstCacheModuleData->pstDsmccDescriptors; pstDsmccDesc != NULL; pstDsmccDesc=pstDsmccDesc->next)
	{
		if(pstDsmccDesc->tag == TAG_COMPRESSED_MODULE) { break; }
	}

	len = (pstDsmccDesc != NULL) ? pstDsmccDesc->data.compressed.original_size : pstCacheModuleData->module_size;

	memset(&stBiopMsg, 0x00, sizeof(BIOP_Message_t));

	pstCacheModuleData->ulReadPosition = 0;

	/* Replace off with cachep->curp.... */
	while(pstCacheModuleData->ulReadPosition < len)
	{
		/* Parse header */
		ret = dsmcc_biop_process_MessageHeader(&stBiopMsg, pstCacheModuleData);
		if(ret < 0)			{ break; }

		if(stBiopMsg.stBiopMsgHeader.objkind != NULL)
		{
			/* Handle each message type */
			if (strcmp(stBiopMsg.stBiopMsgHeader.objkind, "fil") == 0)
			{
				dsmcc_biop_process_MessageFile(&stBiopMsg, pstCacheModuleData, pstCacheAll);
			}
			else if(strcmp(stBiopMsg.stBiopMsgHeader.objkind, "dir") == 0)
			{
				dsmcc_biop_process_MessageDirectory(&stBiopMsg, pstCacheModuleData, pstCacheAll);
			}
			else if(strcmp(stBiopMsg.stBiopMsgHeader.objkind, "srg") == 0)
			{
				dsmcc_biop_process_MessageServiceGateWay(&stBiopMsg, pstCacheModuleData, pstCacheAll);
			}
#if 0
			else if(strcmp(stBiopMsg.stBiopMsgHeader.objkind, "str") == 0)
			{
				dsmcc_biop_process_stream(&bm, cachep);
			}
#endif
			else if(strcmp(stBiopMsg.stBiopMsgHeader.objkind, "ste") == 0)
			{
				dsmcc_biop_process_MessageStreamEvent(&stBiopMsg, pstCacheModuleData, pstCacheAll);
			}
			else
			{
				pstCacheModuleData->ulReadPosition = pstCacheModuleData->ulNextPosition;
			}
		}

		if(stBiopMsg.stBiopMsgHeader.objkey !=NULL)
		{
			free(stBiopMsg.stBiopMsgHeader.objkey);
			stBiopMsg.stBiopMsgHeader.objkey = NULL;
		}

		if(stBiopMsg.stBiopMsgHeader.objkind !=NULL)
		{
			free(stBiopMsg.stBiopMsgHeader.objkind);
			stBiopMsg.stBiopMsgHeader.objkind = NULL;
		}

		if(stBiopMsg.stBiopMsgHeader.objinfo !=NULL)
		{
			free(stBiopMsg.stBiopMsgHeader.objinfo);
			stBiopMsg.stBiopMsgHeader.objinfo = NULL;
		}

		pstCacheModuleData->ulReadPosition = pstCacheModuleData->ulNextPosition;
	}

}

// 13818-6 Figure F-2. The structure of the IOR with BIOP ProfileBody inside 참조.
/* 13818-6 5.6.3 The Object Reference 참조
	1. Profile Tags 0x49534F00 ~ 0x49534F0F (the first 3 octets spell “ISO”)
	 	- Min Protocol profile (TAG_MIN)  : 1230196480 (0x49534F00)
		- Child Protocol Profile (TAG_CHILD) : 1230196481 (0x49534F01)
		- Options Protocol Profile (TAG_OPTIONS) : 1230196482 (0x49534F02)
		- Lite Protocol Profile
			-> TAG_LITE_MIN : 1230196483 (0x49534F03)
			-> TAG_LITE_CHILD : 1230196484 (0x49534F04)
			-> TAG_LITE_OPTIONS : 1230196485 (0x49534F05)
		- BIOP Protocol Profile (TAG_BIOP) : : 1230196486 (0x49534F06)
	2. Component Tags
*/
int dsmcc_biop_process_Ior(BIOP_Ior_t *pstIor, unsigned char *pucRawData)
{
	int biop_parsed = 0;
	int offset = 0, ret = 0 ;
	int alignment_gap = 0 ;
	unsigned long i, profile_id_tag, profile_data_length = 0;

	// type_id_length
	pstIor->type_id_length = ((pucRawData[0] << 24) | (pucRawData[1] << 16) | (pucRawData[2] << 8)  | (pucRawData[3]));
	offset+=4;

	// type_id
	pstIor->type_id = (char *)malloc(pstIor->type_id_length + 1);
	if(pstIor->type_id == NULL)
	{
		//mollc 실패시 어찌할까요..우선 error return 함.
		return -1;
		//return ior->type_id_len;
	}

	memcpy(pstIor->type_id, pucRawData+offset, pstIor->type_id_length);
	pstIor->type_id[pstIor->type_id_length] = '\0';
	offset+=pstIor->type_id_length;

	// alignment gap  (CDR alignment rule)
	alignment_gap = pstIor->type_id_length % 4;
	if (alignment_gap)
	{
		offset += 4-alignment_gap;
	}

	// 13818-6 스펙상 1개일 듯 하다..
	//그러나 추후에 2개이상의 Profile이 되는 경우에 대한 처리가 필요해 보임.
	pstIor->tagged_profiles_count = (pucRawData[offset] << 24) | (pucRawData[offset+1] << 16) |(pucRawData[offset+2] << 8) | (pucRawData[offset+3]);
	offset+=4;

 	for (i = 0; i < pstIor->tagged_profiles_count; i++)
	{
		profile_id_tag = (pucRawData[offset] << 24) | (pucRawData[offset+1] << 16) |  (pucRawData[offset+2] << 8)  | pucRawData[offset+3];
		offset += 4;

		profile_data_length = (pucRawData[offset] << 24) | (pucRawData[offset+1] << 16) |  (pucRawData[offset+2] << 8)  | pucRawData[offset+3];
		offset += 4;

		// BIOP Protocol Profile만 처리한다..
		// Object Carousel에서는 다른 Profile은 처리하지 않는다...
		if((profile_id_tag & 0xFF) == 0x06) //	BIOP Profile Body (0x49534f06)
		{
			if (biop_parsed == 0)
			{
				pstIor->profile_id_tag = profile_id_tag;
				pstIor->profile.stBiopProfileBody.data_len = profile_data_length;

				ret = dsmcc_biop_process_BIOPProfileBody(&(pstIor->profile.stBiopProfileBody), pucRawData + offset);
				if(ret < 0)
				{
					if(pstIor->type_id != NULL)
					{
						free(pstIor->type_id);
						pstIor->type_id = NULL;
					}
				}

				biop_parsed = 1;
			}
		}
		else if((pstIor->profile_id_tag & 0xFF) == 0x05) //  Lite Options Profile Body (0x49534f05)
		{
		}

		offset += profile_data_length;
	}

	return offset;
}



