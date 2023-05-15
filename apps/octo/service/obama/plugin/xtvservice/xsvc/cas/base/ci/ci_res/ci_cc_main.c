/***************************************************************
* $Workfile:   ci_cc_main.c  $
* $Modtime:
* Auther : hmkim / dslee for X.509 functions
*
* Desc : Content Control (CI+) Main
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/


/***************************************************************
* #ifdef statements and include ////////////////////////////////
***************************************************************/

#include <svc_cas.h>
#include <_svc_cas_sub_api.h>
#include <_svc_cas_sub_dev_api.h>
#include <ci_cc_main.h>

#include <cb_list.h>

#include <ci_plus.h>
#include "./ci_cc_lib/ci_cc_cipher.h"
#include "./ci_cc_lib/ci_cc_x509.h"

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
#include <util.h>
#include <hlib.h>
#endif

/***************************************************************
* debugging ////////////////////////////////////////////////////
***************************************************************/

#ifdef _CI_PLUS_DEBUG_
#define _CI_DEBUG_CC_DATA_
//#define _CI_DEBUG_CC_SELF_CHECKING_
//#define _CI_DEBUG_CC_TIME_ISSUE_
//#define _CI_DEBUG_CC_PROC_TIME_
//#define _CI_DEBUG_CC_FUNC_TIME_
#endif

#if defined(CONFIG_DEBUG)
static unsigned long s_ulCheckTime_0;
static unsigned long s_LastulCheckTime_0;
static unsigned long s_ulCheckTime_1;
static unsigned long s_LastulCheckTime_1;

#define CHECK_AUTH_TIME(x)	\
{\
	if(x == 0)\
	{\
		s_ulCheckTime_0 = VK_TIMER_GetSystemTick();\
		HxLOG_Error("==>CHECK_AUTH_TIME_0  slot id(%d) [%d], [%d]\n", x, s_ulCheckTime_0, s_ulCheckTime_0 - s_LastulCheckTime_0);\
		s_LastulCheckTime_0 = s_ulCheckTime_0;\
	}\
	else\
	{\
		s_ulCheckTime_1 = VK_TIMER_GetSystemTick();\
		HxLOG_Error("==>CHECK_AUTH_TIME_1  slot id(%d) [%d], [%d]\n", x, s_ulCheckTime_1, s_ulCheckTime_1 - s_LastulCheckTime_1);\
		s_LastulCheckTime_1 = s_ulCheckTime_1;\
	}\
}
#endif

/***************************************************************
* compile definition ///////////////////////////////////////////
***************************************************************/

/* �Ʒ� define �� Ű ����� ������� ķ�� confirm �� ���� ������ �ֱ� ���� ����Ѵ�.
   �� define �� ������� ������ Ű ������ ��ġ�� ķ�� confirm �� �ְ� �ȴ� (�����ս��� ���ٸ� �̰��� �����). */
#ifdef CONFIG_DOUGLAS
#define USE_SUB_TASK_TO_CALCULATE_KEYS
#endif

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
#define USE_SAC_TASK_TO_AVOID_SAC_MESSAGE_ERROR
#endif

/* ��Ÿ�� ���� �ʿ� : see USE_SUB_TASK_TO_CALCULATE_KEYS also */
//#define CI_CC_CalculateCckAndSetDescramblerBeforeSync
//#define CI_CC_CalculateCckBeforeSyncAndSetDescramblerOnSync
#if 0
#if defined(CONFIG_DOUGLAS) // LF7300 ���� ���� (org)
#define CI_CC_CalculateCckAndSetDescramblerBeforeSync
#elif defined(CONFIG_PROD_CPU_BCM) // LE5400 ���� ����
#define CI_CC_CalculateCckBeforeSyncAndSetDescramblerOnSync
#else
#define CI_CC_CalculateCckAndSetDescramblerBeforeSync
#endif
#else // 2011-07-11, iCord Cable ���� CPU Load ���� �� Descrambler Key Setting �ð� Delay�Ǹ鼭 Video ������ ���� �߻��Ͽ� ������. �����δ� �Ϲ������� �� ������ �⺻���� �Ѵ�.
#define CI_CC_CalculateCckAndSetDescramblerBeforeSync
#endif


/* verify received sac_message() - ������ �ϸ� 1 �̾�� �ϳ� �����ս��� �����ٸ� 0 ���. ��, ���� ���� ���� �ݵ�� 1 �� �����ؾ� �Ѵ�!! */
#define CI_CC_VerifyReceivedSacMessage	(1)

/***************************************************************
* local definition /////////////////////////////////////////////
***************************************************************/

/* host license constants data downloading related (on booting process) */
#define CI_CC_LICENSE_DATA_DOWN_CHECK_MAX_COUNT	1000

#define CI_CC_SYSTEM_ID_BITMASK_V1	0x01		// cc_system_id_bitmask for version 1
#define CI_CC_MAX_DATA_NBR			4			// CI+ ���� 1.2 & 1.3 �������� CC �� �������� �󿡼� �ִ� 4�������� ���ȴ�

/**************************************/
/* Secure Authenticated Channel (SAC) */
/**************************************/

#define CI_CC_SAC_MSG_FLAG_FALSE		0
#define CI_CC_SAC_MSG_FLAG_TRUE		1
#define CI_CC_SAC_MSG_CNT_MAX		0xffffffff

/*********************************/
/* Usage Rules Information (URI) */
/*********************************/

#define CI_CC_URI_TIMEOUT				10000		// URI message delivery timeout value in ms
#define CI_CC_URI_PROTOCOL_V1		0x01
#define CI_CC_URI_PROTOCOL_V2		0x02		// CI+ v1.3

/*****************************************/
/* Tag Length Format (TLF) for Signature */
/*****************************************/

#define CI_CC_TLF_VER_DEFAULT		0x01
#define CI_CC_TLF_MSG_LABEL_HOST_A	0x02		// msg_label when host authenticate Signature_A
#define CI_CC_TLF_MSG_LABEL_HOST_V	0x03		// msg_label when host verify Signature_B

#define CI_CC_TLF_TAG_VER				0x00
#define CI_CC_TLF_TAG_MSG_LABEL		0x01
#define CI_CC_TLF_TAG_AUTH_NONCE		0x02
#define CI_CC_TLF_TAG_DHPM			0x03
#define CI_CC_TLF_TAG_DHPH			0x04

#define CI_CC_TLF_LEN_VER				0x0008
#define CI_CC_TLF_LEN_MSG_LABEL		0x0008
#define CI_CC_TLF_LEN_AUTH_NONCE		0x0100
#define CI_CC_TLF_LEN_DHPM			0x0800
#define CI_CC_TLF_LEN_DHPH			0x0800

/*********************************/
/* Recording					 */
/*********************************/

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
#define CI_CC_RECORDING_PIN_RETRY_MAX	2		// cached PIN �����ϱ� ���� PIN�� ���� �� ��, ����ڷκ��� Ʋ�� PIN�� �޾��� ��, ����ڿ��� PIN�Է��� �䱸�ϴ� �ִ� Ƚ��.
#define CI_CC_PB_LICENSE_TIMEOUT		10000
#endif

/***************************************************************
* typedef //////////////////////////////////////////////////////
***************************************************************/

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)

/**********************/
/* for "Make Sac Message (Message Type)" (added from CI+ v1.3) */
/**********************/
typedef enum {
	CI_CC_SAC_MESSAGE_RECORD_START_WITH_PIN = 0x01,
	CI_CC_SAC_MESSAGE_RECORD_START_NO_PIN,
	CI_CC_SAC_MESSAGE_CHANGE_OPERATING_MODE,
	CI_CC_SAC_MESSAGE_RECORD_STOP,
	CI_CC_SAC_MESSAGE_PLAYBACK_LICENSE_EXCHANGE,
	CI_CC_SAC_MESSAGE_LICENSE_CHECK_EXCHANGE
} CI_CC_SAC_MESSAGE_TYPE;

/*******************************************************************/
/* for "Record Start Protocol (SAC Message)" (added from CI+ v1.3) */
/*******************************************************************/
typedef struct {
	unsigned char	ucOperatingMode;
	unsigned short	usProgramNumber;
	unsigned char	aucPinCodeData[CICAM_PIN_LENGTH];
	unsigned char	ucPinCodeLength;
} CI_CC_SAC_MSG_RECORD_START;

/******************************************************************/
/* for "Record Stop Protocol (SAC Message)" (added from CI+ v1.3) */
/******************************************************************/
typedef struct {
	unsigned short	usProgramNumber;
} CI_CC_SAC_MSG_RECORD_STOP;

/********************************************************************************/
/* for "Playback License Exchange Protocol (SAC Message)" (added from CI+ v1.3) */
/********************************************************************************/
typedef struct {
	unsigned short	usProgramNumber;
	unsigned char	*pucHostLicense;
	unsigned short	usLicenseDataLen;
} CI_CC_SAC_MSG_PLAYBACK_LICENSE;

/*****************************************************************************/
/* for "License Check Exchange Protocol (SAC Message)" (added from CI+ v1.3) */
/*****************************************************************************/
typedef struct {
	unsigned char	*pucHostLicense;
	unsigned short	usLicenseDataLen;
} CI_CC_SAC_MSG_LICENSE_CHECK;

/****************************************************************************/
/* for "Change Operating Mode Protocol (SAC Message)" (added from CI+ v1.3) */
/****************************************************************************/
typedef struct {
	unsigned char	ucOperatingMode;
	unsigned short	usProgramNumber;
} CI_CC_SAC_MSG_CHANGE_OPERATING_MODE;


/**********************/
/* for "PINcode_status_field" (added from CI+ v1.3) */
/**********************/
/*
typedef enum {
	CI_CC_PIN_STATUS_BAD_CODE = 0x00,
	CI_CC_PIN_STATUS_CICAM_BUSY = 0x01,
	CI_CC_PIN_STATUS_OK = 0x02,
	CI_CC_PIN_STATUS_UNCONFIRMED = 0x03,
	CI_CC_PIN_STATUS_VIDEO_BLANKING_NOT_REQ = 0x04,
	CI_CC_PIN_STATUS_STILL_CA_SCRAMBLED = 0x05
} CI_CC_PIN_STATUS;
*/
/**********************/
/* for "license_rcvd_status" (added from CI+ v1.3) */
/**********************/
typedef enum {
	CI_CC_LICENSE_RCVD_STATUS_OK = 0x00,
	CI_CC_LICENSE_RCVD_STATUS_HOST_BUSY = 0x01,
	CI_CC_LICENSE_RCVD_STATUS_INVALID_DATA = 0x02,
	CI_CC_LICENSE_RCVD_STATUS_HOST_ERROR = 0x03
} CI_CC_LICENSE_RCVD_STATUS;

typedef enum {
	CI_CC_PIN_REPLY_FOR_WATCH_TV	= 0x00,
	CI_CC_PIN_REPLY_FOR_PLAYBACK	= 0x01,
	CI_CC_PIN_REPLY_FOR_CMD			= 0x02
} CI_CC_PIN_REPLY_STATUS;

#endif

/****************************************/
/* Secure Authenticated Channel message */
/****************************************/
/*
	sac_message() {
		message_counter											: 32 bits
		- message header -
		protocol_version										: 4 bits (should be 0)
		authentication_cipher_flag								: 3 bits (indicates the cipher that is used to generate the authentication field. should be 0 = AES-128-XCBC-MAC)
		payload_encryption_flag									: 1 bits (should be 1 for cc_sac_data_req(), cc_sac_data_cnf(), cc_sac_sync_req(), cc_sac_sync_cnf())
		encryption_cipher_flag									: 3 bits (indicates the cipher that is used to encrypt the message payload. should be 0 = AES-128 in CBC mode)
		reserved for future use									: 5 bits
		length_payload											: 16 bits
		- message body -
		if (payload_encryption_flag == MSG_FLAG_TRUE) {
			encrypted_payload									: length_payload * 8 + 128 bits -> If payload is not any multiple of CI_CC_CIPHER_BLOCK_SIZE the message is padded by adding a 1 bit and then 0 bits until the block size is filled (OneAndZeroes padding)
		} else if (payload_encryption_flag == MSG_FLAG_FALSE) {
			payload												: length_payload * 8 bits
			authentication										: 128 bits
		}
	}
*/
typedef struct {
	unsigned		protocol_version:4;
	unsigned		authentication_cipher_flag:3;
	unsigned		payload_encryption_flag:1;
} CI_CC_SAC_MSG_HDR1;	// ����Ϸ��� endian ����ؾ� �ϹǷ� �ϴ� ����μ� ������� ����.

typedef struct {
	unsigned		encryption_cipher_flag:3;
	unsigned		reserved:5;
} CI_CC_SAC_MSG_HDR2;	// ����Ϸ��� endian ����ؾ� �ϹǷ� �ϴ� ����μ� ������� ����.

typedef struct {
	unsigned long	ulMsgCnt;
	unsigned char	ucHdr1;				// CI_CC_SAC_MSG_HDR1
	unsigned char	ucHdr2;				// CI_CC_SAC_MSG_HDR2
	unsigned short	usPayloadLen;
	unsigned char	*pucPayloadData;	// just pointer to payload (don't mem alloc for this)
	unsigned char	ucAuth[16];
} CI_CC_SAC_MSG;

/***********************************************************************************************/
/* Usage Rules Information message (CI+ ���� 1.0 ����Ͽ� 1.1 ���Ŀ��� �Ʒ��� ���� ����Ǿ���) */
/***********************************************************************************************/
/*
	uri_message() for version 1 {
		protocol_version				: 8 bits	(default : 0x01 -> CI_CC_URI_PROTOCOL_V1)
		aps_copy_control_info			: 2 bits  (default : 0b00     -> CI_CC_URI_APS_DEFAULT)
		emi_copy_control_info			: 2 bits  (default : 0b11     -> CI_CC_URI_EMI_DEFAULT)
		ict_copy_control_info			: 1 bit   (default : 0b0      -> CI_CC_URI_ICT_DEFAULT -> if CI_CC_URI_TIMEOUT occurs, CI_CC_URI_ICT_TIMEOUT)
		rct_copy_control_info			: 1 bit   (default : 0b0      -> CI_CC_URI_RCT_DEFAULT)
		reserved for future use			: 4 bits  (all 0s)
		rl_copy_control_info			: 6 bits  (default : 0b000000 -> CI_CC_URI_RL_DEFAULT)
		reserved for future use			: 40 bits (all 0s)
	}

	uri_message() for version 2 from CI+ v1.3 {
		protocol_version				: 8 bits	(default : 0x02 -> CI_CC_URI_PROTOCOL_V2)
		aps_copy_control_info			: 2 bits	(default : 0b00 -> CI_CC_URI_APS_DEFAULT)
		emi_copy_control_info			: 2 bits	(default : 0b11 -> CI_CC_URI_EMI_DEFAULT)
		ict_copy_control_info			: 1 bit	(default : 0b0   -> CI_CC_URI_ICT_DEFAULT -> if CI_CC_URI_TIMEOUT occurs, CI_CC_URI_ICT_TIMEOUT)
		rct_copy_control_info			: 1 bit	(default : 0b0   -> CI_CC_URI_RCT_DEFAULT -> rct_copy_control_info is available only if emi_copy_control_info is CI_CC_URI_EMI_COPY_FREE)
		reserved for future use			: 1 bit	(0)
		dot_copy_control_info			: 1 bit	(default : 0b0   -> CI_CC_URI_DOT_DEFAULT -> dot_copy_control_info is available only if emi_copy_control_info is CI_CC_URI_EMI_COPY_NEVER)
		rl_copy_control_info			: 8 bits	(default : 0b00000000 -> CI_CC_URI_RL_DEFAULT -> rl_copy_control_info is available only if emi_copy_control_info is CI_CC_URI_EMI_COPY_NEVER)
		reserved for future use			: 40 bits	(all 0s)
	}
*/
typedef struct {
	unsigned char	ucMode;
	unsigned char	ucApsCopyCtrl;		// see CI_CC_URI_APS_
	unsigned char	ucEmiCopyCtrl;		// see CI_CC_URI_EMI_
	unsigned char	ucIctCopyCtrl;		// see CI_CC_URI_ICT_
	unsigned char	ucRctCopyCtrl;		// see CI_CC_URI_RCT_
	unsigned char	ucDotCopyCtrl;		// see CI_CC_URI_DOT_ (added from CI+ v1.3)
	unsigned char	ucRlCopyCtrl;		// see CI_CC_URI_RL_
} CI_CC_URI_MSG;


/****************************************/
/* ķ<->ȣ��Ʈ�� ���޵Ǵ� ������ ������ */
/****************************************/
typedef enum {
	CI_CC_Brand_ID = 1,				/* 400 bits (50 bytes)	: Defined by the License Document */ /* removed from CI+ v1.3 */
	CI_CC_Host_ID = 5,				/* 64 bits (8 bytes)	: Generated by the ROT and included in the X.509 certificate. */
	CI_CC_CICAM_ID = 6,				/* 64 bits (8 bytes)	: Generated by the ROT and included in the X.509 certificate. */
	CI_CC_Host_BrandCert = 7, 	/* variable  : Host Brand Certificate */
	CI_CC_CICAM_BrandCert = 8,	/* variable  : CICAM Brand Certificate */
	CI_CC_Kp = 12,					/* 256 bits (32 bytes)	: CICAM's key precursor to Host for CCK */
	CI_CC_DHPH = 13,				/* 2048 bits (256 bytes): DH Public Key Host */ /* volatile */
	CI_CC_DHPM = 14,				/* 2048 bits (256 bytes): DH Public Key module/CICAM */ /* volatile */
	CI_CC_Host_DevCert = 15, 		/* variable  : Host Device Certificate Data */
	CI_CC_CICAM_DevCert = 16,	/* variable  : CICAM Device Certificate Data */
	CI_CC_Signature_A = 17,			/* 2048 bits (256 bytes): The signature of Host DH public key */
	CI_CC_Signature_B = 18,			/* 2048 bits (256 bytes): The signature of CICAM DH public key */
	CI_CC_auth_nonce = 19,			/* 256 bits (32 bytes)	: Random nonce of 256 bits generated by the CICAM and transmitted by the CICAM to the host for use in the authentication protocol. */
	CI_CC_Ns_Host = 20,				/* 64 bits (8 bytes)	: Host's challenge to CICAM for SAC */
	CI_CC_Ns_module = 21,			/* 64 bits (8 bytes)	: CICAM's challenge to Host for SAC */
	CI_CC_AKH = 22,					/* 256 bits (32 bytes)	: Authentication Key Host */ /* non-volatile */ /* host have to reply within 5 sec */ /* If the AKH is not available, then host shall transmit a value of all zeros as an invalid AKH. When receiving this invalid AKH the CICAM starts the authentication protocol. */
	CI_CC_AKM = 23,					/* 256 bits (32 bytes)	: Authentication Key Module/CICAM */ /* non-volatile */
	CI_CC_uri_message = 25,			/* 64 bits (8 bytes)	: Data message carrying the Usage Rules Information. */
	CI_CC_program_number = 26,		/* 16 bits (2 bytes)	: MPEG program number. */
	CI_CC_uri_confirm = 27,			/* 256 bits (32 bytes)	: Hash on the data confirmed by the host. */ /* host have to reply within 1 sec and apply URI to external interface within another 1 sec */
	CI_CC_key_register = 28,			/* 8 bits (1 byte)		: (uimsbf) 0 = even, 1 = odd, other values not supported. */
	CI_CC_uri_versions = 29,			/* 256 bits (32 bytes)	: Bitmask expressing the URI versions that can be supported by the host. Format is 'uimsbf' */
	CI_CC_status_field = 30,			/* 8 bits (1 byte)		: Status field in APDU confirm messages. */
	CI_CC_srm_data = 31,			/* N/C				: System Renewability Message for HDCP. SRMs for HDCP are defined in HDCP specification. First generation SRMs shall not exceed 5 kilobytes. */
	CI_CC_srm_confirm = 32,			/* 256 bits (32 bytes)	: Hash on the data confirmed by the host. */
	CI_CC_cicam_license = 33,		/* variable			: License from CICAM associated with content. Licenses must not be zero length and shall be padded to the next byte boundary. */ /* added from CI+ v1.3 */
	CI_CC_license_status = 34,		/* 8 bits (1 byte)		: Current status of the content license */ /* added from CI+ v1.3 */
	CI_CC_license_rcvd_status = 35,	/* 8 bits (1 byte)		: Status from the exchange of content license */ /* added from CI+ v1.3 */
	CI_CC_Host_license = 36,			/* variable			: License for which the Host requires current status. Licenses must not be zero length and shall be padded to the next byte boundary. */ /* added from CI+ v1.3 */
	CI_CC_play_count = 37,			/* 8 bits (1 byte)		: Remaining Play Count */ /* added from CI+ v1.3 */
	CI_CC_operating_mode = 38,		/* 8 bits (1 byte)		: Record operating mode */ /* added from CI+ v1.3 */
	CI_CC_PINcode_data = 39,		/* variable			: CICAM PIN code one byte for each pin code digit */ /* added from CI+ v1.3 */
	CI_CC_record_start_status = 40,	/* 8 bits (1 byte)		: CICAM status after a record_start protocol */ /* added from CI+ v1.3 */
	CI_CC_mode_change_status = 41,	/* 8 bits (1 byte)		: CICAM status after a change operating mode protocol */ /* added from CI+ v1.3 */
	CI_CC_record_stop_status = 42	/* 8 bits (1 byte)		: CICAM status after a record_stop protocol */ /* added from CI+ v1.3 */
} CI_CC_DATA_ID;

/****************************************************************************************/
/* ķ���� ȣ��Ʈ�� ���޵Ǵ� �����͵� (�ٸ� �����͵��� ���� keep local �ؾ� �ϴ� �͵鸸) */
/****************************************************************************************/
typedef struct {
	unsigned char		CICAM_BrandCert[CI_MAX_CERT_LEN];
	unsigned short		CICAM_BrandCertLen;
	unsigned char		Kp[32];
	unsigned char		DHPM[256];
	unsigned char		CICAM_DevCert[CI_MAX_CERT_LEN];
	unsigned short		CICAM_DevCertLen;
	unsigned char		Signature_B[256];
	unsigned char		auth_nonce[32];
	unsigned char		Ns_module[8];
	unsigned char		uri_message[8];
	CI_CC_URI_MSG		stUri;
	unsigned short		usProgramNumber; // ȣ��Ʈ�� ca_pmt -> ķ -> ķ�� program_number -> ȣ��Ʈ
	CI_CC_KEY_MODE		enKeyRegister;
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	// for PIN code related operation
	CI_PIN_CAPA_INFO			stPinCapaInfo; 			// CI+ v1.3
	unsigned char				ucPinEnterRetryCount;

	// for Recording related status information
	CI_CC_STATUS				enCcStatus;
	CI_CONTENT_LICENSE_STATUS	enLicenseStatus;

	// for License data(Metadata) of recorded content
	unsigned char				*pucLicenseData;		// CAM�ʿ��� License Data�� Host�� Metafile�� ����ϱ� ���� �ʹ� ª�� �ð��� �ٸ� license�� ���� �ִ� ��� �̽��� ���� �� ����. �̷� �̽� �߻��ϸ� �迭 Ȥ�� ť�� ���� �ʿ�.
	unsigned int				unLicenseDataLen;
	unsigned char				ucPlayCount;
	CI_CC_URI_MSG				stUriForRecContent;
	unsigned short				usProgramNumberForRecContent;
#endif
} CI_CC_CICAM_DATA;

/**********************************************************************************************************************************/
/* ȣ��Ʈ ��ü ���� �����͵� (host license constants / non-volatile �� �ƴϸ鼭 �ٸ� �����͵��� ���� keep local �ؾ� �ϴ� �͵鸸) */
/**********************************************************************************************************************************/
typedef struct {
	unsigned char	Ns_Host[8];
	unsigned char	SEK[16];
	unsigned char	SAK[16];
	unsigned char	UCK[32];
	unsigned char	CCK[16];		// AES-128-CBC ����. DES-56-ECB �� ��쿣 [0-7] �� ���.
	unsigned char	CIV[16];		// AES-128-CBC ����. DES-56-ECB �� ��쿣 ������ ����.
	unsigned char	srm_confirm[32];
} CI_CC_HOST_DATA;

/************/
/* CC phase */
/************/
typedef enum {

	/* CC ���ҽ� �ʱ�ȭ ���� �Ǵ� CC ���ҽ� ���� ���� ���� */
	CI_CC_PHASE_Init = 0,

	/* CC ���ҽ� ���� ���� ���� */
	CI_CC_PHASE_SS_Opened,

	/* 1. cc_open_req
	   2. cc_open_cnf : cc_system_id_bitmask ���ޱ��� �Ϸ��� ����.
	   *. ȣ��Ʈ�� 5�� ���� �����ؾ� �Ѵ�. */
	CI_CC_PHASE_Host_Capability_Evaluation_Replied,

	/* 1. cc_data_req : CI_CC_auth_nonce �ް�,
	   2. cc_data_cnf : CI_CC_DHPH, CI_CC_Signature_A, CI_CC_Host_DevCert, CI_CC_Host_BrandCert ����,
	   3. cc_data_req : CI_CC_DHPM, CI_CC_Signature_B, CI_CC_CICAM_DevCert, CI_CC_CICAM_BrandCert �ް�,
	   4. cc_data_cnf : CI_CC_status_field ���ޱ��� �Ϸ��� ����.
	   *. ķ�� ȣ��Ʈ�� �̹� ���ε��Ǿ� �ִٸ� �� ������ �Ͼ�� �ʴ´� (CI+ ���� 1.2 ����) */
	CI_CC_PHASE_Authentication_Requested,
	CI_CC_PHASE_Authentication_Replied,

	/* 1. cc_data_req : CI_CC_AKH ��û�ް�,
	   2. cc_data_cnf : CI_CC_AKH ���ޱ��� �Ϸ��� ����.
	   *. ȣ��Ʈ�� 5�� ���� �����ؾ� �Ѵ�. ���� ������ AKH �� AKM �� ��ġ���� ������ ķ�� 5��(CI_NUM_OF_AUTH_CTX)���� ���û�ϰ� CI_ARC_ERR_Auth_Retries_Exhausted ó���� ���̴� */
	CI_CC_PHASE_Authentication_Key_Verification_Replied,

	/* 1. cc_data_req : CI_CC_CICAM_ID, CI_CC_Ns_module, CI_CC_key_register �ް� (CI+ ���� 1.1 ���Ŀ��� �� ��� CI_CC_key_register�� ���� �ʴ´�),
	   2. cc_data_cnf : CI_CC_Host_ID, CI_CC_Ns_Host ���ޱ��� �Ϸ��� ����.
	   *. ���Ŀ� cc_sync_req : calculating the new SAC key material ��û�ް� �� ���̴� */
	CI_CC_PHASE_SAC_Establishment_Requested,
	CI_CC_PHASE_SAC_Establishment_Replied,

	/* 1. cc_sac_data_req : CI_CC_uri_versions ��û�ް�,
	   2. cc_sac_data_cnf : CI_CC_uri_versions ���ޱ��� �Ϸ��� ����. */
	CI_CC_PHASE_URI_Version_Negotiation_Replied,

	/* 1. cc_sac_data_req : CI_CC_CICAM_ID, CI_CC_Kp, CI_CC_key_register �ް�,
	   2. cc_sac_data_cnf : CI_CC_Host_ID, CI_CC_status_field ���ޱ��� �Ϸ��� ����.
	   *. ���Ŀ� cc_sac_sync_req : calculating the new CC key ��û�ް� �� ���̴� (�̿� ���� ȣ��Ʈ�� 1�� "�̸�" ���� cc_sac_sync_cnf �����ؾ� �Ѵ�) */
	CI_CC_PHASE_CCK_Calculating_Requested,
	CI_CC_PHASE_CCK_Calculating_Replied

	/* ���Ŀ� URI ���� �� CCK refreshing */

} CI_CC_PHASE;

/*
	SMiT CI+ prototype ķ�� ��츦 ���� ��� (AKH �� AKM �� ��ġ���� ���� ��),

	CI_CC_PHASE_Init ->
	CI_CC_PHASE_SS_Opened ->
	CI_CC_PHASE_Host_Capability_Evaluation_Replied ->
	CI_CC_PHASE_Authentication_Key_Verification_Replied -> ...
	CI_CC_PHASE_Authentication_Requested & CI_CC_PHASE_Authentication_Replied ->
	CI_CC_PHASE_Authentication_Key_Verification_Replied ->
	CI_CC_PHASE_SAC_Establishment_Requested -> CI_CC_PHASE_SAC_Establishment_Replied ->
	CI_CC_PHASE_URI_Version_Negotiation_Replied ->
	CI_CC_PHASE_CCK_Calculating_Requested & CI_CC_PHASE_CCK_Calculating_Replied ->
	...
*/

/***************/
/* CC instance */
/***************/
typedef struct {
	unsigned short		usSsNo;
	CI_CC_PHASE			enPhase;
	SvcCas_CiArcErr_e			enArcErr;
	CI_SLOT_ID			usSlotId;

	#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	HBOOL				bUsedCurAuthCtxIdx; //CAM���� ���� CI_CC_Host_ID ��û�� ������  nCurAuthCtxIdx�� ��¥�� ����Ѵٰ� ���� �����Ƿ� �׶� TRUE�� �Ѵ�.
	#endif
	int					nCurAuthCtxIdx;
	int					nTryAuthCtxIdx;

	unsigned long		ulSacMsgCnt;
	unsigned long		ulCcTimerID; /* <TODO_CI_PLUS> : CC �� �������� �󿡼� ķ�� �������� �ʴ� ���(eCiPlus_CcReport_NoResponseFromCam)�� ���� ���� �ڵ鸵�� ó���϶�... */
	unsigned long		ulUriTimerID;
	CI_CC_CICAM_DATA	stCicamData;
	CI_CC_HOST_DATA		stHostData;
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	CI_CC_PROTOCOL_TYPE			enProtocolType;
	Handle_t					hRecAction;
	Handle_t					hPbAction;
	HUINT32						unMetaIndex;
	CI_CC_LICENSE_RCVD_STATUS	enLicenseRcvdStatus;
	CI_CC_PIN_REPLY_STATUS		enPinReplyStatus;
	unsigned long				ulPbLicenseTimerID;
	CI_CC_RECORDING_STATUS		enRecordingStatus;
#endif
} CI_CC_INSTANCE;

/*******************/
/* CC task message */
/*******************/
typedef struct {
	unsigned short		usSsNo;
	unsigned long		ulApduTag;
	unsigned char		*pucApduData;
	unsigned long		ulApduLen;
} CI_CC_TASK_MSG;

#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS

typedef enum {
	CI_CC_DO_CALCULATE_CCK = 1,
	CI_CC_DO_CALCULATE_SAC
} CI_CC_SUB_TASK_CMD;

typedef struct {
	CI_CC_SUB_TASK_CMD	command;
	CI_CC_INSTANCE		*hCc;
	HBOOL				bDoCalculateCck;
	HBOOL				bDoSetDescrambler;
} CI_CC_SUB_TASK_MSG;

#endif

#ifdef USE_SAC_TASK_TO_AVOID_SAC_MESSAGE_ERROR
typedef enum {
	CI_CC_SAC_RECORD_START = 1,
} CI_CC_SAC_TASK_CMD;

typedef struct {
	CI_CC_SAC_TASK_CMD		command;

	union {
		struct{
			CI_SLOT_ID 			usSlotId;
			Handle_t 			hAction;
			CI_OPERATING_MODE 	eOperatingMode;
			HUINT16				usProgramNumber;
		} stRecStart;
	} utInfo;
} CI_CC_SAC_TASK_MSG;
#endif

/*

	==================================
	 ���� ���� : ȣ��Ʈ Key Hierarchy
	==================================

	< Credentials layer >

	* Root certificate                                                : stored (license constant) / keep local (not replaceable)
	* Host_BrandCert (Brand certificate)                              : stored (license constant) / exchange   (not replaceable)
	* Host_DevCert   (Device certificate)                             : stored (license constant) / exchange   (not replaceable)

	* Host_ID (64 bits) <- Host Device certificate <- Host Manufacturer certificate <- Host Root certificate

	* PRNG_seed (Per manufacturer seed for Pseudo-Random Number Generator) : stored (license constant) / keep local (not replaceable)

	* DH_p (Diffie-Hellman prime modulus)                             : stored (license constant) / keep local
	* DH_g (Diffie-Hellman generator/base modulus)                    : stored (license constant) / keep local
	* DH_q (Diffie-Hellman Sophie Germain constant)                   : stored (license constant) / keep local

	* HDQ  (Host Device Private RSA key)                              : stored (license constant) / keep local (not replaceable)
	* HDP  (Host Device Public RSA key)             : stored                    / exchange

	< Authentication layer >

	* DHH  (DH nonce exponent Host, 2048 bits)            : volatile / keep local <- PRNG
	* DHPH (DH Public key Host, 2048 bits)                : volatile / exchange   <- DHPH = DH_g ^ DHH mod DH_p
	* DHSK (DH shared Secret Key, private key, 2048 bits) : stored   / keep local <- DHSK = DHPM ^ DHH mod DH_p (ķ �ʿ����� DHSK = DHPH ^ DHM mod DH_p)
	* AKH  (Authentication Key Host, 256 bits)            : stored (on host) / exchange (protected) <- AKH = AKM = SHA-256 (CICAM_ID || Host_ID || DHSK), CICAM_ID : ķ�� ������ ���������� ����.

	< SAC layer >

	* Ns_Host (nonce from Host, 64 bits)            : volatile / exchange   <- PRNG
	* Ks_Host (Key seed Host, 256 bits)             : volatile / keep local <- Ks_Host = SHA-256 (DHSK' || AKH || Ns_Host || Ns_Module), DHSK' = The LSB 128 bits of DHSK
	* SLK     (not mentioned in CI+ spec)           : This is the 128 bit AES key for the f-SAC dummy function.
	* SEK     (SAC Encryption Key, 128 bits)        : volatile / keep local <- SEK = f-SAC (Ks_Host)
	* SAK     (SAC Authentication Key, 128 bits)    : volatile / keep local <- SAK = f-SAC (Ks_Host)
	* SIV     (SAC Initialisation Vector, 128 bits) : stored (license constant) / keep local

	< CC layer >

	* CLK (not mentioned in CI+ spec)                                  : This is the 128 bit AES key for the f-CC dummy function.
	* CCK (CC Key, 64 bits for DES-56-ECB or 128 bits for AES-128-CBC) : volatile / keep local <- CCK = f-CC (Kp)
	* CIV (CC Initialisation Vector)                                   : volatile / keep local <- CIV = f-CC (Kp)

	================================================
	 ���� ���� : ȣ��Ʈ���� ���Ǵ� Cipher Algo ��
	================================================

	* DH  : Authentication �ÿ� DH 3-pass �������� ���� (CI_CC_PHASE_Authentication_Requested ~ CI_CC_PHASE_Authentication_Replied)
	* RSA : RSASSA-PSS + SHA-1.
	        1. CICAM_BrandCert �� CICAM_DevCert�� signature �����ÿ� ����.
	        2. Signature_A �����ÿ� ����. Signature_A = A_RSA {HDQ} (TLF (version || msg_label || auth_nonce || DHPH)).
	        3. Signature_B �����ÿ� ����. TLF (version || msg_label || auth_nonce || DHPH || DHPM) = V_RSA {MDP} (Signature_B).
	* SHA : SHA-256.
	        1. AKH �����ÿ� ����.
	        2. Ks_Host �����ÿ� ����.
	        3. uri_confirm �����ÿ� ����. uri_confirm = SHA-256 (uri_message || UCK), UCK = SHA-256 (SAK).
	        4. srm_confirm �����ÿ� ����.
	* DES : contents�� local re-scramble / descramble �ϴµ� DES-56-ECB ���� -> ���� ķ�� ȣ��Ʈ descrambler logic ���� ó���ϹǷ� ���⼭�� ������� ����.
	        clear_packet = D {CCK} (ts_packet)
	* AES : 1. contents�� local re-scramble / descramble �ϴµ� AES-128-CBC ���� -> ���� ķ�� ȣ��Ʈ descrambler logic ���� ó���ϹǷ� ���⼭�� ������� ����.
	           clear_packet = D {CCK, CIV} (ts_packet)
	        2. DHH, Ns_Host �����ÿ� AES-128-ECB based PRNG ���� (with PRNG_seed which is a 128 bit AES key).
	        3. sac_message()'s encrypted_payload �� decrypt / encrypt �ϴµ� AES-128-CBC ���� (with SIV).
	           encrypted_payload = E {SEK, SIV} (original_unencrypted_payload || authentication)
	           payload || authentication = D {SEK, SIV} (encrypted_payload)
	        4. sac_message()'s authentication �� verify / generate �ϴµ� AES-128-XCBC-MAC ����.
	           authentication = MAC {SAK} (length(header) || message_counter || header || original_unencrypted_payload)

	================================
	 ���� ���� : Certificate Format
	================================

	Certificate ::= SEQUENCE {
	     tbsCertificate     TBSCertificate,
	     signatureAlgorithm AlgorithmIdentifier,
	     signatureValue BIT STRING }

	TBSCertificate ::= SEQUENCE {
	     version        [0]   EXPLICIT Version DEFAULT v1, -> CI+ implementations shall use X.509 version 3 (02).
	     serialNumber         CertificateSerialNumber, -> Each certificate shall include a unique serial number which shall be assigned by the issuer of the certificate.
	     signature            AlgorithmIdentifier, -> All certificates use RSASSA-PSS signatures with the default parameters.
	     issuer               Name,
	     validity             Validity, -> "YYMMDDHHMMSSZ" (The year field shall be interpreted as 20YY)
	     subject              Name, -> If Certificate type is Device certificate, CN: <device ID>.
	                                   The device ID is a hexadecimal number that consists of 16 digits.
	                                   To store this number in an X.501 Common Name (CN) attribute, it must be converted into a string.
	                                   Each digit is represented by the corresponding ASCII code, i.e. 1 is written as 0x31 and 7 as 0x37.
	                                   For the hexadecimal digits A to F, uppercase letters are used (hex values 0x41 to 0x46).
	     subjectPublicKeyInfo SubjectPublicKeyInfo, -> CI+ : The RSA key's public exponent shall be 65537 == 0x10001, the modulus length shall be 1024, 2048 or 3072 bits.
	     issuerUniqueID [1]   IMPLICIT UniqueIdentifier OPTIONAL,
	                          -- If present, version MUST be v2 or v3
	                          -> CI+ certificates shall not use unique identifiers.
	     subjectUniqueID[2]   IMPLICIT UniqueIdentifier OPTIONAL,
	                          -- If present, version MUST be v2 or v3
	                          -> CI+ certificates shall not use unique identifiers.
	     extensions     [3]   EXPLICIT Extensions OPTIONAL
	                          -- If present, version MUST be v3, see CI+ spec "9.3.9 extensions".
	                          -- "Key usage" shall always be present and marked as critical.
	                          -- "Scrambler capabilities" shall be present in each device certificate and marked as critical.
	                          -- The optional "CI+ info" shall be present in a device certificate only and shall NOT be declared as critical.
	}

	Version ::= INTEGER { v1(0), v2(1), v3(2) }

	CertificateSerialNumber ::= INTEGER

	Validity ::= SEQUENCE {
	    notBefore Time,
	    notAfter Time }

	Time ::= CHOICE {
	     utcTime UTCTime,
	     generalTime GeneralizedTime }

	UniqueIdentifier ::= BIT STRING

	SubjectPublicKeyInfo ::= SEQUENCE {
	     algorithm AlgorithmIdentifier,
	     subjectPublicKey BIT STRING }

	Extensions ::= SEQUENCE SIZE (1..MAX) OF Extension

	Extension ::= SEQUENCE {
	     extnID OBJECT IDENTIFIER,
	     critical BOOLEAN DEFAULT FALSE,
	     extnValue OCTET STRING }

*/
typedef enum
{
	eCiccInitState_wait,
	eCiccInitState_HostLicenseLoading_Ok,
	eCiccInitState_HostLicenseLoading_Fail,
} eCiCcInitState_t;

/***************************************************************
* CC static / extern variables and structures //////////////////
***************************************************************/

/* host license constants */
static int s_nCiHostLicenseConstType; // 0 : ���� credentials, 1 : LLP test credentials, 2 : SmarDTV test credentials
static CI_HOST_LICENSE_CONST s_stCiHostLicenseConst;

/* authentication contexts */
#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
/* static */ CI_AUTH_CTXs s_stCiAuthCtxs;
#else
/* static */ CI_AUTH_CTXs s_stCiAuthCtxs[CARM_MAX_CISLOT_NUMBER];
#endif

static unsigned char	s_ucCiCcInited = 0;

/* CC instance */
static CI_CC_INSTANCE	s_aCiCc[MAX_CI_CC_SS];
static unsigned char	s_ucCiCcCnt;
static eCiCcInitState_t	s_eCiccInitState = eCiccInitState_wait;

/* CC message queue id and task id */
static unsigned long	s_ulCiCcMsgQId;
static unsigned long	s_ulCiCcTaskId;

#if defined(CONFIT_CI_PLUS_AUTH_MULTI_TASK)
static unsigned long	s_ulCiCc2MsgQId;
static unsigned long	s_ulCiCc2TaskId;
#endif

#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
static unsigned long	s_ulCiCcSubMsgQId;
static unsigned long	s_ulCiCcSubTaskId;
#endif

#ifdef USE_SAC_TASK_TO_AVOID_SAC_MESSAGE_ERROR
static unsigned long	s_ulCiCcSacMsgQId;
static unsigned long	s_ulCiCcSacTaskId;
#endif

/* CC status or error reporting */
extern CALLBACK_HANDLE	g_hCbNotifyCcReport;
static CI_NOTIFY_CC_REPORT_FUNC s_pfnNotifyCcReport;
extern CALLBACK_HANDLE	g_hCbNotifyPinCapaInfo;
static CI_NOTIFY_PIN_CAPA_INFO_FUNC s_pfnNotifyPinCapaInfo;
static CIPLUS_CcReportInfo_t s_stCcReportInfo;

/* CC debugging */
#ifdef _CI_DEBUG_CC_TIME_ISSUE_		// ���� �ִ� �������� 2 slot ��� �ȵǾ� �ִ� ����. ���� ����� �뵵�θ� ����� ��.
	static HBOOL	s_bCcProcBusy = 0;
	#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
	static HBOOL	s_bCalculateSacDone = 0;
	static HBOOL	s_bCalculateCckDone = 0;
	#endif
#endif
#ifdef _CI_DEBUG_CC_PROC_TIME_
static unsigned long s_ulCcProcStartTime, s_ulCcProcEndTime;
#endif
#ifdef _CI_DEBUG_CC_FUNC_TIME_
static unsigned long s_ulCcFuncStartTime, s_ulCcFuncEndTime;
#endif

/* work-around for unusual behaviors */
static HBOOL s_bCiCcSacKeyRefreshedUnusually = 0;

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
static CI_NOTIFY_CC_PIN_STATUS_FUNC	s_pfnNotifyPinStatus;
extern CALLBACK_HANDLE				g_hCbNotifyCcPinStatus;

/* Confirm Data */
static CI_NOTIFY_CC_CNF_DATA_FUNC	s_pfnNotifyCcCnfData;
extern CALLBACK_HANDLE				g_hCbNotifyCcCnfData;
/* content license for Recording content */
static CI_NOTIFY_CC_LICENSE_FUNC 	s_pfnNotifyCcLicense;
extern CALLBACK_HANDLE				g_hCbNotifyCcLicense;

static CI_NOTIFY_CC_PIN_EVENT_FUNC 	s_pfnNotifyCcPinEvent;
extern CALLBACK_HANDLE				g_hCbNotifyCcPinEvent;

#endif

/***************************************************************
* CC static function prototypes ////////////////////////////////
***************************************************************/

static void CiCc_Task(void);

#if defined(CONFIT_CI_PLUS_AUTH_MULTI_TASK)
static void CiCc_Task2(void);
#endif

#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
static void CiCc_SubTask(void);
#endif

#ifdef USE_SAC_TASK_TO_AVOID_SAC_MESSAGE_ERROR
static void CiCc_SacTask(void);
static int CiCc_RecordStart(CI_SLOT_ID usSlotId, Handle_t hAction, CI_OPERATING_MODE eOperatingMode, unsigned short usProgramNumber);
#endif

static void CiCc_CredentialsErrorTimerCb(unsigned long ulTimerId, void *pvParam);
static int CiCc_Proc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);

#if 1		// TODO: �Ʒ� �κ� �Լ� ���� �� if 1 �κ� ���ֱ�.
static SvcCas_CiArcErr_e CiCc_ProcOpenReq(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen);
static SvcCas_CiArcErr_e CiCc_ProcDataReq(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen, unsigned char ucSacFlag);
static SvcCas_CiArcErr_e CiCc_ProcSyncReq(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen, unsigned char ucSacFlag);

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
static HERROR CiCc_GetCcInstanceBySlotId(CI_SLOT_ID usSlotId, CI_CC_INSTANCE **hCc);
static SvcCas_CiArcErr_e CiCc_ProcDataCnf(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen, unsigned char ucSacFlag);
static SvcCas_CiArcErr_e CiCc_ProcPinCapaReply(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen);
static SvcCas_CiArcErr_e CiCc_ProcPinReply(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen);
static SvcCas_CiArcErr_e CiCc_ProcPinEvent(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen);
static HERROR CiCc_VerifyAndSaveCnfData(CI_CC_INSTANCE *hCc, HUINT8 ucSendDataId, HUINT16 usSendDataLen, HUINT8 *pucSendData);
static HERROR CiCc_GetApduFromSacMessage(CI_CC_INSTANCE *hCc, HUINT64 ulApduLen, HUINT8 *pucApduData, HUINT64 *pulApduLen, HUINT8 **pucApdu);
static HERROR CiCc_MakeSacMessage (CI_CC_INSTANCE *hCc, CI_CC_SAC_MESSAGE_TYPE eCiCcMessageType, void *message, HUINT32 *punApduLen, HUINT8 **pucApdu);
#endif

#else
static SvcCas_CiArcErr_e CiCc_OpenConfirm(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen);
static SvcCas_CiArcErr_e CiCc_DataConfirm(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen, unsigned char ucSacFlag);
static SvcCas_CiArcErr_e CiCc_SyncConfirm(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen, unsigned char ucSacFlag);
#endif

static void CiCc_VerifyAndSaveSendData(CI_CC_INSTANCE *hCc, unsigned char ucSendDataId, unsigned short usSendDataLen, unsigned char *pucSendData);
static void CiCc_MakeReqDataAndDoSomething(CI_CC_INSTANCE *hCc, unsigned char ucReqDataId, unsigned short *pusReqDataLen, unsigned char **ppucReqData);

static SvcCas_CiArcErr_e CiCc_GenerateSignature(CI_CC_INSTANCE *hCc, unsigned char *pucSignature);
static SvcCas_CiArcErr_e CiCc_VerifyCamAndSaveInfo(CI_CC_INSTANCE *hCc);

static int CiCc_LoadAkh(CI_CC_INSTANCE *hCc, unsigned char *pucAkh);
static SvcCas_CiArcErr_e CiCc_GenerateHostNonVolatileKeys(CI_CC_INSTANCE *hCc);

#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
static SvcCas_CiArcErr_e CiCc_CalculateSacKeysOnSubTask(CI_CC_INSTANCE *hCc);
static SvcCas_CiArcErr_e CiCc_CalculateCckAndSetDescramblerOnSubTask(CI_CC_INSTANCE *hCc, HBOOL bDoCalculateCck, HBOOL bDoSetDescrambler);
#endif
static SvcCas_CiArcErr_e CiCc_CalculateSacKeys(CI_CC_INSTANCE *hCc);
static SvcCas_CiArcErr_e CiCc_CalculateCckAndSetDescrambler(CI_CC_INSTANCE *hCc, HBOOL bDoCalculateCck, HBOOL bDoSetDescrambler);

static void CiCc_GenerateUriConfirm(CI_CC_INSTANCE *hCc, unsigned char *pucUriConfirm);
static void CiCc_GetMinutesForRL(unsigned char ucRL, unsigned long *pulMinutesForRL);
static void CiCc_UriDoCopyCtrl(CI_CC_INSTANCE *hCc, unsigned char ucMode);
static void CiCc_UriTimerCallback(unsigned long ulTimerId, void *pvParam);

static void CiCc_GenerateSrmConfirm(CI_CC_INSTANCE *hCc, unsigned char *pucSrmData, unsigned short usSrmDataLen);
static void CiCc_SrmDoApply(CI_CC_INSTANCE *hCc, unsigned char *pucSrmData, unsigned short usSrmDataLen);

static void CiCc_DoActionForArcErr(CI_CC_INSTANCE *hCc, SvcCas_CiArcErr_e enArcErr);

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
static CI_AUTH_CTX * CiCc_GetAuthContext(int index);
static int CiCc_PutAuthContext (int index, CI_AUTH_CTX *pstCiAuthCtx);
static HBOOL CiCc_IsUsedAuthContextIndex(int index);
#else
static CI_AUTH_CTX * CiCc_GetAuthContext (CI_SLOT_ID usSlotId, int index);
static int CiCc_PutAuthContext (CI_SLOT_ID usSlotId, int index, CI_AUTH_CTX *pstCiAuthCtx);
#endif

static int CiCc_AddInst(unsigned short usSsNo);
static int CiCc_DeleteInst(unsigned short usSsNo);
static CI_CC_INSTANCE * CiCc_GetNewInst(void);
static CI_CC_INSTANCE * CiCc_GetInst(unsigned short usSsNo);

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
static int CiCc_PinCapabilityReq(CI_CC_INSTANCE *hCc);
static int CiCc_PinCmd(CI_CC_INSTANCE *hCc, HUINT8 *pucPinInfo);
#endif

/***************************************************************
* local Functions for CC resource...
***************************************************************/
HULONG	local_CiCc_GetCcResVersion(CI_CC_INSTANCE *hCc)
{
	CI_SS_HANDLE hCiSession;
	hCiSession = CIRH_GetSs(hCc->usSsNo);

	return hCiSession->ulOpendSsResId;
}

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
// local_CiCc_Cached_Pin_Operation: �����ȭ �� �� �� pin capability�� ���� unattended REC�� ���Ͽ� PIN�� cache�� ���µ�, �̸� ���� Host �� PIN_CMD APDU�� ������ CAM���� ���� ���� Reply�� ���Ͽ� �����ϴ� �ó�����.
HERROR	local_CiCc_Cached_Pin_Operation(CI_CC_INSTANCE *hCc, CI_PIN_STATUS ePinStatus)
{
	/* �Ʒ� if���� ���� ��ȭ�� Cached PIN Ȯ���� ���� PIN_CMD�� �����Ϳ� ���� �������� ���� PIN_REPLY�� ó���ϴ� ������. �Ϲ� MMI�� ���� PIN�� ����Ϳ� ���� ������ if�� ������ ����.  */
	if(hCc->stCicamData.stPinCapaInfo.enPinCapa == CI_PIN_CAPABILITY_ONLY_CAS_WITH_CACHE || hCc->stCicamData.stPinCapaInfo.enPinCapa == CI_PIN_CAPABILITY_BOTH_WITH_CACHE)		// Cached PIN�� ó���ϴ� 3�̳� 4 �϶��� �Ʒ� ������ ����
	{
		CI_AUTH_CTX 	*pstCiAuthCtx = NULL;

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
		if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
#else
		if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
#endif
		{
			HxLOG_Error("[CICC]---> CiCc_ProcPinReply()---> Error : Can't load context !!!\n");
			return ERR_FAIL;
		}

		if (pstCiAuthCtx->eStoredPinVerification == CI_CC_STORED_PIN_STATE_STANDBY_FOR_CACHED_PIN)
		{
			if(ePinStatus == CI_PIN_STATUS_PIN_CORRECT)
			{
				pstCiAuthCtx->eStoredPinVerification = CI_CC_STORED_PIN_STATE_OK;
				hCc->stCicamData.ucPinEnterRetryCount = 0;
			}
			else if(ePinStatus == CI_PIN_STATUS_BAD_PIN)
			{
				pstCiAuthCtx->eStoredPinVerification = CI_CC_STORED_PIN_STATE_FAIL;

				if(hCc->stCicamData.ucPinEnterRetryCount < CI_CC_RECORDING_PIN_RETRY_MAX)
				{
					HxLOG_Info("[CICC]---> CiCc_ProcPinReply()---> PIN Enter Retry Count[%d]...\n", hCc->stCicamData.ucPinEnterRetryCount);
					s_stCcReportInfo.eCcReport = eCiPlus_CcReport_PinErrorRetry;
					hCc->stCicamData.ucPinEnterRetryCount++;
				}
				else
				{
					s_stCcReportInfo.eCcReport = eCiPlus_CcReport_PinError;
					hCc->stCicamData.ucPinEnterRetryCount = 0;
				}

				Callback_Reset(g_hCbNotifyCcReport);
				while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
				{
					(*s_pfnNotifyCcReport)(hCc->usSlotId, &s_stCcReportInfo);
				}
			}
			else	// PIN�� �´��� �ƴ��� Ȯ�� �� ���� ����.
			{
				pstCiAuthCtx->eStoredPinVerification = CI_CC_STORED_PIN_STATE_NOT_VERIFIED;
				s_stCcReportInfo.eCcReport = eCiPlus_CcReport_PinNotVarified;
				hCc->stCicamData.ucPinEnterRetryCount = 0;

				Callback_Reset(g_hCbNotifyCcReport);
				while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
				{
					(*s_pfnNotifyCcReport)(hCc->usSlotId, &s_stCcReportInfo);
				}
			}

		#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
		#else
			if (CiCc_PutAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx, pstCiAuthCtx))
			{
				HxLOG_Error("[CICC]---> CiCc_ProcPinReply()---> Error : Can't shift context !!!\n");
				return ERR_FAIL;
			}
		#endif

		#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
			if (CI_SaveAuthContexts())
		#else
			if (CI_SaveAuthContexts(hCc->usSlotId, &s_stCiAuthCtxs[hCc->usSlotId]))
		#endif
			{
				HxLOG_Error("[CICC]---> CiCc_ProcPinReply()---> Error : Can't save PIN Capability Infomation !!!\n");
				return ERR_FAIL;
			}

			return ERR_OK;
		}
	}

	return ERR_OK;
}

// local_CiCc_Playback_Pin_Operation:
HERROR	local_CiCc_Playback_Pin_Operation(CI_CC_INSTANCE *hCc, CI_PIN_STATUS ePinStatus)
{
	CIPLUS_CcPinStatusInfo_t stPinStatusInfo;

	stPinStatusInfo.hAction 	= hCc->hPbAction;
	stPinStatusInfo.unMetaIndex	= hCc->unMetaIndex;
	stPinStatusInfo.ePinStatus 	= ePinStatus;

	Callback_Reset(g_hCbNotifyCcPinStatus);
	while ((s_pfnNotifyPinStatus = (CI_NOTIFY_CC_PIN_STATUS_FUNC)Callback_GetNextFunc(g_hCbNotifyCcPinStatus)) != NULL)
	{
		if ((*s_pfnNotifyPinStatus)(hCc->usSlotId, &stPinStatusInfo) != ERR_OK)
		{
			HxLOG_Error("[CICC]---> CiCc_ProcPinReply()---> Error : SsNo[%d] - Send pin status failed !!!\n", hCc->usSsNo);
		}
	}

	return ERR_OK;
}

// local_CiCc_WatchTv_Pin_Operation:
HERROR	local_CiCc_WatchTv_Pin_Operation(CI_CC_INSTANCE *hCc, CI_PIN_STATUS ePinStatus)
{
	CIPLUS_CcPinStatusInfo_t stPinStatusInfo;

	// �� �������� WatchTv Action�� �� ����� ����.
	// NULL�� �÷��ְ�, CI MGR �ܿ��� NULL�� ������ Watch TV�� ���� ������ �ľ��ϰ� CI MGR�ܿ��� action�� ã�ƴٰ� ó��.
	// struct ���� �ٸ� enum���� �Ѱ� �ִ� ������� �ٲ㵵 ��.
	stPinStatusInfo.hAction = HANDLE_NULL;
	stPinStatusInfo.unMetaIndex	= 0;
	stPinStatusInfo.ePinStatus = ePinStatus;

	Callback_Reset(g_hCbNotifyCcPinStatus);
	while ((s_pfnNotifyPinStatus = (CI_NOTIFY_CC_PIN_STATUS_FUNC)Callback_GetNextFunc(g_hCbNotifyCcPinStatus)) != NULL)
	{
		if ((*s_pfnNotifyPinStatus)(hCc->usSlotId, &stPinStatusInfo) != ERR_OK)
		{
			HxLOG_Error("[CICC]---> CiCc_ProcPinReply()---> Error : SsNo[%d] - Send pin status failed !!!\n", hCc->usSsNo);
		}
	}

	return ERR_OK;
}
#endif

/***************************************************************
* CC functions /////////////////////////////////////////////////
***************************************************************/

int CiCc_Init(void)
{
	if (s_ucCiCcInited != 0)
	{
		return -1;
	}
	s_ucCiCcInited++;

	s_ucCiCcCnt = 0;
	HxSTD_memset(s_aCiCc, 0, sizeof(CI_CC_INSTANCE) * MAX_CI_CC_SS);

	s_nCiHostLicenseConstType = 0;
	HxSTD_memset(&s_stCiHostLicenseConst, 0, sizeof(CI_HOST_LICENSE_CONST));
#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	HxSTD_memset(&s_stCiAuthCtxs, 0, sizeof(CI_AUTH_CTXs));
#else
	HxSTD_memset(s_stCiAuthCtxs, 0, sizeof(CI_AUTH_CTXs) * CARM_MAX_CISLOT_NUMBER);
#endif

/* create message Queue */
	if (VK_MSG_Create(PAL_CI_CC_MSG_Q_SIZE, sizeof(CI_CC_TASK_MSG), "qCICC-1", &s_ulCiCcMsgQId, VK_SUSPENDTYPE_PRIORITY))
	{
		HxLOG_Error("[CICC]---> CiCc_Init()---> Error : VK_MSG_Create() !!!\n");
		return -1;
	}

	#if defined(CONFIT_CI_PLUS_AUTH_MULTI_TASK)
	if (VK_MSG_Create(PAL_CI_CC_MSG_Q_SIZE, sizeof(CI_CC_TASK_MSG), "qCICC-2", &s_ulCiCc2MsgQId, VK_SUSPENDTYPE_PRIORITY))
	{
		HxLOG_Error("[CICC]---> CiCc_Init()---> Error : VK_MSG_Create() !!!\n");
		return -1;
	}
	#endif

	#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
	if (VK_MSG_Create(PAL_CI_CC_MSG_Q_SIZE, sizeof(CI_CC_SUB_TASK_MSG), "qCICCSUB", &s_ulCiCcSubMsgQId, VK_SUSPENDTYPE_PRIORITY))
	{
		HxLOG_Error("[CICC]---> CiCc_Init()---> Error : VK_MSG_Create() for sub !!!\n");
		return -1;
	}
	#endif

	#ifdef USE_SAC_TASK_TO_AVOID_SAC_MESSAGE_ERROR
	if (VK_MSG_Create(PAL_CI_CC_MSG_Q_SIZE, sizeof(CI_CC_SAC_TASK_MSG), "qCICCSAC", &s_ulCiCcSacMsgQId, VK_SUSPENDTYPE_PRIORITY))
	{
		HxLOG_Error("[CICC]---> CiCc_Init()---> Error : VK_MSG_Create() for sac !!!\n");
		return -1;
	}
	#endif

/* create Task */

	if (VK_TASK_Create((void*)CiCc_Task, PAL_CI_CC_MAIN_TASK_PRIORITY, PAL_CI_CC_TASK_STACK_SIZE, "tCICC-1", NULL, &s_ulCiCcTaskId, 0))
	{
		HxLOG_Error("[CICC]---> CiCc_Init()---> Error : tCICC-1 VK_TASK_Create() !!!\n");
		return -1;
	}

	#if defined(CONFIT_CI_PLUS_AUTH_MULTI_TASK)
	if (VK_TASK_Create((void*)CiCc_Task2, PAL_CI_CC_MAIN_TASK_PRIORITY, PAL_CI_CC_TASK_STACK_SIZE, "tCICC-2", NULL, &s_ulCiCc2TaskId, 0))
	{
		HxLOG_Error("[CICC]---> CiCc_Init()---> Error : tCICC-2 VK_TASK_Create() !!!\n");
		return -1;
	}
	#endif

	#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
	if (VK_TASK_Create((void*)CiCc_SubTask, PAL_CI_CC_SUB_TASK_PRIORITY, PAL_CI_CC_TASK_STACK_SIZE, "tCICCSUB", NULL, &s_ulCiCcSubTaskId, 0))
	{
		HxLOG_Error("[CICC]---> CiCc_Init()---> Error : VK_TASK_Create() for sub !!!\n");
		return -1;
	}
	#endif

	#ifdef USE_SAC_TASK_TO_AVOID_SAC_MESSAGE_ERROR
	if (VK_TASK_Create((void*)CiCc_SacTask, PAL_CI_CC_SUB_TASK_PRIORITY, PAL_CI_CC_TASK_STACK_SIZE, "tCICCSAC", NULL, &s_ulCiCcSacTaskId, 0))
	{
		HxLOG_Error("[CICC]---> CiCc_Init()---> Error : VK_TASK_Create() for sac !!!\n");
		return -1;
	}
	#endif

/* start Task */
	if (VK_TASK_Start(s_ulCiCcTaskId))
	{
		HxLOG_Error("[CICC]---> CiCc_Init()---> Error : tCICC-1 VK_TASK_Start() !!!\n");
		return -1;
	}

	#if defined(CONFIT_CI_PLUS_AUTH_MULTI_TASK)
	if (VK_TASK_Start(s_ulCiCc2TaskId))
		{
			HxLOG_Error("[CICC]---> CiCc_Init()---> Error : tCICC-2 VK_TASK_Start() !!!\n");
			return -1;
		}

	#endif

	#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
	if (VK_TASK_Start(s_ulCiCcSubTaskId))
	{
		HxLOG_Error("[CICC]---> CiCc_Init()---> Error : VK_TASK_Start() for sub !!!\n");
		return -1;
	}
	#endif

	#ifdef USE_SAC_TASK_TO_AVOID_SAC_MESSAGE_ERROR
	if (VK_TASK_Start(s_ulCiCcSacTaskId))
	{
		HxLOG_Error("[CICC]---> CiCc_Init()---> Error : VK_TASK_Start() for sac !!!\n");
		return -1;
	}
	#endif

	return ERR_OK;
}

static void CiCc_Task(void)
{
	CI_CC_TASK_MSG msg;

	// wait until host license constants data downloading task complete
	if (TRUE == CI_MES_IsLicenseDownCbRegSuccess())
	{
		int iCbCheckCount = 0;

		while (FALSE == CI_MES_IsLicenseDownTaskEnd() && iCbCheckCount < CI_CC_LICENSE_DATA_DOWN_CHECK_MAX_COUNT)
		{
			VK_TASK_Sleep(10);
			iCbCheckCount++;
			HxLOG_Error("[CICC]---> CiCc_Task()---> iCbCheckCount = %d !!!\n", iCbCheckCount);
		}
	}

	// load host license constants
	if (CI_LoadHostLicenseConstants(&s_stCiHostLicenseConst))
	{
		HxLOG_Error("[CICC]---> CiCc_Task()---> Error : Can't load CI+ host license constants !!!\n");
		s_stCcReportInfo.eCcReport = eCiPlus_CcReport_CanNotLoadHostLicenseConstants;
		goto ERROR;
	}
	if (s_stCiHostLicenseConst.ucSavedFlag != 1 && s_stCiHostLicenseConst.ucSavedFlag != 2)
	{
		HxLOG_Error("[CICC]---> CiCc_Task()---> Error : CI+ host license constants not saved !!!\n");
		s_stCcReportInfo.eCcReport = eCiPlus_CcReport_HostLicenseConstantsNotSaved;
		goto ERROR;
	}

	// check host license constants type - 0 : ���� credentials, 1 : LLP test credentials, 2 : SmarDTV test credentials
	if (!s_nCiHostLicenseConstType)
	{
		unsigned char	Host_ID[8]	= {0xA3, 0x5E, 0x4E, 0x62, 0x12, 0x44, 0xA3, 0x25};
		unsigned char	SLK[16] 	= {0x9c, 0x69, 0xd1, 0x46, 0x70, 0x0d, 0x81, 0x6c, 0xfd, 0x49, 0x69, 0x71, 0x26, 0x93, 0xa3, 0x8a};
		unsigned char	SIV[16] 	= {0xd1, 0xe8, 0xde, 0x32, 0x2e, 0x44, 0xd8, 0x7c, 0x56, 0x90, 0x81, 0x89, 0x5f, 0x50, 0x50, 0x35};
		unsigned char	CLK[16] 	= {0xae, 0xdc, 0x1b, 0x80, 0x4c, 0x50, 0xd9, 0x5d, 0xad, 0x6d, 0x3d, 0x46, 0xac, 0xd6, 0x01, 0xa8};
		if (HxSTD_memcmp(s_stCiHostLicenseConst.Host_ID, Host_ID, 8) == 0
			&& HxSTD_memcmp(s_stCiHostLicenseConst.SLK, SLK, 16) == 0
			&& HxSTD_memcmp(s_stCiHostLicenseConst.SIV, SIV, 16) == 0
			&& HxSTD_memcmp(s_stCiHostLicenseConst.CLK, CLK, 16) == 0)
			s_nCiHostLicenseConstType = 1;
	}
	if (!s_nCiHostLicenseConstType)
	{
		unsigned char	Host_ID[8]	= {0xAB, 0x2E, 0x5A, 0x01, 0x33, 0x02, 0xC1, 0xF3};
		unsigned char	SLK[16] 	= {0x35, 0x6B, 0xF3, 0xB8, 0xB9, 0x3D, 0x75, 0x9C, 0xCF, 0x5C, 0x25, 0x53, 0x2B, 0xA0, 0xFF, 0x60};
		unsigned char	SIV[16] 	= {0x09, 0x3C, 0xB1, 0x0B, 0xE6, 0xFF, 0x4D, 0xAD, 0x1A, 0x48, 0x85, 0x81, 0x34, 0x8B, 0x17, 0x62};
		unsigned char	CLK[16] 	= {0x17, 0xB5, 0xF8, 0xE0, 0x39, 0xDA, 0x8A, 0x47, 0xDE, 0x05, 0x38, 0xB1, 0x8A, 0x6B, 0x26, 0x51};
		if (HxSTD_memcmp(s_stCiHostLicenseConst.Host_ID, Host_ID, 8) == 0
			&& HxSTD_memcmp(s_stCiHostLicenseConst.SLK, SLK, 16) == 0
			&& HxSTD_memcmp(s_stCiHostLicenseConst.SIV, SIV, 16) == 0
			&& HxSTD_memcmp(s_stCiHostLicenseConst.CLK, CLK, 16) == 0)
			s_nCiHostLicenseConstType = 2;
	}
	HxLOG_Info("CI+ host license constants type is %d\n", s_nCiHostLicenseConstType);

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	if (CI_LoadAuthContexts(&s_stCiAuthCtxs))
	{
		HxLOG_Error("[CICC]---> CiCc_Task()---> CI+ authentication contexts not initialized. Now it will be initialized...\n");

		if (CI_InitAuthContexts())
		{
			HxLOG_Error("[CICC]---> CiCc_Task()---> Error : Can't init CI+ authentication contexts for 1st slot !!!\n");
		}
	}
#else
	// load authentication contexts
	if (svc_cas_DevCiGetNumberOfModule() == 1) // 1 slot ��
	{
		if (CI_LoadAuthContexts(0, &s_stCiAuthCtxs[0]))
		{
			HxLOG_Error("[CICC]---> CiCc_Task()---> CI+ authentication contexts not initialized. Now it will be initialized...\n");

			if (CI_InitAuthContexts(0))
			{
				HxLOG_Error("[CICC]---> CiCc_Task()---> Error : Can't init CI+ authentication contexts for 1st slot !!!\n");
			}
		}
	}
	else if (svc_cas_DevCiGetNumberOfModule() == 2) // 2 slot ��
	{
		if (CI_LoadAuthContexts(0, &s_stCiAuthCtxs[0]))
		{
			HxLOG_Error("[CICC]---> CiCc_Task()---> CI+ authentication contexts for 1st slot not initialized. Now it will be initialized...\n");

			if (CI_InitAuthContexts(0))
			{
				HxLOG_Error("[CICC]---> CiCc_Task()---> Error : Can't init CI+ authentication contexts for 1st slot !!!\n");
			}
		}

		if (CI_LoadAuthContexts(1, &s_stCiAuthCtxs[1]))
		{
			HxLOG_Error("[CICC]---> CiCc_Task()---> CI+ authentication contexts for 2nd slot not initialized. Now it will be initialized...\n");

			if (CI_InitAuthContexts(1))
			{
				HxLOG_Error("[CICC]---> CiCc_Task()---> Error : Can't init CI+ authentication contexts for 2nd slot !!!\n");
			}
		}
	}
	else
	{
		HxLOG_Error("[CICC]---> CiCc_Task()---> Error : wrong number of slots (%d) !!!\n", svc_cas_DevCiGetNumberOfModule());
	}
#endif

	s_eCiccInitState = eCiccInitState_HostLicenseLoading_Ok;

	while (1)
	{
		if (VK_MSG_Receive(s_ulCiCcMsgQId, &msg, sizeof(CI_CC_TASK_MSG)) != VK_OK)
		{
			HxLOG_Error("[CICC]---> CiCc_Task()---> Error : fail to get msg !!!\n");
			continue;
		}

		if (CiCc_Proc(msg.usSsNo, msg.ulApduTag, msg.pucApduData, msg.ulApduLen))
		{
			HxLOG_Error("[CICC]---> CiCc_Task()---> Error : CiCc_Proc(%d, 0x%lx) !!!\n", msg.usSsNo, msg.ulApduTag);
		}

		if (msg.pucApduData)
			OxCAS_Free(msg.pucApduData);
	}

ERROR :

	s_eCiccInitState = eCiccInitState_HostLicenseLoading_Fail;

	{
		// UI ó���� ���� �ݹ��� �� ������ available ���� �ʱ� ������ Ÿ�̸Ӹ� �ɾ ó���ϵ��� �Ѵ�...
		unsigned long ulTimerID = 0;
//		unsigned long ulTaskID = 0;
		VK_TIMER_EventAfter(5000, CiCc_CredentialsErrorTimerCb, NULL, 0, (unsigned long *)&ulTimerID);
		#if 0 // �Ʒ� �Լ��� ������ �ִ� ��... crash �� ���� ������ �ϴ� #else ó�� ó���Ѵ�...
		if (VK_TASK_GetCurrentId(&ulTaskID) == VK_OK)
		{
			VK_TASK_Stop(ulTaskID);
			VK_TASK_Destroy(ulTaskID);
		}
		#else
		while (1)
		{
			VK_TASK_Sleep(10000);
		}
		#endif
	}

}

#if defined(CONFIT_CI_PLUS_AUTH_MULTI_TASK)
static void CiCc_Task2(void)
{
	CI_CC_TASK_MSG msg;

	/* CiCc_Task���� license read�� �� ������ �켱 ��� �ϴٰ� ����� ���� ���� ������ �ϵ��� �Ѵ�.
	*/
	while(1)
	{
		if(s_eCiccInitState == eCiccInitState_wait)
		{
			VK_TASK_Sleep(100);
			continue;
		}
		else if(s_eCiccInitState == eCiccInitState_HostLicenseLoading_Ok)
		{
			break;
		}
		else if(s_eCiccInitState == eCiccInitState_HostLicenseLoading_Fail)
		{
			goto ERROR;
		}
		else
		{
			HxLOG_Error("[CICC]---> CiCc_Task()---> Error : (%d) unknown state!!!\n", s_eCiccInitState);
		}

		VK_TASK_Sleep(100);
	}

	while (1)
	{

		if (VK_MSG_Receive(s_ulCiCc2MsgQId, &msg, sizeof(CI_CC_TASK_MSG)) != VK_OK)
		{
			HxLOG_Error("[CICC]---> CiCc_Task()---> Error : fail to get msg !!!\n");
			continue;
		}

		if (CiCc_Proc(msg.usSsNo, msg.ulApduTag, msg.pucApduData, msg.ulApduLen))
		{
			HxLOG_Error("[CICC]---> CiCc_Task()---> Error : CiCc_Proc(%d, 0x%lx) !!!\n", msg.usSsNo, msg.ulApduTag);
		}

		if (msg.pucApduData)
			OxCAS_Free(msg.pucApduData);
	}

ERROR :

	while (1)
	{
		VK_TASK_Sleep(10000);
	}

}
#endif

#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
static void CiCc_SubTask(void)
{
	CI_CC_SUB_TASK_MSG msg;

	while (1)
	{
		if (VK_MSG_Receive(s_ulCiCcSubMsgQId, &msg, sizeof(CI_CC_SUB_TASK_MSG)) != VK_OK)
		{
			HxLOG_Error("[CICC]---> CiCc_SubTask()---> Error : fail to get msg !!!\n");
			continue;
		}

		switch (msg.command)
		{
			case CI_CC_DO_CALCULATE_CCK:
				if (CiCc_CalculateCckAndSetDescrambler(msg.hCc, msg.bDoCalculateCck, msg.bDoSetDescrambler))
				{
					HxLOG_Error("[CICC]---> CiCc_SubTask()---> Error : CiCc_CalculateCckAndSetDescrambler() !!!\n");
				}
				break;

			case CI_CC_DO_CALCULATE_SAC:
				if (CiCc_CalculateSacKeys(msg.hCc))
				{
					HxLOG_Error("[CICC]---> CiCc_SubTask()---> Error : CiCc_CalculateSacKeys() !!!\n");
				}
				break;

			default:
				HxLOG_Error("[CICC]---> CiCc_SubTask()---> Error : Unknown command %d !!!\n", msg.command);
				break;
		}
	}
}
#endif

#ifdef USE_SAC_TASK_TO_AVOID_SAC_MESSAGE_ERROR
static void CiCc_SacTask(void)
{
	CI_CC_SAC_TASK_MSG msg;

	while (1)
	{
		if (VK_MSG_Receive(s_ulCiCcSacMsgQId, &msg, sizeof(CI_CC_SAC_TASK_MSG)) != VK_OK)
		{
			HxLOG_Error("Error : fail to get msg !!!\n");
			continue;
		}

		switch (msg.command)
		{
			case CI_CC_SAC_RECORD_START:
			{
				CI_CC_INSTANCE 	*hCc = NULL;

				if(CiCc_GetCcInstanceBySlotId(msg.utInfo.stRecStart.usSlotId, &hCc) != ERR_OK)
				{
					HxLOG_Error("Error : CiCc_GetCcInstanceBySlotId %d !!!\n", msg.utInfo.stRecStart.usSlotId);
					break;
				}

				// URI �� CAM�������� ������ �Ǿ��ų�, Timeout�� �ɷ��� ��(Legacy CI content) Record Start�� �ϵ��� ��.
				do{
					VK_TASK_Sleep(1000);
				} while(hCc->stCicamData.stUri.ucMode == CI_CC_URI_MODE_DEFAULT);

				if (hCc->enRecordingStatus != CI_CC_RECORDING_STARTED)
				{
					CiCc_RecordStart(msg.utInfo.stRecStart.usSlotId, msg.utInfo.stRecStart.hAction, msg.utInfo.stRecStart.eOperatingMode, msg.utInfo.stRecStart.usProgramNumber);
				}
				break;
			}
			default:
				HxLOG_Error("Error : Unknown command %d !!!\n", msg.command);
				break;
		}
	}
}

#endif

static void CiCc_CredentialsErrorTimerCb(unsigned long ulTimerId, void *pvParam)
{
	HxLOG_Error("CiCc_CredentialsErrorTimerCb !!!\n");

	// UI ó���� ���� �ݹ�
	if (g_hCbNotifyCcReport)
	{
		Callback_Reset(g_hCbNotifyCcReport);
		while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
		{
			(*s_pfnNotifyCcReport)(0, &s_stCcReportInfo); // �� ��� ���� ���̵�� �ǹ̰� ����.
		}
	}
	else
	{
		HxLOG_Error("[CICC]---> CiCc_CredentialsErrorTimerCb()---> Error : g_hCbNotifyCcReport is NULL !!!\n");
	}
}

int CiCc_SsOpenedProc(unsigned short usSsNo)
{
	if (CiCc_AddInst(usSsNo) < 0)
	{
		return -1;
	}

	return ERR_OK;
}

/*
	Neotion CI+ CAM (Irdeto) �� ��� ���κҸ�Ȯ�� ������ �Ʒ��� ���� ������ �Ѵ�.
	CAM���� ���� URI ���޹ް� URI confirm�� �Ͽ��µ� ���ڱ� SAC �缳���� ��û�Ѵ�.
	���� HOST�� SAC ������ ������ �������µ� CAM���� ���ڱ� CC�� �ݾƹ�����.
	CC�� ��� close�Ǵ� ��찡 ���� �߻��ϴ� ������ �ƴϱ⿡ Recovery ������ �Ʒ��� ���� WA �ڵ带 �����Ѵ�.
	������ �������´�� �Ʒ� ������ ������ ���������̴�.
*/
#define WA_CAM_RESET_WHEN_CC_SS_CLOSED

int CiCc_SsClosedProc(unsigned short usSsNo)
{
#if defined(WA_CAM_RESET_WHEN_CC_SS_CLOSED)
	CI_SLOT_ID		usSlotId = -1;
	CI_CC_PHASE 	ePhase = CI_CC_PHASE_SAC_Establishment_Replied;
	CI_CC_INSTANCE	*hCc;

	hCc = CiCc_GetInst(usSsNo);
	if (hCc == NULL)
	{
		HxLOG_Error("[CICC]---> CiCc_Proc()---> Error : CiCc_GetInst() SsNo[%d] - NULL hCc !!!\n", usSsNo);
		CiCc_DoActionForArcErr(NULL, CI_ARC_ERR_Get_Instance);
		return -1;
	}

	usSlotId = hCc->usSlotId;
	ePhase = hCc->enPhase;
#endif
	HxLOG_Info("[CICC]---> CiCc_SsClosedProc()---> SsNo[%d]...\n", usSsNo);

	if (CiCc_DeleteInst(usSsNo) < 0)
	{
		return -1;
	}
#if defined(WA_CAM_RESET_WHEN_CC_SS_CLOSED)
	if(usSlotId >= 0)
	{
		switch(ePhase)
		{
			case CI_CC_PHASE_SAC_Establishment_Replied:
			case CI_CC_PHASE_CCK_Calculating_Replied:
				if(CARM_CI_CheckModule(usSlotId) == eCAS_DEV_CI_INSERTED)
				{
					HxLOG_Error("[W/A] CC-SS closed... CAM (#%d) reset...\n", usSlotId);

					if(ePhase == CI_CC_PHASE_SAC_Establishment_Replied)
						CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_SAC_Failed);
					else
						CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_CCK_Failed);

					hCc->enArcErr = CI_ARC_ERR_None;
					CI_ResetSlot(usSlotId);
				}
				break;

			default:
				HxLOG_Warning("[W/A] CC-SS closed... CAM (#%d) ePhase (%d)\n", usSlotId, ePhase);
				break;
		}
	}
#endif

	return ERR_OK;
}

int CiCc_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_CC_TASK_MSG msg;

#ifdef _CI_DEBUG_CC_PROC_TIME_
	HxLOG_Info("[CICC]---> CiCc_ApduProc() for Tag[0x%lx] : %ld tick\n", ulApduTag, VK_TIMER_GetSystemTick());
#endif

	if (CiCc_IsInitialized() != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_ApduProc()---> Error : No credentials in this set !!!\n");
		return -1;
	}

#ifdef _CI_DEBUG_CC_TIME_ISSUE_
	if (s_bCcProcBusy)
	{
		HxLOG_Error("[CICC]---> CiCc_ApduProc()---> Error : CiCc_Proc() is busy !!!\n");
	}
	s_bCcProcBusy = 1;
#endif

	msg.usSsNo = usSsNo;
	msg.ulApduTag = ulApduTag;
	if (ulApduLen)
	{
		msg.pucApduData = (unsigned char *)OxCAS_Malloc(ulApduLen);
		if(msg.pucApduData != NULL)
		{
			HxSTD_memcpy(msg.pucApduData, pucApduData, ulApduLen);
			msg.ulApduLen = ulApduLen;
		}
		else
		{
			msg.pucApduData = NULL;
			msg.ulApduLen = 0;
		}
	}
	else
	{
		msg.pucApduData = NULL;
		msg.ulApduLen = 0;
	}

#if defined(CONFIT_CI_PLUS_AUTH_MULTI_TASK)
	{
		CI_CC_INSTANCE *hCc;

		hCc = CiCc_GetInst(usSsNo);
		if (hCc == NULL)
		{
			HxLOG_Error("[CICC]---> CiCc_Proc()---> Error : CiCc_GetInst() SsNo[%d] - NULL hCc !!!\n", usSsNo);
			CiCc_DoActionForArcErr(NULL, CI_ARC_ERR_Get_Instance);
			return -1;
		}

		if(hCc->usSlotId == 0)
		{
			if (VK_MSG_Send(s_ulCiCcMsgQId, &msg, sizeof(CI_CC_TASK_MSG)))
			{
				HxLOG_Error("[CICC]---> CiCc_ApduProc()---> Error : fail to send msg !!!\n");
				if(msg.pucApduData != NULL)
					OxCAS_Free(msg.pucApduData);
				return -1;
			}
		}
		else if(hCc->usSlotId == 1)
		{
			if (VK_MSG_Send(s_ulCiCc2MsgQId, &msg, sizeof(CI_CC_TASK_MSG)))
			{
				HxLOG_Error("[CICC]---> CiCc_ApduProc()---> Error : fail to send msg !!!\n");
				if(msg.pucApduData != NULL)
					OxCAS_Free(msg.pucApduData);
				return -1;
			}
		}
		else
		{
			HxLOG_Error("[CICC]---> CiCc_ApduProc()---> Error : (%d) It's not exist slot id !!!\n", hCc->usSlotId);
			return -1;
		}
	}

#else
	if (VK_MSG_Send(s_ulCiCcMsgQId, &msg, sizeof(CI_CC_TASK_MSG)))
	{
		HxLOG_Error("[CICC]---> CiCc_ApduProc()---> Error : fail to send msg !!!\n");
		if(msg.pucApduData != NULL)
			OxCAS_Free(msg.pucApduData);
		return -1;
	}
#endif

	return ERR_OK;
}

static int CiCc_Proc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_CC_INSTANCE *hCc;
	SvcCas_CiArcErr_e enArcErr;

	hCc = CiCc_GetInst(usSsNo);
	if (hCc == NULL)
	{
		HxLOG_Error("[CICC]---> CiCc_Proc()---> Error : CiCc_GetInst() SsNo[%d] - NULL hCc !!!\n", usSsNo);
		CiCc_DoActionForArcErr(NULL, CI_ARC_ERR_Get_Instance);
		return -1;
	}

	HxLOG_Info("usSsNo = %d, slot id = %d, ulApduTag = %d\n", usSsNo, hCc->usSlotId, ulApduTag);

#ifdef _CI_DEBUG_CC_PROC_TIME_
	s_ulCcProcStartTime = VK_TIMER_GetSystemTick();
#endif

	switch (ulApduTag)
	{
	case CI_CC_OPEN_REQ:
		/*
		�뵵 : Host capability evaluation

		���� APDU
		=========
		cc_open_req() {
			cc_open_req_tag								= CI_CC_OPEN_REQ
			length_field()								= 0
		}

		���� APDU
		=========
		cc_open_cnf() {
			cc_open_cnf_tag								= CI_CC_OPEN_CNF
			length_field()								= 1
			cc_system_id_bitmask					8 bits (see CI_CC_SYSTEM_ID_BITMASK_V1)
		}
		*/

		HxLOG_Info("CI_CC_OPEN_REQ received\n");
		enArcErr = CiCc_ProcOpenReq(hCc, pucApduData, ulApduLen);
		break;

	case CI_CC_DATA_REQ:
		/*
		�뵵 : Authentication, Auth key verification, SAC key calculation

		���� APDU
		=========
		cc_data_req() {
			cc_data_req_tag								= CI_CC_DATA_REQ
			length_field()
			cc_system_id_bitmask					8 bits (see CI_CC_SYSTEM_ID_BITMASK_V1)
			send_datatype_nbr						8 bits (see CI_CC_MAX_DATA_NBR)
			for (i=0; i<send_datatype_nbr; i++) {
				datatype_id						8 bits (see CI_CC_DATA_ID)
				datatype_length							16 bits
				data_type								8*datatype_length bits
			}
			request_datatype_nbr					8 bits (see CI_CC_MAX_DATA_NBR)
			for (i=0; i<request_datatype_nbr; i++) {
				datatype_id						8 bits (see CI_CC_DATA_ID)
			}
		}

		���� APDU
		=========
		cc_data_cnf() {
			cc_data_cnf_tag								= CI_CC_DATA_CNF
			length_field()
			cc_system_id_bitmask					8 bits (see CI_CC_SYSTEM_ID_BITMASK_V1)
			send_datatype_nbr						8 bits (see CI_CC_MAX_DATA_NBR)
			for (i=0; i<send_datatype_nbr; i++) {
				datatype_id						8 bits (see CI_CC_DATA_ID)
				datatype_length							16 bits
				data_type								8*datatype_length bits
			}
		}
		*/

		HxLOG_Info("CI_CC_DATA_REQ received\n");
		enArcErr = CiCc_ProcDataReq(hCc, pucApduData, ulApduLen, CI_CC_SAC_MSG_FLAG_FALSE);
		break;

	case CI_CC_SAC_DATA_REQ:
		/*
		�뵵 : CC key calculation, URI transmission and acknowledgement, URI version negotiation, SRM transmission and acknowledgement, Content license exchange (CI+ v1.3)

		���� APDU
		=========
		cc_sac_data_req() {
			cc_sac_data_req_tag							= CI_CC_SAC_DATA_REQ
			length_field()
			sac_message()
		}
		sac_message()�� encrypted_payload {
			cc_data_req()�� payload (encrypted)
		}

		���� APDU
		=========
		cc_sac_data_cnf() {
			cc_sac_data_cnf_tag							= CI_CC_SAC_DATA_CNF
			length_field()
			sac_message()
		}
		sac_message()�� encrypted_payload {
			cc_data_cnf()�� payload (encrypted)
		}
		*/

		HxLOG_Info("CI_CC_SAC_DATA_REQ received\n");
		enArcErr = CiCc_ProcDataReq(hCc, pucApduData, ulApduLen, CI_CC_SAC_MSG_FLAG_TRUE);
		break;

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	case CI_CC_SAC_DATA_CNF:			// CC resource Version2 �������� CAM -> Host�� Confirm�� ���� ���� ����!
		/*
		�뵵 : Content license exchange (CI+ v1.3)

		���� APDU
		=========
		cc_sac_data_cnf() {
			cc_sac_data_cnf_tag					= CI_CC_SAC_DATA_CNF
			length_field()
			sac_message()
		}
		sac_message()�� encrypted_payload {
			cc_data_cnf()�� payload (encrypted)
		}
		*/

		HxLOG_Info("CI_CC_SAC_DATA_CNF received\n");
		enArcErr = CiCc_ProcDataCnf(hCc, pucApduData, ulApduLen, CI_CC_SAC_MSG_FLAG_TRUE);
		break;
#endif

	case CI_CC_SYNC_REQ:
		/*
		�뵵 : SAC key calculation

		���� APDU
		=========
		cc_sync_req() {
			cc_sync_req_tag								= CI_CC_SYNC_REQ
			length_field()								= 0
		}

		���� APDU
		=========
		cc_sync_cnf() {
			cc_sync_cnf_tag								= CI_CC_SYNC_CNF
			length_field()								= 1
			status_field							8 bits (see CI_CC_STATUS)
		}
		*/

		HxLOG_Info("CI_CC_SYNC_REQ received\n");
		enArcErr = CiCc_ProcSyncReq(hCc, pucApduData, ulApduLen, CI_CC_SAC_MSG_FLAG_FALSE);
		break;

	case CI_CC_SAC_SYNC_REQ:
		/*
		�뵵 : CC key calculation

		���� APDU
		=========
		cc_sac_sync_req() {
			cc_sac_sync_req_tag							= CI_CC_SAC_SYNC_REQ
			length_field()
			sac_message()
		}
		sac_message()�� encrypted_payload {
			empty
		}

		���� APDU
		=========
		cc_sac_sync_cnf() {
			cc_sac_sync_cnf_tag							= CI_CC_SAC_SYNC_CNF
			length_field()
			sac_message()
		}
		sac_message()�� encrypted_payload {
			cc_sync_cnf()�� payload (encrypted)
		}
		*/

		HxLOG_Info("CI_CC_SAC_SYNC_REQ received\n");
		enArcErr = CiCc_ProcSyncReq(hCc, pucApduData, ulApduLen, CI_CC_SAC_MSG_FLAG_TRUE);
		break;

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	case CI_CC_PIN_CAPABILITIES_REPLY:
		/*
		�뵵 : Parental Control (CI+ v1.3)

		���� APDU
		=========
		cc_PIN_capabilities_reply() {
			cc_PIN_capabilities_reply_tag			= CI_CC_PIN_CAPABILITIES_REPLY
			length_field()							= 7
			capability_field						8 bits (see CI_PIN_CAPABILITY)
			pin_change_time_utc					40 bits
			rating								8 bits
		}
		*/

		HxLOG_Info("CI_CC_PIN_CAPABILITIES_REPLY received\n");
		enArcErr = CiCc_ProcPinCapaReply(hCc, pucApduData, ulApduLen);
		break;

	case CI_CC_PIN_REPLY:
		/*
		�뵵 : Parental Control (CI+ v1.3)

		���� APDU
		=========
		cc_PIN_reply() {
			cc_PIN_reply_tag						= CI_CC_PIN_REPLY
			length_field()							= 1
			PINcode_status_field					8 bits (see CI_CC_PIN_STATUS)
		}
		*/

		HxLOG_Info("CI_CC_PIN_REPLY received\n");
		enArcErr = CiCc_ProcPinReply(hCc, pucApduData, ulApduLen);
		break;

	case CI_CC_PIN_EVENT:
		/*
		�뵵 : Parental Control (CI+ v1.3)

		���� APDU
		=========
		cc_PIN_event() {
			cc_PIN_event_tag						= CI_CC_PIN_EVENT
			length_field()							= 25
			program_number						16 bits
			PINcode_status_field					8 bits (see CI_CC_PIN_STATUS)
			rating								8 bits
			pin_event_time_utc					40 bits
			pin_event_time_centiseconds			8 bits
			private_data							8*15 bits
		}
		*/

		HxLOG_Info("CI_CC_PIN_EVENT received\n");
		enArcErr = CiCc_ProcPinEvent(hCc, pucApduData, ulApduLen);
		break;
#endif
	default:
		enArcErr = CI_ARC_ERR_Apdu_Tag;
		HxLOG_Error("[CICC]---> CiCc_Proc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag);
	}

#ifdef _CI_DEBUG_CC_TIME_ISSUE_
	s_bCcProcBusy = 0;
#endif

	HxLOG_Info("[CICC]---> CiCc_Proc()---> SsNo[%d], tag[0x%lx], len[%ld]---> %d\n", usSsNo, ulApduTag, ulApduLen, enArcErr);

#ifdef _CI_DEBUG_CC_PROC_TIME_
	s_ulCcProcEndTime = VK_TIMER_GetSystemTick();
	HxLOG_Info("[CICC]---> CiCc_Proc() for Tag[0x%lx] : %ld ms from %ld tick\n", ulApduTag, s_ulCcProcEndTime - s_ulCcProcStartTime, s_ulCcProcStartTime);
#endif

	if (enArcErr != CI_ARC_ERR_None)
	{
		CiCc_DoActionForArcErr(hCc, enArcErr);
		return -1;
	}
	else
	{
		return ERR_OK;
	}
}

#if 1

static SvcCas_CiArcErr_e CiCc_ProcOpenReq(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char ucMsg[5];

	ucMsg[0] = (unsigned char)((CI_CC_OPEN_CNF >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_CC_OPEN_CNF >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_CC_OPEN_CNF & 0xff);
	ucMsg[3] = 0x01;
	ucMsg[4] = CI_CC_SYSTEM_ID_BITMASK_V1;

	if (CIRH_SendApdu(hCc->usSsNo, ucMsg, 5) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_ProcOpenReq()---> Error : fail to send CI_CC_OPEN_CNF for SsNo[%d] !!!\n", hCc->usSsNo);
		return CI_ARC_ERR_Send_Apdu;
	}

	hCc->enPhase = CI_CC_PHASE_Host_Capability_Evaluation_Replied;

	HxLOG_Info("[CICC]---> CiCc_ProcOpenReq()---> SsNo[%d]...\n", hCc->usSsNo);

	return CI_ARC_ERR_None;
}


static SvcCas_CiArcErr_e CiCc_ProcDataReq(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen, unsigned char ucSacFlag)
{
	CI_CC_SAC_MSG	stSacMsg;						// for host received sac_message()
	unsigned char	*pucDecryptedSacPayload = NULL;	// for host received sac_message()'s payload
	unsigned short	usOriginalSacPayloadLen;		// for host sending sac_message()'s payload
	unsigned short	usAlignedSacPayloadLen;			//
	unsigned char	*pucOriginalSacPayload = NULL;	//
	unsigned char	*pucEncryptedSacPayload = NULL;	//
	unsigned char	ucSacBuf[8];

	unsigned char	*pucData = NULL;					// just pointer to data (don't mem alloc for this)
	unsigned short	usDataLen;

	unsigned char	uc_cc_system_id_bitmask;

	unsigned char	uc_send_data_nbr = 0;
	unsigned char	uc_send_data_id[CI_CC_MAX_DATA_NBR];
	unsigned short	us_send_data_length[CI_CC_MAX_DATA_NBR];
	unsigned char	*puc_send_data[CI_CC_MAX_DATA_NBR] = {NULL,};

	unsigned char	uc_request_data_nbr = 0;
	unsigned char	uc_request_data_id[CI_CC_MAX_DATA_NBR];
	unsigned short	us_request_data_length[CI_CC_MAX_DATA_NBR];
	unsigned char	*puc_request_data[CI_CC_MAX_DATA_NBR] = {NULL,};

	int				pos, i;

	unsigned char	*pucMsg = NULL;
	unsigned long	ulMsgPayloadLen;
	unsigned char	ucLenFieldSize;

	SvcCas_CiArcErr_e		enArcErr = CI_ARC_ERR_None;

	/* ------------------------------------------------ */
	/* --- parse cc_data_req() or cc_sac_data_req() --- */
	/* ------------------------------------------------ */

	if (ucSacFlag) // parse received sac_message(), decrypt its encrypted_payload and verify sac_message() in case of cc_sac_data_req()
	{
		if (pucApduData == NULL || ulApduLen < 9)
		{
			HxLOG_Error("[CICC]---> CiCc_ProcDataReq()---> Error : Invalid ulApduLen[%ld] !!!\n", ulApduLen);
			enArcErr = CI_ARC_ERR_Apdu_Len1;
			goto EXIT;
		}

		stSacMsg.ulMsgCnt = ((unsigned long)pucApduData[0] << 24) & 0xff000000;
		stSacMsg.ulMsgCnt |= (((unsigned long)pucApduData[1] << 16) & 0x00ff0000);
		stSacMsg.ulMsgCnt |= (((unsigned long)pucApduData[2] << 8) & 0x0000ff00);
		stSacMsg.ulMsgCnt |= ((unsigned long)pucApduData[3] & 0x000000ff);

		stSacMsg.ucHdr1 = pucApduData[4];
		stSacMsg.ucHdr2 = pucApduData[5];

		stSacMsg.usPayloadLen = ((unsigned short)pucApduData[6] << 8) & 0xff00;
		stSacMsg.usPayloadLen |= ((unsigned short)pucApduData[7] & 0x00ff);

		stSacMsg.pucPayloadData = &pucApduData[8];

		HxLOG_Info("[CICC]---> CiCc_ProcDataReq()---> MsgCnt = %ld, Hdr = %02X %02X, PayloadLen = %d\n", stSacMsg.ulMsgCnt, stSacMsg.ucHdr1, stSacMsg.ucHdr2, stSacMsg.usPayloadLen);

		// ignore received sac_message() if...
		#if 0 // org
		if (stSacMsg.ulMsgCnt == hCc->ulSacMsgCnt + 1) // The first message shall use number 0x1 and wraps back to 0x1 (not zero).
		{
			hCc->ulSacMsgCnt = (stSacMsg.ulMsgCnt == CI_CC_SAC_MSG_CNT_MAX) ? 0 : stSacMsg.ulMsgCnt;
		}
		else
		{
			PrintErr(("[CICC]---> CiCc_ProcDataReq()---> Error : Invalid ulMsgCnt[%ld]---> %ld expected !!!\n", stSacMsg.ulMsgCnt, hCc->ulSacMsgCnt+1));
			enArcErr = CI_ARC_ERR_SAC_MSG_Counter;
			goto EXIT;
		}
		#else // SMiT �� �޸� Neotion �� ��� SacMsgCnt �� �������� �����Ƿ� �̿� ���� W/A ������.
		if (s_bCiCcSacKeyRefreshedUnusually)
		{
			hCc->ulSacMsgCnt = (stSacMsg.ulMsgCnt == CI_CC_SAC_MSG_CNT_MAX) ? 0 : stSacMsg.ulMsgCnt;
			s_bCiCcSacKeyRefreshedUnusually = 0;
			#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
			hCc->enProtocolType = eCiCcProtocolType_None;
			#endif
		}
		else
		{
			if (stSacMsg.ulMsgCnt == hCc->ulSacMsgCnt + 1) // The first message shall use number 0x1 and wraps back to 0x1 (not zero).
			{
				hCc->ulSacMsgCnt = (stSacMsg.ulMsgCnt == CI_CC_SAC_MSG_CNT_MAX) ? 0 : stSacMsg.ulMsgCnt;
				#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
				hCc->enProtocolType = eCiCcProtocolType_None;
				#endif
			}
			else
			{
				HxLOG_Error("[CICC]---> CiCc_ProcDataReq()---> Error : Invalid ulMsgCnt[%ld]---> %ld expected !!!\n", stSacMsg.ulMsgCnt, hCc->ulSacMsgCnt+1);
				enArcErr = CI_ARC_ERR_SAC_MSG_Counter;
				goto EXIT;
			}
		}
		#endif
		if (stSacMsg.ucHdr1 != 0x01)
		{
			HxLOG_Error("[CICC]---> CiCc_ProcDataReq()---> Error : Invalid ucHdr1[0x%x] !!!\n", stSacMsg.ucHdr1);
			enArcErr = CI_ARC_ERR_SAC_MSG_Header1;
			goto EXIT;
		}
		if ((stSacMsg.ucHdr2 >> 5) != 0x0)
		{
			HxLOG_Error("[CICC]---> CiCc_ProcDataReq()---> Error : Invalid ucHdr2[0x%x] !!!\n", stSacMsg.ucHdr2);
			enArcErr = CI_ARC_ERR_SAC_MSG_Header2;
			goto EXIT;
		}
		if (stSacMsg.usPayloadLen & CI_CC_CIPHER_BLOCK_SIZE_1)
		{
			HxLOG_Error("[CICC]---> CiCc_ProcDataReq()---> Error : Invalid usPayloadLen[%d] !!!\n", stSacMsg.usPayloadLen);
			enArcErr = CI_ARC_ERR_SAC_MSG_PayloadLen;
			goto EXIT;
		}

		// decrypt received sac_message()'s encrypted_payload
		pucDecryptedSacPayload = (unsigned char *)OxCAS_Malloc(stSacMsg.usPayloadLen + 16);
		D_AES_CBC(hCc->stHostData.SEK, stSacMsg.pucPayloadData, pucDecryptedSacPayload, stSacMsg.usPayloadLen + 16, s_stCiHostLicenseConst.SIV); // �� ������ SEK ���� ��ȿ�� ���̴�.

		#if CI_CC_VerifyReceivedSacMessage // verify received sac_message()
		HxSTD_memcpy(stSacMsg.ucAuth, &pucDecryptedSacPayload[stSacMsg.usPayloadLen], 16);
		{
			unsigned char *pucTempBuf = NULL;
			unsigned char MAC[16];
			pucTempBuf = (unsigned char *)OxCAS_Malloc(1+8+stSacMsg.usPayloadLen);
			if(pucTempBuf != NULL)
			{
				pucTempBuf[0] = 4;
				HxSTD_memcpy(&pucTempBuf[1], pucApduData, 8);
				HxSTD_memcpy(&pucTempBuf[1+8], pucDecryptedSacPayload, stSacMsg.usPayloadLen);
				MAC_AES_XCBC(hCc->stHostData.SAK, pucTempBuf, 1+8+stSacMsg.usPayloadLen, MAC); // �� ������ SAK ���� ��ȿ�� ���̴�.
				if(pucTempBuf != NULL)
					OxCAS_Free(pucTempBuf);
				if (HxSTD_memcmp(MAC, stSacMsg.ucAuth, 16) != 0)
				{
					HxLOG_Error("[CICC]---> CiCc_ProcDataReq()---> Error : Invalid authentication !!!\n");
					enArcErr = CI_ARC_ERR_SAC_MSG_Authentication;
					goto EXIT;
				}
			}
			else
			{
				HxLOG_Error("[CICC]---> CiCc_ProcDataReq()---> pucTempBuf is NULL !!!\n");
				enArcErr = CI_ARC_ERR_SAC_MSG_Authentication;
				goto EXIT;
			}
		}
		#endif

		pucData = pucDecryptedSacPayload;
		usDataLen = stSacMsg.usPayloadLen;
	}
	else
	{
		pucData = pucApduData;
		usDataLen = (unsigned short)ulApduLen;
	}

	if (pucData == NULL || usDataLen < 3)
	{
		HxLOG_Error("[CICC]---> CiCc_ProcDataReq()---> Error : Invalid usDataLen[%d] !!!\n", usDataLen);
		enArcErr = CI_ARC_ERR_Apdu_Len2;
		goto EXIT;
	}

	// get cc_system_id_bitmask
	uc_cc_system_id_bitmask = pucData[0]; /* check with CI_CC_SYSTEM_ID_BITMASK_V1 ? */

	// get send_data_nbr
	uc_send_data_nbr = pucData[1];
	if (uc_send_data_nbr > CI_CC_MAX_DATA_NBR)
	{
		HxLOG_Error("[CICC]---> CiCc_ProcDataReq()---> Error : CI_CC_MAX_DATA_NBR(%d) is too small for send %d !!!\n", CI_CC_MAX_DATA_NBR, uc_send_data_nbr);
		uc_send_data_nbr = 0;
		enArcErr = CI_ARC_ERR_Send_data_nbr;
		goto EXIT;
	}

	// get send_data
	pos = 2;
	for (i = 0; i < uc_send_data_nbr; i++)
	{
		uc_send_data_id[i] = pucData[pos];
		us_send_data_length[i] = ((unsigned short)pucData[pos+1] << 8) & 0xff00;
		us_send_data_length[i] |= ((unsigned short)pucData[pos+2] & 0x00ff);
		if (us_send_data_length[i])
		{
			puc_send_data[i] = (unsigned char *)OxCAS_Malloc(us_send_data_length[i]);
			HxSTD_memcpy(puc_send_data[i], &pucData[pos+3], us_send_data_length[i]);
		}
		pos += (3 + us_send_data_length[i]);
	}

	// get request_data_nbr
	uc_request_data_nbr = pucData[pos++];
	if (uc_request_data_nbr > CI_CC_MAX_DATA_NBR)
	{
		HxLOG_Error("[CICC]---> CiCc_ProcDataReq()---> Error : CI_CC_MAX_DATA_NBR(%d) is too small for request %d !!!\n", CI_CC_MAX_DATA_NBR, uc_request_data_nbr);
		uc_request_data_nbr = 0;
		enArcErr = CI_ARC_ERR_Request_data_nbr;
		goto EXIT;
	}

	// get request_data_id
	for (i = 0; i < uc_request_data_nbr; i++)
	{
		uc_request_data_id[i] = pucData[pos++];
	}

	// checking : �����͵� ������ B�� �����ϱ� ���ؼ��� A�� �ʿ��ϴٴ� ���� ���� ���谡 �����Ѵ�. ���� ���忡 ��õ� ������ �������� ������ ������ �߻��Ѵ�.
	#if 0 // #ifdef _CI_DEBUG_CC_DATA_
	VK_Print("\n - CiCc_ProcDataReq() - \n");
	VK_Print("\t cc_system_id_bitmask = 0x%x \n", uc_cc_system_id_bitmask);
	VK_Print("\t send_data_nbr = %d \n", uc_send_data_nbr);
	for (i = 0; i < uc_send_data_nbr; i++)
		VK_Print("\t\t %d : id = %d (%d bits) \n", i, uc_send_data_id[i], us_send_data_length[i] * 8);
	VK_Print("\t request_data_nbr = %d \n", uc_request_data_nbr);
	for (i = 0; i < uc_request_data_nbr; i++)
		VK_Print("\t\t %d : id = %d \n", i, uc_request_data_id[i]);
	VK_Print("\n");
	#endif

	/* -------------------------------------------- */
	/* --- save send_data and make request_data --- */
	/* -------------------------------------------- */

	// verify and save send_data
	for (i = 0; i < uc_send_data_nbr; i++)
	{
		CiCc_VerifyAndSaveSendData(hCc, uc_send_data_id[i], us_send_data_length[i], puc_send_data[i]);
	}

	if(hCc->enArcErr == CI_ARC_ERR_Auth_Key_Verification_Failed)
	{
		HxLOG_Error("Error : Invalid nonce data !!!\n");
		uc_request_data_nbr = 0;
		goto EXIT;
	}

	// make request_data
	for (i = 0; i < uc_request_data_nbr; i++)
	{
		CiCc_MakeReqDataAndDoSomething(hCc, uc_request_data_id[i], &us_request_data_length[i], &puc_request_data[i]);
	}

	/* ----------------------------------------------------------- */
	/* --- make cc_data_cnf() or cc_sac_data_cnf() and send it --- */
	/* ----------------------------------------------------------- */

	if (ucSacFlag)
	{
		/*** make sending sac_message()'s encrypted_payload in case of cc_sac_data_cnf() ***/

		// calculate original & aligned length
		usOriginalSacPayloadLen = 2;
		for (i = 0; i < uc_request_data_nbr; i++)
			usOriginalSacPayloadLen += (3 + us_request_data_length[i]);
		usAlignedSacPayloadLen = GetAlignedLen(usOriginalSacPayloadLen);

		// set sending sac_message()'s message_counter and header
		ucSacBuf[0] = (unsigned char)((stSacMsg.ulMsgCnt >> 24) & 0xff);
		ucSacBuf[1] = (unsigned char)((stSacMsg.ulMsgCnt >> 16) & 0xff);
		ucSacBuf[2] = (unsigned char)((stSacMsg.ulMsgCnt >> 8) & 0xff);
		ucSacBuf[3] = (unsigned char)(stSacMsg.ulMsgCnt & 0xff);
		ucSacBuf[4] = stSacMsg.ucHdr1;
		ucSacBuf[5] = stSacMsg.ucHdr2;
		ucSacBuf[6] = (unsigned char)((usAlignedSacPayloadLen >> 8) & 0xff);
		ucSacBuf[7] = (unsigned char)(usAlignedSacPayloadLen & 0xff);

		// allocate aligned buffers and set data
		pucOriginalSacPayload = (unsigned char *)OxCAS_Malloc(usAlignedSacPayloadLen + 16);
		pucEncryptedSacPayload = (unsigned char *)OxCAS_Malloc(usAlignedSacPayloadLen + 16);
		pucOriginalSacPayload[0] = uc_cc_system_id_bitmask;
		pucOriginalSacPayload[1] = uc_request_data_nbr;
		pos = 2;
		for (i = 0; i < uc_request_data_nbr; i++)
		{
			pucOriginalSacPayload[pos] = uc_request_data_id[i];
			pucOriginalSacPayload[pos+1] = (unsigned char)((us_request_data_length[i] >> 8) & 0xff);
			pucOriginalSacPayload[pos+2] = (unsigned char)(us_request_data_length[i] & 0xff);
			HxSTD_memcpy(&pucOriginalSacPayload[pos+3], puc_request_data[i], us_request_data_length[i]);
			pos += (3 + us_request_data_length[i]);
		}

		// padding
		OneAndZeroesPadding(pucOriginalSacPayload, usOriginalSacPayloadLen);

		// authentication
		{
			unsigned char *pucTempBuf = NULL;
			unsigned char MAC[16];
			pucTempBuf = (unsigned char *)OxCAS_Malloc(1+8+usAlignedSacPayloadLen);
			if(pucTempBuf != NULL)
			{
				pucTempBuf[0] = 4;
				HxSTD_memcpy(&pucTempBuf[1], ucSacBuf, 8);
				HxSTD_memcpy(&pucTempBuf[1+8], pucOriginalSacPayload, usAlignedSacPayloadLen);
				MAC_AES_XCBC(hCc->stHostData.SAK, pucTempBuf, 1+8+usAlignedSacPayloadLen, MAC); // �� ������ SAK ���� ��ȿ�� ���̴�.
				if(pucTempBuf != NULL)
					OxCAS_Free(pucTempBuf);
				HxSTD_memcpy(&pucOriginalSacPayload[usAlignedSacPayloadLen], MAC, 16);
			}
			else
			{
				HxLOG_Error("[CICC]---> CiCc_ProcDataReq()---> pucTempBuf is NULL !!!\n");
				enArcErr = CI_ARC_ERR_SAC_MSG_Authentication;
				goto EXIT;
			}
		}

		// encrypt
		E_AES_CBC(hCc->stHostData.SEK, pucOriginalSacPayload, pucEncryptedSacPayload, usAlignedSacPayloadLen + 16, s_stCiHostLicenseConst.SIV); // �� ������ SEK ���� ��ȿ�� ���̴�.

		/*** make message to send ***/

		// calculate message payload length
		ulMsgPayloadLen = 8 + usAlignedSacPayloadLen + 16;

		// allocate message buffer
		pucMsg = (unsigned char *)OxCAS_Malloc(/*6*/ 8 + ulMsgPayloadLen); /* tag field (3 bytes) + max length field (3->5 bytes) + payload length */

		// set message tag
		pucMsg[0] = (unsigned char)((CI_CC_SAC_DATA_CNF >> 16) & 0xff);
		pucMsg[1] = (unsigned char)((CI_CC_SAC_DATA_CNF >> 8) & 0xff);
		pucMsg[2] = (unsigned char)(CI_CC_SAC_DATA_CNF & 0xff);

		// set message length
		CIRH_ConvertApduLengthField(ulMsgPayloadLen, &pucMsg[3], &ucLenFieldSize);

		// set message payload
		HxSTD_memcpy(&pucMsg[3+ucLenFieldSize], ucSacBuf, 8);
		HxSTD_memcpy(&pucMsg[11+ucLenFieldSize], pucEncryptedSacPayload, usAlignedSacPayloadLen + 16);
	}
	else
	{
		/*** make message to send ***/

		// calculate message payload length
		ulMsgPayloadLen = 2;
		for (i = 0; i < uc_request_data_nbr; i++)
			ulMsgPayloadLen += (3 + us_request_data_length[i]);

		// allocate message buffer
		pucMsg = (unsigned char *)OxCAS_Malloc(/*6*/ 8 + ulMsgPayloadLen); /* tag field (3 bytes) + max length field (3->5 bytes) + payload length */

		// set message tag
		pucMsg[0] = (unsigned char)((CI_CC_DATA_CNF >> 16) & 0xff);
		pucMsg[1] = (unsigned char)((CI_CC_DATA_CNF >> 8) & 0xff);
		pucMsg[2] = (unsigned char)(CI_CC_DATA_CNF & 0xff);

		// set message length
		CIRH_ConvertApduLengthField(ulMsgPayloadLen, &pucMsg[3], &ucLenFieldSize);

		// set message payload
		pucMsg[3+ucLenFieldSize] = uc_cc_system_id_bitmask;
		pucMsg[4+ucLenFieldSize] = uc_request_data_nbr;
		pos = 5 + ucLenFieldSize;
		for (i = 0; i < uc_request_data_nbr; i++)
		{
			pucMsg[pos] = uc_request_data_id[i];
			pucMsg[pos+1] = (unsigned char)((us_request_data_length[i] >> 8) & 0xff);
			pucMsg[pos+2] = (unsigned char)(us_request_data_length[i] & 0xff);
			HxSTD_memcpy(&pucMsg[pos+3], puc_request_data[i], us_request_data_length[i]);
			pos += (3 + us_request_data_length[i]);
		}
	}

	if (CIRH_SendApdu(hCc->usSsNo, pucMsg, 3 + ucLenFieldSize + ulMsgPayloadLen) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_ProcDataReq()---> Error : fail to send APDU for SsNo[%d] !!!\n", hCc->usSsNo);
		enArcErr = CI_ARC_ERR_Send_Apdu;
		goto EXIT;
	}

	HxLOG_Info("[CICC]---> CiCc_ProcDataReq()---> %ld bytes payload sent\n", ulMsgPayloadLen);
	#if 0 // #ifdef _CI_DEBUG_CC_DATA_
	{
		for (i = 0; i < 3 + ucLenFieldSize; i++)
		{
			if ((i & 15) == 0)
				VK_Print("\n");
			VK_Print(" %02x", pucMsg[i]);
		}
		for (i = 0; i < usMsgPayloadLen; i++)
		{
			if ((i & 15) == 0)
				VK_Print("\n");
			VK_Print(" %02x", pucMsg[3 + ucLenFieldSize + i]);
		}
		VK_Print("\n\n");
	}
	#endif

EXIT :

	if (ucSacFlag)
	{
		if (pucDecryptedSacPayload != NULL)
			OxCAS_Free(pucDecryptedSacPayload);
		if (pucOriginalSacPayload != NULL)
			OxCAS_Free(pucOriginalSacPayload);
		if (pucEncryptedSacPayload != NULL)
			OxCAS_Free(pucEncryptedSacPayload);
	}

	for (i = 0; i < uc_send_data_nbr; i++)
	{
		if (puc_send_data[i] != NULL)
			OxCAS_Free(puc_send_data[i]);
	}

	for (i = 0; i < uc_request_data_nbr; i++)
	{
		if (puc_request_data[i] != NULL)
			OxCAS_Free(puc_request_data[i]);
	}

	if (pucMsg != NULL)
		OxCAS_Free(pucMsg);

	HxLOG_Info("[CICC]---> CiCc_ProcDataReq()---> SsNo[%d]...\n", hCc->usSsNo);

	return enArcErr;
}

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
static HERROR CiCc_VerifyReceivedProtocol(CI_CC_INSTANCE *hCc, unsigned char *pucSendDataId, unsigned char ucSendDataNbr)
{
	HUINT8	i = 0;
	HUINT8	ucCount = 0;

	switch (hCc->enProtocolType)
	{
		case eCiCcProtocolType_PlaybackLicenseExchange:
			for (i = 0; i < ucSendDataNbr; i++)
			{
				if(pucSendDataId[i] == CI_CC_program_number || pucSendDataId[i] == CI_CC_license_status || pucSendDataId[i] == CI_CC_uri_message || pucSendDataId[i] == CI_CC_cicam_license)
				{
					ucCount++;
				}
			}
			if (ucCount != 4)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyReceivedProtocol()---> Error : Wrong Protocol type received [%x] !!!\n", hCc->enProtocolType);
				return ERR_FAIL;
			}
			break;
		case eCiCcProtocolType_LicenseCheckExchange:
			for (i = 0; i < ucSendDataNbr; i++)
			{
				if(pucSendDataId[i] == CI_CC_license_status || pucSendDataId[i] == CI_CC_play_count)
				{
					ucCount++;
				}
			}
			if (ucCount != 2)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyReceivedProtocol()---> Error : Wrong Protocol type received [%x] !!!\n", hCc->enProtocolType);
				return ERR_FAIL;
			}
			break;
		case eCiCcProtocolType_RecordStart:
			if(ucSendDataNbr != 1 || pucSendDataId[0] != CI_CC_record_start_status)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyReceivedProtocol()---> Error : Wrong Protocol type received [%x] !!!\n", hCc->enProtocolType);
				return ERR_FAIL;
			}
			break;
		case eCiCcProtocolType_RecordStop:
			if(ucSendDataNbr != 1 || pucSendDataId[0] != CI_CC_record_stop_status)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyReceivedProtocol()---> Error : Wrong Protocol type received [%x] !!!\n", hCc->enProtocolType);
				return ERR_FAIL;
			}
			break;
		case eCiCcProtocolType_ChangeOperatingMode:
			if(ucSendDataNbr != 1 || pucSendDataId[0] != CI_CC_mode_change_status)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyReceivedProtocol()---> Error : Wrong Protocol type received [%x] !!!\n", hCc->enProtocolType);
				return ERR_FAIL;
			}
			break;
		default:
			HxLOG_Error("[CICC]---> CiCc_VerifyReceivedProtocol()---> Error : Wrong Protocol type received [%x] !!!\n", hCc->enProtocolType);
			return ERR_FAIL;
	}

	// ���޹��� Protocol�� tv manager �� tv service�� �÷��� �� protocol�� ��û�ߴ� ����� Action�� ���� �Ǿ� �־�� ��. Action ������ �Ǿ����� �ʴٸ� ġ������ ������
	// Recording ������ �ϳ��� service�� Record Start �� �������� Record Stop�� ���� �������� ������ Action���� ���� �Ǿ�� ��.
	// Playback ������ �׻� Tv manager �� service���� ���� CAM������ ���� ��û�� �ϸ�, �̿����� CAM�� �ٷ� �����ϰ�, ����� playback Action���� �÷� �� �� Action ������ �����ص� ��.
	if (hCc->enProtocolType == eCiCcProtocolType_RecordStart || hCc->enProtocolType == eCiCcProtocolType_RecordStop || hCc->enProtocolType == eCiCcProtocolType_ChangeOperatingMode)
	{
		if (hCc->hRecAction == (Handle_t)NULL)
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyReceivedProtocol()---> Error : Record Action Handle ERROR!! This error must be fixed!!\n");
			return ERR_FAIL;
		}
	}
	else if (hCc->enProtocolType == eCiCcProtocolType_LicenseCheckExchange || hCc->enProtocolType == eCiCcProtocolType_PlaybackLicenseExchange)
	{
		if (hCc->hPbAction == (Handle_t)NULL)
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyReceivedProtocol()---> Error : Playback Action Handle ERROR!! This error must be fixed!!\n");
			return ERR_FAIL;
		}
	}
	else
	{
		HxLOG_Error("[CICC]---> CiCc_VerifyReceivedProtocol()---> Error : Wrong Protocol Type received [%x]! This error must be fixed!!\n", hCc->enProtocolType);
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR CiCc_NofityCnfDataInfo(CI_CC_INSTANCE *hCc)
//	(CI_SLOT_ID usSlotId, Handle_t hAction, CI_CC_PROTOCOL_TYPE eProtocolType, CI_CC_CICAM_DATA *pstCicamData)
{
	CIPLUS_CcConfirmDataInfo_t	stConfirmData;

	stConfirmData.eCcProtocolType = hCc->enProtocolType;

	switch(hCc->enProtocolType)
	{
		case eCiCcProtocolType_PlaybackLicenseExchange:
			stConfirmData.data.stPbLicenseInfo.ucAps = hCc->stCicamData.stUriForRecContent.ucApsCopyCtrl;
			stConfirmData.data.stPbLicenseInfo.ucEmi = hCc->stCicamData.stUriForRecContent.ucEmiCopyCtrl;
			stConfirmData.data.stPbLicenseInfo.ucIct = hCc->stCicamData.stUriForRecContent.ucIctCopyCtrl;
			stConfirmData.data.stPbLicenseInfo.ucRct = hCc->stCicamData.stUriForRecContent.ucRctCopyCtrl;
			stConfirmData.data.stPbLicenseInfo.ucDot = hCc->stCicamData.stUriForRecContent.ucDotCopyCtrl;
			CiCc_GetMinutesForRL(hCc->stCicamData.stUriForRecContent.ucRlCopyCtrl, &stConfirmData.data.stPbLicenseInfo.ulMinutesForRL);

			stConfirmData.data.stPbLicenseInfo.usProgramNumber = hCc->stCicamData.usProgramNumberForRecContent;

			stConfirmData.data.stPbLicenseInfo.unLicenseDataLen = hCc->stCicamData.unLicenseDataLen;
			stConfirmData.data.stPbLicenseInfo.pucLicenseData = hCc->stCicamData.pucLicenseData;
			stConfirmData.data.stPbLicenseInfo.eLicenseStatus = hCc->stCicamData.enLicenseStatus;
			stConfirmData.data.stPbLicenseInfo.unMetaIndex = hCc->unMetaIndex;

			stConfirmData.hAction = hCc->hPbAction;

			if (hCc->ulPbLicenseTimerID)
			{
				VK_TIMER_Cancel(hCc->ulPbLicenseTimerID);
				hCc->ulPbLicenseTimerID = VK_TIMERID_NULL;
			}
			break;
		case eCiCcProtocolType_LicenseCheckExchange:
			stConfirmData.data.stLicenseCheckInfo.eLicenseStatus = hCc->stCicamData.enLicenseStatus;
			stConfirmData.data.stLicenseCheckInfo.ucPlayCount = hCc->stCicamData.ucPlayCount;

			stConfirmData.hAction = hCc->hPbAction;		// �ó������� License Check Exchange�� Action�� �ȿ÷��൵ �� �������� Ȥ�� �𸣴� �ϴ� ó�� ��.
			break;
		case eCiCcProtocolType_RecordStart:
			stConfirmData.data.stRecordStartInfo.eRecordStartStatus = hCc->stCicamData.enCcStatus;
			stConfirmData.hAction = hCc->hRecAction;
			break;
		case eCiCcProtocolType_RecordStop:
			stConfirmData.data.stRecordStopInfo.eRecordStopStatus = hCc->stCicamData.enCcStatus;
			stConfirmData.hAction = hCc->hRecAction;
			break;
		case eCiCcProtocolType_ChangeOperatingMode:
			stConfirmData.data.stChangeOpModeInfo.eChangeOperateModeStatus = hCc->stCicamData.enCcStatus;
			stConfirmData.hAction = hCc->hRecAction;
			break;
		default:
			HxLOG_Error("[CICC]---> CiCc_NofityCnfDataInfo()---> Error : Wrong Protocol type received [%x] !!!\n", hCc->enProtocolType);
			return ERR_FAIL;
	}

	Callback_Reset(g_hCbNotifyCcCnfData);
	while ((s_pfnNotifyCcCnfData = (CI_NOTIFY_CC_CNF_DATA_FUNC)Callback_GetNextFunc(g_hCbNotifyCcCnfData)) != NULL)
	{
		(*s_pfnNotifyCcCnfData)(hCc->usSlotId, &stConfirmData);
	}

	return ERR_OK;
}


static SvcCas_CiArcErr_e CiCc_ProcDataCnf(CI_CC_INSTANCE *hCc, unsigned char *pucData, unsigned long ulDataLen, unsigned char ucSacFlag)
{
	HUINT8		*pucApduData = NULL;
	HUINT64		ulApduLen;
	SvcCas_CiArcErr_e	hErr = CI_ARC_ERR_Not_Defined;
	HUINT8		ucSendDataNbr = 0;
	HUINT8		aucSendDataId[CI_CC_MAX_DATA_NBR];
	HUINT16		usSendDataLength[CI_CC_MAX_DATA_NBR];
	HUINT8		*pucSendData[CI_CC_MAX_DATA_NBR] = {NULL,};
	HUINT8		i;
	HUINT32		nPos = 0;

	/*
		���� APDU
		=======
		cc_data_cnf() {
			cc_data_cnf_tag						= CI_CC_DATA_CNF
			length_field()
			cc_system_id_bitmask					8 bits (see CI_CC_SYSTEM_ID_BITMASK_V1)
			send_datatype_nbr						8 bits
			for (i=0; i<send_datatype_nbr; i++) {
				datatype_id						8 bits (see CI_CC_DATA_ID)
				datatype_length					16 bits
				data_type						8*datatype_length bits
			}
		}
	*/

	if ( (pucData == NULL) || (ulDataLen < 2))
	{
		HxLOG_Error("[CICC]---> CiCc_ProcDataCnf()---> Error : Wrong APDU or Wrong ulApduLen[%ld] !!!\n", ulDataLen);
		hErr = CI_ARC_ERR_Apdu_Len1;
		goto EXIT;
	}

	if(ucSacFlag)
	{
		if(CiCc_GetApduFromSacMessage(hCc, ulDataLen, pucData, &ulApduLen, &pucApduData) != ERR_OK)
		{
			HxLOG_Error("[CICC]---> CiCc_ProcDataCnf()---> Error : CiCc_GetApduFromSacMessage() failed\n");
			hErr = CI_ARC_ERR_Not_Defined;
			goto EXIT;
		}
	}
	else
	{
		ulApduLen = ulDataLen;
		pucApduData = pucData;
	}

	if ((pucApduData[0] & CI_CC_SYSTEM_ID_BITMASK_V1) == 0)
	{
		HxLOG_Error("[CICC]---> CiCc_ProcDataCnf()---> Error : invalid cc_system_id_bitmask (0x%02x)\n", pucApduData[0]);
		hErr = CI_ARC_ERR_Not_Defined;
		goto EXIT;
	}

	ucSendDataNbr = pucApduData[1];
	if ((ucSendDataNbr == 0) || (ucSendDataNbr > CI_CC_MAX_DATA_NBR))
	{
		HxLOG_Error("[CICC]---> CiCc_ProcDataCnf()---> Error : invalid send_datatype_nbr (%d)\n", pucApduData[1]);
		hErr = CI_ARC_ERR_SAC_MSG_Counter;
		goto EXIT;
	}

	// Get Send Data
	nPos = 2;
	for ( i=0; i<ucSendDataNbr; i++ )
	{
		aucSendDataId[i] = pucApduData[nPos];
		usSendDataLength[i] = ((HUINT16)pucApduData[nPos+1] << 8) & 0xff00;
		usSendDataLength[i] |= ((HUINT16)pucApduData[nPos+2] & 0x00ff);
		if (usSendDataLength[i])
		{
			pucSendData[i] = (unsigned char *)OxCAS_Malloc(usSendDataLength[i]);
			HxSTD_memcpy(pucSendData[i], &pucApduData[nPos+3], usSendDataLength[i]);
		}
		nPos += (3 + usSendDataLength[i]);
	}

	if(CiCc_VerifyReceivedProtocol(hCc, aucSendDataId, ucSendDataNbr) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_ProcDataCnf()---> Error : CiCc_VerifyReceivedProtocol(0x%02x)\n", hCc->enProtocolType);
		hErr = CI_ARC_ERR_Not_Defined;
		goto EXIT;
	}

	// process each content(send data) of data cnf apdu
	for (i = 0; i < ucSendDataNbr; i++)
	{
		CiCc_VerifyAndSaveCnfData(hCc, aucSendDataId[i], usSendDataLength[i], pucSendData[i]);
	}

	// TODO: �� message�� ���� ó���ϴ� �Լ� ���� (kkkim)
	if(CiCc_NofityCnfDataInfo(hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_ProcDataCnf()---> Error : CiCc_NofityCnfDataInfo(0x%02x)\n", hCc->enProtocolType);
		hErr = CI_ARC_ERR_Not_Defined;
		goto EXIT;
	}

	hErr = CI_ARC_ERR_None;

EXIT:
	if(ucSacFlag && pucApduData)
		OxCAS_Free(pucApduData);

	return hErr;
}
#endif

static SvcCas_CiArcErr_e CiCc_ProcSyncReq(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen, unsigned char ucSacFlag)
{
	CI_CC_SAC_MSG	stSacMsg;						// for host received sac_message()
	unsigned char	*pucDecryptedSacPayload = NULL;	// for host received sac_message()'s payload
	unsigned short	usOriginalSacPayloadLen;		// for host sending sac_message()'s payload
	unsigned short	usAlignedSacPayloadLen;			//
	unsigned char	*pucOriginalSacPayload = NULL;	//
	unsigned char	*pucEncryptedSacPayload = NULL;	//
	unsigned char	ucSacBuf[8];

	unsigned char	*pucMsg = NULL;
	unsigned long	ulMsgPayloadLen;
	unsigned char	ucLenFieldSize;

	SvcCas_CiArcErr_e		enArcErr = CI_ARC_ERR_None;

	/* ------------------------------------------------ */
	/* --- parse cc_sync_req() or cc_sac_sync_req() --- */
	/* ------------------------------------------------ */

	if (ucSacFlag) // parse received sac_message(), decrypt its encrypted_payload and verify sac_message() in case of cc_sac_sync_req()
	{
		if (pucApduData == NULL || ulApduLen < 9)
		{
			HxLOG_Error("[CICC]---> CiCc_ProcSyncReq()---> Error : Invalid ulApduLen[%ld] !!!\n", ulApduLen);
			enArcErr = CI_ARC_ERR_Apdu_Len1;
			goto EXIT;
		}

		stSacMsg.ulMsgCnt = ((unsigned long)pucApduData[0] << 24) & 0xff000000;
		stSacMsg.ulMsgCnt |= (((unsigned long)pucApduData[1] << 16) & 0x00ff0000);
		stSacMsg.ulMsgCnt |= (((unsigned long)pucApduData[2] << 8) & 0x0000ff00);
		stSacMsg.ulMsgCnt |= ((unsigned long)pucApduData[3] & 0x000000ff);

		stSacMsg.ucHdr1 = pucApduData[4];
		stSacMsg.ucHdr2 = pucApduData[5];

		stSacMsg.usPayloadLen = ((unsigned short)pucApduData[6] << 8) & 0xff00;
		stSacMsg.usPayloadLen |= ((unsigned short)pucApduData[7] & 0x00ff);

		stSacMsg.pucPayloadData = &pucApduData[8];

		HxLOG_Info("[CICC]---> CiCc_ProcSyncReq()---> MsgCnt = %ld, Hdr = %02X %02X, PayloadLen = %d\n", stSacMsg.ulMsgCnt, stSacMsg.ucHdr1, stSacMsg.ucHdr2, stSacMsg.usPayloadLen);

		// ignore received sac_message() if...
		#if 0 // org
		if (stSacMsg.ulMsgCnt == hCc->ulSacMsgCnt + 1) // The first message shall use number 0x1 and wraps back to 0x1 (not zero).
		{
			hCc->ulSacMsgCnt = (stSacMsg.ulMsgCnt == CI_CC_SAC_MSG_CNT_MAX) ? 0 : stSacMsg.ulMsgCnt;
		}
		else
		{
			PrintErr(("[CICC]---> CiCc_ProcSyncReq()---> Error : Invalid ulMsgCnt[%ld]---> %ld expected !!!\n", stSacMsg.ulMsgCnt, hCc->ulSacMsgCnt+1));
			enArcErr = CI_ARC_ERR_SAC_MSG_Counter;
			goto EXIT;
		}
		#else // SMiT �� �޸� Neotion �� ��� SacMsgCnt �� �������� �����Ƿ� �̿� ���� W/A ������.
		if (s_bCiCcSacKeyRefreshedUnusually)
		{
			hCc->ulSacMsgCnt = (stSacMsg.ulMsgCnt == CI_CC_SAC_MSG_CNT_MAX) ? 0 : stSacMsg.ulMsgCnt;
			s_bCiCcSacKeyRefreshedUnusually = 0;
			#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
			hCc->enProtocolType = eCiCcProtocolType_None;
			#endif
		}
		else
		{
			if (stSacMsg.ulMsgCnt == hCc->ulSacMsgCnt + 1) // The first message shall use number 0x1 and wraps back to 0x1 (not zero).
			{
				hCc->ulSacMsgCnt = (stSacMsg.ulMsgCnt == CI_CC_SAC_MSG_CNT_MAX) ? 0 : stSacMsg.ulMsgCnt;
				#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
				hCc->enProtocolType = eCiCcProtocolType_None;
				#endif
			}
			else
			{
				HxLOG_Error("[CICC]---> CiCc_ProcSyncReq()---> Error : Invalid ulMsgCnt[%ld]---> %ld expected !!!\n", stSacMsg.ulMsgCnt, hCc->ulSacMsgCnt+1);
				enArcErr = CI_ARC_ERR_SAC_MSG_Counter;
				goto EXIT;
			}
		}
		#endif
		if (stSacMsg.ucHdr1 != 0x01)
		{
			HxLOG_Error("[CICC]---> CiCc_ProcSyncReq()---> Error : Invalid ucHdr1[0x%x] !!!\n", stSacMsg.ucHdr1);
			enArcErr = CI_ARC_ERR_SAC_MSG_Header1;
			goto EXIT;
		}
		if ((stSacMsg.ucHdr2 >> 5) != 0x0)
		{
			HxLOG_Error("[CICC]---> CiCc_ProcSyncReq()---> Error : Invalid ucHdr2[0x%x] !!!\n", stSacMsg.ucHdr2);
			enArcErr = CI_ARC_ERR_SAC_MSG_Header2;
			goto EXIT;
		}
		if (stSacMsg.usPayloadLen & CI_CC_CIPHER_BLOCK_SIZE_1)
		{
			HxLOG_Error("[CICC]---> CiCc_ProcSyncReq()---> Error : Invalid usPayloadLen[%d] !!!\n", stSacMsg.usPayloadLen);
			enArcErr = CI_ARC_ERR_SAC_MSG_PayloadLen;
			goto EXIT;
		}

		// decrypt received sac_message()'s encrypted_payload
		pucDecryptedSacPayload = (unsigned char *)OxCAS_Malloc(stSacMsg.usPayloadLen + 16);
		D_AES_CBC(hCc->stHostData.SEK, stSacMsg.pucPayloadData, pucDecryptedSacPayload, stSacMsg.usPayloadLen + 16, s_stCiHostLicenseConst.SIV); // �� ������ SEK ���� ��ȿ�� ���̴�.

		#if CI_CC_VerifyReceivedSacMessage // verify received sac_message()
		HxSTD_memcpy(stSacMsg.ucAuth, &pucDecryptedSacPayload[stSacMsg.usPayloadLen], 16);
		{
			unsigned char *pucTempBuf = NULL;
			unsigned char MAC[16];
			pucTempBuf = (unsigned char *)OxCAS_Malloc(1+8+stSacMsg.usPayloadLen);
			if(pucTempBuf != NULL)
			{
				pucTempBuf[0] = 4;
				HxSTD_memcpy(&pucTempBuf[1], pucApduData, 8);
				HxSTD_memcpy(&pucTempBuf[1+8], pucDecryptedSacPayload, stSacMsg.usPayloadLen);
				MAC_AES_XCBC(hCc->stHostData.SAK, pucTempBuf, 1+8+stSacMsg.usPayloadLen, MAC); // �� ������ SAK ���� ��ȿ�� ���̴�.
				if(pucTempBuf != NULL)
					OxCAS_Free(pucTempBuf);
				if (HxSTD_memcmp(MAC, stSacMsg.ucAuth, 16) != 0)
				{
					HxLOG_Error("[CICC]---> CiCc_ProcSyncReq()---> Error : Invalid authentication !!!\n");
					enArcErr = CI_ARC_ERR_SAC_MSG_Authentication;
					goto EXIT;
				}
			}
			else
			{
				HxLOG_Error("[CICC]---> CiCc_ProcSyncReq()---> pucTempBuf is NULL !!!\n");
				enArcErr = CI_ARC_ERR_SAC_MSG_Authentication;
				goto EXIT;
			}
		}
		#endif

		/* nothing to do more for cc_sac_sync_req() */

	}
	else
	{
		/* nothing to do for cc_sync_req() */
	}

	/* --------------------- */
	/* --- calculate key --- */
	/* --------------------- */

	if (ucSacFlag)
	{
		#ifdef CI_CC_CalculateCckBeforeSyncAndSetDescramblerOnSync
		#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
		if (CiCc_CalculateCckAndSetDescramblerOnSubTask(hCc, FALSE, TRUE) == CI_ARC_ERR_CCK_Failed)
		#else
		if (CiCc_CalculateCckAndSetDescrambler(hCc, FALSE, TRUE) == CI_ARC_ERR_CCK_Failed)
		#endif
		{
			HxLOG_Error("[CICC]---> CiCc_ProcSyncReq()---> Error : CCK failed !!!\n"); // �̷� ��Ȳ�� �߻��ϸ� ���. ����� �ʿ�!!
			CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_CCK_Failed);
		}
		#endif

		#if defined(USE_SUB_TASK_TO_CALCULATE_KEYS) && defined(_CI_DEBUG_CC_TIME_ISSUE_)
		if (!s_bCalculateCckDone)
		{
			HxLOG_Error("[CICC]---> CiCc_ProcSyncReq()---> Error : CCK calculation not done yet !!!\n"); // �̷� ��Ȳ�� �߻��ϸ� ���. ����� �ʿ�!!
//			CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_CCK_Failed);
		}
		#endif
	}
	else
	{
		#if defined(USE_SUB_TASK_TO_CALCULATE_KEYS) && defined(_CI_DEBUG_CC_TIME_ISSUE_)
		if (!s_bCalculateSacDone)
		{
			HxLOG_Error("[CICC]---> CiCc_ProcSyncReq()---> Error : SAC calculation not done yet !!!\n"); // �̷� ��Ȳ�� �߻��ϸ� ���. ����� �ʿ�!!
//			CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_SAC_Failed);
		}
		#endif

		if (hCc->ulSacMsgCnt)
		{
			// �̷� ��Ȳ�� �������� ���� ������ ķ ���Խ� SAC ä���� �缳���Ǵ� ����� ���� �̻��� ���̴�!
			// ��·�� SAC key �� refresh �Ǹ� ulSacMsgCnt �� �����ؾ� �Ѵ�.
			HxLOG_Error("[CICC]---> CiCc_ProcSyncReq()---> SAC key refresh on MsgCnt = %ld !!!\n", hCc->ulSacMsgCnt);
			#if 0 // org
			hCc->ulSacMsgCnt = 0;
			#else // SMiT �� �޸� Neotion �� ��� SacMsgCnt �� �������� �����Ƿ� �̿� ���� W/A ������.
			s_bCiCcSacKeyRefreshedUnusually = 1;
			#endif
		}
	}

	/* ----------------------------------------------------------- */
	/* --- make cc_sync_cnf() or cc_sac_sync_cnf() and send it --- */
	/* ----------------------------------------------------------- */

	/* ���⼭ status_field �������� CI_CC_STATUS_HOST_BUSY �� �����ϰ����� ��ǻ� �׻� CI_CC_STATUS_OK �� �����ؾ� �� ��... */

	if (ucSacFlag)
	{
		/*** make sending sac_message()'s encrypted_payload in case of cc_sac_sync_cnf() ***/

		// calculate original & aligned length
		usOriginalSacPayloadLen = 1;
		usAlignedSacPayloadLen = GetAlignedLen(usOriginalSacPayloadLen);

		// set sending sac_message()'s message_counter and header
		ucSacBuf[0] = (unsigned char)((stSacMsg.ulMsgCnt >> 24) & 0xff);
		ucSacBuf[1] = (unsigned char)((stSacMsg.ulMsgCnt >> 16) & 0xff);
		ucSacBuf[2] = (unsigned char)((stSacMsg.ulMsgCnt >> 8) & 0xff);
		ucSacBuf[3] = (unsigned char)(stSacMsg.ulMsgCnt & 0xff);
		ucSacBuf[4] = stSacMsg.ucHdr1;
		ucSacBuf[5] = stSacMsg.ucHdr2;
		ucSacBuf[6] = (unsigned char)((usAlignedSacPayloadLen >> 8) & 0xff);
		ucSacBuf[7] = (unsigned char)(usAlignedSacPayloadLen & 0xff);

		// allocate aligned buffers and set data
		pucOriginalSacPayload = (unsigned char *)OxCAS_Malloc(usAlignedSacPayloadLen + 16);
		pucEncryptedSacPayload = (unsigned char *)OxCAS_Malloc(usAlignedSacPayloadLen + 16);
		pucOriginalSacPayload[0] = CI_CC_STATUS_OK;

		// padding
		OneAndZeroesPadding(pucOriginalSacPayload, usOriginalSacPayloadLen);

		// authentication
		{
			unsigned char *pucTempBuf = NULL;
			unsigned char MAC[16];
			pucTempBuf = (unsigned char *)OxCAS_Malloc(1+8+usAlignedSacPayloadLen);
			if(pucTempBuf != NULL)
			{
				pucTempBuf[0] = 4;
				HxSTD_memcpy(&pucTempBuf[1], ucSacBuf, 8);
				HxSTD_memcpy(&pucTempBuf[1+8], pucOriginalSacPayload, usAlignedSacPayloadLen);
				MAC_AES_XCBC(hCc->stHostData.SAK, pucTempBuf, 1+8+usAlignedSacPayloadLen, MAC); // �� ������ SAK ���� ��ȿ�� ���̴�.
				if(pucTempBuf != NULL)
					OxCAS_Free(pucTempBuf);
				HxSTD_memcpy(&pucOriginalSacPayload[usAlignedSacPayloadLen], MAC, 16);
			}
			else
			{
				HxLOG_Error("[CICC]---> CiCc_ProcSyncReq()---> pucTempBuf is NULL !!!\n");
				enArcErr = CI_ARC_ERR_SAC_MSG_Authentication;
				goto EXIT;
			}
		}

		// encrypt
		E_AES_CBC(hCc->stHostData.SEK, pucOriginalSacPayload, pucEncryptedSacPayload, usAlignedSacPayloadLen + 16, s_stCiHostLicenseConst.SIV); // �� ������ SEK ���� ��ȿ�� ���̴�.

		/*** make message to send ***/

		// calculate message payload length
		ulMsgPayloadLen = 8 + usAlignedSacPayloadLen + 16;

		// allocate message buffer
		pucMsg = (unsigned char *)OxCAS_Malloc(/*6*/ 8 + ulMsgPayloadLen); /* tag field (3 bytes) + max length field (3->5 bytes) + payload length */

		// set message tag
		pucMsg[0] = (unsigned char)((CI_CC_SAC_SYNC_CNF >> 16) & 0xff);
		pucMsg[1] = (unsigned char)((CI_CC_SAC_SYNC_CNF >> 8) & 0xff);
		pucMsg[2] = (unsigned char)(CI_CC_SAC_SYNC_CNF & 0xff);

		// set message length
		CIRH_ConvertApduLengthField(ulMsgPayloadLen, &pucMsg[3], &ucLenFieldSize);

		// set message payload
		HxSTD_memcpy(&pucMsg[3+ucLenFieldSize], ucSacBuf, 8);
		HxSTD_memcpy(&pucMsg[11+ucLenFieldSize], pucEncryptedSacPayload, usAlignedSacPayloadLen + 16);
	}
	else
	{
		ulMsgPayloadLen = 1;
		ucLenFieldSize = 1;
		pucMsg = (unsigned char *)OxCAS_Malloc(3 + ucLenFieldSize + ulMsgPayloadLen);
		pucMsg[0] = (unsigned char)((CI_CC_SYNC_CNF >> 16) & 0xff);
		pucMsg[1] = (unsigned char)((CI_CC_SYNC_CNF >> 8) & 0xff);
		pucMsg[2] = (unsigned char)(CI_CC_SYNC_CNF & 0xff);
		pucMsg[3] = 0x01;
		pucMsg[4] = CI_CC_STATUS_OK;
	}

	if (CIRH_SendApdu(hCc->usSsNo, pucMsg, 3 + ucLenFieldSize + ulMsgPayloadLen) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_ProcSyncReq()---> Error : fail to send APDU for SsNo[%d] !!!\n", hCc->usSsNo);
		enArcErr = CI_ARC_ERR_Send_Apdu;
		goto EXIT;
	}

	HxLOG_Info("[CICC]---> CiCc_ProcSyncReq()---> %ld bytes payload sent\n", ulMsgPayloadLen);
	#if 0 // #ifdef _CI_DEBUG_CC_DATA_
	{
		int i;
		for (i = 0; i < 3 + ucLenFieldSize; i++)
		{
			if ((i & 15) == 0)
				VK_Print("\n");
			HxLOG_Print(" %02x", pucMsg[i]);
		}
		for (i = 0; i < usMsgPayloadLen; i++)
		{
			if ((i & 15) == 0)
				HxLOG_Print("\n");
			HxLOG_Print(" %02x", pucMsg[3 + ucLenFieldSize + i]);
		}
		HxLOG_Print("\n\n");
	}
	#endif

EXIT :

	if (ucSacFlag)
	{
		if (pucDecryptedSacPayload != NULL)
			OxCAS_Free(pucDecryptedSacPayload);
		if (pucOriginalSacPayload != NULL)
			OxCAS_Free(pucOriginalSacPayload);
		if (pucEncryptedSacPayload != NULL)
			OxCAS_Free(pucEncryptedSacPayload);
	}

	if (pucMsg != NULL)
		OxCAS_Free(pucMsg);

	HxLOG_Info("[CICC]---> CiCc_ProcSyncReq()---> SsNo[%d]...\n", hCc->usSsNo);

	return enArcErr;
}

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
static SvcCas_CiArcErr_e CiCc_ProcPinCapaReply(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_PIN_CAPA_INFO 	*pstPinCapaInfo;
	CI_AUTH_CTX 		*pstCiAuthCtx = NULL;

	if (pucApduData == NULL || ulApduLen < 7)
	{
		HxLOG_Error("[CICC]---> CiCc_ProcPinCapaReply()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return CI_ARC_ERR_Apdu_Len1;
	}

	pstPinCapaInfo = &hCc->stCicamData.stPinCapaInfo;
	pstPinCapaInfo->enPinCapa = pucApduData[0];
	pstPinCapaInfo->ucPinChangeTimeUtc[0] = pucApduData[1];
	pstPinCapaInfo->ucPinChangeTimeUtc[1] = pucApduData[2];
	pstPinCapaInfo->ucPinChangeTimeUtc[2] = pucApduData[3];
	pstPinCapaInfo->ucPinChangeTimeUtc[3] = pucApduData[4];
	pstPinCapaInfo->ucPinChangeTimeUtc[4] = pucApduData[5];
	pstPinCapaInfo->ucRating = pucApduData[6];
	pstPinCapaInfo->bPinCapaSet = TRUE;

	HxLOG_Info("SsNo[%d], PinCapa[%02x]...\n", hCc->usSsNo, pstPinCapaInfo->enPinCapa);

	Callback_Reset(g_hCbNotifyPinCapaInfo);
	while ((s_pfnNotifyPinCapaInfo = (CI_NOTIFY_PIN_CAPA_INFO_FUNC)Callback_GetNextFunc(g_hCbNotifyPinCapaInfo)) != NULL)
	{
		HxLOG_Info("usSlotId[%d]...\n", hCc->usSlotId);
		(*s_pfnNotifyPinCapaInfo)(hCc->usSlotId); // �� ��� ���� ���̵�� �ǹ̰� ����.
	}

	if (pstPinCapaInfo->enPinCapa == 0)
	{
		/* 0���� �ʱ�ȭ �� �� �״�� ���� �Ǹ� ��. ���� �ش� ���� �����Ͽ� ���� �� �ʿ� ����. */
	}
	else if(pstPinCapaInfo->enPinCapa == CI_PIN_CAPABILITY_ONLY_CAS || pstPinCapaInfo->enPinCapa == CI_PIN_CAPABILITY_BOTH)
	{
#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
		if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
#else
		if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
#endif
		{
			HxLOG_Error("[CICC]---> CiCc_ProcPinCapaReply()---> Error : Can't load context !!!\n");
			return CI_ARC_ERR_Not_Defined;
		}
		pstCiAuthCtx->ucPinCapability 	= (unsigned char)pstPinCapaInfo->enPinCapa;
		pstCiAuthCtx->ucRating			= pstPinCapaInfo->ucRating;

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
#else
		if (CiCc_PutAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx, pstCiAuthCtx))
		{
			HxLOG_Error("[CICC]---> CiCc_ProcPinCapaReply()---> Error : Can't shift context !!!\n");
			return CI_ARC_ERR_Not_Defined;
		}
#endif


#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
		if(CI_SaveAuthContexts())
#else
		if (CI_SaveAuthContexts(hCc->usSlotId, &s_stCiAuthCtxs[hCc->usSlotId]))

#endif
		{
			HxLOG_Error("[CICC]---> CiCc_ProcPinCapaReply()---> Error : Can't save PIN Capability Infomation !!!\n");
			return CI_ARC_ERR_Not_Defined;
		}

		return ERR_OK;
	}
	else if(pstPinCapaInfo->enPinCapa == CI_PIN_CAPABILITY_ONLY_CAS_WITH_CACHE || pstPinCapaInfo->enPinCapa == CI_PIN_CAPABILITY_BOTH_WITH_CACHE)
	{
		HxDATETIME_t		stDateTime;
		HUINT16				mjd = 0;
		UNIXTIME			utPinLastChangedTime;
		HUINT8				*pucBcdTime = &pstPinCapaInfo->ucPinChangeTimeUtc[2];

		//mjd = get16bit(pstPinCapaInfo->ucPinChangeTimeUtc); //Raw Data -> UNIXTIME ��ȯ Ȯ�� �ʿ�.
		mjd = (pstPinCapaInfo->ucPinChangeTimeUtc[0] << 8) | pstPinCapaInfo->ucPinChangeTimeUtc[1];

		HxLOG_Info("MJD[%d]\n", mjd);
		if (mjd == 0)
		{
			// mjd�� 0���� HxDateTime_ConvertMJDToYMD �Լ��� Ÿ��, Y:[1859] M:[255] D:[235] ������ �����µ�, �� ���� UNIXTIME ��ȯ�� ���� �߻�
			// ���� mjd�� 0�̸� UNIXTIME ��ü�� 0���� set ���൵ ������.
			utPinLastChangedTime = 0;
		}
		else
		{
			HLIB_DATETIME_ConvertMJDToYMD(mjd, &stDateTime.stDate);
			HxLOG_Info("Converted Y:[%d] M:[%d] D:[%d]\n", stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay);
			stDateTime.stTime.ucHour = 0;
			stDateTime.stTime.ucMinute = 0;
			stDateTime.stTime.ucSecond = 0;
			HLIB_DATETIME_ConvertBcdTimeToDecimalTime(pucBcdTime, 3, &stDateTime.stTime);
			HxLOG_Info("Converted H:[%d] M:[%d] S:[%d] MS:[%d]\n", stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond, stDateTime.stTime.usMillisecond);
			HLIB_DATETIME_ConvertDateTimeToUnixTime(&stDateTime, &utPinLastChangedTime);
		}

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
		if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
#else
		if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
#endif
		{
			HxLOG_Error("[CICC]---> CiCc_ProcPinCapaReply()---> Error : Can't load context !!!\n");
			return -1;
		}

		if ((pstCiAuthCtx->ucPinCapability == (unsigned char)pstPinCapaInfo->enPinCapa)
			&& (pstCiAuthCtx->ucRating == pstPinCapaInfo->ucRating)
			&& (pstCiAuthCtx->utPinSavedTimeByUser > utPinLastChangedTime)
			&& (pstCiAuthCtx->eStoredPinVerification != CI_CC_STORED_PIN_STATE_FAIL)
			&& (pstCiAuthCtx->utPinSavedTimeByUser != 0))
		{
			HxLOG_Info("[CICC]---> CiCc_ProcPinCapaReply()---> No Changes. Do nothing. \n");
			return CI_ARC_ERR_None;
		}


		pstCiAuthCtx->ucPinCapability 	= (unsigned char)pstPinCapaInfo->enPinCapa;
		pstCiAuthCtx->ucRating			= pstPinCapaInfo->ucRating;

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
#else
		if (CiCc_PutAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx, pstCiAuthCtx))
		{
			HxLOG_Error("[CICC]---> CiCc_ProcPinCapaReply()---> Error : Can't shift context !!!\n");
			return -1;
		}
#endif

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
		if (CI_SaveAuthContexts())
#else
		if (CI_SaveAuthContexts(hCc->usSlotId, &s_stCiAuthCtxs[hCc->usSlotId]))
#endif
		{
			HxLOG_Error("[CICC]---> CiCc_ProcPinCapaReply()---> Error : Can't save PIN Capability Infomation !!!\n");
			return -1;
		}

		/* UI ����� PIN ����� ��ƾ. ���� PIN�� ���� �����ϵ��� �Ѵ�. */
		// UI ó���� ���� �ݹ�

		s_stCcReportInfo.eCcReport = eCiPlus_CcReport_RequireCicamPin;
		Callback_Reset(g_hCbNotifyCcReport);
		while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
		{
			(*s_pfnNotifyCcReport)(hCc->usSlotId, &s_stCcReportInfo);
		}

	}
	else
	{
		HxLOG_Error("[CICC]---> CiCc_ProcPinCapaReply()---> PIN capability error");
		return CI_ARC_ERR_Not_Defined;
	}

	return CI_ARC_ERR_None;
}

/* �Լ� ����: CiCc_ProcPinReply()�� ���� CAM�ʿ� ���޵� CICAM PIN�� status�� �˷��ش�. 			*/
/* �� �Լ��� �޴� Ÿ�̹�: 1. ���� ��ȭ�� �� cached PIN Ȯ���� ���� PIN_CMD�� ���� ��������...	*/
/*						  2. Watch TV ���¿��� MMI�� ���� PIN �Է� �� �׿����� ��������...		*/
/*						  3. ��ȭ�� Playback �� MMI�� ���� PIN �Է� �� �׿����� ��������...		*/
/* �߰������� �� �Լ��� �Ҹ� ���� �������� ��. ���� �׽�Ʈ �غ��� ����غ����� ����.	 		*/
/* �� APDU�� �޾��� �� � ���� �������� �˾ƾ� �� �ʿ䰡 ������...							*/

// TODO: 1���� ���� ó�� �Ϸ� ��. 2, 3���� ���� ó�� ���� �ʿ�! (kkkim)
static SvcCas_CiArcErr_e CiCc_ProcPinReply(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_PIN_STATUS	ePinStatus;
	HERROR			hErr;

	if (pucApduData == NULL || ulApduLen < 1)
	{
		HxLOG_Error("[CICC]---> CiCc_ProcPinReply()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return CI_ARC_ERR_Not_Defined;
	}

	HxLOG_Info("[CICC]---> CiCc_ProcPinReply()---> SsNo[%d]...\n", hCc->usSsNo);

	ePinStatus = pucApduData[0];

	switch (hCc->enPinReplyStatus)
	{
		case CI_CC_PIN_REPLY_FOR_WATCH_TV:
			hErr = local_CiCc_WatchTv_Pin_Operation(hCc, ePinStatus);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("ERROR: local_CiCc_WatchTv_Pin_Operation!!!\n");
				return CI_ARC_ERR_Not_Defined;
			}
			break;
		case CI_CC_PIN_REPLY_FOR_PLAYBACK:
			hErr = local_CiCc_Playback_Pin_Operation(hCc, ePinStatus);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("ERROR: local_CiCc_Playback_Pin_Operation!!!\n");
				return CI_ARC_ERR_Not_Defined;
			}
			break;
		case CI_CC_PIN_REPLY_FOR_CMD:
			hErr = local_CiCc_Cached_Pin_Operation(hCc, ePinStatus);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("ERROR: local_CiCc_Cached_Pin_Operation!!!\n");
				return CI_ARC_ERR_Not_Defined;
			}
			break;
		default:
			HxLOG_Error("ERROR: undefined enPinReplyStatus!!!\n");
			break;
	}

	// �׻� PIN reply�� ó���ϰ� ���� default ������ Watch TV ���·� �������.
	// ������ Playback �� CMD �� ���� reply�� ���� ������� Ÿ�̹��� ��Ȯ�ѵ�,
	// Watch TV �� ���� �� ����� ���� ����.
	// ��� Watch TV�� ��쿡�� ���⸦ Ÿ�� ���� A/V Blanking�� �� ��찡 �������� �׽�Ʈ �ʿ�. (CAM ��ü���� descramble ���� ���� �� ����)
	hCc->enPinReplyStatus = CI_CC_PIN_REPLY_FOR_WATCH_TV;

	return CI_ARC_ERR_None;
}

static SvcCas_CiArcErr_e CiCc_ProcPinEvent(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_PIN_EVENT_INFO 	stPinEvtInfo;
	HxDATETIME_t		stDateTime;
	HUINT16 			mjd;
	HUINT8				*pucBcdTime;
	HUINT8				ucCount = 0;
	CI_AUTH_CTX 		*pstCiAuthCtx = NULL;

	HxLOG_Info("[CICC]---> CiCc_ProcPinEvent()---> SsNo[%d]...\n", hCc->usSsNo);

	if (pucApduData == NULL || ulApduLen < 25)
	{
		HxLOG_Error("[CICC]---> CiCc_ProcPinEvent()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}

	// todo here ...
	if (hCc->stCicamData.stPinCapaInfo.enPinCapa == CI_PIN_CAPABILITY_NONE)
	{
		HxLOG_Error("Error : CiCc_ProcPinEvent APDU can be received when Pin Capability is not CI_PIN_CAPABILITY_NONE !!!\n");
		return -1;
	}

	#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
	#else
	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
	#endif
	{
		HxLOG_Error("[CICC]---> CiCc_ProcPinEvent()---> Error : Can't load context !!!\n");
		return -1;
	}

	stPinEvtInfo.usProgramNum = get16bit(&pucApduData[ucCount]);
	ucCount += 2;
	stPinEvtInfo.ePinStatus = pucApduData[ucCount];
	ucCount++;
	stPinEvtInfo.ucRating = pucApduData[ucCount];
	ucCount++;
	mjd = get16bit(&pucApduData[ucCount]);
	HLIB_DATETIME_ConvertMJDToYMD(mjd, &stDateTime.stDate);
	ucCount += 2;
	pucBcdTime = &pucApduData[ucCount];
	HLIB_DATETIME_ConvertBcdTimeToDecimalTime(pucBcdTime, 3, &stDateTime.stTime);
	ucCount += 3;
	stDateTime.stTime.usMillisecond = pucApduData[ucCount] * 10;
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stDateTime, &stPinEvtInfo.utEventTime);
	ucCount++;
	HxSTD_memcpy(stPinEvtInfo.aucPrivateData, &pucApduData[ucCount], 15);
	HxSTD_memcpy(stPinEvtInfo.aucBindedCamId, pstCiAuthCtx->ucBindedCamId, 8);

	if (hCc->stCicamData.stPinCapaInfo.enPinCapa == CI_PIN_CAPABILITY_ONLY_CAS_WITH_CACHE || hCc->stCicamData.stPinCapaInfo.enPinCapa == CI_PIN_CAPABILITY_BOTH_WITH_CACHE)
	{
		HBOOL			bStatusChanged = FALSE;

		if (pstCiAuthCtx->eStoredPinVerification == CI_CC_STORED_PIN_STATE_NOT_VERIFIED)
		{
			if(stPinEvtInfo.ePinStatus == CI_PIN_STATUS_PIN_CORRECT)
			{
				pstCiAuthCtx->eStoredPinVerification = CI_CC_STORED_PIN_STATE_OK;
				bStatusChanged = TRUE;
			}
			else if(stPinEvtInfo.ePinStatus == CI_PIN_STATUS_BAD_PIN)
			{
				pstCiAuthCtx->eStoredPinVerification = CI_CC_STORED_PIN_STATE_FAIL;
				bStatusChanged = TRUE;
			}

			if(bStatusChanged == TRUE)
			{
			#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
				if (CI_SaveAuthContexts())
				{
					HxLOG_Error("[CICC]---> CiCc_ProcPinEvent()---> Error : Can't save PIN Capability Infomation !!!\n");
					return -1;
				}
			#else
				if (CiCc_PutAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx, pstCiAuthCtx))
				{
					HxLOG_Error("[CICC]---> CiCc_ProcPinEvent()---> Error : Can't shift context !!!\n");
					return -1;
				}
				if (CI_SaveAuthContexts(hCc->usSlotId, &s_stCiAuthCtxs[hCc->usSlotId]))
				{
					HxLOG_Error("[CICC]---> CiCc_ProcPinEvent()---> Error : Can't save PIN Capability Infomation !!!\n");
					return -1;
				}
			#endif
			}
		}
	}

	Callback_Reset(g_hCbNotifyCcPinEvent);
	while ((s_pfnNotifyCcPinEvent = (CI_NOTIFY_CC_PIN_EVENT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcPinEvent)) != NULL)
	{
		if ((*s_pfnNotifyCcPinEvent)(hCc->usSlotId, &stPinEvtInfo, hCc->hRecAction) != ERR_OK)
		{
			HxLOG_Error("[CICC]---> CiCc_ProcPinEvent()---> Error : SsNo[%d] - Send pin status failed !!!\n", hCc->usSsNo);
		}
	}

	return CI_ARC_ERR_None;
}

/* CI+ ver1.3 �߰� : Host�� CAM���� �ʿ��� �����͸� ��� ���� cc_sac_data_req()�� ������ �׿� ���� ������ cc_sac_data_cnf() �ް� �� Content�� CICAM Data�ʿ� ����. 	*/
/*					CiCc_VerifyAndSaveSendData() �Լ� �����̶� ���� �Ȱ��� ������ �����ؼ� �ᵵ �ɰ� ������, Ȥ�� �� side effect������ ���� ���� ��. 				*/
/*					�����ؾ� �� Data�� CC Instance �ȿ� CICAM�� �����ϴµ�, CiCc_VerifyAndSaveSendData���� �����ϴ� ������ ���� ���� ����. ���� ������ ���� ������!	*/
static HERROR CiCc_VerifyAndSaveCnfData(CI_CC_INSTANCE *hCc, HUINT8 ucSendDataId, HUINT16 usSendDataLen, HUINT8 *pucSendData)
{
	switch (ucSendDataId)
	{
		case CI_CC_record_start_status:
			if (usSendDataLen != 1)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveCnfData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				return ERR_FAIL;
			}
			hCc->stCicamData.enCcStatus = (CI_CC_STATUS)pucSendData[0];
			if (hCc->stCicamData.enCcStatus == CI_CC_STATUS_OK)
			{
				hCc->enRecordingStatus = CI_CC_RECORDING_STARTED;
			}
			break;

		case CI_CC_record_stop_status:
			if (usSendDataLen != 1)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveCnfData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				return ERR_FAIL;
			}
			hCc->stCicamData.enCcStatus = (CI_CC_STATUS)pucSendData[0];
			hCc->enRecordingStatus = CI_CC_RECORDING_STOPPED;
			break;

		case CI_CC_mode_change_status:
			if (usSendDataLen != 1)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveCnfData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				return ERR_FAIL;
			}
			hCc->stCicamData.enCcStatus = (CI_CC_STATUS)pucSendData[0];
			break;

		case CI_CC_program_number:
			if (usSendDataLen != 2)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveCnfData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_program_number);
			}
			else
			{
				hCc->stCicamData.usProgramNumberForRecContent = ((unsigned short)pucSendData[0] << 8) & 0xff00; // keep program_number for further use
				hCc->stCicamData.usProgramNumberForRecContent |= ((unsigned short)pucSendData[1] & 0x00ff);
			}
			HxLOG_Info("CI_CC_program_number done\n");
			break;

		case CI_CC_license_status:
			if (usSendDataLen != 1)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveCnfData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				return ERR_FAIL;
			}
			hCc->stCicamData.enLicenseStatus = (CI_CONTENT_LICENSE_STATUS)pucSendData[0];
			break;

		case CI_CC_uri_message:
			if (usSendDataLen != 8)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveCnfData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_uri_message);
			}
			else
			{
				if (pucSendData[0] != CI_CC_URI_PROTOCOL_V1 && pucSendData[0] != CI_CC_URI_PROTOCOL_V2)
				{
					HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveCnfData()---> Error : URI protocol not supported[0x%x] !!!\n", pucSendData[0]);
					/* ȣȯ�� ��������... �����ϴ� version �� �ƴϾ �ϴ� ��� �ְ� ó���� ������... */
				}

				if (pucSendData[0] == CI_CC_URI_PROTOCOL_V1)
				{
					hCc->stCicamData.stUriForRecContent.ucApsCopyCtrl = (pucSendData[1] & 0xC0) >> 6;
					hCc->stCicamData.stUriForRecContent.ucEmiCopyCtrl = (pucSendData[1] & 0x30) >> 4;
					hCc->stCicamData.stUriForRecContent.ucIctCopyCtrl = (pucSendData[1] & 0x08) >> 3;
					hCc->stCicamData.stUriForRecContent.ucRctCopyCtrl = (pucSendData[1] & 0x04) >> 2;
					hCc->stCicamData.stUriForRecContent.ucDotCopyCtrl = CI_CC_URI_DOT_OFF;
					// Retention Limit�� 90������ �����ϴ� CI+ v1.2 CAM �� ����
					// ��� CI+ CAM(Irdeto CI+CAM ������)�� RL�� 90������ �����µ��� �ұ��ϰ� ��ȭ ���� 90���� ������ PlayBack�� �����ϵ��� ��û��.
					// �̴� CI_CC_URI_PROTOCOL_V2 Syntax�� ������ ��Ȯ�ϴ�.
					// ���� Retention Limit�� EMI ���� Copy Never �� ������ �����ϵ��� �Ѵ�.
					// Ȥ�� ���Ŀ� EMI���� Copy Never�� �ƴѵ�, Retention Limit ���� �����ؾ� �ϴ� CI+ v1.2 CAM�� �ִٸ� �߰��� ����� �غ��� �Ѵ�. (�̷� ���� ������ ��.)
					if (hCc->stCicamData.stUriForRecContent.ucEmiCopyCtrl == CI_CC_URI_EMI_COPY_NEVER)
					{
						hCc->stCicamData.stUriForRecContent.ucRlCopyCtrl = pucSendData[2] & 0x3F;
					}
					else
					{
						hCc->stCicamData.stUriForRecContent.ucRlCopyCtrl = CI_CC_URI_RL_UNLIMITED;
					}
				}
				else // if (pucSendData[0] == CI_CC_URI_PROTOCOL_V2)
				{
					hCc->stCicamData.stUriForRecContent.ucApsCopyCtrl = (pucSendData[1] & 0xC0) >> 6;
					hCc->stCicamData.stUriForRecContent.ucEmiCopyCtrl = (pucSendData[1] & 0x30) >> 4;
					hCc->stCicamData.stUriForRecContent.ucIctCopyCtrl = (pucSendData[1] & 0x08) >> 3;
					if (hCc->stCicamData.stUriForRecContent.ucEmiCopyCtrl == CI_CC_URI_EMI_COPY_FREE)
					{
						hCc->stCicamData.stUriForRecContent.ucRctCopyCtrl = (pucSendData[1] & 0x04) >> 2;
					}
					else
					{
						hCc->stCicamData.stUriForRecContent.ucRctCopyCtrl = CI_CC_URI_RCT_OFF;
					}
					if (hCc->stCicamData.stUriForRecContent.ucEmiCopyCtrl == CI_CC_URI_EMI_COPY_NEVER)
					{
						hCc->stCicamData.stUriForRecContent.ucDotCopyCtrl = (pucSendData[1] & 0x01);
						hCc->stCicamData.stUriForRecContent.ucRlCopyCtrl = pucSendData[2];
					}
					else
					{
						hCc->stCicamData.stUriForRecContent.ucDotCopyCtrl = CI_CC_URI_DOT_OFF;
						hCc->stCicamData.stUriForRecContent.ucRlCopyCtrl = CI_CC_URI_RL_UNLIMITED;
					}
				}
			}
			HxLOG_Info("CI_CC_uri_message done\n");
			break;

		case CI_CC_cicam_license:
			hCc->stCicamData.unLicenseDataLen = usSendDataLen;
			if( hCc->stCicamData.pucLicenseData != NULL )
			{
				OxCAS_Free(hCc->stCicamData.pucLicenseData);
				hCc->stCicamData.pucLicenseData = NULL;
			}
			hCc->stCicamData.pucLicenseData = OxCAS_Malloc(usSendDataLen);
			HxSTD_memcpy(hCc->stCicamData.pucLicenseData, pucSendData, usSendDataLen);
			break;

		case CI_CC_play_count:
			if (usSendDataLen != 1)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveCnfData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				return ERR_FAIL;
			}
			hCc->stCicamData.ucPlayCount = pucSendData[0];
			break;
		default:
			HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveCnfData()---> Error : Unknown SendDataId[%d] !!!\n", ucSendDataId);
			break;
	}
	return ERR_OK;
}

static HERROR CiCc_GetApduFromSacMessage(CI_CC_INSTANCE *hCc, HUINT64 ulApduLen, HUINT8 *pucApduData, HUINT64 *pulApduLen, HUINT8 **pucApdu)
{
	HUINT16			usPayloadLen;
	HUINT8			*pucPayload = NULL;
	HUINT8			*pucDecryptedSacPayload = NULL;
	HUINT8			*pucTempBuf = NULL;
	HUINT8			MAC[16];


	if (pucApduData == NULL || ulApduLen < 9)
	{
		HxLOG_Error("[CICC]---> CiCc_GetApduFromSacMessage()---> Error : Invalid unApduLen[%d] !!!\n", ulApduLen);
		return ERR_FAIL;
	}

	if (hCc->enPhase < CI_CC_PHASE_Host_Capability_Evaluation_Replied) //�̰� ���߿� �ٽ� Ȯ���� �� ��
	{
		HxLOG_Error("[CICC]---> CiCc_GetApduFromSacMessage()---> Error : enPhase %d !!!\n", hCc->enPhase);
		return ERR_FAIL; // ���� ���μ����� ������ �ʾ� ��ȿ�� ����� ���� �� ���� (ķ ���� ���Ķ�� ����).
	}

	if (hCc->enArcErr != CI_ARC_ERR_None)
	{
		HxLOG_Error("[CICC]---> CiCc_GetApduFromSacMessage()---> Error : enArcErr %d !!!\n", hCc->enArcErr);
		return ERR_FAIL; // ���μ��� �󿡼� ���� �߻��Ͽ� ��ȿ�� ����� ���� �� ���� (ķ �� ������� ����).
	}

	if (hCc->ulSacMsgCnt != get32bit(pucApduData))
	{
		HxLOG_Error("[CICC]---> CiCc_GetApduFromSacMessage()---> Error : Invalid unSacMsgCnt[%d]---> %d expected !!!\n", get32bit(pucApduData), hCc->ulSacMsgCnt);
		return ERR_FAIL;
	}

	if (pucApduData[4] != 0x01)
	{
		HxLOG_Error("[CICC]---> CiCc_GetApduFromSacMessage()---> Error : Invalid Header1[0x%x] !!!\n", pucApduData[4]);
		return ERR_FAIL;
	}

	if ((pucApduData[5] >> 5) != 0x0)
	{
		HxLOG_Error("[CICC]---> CiCc_GetApduFromSacMessage()---> Error : Invalid Header2[0x%x] !!!\n", pucApduData[5]);
		return ERR_FAIL;
	}

	usPayloadLen = get16bit(pucApduData + 6);

	if (usPayloadLen & CI_CC_CIPHER_BLOCK_SIZE_1)
	{
		HxLOG_Error("[CICC]---> CiCc_GetApduFromSacMessage()---> Error : Invalid usPayloadLen[%d] !!!\n", usPayloadLen);
		return ERR_FAIL;
	}

	pucPayload = pucApduData + 8;

	// decrypt received sac_message()'s encrypted_payload
	pucDecryptedSacPayload = OxCAS_Malloc(usPayloadLen + 16);
	if(pucDecryptedSacPayload == NULL)
		return ERR_FAIL;

	D_AES_CBC(hCc->stHostData.SEK, pucPayload, pucDecryptedSacPayload, usPayloadLen + 16, s_stCiHostLicenseConst.SIV); // �� ������ SEK ���� ��ȿ�� ���̴�.

	// verify received sac_message()
	pucTempBuf = OxCAS_Malloc(9 + usPayloadLen);
	if(pucTempBuf != NULL)
	{
		pucTempBuf[0] = 4;
		HxSTD_memcpy(pucTempBuf + 1, pucApduData, 8);
		HxSTD_memcpy(pucTempBuf + 9, pucDecryptedSacPayload, usPayloadLen);
		MAC_AES_XCBC(hCc->stHostData.SAK, pucTempBuf, 9 + usPayloadLen, MAC); // �� ������ SAK ���� ��ȿ�� ���̴�.

		if(pucTempBuf != NULL)
			OxCAS_Free(pucTempBuf);

		if (HxSTD_memcmp(MAC, pucDecryptedSacPayload + usPayloadLen, 16) != 0)
		{
			HxLOG_Error("[CICC]---> CiCc_GetApduFromSacMessage()---> Error : Invalid authentication !!!\n");
			if(pucDecryptedSacPayload != NULL)
				OxCAS_Free(pucDecryptedSacPayload);

			return ERR_FAIL;
		}
	}
	else
	{
		HxLOG_Error("[CICC]---> CiCc_GetApduFromSacMessage()---> pucTempBuf is NULL !!!\n");
		if(pucDecryptedSacPayload != NULL)
			OxCAS_Free(pucDecryptedSacPayload);

		return ERR_FAIL;
	}

	*pucApdu = pucDecryptedSacPayload;
	*pulApduLen = usPayloadLen;

	return ERR_OK;
}


static HERROR CiCc_MakeSacMessage (CI_CC_INSTANCE *hCc, CI_CC_SAC_MESSAGE_TYPE eCiCcMessageType, void *message, HUINT32 *punApduLen, HUINT8 **pucApdu)
{
	HUINT16	usOrgPayloadLen, usAlignedPayloadLen;
	HUINT8	*pucPlainPayload = NULL;
	HUINT8	*pucEncryptPayload = NULL;
	HUINT8	*pucPayloadOffset = NULL;
	HUINT8	*pucTempBuf = NULL;
	HUINT8	ucSendDataNbr = 0, ucRequestDataNbr = 0;

	HUINT8	aucSacMsg[8] = {0,};
	HUINT8	aucSendDataId[CI_CC_MAX_DATA_NBR] = {0,}, aucRequestDataId[CI_CC_MAX_DATA_NBR] = {0,};
	HUINT16	ausSendDataLen[CI_CC_MAX_DATA_NBR] = {0,};
	HUINT8	*paucSendData[CI_CC_MAX_DATA_NBR] = {0,};
	int i;

	if (hCc->enPhase < CI_CC_PHASE_Host_Capability_Evaluation_Replied) //�̰� ���߿� �ٽ� Ȯ���� �� ��
	{
		HxLOG_Error("[CICC]---> CiCc_MakeSacMessage()---> Error : enPhase %d !!!\n", hCc->enPhase);
		return ERR_FAIL; // ���� ���μ����� ������ �ʾ� ��ȿ�� ����� ���� �� ���� (ķ ���� ���Ķ�� ����).
	}

	if (hCc->enArcErr != CI_ARC_ERR_None)
	{
		HxLOG_Error("[CICC]---> CiCc_MakeSacMessage()---> Error : enArcErr %d !!!\n", hCc->enArcErr);
		return ERR_FAIL; // ���μ��� �󿡼� ���� �߻��Ͽ� ��ȿ�� ����� ���� �� ���� (ķ �� ������� ����).
	}


	if (eCiCcMessageType == CI_CC_SAC_MESSAGE_RECORD_START_WITH_PIN)
	{
		CI_CC_SAC_MSG_RECORD_START *pstRecordStartMsg;
		pstRecordStartMsg = (CI_CC_SAC_MSG_RECORD_START *)message;

		ucSendDataNbr = 3;										// for send_data_nbr

		aucSendDataId[0] = CI_CC_operating_mode;				// for send_datatype_id
		aucSendDataId[1] = CI_CC_program_number;
		aucSendDataId[2] = CI_CC_PINcode_data;

		ausSendDataLen[0] = 1;									// for send_datatype_length
		ausSendDataLen[1] = 2;
		ausSendDataLen[2] = pstRecordStartMsg->ucPinCodeLength;

		paucSendData[0] = OxCAS_Malloc(ausSendDataLen[0]);		// for send_datatype
		paucSendData[1] = OxCAS_Malloc(ausSendDataLen[1]);
		paucSendData[2] = OxCAS_Malloc(ausSendDataLen[2]);

		paucSendData[0][0] = pstRecordStartMsg->ucOperatingMode;
		paucSendData[1][0] = (unsigned char)((pstRecordStartMsg->usProgramNumber >> 8) & 0xff);
		paucSendData[1][1] = (unsigned char)(pstRecordStartMsg->usProgramNumber & 0xff);
		HxSTD_memcpy(paucSendData[2], pstRecordStartMsg->aucPinCodeData, ausSendDataLen[2]);

		ucRequestDataNbr = 1;									// for request_data_nbr
		aucRequestDataId[0] = CI_CC_record_start_status;		// for request_datatype_id

		/* Data CFN �޾��� �� ��û�� �Ϳ� ���� �Դ��� ���� Ȯ���� ���� protocol ������ ��� */
		hCc->enProtocolType = eCiCcProtocolType_RecordStart;
	}
	else if (eCiCcMessageType == CI_CC_SAC_MESSAGE_RECORD_START_NO_PIN)
	{
		CI_CC_SAC_MSG_RECORD_START *pstRecordStartMsg;
		pstRecordStartMsg = (CI_CC_SAC_MSG_RECORD_START *)message;

		ucSendDataNbr = 2;										// for send_data_nbr

		aucSendDataId[0] = CI_CC_operating_mode;				// for send_datatype_id
		aucSendDataId[1] = CI_CC_program_number;

		ausSendDataLen[0] = 1;									// for send_datatype_length
		ausSendDataLen[1] = 2;

		paucSendData[0] = OxCAS_Malloc(ausSendDataLen[0]);		// for send_datatype
		paucSendData[1] = OxCAS_Malloc(ausSendDataLen[1]);

		paucSendData[0][0] = pstRecordStartMsg->ucOperatingMode;
		paucSendData[1][0] = (unsigned char)((pstRecordStartMsg->usProgramNumber >> 8) & 0xff);
		paucSendData[1][1] = (unsigned char)(pstRecordStartMsg->usProgramNumber & 0xff);

		ucRequestDataNbr = 1;									// for request_data_nbr
		aucRequestDataId[0] = CI_CC_record_start_status;		// for request_datatype_id

		/* Data CFN �޾��� �� ��û�� �Ϳ� ���� �Դ��� ���� Ȯ���� ���� protocol ������ ��� */
		hCc->enProtocolType = eCiCcProtocolType_RecordStart;
	}
	else if (eCiCcMessageType == CI_CC_SAC_MESSAGE_CHANGE_OPERATING_MODE)
	{
		CI_CC_SAC_MSG_CHANGE_OPERATING_MODE	*pstChangeOperatingModeMsg;
		pstChangeOperatingModeMsg = (CI_CC_SAC_MSG_CHANGE_OPERATING_MODE *)message;

		ucSendDataNbr = 2;										// for send_data_nbr

		aucSendDataId[0] = CI_CC_operating_mode;				// for send_datatype_id
		aucSendDataId[1] = CI_CC_program_number;

		ausSendDataLen[0] = 1;									// for send_datatype_length
		ausSendDataLen[1] = 2;

		paucSendData[0] = OxCAS_Malloc(ausSendDataLen[0]);		// for send_datatype
		paucSendData[1] = OxCAS_Malloc(ausSendDataLen[1]);

		paucSendData[0][0] = pstChangeOperatingModeMsg->ucOperatingMode;
		paucSendData[1][0] = (unsigned char)((pstChangeOperatingModeMsg->usProgramNumber >> 8) & 0xff);
		paucSendData[1][1] = (unsigned char)(pstChangeOperatingModeMsg->usProgramNumber & 0xff);

		ucRequestDataNbr = 1;									// for request_data_nbr
		aucRequestDataId[0] = CI_CC_mode_change_status;

		/* Data CFN �޾��� �� ��û�� �Ϳ� ���� �Դ��� ���� Ȯ���� ���� protocol ������ ��� */
		hCc->enProtocolType = eCiCcProtocolType_ChangeOperatingMode;
	}
	else if (eCiCcMessageType == CI_CC_SAC_MESSAGE_RECORD_STOP)
	{
		CI_CC_SAC_MSG_RECORD_STOP *pstRecordStopMsg;
		pstRecordStopMsg = (CI_CC_SAC_MSG_RECORD_STOP *)message;

		ucSendDataNbr = 1;										// for send_data_nbr

		aucSendDataId[0] = CI_CC_program_number;				// for send_datatype_id
		ausSendDataLen[0] = 2;									// for send_datatype_length
		paucSendData[0] = OxCAS_Malloc(ausSendDataLen[0]);		// for send_datatype

		paucSendData[0][0] = (unsigned char)((pstRecordStopMsg->usProgramNumber >> 8) & 0xff);
		paucSendData[0][1] = (unsigned char)(pstRecordStopMsg->usProgramNumber & 0xff);

		ucRequestDataNbr = 1;									// for request_data_nbr
		aucRequestDataId[0] = CI_CC_record_stop_status;

		/* Data CFN �޾��� �� ��û�� �Ϳ� ���� �Դ��� ���� Ȯ���� ���� protocol ������ ��� */
		hCc->enProtocolType = eCiCcProtocolType_RecordStop;
	}
	else if (eCiCcMessageType == CI_CC_SAC_MESSAGE_PLAYBACK_LICENSE_EXCHANGE)
	{
		CI_CC_SAC_MSG_PLAYBACK_LICENSE *pstPlaybackLicenseMsg;
		pstPlaybackLicenseMsg = (CI_CC_SAC_MSG_PLAYBACK_LICENSE *)message;

		ucSendDataNbr = 2;

		aucSendDataId[0] = CI_CC_program_number;				// for send_datatype_id
		aucSendDataId[1] = CI_CC_Host_license;

		ausSendDataLen[0] = 2;									// for send_datatype_length
		ausSendDataLen[1] = pstPlaybackLicenseMsg->usLicenseDataLen;

		paucSendData[0] = OxCAS_Malloc(ausSendDataLen[0]);		// for send_datatype
		paucSendData[1] = OxCAS_Malloc(ausSendDataLen[1]);

		paucSendData[0][0] = (unsigned char)((pstPlaybackLicenseMsg->usProgramNumber >> 8) & 0xff);
		paucSendData[0][1] = (unsigned char)(pstPlaybackLicenseMsg->usProgramNumber & 0xff);
		HxSTD_memcpy(paucSendData[1], pstPlaybackLicenseMsg->pucHostLicense, ausSendDataLen[1]);

		ucRequestDataNbr = 4;									// for request_data_nbr
		aucRequestDataId[0] = CI_CC_program_number;
		aucRequestDataId[1] = CI_CC_license_status;
		aucRequestDataId[2] = CI_CC_uri_message;
		aucRequestDataId[3] = CI_CC_cicam_license;

		/* Data CFN �޾��� �� ��û�� �Ϳ� ���� �Դ��� ���� Ȯ���� ���� protocol ������ ��� */
		hCc->enProtocolType = eCiCcProtocolType_PlaybackLicenseExchange;
	}
	else if(CI_CC_SAC_MESSAGE_LICENSE_CHECK_EXCHANGE)
	{
		CI_CC_SAC_MSG_LICENSE_CHECK *pstLicenseCheckMsg;
		pstLicenseCheckMsg = (CI_CC_SAC_MSG_LICENSE_CHECK *)message;

		ucSendDataNbr = 1;

		aucSendDataId[0] = CI_CC_Host_license;						// for send_datatype_id
		ausSendDataLen[0] = pstLicenseCheckMsg->usLicenseDataLen;	// for send_datatype_length
		paucSendData[0] = OxCAS_Malloc(ausSendDataLen[0]);			// for send_datatype
		HxSTD_memcpy(paucSendData[0], pstLicenseCheckMsg->pucHostLicense, ausSendDataLen[0]);

		ucRequestDataNbr = 2;									// for request_data_nbr
		aucRequestDataId[0] = CI_CC_license_status;
		aucRequestDataId[1] = CI_CC_play_count;

		/* Data CFN �޾��� �� ��û�� �Ϳ� ���� �Դ��� ���� Ȯ���� ���� protocol ������ ��� */
		hCc->enProtocolType = eCiCcProtocolType_LicenseCheckExchange;
	}
	else
	{
		HxLOG_Error("unknown cc Message Type : %d.\n", eCiCcMessageType);
		return ERR_FAIL;
	}

	// TODO: �Ʒ� ��Ȳ�� ���� �̽� �߻� ���ɼ� ����. ������ test �غ��� ���� �߻��� �ڵ� ���� (kkkim)
	// ��Ȳ: SAC message Counter�� ���� ������, async�ϰ� CiCc_ProcDataCnf �Լ����� ������ �޴µ�,
	// 		 CiCc_GetApduFromSacMessage�Լ����� ���� hCc�� ����� ulSacMsgCnt�� �ٸ��� error�� �����Ѵ�.
	// 		 ���� Host�ʿ��� SAC SAC message�� ����� �Ʒ�ó�� count ��Ƽ� ������,
	//		 CAM���κ��� ���� �ޱ� ���� �� �ٸ� SAC message�� ����� �����鼭 Count�� �÷��� ������ ������ �޾����� �翬�� Count�� ���� �ʾ� Erroró�� �� ���̴�.
	//		 �� ���� Ȯ���ϰ� �����Ұ�!
	if(hCc->ulSacMsgCnt == CI_CC_SAC_MSG_CNT_MAX)
	{
		hCc->ulSacMsgCnt = 1;		// The first message shall use number 0x1 and wraps back to 0x1 (not zero).
	}
	else
	{
		hCc->ulSacMsgCnt++;
	}

	// calculate original & aligned length
	usOrgPayloadLen = 3;							// for cc_system_id_bitmask, send_datatype_nbr, request_datatype_nbr
	usOrgPayloadLen += ucRequestDataNbr;			// for request_datatype_id
	for(i = 0 ; i < ucSendDataNbr ; i++)				// for send_datatype_id, send_datatype_length, send_datatype
		usOrgPayloadLen += (3 + ausSendDataLen[i]);

	usAlignedPayloadLen = GetAlignedLen(usOrgPayloadLen);

	// set sending sac_message()'s message_counter and header
	put32bit(aucSacMsg, hCc->ulSacMsgCnt);
	aucSacMsg[4] = 0x01;
	aucSacMsg[5] = 0x00;
	put16bit(aucSacMsg + 6, usAlignedPayloadLen);

	pucPlainPayload = OxCAS_Malloc(usAlignedPayloadLen + 16);
	if (pucPlainPayload == NULL)
		return ERR_FAIL;
	pucEncryptPayload = OxCAS_Malloc(usAlignedPayloadLen + 16);
	if (pucEncryptPayload == NULL)
	{
		OxCAS_Free(pucPlainPayload);
		return ERR_FAIL;
	}

	pucPayloadOffset = pucPlainPayload;

	pucPayloadOffset[0] = CI_CC_SYSTEM_ID_BITMASK_V1;
	pucPayloadOffset[1] = ucSendDataNbr;

	pucPayloadOffset += 2;

	for(i = 0 ; i < ucSendDataNbr ; i++)
	{
		pucPayloadOffset[0] = aucSendDataId[i];
		put16bit(pucPayloadOffset + 1, ausSendDataLen[i]);
		HxSTD_memcpy(pucPayloadOffset + 3, paucSendData[i], ausSendDataLen[i]);

		pucPayloadOffset += (3 + ausSendDataLen[i]);
	}

	pucPayloadOffset[0] = ucRequestDataNbr;
	pucPayloadOffset++;

	for(i = 0 ; i < ucRequestDataNbr ; i++)
		pucPayloadOffset[i] = aucRequestDataId[i];

	pucPayloadOffset += ucRequestDataNbr;

	OneAndZeroesPadding(pucPlainPayload, usOrgPayloadLen);

	// authentication
	pucTempBuf = (HUINT8 *)OxCAS_Malloc(9 + usAlignedPayloadLen);
	if(pucTempBuf == NULL)
	{
		OxCAS_Free(pucPlainPayload);
		OxCAS_Free(pucEncryptPayload);
		return ERR_FAIL;
	}

	pucTempBuf[0] = 4;													// header length
	HxSTD_memcpy(pucTempBuf + 1, aucSacMsg, 8);							// header
	HxSTD_memcpy(pucTempBuf + 9, pucPlainPayload, usAlignedPayloadLen);		// payload
	MAC_AES_XCBC(hCc->stHostData.SAK, pucTempBuf, 9 + usAlignedPayloadLen, pucPlainPayload + usAlignedPayloadLen); // �� ������ SAK ���� ��ȿ�� ���̴�.

	OxCAS_Free(pucTempBuf);

	E_AES_CBC(hCc->stHostData.SEK, pucPlainPayload, pucEncryptPayload, usAlignedPayloadLen + 16, s_stCiHostLicenseConst.SIV); // �� ������ SEK ���� ��ȿ�� ���̴�.

	pucTempBuf = OxCAS_Malloc(8 + usAlignedPayloadLen + 16);
	if(pucTempBuf == NULL)
	{
		OxCAS_Free(pucPlainPayload);
		OxCAS_Free(pucEncryptPayload);
		return ERR_FAIL;
	}

	HxSTD_memcpy(pucTempBuf, aucSacMsg, 8);
	HxSTD_memcpy(pucTempBuf + 8, pucEncryptPayload, usAlignedPayloadLen + 16);

	*punApduLen = 8 + usAlignedPayloadLen + 16;
	*pucApdu = pucTempBuf;

	if(pucPlainPayload != NULL)
		OxCAS_Free(pucPlainPayload);

	if(pucEncryptPayload != NULL)
		OxCAS_Free(pucEncryptPayload);

	for(i = 0 ; i < CI_CC_MAX_DATA_NBR ; i++)
	{
		if(paucSendData[i])
			OxCAS_Free(paucSendData[i]);
	}

	return ERR_OK;
}
#endif

#else
static SvcCas_CiArcErr_e CiCc_OpenConfirm(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char ucMsg[5];

	ucMsg[0] = (unsigned char)((CI_CC_OPEN_CNF >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_CC_OPEN_CNF >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_CC_OPEN_CNF & 0xff);
	ucMsg[3] = 0x01;
	ucMsg[4] = CI_CC_SYSTEM_ID_BITMASK_V1;

	if (CIRH_SendApdu(hCc->usSsNo, ucMsg, 5) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_OpenConfirm()---> Error : fail to send CI_CC_OPEN_CNF for SsNo[%d] !!!\n", hCc->usSsNo);
		return CI_ARC_ERR_Send_Apdu;
	}

	hCc->enPhase = CI_CC_PHASE_Host_Capability_Evaluation_Replied;

	HxLOG_Print("[CICC]---> CiCc_OpenConfirm()---> SsNo[%d]...\n", hCc->usSsNo);

	return CI_ARC_ERR_NONE;
}

static SvcCas_CiArcErr_e CiCc_DataConfirm(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen, unsigned char ucSacFlag)
{
	CI_CC_SAC_MSG	stSacMsg;							// for host received sac_message()
	unsigned char	* pucDecryptedSacPayload = NULL;	// for host received sac_message()'s payload
	unsigned short	usOriginalSacPayloadLen;			// for host sending sac_message()'s payload
	unsigned short	usAlignedSacPayloadLen;			//
	unsigned char	* pucOriginalSacPayload = NULL;	//
	unsigned char	* pucEncryptedSacPayload = NULL;	//
	unsigned char	ucSacBuf[8];

	unsigned char	* pucData = NULL;					// just pointer to data (don't mem alloc for this)
	unsigned short	usDataLen;

	unsigned char	uc_cc_system_id_bitmask;

	unsigned char	uc_send_data_nbr = 0;
	unsigned char	uc_send_data_id[CI_CC_MAX_DATA_NBR];
	unsigned short	us_send_data_length[CI_CC_MAX_DATA_NBR];
	unsigned char	* puc_send_data[CI_CC_MAX_DATA_NBR] = {NULL,};

	unsigned char	uc_request_data_nbr = 0;
	unsigned char	uc_request_data_id[CI_CC_MAX_DATA_NBR];
	unsigned short	us_request_data_length[CI_CC_MAX_DATA_NBR];
	unsigned char	* puc_request_data[CI_CC_MAX_DATA_NBR] = {NULL,};

	int				pos, i;

	unsigned char	* pucMsg = NULL;
	unsigned long	ulMsgPayloadLen;
	unsigned char	ucLenFieldSize;

	SvcCas_CiArcErr_e		enArcErr = CI_ARC_ERR_NONE;

	/* ------------------------------------------------ */
	/* --- parse cc_data_req() or cc_sac_data_req() --- */
	/* ------------------------------------------------ */

	if (ucSacFlag) // parse received sac_message(), decrypt its encrypted_payload and verify sac_message() in case of cc_sac_data_req()
	{
		if (pucApduData == NULL || ulApduLen < 9)
		{
			HxLOG_Error("[CICC]---> CiCc_DataConfirm()---> Error : Invalid ulApduLen[%ld] !!!\n", ulApduLen);
			enArcErr = CI_ARC_ERR_Apdu_Len1;
			goto EXIT;
		}

		stSacMsg.ulMsgCnt = ((unsigned long)pucApduData[0] << 24) & 0xff000000;
		stSacMsg.ulMsgCnt |= (((unsigned long)pucApduData[1] << 16) & 0x00ff0000);
		stSacMsg.ulMsgCnt |= (((unsigned long)pucApduData[2] << 8) & 0x0000ff00);
		stSacMsg.ulMsgCnt |= ((unsigned long)pucApduData[3] & 0x000000ff);

		stSacMsg.ucHdr1 = pucApduData[4];
		stSacMsg.ucHdr2 = pucApduData[5];

		stSacMsg.usPayloadLen = ((unsigned short)pucApduData[6] << 8) & 0xff00;
		stSacMsg.usPayloadLen |= ((unsigned short)pucApduData[7] & 0x00ff);

		stSacMsg.pucPayloadData = &pucApduData[8];

		HxLOG_Print("[CICC]---> CiCc_DataConfirm()---> MsgCnt = %ld, Hdr = %02X %02X, PayloadLen = %d\n", stSacMsg.ulMsgCnt, stSacMsg.ucHdr1, stSacMsg.ucHdr2, stSacMsg.usPayloadLen);

		// ignore received sac_message() if...
		#if 0 // org
		if (stSacMsg.ulMsgCnt == hCc->ulSacMsgCnt + 1) // The first message shall use number 0x1 and wraps back to 0x1 (not zero).
		{
			hCc->ulSacMsgCnt = (stSacMsg.ulMsgCnt == CI_CC_SAC_MSG_CNT_MAX) ? 0 : stSacMsg.ulMsgCnt;
		}
		else
		{
			HxLOG_Error("[CICC]---> CiCc_DataConfirm()---> Error : Invalid ulMsgCnt[%ld]---> %ld expected !!!\n", stSacMsg.ulMsgCnt, hCc->ulSacMsgCnt+1);
			enArcErr = CI_ARC_ERR_SAC_MSG_Counter;
			goto EXIT;
		}
		#else // SMiT �� �޸� Neotion �� ��� SacMsgCnt �� �������� �����Ƿ� �̿� ���� W/A ������.
		if (s_bCiCcSacKeyRefreshedUnusually)
		{
			hCc->ulSacMsgCnt = (stSacMsg.ulMsgCnt == CI_CC_SAC_MSG_CNT_MAX) ? 0 : stSacMsg.ulMsgCnt;
			s_bCiCcSacKeyRefreshedUnusually = 0;
		}
		else
		{
			if (stSacMsg.ulMsgCnt == hCc->ulSacMsgCnt + 1) // The first message shall use number 0x1 and wraps back to 0x1 (not zero).
			{
				hCc->ulSacMsgCnt = (stSacMsg.ulMsgCnt == CI_CC_SAC_MSG_CNT_MAX) ? 0 : stSacMsg.ulMsgCnt;
			}
			else
			{
				HxLOG_Error("[CICC]---> CiCc_DataConfirm()---> Error : Invalid ulMsgCnt[%ld]---> %ld expected !!!\n", stSacMsg.ulMsgCnt, hCc->ulSacMsgCnt+1);
				enArcErr = CI_ARC_ERR_SAC_MSG_Counter;
				goto EXIT;
			}
		}
		#endif
		if (stSacMsg.ucHdr1 != 0x01)
		{
			HxLOG_Error("[CICC]---> CiCc_DataConfirm()---> Error : Invalid ucHdr1[0x%x] !!!\n", stSacMsg.ucHdr1);
			enArcErr = CI_ARC_ERR_SAC_MSG_Header1;
			goto EXIT;
		}
		if ((stSacMsg.ucHdr2 >> 5) != 0x0)
		{
			HxLOG_Error("[CICC]---> CiCc_DataConfirm()---> Error : Invalid ucHdr2[0x%x] !!!\n", stSacMsg.ucHdr2);
			enArcErr = CI_ARC_ERR_SAC_MSG_Header2;
			goto EXIT;
		}
		if (stSacMsg.usPayloadLen & CI_CC_CIPHER_BLOCK_SIZE_1)
		{
			HxLOG_Error("[CICC]---> CiCc_DataConfirm()---> Error : Invalid usPayloadLen[%d] !!!\n", stSacMsg.usPayloadLen);
			enArcErr = CI_ARC_ERR_SAC_MSG_PayloadLen;
			goto EXIT;
		}

		// decrypt received sac_message()'s encrypted_payload
		pucDecryptedSacPayload = (unsigned char *)OxCAS_Malloc(stSacMsg.usPayloadLen + 16);
		D_AES_CBC(hCc->stHostData.SEK, stSacMsg.pucPayloadData, pucDecryptedSacPayload, stSacMsg.usPayloadLen + 16, s_stCiHostLicenseConst.SIV); // �� ������ SEK ���� ��ȿ�� ���̴�.

		#if CI_CC_VerifyReceivedSacMessage // verify received sac_message()
		HxSTD_memcpy(stSacMsg.ucAuth, &pucDecryptedSacPayload[stSacMsg.usPayloadLen], 16);
		{
			unsigned char *pucTempBuf = NULL;
			unsigned char MAC[16];
			pucTempBuf = (unsigned char *)OxCAS_Malloc(1+8+stSacMsg.usPayloadLen);
			pucTempBuf[0] = 4;
			HxSTD_memcpy(&pucTempBuf[1], pucApduData, 8);
			HxSTD_memcpy(&pucTempBuf[1+8], pucDecryptedSacPayload, stSacMsg.usPayloadLen);
			MAC_AES_XCBC(hCc->stHostData.SAK, pucTempBuf, 1+8+stSacMsg.usPayloadLen, MAC); // �� ������ SAK ���� ��ȿ�� ���̴�.
			OxCAS_Free(pucTempBuf);
			if (HxSTD_memcmp(MAC, stSacMsg.ucAuth, 16) != 0)
			{
				HxLOG_Error("[CICC]---> CiCc_DataConfirm()---> Error : Invalid authentication !!!\n");
				enArcErr = CI_ARC_ERR_SAC_MSG_Authentication;
				goto EXIT;
			}
		}
		#endif

		pucData = pucDecryptedSacPayload;
		usDataLen = stSacMsg.usPayloadLen;
	}
	else
	{
		pucData = pucApduData;
		usDataLen = (unsigned short)ulApduLen;
	}

	if (pucData == NULL || usDataLen < 3)
	{
		HxLOG_Error("[CICC]---> CiCc_DataConfirm()---> Error : Invalid usDataLen[%d] !!!\n", usDataLen);
		enArcErr = CI_ARC_ERR_Apdu_Len2;
		goto EXIT;
	}

	// get cc_system_id_bitmask
	uc_cc_system_id_bitmask = pucData[0]; /* check with CI_CC_SYSTEM_ID_BITMASK_V1 ? */

	// get send_data_nbr
	uc_send_data_nbr = pucData[1];
	if (uc_send_data_nbr > CI_CC_MAX_DATA_NBR)
	{
		HxLOG_Error("[CICC]---> CiCc_DataConfirm()---> Error : CI_CC_MAX_DATA_NBR(%d) is too small for send %d !!!\n", CI_CC_MAX_DATA_NBR, uc_send_data_nbr);
		uc_send_data_nbr = 0;
		enArcErr = CI_ARC_ERR_Send_data_nbr;
		goto EXIT;
	}

	// get send_data
	pos = 2;
	for (i = 0; i < uc_send_data_nbr; i++)
	{
		uc_send_data_id[i] = pucData[pos];
		us_send_data_length[i] = ((unsigned short)pucData[pos+1] << 8) & 0xff00;
		us_send_data_length[i] |= ((unsigned short)pucData[pos+2] & 0x00ff);
		if (us_send_data_length[i])
		{
			puc_send_data[i] = (unsigned char *)OxCAS_Malloc(us_send_data_length[i]);
			HxSTD_memcpy(puc_send_data[i], &pucData[pos+3], us_send_data_length[i]);
		}
		pos += (3 + us_send_data_length[i]);
	}

	// get request_data_nbr
	uc_request_data_nbr = pucData[pos++];
	if (uc_request_data_nbr > CI_CC_MAX_DATA_NBR)
	{
		HxLOG_Error("[CICC]---> CiCc_DataConfirm()---> Error : CI_CC_MAX_DATA_NBR(%d) is too small for request %d !!!\n", CI_CC_MAX_DATA_NBR, uc_request_data_nbr);
		uc_request_data_nbr = 0;
		enArcErr = CI_ARC_ERR_Request_data_nbr;
		goto EXIT;
	}

	// get request_data_id
	for (i = 0; i < uc_request_data_nbr; i++)
	{
		uc_request_data_id[i] = pucData[pos++];
	}

	// checking : �����͵� ������ B�� �����ϱ� ���ؼ��� A�� �ʿ��ϴٴ� ���� ���� ���谡 �����Ѵ�. ���� ���忡 ��õ� ������ �������� ������ ������ �߻��Ѵ�.
	#if 0 // #ifdef _CI_DEBUG_CC_DATA_
	HxLOG_Print("\n - CiCc_DataConfirm() - \n");
	HxLOG_Print("\t cc_system_id_bitmask = 0x%x \n", uc_cc_system_id_bitmask);
	HxLOG_Print("\t send_data_nbr = %d \n", uc_send_data_nbr);
	for (i = 0; i < uc_send_data_nbr; i++)
		HxLOG_Print("\t\t %d : id = %d (%d bits) \n", i, uc_send_data_id[i], us_send_data_length[i] * 8);
	HxLOG_Print("\t request_data_nbr = %d \n", uc_request_data_nbr);
	for (i = 0; i < uc_request_data_nbr; i++)
		HxLOG_Print("\t\t %d : id = %d \n", i, uc_request_data_id[i]);
	HxLOG_Print("\n");
	#endif

	/* -------------------------------------------- */
	/* --- save send_data and make request_data --- */
	/* -------------------------------------------- */

	// verify and save send_data
	for (i = 0; i < uc_send_data_nbr; i++)
	{
		CiCc_VerifyAndSaveSendData(hCc, uc_send_data_id[i], us_send_data_length[i], puc_send_data[i]);
	}

	// make request_data
	for (i = 0; i < uc_request_data_nbr; i++)
	{
		CiCc_MakeReqDataAndDoSomething(hCc, uc_request_data_id[i], &us_request_data_length[i], &puc_request_data[i]);
	}

	/* ----------------------------------------------------------- */
	/* --- make cc_data_cnf() or cc_sac_data_cnf() and send it --- */
	/* ----------------------------------------------------------- */

	if (ucSacFlag)
	{
		/*** make sending sac_message()'s encrypted_payload in case of cc_sac_data_cnf() ***/

		// calculate original & aligned length
		usOriginalSacPayloadLen = 2;
		for (i = 0; i < uc_request_data_nbr; i++)
			usOriginalSacPayloadLen += (3 + us_request_data_length[i]);
		usAlignedSacPayloadLen = GetAlignedLen(usOriginalSacPayloadLen);

		// set sending sac_message()'s message_counter and header
		ucSacBuf[0] = (unsigned char)((stSacMsg.ulMsgCnt >> 24) & 0xff);
		ucSacBuf[1] = (unsigned char)((stSacMsg.ulMsgCnt >> 16) & 0xff);
		ucSacBuf[2] = (unsigned char)((stSacMsg.ulMsgCnt >> 8) & 0xff);
		ucSacBuf[3] = (unsigned char)(stSacMsg.ulMsgCnt & 0xff);
		ucSacBuf[4] = stSacMsg.ucHdr1;
		ucSacBuf[5] = stSacMsg.ucHdr2;
		ucSacBuf[6] = (unsigned char)((usAlignedSacPayloadLen >> 8) & 0xff);
		ucSacBuf[7] = (unsigned char)(usAlignedSacPayloadLen & 0xff);

		// allocate aligned buffers and set data
		pucOriginalSacPayload = (unsigned char *)OxCAS_Malloc(usAlignedSacPayloadLen + 16);
		pucEncryptedSacPayload = (unsigned char *)OxCAS_Malloc(usAlignedSacPayloadLen + 16);
		pucOriginalSacPayload[0] = uc_cc_system_id_bitmask;
		pucOriginalSacPayload[1] = uc_request_data_nbr;
		pos = 2;
		for (i = 0; i < uc_request_data_nbr; i++)
		{
			pucOriginalSacPayload[pos] = uc_request_data_id[i];
			pucOriginalSacPayload[pos+1] = (unsigned char)((us_request_data_length[i] >> 8) & 0xff);
			pucOriginalSacPayload[pos+2] = (unsigned char)(us_request_data_length[i] & 0xff);
			HxSTD_memcpy(&pucOriginalSacPayload[pos+3], puc_request_data[i], us_request_data_length[i]);
			pos += (3 + us_request_data_length[i]);
		}

		// padding
		OneAndZeroesPadding(pucOriginalSacPayload, usOriginalSacPayloadLen);

		// authentication
		{
			unsigned char *pucTempBuf = NULL;
			unsigned char MAC[16];
			pucTempBuf = (unsigned char *)OxCAS_Malloc(1+8+usAlignedSacPayloadLen);
			pucTempBuf[0] = 4;
			HxSTD_memcpy(&pucTempBuf[1], ucSacBuf, 8);
			HxSTD_memcpy(&pucTempBuf[1+8], pucOriginalSacPayload, usAlignedSacPayloadLen);
			MAC_AES_XCBC(hCc->stHostData.SAK, pucTempBuf, 1+8+usAlignedSacPayloadLen, MAC); // �� ������ SAK ���� ��ȿ�� ���̴�.
			OxCAS_Free(pucTempBuf);
			HxSTD_memcpy(&pucOriginalSacPayload[usAlignedSacPayloadLen], MAC, 16);
		}

		// encrypt
		E_AES_CBC(hCc->stHostData.SEK, pucOriginalSacPayload, pucEncryptedSacPayload, usAlignedSacPayloadLen + 16, s_stCiHostLicenseConst.SIV); // �� ������ SEK ���� ��ȿ�� ���̴�.

		/*** make message to send ***/

		// calculate message payload length
		ulMsgPayloadLen = 8 + usAlignedSacPayloadLen + 16;

		// allocate message buffer
		pucMsg = (unsigned char *)OxCAS_Malloc(/*6*/ 8 + ulMsgPayloadLen); /* tag field (3 bytes) + max length field (3->5 bytes) + payload length */

		// set message tag
		pucMsg[0] = (unsigned char)((CI_CC_SAC_DATA_CNF >> 16) & 0xff);
		pucMsg[1] = (unsigned char)((CI_CC_SAC_DATA_CNF >> 8) & 0xff);
		pucMsg[2] = (unsigned char)(CI_CC_SAC_DATA_CNF & 0xff);

		// set message length
		CIRH_ConvertApduLengthField(ulMsgPayloadLen, &pucMsg[3], &ucLenFieldSize);

		// set message payload
		HxSTD_memcpy(&pucMsg[3+ucLenFieldSize], ucSacBuf, 8);
		HxSTD_memcpy(&pucMsg[11+ucLenFieldSize], pucEncryptedSacPayload, usAlignedSacPayloadLen + 16);
	}
	else
	{
		/*** make message to send ***/

		// calculate message payload length
		ulMsgPayloadLen = 2;
		for (i = 0; i < uc_request_data_nbr; i++)
			ulMsgPayloadLen += (3 + us_request_data_length[i]);

		// allocate message buffer
		pucMsg = (unsigned char *)OxCAS_Malloc(/*6*/ 8 + ulMsgPayloadLen); /* tag field (3 bytes) + max length field (3->5 bytes) + payload length */

		// set message tag
		pucMsg[0] = (unsigned char)((CI_CC_DATA_CNF >> 16) & 0xff);
		pucMsg[1] = (unsigned char)((CI_CC_DATA_CNF >> 8) & 0xff);
		pucMsg[2] = (unsigned char)(CI_CC_DATA_CNF & 0xff);

		// set message length
		CIRH_ConvertApduLengthField(ulMsgPayloadLen, &pucMsg[3], &ucLenFieldSize);

		// set message payload
		pucMsg[3+ucLenFieldSize] = uc_cc_system_id_bitmask;
		pucMsg[4+ucLenFieldSize] = uc_request_data_nbr;
		pos = 5 + ucLenFieldSize;
		for (i = 0; i < uc_request_data_nbr; i++)
		{
			pucMsg[pos] = uc_request_data_id[i];
			pucMsg[pos+1] = (unsigned char)((us_request_data_length[i] >> 8) & 0xff);
			pucMsg[pos+2] = (unsigned char)(us_request_data_length[i] & 0xff);
			HxSTD_memcpy(&pucMsg[pos+3], puc_request_data[i], us_request_data_length[i]);
			pos += (3 + us_request_data_length[i]);
		}
	}

	if (CIRH_SendApdu(hCc->usSsNo, pucMsg, 3 + ucLenFieldSize + ulMsgPayloadLen) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_DataConfirm()---> Error : fail to send APDU for SsNo[%d] !!!\n", hCc->usSsNo);
		enArcErr = CI_ARC_ERR_Send_Apdu;
		goto EXIT;
	}

	HxLOG_Print("[CICC]---> CiCc_DataConfirm()---> %ld bytes payload sent\n", ulMsgPayloadLen);
	#if 0 // #ifdef _CI_DEBUG_CC_DATA_
	{
		for (i = 0; i < 3 + ucLenFieldSize; i++)
		{
			if ((i & 15) == 0)
				HxLOG_Print("\n");
			HxLOG_Print(" %02x", pucMsg[i]);
		}
		for (i = 0; i < ulMsgPayloadLen; i++)
		{
			if ((i & 15) == 0)
				HxLOG_Print("\n");
			HxLOG_Print(" %02x", pucMsg[3 + ucLenFieldSize + i]);
		}
		HxLOG_Print("\n\n");
	}
	#endif

EXIT :

	if (ucSacFlag)
	{
		if (pucDecryptedSacPayload)
			OxCAS_Free(pucDecryptedSacPayload);
		if (pucOriginalSacPayload)
			OxCAS_Free(pucOriginalSacPayload);
		if (pucEncryptedSacPayload)
			OxCAS_Free(pucEncryptedSacPayload);
	}

	for (i = 0; i < uc_send_data_nbr; i++)
	{
		if (puc_send_data[i])
			OxCAS_Free(puc_send_data[i]);
	}

	for (i = 0; i < uc_request_data_nbr; i++)
	{
		if (puc_request_data[i])
			OxCAS_Free(puc_request_data[i]);
	}

	if (pucMsg)
		OxCAS_Free(pucMsg);

	HxLOG_Print("[CICC]---> CiCc_DataConfirm()---> SsNo[%d]...\n", hCc->usSsNo);

	return enArcErr;
}

static SvcCas_CiArcErr_e CiCc_SyncConfirm(CI_CC_INSTANCE *hCc, unsigned char *pucApduData, unsigned long ulApduLen, unsigned char ucSacFlag)
{
	CI_CC_SAC_MSG	stSacMsg;							// for host received sac_message()
	unsigned char	* pucDecryptedSacPayload = NULL;	// for host received sac_message()'s payload
	unsigned short	usOriginalSacPayloadLen;			// for host sending sac_message()'s payload
	unsigned short	usAlignedSacPayloadLen;			//
	unsigned char	* pucOriginalSacPayload = NULL;	//
	unsigned char	* pucEncryptedSacPayload = NULL;	//
	unsigned char	ucSacBuf[8];

	unsigned char	* pucMsg = NULL;
	unsigned long	ulMsgPayloadLen;
	unsigned char	ucLenFieldSize;

	SvcCas_CiArcErr_e		enArcErr = CI_ARC_ERR_NONE;

	/* ------------------------------------------------ */
	/* --- parse cc_sync_req() or cc_sac_sync_req() --- */
	/* ------------------------------------------------ */

	if (ucSacFlag) // parse received sac_message(), decrypt its encrypted_payload and verify sac_message() in case of cc_sac_sync_req()
	{
		if (pucApduData == NULL || ulApduLen < 9)
		{
			HxLOG_Error("[CICC]---> CiCc_SyncConfirm()---> Error : Invalid ulApduLen[%ld] !!!\n", ulApduLen);
			enArcErr = CI_ARC_ERR_Apdu_Len1;
			goto EXIT;
		}

		stSacMsg.ulMsgCnt = ((unsigned long)pucApduData[0] << 24) & 0xff000000;
		stSacMsg.ulMsgCnt |= (((unsigned long)pucApduData[1] << 16) & 0x00ff0000);
		stSacMsg.ulMsgCnt |= (((unsigned long)pucApduData[2] << 8) & 0x0000ff00);
		stSacMsg.ulMsgCnt |= ((unsigned long)pucApduData[3] & 0x000000ff);

		stSacMsg.ucHdr1 = pucApduData[4];
		stSacMsg.ucHdr2 = pucApduData[5];

		stSacMsg.usPayloadLen = ((unsigned short)pucApduData[6] << 8) & 0xff00;
		stSacMsg.usPayloadLen |= ((unsigned short)pucApduData[7] & 0x00ff);

		stSacMsg.pucPayloadData = &pucApduData[8];

		HxLOG_Print("[CICC]---> CiCc_SyncConfirm()---> MsgCnt = %ld, Hdr = %02X %02X, PayloadLen = %d\n", stSacMsg.ulMsgCnt, stSacMsg.ucHdr1, stSacMsg.ucHdr2, stSacMsg.usPayloadLen);

		// ignore received sac_message() if...
		#if 0 // org
		if (stSacMsg.ulMsgCnt == hCc->ulSacMsgCnt + 1) // The first message shall use number 0x1 and wraps back to 0x1 (not zero).
		{
			hCc->ulSacMsgCnt = (stSacMsg.ulMsgCnt == CI_CC_SAC_MSG_CNT_MAX) ? 0 : stSacMsg.ulMsgCnt;
		}
		else
		{
			HxLOG_Error("[CICC]---> CiCc_SyncConfirm()---> Error : Invalid ulMsgCnt[%ld]---> %ld expected !!!\n", stSacMsg.ulMsgCnt, hCc->ulSacMsgCnt+1);
			enArcErr = CI_ARC_ERR_SAC_MSG_Counter;
			goto EXIT;
		}
		#else // SMiT �� �޸� Neotion �� ��� SacMsgCnt �� �������� �����Ƿ� �̿� ���� W/A ������.
		if (s_bCiCcSacKeyRefreshedUnusually)
		{
			hCc->ulSacMsgCnt = (stSacMsg.ulMsgCnt == CI_CC_SAC_MSG_CNT_MAX) ? 0 : stSacMsg.ulMsgCnt;
			s_bCiCcSacKeyRefreshedUnusually = 0;
		}
		else
		{
			if (stSacMsg.ulMsgCnt == hCc->ulSacMsgCnt + 1) // The first message shall use number 0x1 and wraps back to 0x1 (not zero).
			{
				hCc->ulSacMsgCnt = (stSacMsg.ulMsgCnt == CI_CC_SAC_MSG_CNT_MAX) ? 0 : stSacMsg.ulMsgCnt;
			}
			else
			{
				HxLOG_Error("[CICC]---> CiCc_SyncConfirm()---> Error : Invalid ulMsgCnt[%ld]---> %ld expected !!!\n", stSacMsg.ulMsgCnt, hCc->ulSacMsgCnt+1);
				enArcErr = CI_ARC_ERR_SAC_MSG_Counter;
				goto EXIT;
			}
		}
		#endif
		if (stSacMsg.ucHdr1 != 0x01)
		{
			HxLOG_Error("[CICC]---> CiCc_SyncConfirm()---> Error : Invalid ucHdr1[0x%x] !!!\n", stSacMsg.ucHdr1);
			enArcErr = CI_ARC_ERR_SAC_MSG_Header1;
			goto EXIT;
		}
		if ((stSacMsg.ucHdr2 >> 5) != 0x0)
		{
			HxLOG_Error("[CICC]---> CiCc_SyncConfirm()---> Error : Invalid ucHdr2[0x%x] !!!\n", stSacMsg.ucHdr2);
			enArcErr = CI_ARC_ERR_SAC_MSG_Header2;
			goto EXIT;
		}
		if (stSacMsg.usPayloadLen & CI_CC_CIPHER_BLOCK_SIZE_1)
		{
			HxLOG_Error("[CICC]---> CiCc_SyncConfirm()---> Error : Invalid usPayloadLen[%d] !!!\n", stSacMsg.usPayloadLen);
			enArcErr = CI_ARC_ERR_SAC_MSG_PayloadLen;
			goto EXIT;
		}

		// decrypt received sac_message()'s encrypted_payload
		pucDecryptedSacPayload = (unsigned char *)OxCAS_Malloc(stSacMsg.usPayloadLen + 16);
		D_AES_CBC(hCc->stHostData.SEK, stSacMsg.pucPayloadData, pucDecryptedSacPayload, stSacMsg.usPayloadLen + 16, s_stCiHostLicenseConst.SIV); // �� ������ SEK ���� ��ȿ�� ���̴�.

		#if CI_CC_VerifyReceivedSacMessage // verify received sac_message()
		HxSTD_memcpy(stSacMsg.ucAuth, &pucDecryptedSacPayload[stSacMsg.usPayloadLen], 16);
		{
			unsigned char *pucTempBuf = NULL;
			unsigned char MAC[16];
			pucTempBuf = (unsigned char *)OxCAS_Malloc(1+8+stSacMsg.usPayloadLen);
			pucTempBuf[0] = 4;
			HxSTD_memcpy(&pucTempBuf[1], pucApduData, 8);
			HxSTD_memcpy(&pucTempBuf[1+8], pucDecryptedSacPayload, stSacMsg.usPayloadLen);
			MAC_AES_XCBC(hCc->stHostData.SAK, pucTempBuf, 1+8+stSacMsg.usPayloadLen, MAC); // �� ������ SAK ���� ��ȿ�� ���̴�.
			OxCAS_Free(pucTempBuf);
			if (HxSTD_memcmp(MAC, stSacMsg.ucAuth, 16) != 0)
			{
				HxLOG_Error("[CICC]---> CiCc_SyncConfirm()---> Error : Invalid authentication !!!\n");
				enArcErr = CI_ARC_ERR_SAC_MSG_Authentication;
				goto EXIT;
			}
		}
		#endif

		/* nothing to do more for cc_sac_sync_req() */

	}
	else
	{
		/* nothing to do for cc_sync_req() */
	}

	/* --------------------- */
	/* --- calculate key --- */
	/* --------------------- */

	if (ucSacFlag)
	{
		#ifdef CI_CC_CalculateCckBeforeSyncAndSetDescramblerOnSync
		#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
		if (CiCc_CalculateCckAndSetDescramblerOnSubTask(hCc, FALSE, TRUE) == CI_ARC_ERR_CCK_Failed)
		#else
		if (CiCc_CalculateCckAndSetDescrambler(hCc, FALSE, TRUE) == CI_ARC_ERR_CCK_Failed)
		#endif
		{
			HxLOG_Error("[CICC]---> CiCc_SyncConfirm()---> Error : CCK failed !!!\n");
			CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_CCK_Failed);
		}
		#endif

		#if defined(USE_SUB_TASK_TO_CALCULATE_KEYS) && defined(_CI_DEBUG_CC_TIME_ISSUE_)
		if (!s_bCalculateCckDone)
		{
			HxLOG_Error("[CICC]---> CiCc_SyncConfirm()---> Error : CCK calculation not done yet !!!\n");
//			CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_CCK_Failed);
		}
		#endif
	}
	else
	{
		#if defined(USE_SUB_TASK_TO_CALCULATE_KEYS) && defined(_CI_DEBUG_CC_TIME_ISSUE_)
		if (!s_bCalculateSacDone)
		{
			HxLOG_Error("[CICC]---> CiCc_SyncConfirm()---> Error : SAC calculation not done yet !!!\n");
//			CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_SAC_Failed);
		}
		#endif

		if (hCc->ulSacMsgCnt)
		{
			// �̷� ��Ȳ�� �������� ���� ������ ķ ���Խ� SAC ä���� �缳���Ǵ� ����� ���� �̻��� ���̴�!
			// ��·�� SAC key �� refresh �Ǹ� ulSacMsgCnt �� �����ؾ� �Ѵ�.
			HxLOG_Error("[CICC]---> CiCc_SyncConfirm()---> SAC key refresh on MsgCnt = %ld !!!\n", hCc->ulSacMsgCnt);
			#if 0 // org
			hCc->ulSacMsgCnt = 0;
			#else // SMiT �� �޸� Neotion �� ��� SacMsgCnt �� �������� �����Ƿ� �̿� ���� W/A ������.
			s_bCiCcSacKeyRefreshedUnusually = 1;
			#endif
		}
	}

	/* ----------------------------------------------------------- */
	/* --- make cc_sync_cnf() or cc_sac_sync_cnf() and send it --- */
	/* ----------------------------------------------------------- */

	/* ���⼭ status_field �������� CI_CC_STATUS_HOST_BUSY �� �����ϰ����� ��ǻ� �׻� CI_CC_STATUS_OK �� �����ؾ� �� ��... */

	if (ucSacFlag)
	{
		/*** make sending sac_message()'s encrypted_payload in case of cc_sac_sync_cnf() ***/

		// calculate original & aligned length
		usOriginalSacPayloadLen = 1;
		usAlignedSacPayloadLen = GetAlignedLen(usOriginalSacPayloadLen);

		// set sending sac_message()'s message_counter and header
		ucSacBuf[0] = (unsigned char)((stSacMsg.ulMsgCnt >> 24) & 0xff);
		ucSacBuf[1] = (unsigned char)((stSacMsg.ulMsgCnt >> 16) & 0xff);
		ucSacBuf[2] = (unsigned char)((stSacMsg.ulMsgCnt >> 8) & 0xff);
		ucSacBuf[3] = (unsigned char)(stSacMsg.ulMsgCnt & 0xff);
		ucSacBuf[4] = stSacMsg.ucHdr1;
		ucSacBuf[5] = stSacMsg.ucHdr2;
		ucSacBuf[6] = (unsigned char)((usAlignedSacPayloadLen >> 8) & 0xff);
		ucSacBuf[7] = (unsigned char)(usAlignedSacPayloadLen & 0xff);

		// allocate aligned buffers and set data
		pucOriginalSacPayload = (unsigned char *)OxCAS_Malloc(usAlignedSacPayloadLen + 16);
		pucEncryptedSacPayload = (unsigned char *)OxCAS_Malloc(usAlignedSacPayloadLen + 16);
		pucOriginalSacPayload[0] = CI_CC_STATUS_OK;

		// padding
		OneAndZeroesPadding(pucOriginalSacPayload, usOriginalSacPayloadLen);

		// authentication
		{
			unsigned char *pucTempBuf = NULL;
			unsigned char MAC[16];
			pucTempBuf = (unsigned char *)OxCAS_Malloc(1+8+usAlignedSacPayloadLen);
			pucTempBuf[0] = 4;
			HxSTD_memcpy(&pucTempBuf[1], ucSacBuf, 8);
			HxSTD_memcpy(&pucTempBuf[1+8], pucOriginalSacPayload, usAlignedSacPayloadLen);
			MAC_AES_XCBC(hCc->stHostData.SAK, pucTempBuf, 1+8+usAlignedSacPayloadLen, MAC); // �� ������ SAK ���� ��ȿ�� ���̴�.
			OxCAS_Free(pucTempBuf);
			HxSTD_memcpy(&pucOriginalSacPayload[usAlignedSacPayloadLen], MAC, 16);
		}

		// encrypt
		E_AES_CBC(hCc->stHostData.SEK, pucOriginalSacPayload, pucEncryptedSacPayload, usAlignedSacPayloadLen + 16, s_stCiHostLicenseConst.SIV); // �� ������ SEK ���� ��ȿ�� ���̴�.

		/*** make message to send ***/

		// calculate message payload length
		ulMsgPayloadLen = 8 + usAlignedSacPayloadLen + 16;

		// allocate message buffer
		pucMsg = (unsigned char *)OxCAS_Malloc(/*6*/ 8 + ulMsgPayloadLen); /* tag field (3 bytes) + max length field (3->5 bytes) + payload length */

		// set message tag
		pucMsg[0] = (unsigned char)((CI_CC_SAC_SYNC_CNF >> 16) & 0xff);
		pucMsg[1] = (unsigned char)((CI_CC_SAC_SYNC_CNF >> 8) & 0xff);
		pucMsg[2] = (unsigned char)(CI_CC_SAC_SYNC_CNF & 0xff);

		// set message length
		CIRH_ConvertApduLengthField(ulMsgPayloadLen, &pucMsg[3], &ucLenFieldSize);

		// set message payload
		HxSTD_memcpy(&pucMsg[3+ucLenFieldSize], ucSacBuf, 8);
		HxSTD_memcpy(&pucMsg[11+ucLenFieldSize], pucEncryptedSacPayload, usAlignedSacPayloadLen + 16);
	}
	else
	{
		ulMsgPayloadLen = 1;
		ucLenFieldSize = 1;
		pucMsg = (unsigned char *)OxCAS_Malloc(3 + ucLenFieldSize + ulMsgPayloadLen);
		pucMsg[0] = (unsigned char)((CI_CC_SYNC_CNF >> 16) & 0xff);
		pucMsg[1] = (unsigned char)((CI_CC_SYNC_CNF >> 8) & 0xff);
		pucMsg[2] = (unsigned char)(CI_CC_SYNC_CNF & 0xff);
		pucMsg[3] = 0x01;
		pucMsg[4] = CI_CC_STATUS_OK;
	}

	if (CIRH_SendApdu(hCc->usSsNo, pucMsg, 3 + ucLenFieldSize + ulMsgPayloadLen) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_SyncConfirm()---> Error : fail to send APDU for SsNo[%d] !!!\n", hCc->usSsNo);
		enArcErr = CI_ARC_ERR_Send_Apdu;
		goto EXIT;
	}

	HxLOG_Print("[CICC]---> CiCc_SyncConfirm()---> %ld bytes payload sent\n", ulMsgPayloadLen);
	#if 0 // #ifdef _CI_DEBUG_CC_DATA_
	{
		int i;
		for (i = 0; i < 3 + ucLenFieldSize; i++)
		{
			if ((i & 15) == 0)
				HxLOG_Print("\n");
			HxLOG_Print(" %02x", pucMsg[i]);
		}
		for (i = 0; i < usMsgPayloadLen; i++)
		{
			if ((i & 15) == 0)
				HxLOG_Print("\n");
			HxLOG_Print(" %02x", pucMsg[3 + ucLenFieldSize + i]);
		}
		HxLOG_Print("\n\n");
	}
	#endif

EXIT :

	if (ucSacFlag)
	{
		if (pucDecryptedSacPayload)
			OxCAS_Free(pucDecryptedSacPayload);
		if (pucOriginalSacPayload)
			OxCAS_Free(pucOriginalSacPayload);
		if (pucEncryptedSacPayload)
			OxCAS_Free(pucEncryptedSacPayload);
	}

	if (pucMsg)
		OxCAS_Free(pucMsg);

	HxLOG_Print("[CICC]---> CiCc_SyncConfirm()---> SsNo[%d]...\n", hCc->usSsNo);

	return enArcErr;
}
#endif

/* �Լ� ���� : cc_data_req() or cc_sac_data_req() �� ���� ķ���� ȣ��Ʈ�� ���޵� �����͵��� �����ϰ� �ʿ��ϴٸ� ȣ��Ʈ ���̵忡 �����Ѵ�.
               �� ���� �׼��� CiCc_MakeReqDataAndDoSomething()���� ó���ϵ��� �Ѵ�! */
static void CiCc_VerifyAndSaveSendData(CI_CC_INSTANCE *hCc, unsigned char ucSendDataId, unsigned short usSendDataLen, unsigned char *pucSendData)
{
	switch (ucSendDataId)
	{
		case CI_CC_CICAM_ID:
			if (usSendDataLen != 8)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				CiCc_DoActionForArcErr(hCc, hCc->enPhase == CI_CC_PHASE_Authentication_Key_Verification_Replied ? CI_ARC_ERR_SAC_Failed : CI_ARC_ERR_CCK_Failed);
			}
			else
			{
				CI_AUTH_CTX * pstCiAuthCtx = NULL;
			#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
			#else
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
			#endif
				{
					HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Can't load binded CICAM_ID !!!\n");
					CiCc_DoActionForArcErr(hCc, hCc->enPhase == CI_CC_PHASE_Authentication_Key_Verification_Replied ? CI_ARC_ERR_SAC_Failed : CI_ARC_ERR_CCK_Failed);
				}
				else
				{
					if (HxSTD_memcmp(pucSendData, pstCiAuthCtx->ucBindedCamId, usSendDataLen) != 0) // �� ������ ucBindedCamId ���� ��ȿ�� ���̴�.
					{
						HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Not matched CICAM_ID !!!\n");
						HxLOG_Error("[%02x %02x %02x %02x %02x %02x %02x %02x] vs [%02x %02x %02x %02x %02x %02x %02x %02x]\n",
							pucSendData[0], pucSendData[1], pucSendData[2], pucSendData[3], pucSendData[4], pucSendData[5], pucSendData[6], pucSendData[7],
							pstCiAuthCtx->ucBindedCamId[0], pstCiAuthCtx->ucBindedCamId[1], pstCiAuthCtx->ucBindedCamId[2], pstCiAuthCtx->ucBindedCamId[3],
							pstCiAuthCtx->ucBindedCamId[4], pstCiAuthCtx->ucBindedCamId[5], pstCiAuthCtx->ucBindedCamId[6], pstCiAuthCtx->ucBindedCamId[7]);
						CiCc_DoActionForArcErr(hCc, hCc->enPhase == CI_CC_PHASE_Authentication_Key_Verification_Replied ? CI_ARC_ERR_SAC_Failed : CI_ARC_ERR_CCK_Failed);
					}
				}
			}
			HxLOG_Info("CI_CC_CICAM_ID done\n");
			break;

		case CI_CC_CICAM_BrandCert:
			// Authentication �������� CI_CC_DHPM, CI_CC_Signature_B, CI_CC_CICAM_DevCert, CI_CC_CICAM_BrandCert ������ ���� ���谡 �����ϹǷ�
			// CI_CC_CICAM_BrandCert �� ���� ������ ���⼭ ó������ �ʰ� CiCc_MakeReqDataAndDoSomething()���� ó���ϵ��� �Ѵ�.
			HxSTD_memcpy(hCc->stCicamData.CICAM_BrandCert, pucSendData, usSendDataLen); // keep CICAM_BrandCert for further use
			hCc->stCicamData.CICAM_BrandCertLen = usSendDataLen;
			HxLOG_Info("CI_CC_CICAM_BrandCert done\n");
			break;

		case CI_CC_Kp:
			if (usSendDataLen != 32)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_CCK_Failed);
			}
			else
			{
				HxSTD_memcpy(hCc->stCicamData.Kp, pucSendData, usSendDataLen); // keep Kp for further use
				#ifdef CI_CC_CalculateCckBeforeSyncAndSetDescramblerOnSync
				#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
				if (CiCc_CalculateCckAndSetDescramblerOnSubTask(hCc, TRUE, FALSE) == CI_ARC_ERR_CCK_Failed)
				#else
				if (CiCc_CalculateCckAndSetDescrambler(hCc, TRUE, FALSE) == CI_ARC_ERR_CCK_Failed)
				#endif
				{
					HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : CCK failed !!!\n");
					CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_CCK_Failed);
				}
				#endif
				hCc->enPhase = CI_CC_PHASE_CCK_Calculating_Requested;
			}
			HxLOG_Info("CI_CC_Kp done\n");
			break;

		case CI_CC_DHPM:
			// Authentication �������� CI_CC_DHPM, CI_CC_Signature_B, CI_CC_CICAM_DevCert, CI_CC_CICAM_BrandCert ������ ���� ���谡 �����ϹǷ�
			// CI_CC_DHPM �� ���� ������ ���⼭ ó������ �ʰ� CiCc_MakeReqDataAndDoSomething()���� ó���ϵ��� �Ѵ�.
			if (usSendDataLen != 256)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_Auth_DH_Failed);
			}
			else
			{
				HxSTD_memcpy(hCc->stCicamData.DHPM, pucSendData, usSendDataLen); // keep DHPM for further use
			}
			HxLOG_Info("CI_CC_DHPM done\n");
			break;

		case CI_CC_CICAM_DevCert:
			// Authentication �������� CI_CC_DHPM, CI_CC_Signature_B, CI_CC_CICAM_DevCert, CI_CC_CICAM_BrandCert ������ ���� ���谡 �����ϹǷ�
			// CI_CC_CICAM_DevCert �� ���� ������ ���⼭ ó������ �ʰ� CiCc_MakeReqDataAndDoSomething()���� ó���ϵ��� �Ѵ�.
			HxSTD_memcpy(hCc->stCicamData.CICAM_DevCert, pucSendData, usSendDataLen); // keep CICAM_DevCert for further use
			hCc->stCicamData.CICAM_DevCertLen = usSendDataLen;
			HxLOG_Info("CI_CC_CICAM_DevCert done\n");
			break;

		case CI_CC_Signature_B:
			// Authentication �������� CI_CC_DHPM, CI_CC_Signature_B, CI_CC_CICAM_DevCert, CI_CC_CICAM_BrandCert ������ ���� ���谡 �����ϹǷ�
			// CI_CC_Signature_B �� ���� ������ ���⼭ ó������ �ʰ� CiCc_MakeReqDataAndDoSomething()���� ó���ϵ��� �Ѵ�.
			if (usSendDataLen != 256)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_Auth_Signature_Failed);
			}
			else
			{
				HxSTD_memcpy(hCc->stCicamData.Signature_B, pucSendData, usSendDataLen); // keep Signature_B for further use
			}
			HxLOG_Info("CI_CC_Signature_B done\n");
			break;

		case CI_CC_auth_nonce:
			if (usSendDataLen != 32)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_Auth_Key_Verification_Failed);
			}
			else
			{
				HxSTD_memcpy(hCc->stCicamData.auth_nonce, pucSendData, usSendDataLen); // keep auth_nonce for further use
				hCc->enPhase = CI_CC_PHASE_Authentication_Requested;

				// UI ó���� ���� �ݹ�
				s_stCcReportInfo.eCcReport = eCiPlus_CcReport_VirginCamInsertedAndAuthenticationInProgress;
				Callback_Reset(g_hCbNotifyCcReport);
				while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
				{
					(*s_pfnNotifyCcReport)(hCc->usSlotId, &s_stCcReportInfo);
				}
			}
			HxLOG_Info("CI_CC_auth_nonce done\n");
			break;

		case CI_CC_Ns_module:
			if (usSendDataLen != 8)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_SAC_Failed);
			}
			else
			{
				HxSTD_memcpy(hCc->stCicamData.Ns_module, pucSendData, usSendDataLen); // keep Ns_module for further use
				hCc->enPhase = CI_CC_PHASE_SAC_Establishment_Requested;

				// CAS/CI mgr ó���� ���� �ݹ�
				s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationDoneAndStartSacEstablishment;
				Callback_Reset(g_hCbNotifyCcReport);
				while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
				{
					(*s_pfnNotifyCcReport)(hCc->usSlotId, &s_stCcReportInfo);
				}
			}
			HxLOG_Info("CI_CC_Ns_module done\n");
			break;

		case CI_CC_uri_message:
			if (usSendDataLen != 8)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_uri_message);
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
				hCc->enLicenseRcvdStatus = CI_CC_LICENSE_RCVD_STATUS_INVALID_DATA;
#endif
			}
			else
			{
				if (pucSendData[0] != CI_CC_URI_PROTOCOL_V1 && pucSendData[0] != CI_CC_URI_PROTOCOL_V2)
				{
					HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : URI protocol not supported[0x%x] !!!\n", pucSendData[0]);
					/* ȣȯ�� ��������... �����ϴ� version �� �ƴϾ �ϴ� ��� �ְ� ó���� ������... */
				}

				HxSTD_memcpy(hCc->stCicamData.uri_message, pucSendData, usSendDataLen); // keep uri_message for further use

				if (pucSendData[0] == CI_CC_URI_PROTOCOL_V1)
				{
					hCc->stCicamData.stUri.ucApsCopyCtrl = (pucSendData[1] & 0xC0) >> 6;
					hCc->stCicamData.stUri.ucEmiCopyCtrl = (pucSendData[1] & 0x30) >> 4;
					hCc->stCicamData.stUri.ucIctCopyCtrl = (pucSendData[1] & 0x08) >> 3;
					hCc->stCicamData.stUri.ucRctCopyCtrl = (pucSendData[1] & 0x04) >> 2;
					hCc->stCicamData.stUri.ucDotCopyCtrl = CI_CC_URI_DOT_OFF;
					if (hCc->stCicamData.stUri.ucEmiCopyCtrl == CI_CC_URI_EMI_COPY_NEVER)
					{
						hCc->stCicamData.stUri.ucRlCopyCtrl = pucSendData[2] & 0x3F;
					}
					else
					{
						hCc->stCicamData.stUri.ucRlCopyCtrl = CI_CC_URI_RL_UNLIMITED;
					}
				}
				else // if (pucSendData[0] == CI_CC_URI_PROTOCOL_V2)
				{
					hCc->stCicamData.stUri.ucApsCopyCtrl = (pucSendData[1] & 0xC0) >> 6;
					hCc->stCicamData.stUri.ucEmiCopyCtrl = (pucSendData[1] & 0x30) >> 4;
					hCc->stCicamData.stUri.ucIctCopyCtrl = (pucSendData[1] & 0x08) >> 3;
					if (hCc->stCicamData.stUri.ucEmiCopyCtrl == CI_CC_URI_EMI_COPY_FREE)
					{
						hCc->stCicamData.stUri.ucRctCopyCtrl = (pucSendData[1] & 0x04) >> 2;
					}
					else
					{
						hCc->stCicamData.stUri.ucRctCopyCtrl = CI_CC_URI_RCT_OFF;
					}
					if (hCc->stCicamData.stUri.ucEmiCopyCtrl == CI_CC_URI_EMI_COPY_NEVER)
					{
						hCc->stCicamData.stUri.ucDotCopyCtrl = (pucSendData[1] & 0x01);
						hCc->stCicamData.stUri.ucRlCopyCtrl = pucSendData[2];
					}
					else
					{
						hCc->stCicamData.stUri.ucDotCopyCtrl = CI_CC_URI_DOT_OFF;
						hCc->stCicamData.stUri.ucRlCopyCtrl = CI_CC_URI_RL_UNLIMITED;
					}
				}
			}
			HxLOG_Info("CI_CC_uri_message done\n");
			break;

		case CI_CC_program_number:
			if (usSendDataLen != 2)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
				hCc->enLicenseRcvdStatus = CI_CC_LICENSE_RCVD_STATUS_INVALID_DATA;
#endif
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_program_number);
			}
			else
			{
				hCc->stCicamData.usProgramNumber = ((unsigned short)pucSendData[0] << 8) & 0xff00; // keep program_number for further use
				hCc->stCicamData.usProgramNumber |= ((unsigned short)pucSendData[1] & 0x00ff);
			}
			HxLOG_Info("CI_CC_program_number done\n");
			break;

		case CI_CC_key_register:
			if (usSendDataLen != 1)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				CiCc_DoActionForArcErr(hCc, hCc->enPhase == CI_CC_PHASE_SAC_Establishment_Requested ? CI_ARC_ERR_SAC_Failed : CI_ARC_ERR_CCK_Failed);
			}
			else
			{
				if (pucSendData[0] != CI_CC_KEY_MODE_EVEN && pucSendData[0] != CI_CC_KEY_MODE_ODD)
				{
					HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid Key[0x%x] !!!\n", pucSendData[0]);
					CiCc_DoActionForArcErr(hCc, hCc->enPhase == CI_CC_PHASE_SAC_Establishment_Requested ? CI_ARC_ERR_SAC_Failed : CI_ARC_ERR_CCK_Failed);
				}
				else
				{
					hCc->stCicamData.enKeyRegister = pucSendData[0]; // keep key_register for further use
				}
			}
			HxLOG_Info("CI_CC_key_register done (%d)\n", pucSendData[0]);
			break;

		case CI_CC_srm_data:
			// CI_CC_uri_message ~ CI_CC_uri_confirm ó���� �����ϳ� SRM data ����� �������̸� ����� Ŭ �� �ֱ� ������ SRM data �� ������ �ʿ䰡 ������
			// CI_CC_srm_confirm �� ���� ó���� CiCc_MakeReqDataAndDoSomething()���� ���� �ʰ� ���⼭ �ϵ��� �Ѵ�.
			CiCc_GenerateSrmConfirm(hCc, pucSendData, usSendDataLen);
			CiCc_SrmDoApply(hCc, pucSendData, usSendDataLen);
			HxLOG_Info("CI_CC_srm_data done\n");
			break;

		case CI_CC_cicam_license:
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
			hCc->stCicamData.unLicenseDataLen = usSendDataLen;
			if( hCc->stCicamData.pucLicenseData != NULL )
			{
				OxCAS_Free(hCc->stCicamData.pucLicenseData);
				hCc->stCicamData.pucLicenseData = NULL;
			}
			hCc->stCicamData.pucLicenseData = OxCAS_Malloc(usSendDataLen);
			HxSTD_memcpy(hCc->stCicamData.pucLicenseData, pucSendData, usSendDataLen);
			HxLOG_Info("CI_CC_cicam_license done\n");
#endif
			break;
		case CI_CC_license_status:
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
			if (usSendDataLen != 1)
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId);
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_Not_Defined);
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
				hCc->enLicenseRcvdStatus = CI_CC_LICENSE_RCVD_STATUS_INVALID_DATA;
#endif
			}
			hCc->stCicamData.enLicenseStatus = (CI_CONTENT_LICENSE_STATUS)pucSendData[0];
			HxLOG_Info("CI_CC_license_status done\n");
#endif
			break;
		/* ���� data id ���� ȣ��Ʈ�� �������� �ʴ� �͵��̰ų� CI+ ���� 1.2 & 1.3 �������� �� ���� ������ �ʴ� �͵� */
		case CI_CC_Brand_ID:
		case CI_CC_Host_ID:
		case CI_CC_Host_BrandCert:
		case CI_CC_DHPH:
		case CI_CC_Host_DevCert:
		case CI_CC_Signature_A:
		case CI_CC_Ns_Host:
		case CI_CC_AKH:
		case CI_CC_AKM:
		case CI_CC_uri_confirm:
		case CI_CC_uri_versions:
		case CI_CC_status_field:
		case CI_CC_srm_confirm:
		case CI_CC_license_rcvd_status:
			HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid SendDataId[%d] !!!\n", ucSendDataId);
			break;

		default:
			HxLOG_Error("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Unknown SendDataId[%d] !!!\n", ucSendDataId);
			break;
	}
}

/* �Լ� ���� : cc_data_cnf() or cc_sac_data_cnf() �� ���� ȣ��Ʈ���� ķ���� ���޵� �����͵��� �����ϰ� �ʿ��� �׼��� �ִٸ� ���⼭ �����Ѵ�. */
static void CiCc_MakeReqDataAndDoSomething(CI_CC_INSTANCE *hCc, unsigned char ucReqDataId, unsigned short *pusReqDataLen, unsigned char **ppucReqData)
{
	unsigned short	usDataLen = 0;
	unsigned char	*pucData = NULL;

	switch (ucReqDataId)
	{
		case CI_CC_Host_ID:
			usDataLen = 8;
			pucData = (unsigned char *)OxCAS_Malloc(usDataLen);
			HxSTD_memcpy(pucData, s_stCiHostLicenseConst.Host_ID, usDataLen);

			#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
			hCc->bUsedCurAuthCtxIdx = TRUE;
			#endif

			HxLOG_Info("CI_CC_Host_ID done\n");
			break;

		case CI_CC_Host_BrandCert:
			usDataLen = s_stCiHostLicenseConst.Host_BrandCertLen;
			pucData = (unsigned char *)OxCAS_Malloc(usDataLen);
			HxSTD_memcpy(pucData, s_stCiHostLicenseConst.Host_BrandCert, usDataLen);
			HxLOG_Info("CI_CC_Host_BrandCert done\n");
			break;

		case CI_CC_DHPH:
			usDataLen = 256;
			pucData = (unsigned char *)OxCAS_Malloc(usDataLen);
			HxSTD_memcpy(pucData, s_stCiHostLicenseConst.DHPH, usDataLen);
			HxLOG_Info("CI_CC_DHPH done\n");
			break;

		case CI_CC_Host_DevCert:
			usDataLen = s_stCiHostLicenseConst.Host_DevCertLen;
			pucData = (unsigned char *)OxCAS_Malloc(usDataLen);
			HxSTD_memcpy(pucData, s_stCiHostLicenseConst.Host_DevCert, usDataLen);
			HxLOG_Info("CI_CC_Host_DevCert done\n");
			break;

		case CI_CC_Signature_A:
			HxLOG_Info("CI_CC_Signature_A start \n");
			usDataLen = 256;
			pucData = (unsigned char *)OxCAS_Malloc(usDataLen);
			if (CiCc_GenerateSignature(hCc, pucData) == CI_ARC_ERR_Auth_Signature_Failed)
			{
				HxLOG_Error("[CICC]---> CiCc_MakeReqData()---> Error : host signature !!!\n"); // �̷� ��Ȳ�� �߻��ϸ� ���. ����� �ʿ�!!
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_Auth_Signature_Failed);
				goto ERROR;
			}
			HxLOG_Info("CI_CC_Signature_A done\n");
			break;

		case CI_CC_Ns_Host:
			usDataLen = 8;
			// Ns_Host[8] -> CI_CC_CIPHER_BLOCK_SIZE ��ŭ�� ������ alloc -> PRNG(Ns_Host) 1�� ó�� -> truncated LSB to 8 bytes -> OK?
			pucData = (unsigned char *)OxCAS_Malloc(CI_CC_CIPHER_BLOCK_SIZE);
			PRNG(s_stCiHostLicenseConst.PRNG_seed, pucData);
			HxSTD_memcpy(hCc->stHostData.Ns_Host, pucData, usDataLen); // keep Ns_Host for further use
			#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
			if (CiCc_CalculateSacKeysOnSubTask(hCc) == CI_ARC_ERR_SAC_Failed)
			#else
			if (CiCc_CalculateSacKeys(hCc) == CI_ARC_ERR_SAC_Failed)
			#endif
			{
				HxLOG_Error("[CICC]---> CiCc_MakeReqData()---> Error : SAC failed !!!\n"); // �̷� ��Ȳ�� �߻��ϸ� ���. ����� �ʿ�!!
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_SAC_Failed);
			}
			hCc->enPhase = CI_CC_PHASE_SAC_Establishment_Replied;
			HxLOG_Info("CI_CC_Ns_Host done\n");
			break;

		case CI_CC_AKH:
			usDataLen = 32;
			pucData = (unsigned char *)OxCAS_Malloc(usDataLen);
			if (CiCc_LoadAkh(hCc, pucData) < 0) // ����� AKH�� ���� ��� -> invalid AKH �� ������ -> ���� ������ �Ͼ ���̴�...
			{
				HxSTD_memset(pucData, 0x00, usDataLen);
			}

			#if defined(CONFIG_DEBUG)
			{
				HUINT32			i, j;
				CI_AUTH_CTX		*pstCiAuthCtx = NULL;

				#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
				HxLOG_Info("bUsedCurAuthCtxIdx = %d\n", hCc->bUsedCurAuthCtxIdx);
				#endif

				HxLOG_Info("nCurAuthCtxIdx = %d\n", hCc->nCurAuthCtxIdx);
				HxLOG_Info("nTryAuthCtxIdx = %d\n", hCc->nTryAuthCtxIdx);

				for (i = 0; i < CI_NUM_OF_AUTH_CTX; i++)
				{
					#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
					pstCiAuthCtx = CiCc_GetAuthContext(i);
					#else
					pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, i);
					#endif

					HxLOG_Print("-----------------------------------\n");
					HxLOG_Print("(%d) Ci auth Ctx info\n", i);
					HxLOG_Print("ucSavedFlag = %d\n", pstCiAuthCtx->ucSavedFlag);
					HxLOG_Print("AKH = [ ");
					for(j = 0; j < 32; j++) HxLOG_Print(" %02x", pstCiAuthCtx->AKH[j]);
					HxLOG_Print(" ]\n");
				}
			}
			#endif
			hCc->enPhase = CI_CC_PHASE_Authentication_Key_Verification_Replied;
			HxLOG_Info("CI_CC_AKH done\n");
			break;

		case CI_CC_uri_confirm:
			usDataLen = 32;
			pucData = (unsigned char *)OxCAS_Malloc(usDataLen);
			if (hCc->ulUriTimerID)
			{
				VK_TIMER_Cancel(hCc->ulUriTimerID);
				hCc->ulUriTimerID = VK_TIMERID_NULL;
			}
			CiCc_GenerateUriConfirm(hCc, pucData);
			CiCc_UriDoCopyCtrl(hCc, CI_CC_URI_MODE_FROMCAM);
			HxLOG_Info("CI_CC_uri_confirm done\n");
			break;

		case CI_CC_uri_versions:
			usDataLen = 32;
			pucData = (unsigned char *)OxCAS_Malloc(usDataLen);
			HxSTD_memset(pucData, 0x00, 31);
			#ifdef CONFIG_MW_CI_PLUS_V_1_3 // ���߿� CI+ v1.3 �� default �� �Ǹ� ifdef �����ϰ� �Ϲ������� ó���ϵ��� �� ��
			HxLOG_Info("CI_CC_uri_versions: Host support CI+ 1.3!\n");
			if (CI_IsCiPlusCam(hCc->usSlotId) > 0 // CI+ CAM �̰�,
				&& local_CiCc_GetCcResVersion(hCc) == CI_CC_RES_V2_ID)	// �ش� Session�� CC_RES_V2�� ������ ���
			{
				HxLOG_Info("CI_CC_uri_versions is operated as CI+ 1.3.\n");
				pucData[31] = CI_CC_URI_PROTOCOL_V2 | CI_CC_URI_PROTOCOL_V1;
			}
			else
			{
				HxLOG_Info("CI_CC_uri_versions is operated as CI+ 1.2.\n");
				pucData[31] = CI_CC_URI_PROTOCOL_V1;
			}
			#else
			pucData[31] = CI_CC_URI_PROTOCOL_V1;
			#endif
			hCc->enPhase = CI_CC_PHASE_URI_Version_Negotiation_Replied;
			#if 1 // CAS/CI mgr ó���� ���� �ݹ� -> Multi SLOT�� instance �������� CI+ Authentication ������ �Ϸ� �Ǿ������� ���.
			s_stCcReportInfo.eCcReport = eCiPlus_CcReport_UriVersionNegotiationDone;
			Callback_Reset(g_hCbNotifyCcReport);
			while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
			{
				(*s_pfnNotifyCcReport)(hCc->usSlotId, &s_stCcReportInfo);
			}
			#endif
			HxLOG_Info("CI_CC_uri_versions done\n");
			break;

		case CI_CC_status_field:
			usDataLen = 1;
			pucData = (unsigned char *)OxCAS_Malloc(usDataLen);
			if (hCc->enPhase == CI_CC_PHASE_CCK_Calculating_Requested) // for CCK calculation
			{
				#ifdef CI_CC_CalculateCckAndSetDescramblerBeforeSync
				#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
				if (CiCc_CalculateCckAndSetDescramblerOnSubTask(hCc, TRUE, TRUE) == CI_ARC_ERR_CCK_Failed)
				#else
				if (CiCc_CalculateCckAndSetDescrambler(hCc, TRUE, TRUE) == CI_ARC_ERR_CCK_Failed)
				#endif
				{
					HxLOG_Error("[CICC]---> CiCc_MakeReqData()---> Error : CCK failed !!!\n");
					CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_CCK_Failed);
				}
				#endif
				if (hCc->enArcErr != CI_ARC_ERR_None)
				{
					pucData[0] = CI_CC_STATUS_NO_CC_SUPPORT;
				}
				else
				{
					pucData[0] = CI_CC_STATUS_OK;
					// TODO: ���������� ������ �ó������� ���� Authentication�� �Ϸ�� ���� ���Ŀ� CC resource v.2�� PIN capability request�� ���� �ؾ� ��.
					// TODO: ���⿡ ����־ �Ǵ��� ���� �ʿ� (kkkim)
					#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
					HxLOG_Info("[CICC/CCv2]---> Check that the CC resource of CAM is operated with version 2 or not.\n");
					if (CI_IsCiPlusCam(hCc->usSlotId) > 0 // CI+ CAM �̰�,
						&& local_CiCc_GetCcResVersion(hCc) == CI_CC_RES_V2_ID)	// �ش� Session�� CC_RES_V2�� ������ ���
					{
						HxLOG_Info("[CICC/CCv2]---> The CC resource of CAM is operating with version 2.\n");
						if (hCc->stCicamData.stPinCapaInfo.bPinCapaSet == FALSE)
						{
							HxLOG_Print("[CICC/CCv2]---> After authentication succeed, Sending PinCapaReq().\n");
							CiCc_PinCapabilityReq(hCc);
						}
					}
					#endif
				}
				hCc->enPhase = CI_CC_PHASE_CCK_Calculating_Replied;
			}
			else if (hCc->enPhase == CI_CC_PHASE_Authentication_Requested) // for full Authentication
			{
				SvcCas_CiArcErr_e enArcErr;

				#if defined(CONFIG_DEBUG)
				CHECK_AUTH_TIME(hCc->usSlotId);
				#endif

				enArcErr = CiCc_VerifyCamAndSaveInfo(hCc);
				if (enArcErr != CI_ARC_ERR_None)
				{
					HxLOG_Error("[CICC]---> CiCc_MakeReqData()---> Error : CiCc_VerifyCamAndSaveInfo() return %d !!!\n", enArcErr);
					CiCc_DoActionForArcErr(hCc, enArcErr);
				}
				else
				{
					if (CiCc_GenerateHostNonVolatileKeys(hCc) != CI_ARC_ERR_None)
					{
						HxLOG_Error("[CICC]---> CiCc_MakeReqData()---> Error : host non-volatile keys !!!\n");
						CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_Auth_Key_Computation_Failed);
					}
				}
				if (hCc->enArcErr != CI_ARC_ERR_None)
				{
					pucData[0] = CI_CC_STATUS_AUTH_FAILED_OR_NOT_REQ;
				}
				else
				{
					pucData[0] = CI_CC_STATUS_OK;

					#if 0 // UI ó���� ���� �ݹ� �ʿ� ������ ���� ��! (���� SMiT ��û���� �� ����� ��� ǥ�ø� ���� �ʵ��� �������� ����.)
					s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationSuccess;
					Callback_Reset(g_hCbNotifyCcReport);
					while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
					{
						(*s_pfnNotifyCcReport)(hCc->usSlotId, &s_stCcReportInfo);
					}
					#endif
				}
				hCc->enPhase = CI_CC_PHASE_Authentication_Replied;

				#if defined(CONFIG_DEBUG)
				CHECK_AUTH_TIME(hCc->usSlotId);
				#endif

			}
			else // for SRM transmission and acknowledgement
			{
				pucData[0] = CI_CC_STATUS_OK;
			}

			HxLOG_Info("CI_CC_status_field done (0x%x)\n", pucData[0]);
			break;

		case CI_CC_srm_confirm:
			// CI_CC_uri_message ~ CI_CC_uri_confirm ó���� �����ϳ� SRM data ����� �������̸� ����� Ŭ �� �ֱ� ������ SRM data �� ������ �ʿ䰡 ������
			// CI_CC_srm_confirm �� ���� ó���� ���⼭ ���� �ʰ� CiCc_VerifyAndSaveSendData()���� �ϵ��� �Ѵ�.
			usDataLen = 32;
			pucData = (unsigned char *)OxCAS_Malloc(usDataLen);
			HxSTD_memcpy(pucData, hCc->stHostData.srm_confirm, usDataLen); // �� ������ srm_confirm ���� ��ȿ�� ���̴�.
			HxLOG_Info("CI_CC_srm_confirm done\n");
			break;

		case CI_CC_license_rcvd_status:
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
			{
				CI_CONTENT_LICENSE_INFO 	stLicenseInfo;
				CI_AUTH_CTX 				*pstCiAuthCtx = NULL;

			#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
			#else
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
			#endif
				{
					HxLOG_Error("[CICC]---> CiCc_ProcPinReply()---> Error : Can't load context !!!\n");
					return;
				}

				if (hCc->hRecAction == (Handle_t)NULL)
				{
					HxLOG_Error("[CICC]---> CiCc_MakeReqData()---> Error : CI_CC_license_rcvd_status [No RecAction Info]!!!\n"); // �̷� ��Ȳ�� �߻��ϸ� ���. ����� �ʿ�!!
					// TODO: �Ʒ� CiCc_DoActionForArcErr()�Լ��� ���� UI�� Error �Ѹ��� �κ��� ��ǰ ��õɶ��� ���� �Ǿ�� �Ѵ�. (kkkim)
					// �̷� ��Ȳ ��ü�� �߻��ϸ� �ȵǴ� ���� ���⸦ Ÿ�� ��찡 �ִٸ� �ó������� ���� �ڵ��� �� �� �ֵ��� ó�� �Ǿ�� �ҵ�.
					CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_Not_Defined);
					goto ERROR;
				}

				usDataLen = 1;
				pucData = (unsigned char *)OxCAS_Malloc(usDataLen);
				pucData[0] = hCc->enLicenseRcvdStatus;

				// ���� license data(4��)�� ���� ���� �� �޾Ҵ��� ���θ� ����  LicenseRcvdStatus�� ���� �Ǿ� �ִ� ���¿��� �ش� ���� pucData�� �����ϰ�, �ٽ� OK�� �ʱ�ȭ �ϵ��� �Ѵ�.
				hCc->enLicenseRcvdStatus = CI_CC_LICENSE_RCVD_STATUS_OK;

				stLicenseInfo.hAction = hCc->hRecAction;
				stLicenseInfo.usProgramNumber = hCc->stCicamData.usProgramNumber;

				stLicenseInfo.ucAps = hCc->stCicamData.stUri.ucApsCopyCtrl;
				stLicenseInfo.ucEmi = hCc->stCicamData.stUri.ucEmiCopyCtrl;
				stLicenseInfo.ucIct = hCc->stCicamData.stUri.ucIctCopyCtrl;
				stLicenseInfo.ucRct = hCc->stCicamData.stUri.ucRctCopyCtrl;
				stLicenseInfo.ucDot = hCc->stCicamData.stUri.ucDotCopyCtrl;
				CiCc_GetMinutesForRL(hCc->stCicamData.stUri.ucRlCopyCtrl, &stLicenseInfo.ulMinutesForRL);

				stLicenseInfo.unLicenseDataLen = hCc->stCicamData.unLicenseDataLen;
				stLicenseInfo.pucLicenseData = hCc->stCicamData.pucLicenseData;

				HxSTD_memcpy(stLicenseInfo.ucBindedCamId, pstCiAuthCtx->ucBindedCamId, 8); // �� ������ auth_nonce ���� ��ȿ�� ���̴�.

				Callback_Reset(g_hCbNotifyCcLicense);
				while ((s_pfnNotifyCcLicense = (CI_NOTIFY_CC_LICENSE_FUNC)Callback_GetNextFunc(g_hCbNotifyCcLicense)) != NULL)
				{
					(*s_pfnNotifyCcLicense)(hCc->usSlotId, hCc->usSsNo, hCc->stCicamData.enLicenseStatus, &stLicenseInfo);
				}
				HxLOG_Info("CI_CC_license_rcvd_status done\n");
			}
#endif
			break;

		/* ���� data id ���� ȣ��Ʈ���� �������� �ʴ� �͵��̰ų� CI+ ���� 1.2 & 1.3 �������� �� ���� ������ �ʴ� �͵� */
		case CI_CC_Brand_ID:
		case CI_CC_CICAM_ID:
		case CI_CC_CICAM_BrandCert:
		case CI_CC_Kp:
		case CI_CC_DHPM:
		case CI_CC_CICAM_DevCert:
		case CI_CC_Signature_B:
		case CI_CC_auth_nonce:
		case CI_CC_Ns_module:
		case CI_CC_AKM:
		case CI_CC_uri_message:
		case CI_CC_program_number:
		case CI_CC_key_register:
		case CI_CC_srm_data:
		case CI_CC_cicam_license:
		case CI_CC_license_status:
			HxLOG_Error("[CICC]---> CiCc_MakeReqData()---> Error : Invalid ReqDataId[%d] !!!\n", ucReqDataId);
			break;

		default:
			HxLOG_Error("[CICC]---> CiCc_MakeReqData()---> Error : Unknown ReqDataId[%d] !!!\n", ucReqDataId);
			break;
	}

	*pusReqDataLen = usDataLen;
	*ppucReqData = pucData;
	return;

ERROR : // �̷� ��Ȳ�� �߻��ϸ� ���. ����� �ʿ�!!

	if (pucData)
		OxCAS_Free(pucData);
	*pusReqDataLen = 0;
	*ppucReqData = NULL;
	return;
}

/******************************************/
/*** Local functions for Authentication ***/
/******************************************/

static SvcCas_CiArcErr_e CiCc_GenerateSignature(CI_CC_INSTANCE *hCc, unsigned char *pucSignature)
{
	unsigned char buf[302];
	RSA_private_key HDQ;
	bignum n, d, p, q, dP, dQ, qInv;
	bignum M, S;
	bignum salt = {20, {0x62,0x79,0x20,0x68,0x6D,0x6B,0x69,0x6D,0x2C,0x20,0x68,0x75,0x6D,0x61,0x78,0x2E,0x2E,0x2E,0x2E,0x2E}};

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
	#endif

	if (s_stCiHostLicenseConst.ucSavedFlag == 1)
	{
		bn_init(&n, s_stCiHostLicenseConst.HDQ_n, CI_HDQ_N_LEN);
		bn_init(&d, s_stCiHostLicenseConst.HDQ_d, CI_HDQ_D_LEN);
//		bn_init(&p, s_stCiHostLicenseConst.HDQ_p, CI_HDQ_P_LEN);
//		bn_init(&q, s_stCiHostLicenseConst.HDQ_q, CI_HDQ_Q_LEN);
//		bn_init(&dP, s_stCiHostLicenseConst.HDQ_dP, CI_HDQ_DP_LEN);
//		bn_init(&dQ, s_stCiHostLicenseConst.HDQ_dQ, CI_HDQ_DQ_LEN);
//		bn_init(&qInv, s_stCiHostLicenseConst.HDQ_qInv, CI_HDQ_QINV_LEN);

		HDQ.n = &n;
		HDQ.d = &d;
		HDQ.p = NULL;
		HDQ.q = NULL;
		HDQ.dP = NULL;
		HDQ.dQ = NULL;
		HDQ.qInv = NULL;
	}
	else
	{
		bn_init(&n, s_stCiHostLicenseConst.HDQ_n, CI_HDQ_N_LEN);
//		bn_init(&d, s_stCiHostLicenseConst.HDQ_d, CI_HDQ_D_LEN);
		bn_init(&p, s_stCiHostLicenseConst.HDQ_p, CI_HDQ_P_LEN);
		bn_init(&q, s_stCiHostLicenseConst.HDQ_q, CI_HDQ_Q_LEN);
		bn_init(&dP, s_stCiHostLicenseConst.HDQ_dP, CI_HDQ_DP_LEN);
		bn_init(&dQ, s_stCiHostLicenseConst.HDQ_dQ, CI_HDQ_DQ_LEN);
		bn_init(&qInv, s_stCiHostLicenseConst.HDQ_qInv, CI_HDQ_QINV_LEN);

		HDQ.n = &n;
		HDQ.d = NULL;
		HDQ.p = &p;
		HDQ.q = &q;
		HDQ.dP = &dP;
		HDQ.dQ = &dQ;
		HDQ.qInv = &qInv;
	}

	buf[0] = CI_CC_TLF_TAG_VER;
	buf[1] = (unsigned char)((CI_CC_TLF_LEN_VER >> 8) & 0xff);
	buf[2] = (unsigned char)(CI_CC_TLF_LEN_VER & 0xff);
	buf[3] = CI_CC_TLF_VER_DEFAULT;

	buf[4] = CI_CC_TLF_TAG_MSG_LABEL;
	buf[5] = (unsigned char)((CI_CC_TLF_LEN_MSG_LABEL >> 8) & 0xff);
	buf[6] = (unsigned char)(CI_CC_TLF_LEN_MSG_LABEL & 0xff);
	buf[7] = CI_CC_TLF_MSG_LABEL_HOST_A;

	buf[8] = CI_CC_TLF_TAG_AUTH_NONCE;
	buf[9] = (unsigned char)((CI_CC_TLF_LEN_AUTH_NONCE >> 8) & 0xff);
	buf[10] = (unsigned char)(CI_CC_TLF_LEN_AUTH_NONCE & 0xff);
	HxSTD_memcpy(&buf[11], hCc->stCicamData.auth_nonce, 32); // �� ������ auth_nonce ���� ��ȿ�� ���̴�.

	buf[43] = CI_CC_TLF_TAG_DHPH;
	buf[44] = (unsigned char)((CI_CC_TLF_LEN_DHPH >> 8) & 0xff);
	buf[45] = (unsigned char)(CI_CC_TLF_LEN_DHPH & 0xff);
	HxSTD_memcpy(&buf[46], s_stCiHostLicenseConst.DHPH, 256);

	bn_init(&M, buf, 302);

	if (A_RSA_PSS(&HDQ, &M, &salt, &S))
	{
		HxLOG_Error("[CICC]---> CiCc_GenerateSignature()---> Error : A_RSA !!!\n");
		return CI_ARC_ERR_Auth_Signature_Failed;
	}

	if (S.len != 256)
	{
		HxLOG_Error("[CICC]---> CiCc_GenerateSignature()---> Error : Invalid len (%d) !!!\n", S.len);
		return CI_ARC_ERR_Auth_Signature_Failed;
	}

	HxSTD_memcpy(pucSignature, S.num, 256);

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
	HxLOG_Info("[CICC]---> CiCc_GenerateSignature() : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
	#endif

	#if 0 // #ifdef _CI_DEBUG_CC_DATA_
	{
		bn_len_type i;
		HxLOG_Print("\n CiCc_GenerateSignature : M (%d)", M.len);
		for (i = 0; i < M.len; i++)
		{
			if ((i & 15) == 0)
				HxLOG_Print("\n");
			HxLOG_Print(" %02x", M.num[i]);
		}
		HxLOG_Print("\n CiCc_GenerateSignature : S (%d)", S.len);
		for (i = 0; i < S.len; i++)
		{
			if ((i & 15) == 0)
				HxLOG_Print("\n");
			HxLOG_Print(" %02x", S.num[i]);
		}
		HxLOG_Print("\n\n");
	}
	#endif

	#ifdef _CI_DEBUG_CC_SELF_CHECKING_
	{
		RSA_public_key HDP = {&n, &bn_e};
		if (!V_RSA_PSS(&HDP, &M, &S))
			HxLOG_Info("[CICC]---> CiCc_GenerateSignature()---> Error : Invalid Signature !!!\n");
	}
	#endif

	return CI_ARC_ERR_None;
}

static SvcCas_CiArcErr_e CiCc_VerifyCamAndSaveInfo(CI_CC_INSTANCE *hCc)
{
	Certificate_t * pstRootCertData = NULL;
	Certificate_t * pstCamBrandCertData = NULL;
	Certificate_t * pstCamDevCertData = NULL;

	unsigned char	ucBindedCamId[8];
	unsigned short	usBindedCamBrandId = 0;
	int nScramblerCapabilities, nNegotiatedCckCipher;
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	int i, ret;

	SvcCas_CiArcErr_e err = CI_ARC_ERR_None;

	/*--- Verify DHPM ---*/
	{
		bignum bn_DHPM, bn_DH_p, bn_DH_q, out;

		#ifdef _CI_DEBUG_CC_FUNC_TIME_
		s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
		#endif

		// The host verifies that the DHPM key received from the CICAM is valid by checking the length and values : if 1 < DHPM < DH_p and DHPM ^ DH_q mod DH_p = 1
		bn_init(&bn_DHPM, hCc->stCicamData.DHPM, 256); // �� ������ DHPM ���� ��ȿ�� ���̴�.
		bn_init(&bn_DH_p, s_stCiHostLicenseConst.DH_p, CI_DH_P_LEN);
		bn_init(&bn_DH_q, s_stCiHostLicenseConst.DH_q, CI_DH_Q_LEN);
		if (bn_cmp(&bn_one, &bn_DHPM) >= 0)
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : DHPM not bigger than 1 !!!\n");
			err = CI_ARC_ERR_Auth_DH_Failed;
			goto EXIT;
		}
		if (bn_cmp(&bn_DHPM, &bn_DH_p) >= 0)
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : DHPM not less than DH_p !!!\n");
			err = CI_ARC_ERR_Auth_DH_Failed;
			goto EXIT;
		}
		bn_mod_exp(&bn_DHPM, &bn_DH_q, &bn_DH_p, &out);
		if (bn_cmp(&out, &bn_one) != 0)
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : DHPM ^ DH_q mod DH_p not 1 !!!\n");
			err = CI_ARC_ERR_Auth_DH_Failed;
			goto EXIT;
		}

		#ifdef _CI_DEBUG_CC_FUNC_TIME_
		s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
		HxLOG_Info("[CICC]---> CiCc_VerifyCamAndSaveInfo() for DHPM : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
		#endif
	}

	/*--- Verify RootCert ---*/
	{
		pstRootCertData = X509_ParseRawCert(s_stCiHostLicenseConst.RootCert, s_stCiHostLicenseConst.RootCertLen);
		if (pstRootCertData == NULL)
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid RootCert !!!\n");
			err = CI_ARC_ERR_Root_Cert;
			goto EXIT;
		}
	}

	/*--- Verify CICAM_BrandCert ---*/
	{
		pstCamBrandCertData = X509_ParseRawCert(hCc->stCicamData.CICAM_BrandCert, hCc->stCicamData.CICAM_BrandCertLen);
		if (pstCamBrandCertData == NULL)
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid BrandCert !!!\n");
			err = CI_ARC_ERR_CAM_Cert_Syntax_Incorrect;
			goto EXIT;
		}

		/* 1) Check version (CI+ implementations shall use X.509 version 3 (02)).
		   2) Check that the Issuer of the brand certificate is identical to the Subject of the root certificate.
		   3) Check that the validity period of the brand certificate includes the current date and time.
		   4) Check that each mandatory extension listed in section 9.3.9 exists and the values are valid.
		      Check that no other extension is marked as critical.
		   5) Verify that the KeyIdentifier in the brand certificate's authority key identifier extension
		      is identical to the KeyIdentifier in the root certificate's subject key identifier extension.
		   6) Verify the certificate's signature by using the RSASSA-PSS verification. */

		ret = VerificateBrandCertificate(pstCamBrandCertData, pstRootCertData);
		if (ret)
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid BrandCert (%d) !!!\n", ret);
			err = (ret == -3) ? CI_ARC_ERR_CAM_Cert_Expired : CI_ARC_ERR_CAM_Cert_Syntax_Incorrect;
			goto EXIT;
		}

		if (FALSE == IsSignatureValid(pstRootCertData, pstCamBrandCertData, hCc->stCicamData.CICAM_BrandCert, hCc->stCicamData.CICAM_BrandCertLen))
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid BrandCert Signature !!!\n");
			err = CI_ARC_ERR_CAM_Cert_Signature_Failed;
			goto EXIT;
		}
	}

	/*--- Verify CICAM_DevCert ---*/
	{
		pstCamDevCertData = X509_ParseRawCert(hCc->stCicamData.CICAM_DevCert, hCc->stCicamData.CICAM_DevCertLen);
		if (pstCamDevCertData == NULL)
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid DevCert !!!\n");
			err = CI_ARC_ERR_CAM_Cert_Syntax_Incorrect;
			goto EXIT;
		}

		/* 1) Check version (CI+ implementations shall use X.509 version 3 (02)).
		   2) Check that the Issuer of the device certificate is identical to the Subject of the brand certificate.
		   3) Check that the validity period of the device certificate includes the current date and time.
		   4) Check that each mandatory extension listed in CI+ spec section 9.3.9 exists and the values are valid.
		      Check that no other extension is marked as critical.
		   5) Verify that the KeyIdentifier in the device certificate's authority key identifier extension
		      is identical to the KeyIdentifier in the brand certificate's subject key identifier extension.
		   6) Verify the certificate's signature by using the RSASSA-PSS verification.
		   7) Verify that the device ID (which is part of the Subject field) contains a valid value. See CI+ spec Annex B and C.
		   8) ucBindedCamId, usBindedCamBrandId, nNegotiatedCckCipher (negotiate with CAM's and s_stCiHostLicenseConst.Host_CckCipher) ���� */

		ret = VerificateDeviceCertificate(pstCamDevCertData, pstCamBrandCertData);
		if (ret)
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid DevCert (%d) !!!\n", ret);
			err = (ret == -3) ? CI_ARC_ERR_CAM_Cert_Expired : CI_ARC_ERR_CAM_Cert_Syntax_Incorrect;
			goto EXIT;
		}

		if (FALSE == IsSignatureValid(pstCamBrandCertData, pstCamDevCertData, hCc->stCicamData.CICAM_DevCert, hCc->stCicamData.CICAM_DevCertLen))
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid DevCert Signature !!!\n");
			err = CI_ARC_ERR_CAM_Cert_Signature_Failed;
			goto EXIT;
		}

		// ucBindedCamId ����
		if (FALSE == GetDeviceID(pstCamDevCertData, ucBindedCamId))
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : GetDeviceID() !!!\n");
			err = CI_ARC_ERR_CAM_Cert_Syntax_Incorrect;
			goto EXIT;
		}

		// usBindedCamBrandId ����
		if (FALSE == GetBrandID(pstCamDevCertData, &usBindedCamBrandId))
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : GetBrandID() !!!\n");
			err = CI_ARC_ERR_CAM_Cert_Syntax_Incorrect;
			goto EXIT;
		}

		// nNegotiatedCckCipher ����
		if (FALSE == GetScramblerCapabilities(pstCamDevCertData, &nScramblerCapabilities))
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : GetScramblerCapabilities() !!!\n");
			err = CI_ARC_ERR_CAM_Cert_Syntax_Incorrect;
			goto EXIT;
		}
		if (nScramblerCapabilities == 0 || nScramblerCapabilities == 1)
		{
			nNegotiatedCckCipher = nScramblerCapabilities & s_stCiHostLicenseConst.Host_CckCipher;
		}
		else // unknown value (reserved for future use) -> CI+ ���� 1.2 ���ؿ��� ���ǵ��� ���� ���� ��쿣 �ϴ� CI_CC_KEY_CIPHER_DES_56_ECB �� �����ϵ��� ����...
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : GetScramblerCapabilities()---> unknown value %d !!!\n", nScramblerCapabilities);
			nNegotiatedCckCipher = CI_CC_KEY_CIPHER_DES_56_ECB;
		}
	}

	/*--- Verify Signature_B ---*/
	{
		unsigned char buf[561];
		RSA_public_key MDP;
		bignum n, M, S;

		#ifdef _CI_DEBUG_CC_FUNC_TIME_
		s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
		#endif

		if (FALSE == GetRSAPublicKey_N(pstCamDevCertData, n.num, &n.len))
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : GetRSAPublicKey_N() !!!\n");
			err = CI_ARC_ERR_Auth_Signature_Failed;
			goto EXIT;
		}

		MDP.n = &n;
		MDP.e = &bn_e;

		buf[0] = CI_CC_TLF_TAG_VER;
		buf[1] = (unsigned char)((CI_CC_TLF_LEN_VER >> 8) & 0xff);
		buf[2] = (unsigned char)(CI_CC_TLF_LEN_VER & 0xff);
		buf[3] = CI_CC_TLF_VER_DEFAULT;

		buf[4] = CI_CC_TLF_TAG_MSG_LABEL;
		buf[5] = (unsigned char)((CI_CC_TLF_LEN_MSG_LABEL >> 8) & 0xff);
		buf[6] = (unsigned char)(CI_CC_TLF_LEN_MSG_LABEL & 0xff);
		buf[7] = CI_CC_TLF_MSG_LABEL_HOST_V;

		buf[8] = CI_CC_TLF_TAG_AUTH_NONCE;
		buf[9] = (unsigned char)((CI_CC_TLF_LEN_AUTH_NONCE >> 8) & 0xff);
		buf[10] = (unsigned char)(CI_CC_TLF_LEN_AUTH_NONCE & 0xff);
		HxSTD_memcpy(&buf[11], hCc->stCicamData.auth_nonce, 32); // �� ������ auth_nonce ���� ��ȿ�� ���̴�.

		buf[43] = CI_CC_TLF_TAG_DHPH;
		buf[44] = (unsigned char)((CI_CC_TLF_LEN_DHPH >> 8) & 0xff);
		buf[45] = (unsigned char)(CI_CC_TLF_LEN_DHPH & 0xff);
		HxSTD_memcpy(&buf[46], s_stCiHostLicenseConst.DHPH, 256);

		buf[302] = CI_CC_TLF_TAG_DHPM;
		buf[303] = (unsigned char)((CI_CC_TLF_LEN_DHPM >> 8) & 0xff);
		buf[304] = (unsigned char)(CI_CC_TLF_LEN_DHPM & 0xff);
		HxSTD_memcpy(&buf[305], hCc->stCicamData.DHPM, 256); // �� ������ DHPM ���� ��ȿ�� ���̴�.

		bn_init(&M, buf, 561);
		bn_init(&S, hCc->stCicamData.Signature_B, 256); // �� ������ Signature_B ���� ��ȿ�� ���̴�.

		if (!V_RSA_PSS(&MDP, &M, &S))
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid Signature !!!\n");
			err = CI_ARC_ERR_Auth_Signature_Failed;
			goto EXIT;
		}

		#ifdef _CI_DEBUG_CC_FUNC_TIME_
		s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
		HxLOG_Info("[CICC]---> CiCc_VerifyCamAndSaveInfo() for Signature_B : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
		#endif
	}

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
{
	/*	full authentication ������ ��ġ�� ���� ù��° slot�� �����ϵ��� �Ѵ�.
		ù��° solt�� ��� ���̸� �ٷ� ���� slot�� �����ϵ��� �Ѵ�.
		������ �༮���� �� ĭ�� �и��� ���� ������ slot�� �ִ� �༮�� ���� �ȴ�.
	*/
	int		nSelectIdx = 0;
	HBOOL	bIsUsed;

	bIsUsed = CiCc_IsUsedAuthContextIndex(nSelectIdx);
	if(bIsUsed == TRUE)
	{
		nSelectIdx++;
	}

	for(i = CI_NUM_OF_AUTH_CTX - 1; i > 0; i--)
	{
		int nMoveIdx;

		pstCiAuthCtx = CiCc_GetAuthContext(i-1);
		if(pstCiAuthCtx == NULL)
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Can't load context !!!\n");
			err = CI_ARC_ERR_Not_Defined;
			goto EXIT;
		}

		if (pstCiAuthCtx->ucSavedFlag != 2)
		{
			continue;
		}

		bIsUsed = CiCc_IsUsedAuthContextIndex(i-1);
		if(bIsUsed == TRUE)
		{
			continue;
		}

		bIsUsed = CiCc_IsUsedAuthContextIndex(i);
		if(bIsUsed == TRUE)
		{
			nMoveIdx = i+1;
		}
		else
		{
			nMoveIdx = i;
		}

		if(nMoveIdx > CI_NUM_OF_AUTH_CTX - 1)
		{
			continue;
		}

		if (CiCc_PutAuthContext(nMoveIdx, pstCiAuthCtx))
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Can't shift context !!!\n");
			err = CI_ARC_ERR_Not_Defined;
			goto EXIT;
		}

	}

	if ((pstCiAuthCtx = CiCc_GetAuthContext(nSelectIdx)) == NULL)
	{
		HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Can't load context !!!\n");
		err = CI_ARC_ERR_Not_Defined;
		goto EXIT;
	}
	HxSTD_memset(pstCiAuthCtx, 0, sizeof(CI_AUTH_CTX));
	HxSTD_memcpy(pstCiAuthCtx->ucBindedCamId, ucBindedCamId, 8);
	pstCiAuthCtx->usBindedCamBrandId = usBindedCamBrandId;
	pstCiAuthCtx->nNegotiatedCckCipher = nNegotiatedCckCipher;
	pstCiAuthCtx->ucSavedFlag = 1;

	//hCc->bUsedCurAuthCtxIdx = TRUE;
	hCc->nCurAuthCtxIdx = hCc->nTryAuthCtxIdx = nSelectIdx;
}
#else
	/*--- save ucBindedCamId, usBindedCamBrandId and nNegotiatedCckCipher to non-volatile memory (shift saved authentication contexts and save new one to first) ---*/
	for (i = CI_NUM_OF_AUTH_CTX - 1; i > 0; i--)
	{
		if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, i-1)) == NULL)
		{
			HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Can't load context !!!\n");
			err = CI_ARC_ERR_Not_Defined;
			goto EXIT;
		}
		// �Ʒ� �ּ� �ڵ�� ���� ucBindedCamId �� üũ�ϴ� ���� �°�����
		// SmarDTV CI+ �׽�Ʈ Ŷ ķ 2���� ��� ���� ����̽� �������� ����ϹǷ� ucBindedCamId �� üũ�ϰ� �Ǹ� ķ�� �ٲ� ������ �ٽ� ���ε��ؾ� �Ѵ�.
		// ���� �ϴ� ucBindedCamId �� üũ���� �ʵ��� �Ѵ�...
		if (pstCiAuthCtx->ucSavedFlag == 2) // if ((pstCiAuthCtx->ucSavedFlag == 2) && (HxSTD_memcmp(pstCiAuthCtx->ucBindedCamId, ucBindedCamId, 8) != 0))
		{
			if (CiCc_PutAuthContext(hCc->usSlotId, i, pstCiAuthCtx))
			{
				HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Can't shift context !!!\n");
				err = CI_ARC_ERR_Not_Defined;
				goto EXIT;
			}
		}
	}

	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, 0)) == NULL)
	{
		HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Can't load first context !!!\n");
		err = CI_ARC_ERR_Not_Defined;
		goto EXIT;
	}
	HxSTD_memset(pstCiAuthCtx, 0, sizeof(CI_AUTH_CTX));
	HxSTD_memcpy(pstCiAuthCtx->ucBindedCamId, ucBindedCamId, 8);
	pstCiAuthCtx->usBindedCamBrandId = usBindedCamBrandId;
	pstCiAuthCtx->nNegotiatedCckCipher = nNegotiatedCckCipher;
	pstCiAuthCtx->ucSavedFlag = 1;

	hCc->nCurAuthCtxIdx = hCc->nTryAuthCtxIdx = 0;
#endif

//	���⼱ ������ �ʿ䰡 ����...
//	if (CI_SaveAuthContexts(hCc->usSlotId, &s_stCiAuthCtxs[hCc->usSlotId]))
//	{
//		HxLOG_Error("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Can't save context !!!\n");
//		err = CI_ARC_ERR_Not_Defined;
//		goto EXIT;
//	}

EXIT :

	if (pstRootCertData)
		X509_FreeCert(pstRootCertData);
	if (pstCamBrandCertData)
		X509_FreeCert(pstCamBrandCertData);
	if (pstCamDevCertData)
		X509_FreeCert(pstCamDevCertData);

	return err;
}

/**************************************************/
/*** Local functions for host non-volatile keys ***/
/**************************************************/

static int CiCc_LoadAkh(CI_CC_INSTANCE *hCc, unsigned char *pucAkh)
{
	int				i;
	HBOOL			bIsUsedAuthContext = TRUE;
	CI_AUTH_CTX * pstCiAuthCtx = NULL;


	for (i = hCc->nTryAuthCtxIdx; i < CI_NUM_OF_AUTH_CTX; i++)
	{
	#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
		if ((pstCiAuthCtx = CiCc_GetAuthContext(i)) == NULL)
	#else
		if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, i)) == NULL)
	#endif
		{
			HxLOG_Error("[CICC]---> CiCc_LoadAkh()---> Error : Can't load context !!!\n"); // �̷� ��Ȳ�� �߻��ϸ� ���. ����� �ʿ�!!
			return -1;
		}

	#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
		bIsUsedAuthContext = CiCc_IsUsedAuthContextIndex(i);
		if(bIsUsedAuthContext == TRUE)
		{
			continue;
		}
	#endif

		if (pstCiAuthCtx->ucSavedFlag >= 1)
		{
			break;
		}
	}

	HxLOG_Info("[CICC]---> CiCc_LoadAkh()---> loaded index(%d) !!!\n", i);

	if (i == CI_NUM_OF_AUTH_CTX)
	{
		/* <TODO_CI_PLUS> : 2 ���� ���� ��� ������ ���ε��Ǿ� �ִ� ķ�� �ٸ� �������� ������ ��츦 ����� �ʿ䰡 �ִ�...
		                    ���� �ش� ���Կ� ����� authentication contexts�� CI_NUM_OF_AUTH_CTX �� �̸��̶�� hCc->usSlotId^0x1 �� �õ��� �ʿ䰡 �ִ�... */
		HxLOG_Error("[CICC]---> CiCc_LoadAkh()---> Error : Can't load AKH or AKH not saved !!!\n");
		return -1;
	}

	hCc->nCurAuthCtxIdx = i;
	hCc->nTryAuthCtxIdx = i+1;

	HxSTD_memcpy(pucAkh, pstCiAuthCtx->AKH, 32);

	return ERR_OK;
}

static SvcCas_CiArcErr_e CiCc_GenerateHostNonVolatileKeys(CI_CC_INSTANCE *hCc) /* ���⼭ DHSK�� ���� DHSK�� LSB 128 bits �� ���� */
{
	bignum bn_DHPM, bn_DHH, bn_DH_p, bn_DHSK;
	unsigned char AKH[32];
	unsigned char M[8+8+256];
	CI_AUTH_CTX * pstCiAuthCtx = NULL;

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
	#endif

	// compute DHSK
	bn_init(&bn_DHPM, hCc->stCicamData.DHPM, 256); // �� ������ DHPM ���� ��ȿ�� ���̴�.
	bn_init(&bn_DHH, s_stCiHostLicenseConst.DHH, 256);
	bn_init(&bn_DH_p, s_stCiHostLicenseConst.DH_p, CI_DH_P_LEN);
	bn_mod_exp(&bn_DHPM, &bn_DHH, &bn_DH_p, &bn_DHSK);
	if (bn_DHSK.len != 256)
	{
		HxLOG_Error("[CICC]---> CiCc_GenerateHostNonVolatileKeys()---> Error : Invalid DHSK len (%d) !!!\n", bn_DHSK.len);
		return CI_ARC_ERR_Auth_Key_Computation_Failed;
	}

	// load context
#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
#else
	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
#endif
	{
		HxLOG_Error("[CICC]---> CiCc_GenerateHostNonVolatileKeys()---> Error : Can't load context !!!\n");
		return CI_ARC_ERR_Auth_Key_Computation_Failed;
	}
	if (pstCiAuthCtx->ucSavedFlag != 1)
	{
		HxLOG_Error("[CICC]---> CiCc_GenerateHostNonVolatileKeys()---> Error : authentication context not saved !!!\n");
		return CI_ARC_ERR_Auth_Key_Computation_Failed;
	}

	// compute AKH
	HxSTD_memcpy(M, pstCiAuthCtx->ucBindedCamId, 8); // �� ������ ucBindedCamId ���� ��ȿ�� ���̴�.
	HxSTD_memcpy(&M[8], s_stCiHostLicenseConst.Host_ID, 8);
	HxSTD_memcpy(&M[16], bn_DHSK.num, bn_DHSK.len);
	SHA_256(M, 8+8+bn_DHSK.len, AKH);

	// save DHSK' and AKH to non-volatile memory
	HxSTD_memcpy(pstCiAuthCtx->DHSK, &bn_DHSK.num[bn_DHSK.len-16], 16);
	HxSTD_memcpy(pstCiAuthCtx->AKH, AKH, 32);
	pstCiAuthCtx->ucSavedFlag = 2;

	#if defined(CONFIG_DEBUG)
	{
		HUINT32 		i;

		HxLOG_Info("(%d) AKH = [", hCc->nCurAuthCtxIdx);
		for(i = 0; i < 32; i++) HxLOG_Print(" %0x", pstCiAuthCtx->AKH[i]);
		HxLOG_Print("]\n");
	}
	#endif

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	if (CI_SaveAuthContexts())
#else
	if (CI_SaveAuthContexts(hCc->usSlotId, &s_stCiAuthCtxs[hCc->usSlotId]))
#endif
	{
		HxLOG_Error("[CICC]---> CiCc_GenerateHostNonVolatileKeys()---> Error : Can't save DHSK and AKH !!!\n");
		return CI_ARC_ERR_Auth_Key_Computation_Failed;
	}

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
	HxLOG_Info("[CICC]---> CiCc_GenerateHostNonVolatileKeys() : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
	#endif

	return CI_ARC_ERR_None;
}

/********************************************************/
/*** Local functions for calculating keys (SAC / CCK) ***/
/********************************************************/

/* calculating the new SAC key material - SEK for de/en-cryption and SAK for authentication or uri_confirm (host side) */
#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
static SvcCas_CiArcErr_e CiCc_CalculateSacKeysOnSubTask(CI_CC_INSTANCE *hCc)
{
	CI_CC_SUB_TASK_MSG msg;

	#ifdef _CI_DEBUG_CC_TIME_ISSUE_
	s_bCalculateSacDone = 0;
	#endif

	msg.command = CI_CC_DO_CALCULATE_SAC;
	msg.hCc = hCc;

	if (VK_MSG_Send(s_ulCiCcSubMsgQId, &msg, sizeof(CI_CC_SUB_TASK_MSG)))
	{
		HxLOG_Error("[CICC]---> CiCc_CalculateSacKeysOnSubTask()---> Error : fail to send msg !!!\n");
		return CI_ARC_ERR_SAC_Failed;
	}

	return CI_ARC_ERR_None;
}
#endif
static SvcCas_CiArcErr_e CiCc_CalculateSacKeys(CI_CC_INSTANCE *hCc)
{
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	unsigned char M[16+32+8+8];
	unsigned char Ks_Host[32];
	unsigned char Km[32];
	int i;

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
	#endif

	// load context
#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
#else
	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
#endif
	{
		HxLOG_Error("[CICC]---> CiCc_CalculateSacKeys()---> Error : Can't load context !!!\n");
		return CI_ARC_ERR_SAC_Failed;
	}
	if (pstCiAuthCtx->ucSavedFlag != 2)
	{
		HxLOG_Error("[CICC]---> CiCc_CalculateSacKeys()---> Error : authentication context not saved !!!\n");
		return CI_ARC_ERR_SAC_Failed;
	}

	// Key seed calculation : compute Ks_Host
	HxSTD_memcpy(M, pstCiAuthCtx->DHSK, 16); // �� ������ DHSK ���� ��ȿ�� ���̴�.
	HxSTD_memcpy(&M[16], pstCiAuthCtx->AKH, 32); // �� ������ AKH ���� ��ȿ�� ���̴�.
	HxSTD_memcpy(&M[48], hCc->stHostData.Ns_Host, 8); // �� ������ Ns_Host ���� ��ȿ�� ���̴�.
	HxSTD_memcpy(&M[56], hCc->stCicamData.Ns_module, 8); // �� ������ Ns_module ���� ��ȿ�� ���̴�.
	SHA_256(M, 16+32+8+8, Ks_Host);

	// Key material computation : compute Km
	if (s_nCiHostLicenseConstType) { // #if defined(CI_PLUS_TEST_Technology_from_TrustCenter) || defined(CI_PLUS_TEST_Technology_from_SmarDTV)
		SHA_256(Ks_Host, 32, Km);
		for (i = 0; i < 16; i++)
			Km[i] ^= s_stCiHostLicenseConst.SLK[i];
		for (i = 0; i < 16; i++)
			Km[16+i] ^= s_stCiHostLicenseConst.SLK[i];
	} else { // #else // CI_PLUS_PRODUCTION_Credentials : CI Plus License Specification ���� AES-128-CBC ��� �Ǿ� ������ IV �� ���� ��ǻ� AES-128-ECB ��� �Ǵܵȴ� -> v1.6r2 ���ؿ��� ECB �´�.
		E_AES_ECB(s_stCiHostLicenseConst.SLK, Ks_Host, Km);
		E_AES_ECB(s_stCiHostLicenseConst.SLK, &Ks_Host[16], &Km[16]);
		for (i = 0; i < 32; i++)
			Km[i] ^= Ks_Host[i];
	} // #endif

	// SEK and SAK key derivation : compute SEK, SAK
	HxSTD_memcpy(hCc->stHostData.SEK, Km, 16); // keep SEK for further use
	HxSTD_memcpy(hCc->stHostData.SAK, &Km[16], 16); // keep SAK for further use

	// UCK
	SHA_256(hCc->stHostData.SAK, 16, hCc->stHostData.UCK);

	#if defined(USE_SUB_TASK_TO_CALCULATE_KEYS) && defined(_CI_DEBUG_CC_TIME_ISSUE_)
	s_bCalculateSacDone = 1;
	#endif

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
	HxLOG_Info("[CICC]---> CiCc_CalculateSacKeys() : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
	#endif

	return CI_ARC_ERR_None;
}

/* calculating the new CC key (host side) and set descrambler */
#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
static SvcCas_CiArcErr_e CiCc_CalculateCckAndSetDescramblerOnSubTask(CI_CC_INSTANCE *hCc, HBOOL bDoCalculateCck, HBOOL bDoSetDescrambler)
{
	CI_CC_SUB_TASK_MSG msg;

	#ifdef _CI_DEBUG_CC_TIME_ISSUE_
	if (bDoCalculateCck)
		s_bCalculateCckDone = 0;
	#endif

	msg.command = CI_CC_DO_CALCULATE_CCK;
	msg.hCc = hCc;
	msg.bDoCalculateCck = bDoCalculateCck;
	msg.bDoSetDescrambler = bDoSetDescrambler;

	if (VK_MSG_Send(s_ulCiCcSubMsgQId, &msg, sizeof(CI_CC_SUB_TASK_MSG)))
	{
		HxLOG_Error("[CICC]---> CiCc_CalculateCckAndSetDescramblerOnSubTask()---> Error : fail to send msg !!!\n");
		return CI_ARC_ERR_CCK_Failed;
	}

	return CI_ARC_ERR_None;
}
#endif
static SvcCas_CiArcErr_e CiCc_CalculateCckAndSetDescrambler(CI_CC_INSTANCE *hCc, HBOOL bDoCalculateCck, HBOOL bDoSetDescrambler)
{
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	unsigned char Km[32];
	unsigned char * CCK = hCc->stHostData.CCK;
	unsigned char * CIV = hCc->stHostData.CIV;
	int i, j, cnt;

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
	#endif

	// load context
#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
#else
	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
#endif
	{
		HxLOG_Error("[CICC]---> CiCc_CalculateCckAndSetDescrambler()---> Error : Can't load context !!!\n");
		return CI_ARC_ERR_CCK_Failed;
	}
	if (pstCiAuthCtx->ucSavedFlag != 2)
	{
		HxLOG_Error("[CICC]---> CiCc_CalculateCckAndSetDescrambler()---> Error : authentication context not saved !!!\n");
		return CI_ARC_ERR_CCK_Failed;
	}

	if (pstCiAuthCtx->nNegotiatedCckCipher == CI_CC_KEY_CIPHER_DES_56_ECB) // �� ������ nNegotiatedCckCipher ���� ��ȿ�� ���̴�
	{
		if (bDoCalculateCck)
		{
			// Key precursor calculation : Kp shall be calculated on the CICAM, so we can just use hCc->stCicamData.Kp[32] (�� ������ Kp ���� ��ȿ�� ���̴�)

			// Key material computation : compute Km
			if (s_nCiHostLicenseConstType) { // #if defined(CI_PLUS_TEST_Technology_from_TrustCenter) || defined(CI_PLUS_TEST_Technology_from_SmarDTV)
				SHA_256(hCc->stCicamData.Kp, 32, Km);
				for (i = 0; i < 16; i++)
					Km[i] ^= s_stCiHostLicenseConst.CLK[i];
				for (i = 0; i < 16; i++)
					Km[16+i] ^= s_stCiHostLicenseConst.CLK[i];
			} else { // #else // CI_PLUS_PRODUCTION_Credentials : CI Plus License Specification ���� AES-128-CBC ��� �Ǿ� ������ IV �� ���� ��ǻ� AES-128-ECB ��� �Ǵܵȴ� -> v1.6r2 ���ؿ��� ECB �´�.
				E_AES_ECB(s_stCiHostLicenseConst.CLK, hCc->stCicamData.Kp, Km);
				E_AES_ECB(s_stCiHostLicenseConst.CLK, &hCc->stCicamData.Kp[16], &Km[16]);
				for (i = 0; i < 32; i++)
					Km[i] ^= hCc->stCicamData.Kp[i];
			} // #endif

			// CCK and CIV key derivation : compute CCK, CIV
			CCK[0] = Km[0];
			CCK[1] = (Km[0] << 7) | (Km[1] >> 1);
			CCK[2] = (Km[1] << 6) | (Km[2] >> 2);
			CCK[3] = (Km[2] << 5) | (Km[3] >> 3);
			CCK[4] = (Km[3] << 4) | (Km[4] >> 4);
			CCK[5] = (Km[4] << 3) | (Km[5] >> 5);
			CCK[6] = (Km[5] << 2) | (Km[6] >> 6);
			CCK[7] = Km[6] << 1;
			for (i = 0; i < 8; i++)
			{
				cnt = 0;
				for (j = 1; j < 8; j++)
				{
					if (CCK[i] >> j & 0x1) cnt++;
				}
				if (cnt % 2)
					CCK[i] &= 0xFE;
				else
					CCK[i] |= 0x01;
			}
		}

		if (bDoSetDescrambler)
		{
			if (CI_SetCckToDsc(hCc->usSlotId, CI_CC_KEY_CIPHER_DES_56_ECB, hCc->stCicamData.enKeyRegister, CCK, NULL)) // �� ������ enKeyRegister ���� ��ȿ�� ���̴�
			{
				HxLOG_Error("[CICC]---> CiCc_CalculateCckAndSetDescrambler()---> Error : Can't set descrambler for DES !!!\n");
//				��ũ���� �ʿ� ������ �־ �Ͻ������� ������ ���� ����� ���� �������� �ʰ� �ϴ°� ���� ������... �ϴ� ������ ����ְ� ���� ó���� ���� �ʵ��� �Ѵ�.
//				return CI_ARC_ERR_CCK_Failed;
			}
		}
 	}
	else
	{
		if (bDoCalculateCck)
		{
			// Key precursor calculation : Kp shall be calculated on the CICAM, so we can just use hCc->stCicamData.Kp[32] (�� ������ Kp ���� ��ȿ�� ���̴�)

			// Key material computation : compute Km
			if (s_nCiHostLicenseConstType) { // #if defined(CI_PLUS_TEST_Technology_from_TrustCenter) || defined(CI_PLUS_TEST_Technology_from_SmarDTV)
				SHA_256(hCc->stCicamData.Kp, 32, Km);
				for (i = 0; i < 16; i++)
					Km[i] ^= s_stCiHostLicenseConst.CLK[i];
				for (i = 0; i < 16; i++)
					Km[16+i] ^= s_stCiHostLicenseConst.CLK[i];
			} else { // #else // CI_PLUS_PRODUCTION_Credentials : CI Plus License Specification ���� AES-128-CBC ��� �Ǿ� ������ IV �� ���� ��ǻ� AES-128-ECB ��� �Ǵܵȴ� -> v1.6r2 ���ؿ��� ECB �´�.
				E_AES_ECB(s_stCiHostLicenseConst.CLK, hCc->stCicamData.Kp, Km);
				E_AES_ECB(s_stCiHostLicenseConst.CLK, &hCc->stCicamData.Kp[16], &Km[16]);
				for (i = 0; i < 32; i++)
					Km[i] ^= hCc->stCicamData.Kp[i];
			} // #endif

			// CCK and CIV key derivation : compute CCK, CIV
			HxSTD_memcpy(CCK, Km, 16);
			HxSTD_memcpy(CIV, &Km[16], 16);
		}

		if (bDoSetDescrambler)
		{
			if (CI_SetCckToDsc(hCc->usSlotId, CI_CC_KEY_CIPHER_AES_128_CBC, hCc->stCicamData.enKeyRegister, CCK, CIV)) // �� ������ enKeyRegister ���� ��ȿ�� ���̴�
			{
				HxLOG_Error("[CICC]---> CiCc_CalculateCckAndSetDescrambler()---> Error : Can't set descrambler for AES !!!\n");
//				��ũ���� �ʿ� ������ �־ �Ͻ������� ������ ���� ����� ���� �������� �ʰ� �ϴ°� ���� ������... �ϴ� ������ ����ְ� ���� ó���� ���� �ʵ��� �Ѵ�.
//				return CI_ARC_ERR_CCK_Failed;
			}
		}
	}

	#if defined(USE_SUB_TASK_TO_CALCULATE_KEYS) && defined(_CI_DEBUG_CC_TIME_ISSUE_)
	if (bDoCalculateCck)
		s_bCalculateCckDone = 1;
	#endif

	#ifdef _CI_DEBUG_CC_DATA_
	HxLOG_Info("\n - CiCc_%s_&_%s() - \n", bDoCalculateCck ? "DoCalculateCck" : "DontCalculateCck", bDoSetDescrambler ? "DoSetDescrambler" : "DontSetDescrambler");
	#if 0
	if (bDoCalculateCck)
	{
		if (pstCiAuthCtx->nNegotiatedCckCipher == CI_CC_KEY_CIPHER_DES_56_ECB)
		{
			HxLOG_Print("\t CCK = %02x %02x %02x %02x %02x %02x %02x %02x \n",
				CCK[0], CCK[1], CCK[2], CCK[3], CCK[4], CCK[5], CCK[6], CCK[7]);
		}
		else
		{
			HxLOG_Print("\t CCK = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
				CCK[0], CCK[1], CCK[2], CCK[3], CCK[4], CCK[5], CCK[6], CCK[7], CCK[8], CCK[9], CCK[10], CCK[11], CCK[12], CCK[13], CCK[14], CCK[15]);
			HxLOG_Print("\t CIV = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
				CIV[0], CIV[1], CIV[2], CIV[3], CIV[4], CIV[5], CIV[6], CIV[7], CIV[8], CIV[9], CIV[10], CIV[11], CIV[12], CIV[13], CIV[14], CIV[15]);
		}
	}
	#endif
	if (bDoSetDescrambler)
	{
		HxLOG_Info("\t nNegotiatedCckCipher = %s \n", pstCiAuthCtx->nNegotiatedCckCipher == CI_CC_KEY_CIPHER_DES_56_ECB ? "DES" : "AES");
		HxLOG_Info("\t enKeyRegister = %s \n", hCc->stCicamData.enKeyRegister == CI_CC_KEY_MODE_EVEN ? "even" : "odd");
	}
	HxLOG_Info("\n");
	#endif

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
	HxLOG_Info("[CICC]---> CiCc_CalculateCckAndSetDescrambler() : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
	#endif

	return CI_ARC_ERR_None;
}

/*******************************/
/*** Local functions for URI ***/
/*******************************/
static void CiCc_GetMinutesForRL(unsigned char ucRL, unsigned long *pulMinutesForRL)
{
	switch (ucRL)
	{
		case CI_CC_URI_RL_UNLIMITED:	// Unlimited retention period (added from CI+ v1.3)
			*pulMinutesForRL = 0;
			break;
		case CI_CC_URI_RL_DEFAULT:								// Default retention limit of 90 minutes applies
			*pulMinutesForRL = 90;
			break;
		case 0x01:												// Retention limit of 6 hours applies
			*pulMinutesForRL = 6 * 60;
			break;
		case 0x02:												// Retention limit of 12 hours applies
			*pulMinutesForRL = 12 * 60;
			break;
		default:							// Retention limit of 1-252 multiples of 24 Hrs applies as signalled by bits (0x03~0x3F : 1~16 -> 0x03~0xFE : 1~252 for CI+ v1.3)
			*pulMinutesForRL = (ucRL - 0x02) * 24 * 60;
			break;
	}
}

static void CiCc_GenerateUriConfirm(CI_CC_INSTANCE *hCc, unsigned char *pucUriConfirm)
{
	unsigned char M[8+32];

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
	#endif

	HxSTD_memcpy(M, hCc->stCicamData.uri_message, 8); // �� ������ uri_message ���� ��ȿ�� ���̴�.
	HxSTD_memcpy(&M[8], hCc->stHostData.UCK, 32); // �� ������ UCK ���� ��ȿ�� ���̴�.

	SHA_256(M, 8+32, pucUriConfirm);

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
	HxLOG_Info("[CICC]---> CiCc_GenerateUriConfirm() : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
	#endif
}

static void CiCc_UriDoCopyCtrl(CI_CC_INSTANCE *hCc, unsigned char ucMode)
{
	CIPLUS_UriInfo_t			stUriInfo;

	hCc->stCicamData.stUri.ucMode = ucMode;

	if (ucMode == CI_CC_URI_MODE_DEFAULT)
	{
		// set URI default values and do control external interface with them
		hCc->stCicamData.stUri.ucApsCopyCtrl = CI_CC_URI_APS_DEFAULT;
		hCc->stCicamData.stUri.ucEmiCopyCtrl = CI_CC_URI_EMI_DEFAULT;
		hCc->stCicamData.stUri.ucIctCopyCtrl = CI_CC_URI_ICT_DEFAULT;
		hCc->stCicamData.stUri.ucRctCopyCtrl = CI_CC_URI_RCT_DEFAULT;
		hCc->stCicamData.stUri.ucDotCopyCtrl = CI_CC_URI_DOT_DEFAULT;
		hCc->stCicamData.stUri.ucRlCopyCtrl = CI_CC_URI_RL_DEFAULT;
	}
	else if (ucMode == CI_CC_URI_MODE_TIMEOUT)
	{
		// set URI timeout values and do control external interface with them
		hCc->stCicamData.stUri.ucApsCopyCtrl = CI_CC_URI_APS_DEFAULT;
		hCc->stCicamData.stUri.ucEmiCopyCtrl = CI_CC_URI_EMI_DEFAULT;
		hCc->stCicamData.stUri.ucIctCopyCtrl = CI_CC_URI_ICT_TIMEOUT;
		hCc->stCicamData.stUri.ucRctCopyCtrl = CI_CC_URI_RCT_DEFAULT;
		hCc->stCicamData.stUri.ucDotCopyCtrl = CI_CC_URI_DOT_DEFAULT;
		hCc->stCicamData.stUri.ucRlCopyCtrl = CI_CC_URI_RL_UNLIMITED;
	}
	else // if (ucMode == CI_CC_URI_MODE_FROMCAM)
	{
		#if 0 // #ifdef _CI_DEBUG_CC_DATA_ // CI mgr ���� ��� �ֹǷ� ���⼭�� ������ �Ѵ�.
		VK_Print("\n - CiCc_UriDoCopyCtrl() - \n");
		VK_Print("\t stCicamData.usProgramNumber = 0x%x (%d) \n", hCc->stCicamData.usProgramNumber, hCc->stCicamData.usProgramNumber);
		VK_Print("\t stCicamData.uri_message = %02X %02X %02X %02X %02X %02X %02X %02X \n",
			hCc->stCicamData.uri_message[0], hCc->stCicamData.uri_message[1], hCc->stCicamData.uri_message[2], hCc->stCicamData.uri_message[3],
			hCc->stCicamData.uri_message[4], hCc->stCicamData.uri_message[5], hCc->stCicamData.uri_message[6], hCc->stCicamData.uri_message[7]);
		VK_Print("\t APS (%x), EMI (%x), ICT (%x), RCT (%x), RL (%x) \n\n",
			hCc->stCicamData.stUri.ucApsCopyCtrl,
			hCc->stCicamData.stUri.ucEmiCopyCtrl,
			hCc->stCicamData.stUri.ucIctCopyCtrl,
			hCc->stCicamData.stUri.ucRctCopyCtrl,
			hCc->stCicamData.stUri.ucRlCopyCtrl);
	#endif

		if (hCc->enArcErr == CI_ARC_ERR_uri_message || hCc->enArcErr == CI_ARC_ERR_program_number)
		{
			HxLOG_Info("[CICC]---> CiCc_UriDoCopyCtrl()---> Error : enArcErr %d !!!\n", hCc->enArcErr);
			return;
		}
	}

	// �� CopyCtrl �� �ش��ϴ� �۾��� �����϶� (apply URI to external interface)

	#if 0 // ���⼱ ���� Ȯ���ϴ� �� �ۿ� �� ���� ���µ� �Ʒ� ���� ��Ȳ�� �ٺ������� �߻��� ������ �����Ƿ� ���´�.
	switch (hCc->stCicamData.stUri.ucApsCopyCtrl)
	{
		case CI_CC_URI_APS_OFF:
			break;
		case CI_CC_URI_APS_ON_1:
			break;
		case CI_CC_URI_APS_ON_2:
			break;
		case CI_CC_URI_APS_ON_3:
			break;
		default:
			PrintErr(("[CICC]---> CiCc_UriDoCopyCtrl()---> Error : Unknown ApsCopyCtrl[0x%x] !!!\n", hCc->stCicamData.stUri.ucApsCopyCtrl));
			return;
	}
	#endif

	#if 0 // ���⼱ ���� Ȯ���ϴ� �� �ۿ� �� ���� ���µ� �Ʒ� ���� ��Ȳ�� �ٺ������� �߻��� ������ �����Ƿ� ���´�.
	switch (hCc->stCicamData.stUri.ucEmiCopyCtrl)
	{
		case CI_CC_URI_EMI_COPY_FREE:
			break;
		case CI_CC_URI_EMI_COPY_NO_MORE:
			break;
		case CI_CC_URI_EMI_COPY_ONCE:
			break;
		case CI_CC_URI_EMI_COPY_NEVER:
			break;
		default:
			PrintErr(("[CICC]---> CiCc_UriDoCopyCtrl()---> Error : Unknown EmiCopyCtrl[0x%x] !!!\n", hCc->stCicamData.stUri.ucEmiCopyCtrl));
			return;
	}
	#endif

	#if 0 // ���⼱ ���� Ȯ���ϴ� �� �ۿ� �ǹ̰� �����Ƿ� ���´�.
	switch (hCc->stCicamData.stUri.ucIctCopyCtrl)
	{
		case CI_CC_URI_ICT_OFF:
			break;
		case CI_CC_URI_ICT_ON:
			break;
		default:
			HxLOG_Error("[CICC]---> CiCc_UriDoCopyCtrl()---> Error : Unknown IctCopyCtrl[0x%x] !!!\n", hCc->stCicamData.stUri.ucIctCopyCtrl);
			return;
	}
	#endif

	#if 0 // ���⼱ ���� Ȯ���ϴ� �� �ۿ� �� ���� ���µ� �Ʒ� ���� ��Ȳ�� �ٺ������� �߻��� ������ �����Ƿ� ���´�.
	switch (hCc->stCicamData.stUri.ucRctCopyCtrl)
	{
		case CI_CC_URI_RCT_OFF:
			break;
		case CI_CC_URI_RCT_ON:
			break;
		default:
			HxLOG_Error("[CICC]---> CiCc_UriDoCopyCtrl()---> Error : Unknown ucRctCopyCtrl[0x%x] !!!\n", hCc->stCicamData.stUri.ucRctCopyCtrl);
			return;
	}
	#endif

	#if 0 // ���⼱ ���� Ȯ���ϴ� �� �ۿ� �� ���� ���µ� �Ʒ� ���� ��Ȳ�� �ٺ������� �߻��� ������ �����Ƿ� ���´�.
	switch (hCc->stCicamData.stUri.ucDotCopyCtrl)
	{
		case CI_CC_URI_DOT_OFF:
			break;
		case CI_CC_URI_DOT_ON:
			break;
		default:
			PrintErr(("[CICC]---> CiCc_UriDoCopyCtrl()---> Error : Unknown ucDotCopyCtrl[0x%x] !!!\n", hCc->stCicamData.stUri.ucDotCopyCtrl));
			return;
	}
	#endif

	stUriInfo.usServiceId = hCc->stCicamData.usProgramNumber; // ucMode �� CI_CC_URI_MODE_FROMCAM �� ���� ��ȿ.
	stUriInfo.ucMode = ucMode;
	stUriInfo.ucAps = hCc->stCicamData.stUri.ucApsCopyCtrl;
	stUriInfo.ucEmi = hCc->stCicamData.stUri.ucEmiCopyCtrl;
	stUriInfo.ucIct = hCc->stCicamData.stUri.ucIctCopyCtrl;
	stUriInfo.ucRct = hCc->stCicamData.stUri.ucRctCopyCtrl;
	stUriInfo.ucDot = hCc->stCicamData.stUri.ucDotCopyCtrl;
	CiCc_GetMinutesForRL(hCc->stCicamData.stUri.ucRlCopyCtrl, &stUriInfo.ulMinutesForRL);

	if (CI_ApplyUriToExternalOutput(hCc->usSlotId, stUriInfo))
	{
		HxLOG_Error("[CICC]---> CiCc_UriDoCopyCtrl()---> Error : CI_ApplyUriToExternalOutput() !!!\n");
		return;
	}
}

static void CiCc_UriTimerCallback(unsigned long ulTimerId, void *pvParam)
{
	CI_SLOT_ID usSlotId = *((CI_SLOT_ID *)pvParam);
	CI_CC_INSTANCE * hCc = NULL;
	int i;

	HxLOG_Info("CiCc_UriTimerCallback !!!\n");

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				hCc = &s_aCiCc[i];
				break;
			}
		}
	}

	if (hCc == NULL)
	{
		HxLOG_Error("[CICC]---> CiCc_UriTimerCallback()---> Error : hCc is NULL !!!\n");
		return;
	}

	// init URI timer
	hCc->ulUriTimerID = VK_TIMERID_NULL;

	// set URI timeout values and do control external interface with them
	CiCc_UriDoCopyCtrl(hCc, CI_CC_URI_MODE_TIMEOUT);
}

/*******************************/
/*** Local functions for SRM ***/
/*******************************/

static void CiCc_GenerateSrmConfirm(CI_CC_INSTANCE *hCc, unsigned char *pucSrmData, unsigned short usSrmDataLen)
{
	unsigned char *M;

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
	#endif

	M = (unsigned char *)OxCAS_Malloc(usSrmDataLen + 32);
	if(M != NULL)
	{
		HxSTD_memcpy(M, pucSrmData, usSrmDataLen);
		HxSTD_memcpy(&M[usSrmDataLen], hCc->stHostData.UCK, 32); // �� ������ UCK ���� ��ȿ�� ���̴�.

		SHA_256(M, usSrmDataLen+32, hCc->stHostData.srm_confirm);

		OxCAS_Free(M);
	}
	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
	HxLOG_Info("[CICC]---> CiCc_GenerateSrmConfirm() : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
	#endif
}

static void CiCc_SrmDoApply(CI_CC_INSTANCE *hCc, unsigned char *pucSrmData, unsigned short usSrmDataLen)
{
	if (CI_ApplySrmData(hCc->usSlotId, pucSrmData, usSrmDataLen))
	{
		HxLOG_Error("[CICC]---> CiCc_SrmDoApply()---> Error : CI_ApplySrmData() !!!\n");
		return;
	}
}

/**********************************************/
/*** Local functions for ARC Error Handling ***/
/**********************************************/

static void CiCc_DoActionForArcErr(CI_CC_INSTANCE *hCc, SvcCas_CiArcErr_e enArcErr)
{
	CI_ARC_ACTION enArcAction;

	// CC instance ������ ���� ó���� ���⼭ �ϰ� ������
	if (hCc == NULL || enArcErr == CI_ARC_ERR_Get_Instance)
	{
		s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed;
		s_stCcReportInfo.enArcErr = enArcErr;
		Callback_Reset(g_hCbNotifyCcReport);
		while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
		{
			(*s_pfnNotifyCcReport)(0, &s_stCcReportInfo);
		}
		return;
	}

	// keep ARC error code
	hCc->enArcErr = enArcErr;

	// �ش� ���� ��Ȳ�� �ؾ� �� �׼ǰ� UI ó�� ���� �����Ѵ�
	switch (enArcErr)
	{
		case CI_ARC_ERR_Apdu_Tag :						enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_Apdu_Len1 :						enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_Apdu_Len2 :						enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_Send_Apdu :						enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_SAC_MSG_Counter :				enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_SAC_MSG_Header1 :				enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_SAC_MSG_Header2 :				enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_SAC_MSG_PayloadLen :			enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_SAC_MSG_Authentication :			enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_Send_data_nbr :					enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_Request_data_nbr :				enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
	#if 0
		case CI_ARC_ERR_uri_message :						enArcAction = CI_ARC_ACTION_STOP_CAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_program_number :					enArcAction = CI_ARC_ACTION_STOP_CAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
	#else // UI ó���� �ϵ��� ����
		case CI_ARC_ERR_uri_message :					enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_program_number :				enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
	#endif
		case CI_ARC_ERR_Root_Cert :						enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_Not_Defined :					enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
//		case CI_ARC_ERR_None :							return;
//		case CI_ARC_ERR_Module_Revoked :					return;
//		case CI_ARC_ERR_Host_Revoked :					return;
	#if 0
		case CI_ARC_ERR_SAC_Failed :						enArcAction = CI_ARC_ACTION_STOP_CAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_CCK_Failed :						enArcAction = CI_ARC_ACTION_STOP_CAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
	#else // CI+ v1.3
	  #if defined(WA_CAM_RESET_WHEN_CC_SS_CLOSED)
	  	case CI_ARC_ERR_SAC_Failed :						enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
		case CI_ARC_ERR_CCK_Failed :						enArcAction = CI_ARC_ACTION_NONE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
	  #else
		case CI_ARC_ERR_SAC_Failed :						return;
		case CI_ARC_ERR_CCK_Failed :						return;
	  #endif
	#endif
//		case CI_ARC_ERR_CAM_Upgrade_Bootloader_Failed :	return;
//		case CI_ARC_ERR_CAM_Upgrade_Location_Failed :		return;
//		case CI_ARC_ERR_CAM_Upgrade_Signature_Failed :		return;
//		case CI_ARC_ERR_Auth_Retries_Exhausted :			return;
		case CI_ARC_ERR_Auth_Signature_Failed :			enArcAction = CI_ARC_ACTION_STOP_CAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
		case CI_ARC_ERR_Auth_Key_Verification_Failed :		enArcAction = CI_ARC_ACTION_STOP_CAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
		case CI_ARC_ERR_Auth_Key_Computation_Failed :		enArcAction = CI_ARC_ACTION_STOP_CAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
		case CI_ARC_ERR_Auth_DH_Failed :					enArcAction = CI_ARC_ACTION_STOP_CAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
		case CI_ARC_ERR_CAM_Cert_Syntax_Incorrect :		enArcAction = CI_ARC_ACTION_STOP_CAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
		case CI_ARC_ERR_CAM_Cert_Expired :				enArcAction = CI_ARC_ACTION_GOTO_CI_MODE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
		case CI_ARC_ERR_CAM_Cert_Signature_Failed :		enArcAction = CI_ARC_ACTION_STOP_CAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
//		case CI_ARC_ERR_Host_Cert_Syntax_Incorrect :		return;
//		case CI_ARC_ERR_Host_Cert_Expired : 			return;
//		case CI_ARC_ERR_Host_Cert_Signature_Failed :		return;
//		case CI_ARC_ERR_OP_Cert_Syntax_Incorrect :			return;
//		case CI_ARC_ERR_OP_Cert_Expired :					return;
//		case CI_ARC_ERR_OP_Cert_Signature_Failed :			return;
//		case CI_ARC_ERR_CAM_Requires_Update :			return;
//		case CI_ARC_ERR_Reserved_for_CI_Plus :				return;
//		case CI_ARC_ERR_Private_Use_for_OP :				return;
		default :
			HxLOG_Error("[CICC]---> CiCc_DoActionForArcErr()---> Error : Wrong coding case 1 !!!\n");
			return;
	}

	// UI ó���� ���� �ݹ�
	s_stCcReportInfo.enArcErr = enArcErr;
	Callback_Reset(g_hCbNotifyCcReport);
	while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
	{
		(*s_pfnNotifyCcReport)(hCc->usSlotId, &s_stCcReportInfo);
	}

	// �׼� : stop cam or goto normal mode...
	switch (enArcAction)
	{
		case CI_ARC_ACTION_NONE :
			// ȣ��Ʈ�� �������� ���ο� ���� ���� �Ǵ� ķ�� ���� ������ ��ȸ�� ������ ��� UI �� ������ ����ϵ��� �Ѵ�.
			break;

		case CI_ARC_ACTION_STOP_CAM :
			if (svc_cas_DevCiSetTsPath(hCc->usSlotId, eCAS_DEV_CI_BYPASS) != ERR_OK)
				HxLOG_Error("[CICC]---> CiCc_DoActionForArcErr()---> Error : svc_cas_DevCiSetTsPath() failed !!!\n");
			break;

		case CI_ARC_ACTION_GOTO_CI_MODE :
			/* <TODO_CI_PLUS> : �� ��쿡 ���� CI+ ������ ������ ��ȣ�ϳ�...
								�ϴ� Host Service Shunning ���� �Ϲ� ķ���� �����ϰ� ���� CCK �� URI �������� �ʵ��� ���� (CC ���ҽ��� �ƿ� ����?) ...
			                    Host Service Shunning ������ ó���� ���ؼ� CI_SetPlusMode(), CI_GetPlusMode() ������ ��.
			                    -> ����μ� ���� ó���� �ʿ䰡 ���� �� �ϴ� �ϴ� �������д�... */
			break;

		default :
			HxLOG_Error("[CICC]---> CiCc_DoActionForArcErr()---> Error : Wrong coding case 2 !!!\n");
			break;
	}

	return;
}

/***************************************************/
/*** Local functions for authentication contexts ***/
/***************************************************/

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
static CI_AUTH_CTX * CiCc_GetAuthContext(int index)
{
	if (index >= CI_NUM_OF_AUTH_CTX)
	{
		HxLOG_Error("[CICC]---> CiCc_GetAuthContext()---> Error : Invalid params !!!\n");
		return NULL;
	}

	return &s_stCiAuthCtxs.ctx[index];
}

static int CiCc_PutAuthContext (int index, CI_AUTH_CTX *pstCiAuthCtx)
{
	if ((index >= CI_NUM_OF_AUTH_CTX) || (pstCiAuthCtx == NULL))
	{
		HxLOG_Error("[CICC]---> CiCc_PutAuthContext()---> Error : Invalid params !!!\n");
		return -1;
	}

	s_stCiAuthCtxs.ctx[index] = *pstCiAuthCtx;

	return ERR_OK;
}

static HBOOL CiCc_IsUsedAuthContextIndex(int index)
{
	int i;

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].nCurAuthCtxIdx == index && s_aCiCc[i].usSsNo != 0 && s_aCiCc[i].bUsedCurAuthCtxIdx == TRUE)
		{
			return TRUE;
		}
	}

	return FALSE;
}

#else

static CI_AUTH_CTX * CiCc_GetAuthContext (CI_SLOT_ID usSlotId, int index)
{
	if ((usSlotId >= svc_cas_DevCiGetNumberOfModule()) || (index >= CI_NUM_OF_AUTH_CTX))
	{
		HxLOG_Error("[CICC]---> CiCc_GetAuthContext()---> Error : Invalid params !!!\n");
		return NULL;
	}

	return &s_stCiAuthCtxs[usSlotId].ctx[index];
}

static int CiCc_PutAuthContext (CI_SLOT_ID usSlotId, int index, CI_AUTH_CTX *pstCiAuthCtx)
{
	if ((usSlotId >= svc_cas_DevCiGetNumberOfModule()) || (index >= CI_NUM_OF_AUTH_CTX) || (pstCiAuthCtx == NULL))
	{
		HxLOG_Error("[CICC]---> CiCc_PutAuthContext()---> Error : Invalid params !!!\n");
		return -1;
	}

	HxSTD_memcpy(&s_stCiAuthCtxs[usSlotId].ctx[index], pstCiAuthCtx, sizeof(CI_AUTH_CTX));

	return ERR_OK;
}

#endif

/***************************************/
/*** Local functions for CC instance ***/
/***************************************/

static int CiCc_AddInst(unsigned short usSsNo)
{
	CI_CC_INSTANCE * hCc = NULL;
	CI_SLOT_ID usSlotId;

	if (s_ucCiCcCnt >= MAX_CI_CC_SS)
	{
		HxLOG_Error("[CICC]---> CiCc_AddInst()---> Error : session is full !!!\n");
		return -1;
	}

	hCc = CiCc_GetNewInst();
	if (hCc == NULL)
	{
		HxLOG_Error("[CICC]---> CiCc_AddInst()---> Error : instance is full !!!\n");
		return -1;
	}

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CICC]---> CiCc_AddInst()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	hCc->usSsNo = usSsNo;
	hCc->enPhase = CI_CC_PHASE_SS_Opened;
	hCc->usSlotId = usSlotId;

	s_ucCiCcCnt++;

	return ERR_OK;
}

static int CiCc_DeleteInst(unsigned short usSsNo)
{
	CI_CC_INSTANCE * hCc = NULL;

	hCc = CiCc_GetInst(usSsNo);
	if (hCc == NULL)
	{
		HxLOG_Error("[CICC]---> CiCc_DeleteInst()---> Error : SsNo[%d] incorrect !!!\n", usSsNo);
		return -1;
	}

	if (hCc->ulUriTimerID)
	{
		VK_TIMER_Cancel(hCc->ulUriTimerID);
	}

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	if( hCc->stCicamData.pucLicenseData != NULL )
	{
		OxCAS_Free(hCc->stCicamData.pucLicenseData);
		hCc->stCicamData.pucLicenseData = NULL;
	}

	if (hCc->ulPbLicenseTimerID)
	{
		VK_TIMER_Cancel(hCc->ulPbLicenseTimerID);
		hCc->ulPbLicenseTimerID = VK_TIMERID_NULL;
	}
#endif

	HxSTD_memset(hCc, 0, sizeof(CI_CC_INSTANCE));

	s_ucCiCcCnt--;

	return ERR_OK;
}

static CI_CC_INSTANCE * CiCc_GetNewInst(void)
{
	int i;

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].usSsNo == 0)
			return &s_aCiCc[i];
	}

	return NULL;
}

static CI_CC_INSTANCE * CiCc_GetInst(unsigned short usSsNo)
{
	int i;

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].usSsNo == usSsNo)
		{
			HxLOG_Info("[CICC]---> CiCc_GetInst()---> %d th instance found\n", i);
			return &s_aCiCc[i];
		}
	}

	return NULL;
}

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)

/*******************************/
/*** Local functions for PIN ***/
/*******************************/

/* CiCc_PinCapabilityReq(): CAM�� �����ϴ� PIN capability�� �˱� ���� CI_CC_PIN_CAPABILITIES_REQ APDU�� ����� CAM���� ���� */
int CiCc_PinCapabilityReq(CI_CC_INSTANCE *hCc)
{
	unsigned char ucMsg[4];

	ucMsg[0] = (unsigned char)((CI_CC_PIN_CAPABILITIES_REQ >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_CC_PIN_CAPABILITIES_REQ >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_CC_PIN_CAPABILITIES_REQ & 0xff);
	ucMsg[3] = 0x00;

	if (CI_IsCiPlusCam(hCc->usSlotId) > 0 // CI+ CAM �̰�,
						&& local_CiCc_GetCcResVersion(hCc) == CI_CC_RES_V2_ID)	// �ش� Session�� CC_RES_V2�� ������ ���
	{
		if (CIRH_SendApdu(hCc->usSsNo, ucMsg, 4) != ERR_OK)
		{
			HxLOG_Error("[CICC]---> CiCc_PinCapabilityReq()---> Error : fail to send CI_CC_PIN_CAPABILITIES_REQ for SsNo[%d] !!!\n", hCc->usSsNo);
			return -1;
		}
	}
	else
	{
		// CAM�� CCv2�� �������� �ʴµ� CiCc_PinCapabilityReq ��ƾ�� Ÿ�� ��� ��. �ٺ������� ��� �� �Լ��� �θ����� Ȯ���ؼ� ó�� �ؾ� �� ��!
		HxLOG_Assert(FALSE);
	}

	HxLOG_Info("[CICC]---> CiCc_PinCapabilityReq() SsNo[%d]...\n", hCc->usSsNo);

	return ERR_OK;
}

/* CiCc_PinCmd(): */
int CiCc_PinCmd(CI_CC_INSTANCE *hCc, HUINT8 *pucPinInfo) //, CI_SLOT_ID usSlotId)
{
	unsigned char 	ucMsg[8];
	int				nCurPoint = 0;
	int				i = 0;

	hCc->enPinReplyStatus = CI_CC_PIN_REPLY_FOR_CMD;

	ucMsg[0] = (unsigned char)((CI_CC_PIN_CMD >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_CC_PIN_CMD >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_CC_PIN_CMD & 0xff);
	ucMsg[3] = 0x04;

	nCurPoint = 4;

	for ( i=0; i<CICAM_PIN_LENGTH; i++ )
	{
		ucMsg[nCurPoint] = pucPinInfo[i];
		nCurPoint++;
	}

	if (CI_IsCiPlusCam(hCc->usSlotId) > 0 // CI+ CAM �̰�,
						&& local_CiCc_GetCcResVersion(hCc) == CI_CC_RES_V2_ID)	// �ش� Session�� CC_RES_V2�� ������ ���
	{
		if (CIRH_SendApdu(hCc->usSsNo, ucMsg, 8) != ERR_OK)
		{
			HxLOG_Error("[CICC]---> CiCc_PinCmd()---> Error : fail to send CI_CC_PIN_CMD for SsNo[%d] !!!\n", hCc->usSsNo);
			return ERR_FAIL;
		}
	}
	else
	{
		// CAM�� CCv2�� �������� �ʴµ� CiCc_PinCmd ��ƾ�� Ÿ�� ��� ��. �ٺ������� ��� �� �Լ��� �θ����� Ȯ���ؼ� ó�� �ؾ� �� ��!
		HxLOG_Assert(FALSE);
	}

	HxLOG_Info("[CICC]---> CiCc_PinCmd() SsNo[%d]...\n", hCc->usSsNo);

	return ERR_OK;
}


#endif


/******************************************/
/*** Public functions for other modules ***/
/******************************************/

int CiCc_GetHostId(unsigned char *pucHostId)
	// �� �Լ��� ȣ���� CiCc_Task() �⵿�� �Ϸ�� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
{
	if (pucHostId == NULL)
	{
		HxLOG_Error("[CICC]---> CiCc_GetHostId()---> Error : Invalid params !!!\n");
		return -1;
	}

	if (CiCc_IsInitialized() != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_GetHostId()---> Error : No credentials in this set !!!\n");
		return -1;
	}

	HxSTD_memcpy(pucHostId, s_stCiHostLicenseConst.Host_ID, 8);

	return ERR_OK;
}

int CiCc_GetBindedCamId(CI_SLOT_ID usSlotId, unsigned char *pucBindedCamId)
	// �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
{
	CI_CC_INSTANCE * hCc = NULL;
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	int i;

	if (pucBindedCamId == NULL)
	{
		HxLOG_Error("[CICC]---> CiCc_GetBindedCamId()---> Error : Invalid params !!!\n");
		return -1;
	}

	if (CiCc_IsInitialized() != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_GetBindedCamId()---> Error : No credentials in this set !!!\n");
		return -1;
	}

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				hCc = &s_aCiCc[i];

				if (hCc->enPhase < CI_CC_PHASE_SAC_Establishment_Requested)
				{
					HxLOG_Error("[CICC]---> CiCc_GetBindedCamId()---> Error : enPhase %d !!!\n", hCc->enPhase);
					return -1; // ���� ���μ����� ������ �ʾ� ��ȿ�� ����� ���� �� ���� (ķ ���� ���Ķ�� ����).
				}

				if (hCc->enArcErr != CI_ARC_ERR_None)
				{
					HxLOG_Error("[CICC]---> CiCc_GetBindedCamId()---> Error : enArcErr %d !!!\n", hCc->enArcErr);
					return -1; // ���μ��� �󿡼� ���� �߻��Ͽ� ��ȿ�� ����� ���� �� ���� (ķ �� ������� ����).
				}

			#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
			#else
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
			#endif
				{
					HxLOG_Error("[CICC]---> CiCc_GetBindedCamId()---> Error : Can't load context !!!\n"); // �̷� ��Ȳ�� �߻��ϸ� ���. ����� �ʿ�!!
					return -1;
				}

				if (pstCiAuthCtx->ucSavedFlag != 2)
				{
					HxLOG_Error("[CICC]---> CiCc_GetBindedCamId()---> Error : ucSavedFlag %d !!!\n", pstCiAuthCtx->ucSavedFlag);
					return -1; // ����� ���� ���� (��, ���ε��� �ȵǾ� �ִ�?).
				}

				HxSTD_memcpy(pucBindedCamId, pstCiAuthCtx->ucBindedCamId, 8);

				return ERR_OK;
			}
		}
	}

	return -1; // ķ�� ���� ���� �ʴ�.
}

int CiCc_GetBindedCamBrandId(CI_SLOT_ID usSlotId, unsigned short *pusBindedCamBrandId)
	// �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
{
	CI_CC_INSTANCE * hCc = NULL;
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	int i;

	if (pusBindedCamBrandId == NULL)
	{
		HxLOG_Error("[CICC]---> CiCc_GetBindedCamBrandId()---> Error : Invalid params !!!\n");
		return -1;
	}

	if (CiCc_IsInitialized() != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_GetBindedCamBrandId()---> Error : No credentials in this set !!!\n");
		return -1;
	}

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				hCc = &s_aCiCc[i];

				if (hCc->enPhase < CI_CC_PHASE_SAC_Establishment_Requested)
				{
					HxLOG_Error("[CICC]---> CiCc_GetBindedCamBrandId()---> Error : enPhase %d !!!\n", hCc->enPhase);
					return -1; // ���� ���μ����� ������ �ʾ� ��ȿ�� ����� ���� �� ���� (ķ ���� ���Ķ�� ����).
				}

				if (hCc->enArcErr != CI_ARC_ERR_None)
				{
					HxLOG_Error("[CICC]---> CiCc_GetBindedCamBrandId()---> Error : enArcErr %d !!!\n", hCc->enArcErr);
					return -1; // ���μ��� �󿡼� ���� �߻��Ͽ� ��ȿ�� ����� ���� �� ���� (ķ �� ������� ����).
				}

			#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
			#else
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
			#endif
				{
					HxLOG_Error("[CICC]---> CiCc_GetBindedCamBrandId()---> Error : Can't load context !!!\n"); // �̷� ��Ȳ�� �߻��ϸ� ���. ����� �ʿ�!!
					return -1;
				}

				if (pstCiAuthCtx->ucSavedFlag != 2)
				{
					HxLOG_Error("[CICC]---> CiCc_GetBindedCamBrandId()---> Error : ucSavedFlag %d !!!\n", pstCiAuthCtx->ucSavedFlag);
					return -1; // ����� ���� ���� (��, ���ε��� �ȵǾ� �ִ�?).
				}

				*pusBindedCamBrandId = pstCiAuthCtx->usBindedCamBrandId; // SMiT : 14399 (0x383F), Neotion : 23308 (0x5B0C), SmarDTV : 2898 (0xB52)

				return ERR_OK;
			}
		}
	}

	return -1; // ķ�� ���� ���� �ʴ�.
}

int CiCc_GetNegotiatedCckCipher(CI_SLOT_ID usSlotId, CI_CC_KEY_CIPHER *penNegotiatedCckCipher)
	// �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
{
	CI_CC_INSTANCE * hCc = NULL;
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	int i;

	if (penNegotiatedCckCipher == NULL)
	{
		HxLOG_Error("[CICC]---> CiCc_GetNegotiatedCckCipher()---> Error : Invalid params !!!\n");
		return -1;
	}

	if (CiCc_IsInitialized() != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_GetNegotiatedCckCipher()---> Error : No credentials in this set !!!\n");
		return -1;
	}

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				hCc = &s_aCiCc[i];

				if (hCc->enPhase < CI_CC_PHASE_SAC_Establishment_Requested)
				{
					HxLOG_Error("[CICC]---> CiCc_GetNegotiatedCckCipher()---> Error : enPhase %d !!!\n", hCc->enPhase);
					return -1; // ���� ���μ����� ������ �ʾ� ��ȿ�� ����� ���� �� ���� (ķ ���� ���Ķ�� ����).
				}

				if (hCc->enArcErr != CI_ARC_ERR_None)
				{
					HxLOG_Error("[CICC]---> CiCc_GetNegotiatedCckCipher()---> Error : enArcErr %d !!!\n", hCc->enArcErr);
					return -1; // ���μ��� �󿡼� ���� �߻��Ͽ� ��ȿ�� ����� ���� �� ���� (ķ �� ������� ����).
				}

			#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
			#else
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
			#endif
				{
					HxLOG_Error("[CICC]---> CiCc_GetNegotiatedCckCipher()---> Error : Can't load context !!!\n"); // �̷� ��Ȳ�� �߻��ϸ� ���. ����� �ʿ�!!
					return -1;
				}

				if (pstCiAuthCtx->ucSavedFlag != 2)
				{
					HxLOG_Error("[CICC]---> CiCc_GetNegotiatedCckCipher()---> Error : ucSavedFlag %d !!!\n", pstCiAuthCtx->ucSavedFlag);
					return -1; // ����� ���� ���� (��, ���ε��� �ȵǾ� �ִ�?).
				}

				*penNegotiatedCckCipher = pstCiAuthCtx->nNegotiatedCckCipher;

				return ERR_OK;
			}
		}
	}

	return -1; // ķ�� ���� ���� �ʴ�.
}

int CiCc_SetNegotiatedCckCipher(CI_SLOT_ID usSlotId, CI_CC_KEY_CIPHER enNegotiatedCckCipher)
	// �׽�Ʈ �뵵�� �Լ���. �ٸ� �뵵�� ������� �� ��!
{
#ifdef CONFIG_DEBUG

	CI_CC_INSTANCE * hCc = NULL;
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	int i;

	if (CiCc_IsInitialized() != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_SetNegotiatedCckCipher()---> Error : No credentials in this set !!!\n");
		return -1;
	}

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				hCc = &s_aCiCc[i];

				if (hCc->enPhase < CI_CC_PHASE_SAC_Establishment_Requested)
				{
					HxLOG_Error("[CICC]---> CiCc_SetNegotiatedCckCipher()---> Error : enPhase %d !!!\n", hCc->enPhase);
					return -1; // ���� ���μ����� ������ �ʾ� ��ȿ�� ����� ���� �� ���� (ķ ���� ���Ķ�� ����).
				}

				if (hCc->enArcErr != CI_ARC_ERR_None)
				{
					HxLOG_Error("[CICC]---> CiCc_SetNegotiatedCckCipher()---> Error : enArcErr %d !!!\n", hCc->enArcErr);
					return -1; // ���μ��� �󿡼� ���� �߻��Ͽ� ��ȿ�� ����� ���� �� ���� (ķ �� ������� ����).
				}

			#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
			#else
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
			#endif
				{
					HxLOG_Error("[CICC]---> CiCc_SetNegotiatedCckCipher()---> Error : Can't load context !!!\n"); // �̷� ��Ȳ�� �߻��ϸ� ���. ����� �ʿ�!!
					return -1;
				}

				if (pstCiAuthCtx->ucSavedFlag != 2)
				{
					HxLOG_Error("[CICC]---> CiCc_SetNegotiatedCckCipher()---> Error : ucSavedFlag %d !!!\n", pstCiAuthCtx->ucSavedFlag);
					return -1; // ����� ���� ���� (��, ���ε��� �ȵǾ� �ִ�?).
				}

				pstCiAuthCtx->nNegotiatedCckCipher = enNegotiatedCckCipher;

			#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
				if (CI_SaveAuthContexts())
			#else
				if (CI_SaveAuthContexts(hCc->usSlotId, &s_stCiAuthCtxs[hCc->usSlotId]))
			#endif
				{
					HxLOG_Error("[CICC]---> CiCc_SetNegotiatedCckCipher()---> Error : Can't save context !!!\n");
					return -1;
				}

				return ERR_OK;
			}
		}
	}

#endif

	return -1; // ķ�� ���� ���� �ʴ�.
}

void CiCc_ReqSetCckToDsc(CI_SLOT_ID usSlotId)
	// �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ �� -> ���̻� ������ ������ ���� �ʿ��� �뵵�� ���� ���� �־� �ϴ� ���ܵε��� �Ѵ�.
{
	CI_CC_INSTANCE * hCc = NULL;
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	int i;

	if (CiCc_IsInitialized() != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_ReqSetCckToDsc()---> Error : No credentials in this set !!!\n");
		return;
	}

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				hCc = &s_aCiCc[i];

				if (hCc->enPhase < CI_CC_PHASE_SAC_Establishment_Requested)
				{
					HxLOG_Error("[CICC]---> CiCc_ReqSetCckToDsc()---> Error : enPhase %d !!!\n", hCc->enPhase);
					return; // ���� ���μ����� ������ �ʾ� ��ȿ�� ����� ���� �� ���� (ķ ���� ���Ķ�� ����).
				}

				if (hCc->enArcErr != CI_ARC_ERR_None)
				{
					HxLOG_Error("[CICC]---> CiCc_ReqSetCckToDsc()---> Error : enArcErr %d !!!\n", hCc->enArcErr);
					return; // ���μ��� �󿡼� ���� �߻��Ͽ� ��ȿ�� ����� ���� �� ���� (ķ �� ������� ����).
				}

			#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
			#else
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
			#endif
				{
					HxLOG_Error("[CICC]---> CiCc_ReqSetCckToDsc()---> Error : Can't load context !!!\n"); // �̷� ��Ȳ�� �߻��ϸ� ���. ����� �ʿ�!!
					return;
				}

				if (pstCiAuthCtx->ucSavedFlag != 2)
				{
					HxLOG_Error("[CICC]---> CiCc_ReqSetCckToDsc()---> Error : ucSavedFlag %d !!!\n", pstCiAuthCtx->ucSavedFlag);
					return; // ����� ���� ���� (��, ���ε��� �ȵǾ� �ִ�?).
				}

				// set descrambler (�� ������ nNegotiatedCckCipher, enKeyRegister ���� ��ȿ�� ���̴�)
				if (pstCiAuthCtx->nNegotiatedCckCipher == CI_CC_KEY_CIPHER_DES_56_ECB)
				{
					if (CI_SetCckToDsc(hCc->usSlotId, CI_CC_KEY_CIPHER_DES_56_ECB, hCc->stCicamData.enKeyRegister, hCc->stHostData.CCK, NULL))
					{
						HxLOG_Error("[CICC]---> CiCc_ReqSetCckToDsc()---> Error : Can't set descrambler for DES !!!\n");
						return;
					}
				}
				else
				{
					if (CI_SetCckToDsc(hCc->usSlotId, CI_CC_KEY_CIPHER_AES_128_CBC, hCc->stCicamData.enKeyRegister, hCc->stHostData.CCK, hCc->stHostData.CIV))
					{
						HxLOG_Error("[CICC]---> CiCc_ReqSetCckToDsc()---> Error : Can't set descrambler for AES !!!\n");
						return;
					}
				}
			}
		}
	}
}

void CiCc_SetUriDefault(CI_SLOT_ID usSlotId, HBOOL bOn)
	// �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
{
	CI_CC_INSTANCE * hCc = NULL;
	int i;

	if (CiCc_IsInitialized() != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_SetUriDefault()---> Error : No credentials in this set !!!\n");
		return;
	}

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				hCc = &s_aCiCc[i];

				#if 1 // TODO : ���� ���� üũ�� ����� ���� ��... ���߿� Ȯ���ϰ� ���ֵ��� ����...

				if (hCc->enPhase < CI_CC_PHASE_SAC_Establishment_Requested)
				{
					HxLOG_Error("[CICC]---> CiCc_SetUriDefault()---> Error : enPhase %d !!!\n", hCc->enPhase);
					return; // ���� ���μ����� ������ �ʾ� ��ȿ�� ����� ���� �� ���� (ķ ���� ���Ķ�� ����).
				}

				if (hCc->enArcErr != CI_ARC_ERR_None)
				{
					HxLOG_Error("[CICC]---> CiCc_SetUriDefault()---> Error : enArcErr %d !!!\n", hCc->enArcErr);
					return; // ���μ��� �󿡼� ���� �߻��Ͽ� ��ȿ�� ����� ���� �� ���� (ķ �� ������� ����).
				}

				#endif

				// if URI timer is running, cancel it
				if (hCc->ulUriTimerID)
				{
					VK_TIMER_Cancel(hCc->ulUriTimerID);
					hCc->ulUriTimerID = VK_TIMERID_NULL;
				}

				if (bOn == TRUE)
				{
					// set URI default values and do control external interface with them
					CiCc_UriDoCopyCtrl(hCc, CI_CC_URI_MODE_DEFAULT);

					// set URI timer
					if (hCc->ulUriTimerID == VK_TIMERID_NULL)
					{
						VK_TIMER_EventAfter(CI_CC_URI_TIMEOUT, CiCc_UriTimerCallback, (void *)&usSlotId, sizeof(CI_SLOT_ID), (unsigned long *)&hCc->ulUriTimerID);
					}
				}

				return;
			}
		}
	}

	/* nothing to do in this case */

}

HERROR CiCc_IsInitialized(void) /* CI_CC_PHASE_Init */
	// �� �Լ��� ȣ���� CiCc_Task() �⵿�� �Ϸ�� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
{
	static HUINT32 ulWaitCount = 0;

	/* license �� �б� �������� ��� ��� �ؾ� �Ѵ�. (���� ������ ���� ������ �������� ...)
		�׷��� ������ Ư�� CAM�� CC resource�� �ִ� ���� �˸��� ���Ѵ�.
	*/
	while(s_eCiccInitState == eCiccInitState_wait)
	{
		#if 0
		if(ulWaitCount > 5)
		{
			break;
		}
		#endif

		ulWaitCount++;
		VK_TASK_Sleep(10);
	}

	if(s_eCiccInitState == eCiccInitState_HostLicenseLoading_Ok)
	{
		return ERR_OK;

	}

	return ERR_FAIL;
}

int CiCc_IsOpened(CI_SLOT_ID usSlotId) /* CI_CC_PHASE_SS_Opened */
	// �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
{
	int i;

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				return 1;
			}
		}
	}

	return 0;
}

int CiCc_IsBusy(CI_SLOT_ID usSlotId)
	// �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
{
	int i;

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
//				HxLOG_Print("CiCc_IsBusy : %d\n", s_aCiCc[i].enPhase);
				if (s_aCiCc[i].enArcErr == CI_ARC_ERR_None && s_aCiCc[i].enPhase < CI_CC_PHASE_CCK_Calculating_Replied)
					return 1;
				else
					return 0;
			}
		}
	}

	return 0;
}

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
HULONG CiCc_GetCcResourceVersionBySlot(CI_SLOT_ID usSlotId)
{
	CI_CC_INSTANCE 	*hCc = NULL;
	HULONG			ulCcResVersion;

	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) != ERR_OK)
	{
		HxLOG_Error("Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		return ERR_FAIL;
	}

	ulCcResVersion = local_CiCc_GetCcResVersion(hCc);

	return ulCcResVersion;
}

static void CiCc_PbLicenseTimerCallback(unsigned long ulTimerId, void *pvParam)
{
	CI_SLOT_ID usSlotId = *((CI_SLOT_ID *)pvParam);
	CI_CC_INSTANCE * hCc = NULL;
	int i;

	HxLOG_Info("CiCc_UriTimerCallback !!!\n");

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				hCc = &s_aCiCc[i];
				break;
			}
		}
	}

	if (hCc == NULL)
	{
		HxLOG_Error("[CICC]---> CiCc_UriTimerCallback()---> Error : hCc is NULL !!!\n");
		return;
	}

	// init Plyaback license timer
	hCc->ulPbLicenseTimerID = VK_TIMERID_NULL;

	// TODO: Timer �ɸ������� CAM���κ��� ������ ���� ���ϴ� ��� Playback�� Stop ��Ŵ.
}

HERROR CiCc_GetCcInstanceBySlotId(CI_SLOT_ID usSlotId, CI_CC_INSTANCE **hCc)
{
	int i;

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				*hCc = &s_aCiCc[i];
				return ERR_OK;
			}
		}
	}

	HxLOG_Error("CiCc_GetCcInstanceBySlotId() Slot ID[%02x] ERROR.\n", usSlotId);

	return ERR_FAIL;
}

int CiCc_CheckStatusForCcVer2SacMessage(CI_CC_INSTANCE *hCc)
{

	if (hCc->enPhase < CI_CC_PHASE_Host_Capability_Evaluation_Replied) //�̰� ���߿� �ٽ� Ȯ���� �� ��
	{
		HxLOG_Error("[CICC]---> CiCc_CheckStatuesToUseCcVer2SacMessage()---> Error : enPhase %d !!!\n", hCc->enPhase);
		return ERR_FAIL; // ���� ���μ����� ������ �ʾ� ��ȿ�� ����� ���� �� ���� (ķ ���� ���Ķ�� ����).
	}

	if (hCc->enArcErr != CI_ARC_ERR_None)
	{
		HxLOG_Error("[CICC]---> CiCc_CheckStatuesToUseCcVer2SacMessage()---> Error : enArcErr %d !!!\n", hCc->enArcErr);
		return ERR_FAIL; // ���μ��� �󿡼� ���� �߻��Ͽ� ��ȿ�� ����� ���� �� ���� (ķ �� ������� ����).
	}

	if (local_CiCc_GetCcResVersion(hCc) == CI_CC_RES_V1_ID)	// �ش� Session�� CC_RES_V1���� ������ ���
	{
		HxLOG_Error("[CICC]---> CiCc_CheckStatuesToUseCcVer2SacMessage()---> Error : CI+CAM do not support CI+ ver 1.3!!!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

int CiCc_SendSacDataReqApdu(CI_CC_INSTANCE *hCc, HUINT32 unApduLen, HUINT8 *pucApduData)
{
	HUINT8 	*pucMsg = NULL;
	HUINT8	ucLenFieldSize = 0;
	HERROR	hErr = ERR_OK;

	// allocate message buffer
	pucMsg = (unsigned char *)OxCAS_Malloc( 8 + unApduLen );
	if (pucMsg == NULL)
		return ERR_FAIL;

	pucMsg[0] = (unsigned char)((CI_CC_SAC_DATA_REQ >> 16) & 0xff);
	pucMsg[1] = (unsigned char)((CI_CC_SAC_DATA_REQ >> 8) & 0xff);
	pucMsg[2] = (unsigned char)(CI_CC_SAC_DATA_REQ & 0xff);

	// set message length
	CIRH_ConvertApduLengthField(unApduLen, &pucMsg[3], &ucLenFieldSize);

	// set message payload
	HxSTD_memcpy(&pucMsg[3+ucLenFieldSize], pucApduData, unApduLen);

	if (CI_IsCiPlusCam(hCc->usSlotId) > 0 // CI+ CAM �̰�,
						&& local_CiCc_GetCcResVersion(hCc) == CI_CC_RES_V2_ID)	// �ش� Session�� CC_RES_V2�� ������ ���
	{
		if (CIRH_SendApdu(hCc->usSsNo, pucMsg, 3 + ucLenFieldSize + unApduLen) != ERR_OK)
		{
			HxLOG_Error("[CICC]---> CiCc_SendSacDataReqApdu()---> Error : fail to send CI_CC_SAC_DATA_REQ for SsNo[%d] !!!\n", hCc->usSsNo);
			hErr = ERR_FAIL;
		}
	}
	else
	{
		// CAM�� CCv2�� �������� �ʴµ� CiCc_SendSacDataReqApdu ��ƾ�� Ÿ�� ��� ��. �ٺ������� ��� �� �Լ��� �θ����� Ȯ���ؼ� ó�� �ؾ� �� ��!
		HxLOG_Assert(FALSE);
	}

	if (pucMsg != NULL)
		OxCAS_Free(pucMsg);

	return hErr;
}


int CiCc_RecordStart(CI_SLOT_ID usSlotId, Handle_t hAction, CI_OPERATING_MODE eOperatingMode, unsigned short usProgramNumber)
{
	/*
		�뵵: CI+CAM contents�� ��ȭ �� �� �ش� �������ݷ� ��ȭ�� ������ CAM�ʿ� �˸���.

		���� APDU
		=========
		cc_sac_data_req() {
			cc_sac_data_req_tag
			length_field()
			sac_message()
		}
	*/

	CI_CC_SAC_MSG_RECORD_START 	stRecordStartMsg;
	HUINT32						unSendApduLen;
	HUINT8						*pucSendApduData = NULL;	// alloced pointer
	CI_CC_INSTANCE 				*hCc = NULL;
	CI_AUTH_CTX 				*pstCiAuthCtx = NULL;
	HUINT8						ucPinCodeLen = 0;
	HUINT8						aucPinCode[CICAM_PIN_LENGTH];
	HERROR						hErr = ERR_OK;

	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_RecordStart()---> Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		hErr = ERR_FAIL;
		goto EXIT;
	}

	if(CiCc_CheckStatusForCcVer2SacMessage(hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_RecordStart()---> Error : CiCc_CheckStatusForCcVer2SacMessage!!!\n");
		hErr = ERR_FAIL;
		goto EXIT;
	}

	HxSTD_memset(aucPinCode, 0, sizeof(unsigned char) * CICAM_PIN_LENGTH);

	stRecordStartMsg.ucOperatingMode = (unsigned char)eOperatingMode;
	stRecordStartMsg.usProgramNumber = usProgramNumber;

	if ( hCc->stCicamData.stPinCapaInfo.enPinCapa == CI_PIN_CAPABILITY_ONLY_CAS || hCc->stCicamData.stPinCapaInfo.enPinCapa == CI_PIN_CAPABILITY_BOTH )
	{
		ucPinCodeLen = 0;
	}
	else if ( hCc->stCicamData.stPinCapaInfo.enPinCapa == CI_PIN_CAPABILITY_ONLY_CAS_WITH_CACHE || hCc->stCicamData.stPinCapaInfo.enPinCapa == CI_PIN_CAPABILITY_BOTH_WITH_CACHE )
	{
	#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
		if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
	#else
		if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
	#endif
		{
			HxLOG_Error("[CICC]---> CiCc_StoreCicamPinInfo()---> Error : Can't load context !!!\n");
			hErr = ERR_FAIL;
			goto EXIT;
		}

		ucPinCodeLen = CICAM_PIN_LENGTH;
		HxSTD_memcpy(aucPinCode, pstCiAuthCtx->aucCicamPinCode, sizeof(unsigned char) * CICAM_PIN_LENGTH);
	}

	// calculate the number of send data
	if (ucPinCodeLen == 0 )	 // PIN code�� ���� ������ ���ٸ� pin_code_data�� ���� ���̰� send data�� 2�� ��.
	{
		stRecordStartMsg.ucPinCodeLength = 0;
		HxSTD_memset(stRecordStartMsg.aucPinCodeData, 0, sizeof(unsigned char)*ucPinCodeLen);

		hErr = CiCc_MakeSacMessage(hCc, CI_CC_SAC_MESSAGE_RECORD_START_NO_PIN, (void *)&stRecordStartMsg, &unSendApduLen, &pucSendApduData);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("[CICC]---> CiCc_RecordStart()---> Error : Make SAC Message FAIL!!!\n");
			goto EXIT;
		}
	}
	else
	{
		stRecordStartMsg.ucPinCodeLength = ucPinCodeLen;
		HxSTD_memcpy(stRecordStartMsg.aucPinCodeData, aucPinCode, sizeof(unsigned char)*ucPinCodeLen);

		hErr = CiCc_MakeSacMessage(hCc, CI_CC_SAC_MESSAGE_RECORD_START_WITH_PIN, (void *)&stRecordStartMsg, &unSendApduLen, &pucSendApduData);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("[CICC]---> CiCc_RecordStart()---> Error : Make SAC Message FAIL!!!\n");
			goto EXIT;
		}
	}

	hCc->hRecAction = hAction;

	hErr = CiCc_SendSacDataReqApdu(hCc, unSendApduLen, pucSendApduData);
	if( hErr != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_RecordStart()---> Error : Send SAC_DATA_REQ APDU FAIL!!!\n");
		goto EXIT;
	}

EXIT:
	if (pucSendApduData != NULL)
		OxCAS_Free(pucSendApduData);

	if(hCc)
	{
		HxLOG_Info("[CICC]---> CiCc_RecordStart() SsNo[%d]...\n", hCc->usSsNo);
	}

	return hErr;
}

int CiCc_PrepareRecordStart(CI_SLOT_ID usSlotId, Handle_t hAction, CI_OPERATING_MODE eOperatingMode, unsigned short usProgramNumber) // TODO for CI_PLUS_V_1_3_FEATURES_ENABLE
{
	/*
		�뵵: CI+CAM contents�� ��ȭ �� �� �ش� �������ݷ� ��ȭ�� ������ CAM�ʿ� �˸���.

		���� APDU
		=========
		cc_sac_data_req() {
			cc_sac_data_req_tag
			length_field()
			sac_message()
		}
	*/

	// Record Start�� CI Manager�ʿ��� ���� �Դٰ� �ϴ���,
	// CiCc_PrepareRecordStart �Լ��� �Ҹ��� ���� �ٷ� SAC message�� ����� CAM���� �����ϸ� CAM���� Host������ ������� URI SAC message �� ���� ������ �߻� ��.
	// �׷��Ƿ� ���� Task�� �ξ� URI�� ���������� ���� ���� Record Start SAC message �� ���� �ϵ��� ��.
	CI_CC_SAC_TASK_MSG msg;

	msg.command = CI_CC_SAC_RECORD_START;
	msg.utInfo.stRecStart.usSlotId = usSlotId;
	msg.utInfo.stRecStart.hAction = hAction;
	msg.utInfo.stRecStart.eOperatingMode = eOperatingMode;
	msg.utInfo.stRecStart.usProgramNumber = usProgramNumber;

	if (VK_MSG_Send(s_ulCiCcSacMsgQId, &msg, sizeof(CI_CC_SAC_TASK_MSG)))
	{
		HxLOG_Error("Error : fail to send msg !!!\n");
		return -1;
	}

	return ERR_OK;
}

int CiCc_ChangeOperatingMode(CI_SLOT_ID usSlotId, unsigned int unSessionId, CI_OPERATING_MODE eOperatingMode, unsigned short usProgramNumber)
{
	/*
		�뵵: Record Start Protocol�� ���� ��ȭ ���� �� ��ȭ ��尡 �ٲ�� �� �Լ��� ����Ǿ� CAM�ʿ� ��ȭ ��尡 �ٲ������ �˷��ش�.
		���� APDU
		=========
		cc_sac_data_req() {
			cc_sac_data_req_tag
			length_field()
			sac_message()
		}
	*/

	CI_CC_SAC_MSG_CHANGE_OPERATING_MODE 	stChangeOperatingModeMsg;
	HUINT32									unSendApduLen;
	HUINT8									*pucSendApduData = NULL;	// alloced pointer
	CI_CC_INSTANCE 							*hCc = NULL;
	HERROR									hErr = ERR_OK;

	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_ChangeOperatingMode()---> Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		hErr = ERR_FAIL;
		goto EXIT;
	}

	if(CiCc_CheckStatusForCcVer2SacMessage(hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_ChangeOperatingMode()---> Error : CiCc_CheckStatusForCcVer2SacMessage!!!\n");
		hErr = ERR_FAIL;
		goto EXIT;
	}

	stChangeOperatingModeMsg.ucOperatingMode = (unsigned char)eOperatingMode;
	stChangeOperatingModeMsg.usProgramNumber = usProgramNumber;

	hErr = CiCc_MakeSacMessage(hCc, CI_CC_SAC_MESSAGE_CHANGE_OPERATING_MODE, (void *)&stChangeOperatingModeMsg, &unSendApduLen, &pucSendApduData);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_ChangeOperatingMode()---> Error : Make SAC Message FAIL!!!\n");
		goto EXIT;
	}

	hErr = CiCc_SendSacDataReqApdu(hCc, unSendApduLen, pucSendApduData);
	if( hErr != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_ChangeOperatingMode()---> Error : Send SAC_DATA_REQ APDU FAIL!!!\n");
		goto EXIT;
	}

EXIT:
	if (pucSendApduData != NULL)
		OxCAS_Free(pucSendApduData);

	HxLOG_Info("[CICC]---> CiCc_ChangeOperatingMode() SsNo[%d]...\n", hCc->usSsNo);

	return hErr;
}

int CiCc_GetRecordingStatus(CI_SLOT_ID usSlotId, HUINT8 *pucCcRecordingStatus)
{
	CI_CC_INSTANCE 					*hCc = NULL;

	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) != ERR_OK)
	{
		HxLOG_Error("Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		return ERR_FAIL;
	}

	*pucCcRecordingStatus = hCc->enRecordingStatus;

	return ERR_OK;
}

int CiCc_RecordStop(CI_SLOT_ID usSlotId, unsigned short usProgramNumber) // TODO for CI_PLUS_V_1_3_FEATURES_ENABLE
{
	/*
		�뵵: Record Start Protocol�� ���� ��ȭ ������ �������� �����ϱ� ���� ���
		���� APDU
		=========
		cc_sac_data_req() {
			cc_sac_data_req_tag
			length_field()
			sac_message()
		}
	*/
	CI_CC_SAC_MSG_RECORD_STOP		stRecordStopMsg;
	HUINT32							unSendApduLen;
	HUINT8							*pucSendApduData = NULL;	// alloced pointer
	CI_CC_INSTANCE 					*hCc = NULL;
	HERROR							hErr = ERR_OK;

	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_RecordStop()---> Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		hErr = ERR_FAIL;
		goto EXIT;
	}

	if(CiCc_CheckStatusForCcVer2SacMessage(hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_RecordStop()---> Error : CiCc_CheckStatusForCcVer2SacMessage!!!\n");
		hErr = ERR_FAIL;
		goto EXIT;
	}

	stRecordStopMsg.usProgramNumber = usProgramNumber;

	hErr = CiCc_MakeSacMessage(hCc, CI_CC_SAC_MESSAGE_RECORD_STOP, (void *)&stRecordStopMsg, &unSendApduLen, &pucSendApduData);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_RecordStop()---> Error : Make SAC Message FAIL!!!\n");
		goto EXIT;
	}

	hErr = CiCc_SendSacDataReqApdu(hCc, unSendApduLen, pucSendApduData);
	if( hErr != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_RecordStop()---> Error : Send SAC_DATA_REQ APDU FAIL!!!\n");
		goto EXIT;
	}

EXIT:
	if (pucSendApduData != NULL)
		OxCAS_Free(pucSendApduData);

	HxLOG_Info("[CICC]---> CiCc_RecordStop() SsNo[%d]...\n", hCc->usSsNo);

	return hErr;
}
/*
int CiCc_Playback(CI_SLOT_ID usSlotId, unsigned int unSessionId, unsigned short usProgramNumber, unsigned char *pucLicenseData, unsigned short usLicenseDataLen)
{
	//
	//	�뵵: Play back �� license �� ����Ǿ� �ִ� �κ��� ������ �ش� license�� CAM���� �����ϰ�, �׿����� ������ �޾� �����ϱ� ���� Playback License Exchange Protocol�� ����Ѵ�.
	//	���� APDU
	//	=========
	//	cc_sac_data_req() {
	//		cc_sac_data_req_tag
	//		length_field()
	//		sac_message()
	//	}
	//
	CI_CC_SAC_MSG_PLAYBACK_LICENSE	stPlaybackLicenseMsg;
	HUINT32							unSendApduLen;
	HUINT8							*pucSendApduData = NULL;	// alloced pointer
	CI_CC_INSTANCE 					*hCc = NULL;
	HERROR							hErr = ERR_OK;

	stPlaybackLicenseMsg.pucHostLicense = NULL;
	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_Playback()---> Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		hErr = ERR_FAIL;
		goto EXIT;
	}

	if(CiCc_CheckStatusForCcVer2SacMessage(hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_Playback()---> Error : CiCc_CheckStatusForCcVer2SacMessage!!!\n");
		hErr = ERR_FAIL;
		goto EXIT;
	}

	stPlaybackLicenseMsg.usProgramNumber = usProgramNumber;
	stPlaybackLicenseMsg.pucHostLicense = OxCAS_Malloc(usLicenseDataLen);
	HxSTD_memcpy(stPlaybackLicenseMsg.pucHostLicense, pucLicenseData, usLicenseDataLen);
	stPlaybackLicenseMsg.usLicenseDataLen = usLicenseDataLen;

	hErr = CiCc_MakeSacMessage(hCc, CI_CC_SAC_MESSAGE_PLAYBACK_LICENSE_EXCHANGE, (void *)&stPlaybackLicenseMsg, &unSendApduLen, &pucSendApduData);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_Playback()---> Error : Make SAC Message FAIL!!!\n");
		goto EXIT;
	}

	hErr = CiCc_SendSacDataReqApdu(hCc, unSendApduLen, pucSendApduData);
	if( hErr != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_Playback()---> Error : Send SAC_DATA_REQ APDU FAIL!!!\n");
		goto EXIT;
	}

EXIT:
	if (pucSendApduData)
		OxCAS_Free(pucSendApduData);
	if (stPlaybackLicenseMsg.pucHostLicense)
		OxCAS_Free(stPlaybackLicenseMsg.pucHostLicense);

	HxLOG_Print("[CICC]---> CiCc_Playback() SsNo[%d]...\n", hCc->usSsNo);

	return hErr;
}
*/
int CiCc_CheckLicense(CI_SLOT_ID usSlotId, unsigned int unSessionId, unsigned char *pucLicenseData, unsigned short usLicenseDataLen)
{
	/*
		�뵵: Play ���� �ش� contents�� playback count ���� Ƚ���� ȹ���ϱ� ���� ���.
		���� APDU
		=========
		cc_sac_data_req() {
			cc_sac_data_req_tag
			length_field()
			sac_message()
		}
	*/
	CI_CC_SAC_MSG_LICENSE_CHECK		stLicenseCheckMsg;
	HUINT32							unSendApduLen;
	HUINT8							*pucSendApduData = NULL;	// alloced pointer
	CI_CC_INSTANCE 					*hCc = NULL;
	HERROR							hErr = ERR_OK;

	stLicenseCheckMsg.pucHostLicense = NULL;
	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_CheckLicense()---> Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		hErr = ERR_FAIL;
		goto EXIT;
	}

	if(CiCc_CheckStatusForCcVer2SacMessage(hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_CheckLicense()---> Error : CiCc_CheckStatusForCcVer2SacMessage!!!\n");
		hErr = ERR_FAIL;
		goto EXIT;
	}

	stLicenseCheckMsg.pucHostLicense = OxCAS_Malloc(usLicenseDataLen);
	HxSTD_memcpy(stLicenseCheckMsg.pucHostLicense, pucLicenseData, usLicenseDataLen);
	stLicenseCheckMsg.usLicenseDataLen = usLicenseDataLen;

	hErr = CiCc_MakeSacMessage(hCc, CI_CC_SAC_MESSAGE_LICENSE_CHECK_EXCHANGE, (void *)&stLicenseCheckMsg, &unSendApduLen, &pucSendApduData);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_CheckLicense()---> Error : Make SAC Message FAIL!!!\n");
		goto EXIT;
	}

	hErr = CiCc_SendSacDataReqApdu(hCc, unSendApduLen, pucSendApduData);
	if( hErr != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_CheckLicense()---> Error : Send SAC_DATA_REQ APDU FAIL!!!\n");
		goto EXIT;
	}

EXIT:
	if (pucSendApduData != NULL)
		OxCAS_Free(pucSendApduData);
	if (stLicenseCheckMsg.pucHostLicense != NULL)
		OxCAS_Free(stLicenseCheckMsg.pucHostLicense);

	HxLOG_Info("[CICC]---> CiCc_CheckLicense() SsNo[%d]...\n", hCc->usSsNo);

	return hErr;
}

int CiCc_GetPinCapaInfo(CI_SLOT_ID usSlotId, CI_PIN_CAPA_INFO *pstPinCapaInfo) // CI+ v1.3
	// �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
{
	CI_CC_INSTANCE * hCc = NULL;

	if (pstPinCapaInfo == NULL)
	{
		HxLOG_Error("[CICC]---> CiCc_GetPinCapaInfo()---> Error : Invalid params !!!\n");
		return ERR_FAIL;
	}

	if (CiCc_IsInitialized() != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_GetPinCapaInfo()---> Error : No credentials in this set !!!\n");
		return ERR_FAIL;
	}

	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_GetPinCapaInfo()---> Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		return ERR_FAIL;
	}

	if (hCc->stCicamData.stPinCapaInfo.bPinCapaSet != TRUE)
	{
		HxLOG_Error("[CICC]---> CiCc_GetPinCapaInfo()---> Error : CI+CAM has not recieved PIN capability information!!!\n");
		//	CiCc_PinCapabilityReq(hCc);		// PIN capability�� ���� ���� ��Ȳ�̹Ƿ� Req�� �ٽ� ����.
		return ERR_FAIL; // CI+CAM�� ���� PIN capability ���� ������ ���� �ʾҴ�.
	}

	HxSTD_memcpy(pstPinCapaInfo, &hCc->stCicamData.stPinCapaInfo, sizeof(CI_PIN_CAPA_INFO));

	return ERR_OK;
}

int CiCc_StoreCicamPinInfo(CI_SLOT_ID usSlotId, HUINT8 *pucPinInfo, UNIXTIME utPinSavedTime)
{
	CI_AUTH_CTX 		*pstCiAuthCtx = NULL;
	CI_CC_INSTANCE * hCc = NULL;

	if (CiCc_IsInitialized() != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_StoreCicamPinInfo()---> Error : No credentials in this set !!!\n");
		return ERR_FAIL;
	}

	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_StoreCicamPinInfo()---> Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		return ERR_FAIL;
	}

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
#else
	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
#endif
	{
		HxLOG_Error("[CICC]---> CiCc_StoreCicamPinInfo()---> Error : Can't load context !!!\n");
		return -1;
	}

	HxSTD_memcpy(pstCiAuthCtx->aucCicamPinCode, pucPinInfo, CICAM_PIN_LENGTH);
	pstCiAuthCtx->utPinSavedTimeByUser = utPinSavedTime;
	pstCiAuthCtx->eStoredPinVerification = CI_CC_STORED_PIN_STATE_STANDBY_FOR_CACHED_PIN;

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
#else
	if (CiCc_PutAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx, pstCiAuthCtx))
	{
		HxLOG_Error("[CICC]---> CiCc_StoreCicamPinInfo()---> Error : Can't shift context !!!\n");
		return -1;
	}
#endif

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	if (CI_SaveAuthContexts())
#else
	if (CI_SaveAuthContexts(hCc->usSlotId, &s_stCiAuthCtxs[hCc->usSlotId]))
#endif
	{
		HxLOG_Error("[CICC]---> CiCc_StoreCicamPinInfo()---> Error : Can't save PIN Capability Infomation !!!\n");
		return -1;
	}

	if (CiCc_PinCmd(hCc, pucPinInfo) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_StoreCicamPinInfo()---> Error : Sending PIN CMD APDU Error !!!\n");
		return -1;
	}

	return ERR_OK;
}

int CiCc_GetUriInfo(CI_SLOT_ID usSlotId, CIPLUS_UriInfo_t *pstUriInfo)
{
	CI_CC_INSTANCE 				*hCc = NULL;
	HERROR						hErr = ERR_OK;

	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) == ERR_OK)
	{
		pstUriInfo->usServiceId = hCc->stCicamData.usProgramNumber; // ucMode �� CI_CC_URI_MODE_FROMCAM �� ���� ��ȿ.
		pstUriInfo->ucMode = hCc->stCicamData.stUri.ucMode;
		pstUriInfo->ucAps = hCc->stCicamData.stUri.ucApsCopyCtrl;
		pstUriInfo->ucEmi = hCc->stCicamData.stUri.ucEmiCopyCtrl;
		pstUriInfo->ucIct = hCc->stCicamData.stUri.ucIctCopyCtrl;
		pstUriInfo->ucRct = hCc->stCicamData.stUri.ucRctCopyCtrl;
		pstUriInfo->ucDot = hCc->stCicamData.stUri.ucDotCopyCtrl;
		CiCc_GetMinutesForRL(hCc->stCicamData.stUri.ucRlCopyCtrl, &pstUriInfo->ulMinutesForRL);
		hErr = ERR_OK;
	}
	else
	{
		HxLOG_Error("[CICC]---> CiCc_GetUriInfo()---> Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		hErr = ERR_FAIL;
	}

	return hErr;
}

int CiCc_PinPlayback(CI_SLOT_ID usSlotId, HUINT32 unMetaIndex, Handle_t hPbAction, HUINT8 ucAgeLimit, HUINT8 *pucPrivateData)
{
	CI_CC_INSTANCE 				*hCc = NULL;
	unsigned char 				ucMsg[20];

	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_PinPlayback()---> Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		return ERR_FAIL;
	}

	HxLOG_Info("[CICC]---> CiCc_PinPlayback() SsNo[%d]...\n", hCc->usSsNo);

	hCc->enPinReplyStatus = CI_CC_PIN_REPLY_FOR_PLAYBACK;
	hCc->hPbAction = hPbAction;
	hCc->unMetaIndex = unMetaIndex;

	ucMsg[0] = (unsigned char)((CI_CC_PIN_PLAYBACK >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_CC_PIN_PLAYBACK >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_CC_PIN_PLAYBACK & 0xff);
	ucMsg[3] = 0x10;

	ucMsg[4] = ucAgeLimit;
	HxSTD_memcpy(&ucMsg[5], pucPrivateData, 15);

	if (CI_IsCiPlusCam(hCc->usSlotId) > 0 // CI+ CAM �̰�,
						&& local_CiCc_GetCcResVersion(hCc) == CI_CC_RES_V2_ID)	// �ش� Session�� CC_RES_V2�� ������ ���
	{
		if (CIRH_SendApdu(hCc->usSsNo, ucMsg, 20) != ERR_OK)
		{
			HxLOG_Error("[CICC]---> CiCc_PinPlayback()---> Error : fail to send CI_CC_PIN_PLAYBACK for SsNo[%d] !!!\n", hCc->usSsNo);
			return ERR_FAIL;
		}
	}
	else
	{
		// CAM�� CCv2�� �������� �ʴµ� CiCc_PinPlayback ��ƾ�� Ÿ�� ��� ��. �ٺ������� ��� �� �Լ��� �θ����� Ȯ���ؼ� ó�� �ؾ� �� ��!
		HxLOG_Assert(FALSE);
	}

	return ERR_OK;
}

int CiCc_SendHostLicense(CI_SLOT_ID usSlotId, HUINT32 unMetaIndex, Handle_t hPbAction, HUINT16 usProgramNum, HUINT8 *pucLicenseData, HUINT16 usLicenseDataLen)
{
	/*
		�뵵: Play back �� license �� ����Ǿ� �ִ� �κ��� ������ �ش� license�� CAM���� �����ϰ�, �׿����� ������ �޾� �����ϱ� ���� Playback License Exchange Protocol�� ����Ѵ�.
		���� APDU
		=========
		cc_sac_data_req() {
			cc_sac_data_req_tag
			length_field()
			sac_message()
		}
	*/
	CI_CC_SAC_MSG_PLAYBACK_LICENSE	stPlaybackLicenseMsg;
	HUINT32							unSendApduLen;
	HUINT8							*pucSendApduData = NULL;	// alloced pointer
	CI_CC_INSTANCE 					*hCc = NULL;
	HERROR							hErr = ERR_OK;

	stPlaybackLicenseMsg.pucHostLicense = NULL;
	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_SendHostLicense()---> Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		hErr = ERR_FAIL;
		goto EXIT;
	}

	if(CiCc_CheckStatusForCcVer2SacMessage(hCc) != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_SendHostLicense()---> Error : CiCc_CheckStatusForCcVer2SacMessage!!!\n");
		hErr = ERR_FAIL;
		goto EXIT;
	}

	hCc->hPbAction = hPbAction;
	hCc->unMetaIndex = unMetaIndex;

	stPlaybackLicenseMsg.usProgramNumber = usProgramNum;
	stPlaybackLicenseMsg.pucHostLicense = OxCAS_Malloc(usLicenseDataLen);
	HxSTD_memcpy(stPlaybackLicenseMsg.pucHostLicense, pucLicenseData, usLicenseDataLen);
	stPlaybackLicenseMsg.usLicenseDataLen = usLicenseDataLen;

	hErr = CiCc_MakeSacMessage(hCc, CI_CC_SAC_MESSAGE_PLAYBACK_LICENSE_EXCHANGE, (void *)&stPlaybackLicenseMsg, &unSendApduLen, &pucSendApduData);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_SendHostLicense()---> Error : Make SAC Message FAIL!!!\n");
		goto EXIT;
	}

	hErr = CiCc_SendSacDataReqApdu(hCc, unSendApduLen, pucSendApduData);
	if( hErr != ERR_OK)
	{
		HxLOG_Error("[CICC]---> CiCc_SendHostLicense()---> Error : Send SAC_DATA_REQ APDU FAIL!!!\n");
		goto EXIT;
	}

	if (hCc->ulPbLicenseTimerID)
	{
		VK_TIMER_Cancel(hCc->ulPbLicenseTimerID);
		hCc->ulPbLicenseTimerID = VK_TIMERID_NULL;
	}

	// set Playback license timer
	if (hCc->ulPbLicenseTimerID == VK_TIMERID_NULL)
	{
		VK_TIMER_EventAfter(CI_CC_PB_LICENSE_TIMEOUT, CiCc_PbLicenseTimerCallback, (void *)&usSlotId, sizeof(CI_SLOT_ID), (unsigned long *)&hCc->ulPbLicenseTimerID);
	}

EXIT:
	if (pucSendApduData != NULL)
		OxCAS_Free(pucSendApduData);
	if (stPlaybackLicenseMsg.pucHostLicense != NULL)
		OxCAS_Free(stPlaybackLicenseMsg.pucHostLicense);

	HxLOG_Print("[CICC]---> CiCc_SendHostLicense() SsNo[%d]...\n", hCc->usSsNo);

	return hErr;
}

#endif

int CiCc_SetOperatorProfileSupportInfo(CI_SLOT_ID usSlotId, HUINT8 ucNitVer, HUINT8 ucInfoVer)
{
	CI_CC_INSTANCE 		*hCc;
	CI_AUTH_CTX 		*pstCiAuthCtx = NULL;
	HUINT8				i = 0;

	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) != ERR_OK)
	{
		HxLOG_Error("Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		return ERR_FAIL;
	}

	for(i=0; i<CI_NUM_OF_AUTH_CTX; i++)
	{
		#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
		if ((pstCiAuthCtx = CiCc_GetAuthContext(i)) == NULL)
		#else
		if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, i)) == NULL)
		#endif
		{
			HxLOG_Error("Error : Can't load context !!!\n");
			return ERR_FAIL;
		}

		if(hCc->nCurAuthCtxIdx == i)
		{
			pstCiAuthCtx->bOpSupport = TRUE;
			pstCiAuthCtx->bUseOpList = TRUE;
			pstCiAuthCtx->ucNitVer = ucNitVer;
			pstCiAuthCtx->ucInfoVer = ucInfoVer;
		}
		else
		{
			if(pstCiAuthCtx->bOpSupport == TRUE)
			{
				pstCiAuthCtx->bUseOpList = FALSE;
				pstCiAuthCtx->ucNitVer = 0;
				pstCiAuthCtx->ucInfoVer = 0;
			}
		}

		#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
		#else
		if (CiCc_PutAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx, pstCiAuthCtx))
		{
			HxLOG_Error("Error : Can't shift context !!!\n");
			return ERR_FAIL;
		}
		#endif
	}

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	if(CI_SaveAuthContexts())
#else
	if (CI_SaveAuthContexts(hCc->usSlotId, &s_stCiAuthCtxs[hCc->usSlotId]))

#endif
	{
		HxLOG_Error("Error : Can't save PIN Capability Infomation !!!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

int CiCc_GetAuthContextBySlotId(CI_SLOT_ID usSlotId, CI_AUTH_CTX 	*pstCiAuthCtx)
{
	CI_CC_INSTANCE 		*hCc;
	CI_AUTH_CTX			*pstSrcCiAuthCtx;

	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) != ERR_OK)
	{
		HxLOG_Error("Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		return ERR_FAIL;
	}

	#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	if ((pstSrcCiAuthCtx = CiCc_GetAuthContext(hCc->nCurAuthCtxIdx)) == NULL)
	#else
	if ((pstSrcCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
	#endif
	{
		HxLOG_Error("Error : Can't load context !!!\n");
		return ERR_FAIL;
	}

	VK_memcpy(pstCiAuthCtx, pstSrcCiAuthCtx, sizeof(CI_AUTH_CTX));

	return ERR_OK;
}

HBOOL CiCc_CheckCcAuthenticationComplete(HUINT16 usSlotId)
{
	CI_CC_INSTANCE 		*hCc;

	if(CiCc_GetCcInstanceBySlotId(usSlotId, &hCc) != ERR_OK)
	{
		HxLOG_Error("Error : CiCc_GetCcInstanceBySlotId %d !!!\n", usSlotId);
		return FALSE;
	}

	if(hCc->enPhase > CI_CC_PHASE_URI_Version_Negotiation_Replied)
	{
		return TRUE;
	}

	return FALSE;
}

void CiCc_PrintDebugInfo(CI_SLOT_ID usSlotIdToCheck, int flagPrintMoreDetail)
{
#ifdef CONFIG_DEBUG

//	#define Print_CI_Plus_Credentials /* ��ǰ ������ �ÿ� ���� define �Ǿ�� �ȵȴ�! */

	CI_SLOT_ID			usSlotId = 0;
	CI_AUTH_CTX			* pstCiAuthCtx = NULL;
	CI_CC_INSTANCE		* hCc = NULL;
	CI_CC_KEY_CIPHER	enNegotiatedCckCipher = CI_CC_KEY_CIPHER_DES_56_ECB;
	int					i, j;

	/* print host license constants */

	if (s_stCiHostLicenseConst.ucSavedFlag != 1 && s_stCiHostLicenseConst.ucSavedFlag != 2)
	{
		HxLOG_Info("\n There is no host license constants\n\n");
		return;
	}

	HxLOG_Info("\n --- Host license constants (%d) ---\n", s_stCiHostLicenseConst.ucSavedFlag);

	#ifdef Print_CI_Plus_Credentials
	HxLOG_Print("\n RootCert              : %d bytes", s_stCiHostLicenseConst.RootCertLen);
	for (i = 0; i < s_stCiHostLicenseConst.RootCertLen; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.RootCert[i]);
	}
	HxLOG_Print("\n Host_BrandCert        : %d bytes", s_stCiHostLicenseConst.Host_BrandCertLen);
	for (i = 0; i < s_stCiHostLicenseConst.Host_BrandCertLen; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.Host_BrandCert[i]);
	}
	HxLOG_Print("\n Host_DevCert          : %d bytes", s_stCiHostLicenseConst.Host_DevCertLen);
	for (i = 0; i < s_stCiHostLicenseConst.Host_DevCertLen; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.Host_DevCert[i]);
	}
	#endif
	HxLOG_Print("\n Host_ID               :");
	for (i = 0; i < 8; i++)
	{
		HxLOG_Print(" %02X", s_stCiHostLicenseConst.Host_ID[i]);
	}
	HxLOG_Print("\n Host_CckCipher        : %s", s_stCiHostLicenseConst.Host_CckCipher == CI_CC_KEY_CIPHER_DES_56_ECB ? "DES" : "AES");
	#ifdef Print_CI_Plus_Credentials
	HxLOG_Print("\n DH_p                  :");
	for (i = 0; i < CI_DH_P_LEN; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.DH_p[i]);
	}
	HxLOG_Print("\n DH_g                  :");
	for (i = 0; i < CI_DH_G_LEN; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.DH_g[i]);
	}
	HxLOG_Print("\n DH_q                  :");
	for (i = 0; i < CI_DH_Q_LEN; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.DH_q[i]);
	}
	HxLOG_Print("\n HDQ_n                 :");
	for (i = 0; i < CI_HDQ_N_LEN; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.HDQ_n[i]);
	}
	HxLOG_Print("\n HDQ_d                 :");
	for (i = 0; i < CI_HDQ_D_LEN; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.HDQ_d[i]);
	}
	HxLOG_Print("\n HDQ_p                 :");
	for (i = 0; i < CI_HDQ_P_LEN; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.HDQ_p[i]);
	}
	HxLOG_Print("\n HDQ_q                 :");
	for (i = 0; i < CI_HDQ_Q_LEN; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.HDQ_q[i]);
	}
	HxLOG_Info("\n HDQ_dP                :");
	for (i = 0; i < CI_HDQ_DP_LEN; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.HDQ_dP[i]);
	}
	HxLOG_Print("\n HDQ_dQ                :");
	for (i = 0; i < CI_HDQ_DQ_LEN; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.HDQ_dQ[i]);
	}
	HxLOG_Print("\n HDQ_qInv              :");
	for (i = 0; i < CI_HDQ_QINV_LEN; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.HDQ_qInv[i]);
	}
	HxLOG_Print("\n PRNG_seed             :");
	for (i = 0; i < 16; i++)
	{
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.PRNG_seed[i]);
	}
	HxLOG_Print("\n SLK                   :");
	for (i = 0; i < 16; i++)
	{
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.SLK[i]);
	}
	HxLOG_Print("\n SIV                   :");
	for (i = 0; i < 16; i++)
	{
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.SIV[i]);
	}
	HxLOG_Print("\n CLK                   :");
	for (i = 0; i < 16; i++)
	{
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.CLK[i]);
	}
	HxLOG_Print("\n DHH                   :");
	for (i = 0; i < 256; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.DHH[i]);
	}
	HxLOG_Print("\n DHPH                  :");
	for (i = 0; i < 256; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", s_stCiHostLicenseConst.DHPH[i]);
	}
	#endif
	HxLOG_Print("\n");

	/* print authentication contexts for all slots */

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
#else
	for (usSlotId = 0; usSlotId < svc_cas_DevCiGetNumberOfModule(); usSlotId++)
#endif
	{
		HxLOG_Print("\n --- Authentication contexts data for slot %d ---\n", usSlotId);

		for (i = 0; i < CI_NUM_OF_AUTH_CTX; i++)
		{
		#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
			if ((pstCiAuthCtx = CiCc_GetAuthContext(i)) == NULL)
		#else
			if ((pstCiAuthCtx = CiCc_GetAuthContext(usSlotId, i)) == NULL)
		#endif
				break;
			HxLOG_Print("\n - context %d (%s) -", i, (pstCiAuthCtx->ucSavedFlag == 1 || pstCiAuthCtx->ucSavedFlag == 2) ? "saved" : "not saved");
			if (pstCiAuthCtx->ucSavedFlag == 1 || pstCiAuthCtx->ucSavedFlag == 2)
			{
				HxLOG_Print("\n\n ucBindedCamId         :");
				for (j = 0; j < 8; j++)
				{
					HxLOG_Print(" %02X", pstCiAuthCtx->ucBindedCamId[j]);
				}
				// SMiT : 14399 (0x383F), Neotion : 23308 (0x5B0C), SmarDTV : 2898 (0xB52)
				HxLOG_Print("\n usBindedCamBrandId    : %d (0x%X)", pstCiAuthCtx->usBindedCamBrandId, pstCiAuthCtx->usBindedCamBrandId);
				HxLOG_Print("\n nNegotiatedCckCipher  : %s", pstCiAuthCtx->nNegotiatedCckCipher == CI_CC_KEY_CIPHER_DES_56_ECB ? "DES" : "AES");
			}
			#ifdef Print_CI_Plus_Credentials
			if (pstCiAuthCtx->ucSavedFlag == 2)
			{
				HxLOG_Print("\n DHSK                  :");
				for (j = 0; j < 16; j++)
				{
					HxLOG_Print(" %02x", pstCiAuthCtx->DHSK[j]);
				}
				HxLOG_Print("\n AKH                   :");
				for (j = 0; j < 32; j++)
				{
					HxLOG_Print(" %02x", pstCiAuthCtx->AKH[j]);
				}
			}
			#else
			if (pstCiAuthCtx->ucSavedFlag == 2)
				HxLOG_Print("\n DHSK & AKH            : saved");
			#endif
			HxLOG_Print("\n");
		}
	}


	/* print CC instance values */

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if(s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotIdToCheck)
			{
				hCc = &s_aCiCc[i];
				break;
			}
		}
	}

	if (hCc == NULL)
	{
		HxLOG_Print("\n There is no CC instance for slot %d\n\n", usSlotIdToCheck);
		return;
	}

	HxLOG_Print("\n --- CC instance for slot %d ---\n", usSlotIdToCheck);

	HxLOG_Print("\n usSsNo                : %d", hCc->usSsNo);
	HxLOG_Print("\n enPhase               : %d", hCc->enPhase);
	HxLOG_Print("\n enArcErr              : %d", hCc->enArcErr);
	HxLOG_Print("\n nCurAuthCtxIdx        : %d", hCc->nCurAuthCtxIdx);
	HxLOG_Print("\n nTryAuthCtxIdx        : %d", hCc->nTryAuthCtxIdx);
	HxLOG_Print("\n ulSacMsgCnt           : %ld", hCc->ulSacMsgCnt);
	HxLOG_Print("\n ulCcTimerID           : %s", hCc->ulCcTimerID ? "timer running" : "timer not running");
	HxLOG_Print("\n ulUriTimerID          : %s", hCc->ulUriTimerID ? "timer running" : "timer not running");

	HxLOG_Print("\n\n - CAM data -\n");

	#ifdef Print_CI_Plus_Credentials
	HxLOG_Print("\n CICAM_BrandCert       : %d bytes", hCc->stCicamData.CICAM_BrandCertLen);
	for (i = 0; i < hCc->stCicamData.CICAM_BrandCertLen; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", hCc->stCicamData.CICAM_BrandCert[i]);
	}
	HxLOG_Print("\n Kp                    :");
	for (i = 0; i < 32; i++)
	{
		HxLOG_Print(" %02x", hCc->stCicamData.Kp[i]);
	}
	HxLOG_Print("\n DHPM                  :");
	for (i = 0; i < 256; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", hCc->stCicamData.DHPM[i]);
	}
	HxLOG_Print("\n CICAM_DevCertLen      : %d bytes", hCc->stCicamData.CICAM_DevCertLen);
	for (i = 0; i < hCc->stCicamData.CICAM_DevCertLen; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", hCc->stCicamData.CICAM_DevCert[i]);
	}
	HxLOG_Print("\n Signature_B           :");
	for (i = 0; i < 256; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02x", hCc->stCicamData.Signature_B[i]);
	}
	HxLOG_Print("\n auth_nonce            :");
	for (i = 0; i < 32; i++)
	{
		HxLOG_Print(" %02x", hCc->stCicamData.auth_nonce[i]);
	}
	HxLOG_Print("\n Ns_module             :");
	for (i = 0; i < 8; i++)
	{
		HxLOG_Print(" %02x", hCc->stCicamData.Ns_module[i]);
	}
	#endif
	HxLOG_Print("\n latest uri_message    :");
	for (i = 0; i < 8; i++)
	{
		HxLOG_Print(" %02x", hCc->stCicamData.uri_message[i]);
	}
	HxLOG_Print("\n latest program_number : 0x%x (%d)", hCc->stCicamData.usProgramNumber, hCc->stCicamData.usProgramNumber);
	HxLOG_Print("\n latest key_register   : %s", hCc->stCicamData.enKeyRegister == CI_CC_KEY_MODE_EVEN ? "even" : "odd");
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	HxLOG_Print("\n PIN capability        : %d", hCc->stCicamData.stPinCapaInfo.enPinCapa);
	HxLOG_Print("\n PIN change time (UTC) :");
	for (i = 0; i < 5; i++)
	{
		HxLOG_Print(" %02x", hCc->stCicamData.stPinCapaInfo.ucPinChangeTimeUtc[i]);
	}
	HxLOG_Print("\n PIN rating            : %d", hCc->stCicamData.stPinCapaInfo.ucRating);
#endif
	if (flagPrintMoreDetail)
	{
		HxLOG_Print("\n\n - Host data -\n");
		#ifdef Print_CI_Plus_Credentials
		HxLOG_Print("\n Ns_Host               :");
		for (i = 0; i < 8; i++)
		{
			HxLOG_Print(" %02x", hCc->stHostData.Ns_Host[i]);
		}
		HxLOG_Print("\n SEK                   :");
		for (i = 0; i < 16; i++)
		{
			HxLOG_Print(" %02x", hCc->stHostData.SEK[i]);
		}
		HxLOG_Print("\n SAK                   :");
		for (i = 0; i < 16; i++)
		{
			HxLOG_Print(" %02x", hCc->stHostData.SAK[i]);
		}
		HxLOG_Print("\n UCK                   :");
		for (i = 0; i < 32; i++)
		{
			HxLOG_Print(" %02x", hCc->stHostData.UCK[i]);
		}
		#endif
		CiCc_GetNegotiatedCckCipher(usSlotIdToCheck, &enNegotiatedCckCipher);
		HxLOG_Print("\n CCK                   :");
		for (i = 0; i < 8; i++)
		{
			HxLOG_Print(" %02x", hCc->stHostData.CCK[i]);
		}
		if (enNegotiatedCckCipher == CI_CC_KEY_CIPHER_AES_128_CBC)
		{
			for (i = 8; i < 16; i++)
			{
				HxLOG_Print(" %02x", hCc->stHostData.CCK[i]);
			}
			HxLOG_Print("\n CIV                   :");
			for (i = 0; i < 16; i++)
			{
				HxLOG_Print(" %02x", hCc->stHostData.CIV[i]);
			}
		}
	}

	HxLOG_Print("\n\n");

#endif /* #ifdef CONFIG_DEBUG */
}

