/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_download.h
	@brief

	Description: main main procedure 관련 내용들을 분류한 모듈이다. \n
	Module:
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XSVC_DOWNLOAD_H__
#define	__XSVC_DOWNLOAD_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <db_param.h>

/*******************************************************************/
// MW
/*******************************************************************/
#include <svc_download.h>

/*******************************************************************/
// DOWNLOAD
/*******************************************************************/
//#include <_svc_download.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	XEVT_DOWN_START		= eSEVT_DOWN_END,

// CONFIG_OP_FREESAT_GEN2	>>>
	/* FSAT 에서 META FILE 을 위한 DSMCC 를 시작해야 함을 알린다.
	- Parameters Usage :
		handle : Action handle.
		p1 : Seconds to wait
		p2 : TBD
		p3 : TBD */
	EVT_DOWN_FSAT_DETERMINE_METAFILE_DOWNLOAD,

	/* FSAT 에서 META FILE 을 위한 DSMCC 를 시작해야 함을 알린다.
	- Parameters Usage :
		handle : Action handle.
		p1 : DSMCC PID
		p2 : TBD
		p3 : TBD */
	EVT_DOWN_FSAT_START_METAFILE_DSMCC,

	/* FSAT 에서 Receiver Data FILE 을 위한 DSMCC 를 Home TP에서 시작해야 함을 알린다.
	- Parameters Usage :
		handle : Action handle.
		p1 : DSMCC PID
		p2 : TBD
		p3 : TBD */
	EVT_DOWN_FSAT_START_RCVDATA_DSMCC_HOME,

	/* FSAT 에서 각종 Contents XML 파일 1개가 다운로드 됨을 알림
	- Parameters Usage :
		handle : Action handle.
		p1 : Update List Pointer
		p2 : TBD
		p3 : TBD */
	EVT_DOWN_FSAT_COMPLETE_ONE_METAFILE_IP,

	/* FSAT 에서 각종 Contents XML 파일 다운로드 프로세스가 완료됨을 알림
	- Parameters Usage :
		handle : Action handle.
		p1 : Update List Pointer
		p2 : TBD
		p3 : TBD */
	EVT_DOWN_FSAT_COMPLETE_ALL_METAFILE_IP,

	/* FSAT 에서 각종 IMG 파일 다운로드 프로세스가 완료됨을 알림
	- Parameters Usage :
		handle : Action handle.
		p1 : TBD
		p2 : TBD
		p3 : TBD */
	EVT_DOWN_FSAT_COMPLETE_IMGFILE_IP,

	/* FSAT 에서 시작한 Download Task from IP Task 가 종료됨
	- Parameters Usage :
		handle : Action handle.
		p1 : Task Id
		p2 : TBD
		p3 : TBD */
	EVT_DOWN_FSAT_TASK_FINISHED_IP,
// CONFIG_OP_FREESAT_GEN2	<<<

};

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#endif	/*	!__XSVC_DOWNLOAD_H__	*/
