
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


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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


#ifndef	__INT_PL_STREAMING_MODE_SATIP__
#define	__INT_PL_STREAMING_MODE_SATIP__



/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */

#define SATIP_URL_SYNTAX_STREAM						"stream"		// Channel centre frequency expressed in MHz as fixed point type or integer value. ex) freq=754, freq=191.5

/* from 3.5.11 Query Syntax in SAT>IP Protocol Spec. Version 1.2 */
#define SATIP_SAT_QUERY_SYNTAX_FRONTEND_ID			"fe"		// 1~65535
#define SATIP_SAT_QUERY_SYNTAX_SRC_ID				"src"		// 1~255, Default value is "1"
#define SATIP_SAT_QUERY_SYNTAX_FREQ					"freq"		// Channel centre frequency expressed in MHz as fixed point type or integer value. ex) freq=11361.75, freq=11720
#define SATIP_SAT_QUERY_SYNTAX_POLAR				"pol"		// "h" horizontal linear, "v" vertical linear, "l" circular left, "r" circular right
#define SATIP_SAT_QUERY_SYNTAX_ROLL_OFF				"ro"		// S2 only, "0.35", "0.25", "0.20"
#define SATIP_SAT_QUERY_SYNTAX_MODULATION_SYSTEM	"msys"		// "dvbs", "dvbs2"
#define SATIP_SAT_QUERY_SYNTAX_MODULATION_TYPE		"mtype"		// S2 only, "qpsk", "8psk"
#define SATIP_SAT_QUERY_SYNTAX_PILOT_TONES			"plts"		// S2 only, "on", "off"
#define SATIP_SAT_QUERY_SYNTAX_SYMBOL_RATE			"sr"		// Value in kSymb/s
#define SATIP_SAT_QUERY_SYNTAX_FEC					"fec"		// "12", "23", "34", "56", "78", "89", "35", "45", "910"
#define SATIP_SAT_QUERY_SYNTAX_PIDS					"pids"		// (Num. values betw. 0 and 8191) for spts streams, "all" for mpts streams, "none" for no demux output.
#define SATIP_SAT_QUERY_SYNTAX_ADDPIDS				"addpids"
#define SATIP_SAT_QUERY_SYNTAX_DELPIDS				"delpids"

/* from Appendix C: Support for DVB-T/T2 (optional) in SAT>IP Protocol Spec. Version 1.2 */
#define SATIP_TER_QUERY_SYNTAX_FREQ					"freq"		// Channel centre frequency expressed in MHz as fixed point type or integer value. ex) freq=754, freq=191.5
#define SATIP_TER_QUERY_SYNTAX_BANDWIDTH			"bw"		// "5¡±, ¡±6¡±, ¡°7¡±, ¡°8", ¡°10¡±, ¡°1.712¡±
#define SATIP_TER_QUERY_SYNTAX_MODULATION_SYSTEM	"msys"		// "dvbt", "dvbt2"
#define SATIP_TER_QUERY_SYNTAX_TRANSMISSION_MODE	"tmode"		// ¡°2k¡±, ¡°4k¡±, ¡°8k¡±, ¡°1k¡±, ¡°16k¡±, ¡°32k¡±
#define SATIP_TER_QUERY_SYNTAX_MODULATION_TYPE		"mtype"		// "qpsk", ¡°16qam¡±, "64qam", ¡°256qam¡±
#define SATIP_TER_QUERY_SYNTAX_GUIRD_INTERVAL		"gi"		// "14", "18", ¡°116¡±, ¡°132¡±,¡±1128¡±, ¡°19128¡±, ¡°19256¡±
#define SATIP_TER_QUERY_SYNTAX_FEC					"fec"		// "12", "35", "23", "34", "45", ¡°56¡±, ¡°78¡±
#define SATIP_TER_QUERY_SYNTAX_PLP					"plp"		// T2 only - Numerical value between 0 and 255
#define SATIP_TER_QUERY_SYNTAX_T2_SYSTEM_ID			"t2id"		// T2 only - Numerical value between 0 and 65535.
#define SATIP_TER_QUERY_SYNTAX_SISO_MISO			"sm"		// T2 only - ¡°1¡± means MISO, ¡°0¡± means SISO
#define SATIP_TER_QUERY_SYNTAX_PIDS					"pids"		// (Num. values betw. 0 and 8191) for spts streams, "all" for mpts streams, "none" for no demux output.
#define SATIP_TER_QUERY_SYNTAX_ADDPIDS				"addpids"
#define SATIP_TER_QUERY_SYNTAX_DELPIDS				"delpids"

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */
extern	HOM_Result_e 			PL_Streaming_ModeSatIp_Init(void);
extern	void 					PL_Streaming_ModeSatIp_DeInit(void);


