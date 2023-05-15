/*******************************************************************
7	File Description
********************************************************************/
/**
	@file
	@brief
    @author mhkang2@humaxdigital.com

    Description:  	UDS Manager for SAT>IP Streaming			\n
    Module: PL/STREAMING				\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

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

#ifndef	___INT_STREAMING_UDS_H___
#define	___INT_STREAMING_UDS_H___

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

/**
******************************************************************************************
  @brief SAT>IP Owner RTSP SETUP+SRC+TP...(init) 형태로 요청받아서 _plstreaming_Callback_Streaming_Start 호출되었을 때
         UDS Manager를 생성하기 위해서 호출합니다.
  @param socketfd hringbuf에서 TS를 읽어서 live555 모듈에 send할때 사용하는 socketfd입니다.
         socket 생성시 SOCK_NONBLOCK 옵션으로 생성하였습니다.
  @param hringbuf Homma가 생성한 hringbuf handle.
  @return UDS Manager instance가 반환됩니다. 실제 구현체는 class UdsManager 입니다.

******************************************************************************************
*/
void* UDS_CreateManager(int socketfd, Handle_t hringbuf);

/**
******************************************************************************************
  @brief SAT>IP Owner가 RTSP TEARDOWN 요청받아서 _plstreaming_Callback_Streaming_End 호출되었을 때
         UDS Manager를 해제하기 위해서 호출합니다.
  @param UdsObj  UDS_CreateManager호출후 반환된 포인터.
  @return 성공하면 true, 실패하면 false를 반환합니다.
******************************************************************************************
*/
bool UDS_DestroyManager(void* UdsObj);

/**
******************************************************************************************
  @brief SAT>IP Streaming중 hringbuf를 재생성 했을 때 호출합니다. UDS_BUFFER_TIME 시간만큼 buffering하게 됩니다.
  @param UdsObj UDS_CreateManager호출후 반환된 포인터.
  @param hringbuf ringubf handle.
  @return 성공하면 true, 실패하면 false를 반환합니다.
******************************************************************************************
*/
bool        UDS_SetRingBuf      (void* UdsObj, Handle_t hringbuf);

/**
******************************************************************************************
  @brief SAT>IP Non-owner streaming 요청 받았을 때 owner streaming의 ringbuf에서 여러 Client으로 ts packet을 전송하기 위해서
         전송할 client의 socketfd을 추가합니다.
  @param UdsObj UDS_CreateManager호출후 반환된 포인터.
  @param socketfd 새로운 client의 socketfd.
  @return 성공하면 true, 실패하면 false를 반환합니다.
******************************************************************************************
*/
bool        UDS_AddSocket       (void* UdsObj, int socketfd);

/**
******************************************************************************************
  @brief SAT>IP Non-owner streaming이 종료되었을 때 client의 socketfd를 제거할 때 호출합니다.
  @param UdsObj UDS_CreateManager호출후 반환된 포인터.
  @param socketfd 제거할 socketfd.
  @return 성공하면 true, 실패하면 false를 반환합니다.
******************************************************************************************
*/
bool        UDS_RemoveSocket    (void* UdsObj, int socketfd);

/**
******************************************************************************************
  @brief Homma Ringbuf에 TS가 쌓이고 있을 때 bSend=true로 호출해야 live555에게 TS가 전달되기 시작합니다.
         client으로부터 pids=none 같은 요청이 오면 Ringbuf에 쌓이더라도 live555에게 TS를 전달하면 안됩니다. 이때는 bSend=false로 호출해야 합니다.
  @param UdsObj UDS_CreateManager호출후 반환된 포인터.
  @param socketfd 전송할 socketfd
  @param bSend  true/false. true일때마 live555에게 RingBuf에 쌓인 TS를 전달한다.
  @return 성공하면 true, 실패하면 false를 반환합니다.
******************************************************************************************
*/
bool        UDS_SetSendingFlag  (void* UdsObj, int socketfd, bool bSend);

/**
******************************************************************************************
  @brief SAT>IP 요청 후 TP Locking이 완료되었을 때 호출합니다. UDS Manager는 이때부터 RingBuf를 읽기 시작합니다.
         ringbuf에서 수신된 ts를 소진시키는데 add된 각 socket의 sendingFlag에 따라 전송되거나 skip됩니다.
  @param UdsObj UDS_CreateManager호출후 반환된 포인터.
  @param signalLocked true/false
  @return 성공하면 true, 실패하면 false를 반환합니다.
******************************************************************************************
*/
bool        UDS_SetSignalLock   (void* UdsObj, bool signalLocked);

/**
******************************************************************************************
  @brief receiver socket buffer를 flushing 합니다.
  @param socketfd receiver socket.
  @return 성공하면 true, 실패하면 false를 반환합니다.
******************************************************************************************
*/
bool        UDS_ClearSocketBuffer(int socketfd);

#ifdef __cplusplus
}
#endif

#endif	//___INT_STREAMING_H___
