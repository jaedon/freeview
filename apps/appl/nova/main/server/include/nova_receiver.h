/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* �� 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


#ifndef __NOVA_RECEIVER__H__
#define __NOVA_RECEIVER__H__

#include "nova_errcode.h"


/**
*********************************************************************************************************
  @file			nova_receiver.h
  @brief		TS receiver
  @date			2015/7/24
  @author		http://www.humaxdigital.com
  @attention	Copyright (c) 2015 Humax - All rights reserved.
  @mainpage
	TS Receiver ����:
		SAT>IP Client���κ��� Streaming ��û�� ���� Sama���� TP ������ �Բ� hRingbuf �ڵ��� �Ѱ��ָ鼭 Schedule�� �����Ѵ�.
		Schedule ������ Obama���� ���޵Ǿ� valid�� TP ������� Signal lock�� �ȴ�.
		�� ���� Tuner�κ��� TS�� �����ϸ� Tuner ==> DI/Obama ==> Nova (or Homma) ������ TS��Ŷ�� ���޵ȴ�.
		Obama���� Nova Process�� TS ��Ŷ ������ UDS(TCP) socket�� ����ϴµ� ������ ������ ������ ����.
		  * UDS Packet = hringbuf(4byte) + DataSize(4byte) + TS Packets
		TS Receiver�� UDS socket���� ���޹��� ��Ŷ�� hringbuf�� TS��Ŷ�� Write���ִ� ��Ȱ������ ����Ѵ�.
*********************************************************************************************************
*/

#ifdef	__cplusplus
extern "C" {
#endif

/**
*********************************************************************************************************
  @brief	SAT>IP Server�� ���۵� �� ȣ���Ѵ�.
			Obama�κ��� TS packet�� �����ϱ� ���Ͽ� UDS Socket�� �����ϰ� Listening �����Ѵ�.
  @return	�����ϸ� NERR_OK�� �����Ѵ�.
*********************************************************************************************************
*/
NOVA_ErrCode_e NOVA_Receiver_Init(void);


/**
*********************************************************************************************************
  @brief	SAT>IP Server�� ����� �� ȣ���Ͽ� �����ߴ� resource�� ��ȯ�Ѵ�.
  @return	�����ϸ� NERR_OK�� �����Ѵ�.
*********************************************************************************************************
*/
NOVA_ErrCode_e NOVA_Receiver_DeInit(void);

/**
*********************************************************************************************************
  @brief	Obama�κ��� ���Ź޴� TS��  ���Ϸ� dump�ϱ� ���� cmduartcosnole/nova/dump_ringbuf ����� ����Ѵ�.
  @return	N/A
*********************************************************************************************************
*/
void		   NOVA_CMDRegister_StreamingReceive(const HCHAR *hCmdHandle);

#ifdef	__cplusplus
}
#endif

#endif