extern	PLStreaming_ModeInst_t *PL_Streaming_ModeSatIp_SetupInstance(PLStreaming_ModeInst_t *pstInst);
extern	void					PL_Streaming_ModeSatIp_DeleteInstance(PLStreaming_ModeInst_t *pstInst);
extern	HOM_Result_e 			PL_Streaming_ModeSatIp_ResConnect (PLStreaming_ModeInst_t *pstInst);
extern	HOM_Result_e 			PL_Streaming_ModeSatIp_ResConflict (PLStreaming_ModeInst_t *pstInst, HINT32 eNotifyType, HINT32 nRequestPid, HCHAR *fromProcessName);
extern	HOM_Result_e			PL_Streaming_ModeSatIp_GetM3U8(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HUINT8 **ppucData);
extern	HOM_Result_e			PL_Streaming_ModeSatIp_ReleaseM3U8(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT8 *pucData);
extern	HOM_Result_e			PL_Streaming_ModeSatIp_GetTS(PLStreaming_ModeInst_t *pstInst,  const HCHAR *pRequestPath, HUINT32 *pulDataSize, HUINT8 **ppucData);
extern	HOM_Result_e			PL_Streaming_ModeSatIp_ReleaseTS(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT8 *pucData);
extern	HOM_Result_e 			PL_Streaming_ModeSatIp_ExecCmd(PLStreaming_ModeInst_t *pstInst,  const HCHAR *pRequestPath);
extern	HOM_Result_e 			PL_Streaming_ModeSatIp_GetInfo(PLStreaming_ModeInst_t *pstInst, PLStreaming_InfoType_e eInfoType, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HCHAR **ppucData);
extern	HOM_Result_e 			PL_Streaming_ModeSatIp_ReleaseInfo(PLStreaming_ModeInst_t *pstInst, PLStreaming_InfoType_e eInfoType, const HCHAR *pRequestPath, HCHAR *pucData);
extern	HOM_Result_e 			PL_Streaming_ModeSatIp_GetUIDByStartInfo (const PLStreaming_StartInfo_t *pstStartInfo, HUID *ret_uUID);
extern	HOM_Result_e			PL_Streaming_ModeSatIp_StartInfoToSetupInfo(const HCHAR *pszPrefix, const PLStreaming_StartInfo_t *pstStartInfo, PLStreaming_SetupInfo_t *ret_pstSetupInfo);
extern 	HOM_Result_e			PL_Streaming_ModeSatIp_GetProfileItemHandle(PLStreaming_ModeInst_t *pstInst, Handle_t *ret_phProfileItem);
extern	HOM_Result_e 			PL_Streaming_ModeSatIp_GetPIDs(PLStreaming_ModeInst_t *pstInst, HINT32 *ret_pnPIDsCount, HUINT16 *ret_arrusPIDs);
extern	HOM_Result_e			PL_Streaming_ModeSatIp_Task(plStreamingInst_t *pstInst);


/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */

extern 	void					PL_Streaming_ModeSatIp_Dump(PLStreaming_ModeInst_t *pstInst);

#endif /* __INT_PL_STREAMING_MODE_SATIP__ */

