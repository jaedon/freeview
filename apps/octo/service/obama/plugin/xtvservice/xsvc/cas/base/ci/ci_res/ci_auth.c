/***************************************************************
* $Workfile:   ci_auth.c  $
* $Modtime:
* Auther : hmkim
*
* Desc : Authentication
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/

#include <cb_list.h>
#include "ci_plus.h"
//#include "./ci_cc_lib/ci_cc_cipher.h" // 호스트에서 직접 signature 를 생성해야 하는 경우 필요함

/***************************************************************
* local definition
***************************************************************/



/* _INCLUDE_BD_CI_, CONFIG_OP_CANALREADY define 들은 배타적으로 운용해야 한다! */

#ifdef _INCLUDE_BD_CI_

#define	CI_AUTH_BD_PROTOCOL_ID		0x1702

#endif

#ifdef CONFIG_OP_CANALREADY

#define	CI_AUTH_CR_PROTOCOL_ID		0x0001
#define	CI_AUTH_CR_PROTOCOL_VER		0x01
#define CI_AUTH_CR_PROTOCOL_NAME	"CANAL READY ON SATELLITE"

#if !defined(WIN32)
#include <unistd.h> // for sync()
#else
#define	sync
#endif

#include <stdio.h>
#include <errno.h>
#include <db_common.h> // for DB_JFFS2_PATH define
#define CI_AUTH_CR_SIGN_FILE DB_JFFS2_PATH "canal_ready_sign.bin" // 호스트에서 직접 signature 를 생성해야 하는 경우 사용함
#define CI_AUTH_CR_CERT_FILE DB_JFFS2_PATH "canal_ready_cert.bin"

#endif

/***************************************************************
* typedef
***************************************************************/

#ifdef CONFIG_OP_CANALREADY

typedef enum
{
	CI_AUTH_CR_PROTOCOL_VER_REQ			= 0x01, // host <-- app (module)
	CI_AUTH_CR_PROTOCOL_VER_RESP		= 0x02, // host --> app (module)
	CI_AUTH_CR_INIT_NOTIFY				= 0x03, // host <-- app (module)
	CI_AUTH_CR_MORALITY_LEVEL_NOTIFY	= 0x04, // host <-- app (module)
	CI_AUTH_CR_MORALITY_LEVEL_UNLOCK	= 0x05, // host --> app (module)
	CI_AUTH_CR_MORALITY_PIN_RESET		= 0x06  // host <-- app (module)
} CI_AUTH_CR_PRIVATE_MSG_TAG; /* CANAL READY private message tags */

#endif

/***************************************************************
* static / external variables and structures
***************************************************************/

static SvcCas_CiAuthInfo_t s_stCiAuthInfo[CARM_MAX_CISLOT_NUMBER];
static unsigned short s_usCiAuthSsNo[CARM_MAX_CISLOT_NUMBER];

#ifdef CONFIG_OP_CANALREADY

extern CALLBACK_HANDLE	g_hCbNotifyCanalReadyInit;
extern CALLBACK_HANDLE	g_hCbNotifyCanalReadyMoralLevel;
extern CALLBACK_HANDLE	g_hCbNotifyCanalReadyPinReset;
static CI_NOTIFY_CANALREADY_INIT_FUNC s_pfnNotifyCanalReadyInit;
static CI_NOTIFY_CANALREADY_MORAL_LEVEL_FUNC s_pfnNotifyCanalReadyMoralLevel;
static CI_NOTIFY_CANALREADY_MORAL_PINRESET_FUNC s_pfnNotifyCanalReadyPinReset;

#if 0 // TODO : CANAL READY Certification Authority 로 부터 certificate 받아야 처리 가능 (호스트에서 직접 signature 를 생성해야 하는 경우). 모델마다 달라지는 내용이므로 하드 코딩하지 말고 CI+ 인증서처럼 처리해 주면 좋겠다...
static unsigned char CANAL_READY_HOST_BRAND[32] = {0x00,};
static unsigned char CANAL_READY_HOST_MODEL[32] = {0x00,};
static bignum n = {1, {0x00}};
static bignum d = {1, {0x00}};
static bignum p = {1, {0x00}};
static bignum q = {1, {0x00}};
static bignum dP = {1, {0x00}};
static bignum dQ = {1, {0x00}};
static bignum qInv = {1, {0x00}};
static RSA_private_key CANAL_READY_PRIVATE_KEY = {&n, &d, &p, &q, &dP, &dQ, &qInv};
#endif

