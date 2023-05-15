/*
* hxsem.c
*
*  Created on: 2011. 6. 01.
*/

#include <hlib.h>

#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"hxmsg"

#include "vkernel.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define	____HxMSG_PRIVATE_define______________________________________________

#define MAX_MSG_SIZE 256


#define	____HxMSG_PRIVATE_Type______________________________________________

typedef struct
{
    HxMSG_Type_e    	 m_type;
    void            *m_sMSG;
} HxMSG_t;

#define MSG_STRUCTSIZE sizeof(HxMSG_t)

typedef struct
{
    HxMSG_Type_e		m_type;
    HxMSG_Callback_t  m_pcallback;
} HxMsgGroup_t;


#define	____HxMSG_PRIVATE_VALUESS______________________________________________
static HxList_t 									*gGroupMSGlist = NULL;
static HULONG 										g_ulMsgGroupId;
static HULONG 										g_ulMsgGroupsema;
#define	____HxMSG_PRIVATE_FUNCTIONS______________________________________________

#if 0
static HULONG local_GetTypeToStructSize(HINT32 type)
{
    HINT32 i;
    HxMsgGroup_t *data=NULL;
    HxList_t *grouplist,*item =NULL;

	if(gGroupMSGlist ==NULL) 	return 0;

    grouplist = gGroupMSGlist;

    for(i=0; i< HLIB_LIST_Length(grouplist); i++)
    {
		item = (HxList_t*)HLIB_LIST_GetListItem(grouplist,i);
		data = (HxMsgGroup_t*)HLIB_LIST_Data(item);
		if(data->m_type ==  type)
		{
			return data->m_ulsize;
		}
    }

    return 0;
}
#endif

static HBOOL local_comapareItemType(void *data, void *userdata)
{
    HxMsgGroup_t *item = (HxMsgGroup_t*)data;
    if (item->m_type  ==  *(HxMSG_Type_e*)userdata)
        return TRUE;
    return FALSE;
}


HxMSG_t local_makeSendMsg(HINT32 type, const void *msg)
{
    HxMSG_t	 sendmsg;

    sendmsg.m_type = type;
	sendmsg.m_sMSG = (HCHAR*)msg;

	return sendmsg;
}



HINT32 local_CreateGroup(void)
{
	(void)VK_SEM_Create(&g_ulMsgGroupsema, "hxmsgsema", VK_SUSPENDTYPE_FIFO);
	return VK_MSG_Create(512, MSG_STRUCTSIZE,"hxmsg",&g_ulMsgGroupId,VK_SUSPENDTYPE_FIFO);
}


#define	____HxMSG_APIs____________________________________________________


HINT32 HLIB_MSG_Create(HULONG q_count, HULONG msg_size, const HCHAR* q_name, HULONG * q_id, HxMSG_SuspendType_e suspendType)
{
    return VK_MSG_Create(q_count, msg_size,q_name,q_id, (VK_SUSPENDTYPE) suspendType);
}

HINT32 HLIB_MSG_Destroy(HULONG q_id)
{
    return VK_MSG_Destroy(q_id);
}


HINT32 HLIB_MSG_Clear(HULONG q_id)
{
    return VK_MSG_Clear(q_id);
}


HINT32 HLIB_MSG_Send(HULONG q_id, const void *msg, HULONG size)
{
    return VK_MSG_Send(q_id,msg,size);
}

HINT32 HLIB_MSG_SendTimeout(HULONG q_id, const void *msg, HULONG size, HULONG timeout)
{
    return VK_MSG_SendTimeout(q_id, msg,size,timeout);
}


HINT32 HLIB_MSG_SendUrgent(HULONG q_id, const void *msg, HULONG size)
{
    return  VK_MSG_SendUrgent(q_id, msg,  size);
}

HINT32 HLIB_MSG_SendUrgentTimeout(HULONG q_id, const void *msg, HULONG size,HULONG timeout)
{
    return  VK_MSG_SendUrgentTimeout(q_id, msg,  size,timeout);
}



HINT32 HLIB_MSG_Receive(HULONG q_id, void *msg, HULONG size)
{
    return  VK_MSG_Receive(q_id, msg,  size);
}

HINT32 HLIB_MSG_ReceiveTimeout(HULONG q_id, void *msg, HULONG size, HULONG timeout)
{
    return VK_MSG_ReceiveTimeout(q_id, msg,  size,  timeout);
}


