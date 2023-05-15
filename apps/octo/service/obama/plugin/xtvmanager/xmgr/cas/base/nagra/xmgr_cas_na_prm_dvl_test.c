//#define PRM_USAGE_RULE
#if defined (CONFIG_DEBUG) && defined(CONFIG_MW_CAS_NAGRA_PRM)
//#define	PRM_USAGE_URLE
#include <hlib.h>
#include <tapi.h>
//#include <mgr_action.h>
#include <mgr_drmagent.h>
#include <mgr_media.h>

#include <xmgr_cas.h>
#include <namgr_main.h>
#include <svc_output.h>

#include "local_include/_xmgr_cas_na_util.h"
#include "local_include/_xmgr_cas_na_adapt.h"
#include "local_include/_xmgr_cas_na.h"
#include "local_include/_xmgr_cas_na_prm.h"

#include "mgr_live.h"
#include "_svc_cas_mgr_svc_port.h"

#define PRM_TEST_DBG(message)		(VK_Print message)
//#define PRM_TEST_DBG(message)

#define MAX_CREDENTIAL_TEST_CNT			24	/* credential auto test : send msg max count */
#define MAX_RESULT_CODE_TEST_CNT		7
#define	MAX_INVALID_PARAMETER_TEST_CNT	4	/* Error handling (invalid parameters) */

typedef enum
{
	eMSG_SELECT_NORMAL = 0,
	eMSG_SELECT_AUTO_CREDENTIAL,
	eMSG_SELECT_AUTO_RESULTCODE,
	eMSG_SELECT_ERROR_HANDLING_INVALID_PARAMETER,
	eMSG_SELECT_LOAD_CREDENTIAL_DATA,
	eMSG_SELECT_ALL_HANDLING,
#ifdef PRM_USAGE_URLE
	eMSG_SELECT_USAGE_RULE,
#endif
	eMSG_SELECT_PLAYBACK_ERR_CALLBACK,
	eMSG_SELECT_USAGE_RULE_STATUS,
	eMSG_SELECT_MAX
}ePRM_SendMsgUserSelect_t;

// msgType of sendDRMMessage()
typedef enum
{
	eMSG_STRING = 0,			/* msg를 string으로 전달 */
	eMSG_HEXDECIMAL				/* msg를 hexdecimal로 전달 */
}ePRM_SendMsgSort_t;

typedef enum
{
	eMSG_HEX_UPPER_CASE = 0, 	/* 대문자 */
	eMSG_HEX_LOWER_CASE			/* 소문자 */
}eHEX_TYPE_t;

typedef enum
{
	eMSGTYPE_CSPG_HEXBIN = 0,
	eMSGTYPE_HDPLUSCA_SC,
	eMSGTYPE_MAX
}ePRM_SendMsgType_t;

// msg of sendDRMMessage()
typedef enum
{
	eMSG_PRMS_SYSINFO = 0,
	eMSG_PRMS_CHINFO,
	eMSG_CRES_CCRE,
	eMSG_PRMS_PSTART,
	eMSG_PRMS_PSTOP,
	eMSG_PRMS_PSTOPALL,
	eMSG_CRES_DCRE,
	eMSG_CRES_DALL,
	eMSG_CRES_INFO,
	eMSG_CRES_UGMD,
	eMSG_CRES_USMD,
	eMSG_CRES_RCRE,
	eMSG_SC_INSERTED,
	eMSG_SC_PRODUCTVALIDATION,
	eMSG_MAX
}ePRM_SendMsg_t;

typedef enum
{
	eSYSTEMID_6304 = 0,
	eSYSTEMID_6211,
	eSYSTEMID_MAX
}ePRM_SendMsgDRMSystemID_t;

/*
	virtual string
**/
HUINT8 * pVirtualSendMsgType[2] =
{
	"application/vnd.oipf.cspg-hexbinary",
	"hdplusCA_SC"
};

// tsid : 1111, handle : 1234 => fix
HUINT8 * pVirtualSendCspgHexBinMsg[12] =
{
	"PRMS::SYSINFO",							/* 0 */
	"PRMS::CHINFO",								/* 1 */
	"CRES::CCRE::DATA=30313233343536373839",	/* 2 */
	"PRMS::PSTART::TSID=135::CREH=1",			/* 3 */
	"PRMS::PSTOP::PLAYH=1",						/* 4 */
	"PRMS::PSTOPALL",							/* 5 */
	"CRES::DCRE::CREH=1",						/* 6 */
	"CRES::DALL",								/* 7 */
	"CRES::INFO::CREH=1",						/* 8 */
	"CRES::UGMD::CREH=1::DATA=CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC",	/* 9 */
	"CRES::USMD::CREH=1::DATA=CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC",	/* RED */
	"CRES::RCRE::CREH=1",											/* GREEN */
};

HUINT8 * pVirtualSendHdplusCAMsg[2] =
{
	"SC_inserted",
	"SC_productValidation(A30D0180118000021830010F9F20FFAA15028011000000640003032100000002183000000064AA1502801100000BEA0003032100000002183000000BEA)"
};

HUINT8 * pVirtualDRMSystemID[2] =
{
	"urn:dvb:casystemid:6304",
	"urn:dvb:casystemid:6211"
};

typedef struct
{
	ePRM_SendMsgType_t eMsgType;
	ePRM_SendMsg_t eMsg;
	ePRM_SendMsgDRMSystemID_t eDRMSystemID;
}PRM_SendDRMMessage_t;

PRM_SendDRMMessage_t sendDrmMsg;

ePRM_SendMsgSort_t sort = eMSG_STRING;
eHEX_TYPE_t hex_type = eMSG_HEX_UPPER_CASE;
ePRM_SendMsgUserSelect_t eUserSelect = eMSG_SELECT_NORMAL;//eMSG_SELECT_MAX;

static HINT8 expect_resultCode = 0;

#define NAGRA_CREDENTIAL_DATA	"/usr/test.bin"
//static unsigned char 	data[1024];

extern void xmgr_cas_NaDvlCmdInit(HxCMD_Handle_t hCmdHandle);

void _xmag_cas_NaPrmCmdStringToHexDecimal(unsigned char *pStrSource, char *pStrDes, int totalByte)
{
		int i;
	unsigned char ch;
	char tmpBuf[1024];
	char strtmp[1024];

	memset(&tmpBuf, 0, sizeof(tmpBuf));
	memset(&strtmp, 0, sizeof(strtmp));

//	for(i=0;i<strlen(pStrSource);i++)
	for(i=0;i<totalByte;i++)
	{
		ch = pStrSource[i];
		//if(hex_type == eMSG_HEX_UPPER_CASE)
		{
			sprintf(tmpBuf,"%02X", ch);
//			HxLOG_Print ("%s\n", tmpBuf);
		}
		//else
		//{
		//	sprintf(tmpBuf,"%02x", ch);
		//}
		strcat(strtmp, tmpBuf);
	}
	memcpy(pStrDes, strtmp, sizeof(strtmp));
}

static HERROR _xmag_cas_NaPrmCmdGetCredentialData(unsigned char * pData)
{
#if 0	
	int i;
	int nFd, nErr;
	unsigned char 	cre_url[1024];

	HxLOG_Print ("\n++ Get Credential Data ++\n");
	memset( cre_url, 0x00, sizeof( cre_url ) );
	memset( data, 0x00, sizeof( data ) );

	nFd = open(NAGRA_CREDENTIAL_DATA, O_RDONLY | O_SYNC);
	if (nFd < 0)
	{
		HxLOG_Print ("* %s() ERROR 1 < file is not exist >\n", __FUNCTION__);
		return ERR_FAIL;
	}
	nErr = read(nFd, cre_url, sizeof( cre_url ));
	if (nErr < 0)
	{
		HxLOG_Print ("* %s() ERROR 2 < file is not read >\n", __FUNCTION__);
		close(nFd);
		return ERR_FAIL;
	}
	HxLOG_Print ("Total Byte = %d\n", nErr);
	_xmag_cas_NaPrmCmdStringToHexDecimal(cre_url, data, nErr);

	memcpy(pData, data, nErr*2);

	for(i=0;i<nErr*2;i++)
	{
		if(i%32==0)
		{
			HxLOG_Print ("\n");
		}

		HxLOG_Print ("%c", data[i]);
	}

	HxLOG_Print ("\n");

	fsync(nFd);
	close(nFd);
#endif
	return ERR_OK;
}


HERROR _xmag_cas_NaPrmCmdPlaybackCallback(HINT8 cnt, HINT8 *msgType, HINT8 * msg, HINT8 * DRMSystemID)
{
#if 0	
	static int count = 1;
	TDvlAccessStatus status=DVL_ACCESS_GRANTED;

	if(count==14)
		count = 1;

	switch(count)
	{
		case 1:
			status = DVL_ACCESS_GRANTED;
			break;
		case 2:
			status = DVL_ACCESS_DENIED;
			break;
		case 3:
			status = DVL_ACCESS_DENIED_CONTENT_EXPIRED;
			break;
		case 4:
			status = DVL_ACCESS_DENIED_INVALID_CREDENTIALS;
			break;
		case 5:
			status = DVL_ACCESS_DENIED_WRONG_TSID;
			break;
		case 6:
			status = DVL_ACCESS_DENIED_LIMIT_EXCEEDED;
			break;
		case 7:
			status = DVL_ACCESS_DENIED_INVALID_TIME;
			break;
		case 8:
			status = DVL_ACCESS_DENIED_UNKNOWN_STB;
			break;
		case 9:
			status = DVL_ACCESS_DENIED_UNKNOWN_HOME_NETWORK;
			break;
		case 10:
			status = DVL_ACCESS_DENIED_UNKNOWN_NUID;
			break;
		case 11:
			status = DVL_ACCESS_DENIED_INVALID_EMI;
			break;
		case 12:
			status = DVL_ACCESS_DENIED_INVALID_KEY_PROTECTION;
			break;
		case 13:
			status = LAST_DVL_ACCESS_STATUS;
			break;
		default:
			break;

	}
	count++;

//	Browser_drm_sendPlaybackCallback(status);
#endif
	return ERR_FAIL;
}