#endif

/***************************************************************
* static function prototypes
***************************************************************/

#ifdef CONFIG_OP_CANALREADY

static int CiAuth_CanalReadyProtocolVer(unsigned short usSsNo, unsigned char *pucPrivateMsgPayload);
static int CiAuth_CanalReadyInitNotify(unsigned short usSsNo, unsigned char *pucPrivateMsgPayload, unsigned short usPrivateMsgPayloadLen);
static int CiAuth_CanalReadyMoralityLevelNotify(unsigned short usSsNo, unsigned char *pucPrivateMsgPayload);
static int CiAuth_CanalReadyMoralityPinResetNotify(unsigned short usSsNo);

#endif

/***************************************************************
* function bodies
***************************************************************/

int CiAuth_Init(void)
{
	HxSTD_memset(s_stCiAuthInfo, 0, sizeof(SvcCas_CiAuthInfo_t) * CARM_MAX_CISLOT_NUMBER);
	HxSTD_memset(s_usCiAuthSsNo, 0, sizeof(unsigned short) * CARM_MAX_CISLOT_NUMBER);

	return ERR_OK;
}

int CiAuth_SsOpenedProc(unsigned short usSsNo)
{
	CI_SLOT_ID usSlotId;

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_SsOpenedProc()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	s_usCiAuthSsNo[usSlotId] = usSsNo;

	HxLOG_Print("[CIAUTH]---> CiAuth_SsOpenedProc()---> SsNo[%d]...\n", usSsNo);

	return ERR_OK;
}

int CiAuth_SsClosedProc(unsigned short usSsNo)
{
	CI_SLOT_ID usSlotId;

	HxLOG_Print("[CIAUTH]---> CiAuth_SsClosedProc()---> SsNo[%d]...\n", usSsNo);

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_SsClosedProc()---> Error : delete auth info. SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxSTD_memset(&s_stCiAuthInfo[usSlotId], 0, sizeof(SvcCas_CiAuthInfo_t));
	s_usCiAuthSsNo[usSlotId] = 0;

	return ERR_OK;
}

int CiAuth_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
#if defined(_CI_DEBUG_) || defined(_CI_PLUS_DEBUG_)
	if ((pucApduData != NULL) && (ulApduLen != 0))
	{
		int i;
		HxLOG_Print("\n=== [CIAUTH] : Apdu (len=%ld) ===\n", ulApduLen);
		for (i = 0; i < ulApduLen; i++)
		{
			if ((i & 15) == 0)
				HxLOG_Print("\n");
			HxLOG_Print(" %02x", pucApduData[i]);
		}
		HxLOG_Print("\n\n");
	}
