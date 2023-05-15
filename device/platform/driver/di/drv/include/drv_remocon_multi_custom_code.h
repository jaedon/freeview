/* $Header: */

/*******************************************************************/
/************************* File Description ****************************/
/*******************************************************************/
// File Name: drv_remocon_multi_custom_code.h
// Original Author: Yoo Tae Hyun
// File Description: IR Multi Custom Code
// Remarks:

/*******************************************************************/
/* Copyright (c) 2007 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef	__DRV_REMOCON_MULTI_CUSTOM_CODE_H__
#define	__DRV_REMOCON_MULTI_CUSTOM_CODE_H__

/*******************************************************************/
/**************************** Header Files*****************************/
/*******************************************************************/


/*******************************************************************/
/************************ Extern variables *****************************/
/*******************************************************************/


/*******************************************************************/
/************************ Macro Definition *****************************/
/*******************************************************************/
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RS538				0x1701
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RM301				0x4100
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF01				0x1000
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF02				0x1700
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04				0x1000
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RM106				0x1000
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RS636				0x4900
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_URC_39741RJ2_001	0xFFFF	//RC5 do not use multicustom code
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_URC_178200			0x4480
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMM02				0x1000
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMM03         		0xFFFF	//RC5 do not use multicustom code
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMM08				0x0B
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMM14				0x1000
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RJ03				0x01AAA112
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RC01				0x12A1
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RUWIDO_R97			0x7
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_NOVACE 			0x1000


#if defined(CONFIG_RM_301)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RM301
#elif defined(CONFIG_RM_F01)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF01
#elif defined(CONFIG_RM_F02)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT		IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF02
#elif defined(CONFIG_RM_F04)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_RM_E06)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT		IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_RM_E08)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT		IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_RM_E09)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT		IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_RM_E10)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT		IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_RM_G01)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT		IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_RM_G02)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT		IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_RS_538)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RS538
#elif defined(CONFIG_RM_106) || defined(CONFIG_RM_108) || defined(CONFIG_RM_G01) || defined(CONFIG_RM_G07) || defined(CONFIG_RM_G08) || defined(CONFIG_R_838)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RM106
#elif defined(CONFIG_RS_636)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RS636
#elif defined(CONFIG_URC_39741RJ2_001) || defined(CONFIG_RC_05)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_URC_39741RJ2_001
#elif defined(CONFIG_RC_H101)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF01
#elif defined(CONFIG_R_J03)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RJ03
#elif defined(CONFIG_RM_H04S) || defined(CONFIG_RM_H05S) || defined(CONFIG_RM_H04S_E08)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_RM_I02S)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_R_C01)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RC01
#elif defined(CONFIG_RM_L01U)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_URC_178200)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_URC_178200
#elif defined(CONFIG_RM_M02)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMM02
#elif defined(CONFIG_RM_M03)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMM03
#elif defined(CONFIG_RM_M04)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_RM_M08)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMM08
#elif defined(CONFIG_RM_M14)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMM14
#elif defined(CONFIG_RM_H09U)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_RUWIDO_R97)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RUWIDO_R97
#elif defined(CONFIG_R_843)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_R_849)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_R_861)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#elif defined(CONFIG_NOVA_CE)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_NOVACE
#elif defined(CONFIG_RM_N01)
#define IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT        IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_RMF04
#endif

#define IR_MULTI_CUSTOM_CODE_INVALID_KEY		0
#define IR_MULTI_CUSTOM_CODE_VALID_KEY			1

#define IR_MULTI_CUSTOM_MAKE_SECOND_CUSTOM_CODE(x)                  (((x)&0xFF00)|(0xFA))
#define IR_MULTI_CUSTOM_MAKE_INTERIM_CUSTOM_CODE(x)                 ((x)|(0xFF))

#define IR_CHANGE_STATE_TO_INPUT_CUSTOM_CODE_PERIOD                 6000  /* 6 Sedond */
#define IR_CHANGE_STATE_TO_FINISH_CUSTOM_CODE_PERIOD                15000 /* 15 Second */

/*******************************************************************/
/****************************** typedef ******************************/
/*******************************************************************/
typedef enum {
	IR_MULTI_CUSTOM_CODE_1 = IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT,
	IR_MULTI_CUSTOM_CODE_2 = IR_MULTI_CUSTOM_MAKE_SECOND_CUSTOM_CODE(IR_MULTI_CUSTOM_CODE_1),                  //0xZZFA
	IR_MULTI_CUSTOM_CODE_3,                                                                     //0xZZFB
	IR_MULTI_CUSTOM_CODE_4,                                                                     //0xZZFC
	IR_MULTI_CUSTOM_CODE_5,                                                                     //0xZZFD
	IR_MULTI_CUSTOM_CODE_6,                                                                     //0xZZFE
	IR_MULTI_CUSTOM_CODE_INTERIM = IR_MULTI_CUSTOM_MAKE_INTERIM_CUSTOM_CODE(IR_MULTI_CUSTOM_CODE_1)          //0xZZFF
} IR_MULTI_CUSTOM_CUSTOM_CODE;

