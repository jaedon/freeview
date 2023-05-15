/**************************************************************************************/
/**
 * @file oplparser.h
 *
 * Programme_ukdtt
 *
 * @author
 * @date
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLPARSER_H__
#define __OPLPARSER_H__

#include <hlib.h>
#include <silib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define	UKDTT_DES_CHECK_MEM(p)		if (p == NULL){HxLOG_Print("\n\nMemory Allocation Failed. %s %d\n\n", __FILE__, __LINE__);}
#define	UKDTT_DES_EOS					'\0'


#define	MAX_DESCRIPTOR_LIMIT	(unsigned) 255
#define	GUIDANCE_INFO_LEN		(unsigned) 256
#define	ISO639LANG_INFO_LEN		(unsigned) 4

/* EIT Privte Tag - oplprogramme 에서 파싱함 */
#define ____EIT_PRIVATE____
typedef struct
{
	HUINT8							ucGuidanceMode;
	HUINT8  						szIso639LangCode[ISO639LANG_INFO_LEN];
	HUINT8  						szGuidance[GUIDANCE_INFO_LEN];
} APKS_UkDttGuidanceInfo_t;
typedef struct
{
	HUINT8							ucPrivateDataLen;
	HUINT8							*pPrivateData;
} APKS_PrivateDataInfo_t;

typedef struct
{
	HUINT8						ucDesTag;
	HUINT8						ucGuidanceType;
	APKS_UkDttGuidanceInfo_t	*pUkDttGuidanceInfo;
	APKS_PrivateDataInfo_t *pPrivateData;
} APKS_UkDttGuidanceDes_t;

typedef struct
{
	HUINT8		ucDesTag;
	HUINT16		usTsId;
	HUINT16		usOrgNetId;
	HUINT16		usSvcId;
	HUINT8		ucLinkType;		// Link Type 이후의 모든 Private Data Info는 받은 곳에서 직접 parsing 하도록 한다.

	APKS_PrivateDataInfo_t	*pPrivateDataInfo;
} APKS_LinkageDes_t;


/** @brief TAG_CONTENT (0x54) */
typedef struct tagCONTENT_INFO
{
	HUINT8	ucContentNibble;
	HUINT8	ucUserNibble;
	struct	tagCONTENT_INFO		*pNextInfo;
} APKS_ContentInfo_t;

typedef struct
{
	HUINT8	ucDesTag;
	APKS_ContentInfo_t		*pContentInfo;
} APKS_ContentDes_t;


#define ____EIT_PRIVATE____


void  OPL_PARSER_UKDTT_FreeGuidanceDes(void *pucDesc);
HUINT8*  OPL_PARSER_UKDTT_AllocGuidanceDes(void);
APKS_UkDttGuidanceInfo_t *OPL_PARSER_UKDTT_AllocGuidanceInfo(void);
HERROR  OPL_PARSER_UKDTT_ParseGuidanceDes(HUINT8 *pucRaw, HUINT8 *pucDes);

void  OPL_PARSER_UKDTT_PrintGuidanceDes(HUINT8 *pucDes);

void  OPL_PARSER_FreeLinkageDes(void *pucDesc);
HUINT8*  OPL_PARSER_AllocLinkageDes(void);
HERROR  OPL_PARSER_ParseLinkageDes(HUINT8 *pucRaw, HUINT8 *pucDes);

void  OPL_PARSER_PrintLinkageDes(HUINT8 *pucDes);

void  OPL_PARSER_FreeContentDes(void *pucDesc);
HUINT8*  OPL_PARSER_AllocContentDes(void);
APKS_ContentInfo_t *OPL_PARSER_AllocContentInfo(void);
HERROR  OPL_PARSER_ParseContentDes(HUINT8 *pucRaw, HUINT8 *pucDes);

void  OPL_PARSER_PrintContentDes(HUINT8 *pucDes);

HxVector_t	*OPL_PARSER_ParsingDesc( HxVector_t *descriptor, HINT32 tag , HBOOL bPrivate );

#ifdef __cplusplus
};
#endif

#endif


