/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_download.h
	@brief

	Description: main main procedure ���� ������� �з��� ����̴�. \n
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
	/* FSAT ���� META FILE �� ���� DSMCC �� �����ؾ� ���� �˸���.
	- Parameters Usage :
		handle : Action handle.
		p1 : Seconds to wait
		p2 : TBD
		p3 : TBD */
	EVT_DOWN_FSAT_DETERMINE_METAFILE_DOWNLOAD,

	/* FSAT ���� META FILE �� ���� DSMCC �� �����ؾ� ���� �˸���.
	- Parameters Usage :
		handle : Action handle.
		p1 : DSMCC PID
		p2 : TBD
		p3 : TBD */
	EVT_DOWN_FSAT_START_METAFILE_DSMCC,

	/* FSAT ���� Receiver Data FILE �� ���� DSMCC �� Home TP���� �����ؾ� ���� �˸���.
	- Parameters Usage :
		handle : Action handle.
		p1 : DSMCC PID
		p2 : TBD
		p3 : TBD */
	EVT_DOWN_FSAT_START_RCVDATA_DSMCC_HOME,

	/* FSAT ���� ���� Contents XML ���� 1���� �ٿ�ε� ���� �˸�
	- Parameters Usage :
		handle : Action handle.
		p1 : Update List Pointer
		p2 : TBD
		p3 : TBD */
	EVT_DOWN_FSAT_COMPLETE_ONE_METAFILE_IP,

	/* FSAT ���� ���� Contents XML ���� �ٿ�ε� ���μ����� �Ϸ���� �˸�
	- Parameters Usage :
		handle : Action handle.
		p1 : Update List Pointer
		p2 : TBD
		p3 : TBD */
	EVT_DOWN_FSAT_COMPLETE_ALL_METAFILE_IP,

	/* FSAT ���� ���� IMG ���� �ٿ�ε� ���μ����� �Ϸ���� �˸�
	- Parameters Usage :
		handle : Action handle.
		p1 : TBD
		p2 : TBD
		p3 : TBD */
	EVT_DOWN_FSAT_COMPLETE_IMGFILE_IP,

	/* FSAT ���� ������ Download Task from IP Task �� �����
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
