/*
 * (c) 2011-2012 Humax Co., Ltd. 
 * This program is produced by Humax Co., Ltd. ("Humax") and 
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
 * non-assignable, non-transferable and non-exclusive license to use this Software. 
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
 * you agree to the responsibility to take all reasonable efforts to protect the any information 
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, 
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies 
 * of the Software including all documentation. This License will terminate immediately without notice from Humax 
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies 
 * of the Software and all documentation.  

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice. 
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court, 
 * in the Republic of Korea.
*/

#ifndef __MSG_DRV_H__
#define	__MSG_DRV_H__



//#include "ir_common.h"

//#include "util.h"	// DATE_T, TIME_T
#include <hlib.h>


    // Destination ID Types
#define	DECODER_MSG_DESTINATION_ID_TEXT							0x00
#define	DECODER_MSG_DESTINATION_ID_CNTR     					0x01
#define	DECODER_MSG_DESTINATION_ID_CAM							0x02
#define	DECODER_MSG_DESTINATION_ID_PROFDEC						0x03
#define	DECODER_MSG_DESTINATION_ID_ATTRIBUTED_DISPLAY			0x04
#define	DECODER_MSG_DESTINATION_ID_OPENCABLE_HOST				0x05
#define	DECODER_MSG_DESTINATION_ID_CIPLUS_CAM					0x06

    // TEXT Message Type
#define	EPG_MESSAGE_TYPE_TEXT_MAILBOX							0x00    // Individually Address Message
#define	EPG_MESSAGE_TYPE_TEXT_ANNOUNCEMENT						0x10    // Broadcast Message
#define	EPG_MESSAGE_TYPE_CLUB_NUMBER							0x20    // New Assigned Club number

    // TEXT Message Class
#define	EPG_MESSAGE_CLASS_NORMAL								0x00    // Normal class
#define	EPG_MESSAGE_CLASS_TIMED									0x01    // Automatic erase after date/time
#define	EPG_MESSAGE_CLASS_FORCED_DISPLAY						0x02    // Forced Display

    // TEXT Message_Priority
#define	IRDETO_TEXTMESSAGE_PRIORITY_LOW							0x00
#define	IRDETO_TEXTMESSAGE_PRIORITY_MID							0x01
#define	IRDETO_TEXTMESSAGE_PRIORITY_HIGH						0x02
#define	IRDETO_TEXTMESSAGE_PRIORITY_URGENT						0x03

    // Attribute Msg Class (move from msg_drv.c)
#define	ATTRIBUTED_MESSAGE_TYPE_NORMAL										0x00
#define	ATTRIBUTED_MESSAGE_TYPE_FORCEDTEXT									0x01
#define	ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT									0x02
#define	ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT_COVERT_OVERT_OPTION				0x03

    // Decoder Control Message
#define	CONTROL_MESSAGE_TYPE_DOWNLOAD_CONTROL					0x00
#define	CONTROL_MESSAGE_TYPE_CALLBACK							0x01
#define	CONTROL_MESSAGE_TYPE_MONITOR_MODE						0x02
#define	CONTROL_MESSAGE_TYPE_READ_SC_USER_DATA					0x03
#define	CONTROL_MESSAGE_TYPE_CHANGE_PINCODE						0x04
#define	CONTROL_MESSAGE_TYPE_DECODER_RECOVERY					0x05
#define	CONTROL_MESSAGE_TYPE_USER_PAYLOAD						0x06

// RECOVERY TYPE (CONTROL_MESSAGE_TYPE_DECODER_RECOVERY)
typedef	enum
{
	IR_CTRL_MSG_RECOVERY_RESET_IRD			= 0x00,
	IR_CTRL_MSG_RECOVERY_RESET_CH_ORDER		= 0x01,
	IR_CTRL_MSG_RECOVERY_RESCAN				= 0x02,
	IR_CTRL_MSG_RECOVERY_RESCAN_FROM_HOME	= 0x03,
	IR_CTRL_MSG_RECOVERY_RECONNECT			= 0x04,
	IR_CTRL_MSG_RECOVERY_FORCE_TUNE			= 0x05,
	IR_CTRL_MSG_RECOVERY_FORCE_SET_PIN		= 0x06,
	IR_CTRL_MSG_RECOVERY_MAX,
}IR_CTRL_MSG_RECOVERY_Type_e;


//USER PAYLOAD CONTROL TAG FOR DIGITURK
#define	IR_UP_DESTINATION_CA_TASK				0x01
#define	IR_UP_DESTINATION_EPG_APP				0x02

