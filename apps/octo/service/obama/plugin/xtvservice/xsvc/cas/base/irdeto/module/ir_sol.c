#if defined(CONFIG_MW_CAS_IRDETO_SOL)

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include 	"octo_common.h"
#include	"util.h"

#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include	"vd_humax.h"
#include	"ir_svc.h"
#include	"ir_sol.h"
#include	"ir_loader.h"
#include	"ir_api.h"
#include 	"ir_platform.h"
#include 	"pal_cipher.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define ________Debugging_Macro________





#define SOL_DUMP_RSA_DATA		FALSE
#define SOL_DUMP_SI_DATA		FALSE

#define SOL_KEY_SIZE			(128)
#define SOL_SGN_SIZE			(128)
#define SOL_TSID_CNT_MAX 		(64)
#define SOL_ONID_CNT_MAX 		(4)
#define SOL_DELIVERY_CNT_MAX 	(SOL_ONID_CNT_MAX * SOL_TSID_CNT_MAX)


#define SOL_DELIVERY_DESC_LEN	0x0D
#define SOL_INVALID_VAL 		(0xFFFF)


#define IRDETO_GENERIC_KEY_SYSTEM_ID	43
#if defined(CONFIG_OP_TRUEVISION)
#define UBC_KEY_SYSTEM_ID		5
#endif
#define HUMAX_KEY_SYSTEM_ID 	15

/** Set the customer system ID **/
#if defined(CONFIG_OP_TRUEVISION)
#define CUSTOMER_KEY_SYSTEM_ID		UBC_KEY_SYSTEM_ID
#else
#error "CUSTOMER_KEY_SYSTEM_ID is not defined yet! See ir_sol.h"
#endif


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	/*Do NOT TRY TO ADD ANY ITEM OR CHANGE MAPPED VALUE*/
	SOL_BEHAVIOUR_DEFAULT = 0x00,
	SOL_BEHAVIOUR_ALLOW_FTA = 0x01,
	SOL_BEHAVIOUR_ALLOW_FTA_WHEN_SC_INSERTED = 0x02,
	SOL_BEHAVIOUR_ALLOW_FTA_WHEN_SC_INSERTED_WITH_ONE_PRODUCT_SC = 0x03,
	SOL_BEHAVIOUR_DISABLE = 0x04
}SOL_BEHAVIOUR;

typedef enum
{
	/* declaralation for bit wised operation */
	SOL_STATE_INITIALIZED 					= 0x01,
	SOL_STATE_BAT_MUXTP_SIGN_RECEVIED		= (0x01 << 1),
	SOL_STATE_NIT_DELIVERY_RECEVIED			= (0x01 << 2),
	SOL_STATE_DO_START_VALIDATION			=(SOL_STATE_INITIALIZED	| SOL_STATE_BAT_MUXTP_SIGN_RECEVIED | SOL_STATE_NIT_DELIVERY_RECEVIED )
}SOL_OPERATION_STATE;

typedef struct
{
//	HUINT8 		bUsed;
	HUINT16 	ONID;
	HUINT16 	TSID;

	HUINT8 		aucDeliveryDesc[SOL_DELIVERY_DESC_LEN];
	HUINT8		aucNITCRC[4];
}SOL_DELIVERY_DESC;

typedef struct
{
	HUINT8	bNetworkValid;
	unsigned short usONID;
	unsigned short ausTSID[SOL_TSID_CNT_MAX];

}SOL_REG_SVC;

typedef struct
{
	HUINT32		ulMuxdataSemID;

	HUINT8 		ucMuxTpDescVer;
	HUINT8		ucBehavior;
	HUINT16		usDurationMin; /*Not yet supported in PiSys, currently it always has a value of 0xFFFF, that means infinite period*/
	HUINT32 	ulSolTimerId;

	HUINT8		ucMuxSignVer;
	HUINT8		aucMuxSign[SOL_SGN_SIZE];

	HUINT8		*pucMuxTpDesc;

	SOL_VERIFY_STATE		eActivatedSOL;

	/* smartcard information */
	/* following items will be used for function "SOL_is_service_blocked" */
	HUINT8		bSCValidIR;	/* SC valid status : irdeto card or not */
	HUINT8		bSCValidProduct;/*SC Product valid*/

	SOL_REG_SVC		stSolSvc[SOL_ONID_CNT_MAX];		/*MUX_TRANSPORT_LIST_DESCRIPTOR*/
	HUINT32		ulBatCRC;
	HUINT32		ulNitCRC[CAS_MAX_SECTION_NUM];


}SOL_MUXTP_INFO;

typedef struct
{
	HUINT16		usSOL_Key_system_id;
	HUINT8 	*	pucSOL_Key;

} SOL_KEY;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static SOL_MUXTP_INFO		sstMuxTPInfo;
static SOL_MUXTP_INFO		sstVerifiedMuxTPInfo;
static SOL_KEY				sstSOL_KEY;

static SOL_OPERATION_STATE		s_stateSOL;
static SOL_DELIVERY_DESC		sol_delivery[SOL_DELIVERY_CNT_MAX];
static HUINT8 					s_IsBatChanged = FALSE;
static HUINT8 					s_IsNitChanged = FALSE;

static HUINT8	IRDETO_RSA_EXP_VAL[]={ 0x03 };/*THIS VALUE WILL NEVER BE CHANGEABLE. NO RESEASON */
static HUINT8	SOL_DEVELOPMENT_KEY[SOL_KEY_SIZE]=
{
	0xAF,0x88,0x69,0x94,0xDB,0x83,0x62,0xF1,0x57,0x4F,0x94,0xB6,0x99,0xA9,0x1E,0xE0,
	0xC9,0x98,0xF7,0x0B,0x8E,0xA0,0x5C,0x46,0x59,0xBB,0xB9,0x4D,0x4F,0xF4,0xB7,0xCF,
	0xAB,0xAD,0x5A,0xBC,0xF1,0x01,0x39,0xC9,0x8E,0xE8,0x00,0xB7,0xFD,0x69,0x38,0xDA,
	0xDB,0x95,0x8D,0x83,0xE9,0xE6,0x47,0xFF,0xC7,0x37,0xC7,0x5B,0x0E,0x88,0xFA,0x1C,
	0x16,0x02,0xC1,0xF7,0xC9,0x60,0xB6,0x30,0x3B,0x02,0x36,0xB9,0x4E,0x7C,0xC5,0x64,
	0xF7,0xFA,0x7E,0x77,0x3D,0xE7,0xEC,0x5F,0x59,0xEC,0x29,0x71,0x8C,0x35,0x4A,0x32,
	0x10,0xFE,0x0D,0x04,0x45,0xDB,0x44,0x2A,0xCF,0x73,0xFD,0x68,0x46,0x7E,0x1A,0x4E,
	0xBB,0x43,0x31,0x98,0x3B,0x20,0x85,0xA0,0x95,0x3B,0x32,0xF6,0xE0,0x97,0x31,0x6B
};

