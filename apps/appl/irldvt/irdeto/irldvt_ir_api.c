

#include <irldvt_ir_api.h>

#include <tir_nvr.h>

#include <tir_sup.h>
#include <sysdefs.h>
#include <flash.h>

#include <tir_platform.h>
#include <tir_nvr.h>

#include <_hlib_log.h>

#define _____CA_NVRAM_DEFINE_______________________________________________________________


/* CAUTIONS: DO NOT CORRUPT IRDETO SYSTEM PARTITION, it's related with CA SYSTEM & LOADER */
//#define IR_NVRAM_PARTITION_IRDETO_OFFSET			0x0E00
#define IR_NVRAM_PARTITION_IRDETO_SIZE				0x0200
//#define IR_NVRAM_PARTITION_LOADER_OFFSET			(IR_NVRAM_PARTITION_IRDETO_OFFSET+IR_NVRAM_PARTITION_IRDETO_SIZE)//0x1000
#define IR_NVRAM_PARTITION_LOADER_SIZE				0x0200
//#define IR_NVRAM_PARTITION_CA_OFFSET				(IR_NVRAM_PARTITION_LOADER_OFFSET+IR_NVRAM_PARTITION_LOADER_SIZE)//0x1200
#define IR_NVRAM_PARTITION_CA_SIZE					0x0200
//#define IR_NVRAM_PARTITION_IRDETO_BACKUP_OFFSET		(IR_NVRAM_PARTITION_CA_OFFSET+IR_NVRAM_PARTITION_CA_SIZE)//0x1400
#define IR_NVRAM_PARTITION_IRDETO_BACKUP_SIZE		0x0200


static IHL_U8 au8IrdetoScram[IR_PARTITION_IRDETO_SIZE];
static IHL_U8 au8IrdetoClear[IR_PARTITION_IRDETO_SIZE];
//static Boot_Block_Control_Block s_stBBCB;

static IHL_U8 au8MagicNumber[MAGIC_SIZE] =
{
   0x5A,
   0x69,
   'r',
   's'
};


//added by hmjoo

static HUINT8		s_aucCPCB[IR_CPCB_SIZE];
static IHL_STATUS s_stIrLoaderStatus;

