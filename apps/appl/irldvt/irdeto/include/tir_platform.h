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


#ifndef	___IR_PLATFORM_H_
#define	___IR_PLATFORM_H_

//#include <octo_common.h>
//#include <casmgr_common.h>
#include <autoconf.h>
#include <hlib.h>
// TODO: jichoi
//#define USE_INCOMPLETED_IR_LOADER
//#define USE_INCOMPLETED_VD
//#define USE_INCOMPLETED_IR_APP
//#define USE_DONOTUSE_MGR_TS_API
//#define USE_HIGH_PRIORITY_T14
// TODO: jichoi
//#define SUPPORT_IRDETO_VENDOR_ID
//#define SUPPORT_IR_DRV_USE_MUTEX (FALSE)
//#define SUPPORT_DONOT_STORE_SAME_MAIL
//#define SUPPORT_FORCE_TUNE
//#define SUPPORT_POWERON_STANDBY
//#define SUPPORT_IRDETO_PVR
//#define SUPPORT_IRDETO_SOL
//#define SUPPORT_SCSN_ON_EEPROM
//#define SUPPORT_IRDETO_COPYCONTROL


#if defined(SUPPORT_IRDETO_PVR) //for 2 tuner
#define IR_HUMAX_MAX_SERVICE			NUM_MAX_ACTION
#define IR_HUMAX_MAX_TP				NUM_MAX_ACTION
#else
#define IR_HUMAX_MAX_SERVICE			1
#define IR_HUMAX_MAX_TP				1
#endif

#define IR_SOFTCELL_MAX_SERVICE		(IR_HUMAX_MAX_SERVICE)


#define IR_SVC_MAX_PID      			16 //(MXI_MAX_PID_LIST)

#define IR_CA_SYSTEM_ID_MIN		(0x600)
#define IR_CA_SYSTEM_ID_MAX	(0x6FF)

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


#define HMX_INVALID_INDEX			 (0xFFFFFFFF)

#define HMX_DSC_INVALID_PAL_IDX	(0xFFFFFFFF)
#define HMX_INVALID_PID			 	(0x2000)

#define HMX_VD_DMX_MAX_NUM					4 //(MAX_NUM_OF_DMX) // TODO: PAL에서 가져와서 매핑해야 함.
#define HMX_VD_DMX_MAX_FILTER_PER_CHANNEL	16
#define HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX	16
#define HMX_VD_DMX_MAX_TOTAL_CHANNEL		(HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX * HMX_VD_DMX_MAX_NUM)

#define HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX 	8 //16 //(16) // (1 svc has 4 es ) *  2 svc * 2 tuner
#define HMX_VD_DSC_REFID_MAX					HMX_VD_DMX_MAX_TOTAL_CHANNEL//60 // total ref dsc IDs in chipset //(VD_MAX_NUM_OF_DSC_ID * HMX_VD_DMX_MAX_NUM)

#define HMX_VD_TIMER_MAX_NUM		16


/////////////////////////////////////////////////////////////
// VD DRIVER VERSION
#define VD_SMARTCARD_V1				1
#define VD_SMARTCARD_V2_SOFTCLIENT	2
#define VD_SMARTCARD_V3_JAVACARD	3
#define VD_SMARTCARD_V4_CARDFORM	4
#define VD_SMARTCARD_V5_DIRECTATR	5

#define VD_FILTER_V1					1 // TODO: Do not use Demux version 1, It's too slow si handling. jichoi.
#define VD_FILTER_V2_DIRECTREAD	2
#define VD_FILTER_V3_DMB			3

#define VD_DESCRAMBLER_VERSION_V1							1
#define VD_DESCRAMBLER_VERSION_V2_SECURE_CHIP				2
#define VD_DESCRAMBLER_VERSION_V3_SECURE_CHIP_MULTICARD	3
#define VD_DESCRAMBLER_VERSION_V4_MULTIPLE_PID				4

#define VD_MACROVISION_V1			1
#define VD_MACROVISION_V2			2

#define VD_LOADER_VERSION_V1						1
#define VD_LOADER_VERSION_V2_SECURE_CHIP		2

#define VD_TIMER_V1					1
#define VD_STREAMCRYPTO_V1		1
#define VD_EXTERNAL_STORAGE_V1	1
#define VD_PERSISTENT_STORAGE_V1	1