#endif

	switch (ulApduTag)
	{
	case CI_AUTH_REQ:

		/* _INCLUDE_BD_CI_, CONFIG_OP_CANALREADY define 들은 배타적으로 운용해야 한다! */

#ifdef _INCLUDE_BD_CI_
{
		unsigned short usAuthProtocolId;

		if (pucApduData == NULL || ulApduLen < 2) // auth_protocol_id 2 bytes
		{
			HxLOG_Error("[CIAUTH]---> CiAuth_ApduProc()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
			return -1;
		}

		// auth_protocol_id
		usAuthProtocolId = ((unsigned short)pucApduData[0] << 8) & 0xff00;
		usAuthProtocolId |= ((unsigned short)pucApduData[1] & 0x00ff);

		if (usAuthProtocolId != CI_AUTH_BD_PROTOCOL_ID)
		{
			HxLOG_Error("[CIAUTH]---> CiAuth_ApduProc()---> Error : Not supported AuthProtocolId[0x%x] !!!\n", usAuthProtocolId);
			return -1;
		}

		// auth_req_byte
		// process the private messages for BD
}
#endif

#ifdef CONFIG_OP_CANALREADY
{
		unsigned short usAuthProtocolId;
		CI_AUTH_CR_PRIVATE_MSG_TAG ucCanalReadyPrivateMsgTag;
		unsigned short usCanalReadyPrivateMsgLen;

		if (pucApduData == NULL || ulApduLen < 5) // auth_protocol_id 2 bytes + CANAL READY private message tag 1 byte + length 2 bytes
		{
			HxLOG_Error("[CIAUTH]---> CiAuth_ApduProc()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
			return -1;
		}

		// auth_protocol_id
		usAuthProtocolId = ((unsigned short)pucApduData[0] << 8) & 0xff00;
		usAuthProtocolId |= ((unsigned short)pucApduData[1] & 0x00ff);

		if (usAuthProtocolId != CI_AUTH_CR_PROTOCOL_ID)
		{
			HxLOG_Error("[CIAUTH]---> CiAuth_ApduProc()---> Error : Not supported AuthProtocolId[0x%x] !!!\n", usAuthProtocolId);
			return -1;
		}

		// auth_req_byte
		// process the private messages for CANAL READY : canal_ready_ci+_message()

		ucCanalReadyPrivateMsgTag = pucApduData[2];
		usCanalReadyPrivateMsgLen = ((unsigned short)pucApduData[3] << 8) & 0xff00;
		usCanalReadyPrivateMsgLen |= ((unsigned short)pucApduData[4] & 0x00ff);

		switch (ucCanalReadyPrivateMsgTag)
		{
			case CI_AUTH_CR_PROTOCOL_VER_REQ :
				HxLOG_Print("protocol_version_req received (%d bytes)\n", usCanalReadyPrivateMsgLen);
				if (usCanalReadyPrivateMsgLen != 33)
				{
					HxLOG_Error("[CIAUTH]---> CiAuth_ApduProc()---> Error : Wrong PrivateMsgLen[%d] for protocol_version_req !!!\n", usCanalReadyPrivateMsgLen);
					return -1;
				}
				CiAuth_CanalReadyProtocolVer(usSsNo, &pucApduData[5]);
				break;

			case CI_AUTH_CR_INIT_NOTIFY :
				HxLOG_Print("init_notify received (%d bytes)\n", usCanalReadyPrivateMsgLen);
				if (usCanalReadyPrivateMsgLen < 3)
				{
					HxLOG_Error("[CIAUTH]---> CiAuth_ApduProc()---> Error : Wrong PrivateMsgLen[%d] for init_notify !!!\n", usCanalReadyPrivateMsgLen);
					return -1;
				}
				CiAuth_CanalReadyInitNotify(usSsNo, &pucApduData[5], usCanalReadyPrivateMsgLen);
				break;

			case CI_AUTH_CR_MORALITY_LEVEL_NOTIFY :
				HxLOG_Print("morality_level_notify received (%d bytes)\n", usCanalReadyPrivateMsgLen);
				if (usCanalReadyPrivateMsgLen != 1)
				{
					HxLOG_Error("[CIAUTH]---> CiAuth_ApduProc()---> Error : Wrong PrivateMsgLen[%d] for morality_level_notify !!!\n", usCanalReadyPrivateMsgLen);
					return -1;
				}
				CiAuth_CanalReadyMoralityLevelNotify(usSsNo, &pucApduData[5]);
				break;

			case CI_AUTH_CR_MORALITY_PIN_RESET :
				HxLOG_Print("morality_pin_reset_notify received (%d bytes)\n", usCanalReadyPrivateMsgLen);
				CiAuth_CanalReadyMoralityPinResetNotify(usSsNo);
				break;

			default :
				HxLOG_Error("[CIAUTH]---> CiAuth_ApduProc()---> Error : Unknown PrivateMsgTag[0x%x] !!!\n", ucCanalReadyPrivateMsgTag);
				return -1;
		}
}
#endif

		break;

	default:

		HxLOG_Error("[CIAUTH]---> CiAuth_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag);
		return -1;
	}

	HxLOG_Print("[CIAUTH]---> CiAuth_ApduProc()---> SsNo[%d], tag[0x%lx], len[%ld]\n", usSsNo, ulApduTag, ulApduLen);

	return ERR_OK;
}

#ifdef CONFIG_OP_CANALREADY

#if 0 // 호스트에서 직접 signature 를 생성해야 하는 경우

static int CiAuth_CanalReadyLoadSign(unsigned char sign[256])
{
	FILE *fpSignFile = NULL;
	int nSignSize = 0;

	fpSignFile = fopen(CI_AUTH_CR_SIGN_FILE, "rb");
	if (fpSignFile == NULL)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyLoadSign()---> Error : Can't open file - %s !!!\n", strerror(errno));
		return -1;
	}

	nSignSize = fread(sign, 1, 256, fpSignFile);
	fclose(fpSignFile);

	if (nSignSize != 256)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyLoadSign()---> Error : Can't read data - %s !!!\n", strerror(errno));
		return -1;
	}

	#if 0 // self checking for signature
	{
		unsigned char buf[64];
		bignum M, S;
		RSA_public_key CANAL_READY_PUBLIC_KEY = {&n, &bn_e};
		HxSTD_memcpy(buf, CANAL_READY_HOST_BRAND, 32);
		HxSTD_memcpy(&buf[32], CANAL_READY_HOST_MODEL, 32);
		bn_init(&M, buf, 64);
		bn_init(&S, sign, 256);
		if (!V_RSA_PKCS1_V1_5(&CANAL_READY_PUBLIC_KEY, &M, &S))
			HxLOG_Print("[CIAUTH]---> CiAuth_CanalReadyLoadSign()---> Error : Invalid Signature !!!\n");
	}
	#endif

	return ERR_OK;
}