HUINT16	IRLDVT_DRV_Get16Bits(HUINT8 *base)
{
	printf("\n[%s],[%d]++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
	return (base[0] << 8) | (base[1]);
}
HUINT32	IRLDVT_DRV_Get32Bits(HUINT8 *base)
{
	return (base[0] << 24) | (base[1] << 16) | (base[2] << 8) | (base[3]);
}



HINT32 irldvt_Read_BBCB(Boot_Block_Control_Block *pBBCB)
{
	HUINT32	result = ERR_OK;
	HUINT8 aucBBCB[IR_BBCB_SIZE];
	printf("\n[%s],[%d] irldvt_Read_BBCB IR_BBCB_SIZE = %d ++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__, IR_BBCB_SIZE);
	HxLOG_Print("%s\n", __FUNCTION__ );

#if defined(IRPLAT_TEST_INCOMPLETED_LOADER)
	return result ;
#endif

	//result = svc_cas_DevSysGetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_BBCB, 0, aucBBCB, IR_BBCB_SIZE);
	result = irldvt_svc_cas_DevSysGetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_BBCB, 0, aucBBCB, IR_BBCB_SIZE);
	if (ERR_OK != result)
	{
		printf("\n[%s],[%d] in irldvt_svc_cas_DevSysGetField occured ERROR++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
		HxLOG_Error("svc_cas_DevSysGetField %08X", result);
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	/* When you tried to get real value from nvram, you must not use a structure copy method due to fragmentation, jichoi. */
	pBBCB->usCRC16 			= IRLDVT_DRV_Get16Bits(aucBBCB + BBCB_CRC16_OFFSET);
	pBBCB->ucManufacturerCode	= aucBBCB[BBCB_MANCODE_OFFSET] ;
	pBBCB->ucHardwareCode		= aucBBCB[BBCB_HWCODE_OFFSET] ;
	pBBCB->ucLoaderMajor		= aucBBCB[BBCB_LOADERMAJOR_OFFSET] ;
	pBBCB->ucLoaderMinor		= aucBBCB[BBCB_LOADERMINOR_OFFSET] ;
	pBBCB->ucLoaderType		= aucBBCB[BBCB_LOADERTYPE_OFFSET] ;
	pBBCB->ulSerialNumber		= IRLDVT_DRV_Get32Bits(aucBBCB + BBCB_SERIALNUM_OFFSET);
	pBBCB->ulRandomNumber 	= IRLDVT_DRV_Get32Bits(aucBBCB + BBCB_RANDOMNUM_OFFSET);

	printf("\n\n___________________BBCB__DATA_______________________\n");
	printf("usCRC16 = 0x%x\n", pBBCB->usCRC16);
	printf("ucManufacturerCode = 0x%x\n", pBBCB->ucManufacturerCode);
	printf("ucHardwareCode = 0x%x\n", pBBCB->ucHardwareCode);
	printf("ucLoaderMajor = 0x%x\n", pBBCB->ucLoaderMajor);
	printf("ucLoaderMinor = 0x%x\n", pBBCB->ucLoaderMinor);
	printf("ucLoaderType = 0x%x\n", pBBCB->ucLoaderType);
	printf("ulSerialNumber = 0x%x\n", (HUINT32)pBBCB->ulSerialNumber);
	printf("ulRandomNumber = 0x%x\n", (HUINT32)pBBCB->ulRandomNumber);
	printf("\n_____________________________________________________\n");

	return ERR_OK;

}
HINT32 irldvt_Read_CPCB(HUINT8 * pCPCB)
{
	HUINT32	result = ERR_OK;
	HxLOG_Info("\n");

//#if defined(IRPLAT_TEST_INCOMPLETED_LOADER)
//	return result;
//#endif

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	result = irldvt_svc_cas_DevSysGetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED, 0, pCPCB, IR_CPCB_SIZE);
#else
	result = irldvt_svc_cas_DevSysGetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_CPCB, 0, pCPCB, IR_CPCB_SIZE);
#endif
	if (ERR_OK != result)
	{
		printf(" svc_cas_DevSysGetField %08X", result);
		printf("\n\n");

		return ERR_FAIL;
	}

	return ERR_OK;
}

HINT32 irldvt_ReadLoaderStatus(IHL_STATUS * pLoaderStatus )
{
	IHL_S16 nErrCode;

	HxLOG_Info("\n");


	memset(pLoaderStatus, 0, sizeof(IHL_STATUS));

//#if defined(IRPLAT_TEST_INCOMPLETED_LOADER)
#if 0
	pLoaderStatus->u16SysID = 0xFFFF;
	pLoaderStatus->s16SysIDValid = 0;
	pLoaderStatus->u16KeyVer = 0;
	pLoaderStatus->s16KeyVerValid = 0;
	pLoaderStatus->u16SigVer = 0;
	pLoaderStatus->s16SigVerValid = 0;
	pLoaderStatus->s16VariantValid = 0;
	pLoaderStatus->u8BlockFailMask = 0;
#else
	nErrCode = IRLDVT_I_HLSUP_GetStatusInfo(pLoaderStatus);
	if( nErrCode )
	{
		HxLOG_Error("[IRLDVT_I_HLSUP_GetStatusInfo ERROR : %d]\n", nErrCode);
		return ERR_FAIL;
	}
#endif

#if defined(IRPLAT_TEST_FORCE_HUMAX_VARIANT)
		/* it 	must be humax manufacturer ID for descrambing Humax pisys stream, jichoi*/
		pLoaderStatus->u16Variant = 0xF;
#endif

	printf("[Irdeto Status Information]\n");
	printf("usSysID = %d(0x%x)\n", 		pLoaderStatus->u16SysID, pLoaderStatus->u16SysID);
	printf("ssSysIDValid = %d(0x%x)\n", pLoaderStatus->s16SysIDValid, pLoaderStatus->s16SysIDValid);
	printf("usKeyVer = %d(0x%x)\n", 	pLoaderStatus->u16KeyVer, pLoaderStatus->u16KeyVer);
	printf("ssKeyVerValid = %d(0x%x)\n", pLoaderStatus->s16KeyVerValid, pLoaderStatus->s16KeyVerValid);
	printf("usSigVer = %d(0x%x)\n", 	pLoaderStatus->u16SigVer, pLoaderStatus->u16SigVer);
	printf("ssSigVerValid = %d(0x%x)\n", pLoaderStatus->s16SigVerValid, pLoaderStatus->s16SigVerValid);
	printf("usVariant = %d(0x%x)\n", 	pLoaderStatus->u16Variant, pLoaderStatus->u16Variant);
	printf("ssVariantValid = %d(0x%x)\n", pLoaderStatus->s16VariantValid, pLoaderStatus->s16VariantValid);

	return ERR_OK;
}



HINT32 irldvt_cas_IrLoaderInit(void)
{
	HINT32	result = ERR_OK;

	HxLOG_Info("\n");

	if(  ERR_OK != irldvt_Read_BBCB(&s_stBBCB) )
	{
		printf("in local_cas_ir_Read_BBCB !\n");
		result |= ERR_FAIL;
	}
	if(  ERR_OK != irldvt_Read_CPCB(s_aucCPCB) )
	{
		printf("in local_cas_ir_Read_CPCB !\n");
		result |= ERR_FAIL;
	}
	if(  ERR_OK != irldvt_ReadLoaderStatus(&s_stIrLoaderStatus) )
	{
		printf("in local_cas_ir_ReadLoaderStatus !\n");
		result |= ERR_FAIL;
	}

	return result;
}

HIR_ERR IRLDVT_IR_NVR_Partition_Read( IRNVR_PART_Type ePartType, HUINT32 ulPartOffset,HUINT8 *pucData, HUINT32 ulSize )
{
	irldvt_DevFlashField_e	eType;
	HUINT32				ulMaxOffset;

	printf("\n[%s],[%d]++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
	HxLOG_Print("Offset[%d], Size[%d]\n", ulPartOffset, ulSize);
	if(pucData ==NULL)
	{
		HxLOG_Info("null pointer\n");
		return HIR_NOT_OK;
	}

	switch(ePartType)
	{
		case IRNVR_PART_IRDETO :
			printf("\n[%s],[%d] ePartType = IRNVR_PART_IRDETO++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
			eType = IRLDVT_DEV_FLASH_FIELD_IRDETO_IR_PARTITION;
			ulMaxOffset = IR_PARTITION_IRDETO_SIZE;
			break;

		case IRNVR_PART_CA :
			printf("\n[%s],[%d] ePartType = IRNVR_PART_CA++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
			eType = IRLDVT_DEV_FLASH_FIELD_IRDETO_CA_PARTITION;
			ulMaxOffset = IR_PARTITION_CA_SIZE;
			break;
		case IRNVR_PART_LOADER :
			printf("\n[%s],[%d] ePartType = IRNVR_PART_LOADER++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
			eType = IRLDVT_DEV_FLASH_FIELD_IRDETO_LOADER_PARTITION;
			ulMaxOffset = IR_PARTITION_LOADER_SIZE;
			break;

		case IRNVR_WHOLE_AREA:
			printf("\n[%s],[%d] ePartType = IRNVR_WHOLE_AREA++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
		default:
			HxLOG_Info("Invalid partition(%d)\n", ePartType);
			return HIR_NOT_OK;
	}

	if((ulPartOffset + ulSize)> ulMaxOffset)
	{
		HxLOG_Info("Invalid offset(0x%X)\n", ulPartOffset);
		return HIR_NOT_OK;
	}

	if(ERR_OK != irldvt_svc_cas_DevSysGetField(eType, ulPartOffset, pucData, ulSize) )
	{
		HxLOG_Info("svc_cas_DevSysGetField error\n ");
		return HIR_NOT_OK;
	}

	return HIR_OK;
}


static IHL_S16 IRLDVT_M_HLSUP_ReadNVRAM (IHL_PU8 pu8Buffer, IHL_U16 u16Offset,
                           IHL_U16 u16Length)
{
	printf("\n[%s],[%d]++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
	IHL_S16  ret;
	ret =  (IHL_S16)IRLDVT_IR_NVR_Partition_Read(IRNVR_PART_IRDETO, u16Offset, pu8Buffer, u16Length );

	return ret;
}


static IHL_U16 IRLDVT_I_HLSUP_GetWord (IHL_U16 u16Index)
{
   HxLOG_Info("u16Index=0x%X\n", u16Index);
   if (u16Index < IR_NVRAM_PARTITION_IRDETO_SIZE - 1)
   {
      return ((au8IrdetoClear[u16Index] << 8) + au8IrdetoClear[u16Index + 1]);
   }
   else
   {
      return (0);
   }
}

HIR_ERR irldvt_IrLoaderGetSerialNum(HUINT32 *pSerNum)
{
	printf("\n[%s],[%d]++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
	*pSerNum = s_stBBCB.ulSerialNumber;
	HxLOG_Print(" Serial Num : %d\n", *pSerNum);
	printf(" Serial Num : %d\n", *pSerNum);
	return HIR_OK;
}

HIR_ERR irldvt_IrLoaderGetRandomNum(HUINT32 *pRandomNum)
{
	*pRandomNum = s_stBBCB.ulRandomNumber;
	return HIR_OK;
}

static IHL_S16 IRLDVT_M_HLSUP_GetBBInfo (IHL_PU8 pu8SerNum, IHL_PU8 pu8RandNum)
{
	HUINT32 ulSerRanNum;

	HxLOG_Info("\n");
	printf("\n[%s],[%d]++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);

	irldvt_IrLoaderGetSerialNum(&ulSerRanNum);
	memcpy(pu8SerNum, (unsigned char *)&ulSerRanNum, SERIAL_NUMBER_SIZE);

	irldvt_IrLoaderGetRandomNum(&ulSerRanNum);
	memcpy(pu8RandNum, (unsigned char *)&ulSerRanNum, RANDOM_NUMBER_LEN);

	return 0;
}

void ILDVTI_HLSUP_VariBlockCipher (IHL_U8 pau8OutBlock [],
                              const IHL_U8 pau8InBlock [],
                              const IHL_U16 u16BlockSize,
                              const IHL_U8 pau8Key [],
                              const IHL_U8 u8KeySize)
{

   static const IHL_U8 au8Sbox [256] =
   {
      0x10, 0xDA, 0xA9, 0xA8, 0xC6, 0x6A, 0x7A, 0x4D,
      0x52, 0xF2, 0x02, 0xB2, 0xAA, 0x55, 0xB7, 0x45,
      0xA6, 0x9C, 0xFE, 0x90, 0xE8, 0xF1, 0x9E, 0x6F,
      0x98, 0x78, 0x7B, 0x88, 0xFC, 0xA3, 0x58, 0x9A,
      0x1A, 0xB5, 0xED, 0x97, 0x9F, 0x3E, 0xB4, 0x95,
      0x5E, 0xE0, 0x0C, 0x15, 0xBB, 0x1E, 0xD4, 0x13,
      0x17, 0xA4, 0x0A, 0x23, 0xA0, 0x28, 0x38, 0xA5,
      0x79, 0x65, 0xFD, 0xE3, 0x11, 0xEE, 0x9B, 0x87,
      0x18, 0x44, 0x60, 0x34, 0xD3, 0xC2, 0x92, 0xBD,
      0xE6, 0x83, 0xF6, 0x53, 0x7D, 0x99, 0x66, 0x6C,
      0x5C, 0x89, 0x74, 0x6E, 0x40, 0x68, 0xBC, 0xAB,
      0x57, 0x2D, 0xE2, 0x09, 0x85, 0xD8, 0xAC, 0xBA,
      0x77, 0xD9, 0xF7, 0xB8, 0x71, 0x47, 0xC7, 0x2B,
      0xE5, 0x31, 0x82, 0xCC, 0x64, 0x9D, 0x76, 0x8E,
      0x8A, 0xDC, 0x16, 0x7C, 0xFB, 0x33, 0x54, 0xA7,
      0xBF, 0x37, 0x63, 0x0B, 0x35, 0x6D, 0x5B, 0x32,
      0xCF, 0x51, 0xEF, 0xD5, 0x0F, 0x46, 0xB1, 0xC9,
      0x43, 0x00, 0x1C, 0x7E, 0x70, 0x8F, 0x29, 0x3F,
      0x0E, 0x4A, 0x5F, 0x41, 0xAF, 0xF8, 0xCB, 0xCD,
      0xE9, 0x75, 0x1B, 0xDD, 0x8B, 0xE7, 0x27, 0x59,
      0x73, 0x0D, 0xB9, 0x8C, 0x62, 0x20, 0xD6, 0xAD,
      0x7F, 0xA2, 0x1F, 0x49, 0x93, 0xCA, 0x2E, 0x07,
      0xDE, 0xDB, 0x94, 0x69, 0xEA, 0x14, 0x3C, 0x4F,
      0xD0, 0xC1, 0xEC, 0x8D, 0x39, 0xB6, 0xEB, 0xE4,
      0xBE, 0x30, 0xFF, 0x21, 0x42, 0x3A, 0xE1, 0x25,
      0x06, 0xF4, 0x03, 0xC4, 0x3D, 0x6B, 0xDF, 0x1D,
      0x2C, 0x81, 0x2F, 0x4B, 0xD2, 0x84, 0x4C, 0x56,
      0x86, 0x22, 0x01, 0xF3, 0x72, 0x24, 0x08, 0x05,
      0x67, 0x12, 0xFA, 0xD1, 0x80, 0xB3, 0x04, 0xC3,
      0xF5, 0x5A, 0x26, 0xC8, 0x36, 0xF0, 0x19, 0x48,
      0x4E, 0xC5, 0x91, 0x5D, 0x2A, 0xAE, 0x96, 0x50,
      0xF9, 0xB0, 0xA1, 0xD7, 0x61, 0xC0, 0x3B, 0xCE,
   };

   IHL_U16 u16Step;
   IHL_U8 u8Tmp;
   IHL_U16 u16I, u16J, u16T0, u16T1, u16T2;

   HxLOG_Info("\n");

   /* reverse and copy data into the destination, which may be the source */
   for (u16I=(u16BlockSize+1)/2, u16J=u16BlockSize-u16I-1; u16I < u16BlockSize;
         u16I++, u16J--)
   {
      u8Tmp = pau8InBlock[u16I];
      pau8OutBlock[u16I] = pau8InBlock[u16J];
      pau8OutBlock[u16J] = u8Tmp;
   }

   /* perform the ciphering loop */
   u16I = 0;
   u16J = 0;
   u16T0 = u16BlockSize - 1;
   u16T1 = 1;
   u16T2 = (u16BlockSize+1)/2;
   for (u16Step = u16BlockSize * u8KeySize; u16Step != 0; u16Step--)
   {
      u8Tmp = au8Sbox[pau8OutBlock[u16I] ^ pau8Key[u16J]];
      pau8OutBlock[u16T0] ^= u8Tmp;
      pau8OutBlock[u16T1] ^= u8Tmp;
      pau8OutBlock[u16T2] ^= u8Tmp;
      u16T0 = u16I;
      u16I = u16T1;
      u16T1 += 1;
      if (u16T1 == u16BlockSize)
      {
         u16T1 = 0;
      }
      u16T2 += 1;
      if (u16T2 == u16BlockSize)
      {
         u16T2 = 0;
      }
      u16J += 1;
      if (u16J == u8KeySize)
      {
         u16J = 0;
      }
   }

   /* erase internal data for cleanliness */
   u8Tmp = 0x00;
} /* ILDVTI_HLSUP_VariBlockCipher */

static IHL_S8 IRLDVTI_HLSUP_MagicNumbersOK (IHL_PU8 pu8Numbers)
{
   int i;

	HxLOG_Info("\n");

   for (i = 0; i < MAGIC_SIZE; i++)
   {
      if (pu8Numbers[i] != au8MagicNumber[i])
      {
		  HxLOG_Print("Magic Number NOT_OK!\n");

         return (-1);
      }
   }
   HxLOG_Print("Magic Number OK!\n");
   return (0);

}




IHL_S16 IRLDVT_I_HLSUP_GetStatusInfo (IHL_PSTATUS pstStatus)
{
   IHL_U8 au8RevKey[FLASH_BB_KEY_LEN];
   IHL_U8 au8Key[FLASH_BB_KEY_LEN];
   int    i;
   IHL_U8 u8FailMask = 0x01;
   /* Failed NVRAM blocks based on Magic Numbers -
      use bit pattern to indicate failure of multiple blocks */
   IHL_U8 u8FailedBlocks;

   /* Block start indices into NVRAM partition */
   IHL_U16 au16BlockIndex[NO_BLOCKS] =
   {
      (IHL_U16)(SYSID_INDEX),
      (IHL_U16)(SYSIDDUP_INDEX),
      (IHL_U16)(SIGVER0_INDEX),
      (IHL_U16)(SIGVER1_INDEX)
   };

   /* Sizes (in bytes) of blocks in NVRAM partition */
   IHL_U16 au16BlockSize[NO_BLOCKS] =
   {
      (IHL_U16)(SYSIDDUP_INDEX - SYSID_INDEX),
      (IHL_U16)(SIGVER0_INDEX - SYSIDDUP_INDEX),
      (IHL_U16)(SIGVER1_INDEX - SIGVER0_INDEX),
      (IHL_U16)(LAST_INDEX - SIGVER1_INDEX)
   };

   /* Magic number indices into NVRAM partition */
   IHL_U16  au16MagicIndex[NO_BLOCKS] =
   {
      (IHL_U16)(MAGIC1_INDEX),
      (IHL_U16)(MAGIC2_INDEX),
      (IHL_U16)(MAGIC3_INDEX),
      (IHL_U16)(MAGIC4_INDEX)
   };

   u8FailedBlocks = 0;

	HxLOG_Info("");

   /* jfd 11/6/99 Check for NULL ptr passed */
   if (pstStatus == NULL)
   {
   	  printf("\n[%s],[%d] I_HLSUP_ERR_NULLPTR ++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
      return (I_HLSUP_ERR_NULLPTR);
   }

   /* jfd 11/6/99 Initialise output structure */
   pstStatus->s16SysIDValid  = I_HLSUP_INVALID;
   pstStatus->s16KeyVerValid = I_HLSUP_INVALID;
   pstStatus->s16SigVerValid = I_HLSUP_INVALID;
   pstStatus->u8BlockFailMask = 0;
   for (i = 0; i < NO_BLOCKS; i++)
   {
      pstStatus->u8BlockFailMask &= 1 << i;
   }

   /* Read Irdeto NVRAM partition using manufacturer's routine */
   if (IRLDVT_M_HLSUP_ReadNVRAM (au8IrdetoScram, 0, IR_PARTITION_IRDETO_SIZE) == 0)
   {
      /* jfd 11/6/99 Obtain boot block control block information */
      if (IRLDVT_M_HLSUP_GetBBInfo (au8Key, &au8Key[SERIAL_NUMBER_SIZE]) == 0)
      {
			printf("\n[%s],[%d]err okay at IRLDVT_M_HLSUP_GetBBInfo++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
         /* Reverse the key byte order */
         for (i = 0; i < FLASH_BB_KEY_LEN; i++)
         {
            au8RevKey[i] = au8Key[FLASH_BB_KEY_LEN - 1 - i];
         }

         /* Decrypt and check magic numbers of all blocks in NVRAM partition */
         u8FailMask = 0x01;
         for (i = 0; i < NO_BLOCKS; i++)
         {
            ILDVTI_HLSUP_VariBlockCipher (&au8IrdetoClear[au16BlockIndex[i]],
                                     &au8IrdetoScram[au16BlockIndex[i]],
                                     au16BlockSize[i],
                                     &au8RevKey[0],
                                     FLASH_BB_KEY_LEN);

            /* Check for magic numbers in NVRAM partition */
            if (IRLDVTI_HLSUP_MagicNumbersOK(&au8IrdetoClear[au16MagicIndex[i]])
                  == -1)
            {
               u8FailedBlocks |= u8FailMask;
            }
            /* rls - 15/05/98 : bFailMask should be shifted for each loop
               increment */
            u8FailMask <<= 1;
         }

         /* Copy failed block mask into output structure */
         pstStatus->u8BlockFailMask = u8FailedBlocks;

         /* jfd 11/6/99 Decide which elements are valid and update output
            structure accordingly */
         /* jfd 11/6/99 Handle sys_id, key ver and variant together */
         if (BlockGood (NVRAM_KEY_BLOCK))
         {
            if (BlockGood (NVRAM_DUP_BLOCK))
            {
               /* jfd 11/6/99 Both blocks good therefore check individual
                  items */
               if (IRLDVT_I_HLSUP_GetWord (SYSID_INDEX)
                     == IRLDVT_I_HLSUP_GetWord (SYSIDDUP_INDEX))
               {
                  pstStatus->s16SysIDValid = I_HLSUP_VALID;
                  pstStatus->u16SysID = IRLDVT_I_HLSUP_GetWord (SYSID_INDEX);
               }

               if (IRLDVT_I_HLSUP_GetWord (KEYVER_INDEX)
                     == IRLDVT_I_HLSUP_GetWord (KEYVERDUP_INDEX))
               {
                  pstStatus->s16KeyVerValid = I_HLSUP_VALID;
                  pstStatus->u16KeyVer = IRLDVT_I_HLSUP_GetWord (KEYVER_INDEX);
               }

               if (IRLDVT_I_HLSUP_GetWord (VARIANT_INDEX)
                     == IRLDVT_I_HLSUP_GetWord (VARIANTDUP_INDEX))
               {
                  pstStatus->s16VariantValid = I_HLSUP_VALID;
                  pstStatus->u16Variant = IRLDVT_I_HLSUP_GetWord (VARIANT_INDEX);
               }
            }
            else /* BlockGood (NVRAM_DUP_BLOCK) */
            {
               /* Only key block valid, therefore use values from there */
               pstStatus->s16SysIDValid = I_HLSUP_VALID;
               pstStatus->u16SysID = IRLDVT_I_HLSUP_GetWord (SYSID_INDEX);

               pstStatus->s16KeyVerValid = I_HLSUP_VALID;
               pstStatus->u16KeyVer = IRLDVT_I_HLSUP_GetWord (KEYVER_INDEX);

               pstStatus->s16VariantValid = I_HLSUP_VALID;
               pstStatus->u16Variant = IRLDVT_I_HLSUP_GetWord (VARIANT_INDEX);
            }
         }
         else /* BlockGood (NVRAM_KEY_BLOCK) */
         {
            if (BlockGood (NVRAM_DUP_BLOCK))
            {
               /* Only dup block valid, therefore use values from there */
               pstStatus->s16SysIDValid = I_HLSUP_VALID;
               pstStatus->u16SysID = IRLDVT_I_HLSUP_GetWord (SYSIDDUP_INDEX);

               pstStatus->s16KeyVerValid = I_HLSUP_VALID;
               pstStatus->u16KeyVer = IRLDVT_I_HLSUP_GetWord (KEYVERDUP_INDEX);

               pstStatus->s16VariantValid = I_HLSUP_VALID;
               pstStatus->u16Variant = IRLDVT_I_HLSUP_GetWord (VARIANTDUP_INDEX);
            }
         }

         /* jfd 11/6/99 Now handle signature version */
         if (BlockGood (NVRAM_VER0_BLOCK))
         {
            if (BlockGood (NVRAM_VER1_BLOCK))
            {
               /* jfd 11/6/99 Both blocks good therefore check values */
               if (IRLDVT_I_HLSUP_GetWord (SIGVER0_INDEX)
                     == IRLDVT_I_HLSUP_GetWord (SIGVER1_INDEX))
               {
                  pstStatus->s16SigVerValid = I_HLSUP_VALID;
                  pstStatus->u16SigVer = IRLDVT_I_HLSUP_GetWord (SIGVER0_INDEX);
               }
            }
            else /* BlockGood (NVRAM_VER1_BLOCK) */
            {
               /* jfd 11/6/99 Only ver0 good, therefore use value there */
               pstStatus->s16SigVerValid = I_HLSUP_VALID;
               pstStatus->u16SigVer = IRLDVT_I_HLSUP_GetWord (SIGVER0_INDEX);
            }
         }
         else /* BlockGood (NVRAM_VER1_BLOCK) */
         {
            if (BlockGood (NVRAM_VER1_BLOCK))
            {
               /* jfd 11/6/99 Only ver1 good, therefore use value there */
               pstStatus->s16SigVerValid = I_HLSUP_VALID;
               pstStatus->u16SigVer = IRLDVT_I_HLSUP_GetWord (SIGVER1_INDEX);
            }
         }
         /* jfd 11/6/99 Trash clear image */
         for (i = 0; i < IR_PARTITION_IRDETO_SIZE; i++)
         {
            au8IrdetoClear[i] = 0;
         }

         return (I_HLSUP_OK);
      }
      else /* Failed to obtain boot block control block info */
      {
         /* jfd 11/6/99 Trash clear image */
         for (i = 0; i < IR_PARTITION_IRDETO_SIZE; i++)
         {
            au8IrdetoClear[i] = 0;
         }

         return (I_HLSUP_ERR_BBINFO);
      }
   }
   else /* NVRAM read failed */
   {
   	printf("\n[%s],[%d]NVRAM read failed++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
      return(I_HLSUP_ERR_READNVRAM);
   }
} /* I_HLSUP_GetStatusInfo */

