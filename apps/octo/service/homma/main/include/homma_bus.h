/*
 **************************************************************
 *	@file		hom_bus.h
 *	HumaxDataManager internal header for other applications.
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013/04/12
 *	@author			humax
 *	@brief			homma internal header
 *************************************************************
 */

#include <hlib.h>
#include "common.h"

#ifndef	__HOMMA_BUS_H__
#define	__HOMMA_BUS_H__

/**************************************************************
	internal structures and variables
 **************************************************************/
typedef enum {
	eBUS_TARGET_NONE = 0x00,
	eBUS_TARGET_SYS,
	eBUS_TARGET_SERVER,

	eBUS_TARGET_MOD_MIN,

	eBUS_TARGET_MOD_DMS,
	eBUS_TARGET_MOD_DMP,
	eBUS_TARGET_MOD_DMR,
	eBUS_TARGET_MOD_WOONS,
	eBUS_TARGET_MOD_DIAL,
	eBUS_TARGET_MOD_AIRPLAY,
	eBUS_TARGET_MOD_SATIP,
	eBUS_TARGET_MOD_RC,

	eBUS_TARGET_MOD_MAX,

	eBUS_TARGET_MOD_ALL,
	eBUS_TARGET_ALL,

	eBUS_TARGET_MAX,
} HOMMA_BUS_Target_e;

typedef enum {
	eBUS_MSGTYPE_EVENT = 0x00,
	eBUS_MSGTYPE_REQUEST,

	eBUS_MSGTYPE_RESPONSE,
	eBUS_MSGTYPE_COMMAND,
	eBUS_MSGTYPE_JSON,
	eBUS_MSGTYPE_UNKNOWN,
	eBUS_MSGTYPE_MAX,
} HOMMA_BUS_MsgType_e;

typedef enum {
	eBUS_CMD_NONE = 0x00,
	eBUS_CMD_START,
	eBUS_CMD_STOP,
	eBUS_CMD_GET,
	eBUS_CMD_SET,
	eBUS_CMD_STATUS,
	eBUS_CMD_UNKNOWN,
} HOMMA_BUS_Cmd_e;

typedef enum {

	eBUS_REQUEST_SERVER_SENDING,
	eBUS_REQUEST_NET_INFO,
	eBUS_REQUEST_STORAGE_INFO,
	eBUS_REQUEST_SYS_INFO,

	eBUS_REQUEST_JSON_DECODE,
	eBUS_REQUEST_IPC_DECODE,
	eBUS_REQUEST_IPC_ENCODE,

	eBUS_REQUEST_MODUL_INFO,
	eBUS_REQUEST_MODUL_RUN,

	eBUS_REQUEST_UNKNOWN,

	eBUS_REQUEST_MODUL_DEFINE,
} HOMMA_BUS_Request_e;

typedef HOMMA_BUS_Request_e HOMMA_BUS_Response_e;

typedef enum {
	eBUS_DATATYPE_NULL,
	eBUS_DATATYPE_INT,
	eBUS_DATATYPE_FLOAT,
	eBUS_DATATYPE_STR,
	eBUS_DATATYPE_BOOL,
	eBUS_DATATYPE_JSON,
	eBUS_DATATYPE_PARAM,
	eBUS_DATATYPE_BUSDATA,
	eBUS_DATATYPE_USER_DEF,
	eBUS_DATATYPE_IPC_SEDN,
	eBUS_DATATYPE_IPC_RECV,
	eBUS_DATAtYPE_UNKNOWN,
	eBUS_DATAtYPE_MAX,
} HOMMA_BUS_DataType_e;

typedef struct {
	HINT32 nPairId;

	HOMMA_BUS_DataType_e  eDataType;
	HINT32	nDataSize;
	HBOOL	bNeedFree;
	void 	*pvData;

	HINT32	nOption; // client define.
	HINT32	nClientHandle;
} HOMMA_BUS_Data_t;