typedef	enum
{
	IR_UP_CA_TAG_ITO_DDEMM_VIRTUAL_BILL					= 0x01,
	IR_UP_CA_TAG_ITO_DDEMM_DM_PARAMETERS				= 0x02,
	IR_UP_CA_TAG_ITO_DDEMM_FEATURE_BITMAP				= 0x03,
	IR_UP_CA_TAG_ITO_DDEMM_IPPV_IP_AND_PHONE_NO			= 0x04,
	IR_UP_CA_TAG_ITO_DDEMM_IPPV_DEBIT_LIMIT_THRESHOLD	= 0x05,
	IR_UP_CA_TAG_ITO_DDEMM_IPPV_SCHEDULE				= 0x06,
	IR_UP_CA_TAG_ITO_DDEMM_REPORT_STATUS				= 0x07,
	IR_UP_CA_TAG_ITO_DDEMM_SET_DOWNLOAD_TYPE			= 0x08,
	IR_UP_CA_TAG_MAX
}IR_UP_CA_TAG_e;

//USER PAYLOAD CONTROL TAG FOR DIGITURK
typedef enum
{
	IR_UP_EPG_TAG_ITO_DDEMM_DM_SETUP					= 0x01,
	IR_UP_EPG_TAG_ITO_DDEMM_FORCED_STANDBY				= 0x02,
	IR_UP_EPG_TAG_ITO_DDEMM_DISPLAY_CARD_NUMBER			= 0x03,
	IR_UP_EPG_TAG_ITO_DDEMM_REMOTE_TAG					= 0x05,
	IR_UP_EPG_TAG_ITO_DDEMM_REMOTE_RECORD				= 0x06,
	IR_UP_EPG_TAG_ITO_DDEMM_DEL_PVOD_EVENT				= 0x07,
	IR_UP_EPG_TAG_ITO_DDEMM_CLEAR_3PA_DATA				= 0x08,
	IR_UP_EPG_TAG_ITO_DDEMM_COLOUR_DISPLAY				= 0x09,
	IR_UP_EPG_TAG_ITO_DDEMM_CS_PHONE_NO					= 0x0a,
	IR_UP_EPG_TAG_ITO_DDEMM_PPV_PHONE_NO				= 0x0b,
	IR_UP_EPG_TAG_ITO_DDEMM_UPDATE_HDD					= 0x0c,
	IR_UP_EPG_TAG_ITO_DDEMM_FORMAT_HDD					= 0x0d,
	IR_UP_EPG_TAG_ITO_DDEMM_EPG_FEATURES				= 0x0f,
	IR_UP_EPG_TAG_ITO_DDEMM_USER_FOLDER_SETTINGS		= 0x10,

	IR_UP_EPG_TAG_ITO_DDEMM_SET_OTHER_MODE				= 0x50,
	IR_UP_EPG_TAG_ITO_DDEMM_EASY_SMS_DISPLAY			= 0x51,
	IR_UP_EPG_TAG_ITO_DDEMM_REMOTE_TAG_DS265			= 0x52,
	IR_UP_EPG_TAG_ITO_DDEMM_DIGIAKTIF					= 0x53,
	IR_UP_EPG_TAG_ITO_DDEMM_SET_FTA_LIMIT				= 0x54,
	IR_UP_EPG_TAG_MAX = 0xff
}IR_UP_EPG_TAG_e;

    // Definition of the maximum number of club number in CA subsystem
#define	MAX_NUMBER_CA_CLUB										16
#define	MAX_NUMBER_MAIL_MESSAGE									20

    // Defintion of error
#define	EPG_MSG_VALID							0
#define	EPG_MSG_INVALID							-1

    // Information of bits position and mask
#define	BIT_POS_MSG_CLASS					5
#define	BIT_POS_FLUSH_BUFFER				4
#define	BIT_POS_COMPRESSED					3
#define	BIT_POS_CLUB_MSG					2
#define	BIT_POS_MSG_PRIORITY				0

#define	BIT_MASK_MSG_CLASS					0x07U
#define	BIT_MASK_FLUSH_BUFFER				0x01U
#define	BIT_MASK_COMPRESSED					0x01U
#define	BIT_MASK_CLUB_MSG					0x01U
#define	BIT_MASK_MSG_PRIORITY				0x03U

#define	BIT_POS_YEAR						17
#define	BIT_POS_MONTH						13
#define	BIT_POS_DAY							8
#define	BIT_POS_HOUR						3
#define	BIT_POS_MINUTE						0

#define	BIT_MASK_YEAR						0x7fU
#define	BIT_MASK_MONTH						0x0fU
#define	BIT_MASK_DAY						0x1fU
#define	BIT_MASK_HOUR						0x1fU
#define	BIT_MASK_MINUTE						0x07U

#define	BIT_POS_ADD_NEW_CLUB				7
#define	BIT_POS_NUMBER_CLUB					0

#define	BIT_MASK_ADD_NEW_CLUB				0x01U
#define	BIT_MASK_NUMBER_CLUB				0x7fU

// Attribute Display Method
#define	BIT_DISP_METHOD_FLASHING			0
#define	BIT_DISP_METHOD_NOTFLASHING			1

#define	BIT_DISP_METHOD_BANNER				0
#define	BIT_DISP_METHOD_NORMAL				1

// Attribute Finger Print Type
#define	BIT_FP_TYPE_OVERT					0
#define	BIT_FP_TYPE_COVERT					1

