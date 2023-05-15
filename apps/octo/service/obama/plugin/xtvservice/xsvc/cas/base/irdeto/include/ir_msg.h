
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


#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_DIGITURK)
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
#endif

#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_UPC)
typedef	enum
{
	IR_UP_CA_CMD_TYPE_UPC_SRESET						= 0x00,
	IR_UP_CA_CMD_TYPE_UPC_SOCHAN					= 0x01,
	IR_UP_CA_CMD_TYPE_UPC_SOSCAN					= 0x02,
	IR_UP_CA_CMD_TYPE_UPC_SHTUNE						= 0x03,
	IR_UP_CA_CMD_TYPE_UPC_SSVCRC						= 0x04,
	IR_UP_CA_CMD_TYPE_UPC_SSVCID						= 0x05,
	IR_UP_CA_CMD_TYPE_UPC_GOPINI						= 0x06,
	IR_UP_CA_CMD_TYPE_UPC_SHOME						= 0x07,
	IR_UP_CA_CMD_TYPE_UPC_TZREG						= 0x08,
	IR_UP_CA_CMD_TYPE_UPC_TZOFF						= 0x09,
	IR_UP_CA_CMD_TYPE_UPC_SETUHF						= 0x0A,
	IR_UP_CA_CMD_TYPE_UPC_RESFAC						= 0x0B,
	IR_UP_CA_CMD_TYPE_UPC_LNBPWR					= 0x0C,
	IR_UP_CA_CMD_TYPE_UPC_GEOID						= 0x0D,
	IR_UP_CA_CMD_TYPE_UPC_PHONE1					= 0x0E,	//
	IR_UP_CA_CMD_TYPE_UPC_PHONE2					= 0x0F,	//
	IR_UP_CA_CMD_TYPE_UPC_PHONE3					= 0x10,	//
	IR_UP_CA_CMD_TYPE_UPC_DSDIAG						= 0x11,
	IR_UP_CA_CMD_TYPE_UPC_DSSTAT						= 0x12,
	IR_UP_CA_CMD_TYPE_UPC_DSQUAL					= 0x13,
	IR_UP_CA_CMD_TYPE_UPC_DSTARG						= 0x14,
	IR_UP_CA_CMD_TYPE_UPC_DSCLR						= 0x15,
	IR_UP_CA_CMD_TYPE_UPC_MAIL						= 0x16,
	IR_UP_CA_CMD_TYPE_UPC_MAILAPP					= 0x17,
	IR_UP_CA_CMD_TYPE_UPC_TEXT						= 0x18,
	IR_UP_CA_CMD_TYPE_UPC_TEXTAPP					= 0x19,
	IR_UP_CA_CMD_TYPE_UPC_BQBLOCK					= 0x1A,
	IR_UP_CA_CMD_TYPE_UPC_KEEPALV					= 0x1B,
	IR_UP_CA_CMD_TYPE_UPC_SCPAIR						= 0x1C,
	IR_UP_CA_CMD_TYPE_UPC_MUXVER						= 0x1D,
	IR_UP_CA_CMD_TYPE_UPC_SCTEST1					= 0x1E,
	IR_UP_CA_CMD_TYPE_UPC_SCTEST2					= 0x1F,
	IR_UP_CA_CMD_TYPE_UPC_SCTEST3					= 0x20,
	IR_UP_CA_CMD_TYPE_UPC_UNIPOLL					= 0x21,
	IR_UP_CA_CMD_TYPE_UPC_UNIDATA					= 0x22,
	IR_UP_CA_CMD_TYPE_UPC_MNOTI						= 0x23,
	IR_UP_CA_CMD_TYPE_UPC_USAGE_ID					= 0x24,	//
	IR_UP_CA_CMD_TYPE_MAX
}IR_UP_CA_UPC_CMD_TYPE_e;

#endif

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
#if defined(CONFIG_OP_RUSSIA_TVT) || defined(CONFIG_MW_CAS_IRDETO_USE_WINCP1256_MAIL_MSG) // mail msg를 WIN CP로 보내주고 OCTO내에서 WIN CP 처리하기 위한 locae code를 3 byte로 할당
#define	MAX_MAIL_TEXT_LENGTH				(MAX_ATTRIBUTED_TEXT_LENGTH + 3/*Locale Code*/ + 1/*NULL*/)
#else
#define	MAX_MAIL_TEXT_LENGTH				(MAX_ATTRIBUTED_TEXT_LENGTH + 1/*Locale Code*/ + 1/*NULL*/)
#endif

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
	HUINT8		text[MAX_ATTRIBUTED_TEXT_LENGTH+1];

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
void CAS_IR_MemDump(HUINT8 *pszStr, HUINT8 *pucBuf, HUINT32 ulLen);

#endif

/*	end of file	*/