typedef struct {
	HINT32 nPairId;

	HBOOL	bMalloc;
	void 	*pvJson;

	HINT32	nOpCode; // set, get.
	HINT32	nCmd;
	HINT32	nClientHandle;
} HOMMA_BUS_IpcRecv_t;

typedef struct {
	HINT32 nPairId;

	HBOOL	bMalloc;
	HCHAR 	*pszJson;

	HINT32	nOpCode; // set, get.
	HINT32	cmd;
	HINT32	nClientHandle;

	void	*data;
	HUINT32	size;
} HOMMA_BUS_IpcSend_t;

#if 0 // PH2 ??
typedef struct {
	HINT32 	nReqId;

	HINT32	nClientSock; // if 0, Broadcasting
	HINT32	nDataSize;
	HBOOL	bNeedFree;
	void 	*pvData; // HxJSON_t

	HINT32	nOpCode; // get, set, status.

} HOMMA_BUS_IpcData_t;
#endif

typedef struct {

	HINT32	nParam1;
	HINT32	nParam2;
	HINT32	nParam3;

} HOMMA_BUS_PramData_t;

typedef struct {

	HOMMA_BUS_DataType_e  eDataType;
	HINT32	nDataSize;
//	HBOOL	bNeedFree;
	void 	*pvData;

} HOMMA_BUS_MsgEvt_t;

typedef struct {
	HINT32 	nReqId;

	HOMMA_BUS_DataType_e  eDataType;
	HINT32	nDataSize;
	HBOOL	bNeedFree;
	void 	*pvData;

} HOMMA_BUS_MsgData_t;

typedef HOMMA_BUS_Data_t HOMMA_BUS_MsgReq_t;
typedef HOMMA_BUS_Data_t HOMMA_BUS_MsgRes_t;
typedef HOMMA_BUS_Data_t HOMMA_BUS_MsgCmd_t;

typedef struct {
	HOMMA_BUS_MsgType_e eMsgType;

	union {
		HOMMA_BUS_Event_e	eEvt;
		HOMMA_BUS_Cmd_e eCmd;
		HOMMA_BUS_Request_e eReq;
		HOMMA_BUS_Response_e eRes;
	} uEnum;

	// Bus ���� ��...
	//HOMMA_BUS_MsgData_t stData;
#if 1
	HINT32 	nReqId;
	HOMMA_BUS_DataType_e  eDataType;
	HINT32	nDataSize;
	HBOOL	bNeedFree;
	void 	*pvData;
#endif

} HOMMA_BUS_Msg_t;

typedef HINT32 (*BUS_MSG_HANDLE_Func)( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_Msg_t *pstMsg );
typedef HINT32 (*BUS_REQ_HANDLE_Func)( HOMMA_BUS_Request_e eReq, HOMMA_BUS_Data_t *pstRetData );
//typedef HINT32 (*BUS_REL_HANDLE_Func)( HOMMA_BUS_MsgType_e eMsgType, HOMMA_BUS_Data_t *pstData );
typedef HINT32 (*BUS_REL_HANDLE_Func)( HOMMA_BUS_Target_e eSender, HOMMA_BUS_Msg_t *pstMsg );
//typedef HINT32 (*BUS_EVT_HANDLE_Func)( HINT32 event, HINT32 p1, HINT32 p2, HINT32 p3, HOMMA_BUS_Target_e eSender );
typedef HINT32 (*BUS_EVT_HANDLE_Func)( HINT32 event, const HOMMA_BUS_MsgEvt_t *pstMsgEvt, HOMMA_BUS_Target_e eSender );

typedef struct {
	BUS_MSG_HANDLE_Func	cbMsgHandleFunc;
	BUS_REQ_HANDLE_Func	cbReqHandleFunc;
	BUS_REL_HANDLE_Func	cbRelHandleFunc;
} HOMMA_BUS_HndFuncList_t;

typedef	void * BUS_Handle;

/**************************************************************
	internal functions
 **************************************************************/

/**
 * BUS�� �ʱ�ȭ �ϴ� API.\n
 * ���������� TAKSK ����.
 * @return                    	void.
 */