#define	SIZE_MAILBOX_TABLE					(sizeof(MAILBOX_TABLE))
#define	SIZE_ATTRIBUTED_DISPLAY				(sizeof(ATTRIBUTED_DISPLAY))

#define	UTIL_RESET_BIT(x,y)					x=x&~(1U<<y)
#define	UTIL_SET_BIT(x,y)					x=x|(1U<<y)
#define	UTIL_GET_BITS(x,y,z)				(((x)>>(y))&(z))
#define	UTIL_SET_BITS(x,y,z,w)				x=((x)&~((w)<<(z)))|((y)<<(z))

#define	MAX_NORMAL_TEXT_LENGTH				256
#define	MAX_ATTRIBUTED_TEXT_LENGTH			512
#define	MAX_MAIL_TEXT_LENGTH				(MAX_ATTRIBUTED_TEXT_LENGTH + 1/*Locale Code*/ + 1/*NULL*/)

/********************************************************************
*   type definition
********************************************************************/

typedef struct
{
	HUINT8		ucMsg_Type; 	// Normal / Forced Text / Fingerprint
	HUINT16		unDuration;		// 0= Idefinitely
	HUINT8    	ucDisplay_Method;   // Flashing or Not-Flashing , Banner or Normal

/*	HUINT16		Text_length;	// 12bits
	HUINT8		Text_Byte[MAX_ATTRIBUTED_TEXT_LENGTH];	// max 512byte */

	HUINT8		bFlashing;
	HUINT8		bNormal;
	HUINT8		ucCoverage;
   	UNIXTIME	recUnixtime;

	HUINT8		ucFingerprint_Type;	// 0: Overt FP, 1:Covert FP
} ATTRIBUTED_DISPLAY;

typedef struct
{
	UNIXTIME	unixtime;
	HxDATETIME_Date_t	add_date;
	HxDATETIME_Time_t	add_time;
	HxDATETIME_Date_t	del_date;
	HxDATETIME_Time_t	del_time;
	HUINT16		club_msg;
	HUINT16		club;
	HUINT8		ucType; 	// Text-Mailbox / Text-Announcement / Club Numbers

	HUINT8		ucMsgNum;	// mail msg num
	HUINT8		priority;
	HUINT8		msg_class;	//Normal / Timed / Forced Display
	HUINT8		compressed;

	HUINT8    	number_club;   //?????
	HUINT16		msg_length;
	HUINT8		text[MAX_ATTRIBUTED_TEXT_LENGTH];

	HUINT8		bAttributeMsg;
	ATTRIBUTED_DISPLAY	Atrb_Display;
}MAILBOX_TABLE;

typedef struct
{
	HUINT8			ucListValid;
	HUINT8			ucReserved;
	HUINT8			aucMailIndexList[MAX_NUMBER_MAIL_MESSAGE];	// value(not index) is same as mailbox table index
	HUINT16			valid[MAX_NUMBER_MAIL_MESSAGE];					// index is same as mailbox table index
	HUINT16			bReadStatus[MAX_NUMBER_MAIL_MESSAGE];			// index is same as mailbox table index
	MAILBOX_TABLE	mailbox_table[MAX_NUMBER_MAIL_MESSAGE];
	HUINT16			crc16;
} MAILBOX_INFO;


#define	ERR_MSG_DRV_ERROR				0x00000001
#define	ERR_INVALID_MAILBOX_INFO		0x00000002
#define	ERR_MAILBOX_INFO_READ			0x00000003
#define	ERR_MAILBOX_TABLE_READ			0x00000004
#define	ERR_MAILBOX_CRC 				0x00000005


void	CAS_IR_MSG_DRV_Initialise( void );
void	CAS_IR_MSG_DRV_ProcessDecoderDataEMM(Handle_t hAct, HUINT8 *p);
HBOOL	CAS_IR_MSG_DRV_GetValid( HUINT8 ucMailIndex, HUINT16 *psFlag );
HBOOL	CAS_IR_MSG_DRV_GetReadState( HUINT8 ucMailIndex, HUINT16 *pbFlag );
HBOOL	CAS_IR_MSG_DRV_SetReadState( HUINT8 ucMailIndex, HUINT16 sFlag );
HUINT8	CAS_IR_MSG_DRV_DoYouHaveUnreadMail(void);
HBOOL	CAS_IR_MSG_DRV_DeleteMail( HUINT8 ucMailIndex );
void	CAS_IR_MSG_DRV_AllClearExpiredNormalAttributeMail(void);
void	CAS_IR_MSG_DRV_GetMailInfo( HUINT8 *pucNumberOfSavedMail, HUINT8 *paucMailIndexList );
HBOOL	CAS_IR_MSG_DRV_GetMailBox( HUINT8 ucMailIndex, MAILBOX_TABLE *pMailBox );
#if defined(CONFIG_DEBUG)
void	CAS_IR_MSG_DRV_DbgMailBox(void);
#endif

#endif

/*	end of file	*/


