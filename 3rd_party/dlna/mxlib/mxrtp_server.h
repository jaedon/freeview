/**************************************************************************************/
/**
*
* \file mxrtp_server.h
* \brief defines mxrtp APIs
*
* \mainpage
* mxrtp_server 모듈은 live555 모듈에서 RTP 기능만을 간추려 만든 API set이다.
* SAT>IP 기능을 구현할 때 rtp 전송부분을 담당한다.
*
* \author  mhkang2@humaxdigital.com
* \date    2014/1/10
* \attention   Copyright (c) 2014 Humax - All rights reserved.
*
**************************************************************************************
**/

#ifndef MXRTP_SERVER_H
#define MXRTP_SERVER_H

#include "mxapi_define.h"
#include "mxtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OUT /*no op*/
typedef void* RTP_HANDLE;

#define RTCP_TYPE_SR    200     // Sender Report
#define RTCP_TYPE_RR    201     // Receiver Report
#define RTCP_TYPE_SDES  202     // Source Description
#define RTCP_TYPE_BYE   203     // BYE BYE
#define RTCP_TYPE_APP   204     // Application User Definition

/*!
****************************************************************************************************
* RTP Server가 RTCP APP 패킷을 전송하기 전에 필요한 정보(reportInfo)를 얻기 위해서 Callback으로 호출된다.
* mxrtp_openServer API에서 callback 등록한다.
*
* \param hRTP       : RTP HANDLE
* \param reportInfo : SETUP new session 맺을때 사용했던 parameter에서 signal level, lock, quality정보가 추가됨(UTF-8)
*        - level, lock, quality 값 정의는 SAT>IP 1.2 spec doc참고
*        - format:  ver=<major>.<minor>;src=<srcID>;tuner=<feID>,<level>,<lock>,<quality>,<frequency>,<polarisation>, <system>,<type>,<pilots>,<roll_off>,<symbol_rate>,<fec_inner>;pids=<pid0>,...,<pidn>
*        - sample: "ver=1.0;src=1;tuner=1,240,1,7,12402,v,dvbs,,,,27500,34;pids=0,16,56,112,168,1709"
*         * out_reportInfo는 malloc으로 메모리 할당할 것.
* \param infoSize   : reportInfo size(bytes)
*
* \retval MX_RETURN_OK 성공
* \retval MX_RETURN_FAIL  No signal
* \retval MX_RETURN_ERROR 실패
****************************************************************************************************
*/
typedef MX_RETURN (*MXRTP_CB_SEND_REPORT)(void* userObject , char** out_reportInfo, unsigned int* out_infoSize);


/*!
****************************************************************************************************
* RTCP packet을 수신하였을 때 호출된다.
* \param userObject  : mxrtp_openServer 호출시 사용했던 userObject parameter.
* \param rtcp_type   : RECEIVER REPORT(201), SENDER REPORT(202), BYEBYE(203), APP(204)
* \param rtcp_packet : RTCP packet data
* \param rtcp_size   : RTCP packet length
*
* \return 성공하면 MX_RETURN_OK, 실패하면 그 이외 값
****************************************************************************************************
*/
typedef MX_RETURN (*MXRTP_CB_ONRECEIVE_RTCP)(void* userObject, unsigned rtcp_type, unsigned char* rtcp_payload, int payload_size);


__MXAPI MX_RETURN mxrtp_init(void);
__MXAPI MX_RETURN mxrtp_deinit(void);


/*!
****************************************************************************************************
*  RTP Server를 생성하고 unicast할 destination ip/port 설정한 후 MXRTP_CALLBACK_SEND_APP_RTCP 콜백함수를 등록한다.
*  *현재 live555 오픈소스 multicast지원하지 않음. 추가작업 필요함.
* fbReadTS 콜백함수를 사용하지 않고 socket fd를 이용한다.
* \param sessionID      : RTSP sessionID
* \param clientIP       : RTP receiver의 IP
* \param clientRtpPort  : RTP receiver의 RTP port
* \param clientRtcpPort : RTP receiver의 RTCP port(일반적으로 RTP port + 1)
* \param TTL            : Multicast전송시 사용할 ttl(multicasting지원될 때까지 live555 내에서 사용되지 않음)
* \param cbSendReport   : RTCP 전송할 때 필요한 정보(TP 정보)를 채우기위해 호출되는 콜백함수
* \param clientfd       : RTP 패킷을 만들때 TS패킷 정보를 읽어 오기 위한 socketpair api으로 생성한 socket fd.
* \param userObject     : Callback 함수 첫번째 파라미터로 넘겨주는 값
* \param[OUT] serverRtpPort  : RTP Server가 사용하는 RTP Port 값이 반환된다.
* \param[OUT] serverRtcpPort : RTP Server가 사용하는 RTCP Port 값이 반환된다(일반적으로 RTP Port + 1)
*
* \return 성공하면 RTP_HANDLE 값, 실패하면 NULL 값이 return된다.
****************************************************************************************************
*/
__MXAPI RTP_HANDLE mxrtp_openServer(const char* sessionID, const char* clientIP, int clientTTL, int clientRtpPort, int clientRtcpPort,
                                    MXRTP_CB_SEND_REPORT cbSendReport, int clientfd, MXRTP_CB_ONRECEIVE_RTCP cbOnReceiveRTCP,
                                    void* userObject, unsigned int* OUT serverRtpPort,unsigned int* OUT serverRtcpPort);

/*!
****************************************************************************************************
* RTP Server 닫고 사용한 resource를 해제한다.
* \param hRTP : RTP HANDLE
* \return 성공하면 MX_RETURN_OK, 실패하면 그 이외 값
****************************************************************************************************
*/
__MXAPI MX_RETURN mxrtp_closeServer(RTP_HANDLE hRTP);

/*!
****************************************************************************************************
* RTP Server를 시작한다. output parameter들은 RTSP PLAY Request에 대한 Response시에 사용된다.
*
* \param hRTP           : RTP HANDLE
* \param rtpSeq         : 시작 RTP packet의 Sequnce number
* \param rtpTime        : 시작 RTP packet의 Timestamp
* \return 성공하면 MX_RETURN_OK, 실패하면 그 이외 값
****************************************************************************************************
*/
__MXAPI MX_RETURN mxrtp_startServer(RTP_HANDLE hRTP, unsigned int* out_rtpSeq, unsigned int* out_rtpTime);

/*!
****************************************************************************************************
* byebye RTCP 을 전송하고 RTP Server를 stop한다.
* \param  hRTP : RTP HANDLE
* \return 성공하면 MX_RETURN_OK, 실패하면 그 이외 값
****************************************************************************************************
*/
__MXAPI MX_RETURN mxrtp_stopServer(RTP_HANDLE hRTP);

__MXAPI unsigned mxrtp_getSessionID(RTP_HANDLE hRTP);

// dypark 2014-05-25, Get RTP-Info refresh 
__MXAPI MX_RETURN mxrtp_getRTP_Info(RTP_HANDLE hRTP, unsigned int* out_rtpSeq, unsigned int* out_rtpTime);

#ifdef __cplusplus
}
#endif

#endif // MXRTP_SERVER_H