typedef enum
{
	IR_MULTI_CUSTOM_SELECT_KEY_CODE_START = 0x70,
	IR_MULTI_CUSTOM_SELECT_KEY_CODE_1,
	IR_MULTI_CUSTOM_SELECT_KEY_CODE_2,
	IR_MULTI_CUSTOM_SELECT_KEY_CODE_3,
	IR_MULTI_CUSTOM_SELECT_KEY_CODE_4,
	IR_MULTI_CUSTOM_SELECT_KEY_CODE_5,
	IR_MULTI_CUSTOM_SELECT_KEY_CODE_6
}IR_MULTI_CUSTOM_SELECT_KEY_CODE;

 typedef enum
 {
	 IR_MULTI_CUSTOM_OPERATION_IS_NORMAL = 0,
	 IR_MULTI_CUSTOM_OPERATION_IS_START_CHANGE_CUSTOM_CODE,
	 IR_MULTI_CUSTOM_OPERATION_IS_CHANGE_CUSTOM_CODE,
	 IR_MULTI_CUSTOM_OPERATION_IS_INVALID,
	 IR_MULTI_CUSTOM_OPERATION_IS_IGNORE
 }IR_MULTI_CUSTOM_OPERATION_STATE;

typedef enum
{
	IR_MULTI_CUSTOM_MODE_NONE =0,
	IR_MULTI_CUSTOM_MODE_1,
	IR_MULTI_CUSTOM_MODE_2,
	IR_MULTI_CUSTOM_MODE_3,
	IR_MULTI_CUSTOM_MODE_4,
	IR_MULTI_CUSTOM_MODE_5,
	IR_MULTI_CUSTOM_MODE_6,
}IR_MULTI_CUSTOM_MODE;

typedef enum
{
	IR_MULTI_CUSTOM_SELECT_KEY_CODE_UNSELECTED,
	IR_MULTI_CUSTOM_SELECT_KEY_CODE_SELECTED
}IR_MULTI_CUSTOM_CODE_SELECT_STATUS;

typedef struct IRInfo_t
{
	HUINT16 		        usCurrentCustomCode;	  /* 현재 설정된 리모컨 Custom Code */
	HUINT16 		        usPreviousCustomCode;     /* 전에 설정된 리모컨 Custom Code */
	HUINT16 		        usDecodedCustomCode;      /* 현재 입력 된 리모컨 Custom Code*/
	HUINT8 	                ucDecodedCode;            /* 디코딩된 코드 */
	IR_MULTI_CUSTOM_OPERATION_STATE     eOperationState;          /* Multi Custom Code 적용을 위한 상태 flag */
	HUINT32                 ulIRChangeStateTimerID;
	HUINT32                 ulQueueID;
	HUINT32                 ulCurrentMode;
}IRINFO;

typedef struct IRInfo_t * pIRINFO;

typedef	enum tagIR_MULTI_COUSTOM_APP_NOTIFY_MSGTYPE
{
	MSG_MULTICUSTOMCODE_INPUT = 0,
	MSG_MULTICUSTOMCODE_SUCCESS,
	MSG_MULTICUSTOMCODE_CANCEL,
	MSG_MULTICUSTOMCODE_MAX
} IR_MULTI_COUSTOM_APP_NOTIFY_MSGTYPE;

typedef enum tagIR_MULTI_CUSTOM_RET_CODE
{
	IR_MULTI_CUSTOM_RET_CODE_NO_ERR,
	IR_MULTI_CUSTOM_RET_CODE_ERR
}IR_MULTI_CUSTOM_RET_CODE;

typedef int	(*IR_MULTICUSTOMCODE_NOTIFY_APP_FUNC) 		(HUINT8 msgType, HUINT8	mode);

/*******************************************************************/
/******************** global function prototype **************************/
/*******************************************************************/
void DRV_REMOCON_MultiCustomCode_Init(void);
void DRV_REMOCON_MultiCusomCode_ClearState(pIRINFO pIRInfo, IR_MULTI_CUSTOM_OPERATION_STATE eState);
pIRINFO DRV_REMOCON_MultiCusomCode_GetIRInfo(void);
IR_MULTI_CUSTOM_OPERATION_STATE DRV_REMOCON_MultiCusomCode_StateMachine(pIRINFO pIRInfo);
HUINT16 DRV_REMOCON_MultiCusomCode_GetCurrentCustomCode(void);
IR_MULTI_CUSTOM_RET_CODE DRV_REMOCON_MultiCustomCode_WriteDefaultCustomCodeToE2P(void);

HUINT8 DRV_REMOCON_MultiCusomCode_IsValidKey(HUINT16 usCustomCode, HUINT16 usKeyCode);
#if defined(CONFIG_DIRECTFB)
void DRV_REMOCON_MultiCustomCode_RegisterCallBack(pfnDI_KEY_Notify notifyFunc);
#else
void DRV_REMOCON_MultiCustomCode_RegisterCallBack(IR_MULTICUSTOMCODE_NOTIFY_APP_FUNC notifyFunc);
#endif

#endif /* !__DRV_REMOCON_MULTI_CUSTOM_CODE_H__ */