#if defined(CONFIG_OP_TRUEVISION)
static HUINT8 SOL_UBC_KEY[SOL_KEY_SIZE]=
{
	/* following is UBC/True vision's SOL key */
	0xDD,0xD6,0x15,0x4A,0x23,0x71,0xE6,0x19,0x25,0xB5,0xF0,0x88,0xE8,0xCB,0xEA,0x1F
	,0x1F,0xA9,0x02,0x72,0x98,0x0C,0x33,0xD8,0xAE,0xCF,0x66,0x12,0x15,0xDE,0x0A,0x5A
	,0xE9,0x9D,0xEA,0x44,0x64,0x62,0x97,0x8D,0x3E,0x52,0x85,0x88,0xF7,0xB1,0x95,0x5A
	,0x43,0x32,0xB0,0x55,0x0F,0x69,0xFD,0x54,0x18,0x89,0xCA,0x2A,0x31,0xFD,0x4F,0x7C
	,0x07,0x6D,0x61,0x0D,0xEF,0xC6,0xFF,0x8B,0x81,0xF3,0x9F,0x01,0x8B,0xBF,0x36,0xB8
	,0xFC,0xBA,0xF4,0xFA,0x57,0xBC,0x51,0xFB,0xE1,0x6B,0x2C,0x55,0x08,0xA1,0xD9,0x11
	,0xFD,0x5B,0x3B,0x2E,0xC4,0x8A,0x47,0x17,0x46,0xE4,0x79,0x67,0xFE,0x5D,0x34,0x3D
	,0x73,0x87,0x2E,0x8C,0xAC,0x3A,0xE8,0x9F,0x80,0x64,0xD4,0xE6,0xD0,0x33,0x89,0x45
};
#endif

/*******************************************************************/
/********************     declare Functions	   *********************/
/*******************************************************************/
static void 	local_cas_ir_SOL_Clear_State(void);
static HIR_ERR	local_cas_ir_SOL_set_key(void);
static HIR_ERR 	local_cas_ir_SOL_Validate(void);
static HIR_ERR 	local_cas_ir_SOL_SendToAPP_UpdateNeeded(void);
static HUINT32 	local_cas_ir_Verify_Signature(unsigned char * pucSign, int nSign_len, unsigned char *pucMsg, int nMsg_len);
static void 	local_cas_ir_SOL_TimerCallback(unsigned long TimerId,  void *pArg);


/*******************************************************************/
/********************     definition Functions    ******************/
/*******************************************************************/
#define ________Local_Definitions________

#define ENTER_SOL_INFO 		VK_SEM_Get(sstMuxTPInfo.ulMuxdataSemID)
#define LEAVE_SOL_INFO 		VK_SEM_Release(sstMuxTPInfo.ulMuxdataSemID)

/***************************************************************************/
#define ________Local_Functions________
/***************************************************************************/
static HIR_ERR local_cas_ir_SOL_set_key(void)
{
	IHL_STATUS stIRLoaderStatus;

	if( xsvc_cas_IrLoaderGetStatusInfo(&stIRLoaderStatus) )
	{
		return HIR_NOT_OK;
	}

	HxLOG_Info("SetKey: SysID = %d, Variant= %d\n", stIRLoaderStatus.u16SysID, stIRLoaderStatus.u16Variant);

	sstSOL_KEY.pucSOL_Key = NULL;

	if(stIRLoaderStatus.u16SysID == (HUINT16) -1 ) /*Development Key*/
	{
		/* when using Development key, it shall be blocked other variant except variant 43 */
		if( stIRLoaderStatus.u16Variant == (HUINT16 ) IRDETO_GENERIC_KEY_SYSTEM_ID)
		{
			/*use SOL_development_key for DVT or Humax Lab */
			HxLOG_Info("DEV_SYS_ID & SOL_DEVELOP_KEY\n");
			sstSOL_KEY.pucSOL_Key = SOL_DEVELOPMENT_KEY;
		}
#if defined(CONFIG_OP_TRUEVISION)
		else if(stIRLoaderStatus.u16Variant == (IHL_U16)UBC_KEY_SYSTEM_ID )
		{
			/*use corresponding SOL_operator_key for Field-Trial*/
			HxLOG_Info("DEV_SYS_ID & SOL_UBC_KEY\n");
			sstSOL_KEY.pucSOL_Key = SOL_UBC_KEY;
		}
#endif
		else
		{
			/*No key found */
			HxLOG_Info("DEV_SYS_ID & NO KEY\n");
			sstSOL_KEY.pucSOL_Key = NULL;
			CAS_IR_SOL_SetActivation(SOL_DISABLED);
			return HIR_NOT_OK;
		}
	}
	else if(stIRLoaderStatus.u16SysID == (HUINT16 )CUSTOMER_KEY_SYSTEM_ID ) /*Custom Key */
	{
#if defined(CONFIG_OP_TRUEVISION)
		if(stIRLoaderStatus.u16Variant == (IHL_U16)UBC_KEY_SYSTEM_ID)
		{
			/*use corresponding SOL_operator_key for Release*/
			HxLOG_Info("CUSTOM_SYS_ID & SOL_UBC_KEY\n");
			sstSOL_KEY.pucSOL_Key = SOL_UBC_KEY;
		}
		else
#endif
		{
			/* Not yet key registered */
			HxLOG_Info("CUSTOM_SYS_ID(sys id:%d) & NO KEY : %d\n",stIRLoaderStatus.u16SysID);
			sstSOL_KEY.pucSOL_Key = NULL;
			return HIR_NOT_OK;
		}
	}
	else
	{
		/*No key found */
		HxLOG_Error("NO key found report error.\n");

		sstSOL_KEY.pucSOL_Key = NULL;
		return HIR_NOT_OK;
	}
	return HIR_OK;
}