HERROR _xmag_cas_NaPrmCmdAutoTestAllHandling(HINT8 cnt, HINT8 *msgType, HINT8 * msg, HINT8 * DRMSystemID)
{
	HERROR hErr = ERR_OK;

	HUINT8 *pCRES_CCRE[7] =
	{
		"CRES::CCRE::DATA=30313233343536373839", 	// Create Handle 1
		"CRES::CCRE::DATA=40414243444546474849", 	// Create Handle 2
		"CRES::CCRE::DATA=50515253545556575859", 	// Create Handle 3
		"CRES::CCRE::DATA=60616263646566676869", 	// Create Handle 4
		"CRES::CCRE::DATA=70717273747576777879", 	// Create Handle 5
		"CRES::CCRE::DATA=80818283848586878889", 	// Create Handle  : Error : ResultCode 2
		"CRES::CCRE::DATA=90919293949596979899", 	// Create Handle  : Error : ResultCode 2
	};
	HUINT8 *pCRES_PSTART[7] =
	{
		"PRMS::PSTART::TSID=135::CREH=1", 	// Create Handle 1
		"PRMS::PSTART::TSID=135::CREH=2", 	// Create Handle 1
		"PRMS::PSTART::TSID=135::CREH=3", 	// Create Handle 1
		"PRMS::PSTART::TSID=135::CREH=4", 	// Create Handle 1
		"PRMS::PSTART::TSID=135::CREH=5", 	// Create Handle 1
		"PRMS::PSTART::TSID=135::CREH=6", 	// Create Handle 1
		"PRMS::PSTART::TSID=135::CREH=7" 	// Create Handle 1
	};
	HUINT8 *pCRES_PSTOP[7] =
	{
		"PRMS::PSTOP::PLAYH=1",
		"PRMS::PSTOP::PLAYH=2",
		"PRMS::PSTOP::PLAYH=3",
		"PRMS::PSTOP::PLAYH=4",
		"PRMS::PSTOP::PLAYH=5",
		"PRMS::PSTOP::PLAYH=6",
		"PRMS::PSTOP::PLAYH=7"
	};
	HUINT8 *pCRES_DCRE[7] =
	{
		"CRES::DCRE::CREH=1",
		"CRES::DCRE::CREH=2",
		"CRES::DCRE::CREH=3",
		"CRES::DCRE::CREH=4",
		"CRES::DCRE::CREH=5",
		"CRES::DCRE::CREH=6",
		"CRES::DCRE::CREH=7"
	};

	sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
	sendDrmMsg.eMsg = eMSG_CRES_CCRE;
	sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;


	sprintf(msgType, "%s", pVirtualSendMsgType[sendDrmMsg.eMsgType]);
	sprintf(DRMSystemID, "%s", pVirtualDRMSystemID[sendDrmMsg.eDRMSystemID]);

	switch(cnt)
	{
		case 0:	sprintf(msg, "%s", "PRMS::SYSINFO");
			break;
		case 1: sprintf(msg, "%s", "PRMS::CHINFO");
			break;
		case 2: sprintf(msg, "%s", pCRES_CCRE[0]);
			break;
		case 3: sprintf(msg, "%s", pCRES_PSTART[0]);
			break;
		case 4: sprintf(msg, "%s", pCRES_PSTOP[0]);
			break;
		case 5: sprintf(msg, "%s", pCRES_CCRE[1]);
			break;
		case 6: sprintf(msg, "%s", pCRES_PSTART[1]);
			break;
		case 7: sprintf(msg, "%s", pCRES_PSTOP[1]);
			break;
		case 8: sprintf(msg, "%s", pCRES_CCRE[2]);
			break;
		case 9: sprintf(msg, "%s", pCRES_PSTART[2]);
			break;
		case 10: sprintf(msg, "%s", pCRES_PSTOP[2]);
			break;
		case 11: sprintf(msg, "%s", pCRES_CCRE[3]);
			break;
		case 12: sprintf(msg, "%s", pCRES_PSTART[3]);
			break;
		case 13: sprintf(msg, "%s", pCRES_PSTOP[3]);
			break;
		case 14: sprintf(msg, "%s", pCRES_CCRE[4]);
			break;
		case 15: sprintf(msg, "%s", pCRES_PSTART[4]);
			break;
		case 16: sprintf(msg, "%s", pCRES_PSTOP[4]);
			break;
		case 17: sprintf(msg, "%s", pCRES_DCRE[0]);
			break;
		case 18: sprintf(msg, "%s", pCRES_DCRE[1]);
			break;
		case 19: sprintf(msg, "%s", pCRES_DCRE[2]);
			break;
		case 20: sprintf(msg, "%s", pCRES_DCRE[3]);
			break;
		case 21: sprintf(msg, "%s", pCRES_DCRE[4]);
			break;
		default: break;
	}

	return hErr;
}

HERROR _xmag_cas_NaPrmCmdAutoTestCredential(HINT8 cnt, HINT8 *msgType, HINT8 * msg, HINT8 * DRMSystemID)
{
	HERROR hErr = ERR_OK;
	HUINT8 *pVirtualAutoSendCredentialData[MAX_CREDENTIAL_TEST_CNT] =
	{
		"CRES::CCRE::DATA=30313233343536373839", 	// Create Handle 1
		"CRES::CCRE::DATA=40414243444546474849", 	// Create Handle 2
		"CRES::CCRE::DATA=50515253545556575859", 	// Create Handle 3
		"CRES::CCRE::DATA=60616263646566676869", 	// Create Handle 4
		"CRES::CCRE::DATA=70717273747576777879", 	// Create Handle 5
		"CRES::CCRE::DATA=80818283848586878889", 	// Create Handle  : Error : ResultCode 2
		"CRES::CCRE::DATA=90919293949596979899", 	// Create Handle  : Error : ResultCode 2
		"CRES::DALL",								// All Delete
		"CRES::CCRE::DATA=30313233343536373839", 	// Handle 1
		"CRES::CCRE::DATA=40414243444546474849", 	// Handle 2
		"CRES::CCRE::DATA=50515253545556575859", 	// Handle 3
		"CRES::CCRE::DATA=60616263646566676869", 	// Handle 4
		"CRES::CCRE::DATA=70717273747576777879", 	// Handle 5
		"CRES::DCRE::CREH=1",						// Remove Handle 1
		"CRES::DCRE::CREH=3",						// Remove Handle 3
		"CRES::DCRE::CREH=5",						// Remove Handle 5
		"CRES::CCRE::DATA=30313233343536373839", 	// Handle 1
		"CRES::CCRE::DATA=50515253545556575859", 	// Handle 3
		"CRES::CCRE::DATA=70717273747576777879", 	// Handle 5
		"CRES::DCRE::CREH=2",						// Remove Handle 2
		"CRES::DCRE::CREH=4",						// Remove Handle 4
		"CRES::CCRE::DATA=40414243444546474849", 	// Create Handle 2
		"CRES::CCRE::DATA=70717273747576777879", 	// Create Handle 5
		"CRES::DALL"								// All Delete
	};

	sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
	sendDrmMsg.eMsg = eMSG_CRES_CCRE;
	sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;

	sprintf(msgType, "%s", pVirtualSendMsgType[sendDrmMsg.eMsgType]);
	sprintf(DRMSystemID, "%s", pVirtualDRMSystemID[sendDrmMsg.eDRMSystemID]);
	sprintf(msg, "%s", pVirtualAutoSendCredentialData[cnt]);

	return hErr;
}

HERROR _xmag_cas_NaPrmCmdAutoTestResultCode(HINT8 cnt, HINT8 *msgType, HINT8 * msg, HINT8 * DRMSystemID )
{
	HERROR hErr = ERR_OK;
	expect_resultCode = 0;
	switch(cnt)
	{
		case 0: /* resultCode : 3 */
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			sendDrmMsg.eMsg = eMSG_PRMS_SYSINFO;
			sendDrmMsg.eMsgType = eMSGTYPE_HDPLUSCA_SC; // eDRMSystemID와 eMsgType이 매치가 안되는 경우
			expect_resultCode = 3;
			break;
		case 1: /* resultCode : 3 */
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6211;
			sendDrmMsg.eMsg = eMSG_SC_INSERTED;
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN; // eDRMSystemID와 eMsgType이 매치가 안되는 경우
			expect_resultCode = 3;
			break;
		case 2: /* resultCode : 2 */
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_SC_INSERTED;			// 유효하지 않는 msg를 전달한경우.
			expect_resultCode = 2;
			break;
		case 3: /* resultCode : 2 */
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6211;
			sendDrmMsg.eMsgType = eMSGTYPE_HDPLUSCA_SC;
			sendDrmMsg.eMsg = eMSG_PRMS_SYSINFO;		// 유효하지 않는 msg를 전달한경우.
			expect_resultCode = 2;
			break;
		case 4: /* resultCode : 2 */
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_PRMS_PSTOP;
			expect_resultCode = 2;
			break;
		case 5: /* resultCode : 2 */
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_PRMS_PSTOP;	 			// play start없이 play stop이 진행된 경
			expect_resultCode = 2;
			break;
		case 6: /* resultCode : 2 */
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_CRES_DCRE;	 			// activie handle이 존재하는데 delete를 하는경우. // test를 위해서는 우선 active 시켜야함. 추후 test 진행.
			expect_resultCode = 2;
			break;
		case 7: /* resultCode : 2 */
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_CRES_DALL;	 			// activie handle이 존재하는데 delete를 하는경우. // test를 위해서는 우선 active 시켜야함. 추후 test 진행.
			expect_resultCode = 2;
			break;
		case 8: /* resultCode : 2 */
			// PSTART가 연속으로 진행하는경우.
			expect_resultCode = 2;
			hErr = ERR_FAIL;
			break;
		case 9: /* resultCode : 2 */
			// PSTOP가 연속으로 진행하는경우.
			expect_resultCode = 2;
			hErr = ERR_FAIL;
			break;
		default:
			hErr = ERR_FAIL;
			break;
	}
	if(hErr==ERR_OK)
	{
		sprintf(msgType, "%s", pVirtualSendMsgType[sendDrmMsg.eMsgType]);
		sprintf(DRMSystemID, "%s", pVirtualDRMSystemID[sendDrmMsg.eDRMSystemID]);
		if(cnt==4)
			sprintf(msg, "%s", "PRMS::PSTOP::PLAYH=9999"); // play stop한경우 유효하지 않은 handle을 넘겨준경우.
		else
			sprintf(msg, "%s", pVirtualSendCspgHexBinMsg[sendDrmMsg.eMsg]);
	}

	return hErr;
}

HERROR _xmag_cas_NaPrmCmdAutoTestInvalidParameter(HINT8 cnt, HINT8 *msgType, HINT8 * msg, HINT8 * DRMSystemID )
{
	HERROR hErr = ERR_OK;

	HUINT8 *pVirtualAutoTest_InvalidParameter_MsgType[MAX_INVALID_PARAMETER_TEST_CNT] =
	{
		"",
		"YGWPACLRLUCDGGEQXRIQ...",
		"DGGXGOXVMAMZHZFYWOTK...",
		"application/vnd.oipf..."
	};

	HUINT8 *pVirtualAutoTest_InvalidParameter_Msg[MAX_INVALID_PARAMETER_TEST_CNT] =
	{
		"",
		"OSCVKIRMBDAPOCFDTPPA...",
		"FRFWOMNQLJZXFGHEORFW...",
		"RRNBULYGRYOGIJTRNQLI...",
	};

	HUINT8 *pVirtualAutoTest_InvalidParameter_DRMSystemID[MAX_INVALID_PARAMETER_TEST_CNT] =
	{
		"",
		"OHIERRBGKFPPDUGEOGSM...",
		"JDGDVKBMYJIAQCSFTLEM...",
		"urn:dvb:casystemid:6...",
	};

	sendDrmMsg.eMsgType = eMSGTYPE_MAX;
	sendDrmMsg.eMsg = eMSG_MAX;
	sendDrmMsg.eDRMSystemID = eSYSTEMID_MAX;

	sprintf(msgType, "%s", pVirtualAutoTest_InvalidParameter_MsgType[cnt]);
	sprintf(DRMSystemID, "%s", pVirtualAutoTest_InvalidParameter_DRMSystemID[cnt]);
	sprintf(msg, "%s", pVirtualAutoTest_InvalidParameter_Msg[cnt]);

	return hErr;
}

HERROR _xmag_cas_NaPrmCmdLoadCredentialData(HINT8 cnt, HINT8 *msgType, HINT8 * msg, HINT8 * DRMSystemID )
{
	HERROR hErr = ERR_OK;
#if 0
unsigned char data[1024] =
"20A102000053FC018A3351667FFFFFFF04860E993B4E96EBD9786980FAE2276D318DA58EFF6D5C1B2DE5C2E6217359A86EA31CE20CB6A61A157530945AC1B5DC6EB13D57622E46E5B10CB8BDE99E6FA4441622DE640E0D55816BFBF4DA2F35128C3D68CF470AD0F0B5A66F08E993E6E253746D9C9800DBDF64522B507148F0F7E847EEB50CAAD52490020385D8664989F7E5F2072B6E185B16BA1214F26498608373F6682571EDBD33CD133752832FA3A5D5B28F38EC49F625A102000034FF013605DA47FBC69E1CFFC0A3FB9F75C2831DDD08D84CE48F40672F00353F61C9ACD2BB28FEC61DC2261F96E5EA7191B9B356EFFD89553B8F7B36A7DDB89FA499A07EABB7F854778BDF71B677D11ACE0C1DEDF598CE7DD1BEFEE235A001C395F20DCA13E1AD457E6C02C3B7EC33B8042A11DA2C0C6DB53FC746EDC3F5CAB242C53E91FB2BB84CDF156CA0D214E43D7B5FEE1694F6287E51346F0D30F8D807E00DEA5F5D2D5599BB1085";
#else
	unsigned char data[1024];
	memset(data, 0, sizeof(data));
#endif

	sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
	sendDrmMsg.eMsg = eMSG_CRES_CCRE;
	sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;

	hErr = _xmag_cas_NaPrmCmdGetCredentialData(data);
	sprintf(msgType, "%s", pVirtualSendMsgType[sendDrmMsg.eMsgType]);
	sprintf(DRMSystemID, "%s", pVirtualDRMSystemID[sendDrmMsg.eDRMSystemID]);
	sprintf(msg, "%s%s", "CRES::CCRE::DATA=", data);

	return hErr;
}


