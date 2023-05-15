/**
	@file	  port_itk_demux.h													\n
	@brief	  itk main header file for DEMUX porting layer						\n

	Description: ITK에서 DEMUX Porting 에 사용되는 기능을 정의한 header 파일  	\n
	Module: MW/mheg	/itk								 						\n

	Copyright (c) 2008 HUMAX Co., Ltd.											\n
	All rights reserved.														\n
*/


#ifndef _PORT_ITK_DEMUX_H_
#define _PORT_ITK_DEMUX_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include "itk.h"
#include "itk_demux.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define ITK_DEMUX_SESSION_NOTIFY 				0
#define ITK_DEMUX_SITABLE_NOTIFY 				1

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	MSG_ITK_SYSTEM_DMX_SI_ENABLE,						/* demux system message */
	MSG_ITK_SYSTEM_DMX_SI_DISABLE,
	MSG_ITK_SYSTEM_DMX_SI_RELEASE,
};



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
/**
  @remark   Initializes Demux variables.
  @remark	type : sync
  @return	NONE
*/
void PORT_ITK_DEMUX_Init(void);


/**
  @remark   register demux object to redkey library..
  @remark	type : sync
  @return	NONE
*/
void PORT_ITK_DEMUX_RegisterObject(void);


/**
  @remark   Creates a new demux session on a stream.
  @remark	type : sync
            A demux session is used to create filters for collecting private data sections
  @return	A valid handle to the session if it was created
            NULL if the session was not created
*/
itk_demux_session_t* PORT_ITK_DEMUX_NewSession(struct itk_demux_t* thiz, itk_uint16_t onid, itk_uint16_t tsid,
														itk_demux_session_callback_t callback, void* callbackContext);

/**
  @remark   Creates a new SI table filter
             An SI table object provides a handle to an MPEG SI Table sections. This can be used to access
			 the PAT, PMT, NIT and SDT and potentially others
  @remark	type : sync
            A demux session is used to create filters for collecting private data sections
  @return	A valid handle to the session if it was created
            NULL if the session was not created
*/
itk_demux_filter_t* PORT_ITK_DEMUX_NewSiTable(struct itk_demux_t* thiz, itk_uint16_t onid, itk_uint16_t tsid, itk_uint16_t pid,
										itk_uint8_t table_id, itk_uint16_t table_id_extn, itk_demux_si_callback_t callback, void *callback_context);


/**
  @remark   Creates Demux Object.
  @remark	type : sync
  @return	NONE
*/
void PORT_ITK_DEMUX_CreateObject(void);


/**
  @remark  process Filter object, Send SI Table and MPEG Section to Redkey by using Callback function.
  @remark	type : sync
  @return	ERR_OK if success,
            ERR_FAIL otherwisre.
*/
HERROR PORT_ITK_DEMUX_ProcessFilter(HUINT8 ucType, HUINT32 ulCmdType, HUINT32 ulTableId);



#if 0 //choiyj_0503_it will be erased later
/**
  @remark  get elementary stream information from PMT raw data.
  @remark	type : sync
  @return	ERR_OK if success,
            ERR_FAIL otherwisre.
*/
HERROR PORT_ITK_DEMUX_GetComponentInfo(HUINT16 usSvcId, HINT32 component, HUINT8 ucElementType, HUINT16* pucCompPId, HUINT8* pucCompType);


/**
  @remark   reset All sesstion filter-> disable filter..
  @remark	type : sync
  @return	ERR_OK if success,
            ERR_FAIL otherwisre.
*/
#endif
void PORT_ITK_DEMUX_Restore(itk_service_tune_flag_t eTuneFlag);

#ifdef CONFIG_DEBUG
HINT32 PORT_ITK_DEMUX_DEBUG_ShowRawTbInfo(void *pvArg);
HINT32 PORT_ITK_DEMUX_DEBUG_GetXXX(void *pvArg);
HINT32 PORT_ITK_DEMUX_DEBUG_ShowSiFilterList(void *pvArg);
HINT32 PORT_ITK_DEMUX_DEBUG_ShowSiMonitorList(void *pvArg);
#endif	/* CONFIG_DEBUG */


#endif	/* end of ITK_DEMUX_H_ */


