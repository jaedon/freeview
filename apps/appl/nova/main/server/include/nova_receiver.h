/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
	TS Receiver 로직:
		SAT>IP Client으로부터 Streaming 요청이 오면 Sama에게 TP 정보와 함께 hRingbuf 핸들을 넘겨주면서 Schedule을 생성한다.
		Schedule 정보는 Obama에게 전달되어 valid한 TP 정보라면 Signal lock이 된다.
		그 이후 Tuner로부터 TS을 수신하면 Tuner ==> DI/Obama ==> Nova (or Homma) 순으로 TS패킷이 전달된다.
		Obama에서 Nova Process로 TS 패킷 전달은 UDS(TCP) socket을 사용하는데 데이터 포멧은 다음과 같다.
		  * UDS Packet = hringbuf(4byte) + DataSize(4byte) + TS Packets
		TS Receiver는 UDS socket으로 전달받은 패킷을 hringbuf에 TS패킷을 Write해주는 역활까지만 담당한다.
*********************************************************************************************************
*/

#ifdef	__cplusplus
extern "C" {
#endif

/**
*********************************************************************************************************
  @brief	SAT>IP Server가 시작될 때 호출한다.
			Obama로부터 TS packet를 수신하기 위하여 UDS Socket을 생성하고 Listening 시작한다.
  @return	성공하면 NERR_OK을 리턴한다.
*********************************************************************************************************
*/
NOVA_ErrCode_e NOVA_Receiver_Init(void);


/**
*********************************************************************************************************
  @brief	SAT>IP Server가 종료될 때 호출하여 생성했던 resource를 반환한다.
  @return	성공하면 NERR_OK을 리턴한다.
*********************************************************************************************************
*/
NOVA_ErrCode_e NOVA_Receiver_DeInit(void);

/**
*********************************************************************************************************
  @brief	Obama로부터 수신받는 TS를  파일로 dump하기 위한 cmduartcosnole/nova/dump_ringbuf 명령을 등록한다.
  @return	N/A
*********************************************************************************************************
*/
void		   NOVA_CMDRegister_StreamingReceive(const HCHAR *hCmdHandle);

#ifdef	__cplusplus
}
#endif

#endif