#define MAKE_UR_BIT(val, sh, mask)		((val<<sh)&mask)
HERROR _xmag_cas_NaPrmCmdUsageRule(HINT8 cnt, HINT8 *msgType, HINT8 * msg, HINT8 * DRMSystemID )
{
	HUINT8 SendUsageRule[6];
	HUINT8 val = 0;
//	HUINT8 data;
	HUINT8 byte1 = 0;
	HUINT8 byte2 = 0;
	HUINT8 byte3 = 0;
	HUINT8 byte4 = 0;
	sendDrmMsg.eMsgType = eMSGTYPE_MAX;
	sendDrmMsg.eMsg = eMSG_MAX;
	sendDrmMsg.eDRMSystemID = eSYSTEMID_MAX;

	memset(&SendUsageRule, 0, sizeof(SendUsageRule));

/* cVersion : 1100 0000 */
	// ignore
/* cAnticipation_flag : 0010 0000 */
	// ignore
/* cRecording_flag 0001 0000 */
	//val = 0x0;
	val = 0x1;
	byte1 |= MAKE_UR_BIT(val, 4, 0x10);
/* cOriginal_sub_required : 0000 1000 */
/* cDvr_sub_required : 0000 0100 */

/* cTrans_coding_control 0000 0010 */
	//val = 0x0;
	val = 0x1;
	byte1 |= MAKE_UR_BIT(val, 1, 0x02);
/* cImage_constraint 0000 0001 */
	//val = 0x0;
	val = 0x1;
	byte1 |= MAKE_UR_BIT(val, 0, 0x01);

/*cTrick_mode_control 1111 0000*/
	//val = 0x0;
	val = 0xF;
	byte2 |= MAKE_UR_BIT(val, 4, 0xf0);

/*cConcurrent_viewing 1110 0000*/
	//val = 0x0;
	//val = 0x1;
	//val = 0x2;
	//val = 0x3;
	//val = 0x4;
	//val = 0x5;
	//val = 0x6;
	val = 0x7;
	byte3 |= MAKE_UR_BIT(val, 5, 0xe0);
/*cDigital_only_token 0001 0000 */
	//val = 0x0;
	val = 0x1;
	byte3 |= MAKE_UR_BIT(val, 4, 0x10);
/* cUnprotected_digital_output_token 0000 1000*/
	//val = 0x0;
	val = 0x1;
	byte3 |= MAKE_UR_BIT(val, 3, 0x08);
/* cHdcp_token 	0000 0100 */
	//val = 0x0;
	val = 0x1;
	byte3 |= MAKE_UR_BIT(val, 2, 0x04);
/* cDtcp_token 	0000 0010 */
	//val = 0x0;
	val = 0x1;
	byte3 |= MAKE_UR_BIT(val, 1, 0x02);
/* cWMDRM_token 0000 0001 */
	//val = 0x0;
	val = 0x1;
	byte3 |= MAKE_UR_BIT(val, 0, 0x01);
/* cPRM_token 1000 0000 */
	//val = 0x0;
	val = 0x1;
	byte4 |= MAKE_UR_BIT(val, 7, 0x80);
/* cOthers_token 0100 0000 */
	//val = 0x0;
	val = 0x1;
	byte4 |= MAKE_UR_BIT(val, 6, 0x40);
/* cCci 0011 0000 */
	//val = 0x0;
	//val = 0x1;
	//val = 0x2;
	val = 0x3;
	byte4 |= MAKE_UR_BIT(val, 4, 0x30);
/* cUnprotected_analog_output_token 0000 1000 */
	//val = 0x0;
	val = 0x1;
	byte4 |= MAKE_UR_BIT(val, 3, 0x08);
/* cRetention_state 0000 0111 */
	//val = 0x0;
	//val = 0x1;
	//val = 0x2;
	//val = 0x3;
	//val = 0x4;
	//val = 0x5;
	//val = 0x6;
	val = 0x7;
	byte4 |= MAKE_UR_BIT(val, 0, 0x07);


	SendUsageRule[0] = 0x00;
	SendUsageRule[1] = 0x04;

	SendUsageRule[2] = byte1;
	SendUsageRule[3] = byte2;
	SendUsageRule[4] = byte3;
	SendUsageRule[5] = byte4;

//	sprintf(msgType, "%s", pVirtualSendMsgType[sendDrmMsg.eMsgType]);
//	sprintf(DRMSystemID, "%s", pVirtualDRMSystemID[sendDrmMsg.eDRMSystemID]);
	//_xmag_cas_NaPrmCmdStringToHexDecimal(SendUsageRule, data, sizeof(SendUsageRule));
//	sprintf(msg, "%s%s", "UR::%s", data);
	return ERR_OK;
}

HERROR _xmag_cas_NaPrmCmdNormalTest(HUINT8 ucKey, HINT8 *msgType, HINT8 * msg, HINT8 * DRMSystemID )
{
	HERROR hErr = ERR_OK;
	switch(ucKey)
	{
		case KEY_0: /* PRMS::SYSINFO */
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_PRMS_SYSINFO;
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			break;
		case KEY_1: /* PRMS::CHINFO */
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_PRMS_CHINFO;
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			break;
		case KEY_2: /* CRES::CCRE::DATA=[credential] */
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_CRES_CCRE;
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			break;
		case KEY_3: /* PRMS::PSTART::TSID=[contentID]:: CREH=[handle] */
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_PRMS_PSTART;
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			break;
		case KEY_4: /* PRMS::PSTOP::PLAYH=[handle] */
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_PRMS_PSTOP;
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			break;
		case KEY_5: /* PRMS::PSTOPALL */
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_PRMS_PSTOPALL;
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			break;
		case KEY_6: /* CRES::DCRE::CREH=[handle] */
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_CRES_DCRE;
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			break;
		case KEY_7: /* CRES::DALL */
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_CRES_DALL;
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			break;
		case KEY_8: /* CRES::INFO::CREH=[handle] */
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_CRES_INFO;
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			break;
		case KEY_9: /* CRES::UGMD::CREH=1234::DATA=CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC */
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_CRES_UGMD;
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			break;
		case KEY_RED: /* CRES::USMD::CREH=1234::DATA=CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC */
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_CRES_USMD;
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			break;
		case KEY_GREEN: /* CRES::RCRE::CREH=1234 */
			sendDrmMsg.eMsgType = eMSGTYPE_CSPG_HEXBIN;
			sendDrmMsg.eMsg = eMSG_CRES_RCRE;
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6304;
			break;
		case KEY_YELLOW: /* SC_inserted */
			sendDrmMsg.eMsgType = eMSGTYPE_HDPLUSCA_SC;
			sendDrmMsg.eMsg = eMSG_SC_INSERTED;
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6211;
			break;
		case KEY_BLUE: /* SC_productValidation( */
			sendDrmMsg.eMsgType = eMSGTYPE_HDPLUSCA_SC;
			sendDrmMsg.eMsg = eMSG_SC_PRODUCTVALIDATION;
			sendDrmMsg.eDRMSystemID = eSYSTEMID_6211;
			break;
		default:
			hErr = ERR_FAIL;
			break;
	}

	if(hErr==ERR_OK)
	{
		if(sendDrmMsg.eMsgType==eMSGTYPE_CSPG_HEXBIN)
		{
			sprintf(msgType, "%s", pVirtualSendMsgType[sendDrmMsg.eMsgType]);
			sprintf(DRMSystemID, "%s", pVirtualDRMSystemID[sendDrmMsg.eDRMSystemID]);
			//if(sort==eMSG_STRING)
			sprintf(msg, "%s", pVirtualSendCspgHexBinMsg[sendDrmMsg.eMsg]);
			//else if(sort==eMSG_HEXDECIMAL)
			//	_xmag_cas_NaPrmCmdStringToHexDecimal(pVirtualSendCspgHexBinMsg[sendDrmMsg.eMsg], msg);
		}
		else if(sendDrmMsg.eMsgType==eMSGTYPE_HDPLUSCA_SC)
		{
			sprintf(msgType, "%s", pVirtualSendMsgType[sendDrmMsg.eMsgType]);
			sprintf(DRMSystemID, "%s", pVirtualDRMSystemID[sendDrmMsg.eDRMSystemID]);
			//if(sort==eMSG_STRING)
			sprintf(msg, "%s", pVirtualSendHdplusCAMsg[sendDrmMsg.eMsg-eMSG_CRES_RCRE]);
			//else if(sort==eMSG_HEXDECIMAL)
			//	_xmag_cas_NaPrmCmdStringToHexDecimal(pVirtualSendHdplusCAMsg[sendDrmMsg.eMsg-eMSG_CRES_RCRE], msg);
		}
	}

	return hErr;
}

#ifdef PRM_USAGE_RULE
HERROR _xmag_cas_NaPrmCmdUsageRuleState(HUINT8 ucKey)
{
	HdcpMode_t eHCurrentMode;
	MacroVisionMode_t eMCurrentMode;

	if(ucKey==KEY_9)
	{
		OutputFlag_t eAVidOut;
		PAL_OUT_GetAnalogVideoOutputFlag(&eAVidOut);
		PAL_OUT_GetCurrentHdcpMode(&eHCurrentMode);
		PAL_OUT_GetCurrentApsMode(&eMCurrentMode);

		HxLOG_Debug(">>>> Cur Hdcp = %d\n", eHCurrentMode);
		HxLOG_Debug(">>>> Cur Macrovision = %d\n", eMCurrentMode);
		HxLOG_Debug(">>>> analog video : mode = %d\n", eAVidOut);
		return ERR_FAIL;
	}
	else if(ucKey==KEY_8)
	{
		HxLOG_Debug(">>>> macrovision : off\n");
		MW_OUTPUT_ChangeApsMode(eMacroVisionMode_Off);
		return ERR_FAIL;
	}
	else if(ucKey==KEY_7)
	{
		HxLOG_Debug(">>>> macrovision : on\n");
		MW_OUTPUT_ChangeApsMode(eMacroVisionMode_Agc);
		return ERR_FAIL;
	}

	else if(ucKey==KEY_6)
	{
		HxLOG_Debug(">>>> hdcp : off\n");
		MW_OUTPUT_ChangeHdcpMode(eHdcpMode_Off);
		return ERR_FAIL;
	}
	else if(ucKey==KEY_5)
	{
		HxLOG_Debug(">>>> hdcp : on\n");
		MW_OUTPUT_ChangeHdcpMode(eHdcpMode_On);
		return ERR_FAIL;
	}
	else if(ucKey==KEY_4)
	{
		HxLOG_Debug(">>>> analog video : off\n");
		PAL_OUT_DisableAnalogVideo();
		return ERR_FAIL;
	}
	else if(ucKey==KEY_3)
	{
		HxLOG_Debug(">>>> analog video : on\n");
		PAL_OUT_EnableAnalogVideo();
		return ERR_FAIL;
	}
	else if(ucKey==KEY_2)
	{
		OutputFlag_t eAVidOut;
		PAL_OUT_GetAnalogVideoOutputFlag(&eAVidOut);
		HxLOG_Debug(">>>> analog video : mode = %d\n", eAVidOut);
		return ERR_FAIL;
	}
	else if(ucKey==KEY_1)
	{
		OutputFlag_t eAAudOut;
		PAL_OUT_GetAnalogAudioOutputFlag(&eAAudOut);
		HxLOG_Debug(">>>> analog audio : mode = %d\n", eAAudOut);
		return ERR_FAIL;
	}
	else if(ucKey==KEY_0)
	{
		HxLOG_Debug(">>>>> Change UR Data\n");
		MW_OUTPUT_ChangeApsMode(eMacroVisionMode_Agc);
		MW_OUTPUT_ChangeHdcpMode(eHdcpMode_Off);
		PAL_OUT_DisableAnalogVideo();
		return ERR_FAIL;
	}
	return ERR_OK;
}
#endif