void	HOMMA_BUS_Init( void );

/**
 * BUS ���������� ������ �޸𸮸� �����ϴ� API.\n
 * ���������� TAKSK ����.
 * @return                    	void.
 */
void	HOMMA_BUS_DeInit( void );

/**
 * BUS�� �����ϴ� ������ �����ϱ� ���� Handle ���� Bus���� ����.\n
 * �� ����� target �ش��ϴ� enum ������ �ڽ��� Handlle ���� ���� �� ����.\n
 * ������ : Ÿ ��⿡���� enum ������ �ش� Handle�� ȹ���� �� ����.
 * @param[in] 	eTarget     	�ش� ���� ��Ī�Ǵ� enum ��.
 * @return                    	BUS Handle ��. NULL�̸� ����� �� ���� Enum���� ��ϵ� ���.
 */
BUS_Handle	HOMMA_BUS_GetHandle( HOMMA_BUS_Target_e eTarget );

/**
 * BUS�� �ش� ����� GetHanle Func�� ���� ���� Handle������ ���.\n
 * @param[in] 	pstHndFuncList     	Message�� Request�� ó���ϱ� ���� Callbakc �Լ�.
 * @return                    	ERR_OK or ERR_FAIL.
 */
HINT32	HOMMA_BUS_Register( BUS_Handle hHandle, const HOMMA_BUS_HndFuncList_t *pstHndFuncList );

/**
 * BUS���� ����  Handle ������ ��ϵ� ����� ����.\n
 * @return                    	ERR_OK or ERR_FAIL.
 */
HINT32	HOMMA_BUS_UnRegister( BUS_Handle hHandle );

/**
 * Ÿ�� ����� ������ �̺�Ʈ�� �ش� ��� callbakc���� ���� �� ����.
 * @param[in] 	cbNotify     	�̺�Ʈ�� �޾Ƽ� ó���� Callback �Լ�.
 * @return                    	ERR_OK or ERR_FAIL.
 */
HINT32	HOMMA_BUS_Connect( HOMMA_BUS_Target_e eTarget, BUS_EVT_HANDLE_Func cbNotify );

/**
 * Ÿ�� ����� �̺�Ʈ�� �޴� callbakc �Լ��� ����.\n
 * @param[in] 	cbNotify     	�̺�Ʈ�� �޾Ƽ� ó���� Callback �Լ�.
 * @return                    	ERR_OK or ERR_FAIL.
 */
HINT32	HOMMA_BUS_Disconnect( HOMMA_BUS_Target_e eTarget, BUS_EVT_HANDLE_Func cbNotify );

/**
 * Ÿ�� ���� Message�� ���� �� �ִ� API.
 * @param[in] 	eTarget     	Message ���� ����� Enum ��.
 * @param[in] 	hSrcHandle     	������ ����� bus handle.
 * @param[in] 	pstMsg     		Message( �̺�Ʈ, Request/Respone, Command ).
 * @return                    	ERR_OK or ERR_FAIL.
 */
HINT32	HOMMA_BUS_SendMsg( HOMMA_BUS_Target_e eTarget, BUS_Handle hSrcHandle, const HOMMA_BUS_Msg_t *pstMsg );

/**
 * Ÿ�� ��⿡  Sync Call�� �̸� ���ǵ� request�� Ư�� Data�� �������� API.
 * @param[in] 	eTarget     	�ش� ����� Enum ��.
 * @param[in] 	eReq     		��û�ϴ� Enum ��.
 * @param[in] 	pstRetData    	Sync�� �޴� ������.
 * @param[out] 	pnError    		�ش� ��⿡�� ó�� ���.
 * @return                    	ERR_OK or ERR_FAIL.
 */
HINT32	HOMMA_BUS_Request( HOMMA_BUS_Target_e eTarget, HOMMA_BUS_Response_e eReq, HOMMA_BUS_Data_t *pstRetData, HINT32 *pnError );

#endif