static HIR_ERR local_cas_ir_SOL_Validate(void)
{
	int i, j, k, res;
	HUINT8	DeliveryAndMuxTpBuffer[1024]={0,}; /* it's following general RSA rule */
	HUINT16	BufferLength=0 ;
	HUINT32 ucFound;

	HxLOG_Info("SOL_Validate\n");
	ENTER_SOL_INFO;

#if defined(SOL_DUMP_SI_DATA)
	HxLOG_Print("NIT's delivery information \n");
	for(i=0;i<SOL_DELIVERY_CNT_MAX;i++)
	{
		if(sol_delivery[i].ONID != SOL_INVALID_VAL)
		{
			HxLOG_Print("+sol_delivery: ONID=0x%X, TSID= 0x%X\n", sol_delivery[i].ONID, sol_delivery[i].TSID);
		}
	}

	HxLOG_Print("BAT's Transport information \n");
	for(i=0;i<SOL_ONID_CNT_MAX;i++)
	{
		if(sstMuxTPInfo.stSolSvc[i].usONID == SOL_INVALID_VAL)
		{
			break;
		}

		for(j=0; j< SOL_TSID_CNT_MAX; j++)
		{
			if(sstMuxTPInfo.stSolSvc[i].ausTSID[j] != SOL_INVALID_VAL)
			{
				HxLOG_Print("+sstMuxTPInfo: ONID=0x%X, TSID=0x%X\n", sstMuxTPInfo.stSolSvc[i].usONID, sstMuxTPInfo.stSolSvc[i].ausTSID[j]);
			}
		}
	}
#endif
	/*Make data for Verify signature */

	/*Step 1: find out delivery descriptors in described transport mux descriptors, and collect them into temporal buffer. */
	ucFound = FALSE;
	for(i=0;i<SOL_ONID_CNT_MAX; i++)
	{
		for(j=0; j< SOL_ONID_CNT_MAX * SOL_TSID_CNT_MAX; j++)
		{
			/* using the valid flag for reducing complexity */
			if(sol_delivery[j].ONID == SOL_INVALID_VAL )
			{
				break;
			}

			if(sstMuxTPInfo.stSolSvc[i].usONID == sol_delivery[j].ONID  )
			{
				/* It's changed of hand of being increased for reducing complexity.*/
				for(k=0; k< SOL_TSID_CNT_MAX; k++)
				{
					if(sstMuxTPInfo.stSolSvc[i].ausTSID[k] == SOL_INVALID_VAL)
					{
						break;
					}

					if(sstMuxTPInfo.stSolSvc[i].ausTSID[k] == sol_delivery[j].TSID)
					{
						HxSTD_memcpy(DeliveryAndMuxTpBuffer+BufferLength, sol_delivery[j].aucDeliveryDesc, SOL_DELIVERY_DESC_LEN);
						BufferLength += SOL_DELIVERY_DESC_LEN;
						ucFound = TRUE;
					}
				}
			}
		}
	}


	/*Step 2: Filled up with transport mux descriptor */
	HxSTD_memcpy( DeliveryAndMuxTpBuffer+ BufferLength, sstMuxTPInfo.pucMuxTpDesc, *(sstMuxTPInfo.pucMuxTpDesc+1)+2 );
	BufferLength += *(sstMuxTPInfo.pucMuxTpDesc+1)+2;

#if defined(SOL_DUMP_SI_DATA)
/*****************/
{
	int i;

	HxLOG_Print("\t------Signature--------\n\t");
	for(i=0;i<128;i++)
	{
		if(i!=0 && i%16 == 0)
			HxLOG_Print("\n\t");
		HxLOG_Print(" %02X", sstMuxTPInfo.aucMuxSign[i]);
	}
	HxLOG_Print("\n\t-----------------------\n");

	HxLOG_Print("\t------Verify Data(%d)--------\n\t", BufferLength);
	for(i=0;i<BufferLength;i++)
	{
		if(i!=0 && i%16 == 0)
			HxLOG_Print("\n\t");
		HxLOG_Print(" %02X", DeliveryAndMuxTpBuffer[i]);
	}
	HxLOG_Print("\n\t-----------------------\n");

}
/******************/
#endif

	/*Step 3: Validation process */
	res = local_cas_ir_Verify_Signature( sstMuxTPInfo.aucMuxSign, 128 , DeliveryAndMuxTpBuffer, BufferLength ) ;
	HxLOG_Print("SOL: Verification : %s \n", (res==0)? "OK":"FAIL" );


	/*Step 4: Register TP SVC and turn on. */
	if(res != ERR_OK)
	{
		if(sstSOL_KEY.pucSOL_Key == NULL)
		{
			CAS_IR_SOL_SetActivation(SOL_DISABLED);
		}
		else if(CAS_IR_SOL_IsActivated() == SOL_VERIFY_INIT)
		{
			CAS_IR_SOL_SetActivation(SOL_VERIFY_FAIL);
		}
		else if(CAS_IR_SOL_IsActivated() == SOL_VERIFY_SUCCESS) // 이전 상태가 success.
		{
			//restore mux tp info
			if(NULL != sstMuxTPInfo.pucMuxTpDesc)
				OxCAS_Free(sstMuxTPInfo.pucMuxTpDesc);

			HxSTD_memcpy(&sstMuxTPInfo, &sstVerifiedMuxTPInfo, sizeof(SOL_MUXTP_INFO));

			sstMuxTPInfo.pucMuxTpDesc = OxCAS_Malloc(*(sstVerifiedMuxTPInfo.pucMuxTpDesc+1)+2);
			if(NULL == sstMuxTPInfo.pucMuxTpDesc)
			{

				HxLOG_Error("SOL_validate: Mem alloc Error:(Len=%d)\n", *(sstMuxTPInfo.pucMuxTpDesc+1)+2);
				LEAVE_SOL_INFO;
				return HIR_NOT_OK;
			}
			HxSTD_memcpy(sstMuxTPInfo.pucMuxTpDesc, sstVerifiedMuxTPInfo.pucMuxTpDesc, *(sstVerifiedMuxTPInfo.pucMuxTpDesc+1)+2 );

		}
	}
	else
	{
		CAS_IR_SOL_SetActivation(SOL_VERIFY_SUCCESS);
		//update mux tp info

		if(NULL != sstVerifiedMuxTPInfo.pucMuxTpDesc)
			OxCAS_Free(sstVerifiedMuxTPInfo.pucMuxTpDesc);

		HxSTD_memcpy(&sstVerifiedMuxTPInfo, &sstMuxTPInfo, sizeof(SOL_MUXTP_INFO));

		sstVerifiedMuxTPInfo.pucMuxTpDesc = OxCAS_Malloc(*(sstMuxTPInfo.pucMuxTpDesc+1)+2);
		if(NULL == sstVerifiedMuxTPInfo.pucMuxTpDesc)
		{
			HxLOG_Error("SOL_validate: Mem alloc Error:(Len=%d)\n", *(sstMuxTPInfo.pucMuxTpDesc+1)+2);
			LEAVE_SOL_INFO;

			return HIR_NOT_OK;
		}
		HxSTD_memcpy(sstVerifiedMuxTPInfo.pucMuxTpDesc, sstMuxTPInfo.pucMuxTpDesc, *(sstMuxTPInfo.pucMuxTpDesc+1)+2 );

	}

	if(sstVerifiedMuxTPInfo.usDurationMin != 0xFFFF)
	{
		HxLOG_Print("sstVerifiedMuxTPInfo.usDurationMin = %d", sstVerifiedMuxTPInfo.usDurationMin);
		VK_TIMER_EventAfter( (sstVerifiedMuxTPInfo.usDurationMin)*(1000*60), local_cas_ir_SOL_TimerCallback, NULL, 0, &sstVerifiedMuxTPInfo.ulSolTimerId);
	}

	LEAVE_SOL_INFO;

	return HIR_OK;

}
/*****************************************************************************
  Function    : local_ir_sol_SendToAPP_UpdateNeeded
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
static HIR_ERR local_cas_ir_SOL_SendToAPP_UpdateNeeded(void)
{
	HMSG				msg;
	IR_SERVICE_INFO		*pstIrSvcInfo;

	HxLOG_Info("INT_SOL_SendToAPP_UpdateNeeded");

	pstIrSvcInfo = CAS_IR_DRV_GetLiveSvcInfo();

	if(pstIrSvcInfo != NULL)
	{
		VK_memset32(&msg, 0, sizeof(HMSG));

		msg.usMsgClass = IREVT_CLASS_SOL_STATUS_CHANGED;
		CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);
	}
	return ERR_OK;
}

/*****************************************************************************
  Function    : local_cas_ir_SOL_IsVerifiedTS
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
HUINT32 local_cas_ir_SOL_IsVerifiedTS(HUINT16 ONID, HUINT16 TSID)
{
	HUINT32 i, j;

	for(i=0; i< SOL_ONID_CNT_MAX ;i++)
	{
		/* Condition check, check a end of used item in registry ONID list for reducing calculation time */
		if(	sstMuxTPInfo.stSolSvc[i].usONID == SOL_INVALID_VAL )
		{
			continue;
		}

		/*Step 1, find a matched ONID array */
		if(	sstMuxTPInfo.stSolSvc[i].usONID == ONID)
		{
			for(j=0;j<SOL_TSID_CNT_MAX; j++)
			{
				/* Condition check, check a end of used item in registry TS list in ONID array, for reducing calculation time */
				if(sstMuxTPInfo.stSolSvc[i].ausTSID[j] == SOL_INVALID_VAL )
				{
					continue;
				}

				/*Step 2, find a matched TSID in found ONID array */
				if(sstMuxTPInfo.stSolSvc[i].ausTSID[j] == TSID)
				{
					HxLOG_Info(": the TP is registered(ONID=0x%X, TSID=0x%X\n", ONID, TSID);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

/*****************************************************************************
  Function    : INT_SOL_Verify_Signature
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
static HUINT32 local_cas_ir_Verify_Signature(unsigned char * pucSign, int nSign_len, unsigned char *pucMsg, int nMsg_len)
{
	PalCipher_BigNumber_e			bn_M;
	PalCipher_BigNumber_e			bn_S;
	PalCipher_BigNumber_e 			bn_n;
	PalCipher_BigNumber_e 			bn_e = {1, {0x03}};
	//PalCipher_BigNumber_e 			bn_e = {3, {0x01,0x00,0x01}};
	PalCipher_RsaPublicKey_e 	PublicKey;
	HERROR			err = ERR_FAIL;

	if(sstSOL_KEY.pucSOL_Key == NULL)
	{
		return err;
	}

	bn_n.len = SOL_KEY_SIZE;
	HxSTD_memcpy(bn_n.num, sstSOL_KEY.pucSOL_Key, SOL_KEY_SIZE);
//	bn_init(&bn_n, sstSOL_KEY.pucSOL_Key, SOL_KEY_SIZE);

	//public key
	PublicKey.n	= &bn_n;
	PublicKey.e	= &bn_e;

	//Message
	bn_M.len = nMsg_len;
	HxSTD_memcpy(bn_M.num, pucMsg, bn_M.len);

	//sign
	bn_S.len = nSign_len;
	HxSTD_memcpy(bn_S.num, pucSign, bn_S.len);


#if defined(SOL_DUMP_RSA_DATA)
	HxLOG_Info("------- RSA signature(len=%d)------\n", nSign_len);
	{
		HINT32	i;
		for(i=0;i < nSign_len; i++)
		{
			HxLOG_Info("%02X ", pucSign[i]);
			if(i%16== 15)
			{
				HxLOG_Info("\n");
			}
		}	/* End of decrypt Procedure */
		HxLOG_Info("\n");
	}
#endif

	err = PAL_CIPHER_VerifyRsaSsaPkcs1V15(&PublicKey, &bn_M,   &bn_S, eHashingAlg_MD5);

	HxLOG_Info("Verify signature: %d\n", err);

	return err;
}

/****************************************************************************
  Function    : local_SOL_Clear_State
  Parameters  :	void
  Output      :	void
  Return      : void
  Caller      : CAS_BaseTask
  Description :
****************************************************************************/
static void local_cas_ir_SOL_Clear_State(void)
{
	HUINT32 i, j;

	HxLOG_Info("SOL_Clear_State\n");

	ENTER_SOL_INFO;

	s_stateSOL = SOL_STATE_INITIALIZED;

	/*Clear descriptions for Mux-transport data */
	if(	sstMuxTPInfo.pucMuxTpDesc !=NULL)
	{
		OxCAS_Free(sstMuxTPInfo.pucMuxTpDesc);
		sstMuxTPInfo.pucMuxTpDesc = NULL;
	}
	sstMuxTPInfo.ucMuxTpDescVer= 0;
	sstMuxTPInfo.ucBehavior= 0;
	sstMuxTPInfo.usDurationMin= 0;

	/*Clear descriptions for Signature data */
	sstMuxTPInfo.ucMuxSignVer= 0;
	HxSTD_memset(sstMuxTPInfo.aucMuxSign, 0x00, SOL_SGN_SIZE );

	/*Clear SOL operation flags */
	CAS_IR_SOL_SetActivation(SOL_VERIFY_INIT);

	sstMuxTPInfo.ulBatCRC = 0;
	//sstMuxTPInfo.ulNitCRC = 0;
	HxSTD_memset(sstMuxTPInfo.ulNitCRC, 0x00, CAS_MAX_SECTION_NUM );

	/*Clear registerd TS services data base */
	for(i=0; i< SOL_ONID_CNT_MAX; i++)
	{
		if(sstMuxTPInfo.stSolSvc[i].usONID == SOL_INVALID_VAL)	/* Reducing complexity*/
		{
			break;
		}

		sstMuxTPInfo.stSolSvc[i].usONID = SOL_INVALID_VAL;
		for(j=0;j<SOL_TSID_CNT_MAX; j++)
		{
			if(sstMuxTPInfo.stSolSvc[i].ausTSID[j] == SOL_INVALID_VAL)	/* Reducing complexity*/
			{
				break;
			}
			sstMuxTPInfo.stSolSvc[i].ausTSID[j] = SOL_INVALID_VAL;
		}
	}

	/*Clear NIT's satelite delivery descriptors */
	for(i=0;i<SOL_DELIVERY_CNT_MAX; i++)
	{
		if(sol_delivery[i].ONID == SOL_INVALID_VAL)
		{
			break;
		}

		sol_delivery[i].ONID = SOL_INVALID_VAL;
		sol_delivery[i].TSID = SOL_INVALID_VAL;

		HxSTD_memset( sol_delivery[i].aucDeliveryDesc, 0x00, SOL_DELIVERY_DESC_LEN);

	}

	LEAVE_SOL_INFO;
}

static void local_cas_ir_SOL_TimerCallback(unsigned long TimerId,  void *pArg)
{
	sstVerifiedMuxTPInfo.ulSolTimerId = 0;
	CAS_IR_SOL_SetActivation(SOL_VERIFY_FAIL);
	local_cas_ir_SOL_SendToAPP_UpdateNeeded();

	return;
}

/***************************************************************************/
#define _______Global_Functions________
/***************************************************************************/
/****************************************************************************
  Function    : CAS_IR_SOL_initialize
  Parameters  :	void
  Output      :
  Return      : Result of initialization
  Caller      : Humax entry function
  Description :
****************************************************************************/
void CAS_IR_SOL_initialize(void)
{
	HINT32		nErr;
	HIR_ERR		res;

	HxLOG_Info("CAS_IR_SOL_initialize\n");

	/* Create semaphore for SOL data */
	nErr = VK_SEM_Create(&(sstMuxTPInfo.ulMuxdataSemID), "s_ulIrSolSemId", VK_SUSPENDTYPE_FIFO);
	if(nErr != VK_OK)
	{
		HxLOG_Error("Can't create ir_Sol sema. (%d)\n", nErr);
		return;
	}

	/*it's initializing variable memory pointer in SOL data*/
	sstMuxTPInfo.pucMuxTpDesc = NULL;

	HxSTD_memset(&sstVerifiedMuxTPInfo, 0, sizeof(SOL_MUXTP_INFO) );

	/*State and variables would be initialized */
	local_cas_ir_SOL_Clear_State();

	/* After initialize SoftCell it shall be to set SOL key */
	res = local_cas_ir_SOL_set_key();

	return;
}

/*****************************************************************************
  Function    : CAS_IR_SOL_Clear_NIT_Info
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
void CAS_IR_SOL_Clear_NIT_Info(void)
{
	HUINT32 i;

	HxLOG_Info("CAS_IR_SOL_Clear_NIT_Info");
	ENTER_SOL_INFO;

	s_stateSOL ^= SOL_STATE_NIT_DELIVERY_RECEVIED;

	/*Clear SOL operation flags */
	//mininam blocked - Once verified, use it until updated.
	//sstMuxTPInfo.bActivatedSOL = FALSE;

	/*Clear NIT's satelite delivery descriptors */
	for(i=0;i<SOL_DELIVERY_CNT_MAX; i++)
	{
		if(sol_delivery[i].ONID == SOL_INVALID_VAL)
		{
			break;
		}

		sol_delivery[i].ONID = SOL_INVALID_VAL;
		sol_delivery[i].TSID = SOL_INVALID_VAL;

		HxSTD_memset ( sol_delivery[i].aucDeliveryDesc, 0x00, SOL_DELIVERY_DESC_LEN);
	}

	LEAVE_SOL_INFO;
}