HERROR _xmag_cas_NaPrmCmdCreateSendDRMMessageByKey(HUINT8 ucKey, HUINT8 * msgType, HUINT8 * msg, HUINT8 * DRMSystemID, HUINT8 cnt, HUINT8 maxCnt)
{
	HERROR hErr = ERR_FAIL;

	HxLOG_Print("\n######## Test sendDRMMessage(eUserSelect = %d)( Current msg cnt(%d) / Max msg cnt(%d) ) ######\n", eUserSelect, cnt+1, maxCnt);

	if(eUserSelect==eMSG_SELECT_AUTO_CREDENTIAL)
	{
		hErr = _xmag_cas_NaPrmCmdAutoTestCredential(cnt, msgType, msg, DRMSystemID);
	}
	else if(eUserSelect==eMSG_SELECT_AUTO_RESULTCODE)
	{
		hErr = _xmag_cas_NaPrmCmdAutoTestResultCode(cnt, msgType, msg, DRMSystemID);
	}
	else if(eUserSelect==eMSG_SELECT_ERROR_HANDLING_INVALID_PARAMETER)
	{
		hErr = _xmag_cas_NaPrmCmdAutoTestInvalidParameter(cnt, msgType, msg, DRMSystemID);
	}
	else if(eUserSelect==eMSG_SELECT_LOAD_CREDENTIAL_DATA)
	{
		hErr = _xmag_cas_NaPrmCmdLoadCredentialData(cnt, msgType, msg, DRMSystemID);
	}
	else if(eUserSelect==eMSG_SELECT_ALL_HANDLING)
	{
		hErr = _xmag_cas_NaPrmCmdAutoTestAllHandling(cnt, msgType, msg, DRMSystemID);
	}
#ifdef PRM_USAGE_URLE
	else if(eUserSelect==eMSG_SELECT_USAGE_RULE)
	{
		hErr = _xmag_cas_NaPrmCmdUsageRule(cnt, msgType, msg, DRMSystemID);
	}
#endif
	else if(eUserSelect==eMSG_SELECT_PLAYBACK_ERR_CALLBACK)
	{
		hErr = _xmag_cas_NaPrmCmdPlaybackCallback(cnt, msgType, msg, DRMSystemID);
	}
	else if(eUserSelect==eMSG_SELECT_USAGE_RULE_STATUS)
	{
#ifdef PRM_USAGE_RULE
		hErr = _xmag_cas_NaPrmCmdUsageRuleState(ucKey);
#endif
	}
	else
	{
		hErr = _xmag_cas_NaPrmCmdNormalTest(ucKey, msgType, msg, DRMSystemID);
	}

	if(hErr==ERR_OK)
	{
		HxLOG_Print("======== 1. index\n");
		HxLOG_Print("========    * User select = %d\n", eUserSelect);
		HxLOG_Print("========    * msgType = %d, eMsg = %d, eDRMSystemID = %d\n", sendDrmMsg.eMsgType, sendDrmMsg.eMsg, sendDrmMsg.eDRMSystemID);
		HxLOG_Print("======== 2. input string\n");
		HxLOG_Print("========    * msgType =		%s\n", msgType);
		HxLOG_Print("========    * msg =		%s\n", msg);
		HxLOG_Print("========    * DRMSystemID = 	%s\n", DRMSystemID);
		if(eUserSelect==eMSG_SELECT_AUTO_RESULTCODE)
		{
			HxLOG_Print("======== 3. result > Expected_resultCode = %d\n", expect_resultCode);
		}
		HxLOG_Print("======== >. Send ...\n\n");
	}

	return hErr;
}

HERROR  _xmag_cas_NaPrmCmdIsChangeUserTestItem(HUINT8 ucKey)
{
	HERROR hErr = ERR_OK;
	if(ucKey==KEY_OK)
	{
		switch(eUserSelect)
		{
			case eMSG_SELECT_NORMAL:
				HxLOG_Print("======== < %d/%d. Change test item > Credential : auto send message\n", (int)eUserSelect+1, (int)eMSG_SELECT_MAX+1);
				eUserSelect = eMSG_SELECT_AUTO_CREDENTIAL;
				break;
			case eMSG_SELECT_AUTO_CREDENTIAL:
				HxLOG_Print("======== < %d/%d. Change test item > ResultCode : auto send message\n", (int)eUserSelect+1, (int)eMSG_SELECT_MAX+1);
				eUserSelect = eMSG_SELECT_AUTO_RESULTCODE;
				break;
			case eMSG_SELECT_AUTO_RESULTCODE:
				HxLOG_Print("======== < %d/%d. Change test item > Error handling (invalid parameters) : auto send message\n", (int)eUserSelect+1, (int)eMSG_SELECT_MAX+1);
				eUserSelect = eMSG_SELECT_ERROR_HANDLING_INVALID_PARAMETER;
				break;
			case eMSG_SELECT_ERROR_HANDLING_INVALID_PARAMETER:
				HxLOG_Print("======== < %d/%d. Change test item > Loading Credential Data\n", (int)eUserSelect+1, (int)eMSG_SELECT_MAX+1);
				eUserSelect = eMSG_SELECT_LOAD_CREDENTIAL_DATA;
				break;
			case eMSG_SELECT_LOAD_CREDENTIAL_DATA:
				HxLOG_Print("======== < %d/%d. Change test item > All Handling\n", (int)eUserSelect+1, (int)eMSG_SELECT_MAX+1);
				eUserSelect = eMSG_SELECT_ALL_HANDLING;
				break;
			case eMSG_SELECT_ALL_HANDLING:
				HxLOG_Print("======== < %d/%d. Change test item > No Test\n", (int)eUserSelect+1, (int)eMSG_SELECT_MAX+1);
				eUserSelect = eMSG_SELECT_PLAYBACK_ERR_CALLBACK;
				break;
			case eMSG_SELECT_PLAYBACK_ERR_CALLBACK:
				eUserSelect = eMSG_SELECT_USAGE_RULE_STATUS;
				break;
			case eMSG_SELECT_USAGE_RULE_STATUS:
				eUserSelect = eMSG_SELECT_MAX;
			default:
				HxLOG_Print("======== < %d/%d. Change test item > Normal Test\n", (int)eUserSelect+1, (int)eMSG_SELECT_MAX+1);
				eUserSelect = eMSG_SELECT_NORMAL;
				break;
		}
	}
	else
	{
		hErr = ERR_FAIL;
	}

	return hErr;
}

int _xmag_cas_NaPrmCmdSendDRMMessage(HUINT8 ucKey)
{
	// sendDRMMessage parameter : only test
	HUINT8 *pMsg = NULL;
	HUINT8 msgType[300], msg[1024], DRMSystemID[300];
	MgrDrmAgentSendDrmMessage_t stDrmMsg;
	
	HUINT8 testCnt = 1;
	HUINT8 i;

	HxLOG_Print("======== 1. key help\n");
	HxLOG_Print("========    * Select test item	: KEY_OK\n");
	HxLOG_Print("========    * Input test item	: KEY_0 ~ KEY_9, KEY_RED, KEY_GREEN, KEY_YELLOW, KEY_BLUE\n");
	HxLOG_Print("========    * Inputed Key = 0x%x\n", ucKey);

	#if 0
	if(_xmag_cas_NaPrmCmdIsChangeUserTestItem(ucKey)==ERR_OK)
	{
		return 1;
	}
	#endif

	if(eUserSelect==eMSG_SELECT_AUTO_CREDENTIAL)
		testCnt = MAX_CREDENTIAL_TEST_CNT;
	else if(eUserSelect==eMSG_SELECT_AUTO_RESULTCODE)
		testCnt = MAX_RESULT_CODE_TEST_CNT;
	else if(eUserSelect==eMSG_SELECT_ERROR_HANDLING_INVALID_PARAMETER)
		testCnt = MAX_INVALID_PARAMETER_TEST_CNT;
	else if(eUserSelect == eMSG_SELECT_LOAD_CREDENTIAL_DATA)
		testCnt = 1;
	else if(eUserSelect == eMSG_SELECT_ALL_HANDLING)
		testCnt = 22;
#ifdef PRM_USAGE_URLE
	else if(eUserSelect == eMSG_SELECT_USAGE_RULE)
		testCnt = 1;
#endif
	else if(eUserSelect == eMSG_SELECT_PLAYBACK_ERR_CALLBACK)
		testCnt = 1;
	else if(eUserSelect == eMSG_SELECT_USAGE_RULE_STATUS)
		testCnt = 1;
	else
		testCnt = 1;

	pMsg = msg;

	for(i=0;i<testCnt;i++)
	{
		memset(&sendDrmMsg, 0, sizeof(sendDrmMsg));
		sendDrmMsg.eMsgType = eMSGTYPE_MAX;
		sendDrmMsg.eMsg = eMSG_MAX;
		sendDrmMsg.eDRMSystemID = eSYSTEMID_MAX;

		memset(&msgType, 0, sizeof(msgType));
		memset(&msg, 0, sizeof(msg));
		memset(&DRMSystemID, 0, sizeof(DRMSystemID));

		if(eUserSelect == eMSG_SELECT_USAGE_RULE_STATUS)
		{
#ifdef PRM_USAGE_RULE
			if(_xmag_cas_NaPrmCmdUsageRuleState(ucKey)==ERR_OK)
				return 1;
#endif			
		}
		else
		{
			if(_xmag_cas_NaPrmCmdCreateSendDRMMessageByKey(ucKey, msgType, msg, DRMSystemID, i, testCnt)==ERR_OK)
			{
				HINT32 msgId = rand();

				memset(&stDrmMsg, 0, sizeof(MgrDrmAgentSendDrmMessage_t));
				memcpy(&stDrmMsg.szDrmMsgType[0], &msgType[0], 256);
				memcpy(&stDrmMsg.szDrmSystemId[0], &DRMSystemID[0], 256);
				memcpy(&stDrmMsg.szDrmMsg[0], &msg[0], 1024);
				xproc_cas_NaPrmMain(eOEVT_DRMAGENT_SENDDRMMESSAGE_NAGRA_PRM, 0, msgId, 0, (HINT32)&stDrmMsg);

				HxLOG_Print("========  <<<<<<<<< Browser RESULT >>>>>>>>>>> ===========\n");
				HxLOG_Print("======== * Msg		: %s \n", pMsg);
			}
		}
	}
	return 0;
}