#if defined(CAS_IR_ICE2_PROJECT)
#define VD_SMARTCARD_VERSION				VD_SMARTCARD_V5_DIRECTATR
#else
#define VD_SMARTCARD_VERSION				VD_SMARTCARD_V1
#endif
#define VD_PERSISTENT_STORAGE_VERSION	VD_PERSISTENT_STORAGE_V1
#define VD_FILTER_VERSION					VD_FILTER_V2_DIRECTREAD
#define VD_DESCRAMBLER_VERSION			VD_DESCRAMBLER_VERSION_V1
#define VD_TIMER_VERSION					VD_TIMER_V1
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
#define VD_LOADER_VERSION				VD_LOADER_VERSION_V2_SECURE_CHIP
#else
#define VD_LOADER_VERSION				VD_LOADER_VERSION_V1
#endif
#define VD_STREAMCRYPTO_VERSION		VD_STREAMCRYPTO_V1
#define VD_MACROVISION_VERSION		VD_MACROVISION_V2
#define VD_EXTERNAL_STORAGE_VERSION	VD_EXTERNAL_STORAGE_V1

#define IR_CRYPTO_KEY_LEN				16

#ifdef CONFIG_MW_CAS_NUM_SC_SLOT										// Smart card slot 개수, Kconfig에서 설정 됨.
#define HUMAX_VD_SC_SLOT_COUNT			CONFIG_MW_CAS_NUM_SC_SLOT
#else
#define HUMAX_VD_SC_SLOT_COUNT			1
#endif


typedef enum
{
	HIR_OK	=0,
	HIR_NOT_OK,
	HIR_INVALID_PARAMETER,
	HIR_INVALID_MEMORY,
	HIR_ERR_SCELL_INTERNAL,
	HIR_ERR_MAX
}HIR_ERR;

typedef enum IR_MSG_KIND_e
{
	/*Followins are  eIR_MSG_CLASS_CAS Action types.  */
	IR_CATASK_CARD_IN,
	IR_CATASK_CARD_OUT,
	IR_CATASK_NVRAM_WRITE,
	IR_CATASK_FILE_WRITE,
	IR_CATASK_FROM_SOFTCELL,
	IR_CATASK_KIND_MAX_NUM
} IR_MSG_KIND_E;


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/




typedef struct tCAS_IR_SC_MSG
{
	HUINT8		ucSlotID;
	HUINT32		ResourceID;
	HUINT32		ReferenceID;
} CAS_IR_SC_MSG;

typedef struct tCAS_IR_FILTER_MSG
{
	void		*handle;
	HUINT8		ucSvcIndex;
	HUINT8		ucCompIndex;
	HUINT8		ucFilterIndex;
	HUINT8		ucAction;
} CAS_IR_FILTER_MSG;

typedef struct tCAS_IR_CW_MSG
{
	void		*handle;
	HUINT8		ucType;
	HUINT8		*pucEven;
	HUINT8		*pucOdd;
} CAS_IR_CW_MSG;

typedef struct tCAS_IR_EXM_MSG
{
	HUINT8		*pucData;
	HUINT32		ulLen;
	HUINT16		usLoadId;
} CAS_IR_EXM_MSG;

typedef struct tCAS_IR_SOFTCELL_MSG
{
	HUINT32		eOpCode;	// ca_drv_message_type_e
	HUINT16		wOpSeqNo;
	HUINT8		ucFreeFlag;
	HUINT16		wLength;
	void		*pvMessage;
} CAS_IR_SOFTCELL_MSG;

typedef struct tCAS_IR_NVRAM_WRITE_MSG
{
	HUINT32	RefId;
	HUINT32	ulStart;
	HUINT32	ulLen;
	HUINT8	*pucData;
} CAS_IR_STORAGE_WRITE_MSG;

typedef struct tCAS_IR_BASE_MSG
{
	union
	{
		CAS_IR_FILTER_MSG		FILTER;
		CAS_IR_SC_MSG			SC;
		CAS_IR_CW_MSG			CW;
		CAS_IR_EXM_MSG			EXM;
		CAS_IR_SOFTCELL_MSG		SOFTCELL;
		CAS_IR_STORAGE_WRITE_MSG	NVRAM;
		CAS_IR_STORAGE_WRITE_MSG	FILE;
	} msg;
} CAS_IR_BASE_MSG;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


#endif	//___IR_PLATFORM_H_

