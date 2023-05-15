/* $Header: $ */
#ifndef DLIB_CAS_H__
#define DLIB_CAS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>




typedef enum
{
	eDxCAS_GROUPID_NONE		= 0x0000,
	eDxCAS_GROUPID_MG			= 0x0100,
	eDxCAS_GROUPID_VA			= 0x0500,
	eDxCAS_GROUPID_IR			= 0x0600,
	eDxCAS_GROUPID_CX			= 0x0B00,
	eDxCAS_GROUPID_CR			= 0x0D00,
	eDxCAS_GROUPID_NA			= 0x1800,
	eDxCAS_GROUPID_ND			= 0x0900,
	eDxCAS_GROUPID_SATSA		= 0x4400,
	eDxCAS_GROUPID_VX			= 0x5600,	// Verimatrix IPTV Hybrid
	eDxCAS_GROUPID_JP_B_CCAS	= 0xfffd,	// Japan B-CAS, C-CAS
	eDxCAS_GROUPID_COMMON 	= 0xfffe,		// CAS+OP System Message 처리용.
	eDxCAS_GROUPID_CAM		= 0xffff
} DxCAS_GroupId_e;

//=============================>
// MMI
typedef enum
{
	// TODO: web ui 와 enum 맞춰야 함.
	eDxCAS_SESSIONTYPE_NONE		= 0x00000000,

	eDxCAS_SESSIONTYPE_BANNER		= 0x00000002, // web ap에서 open session
	eDxCAS_SESSIONTYPE_MENU		= 0x00000003, // web ap에서 open session
	eDxCAS_SESSIONTYPE_EXTENSION	= 0x00000004, // obama에서 open session
	eDxCAS_SESSIONTYPE_MAX
} DxCAS_MmiSessionType_e;

typedef enum
{
	eDxCASMMI_EVENTTYPE_NONE,
	eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTART,
	eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTOP,
	eDxCASMMI_EVENTTYPE_SESSIONSTARTED,
	eDxCASMMI_EVENTTYPE_SESSIONSTOPPED,
	eDxCASMMI_EVENTTYPE_DISPLAYUI,

	eDxCASMMI_EVENTTYPE_MAX
} DxCAS_MMI_SessionEventType_e;


typedef enum
{
	eDxCasUiAttr_Flashing 			= 0x00000001,
	eDxCasUiAttr_Scrollable		= 0x00000002,
	eDxCasUiAttr_Timeout			= 0x00000004,
	eDxCasUiAttr_SetPosition		= 0x00000008,
	eDxCasUiAttr_RandomPosition	= 0x00000010,
	eDxCasUiAttr_SetSize			= 0x00000020,
	eDxCasUiAttr_SetFgColor		= 0x00000040,
	eDxCasUiAttr_SetBgColor		= 0x00000080,
	eDxCasUiAttr_Title				= 0x00000100,
	eDxCasUiAttr_Subtitle 			= 0x00000200,
	eDxCasUiAttr_Bottom			= 0x00000400,
	eDxCasUiAttr_Multiline			= 0x00000800,
	eDxCasUiAttr_HiddenInput		= 0x00001000,
	eDxCasUiAttr_ProgressBar		= 0x00002000,
	eDxCasUiAttr_InitValue			= 0x00004000,
	eDxCasUiAttr_Selectable		= 0x00008000,
	eDxCasUiAttr_IgnoreKey			= 0x00010000,
	eDxCasUiAttr_AlwaysOnTop		= 0x00020000,
	eDxCasUiAttr_Highlight			= 0x00040000,
	eDxCasUiAttr_SetPrintTime		= 0x00080000,
	eDxCasUiAttr_FingerPrint		= 0x00100000,
} DxCas_UIAttribute_e;

typedef struct
{
	DxCAS_MMI_SessionEventType_e			eventType;
	Handle_t							hSession;
	DxCAS_MmiSessionType_e			sessionType;
	DxCAS_GroupId_e					eModule;
	HUINT32							nSlot;
	HCHAR	 						*pszEncodedUiEvent;
	HUINT32                         nEncodedMessageLength;
} DxCAS_MMI_Event_t;

//<=============================

typedef enum
{
	eDxCAS_UIDATATYPE_NONE,

	eDxCAS_UIDATATYPE_IRDETO_START						= 0x0100,
		eDxCAS_UIDATATYPE_IRDETO_CLIENTVERSION,
		eDxCAS_UIDATATYPE_IRDETO_MANUFACTURERCODE,
		eDxCAS_UIDATATYPE_IRDETO_HWCODE,
		eDxCAS_UIDATATYPE_IRDETO_VARIANT,
		eDxCAS_UIDATATYPE_IRDETO_SYSTEMID,
		eDxCAS_UIDATATYPE_IRDETO_KEYVERSION,
		eDxCAS_UIDATATYPE_IRDETO_SIGNATUREVERSION,
		eDxCAS_UIDATATYPE_IRDETO_PRIVATEDATA,

	//밑으로 sub cas.

	eDxCAS_UIDATATYPE_MAX,
} DxCAS_UiDataType_e;

