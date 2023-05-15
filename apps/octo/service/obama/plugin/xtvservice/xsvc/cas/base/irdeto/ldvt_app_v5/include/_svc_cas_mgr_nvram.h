/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_cas_mgr_nvram.h
	@brief	  Eeprom resource I/F

	Description: CAS Module에서 사용하는 NVRAM i/f 해주는 모듈.\n
	Module: MW/CAS/			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	_CASMGR_NVRAM_H_
#define	_CASMGR_NVRAM_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define CA_NVRAM_START_OFFSET						0x0000

#define ____CA_NVRAM_ZONE_DEFINE_______________________________________________________________
// for example
//#define CA_NVRAM_MW_TIME							CA_NVRAM_START_OFFSET
//#define CA_NVRAM_MW_TIME_SIZE						512



#define _____CA_NVRAM_DEFINE_______________________________________________________________
#if defined(CONFIG_MW_CAS_IRDETO)
/* CAUTIONS: DO NOT CORRUPT IRDETO SYSTEM PARTITION, it's related with CA SYSTEM & LOADER */
//#define IR_NVRAM_PARTITION_IRDETO_OFFSET			0x0E00
#define IR_NVRAM_PARTITION_IRDETO_SIZE				0x0200
//#define IR_NVRAM_PARTITION_LOADER_OFFSET			(IR_NVRAM_PARTITION_IRDETO_OFFSET+IR_NVRAM_PARTITION_IRDETO_SIZE)//0x1000
#define IR_NVRAM_PARTITION_LOADER_SIZE				0x0200
//#define IR_NVRAM_PARTITION_CA_OFFSET				(IR_NVRAM_PARTITION_LOADER_OFFSET+IR_NVRAM_PARTITION_LOADER_SIZE)//0x1200
#define IR_NVRAM_PARTITION_CA_SIZE					0x0200
//#define IR_NVRAM_PARTITION_IRDETO_BACKUP_OFFSET		(IR_NVRAM_PARTITION_CA_OFFSET+IR_NVRAM_PARTITION_CA_SIZE)//0x1400
#define IR_NVRAM_PARTITION_IRDETO_BACKUP_SIZE		0x0200


//#define CA_NVRAM_CAS_IR_CA_SYSTEM_RESERVED_OFFSET		(CA_NVRAM_START_OFFSET)
//#define CA_NVRAM_CAS_IR_CA_SYSTEM_RESERVED_SIZE			(IR_NVRAM_PARTITION_IRDETO_SIZE+IR_NVRAM_PARTITION_LOADER_SIZE+IR_NVRAM_PARTITION_CA_SIZE+IR_NVRAM_PARTITION_IRDETO_BACKUP_SIZE)

// TODO: it would be not used.
//#define CA_NVRAM_PIN_INFORMATION_OFFSET				0
//#define CA_NVRAM_PIN_INFORMATION_SIZE					0

//#define CA_NVRAM_IRDETO_OFFSET_END						(CA_NVRAM_CAS_IR_CA_SYSTEM_RESERVED_OFFSET + CA_NVRAM_CAS_IR_CA_SYSTEM_RESERVED_SIZE)
#endif

#if 0//defined(CONFIG_MW_CAS_IRDETO) /* NA/IR 각각 변도의 file을 access하므로 적용 필요 없음 */
#define CA_NVRAM_CAS_NA_OFFSETSTART						CA_NVRAM_IRDETO_OFFSET_END
#else
#define CA_NVRAM_CAS_NA_OFFSETSTART						CA_NVRAM_START_OFFSET
#endif

#if defined(CONFIG_MW_CAS_NAGRA)
#define CA_NVRAM_CAS_NA									CA_NVRAM_CAS_NA_OFFSETSTART
#define CA_NVRAM_CAS_NA_SIZE							(512)

#define CA_NVRAM_PIN_INFORMATION_OFFSET					(CA_NVRAM_CAS_NA + CA_NVRAM_CAS_NA_SIZE)
#define CA_NVRAM_PIN_INFORMATION_SIZE					(28 * 3)

#define CA_NVRAM_IRDCMD_SEQNUM_OFFSET					(CA_NVRAM_PIN_INFORMATION_OFFSET + CA_NVRAM_PIN_INFORMATION_SIZE)
#define CA_NVRAM_IRDCMD_SEQNUM_SIZE						(748)			// 68 * 11개 Command 를 저장할 공간 필요.

#define CA_NVRAM_IRDCMD_SEQNUM_END						(CA_NVRAM_IRDCMD_SEQNUM_OFFSET + CA_NVRAM_IRDCMD_SEQNUM_SIZE)

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
#define CA_NVRAM_IRDCMD_MAIL_SEQNUM_OFFSET				CA_NVRAM_IRDCMD_SEQNUM_END
#define CA_NVRAM_IRDCMD_MAIL_SEQNUM_SIZE				(1204)

#define CA_NVRAM_IRDCMD_MAIL_HEADER_OFFSET				(CA_NVRAM_IRDCMD_MAIL_SEQNUM_OFFSET + CA_NVRAM_IRDCMD_MAIL_SEQNUM_SIZE)
#define CA_NVRAM_IRDCMD_MAIL_HEADER_SIZE				(60)			// sizeof(TSMailInfo)

