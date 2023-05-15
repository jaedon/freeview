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

#ifndef	___INT_STREAMING_UDS_H___
#define	___INT_STREAMING_UDS_H___

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

/**
******************************************************************************************
  @brief SAT>IP Owner RTSP SETUP+SRC+TP...(init) ���·� ��û�޾Ƽ� _plstreaming_Callback_Streaming_Start ȣ��Ǿ��� ��
         UDS Manager�� �����ϱ� ���ؼ� ȣ���մϴ�.
  @param socketfd hringbuf���� TS�� �о live555 ��⿡ send�Ҷ� ����ϴ� socketfd�Դϴ�.
         socket ������ SOCK_NONBLOCK �ɼ����� �����Ͽ����ϴ�.
  @param hringbuf Homma�� ������ hringbuf handle.
  @return UDS Manager instance�� ��ȯ�˴ϴ�. ���� ����ü�� class UdsManager �Դϴ�.

******************************************************************************************
*/
void* UDS_CreateManager(int socketfd, Handle_t hringbuf);

/**
******************************************************************************************
  @brief SAT>IP Owner�� RTSP TEARDOWN ��û�޾Ƽ� _plstreaming_Callback_Streaming_End ȣ��Ǿ��� ��
         UDS Manager�� �����ϱ� ���ؼ� ȣ���մϴ�.
  @param UdsObj  UDS_CreateManagerȣ���� ��ȯ�� ������.
  @return �����ϸ� true, �����ϸ� false�� ��ȯ�մϴ�.
******************************************************************************************
*/
bool UDS_DestroyManager(void* UdsObj);

/**
******************************************************************************************
  @brief SAT>IP Streaming�� hringbuf�� ����� ���� �� ȣ���մϴ�. UDS_BUFFER_TIME �ð���ŭ buffering�ϰ� �˴ϴ�.
  @param UdsObj UDS_CreateManagerȣ���� ��ȯ�� ������.
  @param hringbuf ringubf handle.
  @return �����ϸ� true, �����ϸ� false�� ��ȯ�մϴ�.
******************************************************************************************
*/
bool        UDS_SetRingBuf      (void* UdsObj, Handle_t hringbuf);

/**
******************************************************************************************
  @brief SAT>IP Non-owner streaming ��û �޾��� �� owner streaming�� ringbuf���� ���� Client���� ts packet�� �����ϱ� ���ؼ�
         ������ client�� socketfd�� �߰��մϴ�.
  @param UdsObj UDS_CreateManagerȣ���� ��ȯ�� ������.
  @param socketfd ���ο� client�� socketfd.
  @return �����ϸ� true, �����ϸ� false�� ��ȯ�մϴ�.
******************************************************************************************
*/
bool        UDS_AddSocket       (void* UdsObj, int socketfd);

/**
******************************************************************************************
  @brief SAT>IP Non-owner streaming�� ����Ǿ��� �� client�� socketfd�� ������ �� ȣ���մϴ�.
  @param UdsObj UDS_CreateManagerȣ���� ��ȯ�� ������.
  @param socketfd ������ socketfd.
  @return �����ϸ� true, �����ϸ� false�� ��ȯ�մϴ�.
******************************************************************************************
*/
bool        UDS_RemoveSocket    (void* UdsObj, int socketfd);

/**
******************************************************************************************
  @brief Homma Ringbuf�� TS�� ���̰� ���� �� bSend=true�� ȣ���ؾ� live555���� TS�� ���޵Ǳ� �����մϴ�.
         client���κ��� pids=none ���� ��û�� ���� Ringbuf�� ���̴��� live555���� TS�� �����ϸ� �ȵ˴ϴ�. �̶��� bSend=false�� ȣ���ؾ� �մϴ�.
  @param UdsObj UDS_CreateManagerȣ���� ��ȯ�� ������.
  @param socketfd ������ socketfd
  @param bSend  true/false. true�϶��� live555���� RingBuf�� ���� TS�� �����Ѵ�.
  @return �����ϸ� true, �����ϸ� false�� ��ȯ�մϴ�.
******************************************************************************************
*/
bool        UDS_SetSendingFlag  (void* UdsObj, int socketfd, bool bSend);

/**
******************************************************************************************
  @brief SAT>IP ��û �� TP Locking�� �Ϸ�Ǿ��� �� ȣ���մϴ�. UDS Manager�� �̶����� RingBuf�� �б� �����մϴ�.
         ringbuf���� ���ŵ� ts�� ������Ű�µ� add�� �� socket�� sendingFlag�� ���� ���۵ǰų� skip�˴ϴ�.
  @param UdsObj UDS_CreateManagerȣ���� ��ȯ�� ������.
  @param signalLocked true/false
  @return �����ϸ� true, �����ϸ� false�� ��ȯ�մϴ�.
******************************************************************************************
*/
bool        UDS_SetSignalLock   (void* UdsObj, bool signalLocked);

/**
******************************************************************************************
  @brief receiver socket buffer�� flushing �մϴ�.
  @param socketfd receiver socket.
  @return �����ϸ� true, �����ϸ� false�� ��ȯ�մϴ�.
******************************************************************************************
*/
bool        UDS_ClearSocketBuffer(int socketfd);

#ifdef __cplusplus
}
#endif

#endif	//___INT_STREAMING_H___