/*****************************************************************************
  Function    : CAS_IR_SOL_Clear_BAT_Info
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
void CAS_IR_SOL_Clear_BAT_Info(void)
{
	HUINT32 i, j;

	HxLOG_Info("CAS_IR_SOL_Clear_BAT_Info");

	ENTER_SOL_INFO;

	/*XOR operation*/
	s_stateSOL ^= SOL_STATE_BAT_MUXTP_SIGN_RECEVIED;

	/*Clear descriptions for Mux-transport data */
	if(	sstMuxTPInfo.pucMuxTpDesc !=NULL)
	{
		OxCAS_Free(sstMuxTPInfo.pucMuxTpDesc);
		sstMuxTPInfo.pucMuxTpDesc = NULL;
	}
	sstMuxTPInfo.ucMuxTpDescVer= 0;
	sstMuxTPInfo.ucBehavior= 0;
	sstMuxTPInfo.usDurationMin= 0;

	/*Clear descriptions for Signature data */
	sstMuxTPInfo.ucMuxSignVer= 0;
	HxSTD_memset(sstMuxTPInfo.aucMuxSign, 0x00, SOL_SGN_SIZE );

// TODO: how ?	sstMuxTPInfo.bOnceVerified = FALSE;

	/*Clear registerd TS services data base */
	for(i=0; i< SOL_ONID_CNT_MAX; i++)
	{
		sstMuxTPInfo.stSolSvc[i].usONID = SOL_INVALID_VAL;
		for(j=0;j<SOL_TSID_CNT_MAX; j++)
		{
			sstMuxTPInfo.stSolSvc[i].ausTSID[j] = SOL_INVALID_VAL;
		}
	}

	LEAVE_SOL_INFO;

}