static int CiAuth_CanalReadyGenerateAndSaveSign(unsigned char sign[256])
{
	unsigned char buf[64];
	bignum M, S;

	FILE *fpSignFile = NULL;
	int nSignSize = 0;

	HxSTD_memcpy(buf, CANAL_READY_HOST_BRAND, 32);
	HxSTD_memcpy(&buf[32], CANAL_READY_HOST_MODEL, 32);

	bn_init(&M, buf, 64);

	if (A_RSA_PKCS1_V1_5(&CANAL_READY_PRIVATE_KEY, &M, &S))
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyGenerateAndSaveSign()---> Error : A_RSA !!!\n");
		return -1;
	}

	if (S.len != 256)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyGenerateAndSaveSign()---> Error : Invalid len (%d) !!!\n", S.len);
		return -1;
	}

	#if 0 // self checking for signature
	{
		RSA_public_key CANAL_READY_PUBLIC_KEY = {&n, &bn_e};
		if (!V_RSA_PKCS1_V1_5(&CANAL_READY_PUBLIC_KEY, &M, &S))
			HxLOG_Print("[CIAUTH]---> CiAuth_CanalReadyGenerateAndSaveSign()---> Error : Invalid Signature !!!\n");
	}
	#endif

	HxSTD_memcpy(sign, S.num, 256);

	fpSignFile = fopen(CI_AUTH_CR_SIGN_FILE, "wb");
	if (fpSignFile == NULL)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyGenerateAndSaveSign()---> Error : Can't open file - %s !!!\n", strerror(errno));
		return -1;
	}

	nSignSize = fwrite(sign, 1, 256, fpSignFile);
	fclose(fpSignFile);

	if (nSignSize != 256)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyGenerateAndSaveSign()---> Error : Can't write data - %s !!!\n", strerror(errno));
		return -1;
	}

	sync();

	return ERR_OK;
}