STATIC int _xmag_cas_NaPrmCmdBasicFunction(void *szArgList)
{
	char *aucArg=NULL;
	HUINT8 aucStr[20+1]={0,};

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);

	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}
	
	if( snprintf(aucStr, 20, "%s", aucArg) == 0 )
	{
		return HxCMD_ERR;
	}

	HLIB_STD_StrUpper(aucStr);

	if(strcmp(aucStr, "KEY_OK") == 0 )
	{
		/* 	
		Select user mode
		입력되는 test type 변경
		AT(Auto Test)
		0.	Normal Test - default
		1.	Credential : AT
		2.	ResultCode : AT
		3.	Error handling (invalid parameters) : AT
		4.	Loading credential data
		*/
		if(_xmag_cas_NaPrmCmdIsChangeUserTestItem(KEY_OK)==ERR_OK)
			return HxCMD_OK;
	}
	else if(strcmp(aucStr, "KEY_0") == 0 )
	{
		/*
		0	PRMS::SYSINFO	DVL에 system info 요청
		*/
		if(_xmag_cas_NaPrmCmdSendDRMMessage(KEY_0) == ERR_OK)
			return HxCMD_OK;
	}
	else if(strcmp(aucStr, "KEY_1") == 0 )
	{
		/*
		1	PRMS::CHINFO	DVL에 chipset info 요청
		*/
		if(_xmag_cas_NaPrmCmdSendDRMMessage(KEY_1) == ERR_OK)
			return HxCMD_OK;
	}
	else if(strcmp(aucStr, "KEY_2") == 0 )
	{
		/*
		2	CRES::CCRE::DATA=[credential]	Credential data를 전달
		*/
		if(_xmag_cas_NaPrmCmdSendDRMMessage(KEY_2) == ERR_OK)
			return HxCMD_OK;
	}
	else if(strcmp(aucStr, "KEY_3") == 0 )
	{
		/*
		3	PRMS::PSTART::TSID=[contentID]:: CREH=[handle]	Video start 요청(TSID와 Handle 이용)
		*/
		if(_xmag_cas_NaPrmCmdSendDRMMessage(KEY_3) == ERR_OK)
			return HxCMD_OK;
	}
	else if(strcmp(aucStr, "KEY_4") == 0 )
	{
		/*
		4	PRMS::PSTOP::PLAYH=1	Video stop 요청 (Handle 이용)
		*/
		if(_xmag_cas_NaPrmCmdSendDRMMessage(KEY_4) == ERR_OK)
			return HxCMD_OK;
	}
	else if(strcmp(aucStr, "KEY_5") == 0 )
	{
		/*
		5	PRMS::PSTOPALL	Video all stop
		*/
		if(_xmag_cas_NaPrmCmdSendDRMMessage(KEY_5) == ERR_OK)
			return HxCMD_OK;
	}
	else if(strcmp(aucStr, "KEY_6") == 0 )
	{
		/*
		6	CRES::DCRE::CREH=1234	Credential 삭제 (Handle 이용)
		*/
		if(_xmag_cas_NaPrmCmdSendDRMMessage(KEY_6) == ERR_OK)
			return HxCMD_OK;
	}
	else if(strcmp(aucStr, "KEY_7") == 0 )
	{
		/*
		7	CRES::DALL	Credential 모두 삭제
		*/
		if(_xmag_cas_NaPrmCmdSendDRMMessage(KEY_7) == ERR_OK)
			return HxCMD_OK;
	}	
	else if(strcmp(aucStr, "KEY_8") == 0 )
	{
		/*
		7	CRES::DALL	Credential 모두 삭제
		*/
		if(_xmag_cas_NaPrmCmdSendDRMMessage(KEY_8) == ERR_OK)
			return HxCMD_OK;
	}
	else if(strcmp(aucStr, "KEY_9") == 0 )
	{
		/*
		9	CRES::UGMD::CREH=1234::DATA= CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC	Generic meta data : upate (Handle와 data 이용)
		*/
		if(_xmag_cas_NaPrmCmdSendDRMMessage(KEY_9) == ERR_OK)
			return HxCMD_OK;
	}
	else if(strcmp(aucStr, "KEY_RED") == 0 )
	{
		/*
		RED	CRES::USMD::CREH=1234::DATA= CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC	Specific meta data : update (Handle와 data 이용)
		*/
		if(_xmag_cas_NaPrmCmdSendDRMMessage(KEY_RED) == ERR_OK)
			return HxCMD_OK;
	}
	else if(strcmp(aucStr, "KEY_GREEN") == 0 )
	{
		/*
		GREEN	CRES::RCRE::CREH=1234	Credential : Read (Handle 이용)
		*/
		if(_xmag_cas_NaPrmCmdSendDRMMessage(KEY_GREEN) == ERR_OK)
			return HxCMD_OK;
	}
	else if(strcmp(aucStr, "KEY_YELLOW") == 0 )
	{
		/*
		YELLOW	SC_inserted	HD+Smart Card 삽입 확인 요청
		*/
		if(_xmag_cas_NaPrmCmdSendDRMMessage(KEY_YELLOW) == ERR_OK)
			return HxCMD_OK;
	}
	else if(strcmp(aucStr, "KEY_BLUE") == 0 )
	{
		/*
		YELLOW	SC_inserted	HD+Smart Card 삽입 확인 요청
		*/
		if(_xmag_cas_NaPrmCmdSendDRMMessage(KEY_BLUE) == ERR_OK)
			return HxCMD_OK;
	}

	return HxCMD_ERR;
}

/*************************************************************************************************************************************************/
/*************************************************************************************************************************************************/
/*************************************************************************************************************************************************/
/*************************************************************************************************************************************************/
/*************************************************************************************************************************************************/
/*************************************************************************************************************************************************/
/*************************************************************************************************************************************************/
/*************************************************************************************************************************************************/
/*************************************************************************************************************************************************/
#if 0//defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
#define REC_MAX_NUM						(10)
#define DEFAULT_EMI_FOR_RECORD			(0x20) // SES를 위한 EMI는 0x20이다 .다른 model인경우에는 확인이 필요핟.

typedef struct
{
	TSize usSize;
	TUnsignedInt8 *pucData;
}XmgrCas_NaCmdOutputCredentialInfo_t;

typedef struct
{
	TTransportSessionId xTSid;
	TDvlHandle	  	*pxRecordHandle;
	TDvlRecordStatus xRecordingStatus;
	TDvlRecordSessionParameters *pxRecordParameters;
	XmgrCas_NaCmdOutputCredentialInfo_t stCreInfo;
}XmgrCas_NaCmdRecordSessionInformation_t;

XmgrCas_NaCmdOutputCredentialInfo_t 	NaCmdOutputCredentialInfo;
XmgrCas_NaCmdRecordSessionInformation_t		NaCmdRecInfo[REC_MAX_NUM];
#endif

#if defined (SUPPORT_TIME_SETTING) || defined (CONFIG_DEBUG)
extern HERROR _xmgr_ca_naprm_SetTime(HUINT16 year, HUINT8 month, HUINT8 day, HUINT8 hour, HUINT8 minute, HUINT8 second);
#endif

/********************************************************************
* For Debugging
********************************************************************/

static void _xmgr_ca_NaCmd_PrintTime(void)
{
    UNIXTIME        utCurrentTime = 0;
    HxDATETIME_t    stDate;
    HERROR          hErr = ERR_OK;
    hErr = VK_CLOCK_GetTime((HULONG*)&utCurrentTime);
    if(hErr != VK_OK)
    {
        HLIB_CMD_Printf("VK_CLOCK_GetTime returned err (%d)  [%s : %d]\n",hErr, __FUNCTION__,__LINE__);
    }
    else
    {
        hErr = HLIB_DATETIME_ConvertUnixTimeToDateTime(utCurrentTime, &stDate);
        if(hErr != ERR_OK)
        {
            HLIB_CMD_Printf("HLIB_DATETIME_ConvertUnixTimeToDateTime returned err (%d)  [%s : %d]\n",hErr, __FUNCTION__,__LINE__);
        }
        else
        {
            HLIB_CMD_Printf("Current Time : %d / %d / %d \t %d : %d : %d\n", \
                    stDate.stDate.usYear, stDate.stDate.ucMonth, stDate.stDate.ucDay, stDate.stTime.ucHour, stDate.stTime.ucMinute, stDate.stTime.ucSecond);
        }
    }
}

static int _xmag_cas_NaTimeCommand(void *szArgList)
{
	char *aucArg=NULL;
	HUINT8 aucStr[20+1]={0,};

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( snprintf(aucStr, 20, "%s", aucArg) == 0 )
		return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if(strcmp(aucStr, "SET") == 0 )
	{
		int year, month, day, hour, minute, second;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &year) != 0 )
			{
				aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
				if( aucArg != NULL )
				{
					if( sscanf(aucArg, "%d", &month) != 0 )
					{
						aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
						if( aucArg != NULL )
						{
							if( sscanf(aucArg, "%d", &day) != 0 )
							{
								aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
								if( aucArg != NULL )
								{
									if( sscanf(aucArg, "%d", &hour) != 0 )
									{
										aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
										if( aucArg != NULL )
										{
											if( sscanf(aucArg, "%d", &minute) != 0 )
											{
												aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
												if( aucArg != NULL )
												{
													if( sscanf(aucArg, "%d", &second) != 0 )
													{
														if(_xmgr_ca_naprm_SetTime(year, month, day, hour, minute, second) == ERR_OK)
														{
															return HxCMD_OK;
														}
														HxLOG_Error("[Time setting is Failed - time (%d.%d.%d, %d:%d:%d)]\n", year, month, day, hour, minute, second);
													}
												}

											}
										}

									}
								}

							}
						}
					}

				}
			}
		}
	}
	else if(strcmp(aucStr, "GET") == 0 )
	{
		_xmgr_ca_NaCmd_PrintTime();
		return HxCMD_OK;
	}

	return HxCMD_ERR;
}

#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)
TUnsignedInt8				*s_strPrmSyntax_1 = "ewoiY29udGVudElkIjogIjEwMDQzNDI3IiwKImtleUlkIjoiMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDEiCn0KCg";
TUnsignedInt8				*s_strPrmSyntax_2 = "ewoiY29udGVudElkIjogIjEwMDQzNDI3IiwKImtleUlkIjoiMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDIiCn0KCg";


#if defined(__FORCED_USE_PRM_2_6__)
void _xmgr_cas_NaCmd_dvlPlaybackErrorCB(TDvlHandle* pxPlaybackHandle, TDvlAccessStatus  xPlaybackStatus)
{
	HLIB_CMD_Printf("[%s][%d] pxPlaybackHandle:[%p], xPlaybackStatus[%d]\n", __FUNCTION__, __LINE__, pxPlaybackHandle, xPlaybackStatus);
	HLIB_CMD_Printf("\n\n\n\n\033[1;92m ############################################################################ \033[0m \n");
	HLIB_CMD_Printf("\t \033[1;92m xPlaybackStatus : [%s] \033[0m \n", _xmgr_cas_naprm_GetDvlStatusName(xPlaybackStatus));
	HLIB_CMD_Printf("\033[1;92m ############################################################################ \033[0m \n\n\n\n\n");
}
#else
void _xmgr_cas_NaCmd_dvlPlaybackNotificationCB(TDvlHandle* pxPlaybackHandle, void *pxPrivateData, TDvlAccessStatus  xPlaybackStatus)
{
	HLIB_CMD_Printf("[%s][%d] pxPlaybackHandle:[%p], pxPrivateData[%p], xPlaybackStatus[%d]\n", __FUNCTION__, __LINE__, pxPlaybackHandle, pxPrivateData, xPlaybackStatus);
}
#endif

void _xmgr_cas_NaCmd_loadCredentioalData(HUINT8 ucCredNum, TSize *tFirstCredentialsSize, TUnsignedInt8 **ppFirstCredentials)
{
	HUINT8		aucFileName[128] = {0, };
	HUINT8		*paszFileName[] = {"Credential_V.bin",		/* 0 */
									"Credential_J.bin",	/* 1 */
									"Credential_L.bin",	/* 2 */
									"Credential_M.bin",	/* 3 */
									"Credential_T.bin",	/* 4 */
									"Credential_U.bin",	/* 5 */
									"Credential_C.bin",	/* 6 */
									"Credential_X.bin",	/* 7 */
									"Credential_Y.bin",	/* 8 */
									"Credential_Z.bin",	/* 9 */
									"Credential_W.bin",	/* 10 */
									"PAM_A.bin"};		/* 11 */

	if(ucCredNum > 10)
	{
		HLIB_CMD_Printf("[%s][%d] Error ucCredNum [%d]\n", __FUNCTION__, __LINE__, ucCredNum);
		return;
	}

	if(1)
	{
		HxSTD_sprintf(aucFileName, "/media/drive1/dvl/%s", paszFileName[ucCredNum]); // HDD가 삽입이 안된경우.
	}
	else
	{
		HxSTD_sprintf(aucFileName, "/media/drive2/dvl/%s", paszFileName[ucCredNum]); // HDD가 삽입된뎡우 
	}

	HLIB_CMD_Printf("Credential File Name : [%s]\n", aucFileName);

	if(HLIB_FILE_ReadFile((const HUINT8 *)aucFileName, (void **)ppFirstCredentials, tFirstCredentialsSize) != ERR_OK)
	{
		*tFirstCredentialsSize = 0;
		HLIB_CMD_Printf("[%s][%d] Error in HLIB_FILE_ReadFile[%s]\n", __FUNCTION__, __LINE__, aucFileName);
	}

	HLIB_CMD_Printf("Credential data size : [%d]\n", *tFirstCredentialsSize);

	_xmgr_ca_naprm_SetTime(2013, 3, 27, 0, 0, 0);
}