HBOOL CAS_IR_SOL_Is_NIT_Changed(HUINT8 secNum, HUINT32 crc)
{

	if(sstMuxTPInfo.ulNitCRC[secNum] == crc)
		return FALSE;
	else
		return TRUE;
}

HUINT8 CAS_IR_SOL_Is_BAT_Changed(HUINT32 crc)
{
	if(sstMuxTPInfo.ulBatCRC == crc)
		return FALSE;
	else
		return TRUE;
}

SOL_VERIFY_STATE  CAS_IR_SOL_IsActivated(void)
{
	return sstMuxTPInfo.eActivatedSOL;
}

HUINT32 CAS_IR_SOL_SetActivation(SOL_VERIFY_STATE state)
{
	sstMuxTPInfo.eActivatedSOL = state;
	return ERR_OK;
}

/*****************************************************************************
  Function    : CAS_IR_SOL_Complete_NITInfo
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
void CAS_IR_SOL_Complete_NITInfo(HUINT8 ucChanged)
{
	HxLOG_Info("CAS_IR_SOL_Complete_NITInfo\n");

	//BAT 나 NIT 중에서 하나만 바뀐 경우에도 업데이트 되어야 한다.
	if(TRUE == ucChanged)
		s_IsNitChanged = TRUE;

	/* Upate SOL status, and check a condition to do verifying mux-descriptor */
	s_stateSOL |=SOL_STATE_NIT_DELIVERY_RECEVIED;

	if(TRUE == s_IsBatChanged || TRUE == s_IsNitChanged)
	{
		if(s_stateSOL == SOL_STATE_DO_START_VALIDATION)
		{
			s_IsBatChanged = s_IsNitChanged = FALSE;
			(void) local_cas_ir_SOL_Validate();
			(void) local_cas_ir_SOL_SendToAPP_UpdateNeeded();

		}
	}

}