static int CiAuth_CanalReadyProtocolVer(unsigned short usSsNo, unsigned char *pucPrivateMsgPayload)
{
	unsigned char ucMsg[364];

	/*--- protocol_version_req message ---*/

	if (pucPrivateMsgPayload[0] != CI_AUTH_CR_PROTOCOL_VER)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyProtocolVer()---> Warning : Wrong ProtocolVer[0x%x] !!!\n", pucPrivateMsgPayload[0]);
	}
	if (HxSTD_memcmp(&pucPrivateMsgPayload[1], CI_AUTH_CR_PROTOCOL_NAME, strlen(CI_AUTH_CR_PROTOCOL_NAME)) != 0)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyProtocolVer()---> Warning : Wrong ProtocolName[%.*s] !!!\n", 32, &pucPrivateMsgPayload[1]);
	}

	/*--- auth_resp() with protocol_version_resp message ---*/

	/* auth_resp_tag */
	ucMsg[0] = (unsigned char)((CI_AUTH_RESP >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_AUTH_RESP >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_AUTH_RESP & 0xff);

	/* length_field() : message 길이가 고정되어 있는 경우로 CIRH_ConvertApduLengthField()를 사용하지 않고 바로 계산 */
	ucMsg[3] = 0x82;
	ucMsg[4] = (unsigned char)((358 >> 8) & 0xff);
	ucMsg[5] = (unsigned char)(358 & 0xff);

	/* auth_protocol_id */
	ucMsg[6] = (unsigned char)((CI_AUTH_CR_PROTOCOL_ID >> 8) & 0xff);
	ucMsg[7] = (unsigned char)(CI_AUTH_CR_PROTOCOL_ID & 0xff);

	/* auth_resp_byte with protocol_version_resp message */

	// tag
	ucMsg[8] = CI_AUTH_CR_PROTOCOL_VER_RESP;

	// length
	ucMsg[9] = (unsigned char)((353 >> 8) & 0xff);
	ucMsg[10] = (unsigned char)(353 & 0xff);

	// protocol_version (1 byte)
	ucMsg[11] = CI_AUTH_CR_PROTOCOL_VER;

	// procotol_name (32 bytes)
	HxSTD_memset(&ucMsg[12], 0x00, 32);
	HxSTD_memcpy(&ucMsg[12], CI_AUTH_CR_PROTOCOL_NAME, strlen(CI_AUTH_CR_PROTOCOL_NAME));

	// CANAL_READY_HOST_BRAND (32 bytes)
	HxSTD_memcpy(&ucMsg[44], CANAL_READY_HOST_BRAND, 32);

	// CANAL_READY_HOST_MODEL (32 bytes)
	HxSTD_memcpy(&ucMsg[76], CANAL_READY_HOST_MODEL, 32);

	// signature (256 bytes)
	if (CiAuth_CanalReadyLoadSign(&ucMsg[108]))
	{
		// signature 파일이 저장되어 있지 않은 경우 : signature 파일은 생산시 미리 저장되어 운용되는 것으로 한다. 따라서 이런 상황이 생기면 안된다!
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyProtocolVer()---> Warning : Can't load signature !!!\n");
		if (CiAuth_CanalReadyGenerateAndSaveSign(&ucMsg[108]))
		{
			HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyProtocolVer()---> Error : Can't generate and save signature !!!\n");
			return -1;
		}
	}

	/* send it */
	if (CIRH_SendApdu(usSsNo, ucMsg, 364) != ERR_OK)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyProtocolVer()---> Error : fail to send protocol_version_resp for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	return ERR_OK;
}

#else

static int CiAuth_CanalReadyLoadCert(unsigned char cert[356])
{
	FILE *fpCertFile = NULL;
	int nCertSize = 0;

	fpCertFile = fopen(CI_AUTH_CR_CERT_FILE, "rb");
	if (fpCertFile == NULL)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyLoadCert()---> Error : Can't open file - %s !!!\n", strerror(errno));
		return -1;
	}

	nCertSize = fread(cert, 1, 356, fpCertFile);
	fclose(fpCertFile);

	if (nCertSize != 356)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyLoadCert()---> Error : Can't read data - %s !!!\n", strerror(errno));
		return -1;
	}

	return ERR_OK;
}