#define CA_NVRAM_IRDCMD_MAIL_END						(CA_NVRAM_IRDCMD_MAIL_HEADER_OFFSET + CA_NVRAM_IRDCMD_MAIL_HEADER_SIZE)
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
#define CA_NVRAM_CAS_NA_OFFSETSTART_MAIL				CA_NVRAM_IRDCMD_MAIL_END
#else
#define CA_NVRAM_CAS_NA_OFFSETSTART_MAIL				CA_NVRAM_IRDCMD_SEQNUM_END
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
#define CA_NVRAM_IRDCMD_POPUP_SEQNUM_OFFSET				CA_NVRAM_CAS_NA_OFFSETSTART_MAIL
#define CA_NVRAM_IRDCMD_POPUP_SEQNUM_SIZE				(1204)

#define CA_NVRAM_IRDCMD_POUP_END						(CA_NVRAM_IRDCMD_POPUP_SEQNUM_OFFSET + CA_NVRAM_IRDCMD_POPUP_SEQNUM_SIZE)
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
#define CA_NVRAM_CAS_NA_OFFSETSTART_POPUP				CA_NVRAM_IRDCMD_POUP_END
#else
#define CA_NVRAM_CAS_NA_OFFSETSTART_POPUP				CA_NVRAM_CAS_NA_OFFSETSTART_MAIL
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
#define CA_NVRAM_IRDCMD_EMMWAKEUP_OFFSET				CA_NVRAM_CAS_NA_OFFSETSTART_POPUP
#define CA_NVRAM_IRDCMD_EMMWAKEUP_SIZE					(128)

#define CA_NVRAM_IRDCMD_EMMWAKEUP_END					(CA_NVRAM_IRDCMD_EMMWAKEUP_OFFSET + CA_NVRAM_IRDCMD_EMMWAKEUP_SIZE)
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
#define CA_NVRAM_CAS_NA_OFFSETSTART_EMMWAKEUP			CA_NVRAM_IRDCMD_EMMWAKEUP_END
#else
#define CA_NVRAM_CAS_NA_OFFSETSTART_EMMWAKEUP			CA_NVRAM_CAS_NA_OFFSETSTART_POPUP
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
#define CA_NVRAM_IRDCMD_MAGICCODE_OFFESET				CA_NVRAM_CAS_NA_OFFSETSTART_EMMWAKEUP
#define CA_NVRAM_IRDCMD_MAGICCODE_SIZE					(4)

#define CA_NVRAM_IRDCMD_CUR_PROFILE_OFFESET				(CA_NVRAM_IRDCMD_MAGICCODE_OFFESET + CA_NVRAM_IRDCMD_MAGICCODE_SIZE)
#define CA_NVRAM_IRDCMD_CUR_PROFILE_SIZE				(8 * 2)

#define CA_NVRAM_IRDCMD_PROFILE_OFFESET					(CA_NVRAM_IRDCMD_CUR_PROFILE_OFFESET + CA_NVRAM_IRDCMD_CUR_PROFILE_SIZE)
#define CA_NVRAM_IRDCMD_PROFILE_SIZE					(21 * 2)  	// 4*5 = 20 + 1

#define CA_NVRAM_IRDCMD_MESSAGE_OFFSET					(CA_NVRAM_IRDCMD_PROFILE_OFFESET + CA_NVRAM_IRDCMD_PROFILE_SIZE)
#define CA_NVRAM_IRDCMD_MESSAGE_SIZE					(51 * 2)

#define CA_NVRAM_NAGRA_APS_END							(CA_NVRAM_IRDCMD_MESSAGE_OFFSET + CA_NVRAM_IRDCMD_MESSAGE_SIZE)
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
#define CA_NVRAM_CAS_NA_OFFSETSTART_APS					CA_NVRAM_NAGRA_APS_END
#else
#define CA_NVRAM_CAS_NA_OFFSETSTART_APS					CA_NVRAM_CAS_NA_OFFSETSTART_EMMWAKEUP
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
#define CA_NVRAM_CREDIT_INFO_OFFSET						CA_NVRAM_CAS_NA_OFFSETSTART_APS
#define CA_NVRAM_CREDIT_INFO_SIZE						(1)

#define CA_NVRAM_NAGRA_RCS_END							(CA_NVRAM_CREDIT_INFO_OFFSET + CA_NVRAM_CREDIT_INFO_SIZE)
#endif

#if defined(CONFIG_NAGRA_RCS)
#define CA_NVRAM_CAS_NA_OFFSETSTART_RCS					CA_NVRAM_NAGRA_RCS_END
#else
#define CA_NVRAM_CAS_NA_OFFSETSTART_RCS					CA_NVRAM_CAS_NA_OFFSETSTART_APS
#endif

#define CA_NVRAM_CAS_NA_CSCD_OFFSET						CA_NVRAM_CAS_NA_OFFSETSTART_RCS
#define CA_NVRAM_CAS_NA_CSCD_SIZE						(256)
#endif


#define _____CA_NVRAM_MW_DEFINE_______________________________________________________________

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

#endif /* !_CASMGR_NVRAM_H_ */