/*****************************************************************************
  Function    : CAS_IR_SOL_Complete_BATInfo
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
void CAS_IR_SOL_Complete_BATInfo( HUINT8 ucChanged )
{
	HxLOG_Info("CAS_IR_SOL_Complete_BATInfo");

	//BAT 나 NIT 중에서 하나만 바뀐 경우에도 업데이트 되어야 한다.
	if(TRUE == ucChanged)
		s_IsBatChanged = TRUE;

	s_stateSOL |=SOL_STATE_BAT_MUXTP_SIGN_RECEVIED;

	if(TRUE == s_IsBatChanged || TRUE == s_IsNitChanged)
	{
		if(s_stateSOL == SOL_STATE_DO_START_VALIDATION)
		{
			s_IsBatChanged = s_IsNitChanged = FALSE;
			(void)local_cas_ir_SOL_Validate();
			(void)local_cas_ir_SOL_SendToAPP_UpdateNeeded();
		}
	}

	/* this is state of waiting for other condition */
}

void CAS_IR_SOL_Complete_BATInfoFail(void   )
{
	HxLOG_Info("CAS_IR_SOL_Complete_BATInfoFail");

	CAS_IR_SOL_SetActivation(SOL_VERIFY_FAIL);

	(void)local_cas_ir_SOL_SendToAPP_UpdateNeeded();

	/* this is state of waiting for other condition */
}