static int CiAuth_CanalReadyProtocolVer(unsigned short usSsNo, unsigned char *pucPrivateMsgPayload)
{
	unsigned char ucMsg[364];

	/*--- protocol_version_req message ---*/

	if (pucPrivateMsgPayload[0] != CI_AUTH_CR_PROTOCOL_VER)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyProtocolVer()---> Warning : Wrong ProtocolVer[0x%x] !!!\n", pucPrivateMsgPayload[0]);
	}
	if (HxSTD_memcmp(&pucPrivateMsgPayload[1], CI_AUTH_CR_PROTOCOL_NAME, strlen(CI_AUTH_CR_PROTOCOL_NAME)) != 0)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyProtocolVer()---> Warning : Wrong ProtocolName[%.*s] !!!\n", 32, &pucPrivateMsgPayload[1]);
	}

	/*--- auth_resp() with protocol_version_resp message ---*/

	/* auth_resp_tag */
	ucMsg[0] = (unsigned char)((CI_AUTH_RESP >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_AUTH_RESP >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_AUTH_RESP & 0xff);

	/* length_field() : message 길이가 고정되어 있는 경우로 CIRH_ConvertApduLengthField()를 사용하지 않고 바로 계산 */
	ucMsg[3] = 0x82;
	ucMsg[4] = (unsigned char)((358 >> 8) & 0xff);
	ucMsg[5] = (unsigned char)(358 & 0xff);

	/* auth_protocol_id */
	ucMsg[6] = (unsigned char)((CI_AUTH_CR_PROTOCOL_ID >> 8) & 0xff);
	ucMsg[7] = (unsigned char)(CI_AUTH_CR_PROTOCOL_ID & 0xff);

	/* auth_resp_byte with protocol_version_resp message */

	// certificate (356 bytes)
	if (CiAuth_CanalReadyLoadCert(&ucMsg[8]))
	{
		// certificate 파일이 저장되어 있지 않은 경우 : certificate 파일은 생산시 미리 저장되어 운용되는 것으로 한다. 따라서 이런 상황이 생기면 안된다!
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyProtocolVer()---> Warning : Can't load certificate !!!\n");
		return -1;
	}

	/* send it */
	if (CIRH_SendApdu(usSsNo, ucMsg, 364) != ERR_OK)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyProtocolVer()---> Error : fail to send protocol_version_resp for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	return ERR_OK;
}

#endif

