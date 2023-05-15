/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_module_portingprotocol.h
	@brief

	Description:  									\n
	Module: Portinig Layer / Porting Protocal			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/

#ifndef	__XMGR_MODULE_PORTINGPROTOCOL_H__
#define	__XMGR_MODULE_PORTINGPROTOCOL_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define XMGR_PORTPROTOCOL_Return(arg3,retval)															\
					do {																				\
						XMGR_PORTPRTC_Result_t		*macro_portresult = (XMGR_PORTPRTC_Result_t *)arg3;	\
						if (NULL != macro_portresult)													\
						{																				\
							macro_portresult->hResult = retval;											\
							macro_portresult->szResponded = (HCHAR *)__FUNCTION__;						\
						}																				\
						return MESSAGE_BREAK;															\
					} while (0)

#define XMGR_PORTPROTOCOL_Request(macroReqMsg,macroInput,macroOutput,macroResult,macroAbortNoRes)		\
					do {																				\
						BUS_SendMessage (NULL, macroReqMsg, HANDLE_NULL, (HINT32)macroInput, (HINT32)macroOutput, (HINT32)macroResult);					\
						if (NULL == (macroResult)->szResponded)											\
						{																				\
							if (TRUE == macroAbortNoRes)												\
							{																			\
								while (1)																\
								{																		\
									HxLOG_Error ("\n\n\n[%s] MSG (%s) not Responded in (proc_init_xxx)\n", __FUNCTION__, #macroReqMsg);						\
									HxLOG_Error ("[%s] You had better to insert the response to XMGR_INIT_XXX and proc_init_xxx...\n\n\n", __FUNCTION__);	\
									abort();															\
								}																		\
							}																			\
							else																		\
							{																			\
								HxLOG_Error ("[%s] MSG (%s) not Responded in (proc_init_xxx)\n", __FUNCTION__, #macroReqMsg);								\
								HxLOG_Error ("[%s] You had better to insert the response to XMGR_INIT_XXX and proc_init_xxx...\n", __FUNCTION__);			\
								return (macroResult)->hResult;												\
							}																			\
						}																				\
						else																			\
						{																				\
							HxLOG_Debug ("[%s] MSG (%s) responded by [%s]\n", __FUNCTION__, #macroReqMsg, (macroResult)->szResponded);							\
						}																				\
					} while (0)


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HERROR			 hResult;
	HCHAR			*szResponded;
} XMGR_PORTPRTC_Result_t;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#endif	/*	!__XMGR_MODULE_PORTINGPROTOCOL_H__	*/

