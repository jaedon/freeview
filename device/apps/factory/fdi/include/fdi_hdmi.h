/*******************************************************************************
* File name : 
* Author :
* description :
*
* Copyright (c) 2006 by Humax Co., Ltd.
* All right reserved
*******************************************************************************/
 
#ifndef _FDI_HDMI_H_
#define _FDI_HDMI_H_

/*******************************************************************************
* headers
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
typedef enum
{
	FDI_HDMI_EVENT_CONNECT=0,
	FDI_HDMI_EVENT_DISCONNECT,
	FDI_HDMI_EVENT_HDCP_SUCCESS,
	FDI_HDMI_EVENT_HDCP_FAIL,
	FDI_HDMI_EVENT_MAX
} FDI_HDMI_EVENT_e;

typedef struct
{
	unsigned char ucDestAddr;
	unsigned char ucInitAddr;
	unsigned char ucMessage[16];
	unsigned char ucMegLength;
} FDI_HDMI_CEC_MSG_t;

typedef void (*FDI_HDMI_CALLBACK)(void*);

/*******************************************************************************
* Global variables and structures
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/

/*******************************************************************************
* Static variables and structures
*******************************************************************************/

/*******************************************************************************
* Static function prototypes
*******************************************************************************/

/*******************************************************************************
* function : FDI_HDMI_Init
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDMI_Open(void);

/*******************************************************************************
* function : FDI_HDMI_IsConnected
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDMI_IsConnected(BOOL *bConnected);

/*******************************************************************************
* function : FDI_HDMI_EnableHdcp
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDMI_EnableHdcp(BOOL bEnable);

/*******************************************************************************
* function : FDI_HDMI_EnableOutput
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDMI_EnableOutput(BOOL bEnable);

/*******************************************************************************
* function : FDI_HDMI_SendCecMsg
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDMI_SendCecMsg(FDI_HDMI_CEC_MSG_t* pstCecMsg);

/*******************************************************************************
* function : FDI_HDMI_RegisterCallback
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDMI_RegisterCallback(FDI_HDMI_EVENT_e eEvent, FDI_HDMI_CALLBACK pfnCallback);


/*******************************************************************************
* function : FDI_HDMI_Hdcp22Validation
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDMI_Hdcp22Validation(void);

#endif /* _FDI_HDMI_H_ */

/* end of file */