void _xmgr_cas_NaCmd_PrintCredentialsInformationt(
        TCredentialsType    xCredentialsType,
  		TCredentialsUnion* pxCredentialsUnion,
        TSize               xGenericMetadataSize,
  		TUnsignedInt8*     pxGenericMetadata,
        TSize               xSpecificMetadataSize,
  		TUnsignedInt8*     pxSpecificMetadata
)
{
	HUINT32	i;

	HLIB_CMD_Printf("###########################################################\n");
	HLIB_CMD_Printf("###########################################################\n");
	HLIB_CMD_Printf("\t xCredentialsType : %d\n", xCredentialsType);
	switch(xCredentialsType)
	{
		case CREDENTIALS_TYPE_PROFILE:
			{
				TProfileData	profileData;
				HxDATETIME_t	dateTime;

				HLIB_CMD_Printf("+\t\t CREDENTIALS_TYPE_PROFILE :	\n");
				HxSTD_memset(&profileData, 0x00, sizeof(TProfileData));
				HxSTD_memcpy(&profileData, &(pxCredentialsUnion->profileData), sizeof(TProfileData));
				HLIB_CMD_Printf("+\t\t * profileData.profileId : %ld\n", profileData.profileId);
				HLIB_CMD_Printf("+\t\t * profileData.profileVersion : %ld\n", profileData.profileVersion);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(profileData.credentialsCreationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * credentialsCreationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_DATETIME_ConvertUnixTimeToDateTime(profileData.profileExpirationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentExpirationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond);

				if (profileData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE)
				{
					HLIB_CMD_Printf("+\t\t * profileData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE \n");
				}
				else if (profileData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION)
				{
					HLIB_CMD_Printf("+\t\t * profileData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION \n");
				}
				else	// LAST_VIEWING_WINDOW_RELATIVITY
				{
					HLIB_CMD_Printf("+\t\t * profileData.contentViewingWindowRelativity : LAST_VIEWING_WINDOW_RELATIVITY \n");
				}

				HLIB_CMD_Printf("+\t\t * profileData.contentViewingWindowDuration : %ld \n", profileData.contentViewingWindowDuration);
				HLIB_CMD_Printf("+\t\t * profileData.contentRetentionDuration : %ld \n", profileData.contentRetentionDuration);
			}
			break;

		case CREDENTIALS_TYPE_VOD_EMM:
			{
				TVodEmmData		vodEmmData;
				HxDATETIME_t	dateTime;

				HLIB_CMD_Printf("+\t\t CREDENTIALS_TYPE_VOD_EMM :	\n");
				HxSTD_memset(&vodEmmData, 0x00, sizeof(TVodEmmData));
				HxSTD_memcpy(&vodEmmData, &(pxCredentialsUnion->vodEmmData), sizeof(TVodEmmData));
				HLIB_CMD_Printf("+\t\t * vodEmmData.irdSerialNumber : %ld\n", vodEmmData.irdSerialNumber);
				HLIB_CMD_Printf("+\t\t * vodEmmData.contentId : %ld\n", vodEmmData.contentId);
				HLIB_CMD_Printf("+\t\t * vodEmmData.vodEmmId : %ld \n", vodEmmData.vodEmmId);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.credentialsCreationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * credentialsCreationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.contentExpirationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentExpirationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond);

				HLIB_CMD_Printf("+\t\t * vodEmmData.contentViewingWindowDuration : %ld \n", vodEmmData.contentViewingWindowDuration);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.contentFirstVisualizationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentFirstVisualizationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.contentViewingWindowEndDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentViewingWindowEndDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				if (vodEmmData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE)
				{
					HLIB_CMD_Printf("+\t\t * vodEmmData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE \n");
				}
				else if (vodEmmData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION)
				{
					HLIB_CMD_Printf("+\t\t * vodEmmData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION \n");
				}
				else	// LAST_VIEWING_WINDOW_RELATIVITY
				{
					HLIB_CMD_Printf("+\t\t * vodEmmData.contentViewingWindowRelativity : LAST_VIEWING_WINDOW_RELATIVITY \n");
				}

				HLIB_CMD_Printf("+\t\t * vodEmmData.contentIsCiphered : %d \n", vodEmmData.contentIsCiphered);
				HLIB_CMD_Printf("+\t\t * vodEmmData.contentKeyIsProtected : %d \n", vodEmmData.contentKeyIsProtected);
				HLIB_CMD_Printf("+\t\t * vodEmmData.nuidValue : 0x%lx \n", vodEmmData.nuidValue);
			}
			break;

		case CREDENTIALS_TYPE_VOD_ECM:
			{
				TVodEcmData		vodEcmData;
				HxDATETIME_t	dateTime;

				HLIB_CMD_Printf("+\t\t CREDENTIALS_TYPE_VOD_ECM :	\n");
				HxSTD_memset(&vodEcmData, 0x00, sizeof(TVodEcmData));
				HxSTD_memcpy(&vodEcmData, &(pxCredentialsUnion->vodEcmData), sizeof(TVodEcmData));
				HLIB_CMD_Printf("+\t\t * vodEcmData.contentId : %ld\n", vodEcmData.contentId);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEcmData.contentCreationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentCreationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEcmData.contentExpirationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentExpirationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond);

				HLIB_CMD_Printf("+\t\t * vodEcmData.contentEmi : %ld \n", vodEcmData.contentEmi);
				HLIB_CMD_Printf("+\t\t * vodEmcData.contentKeyIsProtected : %d \n", vodEcmData.contentKeyIsProtected);
			}
			break;

 		case CREDENTIALS_TYPE_LCM:
			{
				TVodEmmData		vodEmmData;
				HxDATETIME_t	dateTime;

				HLIB_CMD_Printf("+\t\t CREDENTIALS_TYPE_VOD_EMM :	\n");
				HxSTD_memset(&vodEmmData, 0x00, sizeof(TVodEmmData));
				HxSTD_memcpy(&vodEmmData, &(pxCredentialsUnion->vodEmmData), sizeof(TVodEmmData));
				HLIB_CMD_Printf("+\t\t * vodEmmData.irdSerialNumber : %ld\n", vodEmmData.irdSerialNumber);
				HLIB_CMD_Printf("+\t\t * vodEmmData.contentId : %ld\n", vodEmmData.contentId);
				HLIB_CMD_Printf("+\t\t * vodEmmData.vodEmmId : %ld \n", vodEmmData.vodEmmId);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.credentialsCreationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * credentialsCreationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.contentExpirationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentExpirationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond);

				HLIB_CMD_Printf("+\t\t * vodEmmData.contentViewingWindowDuration : %ld \n", vodEmmData.contentViewingWindowDuration);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.contentFirstVisualizationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentFirstVisualizationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.contentViewingWindowEndDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentViewingWindowEndDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				if (vodEmmData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE)
				{
					HLIB_CMD_Printf("+\t\t * vodEmmData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE \n");
				}
				else if (vodEmmData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION)
				{
					HLIB_CMD_Printf("+\t\t * vodEmmData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION \n");
				}
				else	// LAST_VIEWING_WINDOW_RELATIVITY
				{
					HLIB_CMD_Printf("+\t\t * vodEmmData.contentViewingWindowRelativity : LAST_VIEWING_WINDOW_RELATIVITY \n");
				}

				HLIB_CMD_Printf("+\t\t * vodEmmData.contentIsCiphered : %d \n", vodEmmData.contentIsCiphered);
				HLIB_CMD_Printf("+\t\t * vodEmmData.contentKeyIsProtected : %d \n", vodEmmData.contentKeyIsProtected);
				HLIB_CMD_Printf("+\t\t * vodEmmData.nuidValue : 0x%lx \n", vodEmmData.nuidValue);
			}
			break;

		case CREDENTIALS_TYPE_HOME_NETWORK_LCM:

			break;

		default:

			break;
	}

	HLIB_CMD_Printf("\t xGenericMetadataSize : %d\n", xGenericMetadataSize);
	for( i = 0; i < xGenericMetadataSize; i ++)
	{
		if(i != 0 && i%7 == 0)
		{
			HLIB_CMD_Printf("\n");
		}
		HLIB_CMD_Printf("[0x%02x]", pxGenericMetadata[i]);

	}
	HLIB_CMD_Printf("\n");

	HLIB_CMD_Printf("\t xSpecificMetadataSize : %d\n", xSpecificMetadataSize);
	for( i = 0; i < xSpecificMetadataSize; i ++)
	{
		if(i != 0 && i%7 == 0)
		{
			HLIB_CMD_Printf("\n");
		}
		HLIB_CMD_Printf("[0x%02x]", pxSpecificMetadata[i]);

	}
	HLIB_CMD_Printf("\n");

	HLIB_CMD_Printf("###########################################################\n");
	HLIB_CMD_Printf("###########################################################\n");
}

static void _xmgr_cas_NaCmd_StartPrmSignalization(HUINT8 ucPrmSyntaxNum)
{
	HERROR			hErr;
	HUINT32			i, ulPrmSyntaxLen = 0;
	TUnsignedInt8	*pxPrmSyntax;
	TSize			tSize;
	TOdmHandle		*pxOpaqueDataMessageHandle;
	TUnsignedInt8	*pxOpaqueDataMessage;

	HLIB_CMD_Printf("[%s][%d], ucPrmSyntaxNum [%d]\n",__FUNCTION__, __LINE__, ucPrmSyntaxNum);

	HLIB_CMD_Printf("###### Step 1 dvlOpaqueDataMessageCreate\n");
	hErr = xmgr_cas_NadvlOpaqueDataMessageCreate(&pxOpaqueDataMessageHandle);
	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("\t Error in xmgr_cas_NadvlOpaqueDataMessageCreate\n");
		return;
	}
	HLIB_CMD_Printf("[%s][%d] pxOpaqueDataMessageHandle[%p]\n",__FUNCTION__, __LINE__, pxOpaqueDataMessageHandle);


	HLIB_CMD_Printf("###### Step 2 dvlOpaqueDataMessageSetPrmSignalization\n");
	if(ucPrmSyntaxNum == 0)
	{
		ulPrmSyntaxLen = strlen(s_strPrmSyntax_1);
		pxPrmSyntax = s_strPrmSyntax_1;

	}
	else
	{
		ulPrmSyntaxLen = strlen(s_strPrmSyntax_2);
		pxPrmSyntax = s_strPrmSyntax_2;
	}
	hErr = xmgr_cas_NadvlOpaqueDataMessageSetPrmSignalization(pxOpaqueDataMessageHandle, ulPrmSyntaxLen, pxPrmSyntax);
	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("\t Error in xmgr_cas_NadvlOpaqueDataMessageSetPrmSignalization\n");
		return;
	}

	HLIB_CMD_Printf("###### Step 3 dvlOpaqueDataMessageGet\n");
	hErr = xmgr_cas_NadvlOpaqueDataMessageGet(pxOpaqueDataMessageHandle, &tSize, (const TUnsignedInt8 **)&pxOpaqueDataMessage);
	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
		return;
	}

	HLIB_CMD_Printf("Opaque Data Message Size [%d]\n", tSize);
	for( i = 0; i < tSize; i ++)
	{
		if(i != 0 && i%7 == 0)
		{
			HLIB_CMD_Printf("\n");
		}
		HLIB_CMD_Printf("[0x%02x]", pxOpaqueDataMessage[i]);

	}
	HLIB_CMD_Printf("\n");
	HLIB_CMD_Printf("[%s]\n", pxOpaqueDataMessage);

	HLIB_CMD_Printf("###### Step 4 dvlOpaqueDataMessageRelease\n");
	hErr = xmgr_cas_NadvlOpaqueDataMessageRelease(pxOpaqueDataMessageHandle);

	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
		return;
	}

	HLIB_CMD_Printf("###### Step 5 dvlReleaseBuffer\n");
	hErr = xmgr_cas_NadvlReleaseBuffer(pxOpaqueDataMessage);
	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
		return;
	}

}


TTransportSessionId s_xTSid = 2;
TUnsignedInt8		*s_pFirstCredentials = NULL;
TDvlHandle			*s_pxPlaybackHandle = NULL;
TUnsignedInt8		*s_pxOutputCredentials = NULL;

static void _xmgr_cas_NaCmd_RelaseBuffer(void)
{
	HERROR hErr;

	HLIB_CMD_Printf("###### [%s]\n", __FUNCTION__);

	if(s_pxOutputCredentials != NULL)
	{
		hErr = xmgr_cas_NadvlReleaseBuffer(s_pxOutputCredentials);
		if(hErr != ERR_OK)
		{
			HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
		}

		s_pxOutputCredentials = NULL;
	}

	if(s_pFirstCredentials != NULL)
	{
		HLIB_MEM_Free(s_pFirstCredentials);

		s_pFirstCredentials = NULL;
	}

}