static int CiAuth_CanalReadyInitNotify(unsigned short usSsNo, unsigned char *pucPrivateMsgPayload, unsigned short usPrivateMsgPayloadLen)
{
	CI_SLOT_ID     usSlotId;
	unsigned char  bCAT_V_ManagedByTvFlag;
	unsigned char  bHDOfferPresentFlag;
	unsigned short usCaSysId;
	unsigned short usNumOfCaOpId, i, j;
	unsigned long  ulCaOpId[10]; // syntax 상으로는 (0xffff - 3) / 4 = 16383 만큼의 ca_operator_id 가 존재하나 스펙에 언급된 대로 usually 4 개만 처리하도록 한다 -> 10 개로 스펙 변경됨.

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyInitNotify()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	bCAT_V_ManagedByTvFlag = (pucPrivateMsgPayload[0] & 0x80) ? 1 : 0; // CANAL READY on Satellite - Specification for CI Module v1.6 기준에서 이 값은 항상 FALSE 로 온다.
	bHDOfferPresentFlag = (pucPrivateMsgPayload[0] & 0x40) ? 1 : 0;
	usCaSysId = (((unsigned short)pucPrivateMsgPayload[1] << 8) & 0xff00) | ((unsigned short)pucPrivateMsgPayload[2] & 0x00ff); // CaSysId is that of Viaccess.

	if ((usPrivateMsgPayloadLen - 3) % 4)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyInitNotify()---> Warning : Wrong length[%d] of message !!!\n", usPrivateMsgPayloadLen);
	}

	usNumOfCaOpId = (usPrivateMsgPayloadLen - 3) / 4;

	if (usNumOfCaOpId > 10)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyInitNotify()---> Warning : Too many ca_operator_ids (%d) !!!\n", usNumOfCaOpId);
		usNumOfCaOpId = 10;
	}

	for (i = 0, j = 3; i < usNumOfCaOpId; i++, j += 4)
	{
		ulCaOpId[i] = ((unsigned long)pucPrivateMsgPayload[j] << 24) & 0xff000000; // 이 byte 는 reserved bits 로서 값은 0xFF 이다.
		ulCaOpId[i] |= (((unsigned long)pucPrivateMsgPayload[j+1] << 16) & 0x00ff0000); // 이하 3 bytes 는 SOID.
		ulCaOpId[i] |= (((unsigned long)pucPrivateMsgPayload[j+2] << 8) & 0x0000ff00);
		ulCaOpId[i] |= ((unsigned long)pucPrivateMsgPayload[j+3] & 0x000000ff);
	}

	// keep it for further use (this will be used at the CiAuth_CanalReadyGetInitInfo())
	HxSTD_memset(&s_stCiAuthInfo[usSlotId], 0, sizeof(SvcCas_CiAuthInfo_t));
	s_stCiAuthInfo[usSlotId].bSavedFlag = 1;
	s_stCiAuthInfo[usSlotId].bCAT_V_ManagedByTvFlag = bCAT_V_ManagedByTvFlag;
	s_stCiAuthInfo[usSlotId].bHDOfferPresentFlag = bHDOfferPresentFlag;
	s_stCiAuthInfo[usSlotId].usCaSysId = usCaSysId;
	s_stCiAuthInfo[usSlotId].usNumOfCaOpId = usNumOfCaOpId;
	for (i = 0; i < usNumOfCaOpId; i++)
		s_stCiAuthInfo[usSlotId].ulCaOpId[i] = ulCaOpId[i];

	// run callback to notify or send message to app with usSlotId and s_stCiAuthInfo[]
	Callback_Reset(g_hCbNotifyCanalReadyInit);
	while ((s_pfnNotifyCanalReadyInit = (CI_NOTIFY_CANALREADY_INIT_FUNC)Callback_GetNextFunc(g_hCbNotifyCanalReadyInit)) != NULL)
	{
		if ((*s_pfnNotifyCanalReadyInit)(usSlotId, &s_stCiAuthInfo[usSlotId]) != ERR_OK)
		{
			HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyInitNotify()---> Error : callback failed !!!\n");
		}
	}

	return ERR_OK;
}

int CiAuth_CanalReadyGetInitInfo(CI_SLOT_ID usSlotId, SvcCas_CiAuthInfo_t *pstCiAuthInfo)
{
#if 1

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyGetInitInfo()---> Error : incorrect SlotId[%d] !!!\n", usSlotId);
		return -1;
	}

	if (s_usCiAuthSsNo[usSlotId] == 0)
	{
		HxLOG_Print("[CIAUTH]---> CiAuth_CanalReadyGetInitInfo()---> Error : Ss not opened !!!\n"); // APP 호출 패턴에 따라 세션이 없는 경우 정상일 수 있다.
		return -1;
	}

	if (pstCiAuthInfo == NULL)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyGetInitInfo()---> Error : Invalid params !!!\n");
		return -1;
	}

	HxSTD_memcpy(pstCiAuthInfo, &s_stCiAuthInfo[usSlotId], sizeof(SvcCas_CiAuthInfo_t));

#else // 임시 테스트용 코드

	HxSTD_memcpy(pstCiAuthInfo, &s_stCiAuthInfo[usSlotId], sizeof(SvcCas_CiAuthInfo_t));
	pstCiAuthInfo->bSavedFlag = TRUE;
	pstCiAuthInfo->bHDOfferPresentFlag = TRUE;
	pstCiAuthInfo->usCaSysId = 0x500;
	pstCiAuthInfo->usNumOfCaOpId = 1;
	pstCiAuthInfo->ulCaOpId[0] = 0xff032820;

#endif

	return ERR_OK;
}