// CI
// 2013.08.22 이동.
typedef	enum
{
	eDxCASCI_CAM_STATUS_Extracted = 0,
	eDxCASCI_CAM_STATUS_Inserted,
	eDxCASCI_CAM_STATUS_Initializing,
	eDxCASCI_CAM_STATUS_Initialized,

	eDxCASCI_CAM_STATUS_Max
} DxCASCI_CamStatus_e;



// FileReq()'s RequestType
typedef enum
{
	CI_AMMI_REQUEST_TYPE_FILE = 0x00,
	CI_AMMI_REQUEST_TYPE_DATA = 0x01
} CI_AMMI_REQUEST_TYPE; // CI+ 스펙

// AppAbortReq()'s AbortReqCode
typedef enum
{
	CI_AMMI_ABORT_REQUEST_CODE_USER_CANCEL = 0x01,
	CI_AMMI_ABORT_REQUEST_CODE_SYSTEM_CANCEL = 0x02
} CI_AMMI_ABORT_REQUEST_CODE; // CI+ 스펙

/*--------------------------------------------------------------*/
/*  message types to be sent to CI+ MHEG engine from ci_ammi.c  */
/*--------------------------------------------------------------*/

typedef enum
{
	CI_AMMI_MSG_REQ_START,
	CI_AMMI_MSG_FILE_ACK,
	CI_AMMI_MSG_REQ_APP_ABORT,
	CI_AMMI_MSG_APP_ABORT_ACK,
	CI_AMMI_MSG_MAX
} CI_AMMI_MSG_TYPE;

/*-------------------------------------------------------------------------------------------*/
/*  data types to be sent to CI+ MHEG engine from ci_ammi.c according to each message types  */
/*-------------------------------------------------------------------------------------------*/

// 주의 사항 :
// 이하 포인터들은 ci_ammi.c 에서 callback 호출을 통해 CI+ MHEG engine 으로 전달되며 callback 호출이 완료된 시점에서는 무효하게 되므로
// callback 호출이 완료된 이후에 CI+ MHEG engine 에서 이를 사용할 필요가 있다면 반드시 local copy 해서 사용해야 함 (절대 이 포인터 그대로 사용해서는 안됨) !!
// 단, ci_ammi.c 와 CI+ MHEG engine 간의 동작이 synchronous 하다면 신경쓰지 않아도 될 것임.

typedef struct
{
	unsigned short				usSessionId;
	unsigned char				ucAppDomainIdLen;
	unsigned char *				pucAppDomainId;		// in case of CI+ : <applicationDomainIndentifier>[;<option1>;<option2>;…;<option#>], <applicationDomainIndentifier> = CI_AMMI_APP_DOMAIN_ID
	unsigned char				ucInitialObjectLen;
	unsigned char *				pucInitialObject;
} CI_AMMI_TYPE_REQ_START;						// see itk_appmmi_req_start_t in case of using S&T MHEG engine

typedef struct
{
	unsigned short				usSessionId;
	unsigned char				ucFileOK;				// 1 이어야 유효, 0 이면 무효.
	CI_AMMI_REQUEST_TYPE		enReqType;
	unsigned long				ulBytesLen;			// if enReqType is CI_AMMI_REQUEST_TYPE_FILE, "1 + file name length (x) + 4 + file data length (y)" in case of using S&T MHEG engine
	unsigned char *				pucBytes;				// if enReqType is CI_AMMI_REQUEST_TYPE_FILE, "1 byte for x + x bytes for file name + 4 bytes for y + y bytes for file data" in case of using S&T MHEG engine
} CI_AMMI_TYPE_FILE_ACK;							// see itk_appmmi_file_ack_t in case of using S&T MHEG engine

typedef struct
{
	unsigned short				usSessionId;
	CI_AMMI_ABORT_REQUEST_CODE	enAbortReqCode;
} CI_AMMI_TYPE_REQ_APP_ABORT;

typedef struct
{
	unsigned short				usSessionId;
//	unsigned char   			ucAbortAckCode;			// CI+ 스펙 1.2, TS 101 699 1.1.1, D-BOOK 5.0 모두 분명히 명시되어 있지 않아 일단 막아둠
} CI_AMMI_TYPE_APP_ABORT_ACK;

typedef union
{
	CI_AMMI_TYPE_REQ_START		typeReqStart;
	CI_AMMI_TYPE_FILE_ACK		typeFileAck;
	CI_AMMI_TYPE_REQ_APP_ABORT	typeReqAppAbort;
	CI_AMMI_TYPE_APP_ABORT_ACK	typeAppAbortAck;
} CI_AMMI_CONTENT;

typedef int (*CI_AMMI_CALLBACK_T)(CI_AMMI_MSG_TYPE enMsgType, CI_AMMI_CONTENT *pustContent);


#ifdef __cplusplus
}
#endif

#endif // DLIB_CAS_H__