/*****************************************************************************
  Function    : CAS_IR_SOL_update_SC_Status
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
HUINT32 CAS_IR_SOL_update_SC_Status( SOL_SC_UPDATE_TYPE eUpdateType, HUINT8 bValid )
{
	HxLOG_Info("CAS_IR_SOL_update_SC_Status");

	ENTER_SOL_INFO;

	switch(eUpdateType)
	{
	case SOL_SC_UPDATE_IRCARD_STATUS:
		sstMuxTPInfo.bSCValidIR = bValid;
		break;

	case SOL_SC_UPDATE_PRODUCT_STATUS:
		sstMuxTPInfo.bSCValidProduct = bValid;
		HxLOG_Info("CAS_IR_SOL_update_SC_Status : bSCVValidProduct=%d \n", bValid);
		break;
	}

	LEAVE_SOL_INFO;

	local_cas_ir_SOL_SendToAPP_UpdateNeeded();

	return ERR_OK;
}


/*****************************************************************************
  Function    : CAS_IR_SOL_NIT_UpdateCRC
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
HUINT32 CAS_IR_SOL_NIT_UpdateCRC(HUINT8 secNum, HUINT32 crc)
{
	sstMuxTPInfo.ulNitCRC[secNum] = crc;
	return ERR_OK;
}

/*****************************************************************************
  Function    : CAS_IR_SOL_BAT_UpdateCRC
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
HUINT32 CAS_IR_SOL_BAT_UpdateCRC(HUINT32 crc)
{
	sstMuxTPInfo.ulBatCRC = crc;
	return ERR_OK;
}

HUINT32 CAS_IR_SOL_update_DeliveryDesc( HUINT16 ONID, HUINT16 TSID, HUINT8* pDescriptor)
{
	int i;

	HxLOG_Info("CAS_IR_SOL_update_DeliveryDesc");

	ENTER_SOL_INFO;

	HxLOG_Info("+CAS_IR_SOL_update_DeliveryDesc: ONID(0x%04X),TSID(0x%04X)\n", ONID, TSID);

	for(i=0; i< SOL_DELIVERY_CNT_MAX ; i++)
	{
		if(sol_delivery[i].ONID == SOL_INVALID_VAL )
		{
			break;
		}
	}

	if(i == SOL_DELIVERY_CNT_MAX)
	{
		LEAVE_SOL_INFO;
		HxLOG_Info("CAS_IR_SOL_update_DeliveryDesc: repository overflow!\n");
		return ERR_FAIL;
	}

	sol_delivery[i].ONID = ONID;
	sol_delivery[i].TSID = TSID;
	HxSTD_memcpy(sol_delivery[i].aucDeliveryDesc, pDescriptor, SOL_DELIVERY_DESC_LEN );

	LEAVE_SOL_INFO;

	HxLOG_Print("CAS_IR_SOL_update_DeliveryDesc\n" );
	HxLOG_Print(":sol_delivery.ONID = 0x%04X\n", sol_delivery[i].ONID);
	HxLOG_Print(":sol_delivery.TSID = 0x%04X\n", sol_delivery[i].TSID);

	return ERR_OK;
}


/*****************************************************************************
  Function	  : CAS_IR_SOL_update_MuxTPDesc
  Parameters  :
  Output	  :
  Return	  :
  Description :
*****************************************************************************/
HUINT32 CAS_IR_SOL_update_MuxTPDesc( HUINT8* pDesc)
{
	HUINT8 *p = pDesc;

	HUINT8	len1, len2, i,j, ucLenghOfDesc;
	HUINT8	ver;

	HxLOG_Info("CAS_IR_SOL_update_MuxTPDesc");
	ENTER_SOL_INFO;


	/*Parsing a Mux transport Descriptor */
	/*descriptor's content length + Tag & Length size */

	ucLenghOfDesc = *(pDesc+1) + 2;

	p++;
	len1 = *p++;

	ver = *p++;

	sstMuxTPInfo.ucMuxTpDescVer = ver;
	sstMuxTPInfo.ucBehavior = *p++;
	sstMuxTPInfo.usDurationMin = (HUINT16) get16bit(p); p+=2;

	HxLOG_Print("CAS_IR_SOL_update_MuxTPDesc\n: Len(%d)\n", len1);
	HxLOG_Print(":sstMuxTPInfo.ucVersion= %d\n", sstMuxTPInfo.ucMuxTpDescVer);
	HxLOG_Print(":sstMuxTPInfo.ucBehavior=%d\n", sstMuxTPInfo.ucBehavior);
	HxLOG_Print(":sstMuxTPInfo.usDurationMin= 0x%X \n", sstMuxTPInfo.usDurationMin);

	len1 -= 4; 	i=0;

	while(len1>0)
	{
		if(i==SOL_ONID_CNT_MAX)
		{
			HxLOG_Error("CAS_IR_SOL_update_MuxTPDesc: ONIDs overflow!\n");
			break;
		}
		sstMuxTPInfo.stSolSvc[i].usONID = (HUINT16 )get16bit(p); p+=2;
		HxLOG_Info(": sstMuxTPInfo.stSolSvc[%d].usONID = 0x%X\n",i, sstMuxTPInfo.stSolSvc[i].usONID);

		len2 = *p;	p++; j=0;
		len1-= (len2 +3);
		while(len2>0)
		{
			if(i==SOL_TSID_CNT_MAX)
			{
				HxLOG_Error("CAS_IR_SOL_update_MuxTPDesc: TSIDs overflow!\n");
				break;
			}
			sstMuxTPInfo.stSolSvc[i].ausTSID[j] = (HUINT16) get16bit(p); p+=2; len2-=2;
			HxLOG_Info(": sstMuxTPInfo.stSolSvc[%d].ausTSID[%d]= 0x%X\n",i,j, sstMuxTPInfo.stSolSvc[i].ausTSID[j]);
			j++;
		}
		i++;
 	}

	sstMuxTPInfo.pucMuxTpDesc = OxCAS_Malloc(ucLenghOfDesc);
	if(	sstMuxTPInfo.pucMuxTpDesc ==NULL)
	{
		LEAVE_SOL_INFO;
		HxLOG_Error("CAS_IR_SOL_update_MuxTPDesc: Mem alloc Error:(Len=%d)\n", ucLenghOfDesc);
		return ERR_FAIL;
	}

	HxSTD_memcpy( sstMuxTPInfo.pucMuxTpDesc, pDesc , ucLenghOfDesc);

	LEAVE_SOL_INFO;

	return ERR_OK;
}

/*****************************************************************************
  Function    : CAS_IR_SOL_update_SignatureDesc
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
HUINT32 CAS_IR_SOL_update_SignatureDesc( HUINT8* pDescriptor)
{
	HUINT8 *p = pDescriptor;
	HUINT8	ver;

	HxLOG_Info("SOL_update_SignatureDesc");
	ENTER_SOL_INFO;

	/*Parsing a Mux Signature Descriptor */

	/*Jump to descriptor's content */
	p+=2;
	ver = *p++;

	sstMuxTPInfo.ucMuxSignVer = ver;
	HxLOG_Info("sstMuxTPInfo.ucMuxSignVer = 0x%X\n", sstMuxTPInfo.ucMuxSignVer);

	HxSTD_memcpy(sstMuxTPInfo.aucMuxSign , p,  SOL_SGN_SIZE);

	LEAVE_SOL_INFO;

	return ERR_OK;
}