static void _xmgr_cas_NaCmd_CloseSession(void)
{
	HERROR hErr;

	HLIB_CMD_Printf("###### [%s]\n", __FUNCTION__);

	_xmgr_cas_NaCmd_RelaseBuffer();

	if(s_pxPlaybackHandle != NULL)
	{
#if defined(__FORCED_USE_PRM_2_6__)
		hErr = xmgr_cas_NadvlStopPlayback(s_pxPlaybackHandle);
#else
		hErr = xmgr_cas_NadvlPlaybackCloseSession(s_pxPlaybackHandle);
#endif
		if(hErr != ERR_OK)
		{
			HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
		}

		s_pxPlaybackHandle = NULL;

		hErr = xmgr_cas_RemoveTransportSessionIdDirect(s_xTSid);
		if(hErr != ERR_OK)
		{
			HLIB_CMD_Printf("\t Error in xmgr_cas_RemoveTransportSessionIdDirect\n");
			return;
		}
	}
}


static void _xmgr_cas_NaCmd_StartVODPlayback(HUINT32 ulTsId, HUINT8 ucCredNum)
{
	HERROR				hErr;
	HUINT32				/*i, */steps = 0;

	TCredentialsType 	xCredentialsType;
	TCredentialsUnion	*pxCredentialsUnion;
	TSize				xGenericMetadataSize;
	TUnsignedInt8		*pxGenericMetadata;
	TSize			 	xSpecificMetadataSize;
	TUnsignedInt8		*pxSpecificMetadata;

	TSize				tFirstCredentialsSize;

	TDvlAccessStatus			xPlaybackStatus;
	TSize						xOutputCredentialsSize;
#if !defined(__FORCED_USE_PRM_2_6__)
	TDvlEntitlementsParameters	stEntitlementsParameters;
	TSize						xKeyIdSize;
	TUnsignedInt8				*pxKeyId;
#endif

	HLIB_CMD_Printf("[%s][%d], ucCredNum [%d]\n",__FUNCTION__, __LINE__, ucCredNum);

	HLIB_CMD_Printf("###### Step [%d] xmgr_cas_AddTransportSessionIdForPrm\n", steps++);
	s_xTSid = ulTsId;
	hErr = xmgr_cas_AddTransportSessionIdDirect(s_xTSid);
	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("\t Error in xmgr_cas_AddTransportSessionIdDirect\n");
		return;
	}

#if !defined(__FORCED_USE_PRM_2_6__)
	HLIB_CMD_Printf("###### Step [%d] dvlPlaybackOpenSession\n", steps++);
	hErr = xmgr_cas_NadvlPlaybackOpenSession(s_xTSid, &s_pxPlaybackHandle);
	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("\t Error in xmgr_cas_NadvlOpaqueDataMessageCreate\n");
		return;
	}
	HLIB_CMD_Printf("[%s][%d] s_pxPlaybackHandle[%p]\n",__FUNCTION__, __LINE__, s_pxPlaybackHandle);
#endif

	HLIB_CMD_Printf("###### Step [%d] _xmgr_cas_NaCmd_loadCredentioalData\n", steps++);
	_xmgr_cas_NaCmd_loadCredentioalData(ucCredNum, &tFirstCredentialsSize, &s_pFirstCredentials);
	if(tFirstCredentialsSize == 0)
	{
		HLIB_CMD_Printf("[%s][%d] Error CredentialSize : [%d]\n", __FUNCTION__, __LINE__, tFirstCredentialsSize);
		_xmgr_cas_NaCmd_CloseSession();
		return;
	}

	HLIB_CMD_Printf("###### Step [%d] xmgr_cas_NadvlCredentialsInformationRequest\n", steps++);
	hErr = xmgr_cas_NadvlCredentialsInformationRequest(tFirstCredentialsSize, (const TUnsignedInt8 *)s_pFirstCredentials, &xCredentialsType, (const TCredentialsUnion **)&pxCredentialsUnion, &xGenericMetadataSize, (const TUnsignedInt8 **)&pxGenericMetadata, &xSpecificMetadataSize, (const TUnsignedInt8 **)&pxSpecificMetadata);

	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("\t Error in xmgr_cas_NadvlCredentialsInformationRequest\n");
	}
	else
	{
		_xmgr_cas_NaCmd_PrintCredentialsInformationt(xCredentialsType, pxCredentialsUnion, xGenericMetadataSize, pxGenericMetadata, xSpecificMetadataSize, pxSpecificMetadata);

		if(pxCredentialsUnion != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer((const TUnsignedInt8 *)pxCredentialsUnion);
			if(hErr != ERR_OK)
			{
				HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
			}
		}

		if(pxGenericMetadata != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer(pxGenericMetadata);
			if(hErr != ERR_OK)
			{
				HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
			}
		}

		if(pxSpecificMetadata != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer(pxSpecificMetadata);
			if(hErr != ERR_OK)
			{
				HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
			}
		}
	}

#if defined(__FORCED_USE_PRM_2_6__)
	HLIB_CMD_Printf("###### Step [%d] xmgr_cas_NadvlStartPlayback\n", steps++);
	hErr = xmgr_cas_NadvlStartPlayback(s_xTSid
										, tFirstCredentialsSize
										, s_pFirstCredentials
										, _xmgr_cas_NaCmd_dvlPlaybackErrorCB
										, &xOutputCredentialsSize
										, (const TUnsignedInt8 **)&s_pxOutputCredentials
										, &xPlaybackStatus
										, &xGenericMetadataSize
										, (const TUnsignedInt8 **)&pxGenericMetadata
										, &xSpecificMetadataSize
										, (const TUnsignedInt8 **)&pxSpecificMetadata
										, &s_pxPlaybackHandle);
#else
	stEntitlementsParameters.firstCredentialsSize = tFirstCredentialsSize;
	stEntitlementsParameters.firstCredentials = s_pFirstCredentials;
	stEntitlementsParameters.secondCredentialsSize = 0;
	stEntitlementsParameters.secondCredentials = NULL;
	stEntitlementsParameters.playbackNotificationCB = _xmgr_cas_NaCmd_dvlPlaybackNotificationCB;
	stEntitlementsParameters.privateData = NULL;
	hErr = xmgr_cas_NadvlPlaybackSetEntitlements(s_pxPlaybackHandle, &stEntitlementsParameters, &xPlaybackStatus, &xOutputCredentialsSize, (const TUnsignedInt8 **)&s_pxOutputCredentials, &xKeyIdSize, (const TUnsignedInt8 **)&pxKeyId);
	HLIB_CMD_Printf("[%s][%d] ppxOutputCredentials [%p], pxKeyIdSize[%p], ppxKeyId[%p]\n", __FUNCTION__, __HxLINE__, &s_pxOutputCredentials, &xKeyIdSize, &pxKeyId);
#endif
	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("\t Error in xmgr_cas_NadvlPlaybackSetEntitlements\n");
		_xmgr_cas_NaCmd_CloseSession();
		return;
	}

	HLIB_CMD_Printf("xOutputCredentialsSize [%d], s_pxOutputCredentials [%p], s_pxOutputCredentials [%p]\n", xOutputCredentialsSize, s_pxOutputCredentials, &s_pxOutputCredentials);
#if 0
	for( i = 0; i < xOutputCredentialsSize; i ++)
	{
		if(i != 0 && i%7 == 0)
		{
			HLIB_CMD_Printf("\n");
		}
		HLIB_CMD_Printf("[0x%02x]", s_pxOutputCredentials[i]);

	}
	HLIB_CMD_Printf("\n");
#endif

#if 0
	HLIB_CMD_Printf("xKeyIdSize [%d]\n", xKeyIdSize);

	for( i = 0; i < xKeyIdSize; i ++)
	{
		if(i != 0 && i%7 == 0)
		{
			HLIB_CMD_Printf("\n");
		}
		HLIB_CMD_Printf("[0x%02x]", pxKeyId[i]);

	}
	HLIB_CMD_Printf("\n");
#endif

	HLIB_CMD_Printf("###### Step [%d] xmgr_cas_NadvlCredentialsInformationRequest\n", steps++);
	hErr = xmgr_cas_NadvlCredentialsInformationRequest(xOutputCredentialsSize, (const TUnsignedInt8 *)s_pxOutputCredentials, &xCredentialsType, (const TCredentialsUnion **)&pxCredentialsUnion, &xGenericMetadataSize, (const TUnsignedInt8 **)&pxGenericMetadata, &xSpecificMetadataSize, (const TUnsignedInt8 **)&pxSpecificMetadata);
	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("\t Error in xmgr_cas_NadvlCredentialsInformationRequest\n");
	}
	else
	{
		_xmgr_cas_NaCmd_PrintCredentialsInformationt(xCredentialsType, pxCredentialsUnion, xGenericMetadataSize, pxGenericMetadata, xSpecificMetadataSize, pxSpecificMetadata);

		if(pxCredentialsUnion != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer((const TUnsignedInt8 *)pxCredentialsUnion);
			if(hErr != ERR_OK)
			{
				HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
			}
		}

		if(pxGenericMetadata != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer(pxGenericMetadata);
			if(hErr != ERR_OK)
			{
				HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
			}
		}

		if(pxSpecificMetadata != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer(pxSpecificMetadata);
			if(hErr != ERR_OK)
			{
				HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
			}
		}
	}

	HLIB_CMD_Printf("###### Step [%d] End of [%s]\n", steps++ , __FUNCTION__);

	HLIB_CMD_Printf("\n\n\n\n\033[1;92m ############################################################################ \033[0m \n");
	HLIB_CMD_Printf("\t \033[1;92m xPlaybackStatus : [%s] \033[0m \n", _xmgr_cas_naprm_GetDvlStatusName(xPlaybackStatus));
	HLIB_CMD_Printf("\033[1;92m ############################################################################ \033[0m \n\n\n\n\n");

}

static void _xmgr_cas_NaCmd_StopVODPlayback(void)
{
	HLIB_CMD_Printf("###### [%s]\n", __FUNCTION__);
	_xmgr_cas_NaCmd_CloseSession();
}

#if defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
STATIC void _xmgr_cas_NaCmd_PrintRecordSession(void)
{
	int i = 0;
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	HxLOG_Debug("\n=====================================================================\n");
	HxLOG_Debug("+			DVL TEST CMD : RECORD INFO\n");
	HxLOG_Debug("=====================================================================\n");

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &NaCmdRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}

		HxLOG_Debug("[%d] xTSid(0x%08x), pxRecordHandle(0x%08x), pxRecordParameters(0x%08x), xRecordingStatus(%d) ---- ", i, pRecInfo->xTSid, pRecInfo->pxRecordHandle, pRecInfo->pxRecordParameters, pRecInfo->xRecordingStatus);
		if(pRecInfo->stCreInfo.pucData)
		{
			HxLOG_Debug("CredentialData : OK..........CreInfoSize(%d)\n", pRecInfo->stCreInfo.usSize);
		}
		else
		{
			HxLOG_Debug("CredentialData : Not OK..........CreInfoSize(%d)\n", pRecInfo->stCreInfo.usSize);
		}
	}
	HxLOG_Debug("\n");
}

STATIC void _xmgr_cas_NaCmd_InitRecordSessionByIndex(int index)
{
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = &NaCmdRecInfo[index];

	if(pRecInfo)
	{
		if(pRecInfo->pxRecordParameters)
		{
			xmgr_cas_NadvlReleaseBuffer((TUnsignedInt8 *)pRecInfo->pxRecordParameters);
			pRecInfo->pxRecordParameters = NULL;
		}

		pRecInfo->xTSid = 0xff;
		pRecInfo->pxRecordHandle = NULL;
		pRecInfo->xRecordingStatus = DVL_RECORD_GRANTED;
		pRecInfo->stCreInfo.usSize = 0;

		if(pRecInfo->stCreInfo.pucData)
		{
			HLIB_STD_MemFree(pRecInfo->stCreInfo.pucData);
			pRecInfo->stCreInfo.pucData = NULL;
		}
	}
}

STATIC void _xmgr_cas_NaCmd_InitRecordSession(void)
{
	int i = 0;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		_xmgr_cas_NaCmd_InitRecordSessionByIndex(i);
	}
}