static int CiAuth_CanalReadyMoralityLevelNotify(unsigned short usSsNo, unsigned char *pucPrivateMsgPayload)
{
	CI_SLOT_ID usSlotId;
	unsigned char ucLevel = pucPrivateMsgPayload[0]; // CANAL READY on Satellite - Host Module Interface v1.5 기준에서 이 값은 항상 -18 (즉, 0x0F or 0x14) 로 온다.

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyMoralityLevelNotify()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	// run callback to notify or send message to app with usSlotId and ucLevel
	Callback_Reset(g_hCbNotifyCanalReadyMoralLevel);
	while ((s_pfnNotifyCanalReadyMoralLevel = (CI_NOTIFY_CANALREADY_MORAL_LEVEL_FUNC)Callback_GetNextFunc(g_hCbNotifyCanalReadyMoralLevel)) != NULL)
	{
		if ((*s_pfnNotifyCanalReadyMoralLevel)(usSlotId, ucLevel) != ERR_OK)
		{
			HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyMoralityLevelNotify()---> Error : callback failed !!!\n");
		}
	}

	return ERR_OK;
}

int CiAuth_CanalReadyMoralityLevelUnlock(CI_SLOT_ID usSlotId)
{
	unsigned char ucMsg[9];

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule() || s_usCiAuthSsNo[usSlotId] == 0)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyMoralityLevelUnlock()---> Error : incorrect SlotId[%d] or Ss not opened !!!\n", usSlotId);
		return -1;
	}

	/* auth_resp_tag */
	ucMsg[0] = (unsigned char)((CI_AUTH_RESP >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_AUTH_RESP >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_AUTH_RESP & 0xff);

	/* length_field() */
	ucMsg[3] = 0x05;

	/* auth_protocol_id */
	ucMsg[4] = (unsigned char)((CI_AUTH_CR_PROTOCOL_ID >> 8) & 0xff);
	ucMsg[5] = (unsigned char)(CI_AUTH_CR_PROTOCOL_ID & 0xff);

	/* auth_resp_byte with morality_level_unlock message */

	// tag
	ucMsg[6] = CI_AUTH_CR_MORALITY_LEVEL_UNLOCK;

	// length
	ucMsg[7] = 0x00;
	ucMsg[8] = 0x00;

	/* send it */
	if (CIRH_SendApdu(s_usCiAuthSsNo[usSlotId], ucMsg, 9) != ERR_OK)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyMoralityLevelUnlock()---> Error : fail to send morality_level_unlock for SsNo[%d] !!!\n", s_usCiAuthSsNo[usSlotId]);
		return -1;
	}

	return ERR_OK;
}

static int CiAuth_CanalReadyMoralityPinResetNotify(unsigned short usSsNo)
{
	CI_SLOT_ID usSlotId;

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyMoralityPinResetNotify()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	// run callback to notify or send message to app with usSlotId
	Callback_Reset(g_hCbNotifyCanalReadyPinReset);
	while ((s_pfnNotifyCanalReadyPinReset = (CI_NOTIFY_CANALREADY_MORAL_PINRESET_FUNC)Callback_GetNextFunc(g_hCbNotifyCanalReadyPinReset)) != NULL)
	{
		if ((*s_pfnNotifyCanalReadyPinReset)(usSlotId) != ERR_OK)
		{
			HxLOG_Error("[CIAUTH]---> CiAuth_CanalReadyMoralityPinResetNotify()---> Error : callback failed !!!\n");
		}
	}

	return ERR_OK;
}

#if defined(CONFIG_DEBUG) // 임시 테스트용 코드
int CiAuth_Debug_SetCanalCaSysIdAndOperatorId(CI_SLOT_ID usSlotId, HUINT16 usCaSysId, HUINT32 ulOperator)
{
	if(usCaSysId == 0)
	{
		s_stCiAuthInfo[usSlotId].bSavedFlag = FALSE;
		s_usCiAuthSsNo[usSlotId] = 0;
	}
	else
	{
		s_stCiAuthInfo[usSlotId].bSavedFlag = TRUE;
		s_usCiAuthSsNo[usSlotId] = usSlotId + 1;
	}

	s_stCiAuthInfo[usSlotId].usCaSysId = usCaSysId;
	s_stCiAuthInfo[usSlotId].bHDOfferPresentFlag = 1;
	s_stCiAuthInfo[usSlotId].usNumOfCaOpId = 1;
	s_stCiAuthInfo[usSlotId].ulCaOpId[0] = ulOperator;

	return ERR_OK;
}
#endif

#endif