/*****************************************************************************
  Function    : CAS_IR_SOL_UpdatePMT
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
void CAS_IR_SOL_UpdatePMT(void)
{
	//When PMT changed, check SOL block status again.
	//Because service might be changed between scrambled and clear.

	local_cas_ir_SOL_SendToAPP_UpdateNeeded();
}

/*****************************************************************************
  Function	  : CAS_IR_SOL_is_service_blocked
  Parameters  : original network id, transport stream id
  Output	  : TRUE: input tp is blocked state,  FALSE: non-blocked
  Return	  : Application layer: IRMAIN_STATE
  Description : check the input TS registerd sol list, or not
*****************************************************************************/
SOL_SERVICE_STATE CAS_IR_SOL_is_service_blocked(Handle_t hAction, HUINT16 ONID, HUINT16 TSID)
{
	IR_DRV_SC_INFO		stIrSCInfo;
	eIrDrvCaType		eCasType;
	SOL_VERIFY_STATE	eSolVerifyState;

	HxLOG_Info("CAS_IR_SOL_is_service_blocked\n");

	ENTER_SOL_INFO;

#if 0 // Not yet defined policy.
	if( sstMuxTPInfo.bOnceVerified != TRUE )
	{
		LEAVE_SOL_INFO;
		HxLOG_Debug("NOT Verified yet\n");
		return ERR_FAIL;
	}
#endif

	/* Step 1. if not yet verified signature or failed state of verifying signature */
	eSolVerifyState = CAS_IR_SOL_IsActivated();

	if(eSolVerifyState == SOL_DISABLED)
	{
		HxLOG_Info("\t[SOL_DISABLED]\n");
		goto SOL_TP_OK;
	}
	else if(eSolVerifyState == SOL_VERIFY_INIT)
	{
		HxLOG_Info("\t[SOL_VERIFY_INIT]\n");
		HxLOG_Info("NOT activated operation yet.\n");
		goto SOL_TP_UNDEFIEND;
	}
	else if(eSolVerifyState == SOL_VERIFY_FAIL)
	{
		HxLOG_Info("\t[SOL_VERIFY_FAIL]\n");
		goto SOL_TP_FAILED;
	}


	if(sstMuxTPInfo.ucBehavior == SOL_BEHAVIOUR_DISABLE)
	{
		HxLOG_Info("\t[SOL_BEHAVIOUR_DISABLE] -- sstMuxTPInfo.ucBehavior\n");
		goto SOL_TP_OK;
	}


	/*Step 2. Verified TS : it's top priority of criteria */
	if(local_cas_ir_SOL_IsVerifiedTS(ONID, TSID) == TRUE)
	{
		HxLOG_Info("\t[SOL Verified TS] -- INT_SOL_IsVerifiedTS(0x%X,0x%X)\n",ONID, TSID);
		goto SOL_TP_OK;
	}

	HxLOG_Info("Now, current TP was NOT found in registry list\n");

	/* Step 3. Check behaviour*/

	if(sstMuxTPInfo.ucBehavior == SOL_BEHAVIOUR_DEFAULT)
	{
		/*Nothing to do here in default case, Already it's fail state */
		HxLOG_Info("\t[SOL_BEHAVIOUR_DISABLE] -- sstMuxTPInfo.ucBehavior\n");
		goto SOL_TP_FAILED;
	}

	/*Check fta svc, or not */
	CAS_IR_DRV_GetSvcCasKind(hAction,&eCasType);
	if( !(eIrDrvCaType_Fta == eCasType || eIrDrvCaType_None == eCasType))
	{
		HxLOG_Info(":SRAMBLED SVC (eCasType:0x%X)\n",eCasType);
		goto SOL_TP_FAILED;
	}
	else  /*if CAI_CAS_KIND_NONE != eCasType */
	{

		HxLOG_Info(":FTA SVC\n");
		/*In case, FTA*/
		switch(sstMuxTPInfo.ucBehavior)
		{
			case SOL_BEHAVIOUR_ALLOW_FTA:
				HxLOG_Info(":SOL_BEHAVIOUR_ALLOW_FTA\n");
				goto SOL_TP_OK;
				break;

			case SOL_BEHAVIOUR_ALLOW_FTA_WHEN_SC_INSERTED:
				HxLOG_Info(":SOL_BEHAVIOUR_ALLOW_FTA_WHEN_SC_INSERTED\n");
				/*because of Non- being to supportt Softcell3 APi to get a CA system ID in smartcard, then 내맘대로 가정하고 수정함. ㅋㅋ */
				( void )XSVC_CAS_IR_DRV_GetScInfo( 0 /*as ive no idea */, &stIrSCInfo);
				if (stIrSCInfo.stIrScApiInfo.ucValid != TRUE ) /* FALSE: invalid, TRUE: valid */
				{
					HxLOG_Info(":SC INVALID SC\n");
					goto SOL_TP_FAILED;
				}
				HxLOG_Info(":SC VALID\t");
				goto SOL_TP_OK;
				break;

			case SOL_BEHAVIOUR_ALLOW_FTA_WHEN_SC_INSERTED_WITH_ONE_PRODUCT_SC:
				HxLOG_Info(":SOL_BEHAVIOUR_ALLOW_FTA_WHEN_SC_INSERTED_WITH_ONE_PRODUCT_SC\n");
				/* extApp와 IR_DRV layer가 싱크가 안맞으므로 IR_SVC 관련하여 IR_DRV_API를 사용할수 없다. CAI 단의 API를 이용해서 Sync를 맞춘다. */
				/*because of Non- being to supportt Softcell3 APi to get a CA system ID in smartcard, then 내맘대로 가정하고 수정함. ㅋㅋ */

				( void )XSVC_CAS_IR_DRV_GetScInfo( 0 /*as ive no idea */, &stIrSCInfo);
				if (stIrSCInfo.stIrScApiInfo.ucValid != TRUE ) /* FALSE: invalid, TRUE: valid */
				{
					HxLOG_Info(":SC INVALID SC\n");
					goto SOL_TP_FAILED;
				}
				HxLOG_Info(":SC VALID\t");

				if(sstMuxTPInfo.bSCValidProduct != TRUE)
				{
					HxLOG_Info(":SC INVALID PRODUCT\n");
					goto SOL_TP_FAILED;
				}
				HxLOG_Info(":SC PRODUCT\n");

				goto SOL_TP_OK;
				break;

			default:
				break;
		} //	switch(sstMuxTPInfo.ucBehavior)
	}

SOL_TP_UNDEFIEND:
	LEAVE_SOL_INFO;
	return SOL_UNKNOWN_SERVICE;

SOL_TP_FAILED:
	LEAVE_SOL_INFO;
	return SOL_BLOCKED_SERVICE; /* Blocked */

SOL_TP_OK:
	LEAVE_SOL_INFO;
	return SOL_VISIBLE_SERVICE;	/* Not Blocked */

}


#endif /* End of CONFIG_MW_CAS_IRDETO_SOL  */
