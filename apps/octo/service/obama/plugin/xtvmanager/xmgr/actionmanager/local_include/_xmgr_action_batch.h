/**
	@file     _xmgr_action_batch.h
	@brief    file_name & simple comment.

	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	2013/02/15       Start

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n

Default Installation :

*/

#ifndef __XMGR_ACTION_BATCH_INT_H__
#define __XMGR_ACTION_BATCH_INT_H__

// CONFIG_OP_MIDDLE_EAST
extern HUINT32 					 xmgr_action_BatchCount_MeSat(DxBatchType_e eBatchType);
extern MgrAction_BatchInitInfo 	*xmgr_action_BatchGetProcInfo_MeSat(DxBatchType_e eBatchType);
extern HBOOL					 xmgr_action_BatchIsSerial_MeSat(DxBatchType_e eBatchType);

// CONFIG_OP_SES
extern HUINT32 					 xmgr_action_BatchCount_Ses(DxBatchType_e eBatchType);
extern MgrAction_BatchInitInfo 	*xmgr_action_BatchGetProcInfo_Ses(DxBatchType_e eBatchType);
extern HBOOL					 xmgr_action_BatchIsSerial_Ses(DxBatchType_e eBatchType);

// CONFIG_OP_ERTELECOM
extern HUINT32					 xmgr_action_BatchCount_Ertelecom(DxBatchType_e eBatchType);
extern MgrAction_BatchInitInfo	*xmgr_action_BatchGetProcInfo_Ertelecom(DxBatchType_e eBatchType);
extern HBOOL					 xmgr_action_BatchIsSerial_Ertelecom(DxBatchType_e eBatchType);

// CONFIG_OP_AUS_DTT
extern HUINT32					 xmgr_action_BatchCount_AusDtt(DxBatchType_e eBatchType);
extern MgrAction_BatchInitInfo	*xmgr_action_BatchGetProcInfo_AusDtt(DxBatchType_e eBatchType);
extern HBOOL					 xmgr_action_BatchIsSerial_AusDtt(DxBatchType_e eBatchType);

// CONFIG_OP_UK_DTT
extern HUINT32					 xmgr_action_BatchCount_UkDtt(DxBatchType_e eBatchType);
extern MgrAction_BatchInitInfo	*xmgr_action_BatchGetProcInfo_UkDtt(DxBatchType_e eBatchType);
extern HBOOL					 xmgr_action_BatchIsSerial_UkDtt(DxBatchType_e eBatchType);

extern HUINT32					 xmgr_action_BatchCount_Base(DxBatchType_e eBatchType);
extern MgrAction_BatchInitInfo	*xmgr_action_BatchGetProcInfo_Base(DxBatchType_e eBatchType);
extern HBOOL					 xmgr_action_BatchIsSerial_Base(DxBatchType_e eBatchType);

#endif		//__XMGR_ACTION_BATCH_INT_H__