STATIC HERROR _xmgr_cas_NaCmd_MakeRecordSession(TDvlRecordSessionParameters *pxRecordParameters, TTransportSessionId xTSid, TDvlHandle* pxRecordHandle, TSize xCredentialsSize, const TUnsignedInt8* pxCredentials, TDvlRecordStatus xRecordingStatus)
{
	int i = 0;
	HERROR hErr = ERR_FAIL;

	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &NaCmdRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}

		if(pRecInfo->stCreInfo.pucData == NULL)
		{
			pRecInfo->stCreInfo.pucData = (HUINT8*)HLIB_STD_MemAlloc(xCredentialsSize + 1);
			if(pRecInfo->stCreInfo.pucData)
			{
				pRecInfo->pxRecordParameters = pxRecordParameters;
				pRecInfo->pxRecordHandle = pxRecordHandle;
				pRecInfo->xTSid = xTSid;
				pRecInfo->xRecordingStatus = xRecordingStatus;

				HxSTD_memset(pRecInfo->stCreInfo.pucData, 0, xCredentialsSize + 1);
				HxSTD_memcpy(pRecInfo->stCreInfo.pucData, pxCredentials, xCredentialsSize);
				pRecInfo->stCreInfo.usSize = xCredentialsSize;
				
				HxLOG_Debug("[%s][%d] index[%d] = xTSid(%d), pxRecordHandle(0x%08x), CredentialSize(%d), xRecordingStatus(%d)\n", __FUNCTION__, __LINE__, i, xTSid, pxRecordHandle, xCredentialsSize, xRecordingStatus);
				hErr = ERR_OK;
				break;
			}
			else
			{
				HxLOG_Critical("HLIB_STD_MemAlloc Error\n");
			}
		}
	}
	return hErr;
}

STATIC HERROR _xmgr_cas_NaCmd_FreeRecordSessionFromRecordHandle(TDvlHandle* pxRecordHandle)
{
	int i = 0;
	HERROR hErr = ERR_FAIL;
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &NaCmdRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}

		if(pRecInfo->pxRecordHandle == pxRecordHandle)
		{
			_xmgr_cas_NaCmd_InitRecordSessionByIndex(i);
			hErr = ERR_OK;
			HxLOG_Debug("[%s][%d] index[%d] = pxRecordHandle(0x%08x)\n", __FUNCTION__, __LINE__, i, pxRecordHandle);
			break;
		}
	}
	return hErr;
}

STATIC HERROR _xmgr_cas_NaCmd_FreeRecordSessionFromTsId(TTransportSessionId xTSid)
{
	int i = 0;
	HERROR hErr = ERR_FAIL;
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &NaCmdRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}
		
		if(pRecInfo->xTSid == xTSid)
		{
			_xmgr_cas_NaCmd_InitRecordSessionByIndex(i); 
			hErr = ERR_OK;
			HxLOG_Debug("[%s][%d] index[%d] = xTSid(%d)\n", __FUNCTION__, __LINE__, i, xTSid);
			break;
		}
	}
	return hErr;
}

STATIC XmgrCas_NaCmdRecordSessionInformation_t * _xmgr_cas_NaCmd_GetRecordSessionFromTsId(TTransportSessionId xTSid)
{
	int i = 0;
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &NaCmdRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}

		if(pRecInfo->xTSid == xTSid)
		{
			HxLOG_Debug("[%s][%d] index[%d] = xTSid(%d)\n", __FUNCTION__, __LINE__, i, xTSid);
			return &NaCmdRecInfo[i];
		}
	}
	HxLOG_Debug("[%s][%d] index[%d] = xTSid(%d) is not found\n", __FUNCTION__, __LINE__, i, xTSid);
	return NULL;
}

STATIC XmgrCas_NaCmdRecordSessionInformation_t * _xmgr_cas_NaCmd_GetRecordSessionFromRecordHandle(TDvlHandle* pxRecordHandle)
{
	int i = 0;
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &NaCmdRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}

		if(pRecInfo->pxRecordHandle == pxRecordHandle)
		{
			HxLOG_Debug("[%s][%d] index[%d] = pxRecordHandle(0x%08x)\n", __FUNCTION__, __LINE__, i, pxRecordHandle);
			return &NaCmdRecInfo[i];
		}
	}
	return NULL;	
}

STATIC void _xmgr_cas_NaCmd_dvlRecordErrorCB(TDvlHandle* pxRecordHandle, TDvlRecordStatus  xRecordStatus)
{
	HLIB_CMD_Printf("[%s][%d] pxRecordHandle:[%p], xRecordStatus[%d]\n", __FUNCTION__, __LINE__, pxRecordHandle, xRecordStatus);
	HLIB_CMD_Printf("\n\n\n\n\033[1;92m ############################################################################ \033[0m \n");
	HLIB_CMD_Printf("\t \033[1;92m xRecordStatus : [%s] \033[0m \n", _xmgr_cas_naprm_GetDvlStatusName(xRecordStatus));
	HLIB_CMD_Printf("\033[1;92m ############################################################################ \033[0m \n\n\n\n\n");
}

STATIC void _xmgr_cas_NaCmd_StartRecordSession(int tsid)
{
	TDvlRecordSessionParameters *pxRecordParameters = NULL;
	TSize xOutputCredentialsSize = 0;
	const TUnsignedInt8* pxOutputCredentials = NULL;
	TDvlRecordStatus xRecordingStatus = DVL_RECORD_GRANTED;
	TDvlHandle* pxRecordHandle = NULL;
	static HBOOL bRecInitFlag = TRUE;

	TSize					xSpecificMetadataSize = 3;
	TUnsignedInt8			xSpecificMetadata[3] = {0x12, 0x7a, 0x14};//{0xa5, 0x04, 0x01, 0xf0,  0x04, 0x38};

	if(bRecInitFlag == TRUE)
	{
		_xmgr_cas_NaCmd_InitRecordSession();
		bRecInitFlag = FALSE;
	}

	if(xmgr_cas_NadvlCreateRecordSessionParameters((TDvlRecordSessionParameters**)&pxRecordParameters) == ERR_OK)
	{
		pxRecordParameters->tsId = tsid; // tsid를 등록해야함..
		pxRecordParameters->pErrorCB = _xmgr_cas_NaCmd_dvlRecordErrorCB;
		pxRecordParameters->collectionId = 0;  // ?????
		pxRecordParameters->inputCredentialsSize = 0; // inpurt credential size는 없음.
		pxRecordParameters->pInputCredentials = NULL; // input credentials은 없음
		pxRecordParameters->specificMetadataSize = xSpecificMetadataSize; // meta 정보를 얻어와 set해야함.
		pxRecordParameters->pSpecificMetadata = &xSpecificMetadata[0]; // meta 정보를 얻어와 set해야함.
		pxRecordParameters->viewingWindowRelativity = VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE; // meta 정보를 얻어와 set해야함.
		pxRecordParameters->viewingWindowDuration = 60; // meta 정보를 얻어와 set해야함.
		pxRecordParameters->retentionDuration = 60; // meta 정보를 얻어와 set해야함.		
		pxRecordParameters->emi = DEFAULT_EMI_FOR_RECORD;

		xmgr_cas_naprm_pvr_PrintRecordSessionParameters(pxRecordParameters);

		if(xmgr_cas_NadvlStartRecordEx(/*&xRecordParam,*/ pxRecordParameters,
										&xOutputCredentialsSize,
										&pxOutputCredentials,
										&xRecordingStatus,
										&pxRecordHandle) == ERR_OK)
		{
			if(xOutputCredentialsSize == 0 || pxOutputCredentials == NULL)
			{
				HxLOG_Error("pxOutputCredentials is not fail\n");
				return;
			}

			xmgr_cas_naprm_PrintHexDump("Outout Credential", (HUINT8*)pxOutputCredentials, (HUINT32)xOutputCredentialsSize);

			_xmgr_cas_NaCmd_MakeRecordSession(pxRecordParameters, tsid, pxRecordHandle, xOutputCredentialsSize, pxOutputCredentials, xRecordingStatus);

			if(pxOutputCredentials)
			{
				xmgr_cas_NadvlReleaseBuffer(pxOutputCredentials);
			}
		}
		else
		{
			HxLOG_Error("xmgr_cas_NadvlStartRecordEx() is fail\n");
		}
	}
	else
	{
		HxLOG_Error("xmgr_cas_NadvlCreateRecordSessionParameters() is fail\n");		
	}
	_xmgr_cas_NaCmd_PrintRecordSession();
}

STATIC void _xmgr_cas_NaCmd_StopRecordSession(int tsId)
{
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	_xmgr_cas_NaCmd_PrintRecordSession();
	pRecInfo = _xmgr_cas_NaCmd_GetRecordSessionFromTsId(tsId);

	if(pRecInfo)
	{
		(void)xmgr_cas_NadvlStopRecord(pRecInfo->pxRecordHandle);
		(void)_xmgr_cas_NaCmd_FreeRecordSessionFromRecordHandle(pRecInfo->pxRecordHandle);		
	}
}
#endif

static int _xmag_cas_NaPrmCommand(void *szArgList)
{
	char *aucArg=NULL;
	HUINT8 aucStr[20+1]={0,};

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( snprintf(aucStr, 20, "%s", aucArg) == 0 )
		return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if(strcmp(aucStr, "SIG") == 0 )
	{
		int cnt;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &cnt) != 0 )
			{
				_xmgr_cas_NaCmd_StartPrmSignalization(cnt);
				return HxCMD_OK;
			}
		}
	}
	else if(strcmp(aucStr, "PSTART") == 0 )
	{
		int tsid, cred_num;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &tsid) != 0 )
			{
				aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
				if( aucArg != NULL )
				{
					if( sscanf(aucArg, "%d", &cred_num) != 0 )
					{

						_xmgr_cas_NaCmd_StartVODPlayback(tsid, cred_num);
						return HxCMD_OK;
					}
				}
			}
		}
	}
	else if(strcmp(aucStr, "PSTOP") == 0 )
	{
		_xmgr_cas_NaCmd_StopVODPlayback();
		return HxCMD_OK;
	}
#if defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
	else if(strcmp(aucStr, "RSTART") == 0 )
	{
		int tsid;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &tsid) != 0 )
			{
				_xmgr_cas_NaCmd_StartRecordSession(tsid);
				return HxCMD_OK;
			}
		}
		return HxCMD_OK;
	}
	else if(strcmp(aucStr, "RSTOP") == 0 )
	{
		int tsid;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &tsid) != 0 )
			{
				_xmgr_cas_NaCmd_StopRecordSession(tsid);
				return HxCMD_OK;
			}
		}
		return HxCMD_OK;
	}
#endif

	return HxCMD_ERR;
}
#endif

/* TODO : 아래 Command 정리하자 */
void xmgr_cas_NaPrmCmd(HxCMD_Handle_t hCmdHandle)
{
	/* dvl basic functions : 테스트 */
	HLIB_CMD_RegisterWord(hCmdHandle,
						_xmag_cas_NaPrmCmdBasicFunction,
						"basiccmdprm",						/* keyword */
						"Nagra DVL Debug, TV Manager\n cmddvl [cmd] (start[tsid][cerd_num] | stop | restart[tsid])",	/* help   */
						"cmddvl [cmd] ( start [tsid][cerd_num] | stop | restart [tsid])");	/* usage  */

	/* For dvl 인증 */
#if 1//defined(CONFIG_MW_CAS_NAGRA_PRM_2_6_DVL_TEST)
	/* DVL VER 2.6의 DVL Integration Test를 위해 사용됨 */
	xmgr_cas_NaDvlCmdInit(hCmdHandle);
#endif
#if defined (CONFIG_MW_CAS_NAGRA_PRM_3_1)
	HLIB_CMD_RegisterWord(hCmdHandle,
						_xmag_cas_NaPrmCommand,
						"cmdprm",						/* keyword */
						"Nagra DVL Debug, TV Manager\n cmdprm [cmd] (Sig | start[tsid][cerd_num] | stop)",	/* help   */
						"cmdprm [cmd] ( Sig [num] | start [tsid][cerd_num] | stop)");	/* usage  */
#endif

	HLIB_CMD_RegisterWord(hCmdHandle,
					_xmag_cas_NaTimeCommand,
					"time",						/* keyword */
					"Time Setting. time [set|get] [yyyy mm dd hh mm ss]",	/* help   */
					"time [set|get] [yyyy mm dd hh mm ss (set only)]");	/* usage  */
}
#endif