HINT32 HLIB_MSG_GetMessageCount(HULONG q_id, HULONG *pulMsgCount, HULONG *pulMaxMsgCount)
{
    return VK_MSG_GetMessageCount(q_id,pulMsgCount,pulMaxMsgCount);
}



#define ____HxMSG_GROUPMSG_FUNCTIONS______________________________

void HLIB_MSG_AddGroup(HxMSG_Type_e type,HxMSG_Callback_t callback)
{
    HxMsgGroup_t	 *additem;
	additem = (HxMsgGroup_t*)HLIB_MEM_Malloc(sizeof(HxMsgGroup_t));

    additem->m_type    = type;
    additem->m_pcallback = callback; //리턴 받을 callback

	if(HLIB_LIST_Length(gGroupMSGlist) == 0)
		local_CreateGroup();

	gGroupMSGlist = HLIB_LIST_Append(gGroupMSGlist ,additem );
}


void  HLIB_MSG_DestroyGroup(void)
{
  	if(gGroupMSGlist!=NULL)
		HLIB_LIST_RemoveAll(gGroupMSGlist);
	gGroupMSGlist =NULL;
}


HINT32 HLIB_MSG_SendGroup(HxMSG_Type_e type, const void *msg)
{
    HxMSG_t	 sendmsg;
	HINT32 	 error;
	if(gGroupMSGlist ==NULL)
		return -1;

	VK_SEM_Get(g_ulMsgGroupsema);
	sendmsg = local_makeSendMsg(type,msg);
	error  = HLIB_MSG_Send(g_ulMsgGroupId,&sendmsg,MSG_STRUCTSIZE);
	VK_SEM_Release(g_ulMsgGroupsema);
    return error;
}

HINT32 HLIB_MSG_SendTimeoutGroup(HxMSG_Type_e type, const void *msg,HULONG timeout)
{
    HxMSG_t	 sendmsg;
	HINT32 	 error;
	if(gGroupMSGlist ==NULL)		return -1;
	VK_SEM_Get(g_ulMsgGroupsema);
	sendmsg = local_makeSendMsg(type,msg);
	error  = HLIB_MSG_SendTimeout(g_ulMsgGroupId,&sendmsg,MSG_STRUCTSIZE,timeout);
	VK_SEM_Release(g_ulMsgGroupsema);
    return error;

}

HERROR HLIB_MSG_ReceiveGroup(void)
{
    HxList_t 		*item= NULL;
    HxMSG_t	 		receivemsg;
	HxMsgGroup_t	*msgitem;
	if(gGroupMSGlist == NULL) return ERR_FAIL;

    HLIB_MSG_Receive(g_ulMsgGroupId, &receivemsg,  MSG_STRUCTSIZE);

	item = HLIB_LIST_ForeachEx(gGroupMSGlist,local_comapareItemType,&(receivemsg.m_type));
	if (item == NULL)
		return ERR_FAIL;

	msgitem = (HxMsgGroup_t*)HLIB_LIST_Data(item);
	if (msgitem == NULL)
		return ERR_FAIL;

	msgitem->m_pcallback(receivemsg.m_sMSG);
	gGroupMSGlist = HLIB_LIST_First(gGroupMSGlist);

	return ERR_OK;
}

HERROR HLIB_MSG_ReceiveTimeoutGroup(HULONG timeout)
{
    HxList_t 		*item= NULL;
    HxMSG_t	 		receivemsg;
	HxMsgGroup_t	*msgitem=NULL;

	if(gGroupMSGlist == NULL) return ERR_FAIL;

    if(HLIB_MSG_ReceiveTimeout(g_ulMsgGroupId, &receivemsg,  MSG_STRUCTSIZE,timeout) == VK_OK)
    {
		item = HLIB_LIST_ForeachEx(gGroupMSGlist,local_comapareItemType,&(receivemsg.m_type));
		if(item == NULL)
		{
			return ERR_FAIL;
		}
		msgitem = (HxMsgGroup_t*)HLIB_LIST_Data(item);

		if(msgitem == NULL)
		{
			return ERR_FAIL;
		}

		msgitem->m_pcallback(receivemsg.m_sMSG);
		gGroupMSGlist = HLIB_LIST_First(gGroupMSGlist);

		return ERR_OK;
	}
	return ERR_FAIL;
}


#define _______________________________________________

/* end of file */

