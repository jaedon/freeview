/*************************************************************
 * @file		hxmsg.h
 * @date		2011/01/19
 * @author		Jin ho Kim (jhkim5@humaxdigital.com)
 * @brief		HUMAX common library header, related with Message functon.
 * http://www.humaxdigital.com
 *************************************************************/

#ifndef _HXMSG_H_
#define _HXMSG_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	eHLIB_SUSPENDTYPE_FIFO,
	eHLIB_SUSPENDTYPE_PRIORITY,
	eHLIB_SUSPENDTYPE_MAX
} HxMSG_SuspendType_e;

typedef enum
{
	eHLIB_MSG_OAPI = 0,
	eHLIB_MSG_OPL,
	eHLIB_MSG_ONDK,
	eHLIB_MSG_DAPI,
	eHLIB_MSG_UAPI,

	eHLIB_MSG_MAX
} HxMSG_Type_e;

typedef void (* HxMSG_Callback_t)(void *arg);

HINT32 HLIB_MSG_Create (HULONG q_count, HULONG msg_size, const HCHAR* q_name, HULONG * q_id, HxMSG_SuspendType_e suspendType);
HINT32 HLIB_MSG_Destroy (HULONG q_id);
HINT32 HLIB_MSG_Clear (HULONG q_id);
HINT32 HLIB_MSG_Send (HULONG q_id, const void *msg, HULONG size);
HINT32 HLIB_MSG_SendTimeout (HULONG q_id, const void *msg, HULONG size, HULONG timeout);
HINT32 HLIB_MSG_SendUrgent (HULONG q_id, const void *msg, HULONG size);
HINT32 HLIB_MSG_SendUrgentTimeout (HULONG q_id, const void *msg, HULONG size,HULONG timeout);
HINT32 HLIB_MSG_Receive (HULONG q_id, void *msg, HULONG size);
HINT32 HLIB_MSG_ReceiveTimeout (HULONG q_id, void *msg, HULONG size, HULONG timeout);
HINT32 HLIB_MSG_GetMessageCount (HULONG q_id, HULONG *pulMsgCount, HULONG *pulMaxMsgCount);

/*
* Main task 에서 하나의 message reciver 만을 두고자 할때 사용하는 함수군이다.
* HLIB_MSG_AddGroup : 데이터를 받을 콜백과 사이즈, send 할때 보낼  type 을 적어 준다.
* HLIB_MSG_SendGroup /HLIB_MSG_SendTimeoutGroup : HLIB_MSG_AddGroup 할때의 type 을 메시지를 보낸다.
* HxMSG_RecevieGroup / HLIB_MSG_ReceiveTimeoutGroup: 등록된 모든 메시지를 받는다.
*/
void 	HLIB_MSG_AddGroup (HxMSG_Type_e type, HxMSG_Callback_t callback);
void	HLIB_MSG_DestroyGroup (void);
HINT32	HLIB_MSG_SendGroup (HxMSG_Type_e type, const void *msg);
HINT32	HLIB_MSG_SendTimeoutGroup (HxMSG_Type_e type, const void *msg, HULONG timeout);
HERROR	HLIB_MSG_ReceiveGroup (void);
HERROR	HLIB_MSG_ReceiveTimeoutGroup (HULONG timeout);


#ifdef __cplusplus
}
#endif


#endif

