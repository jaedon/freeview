/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   abr_mpdparser.c  $
 * Version:         $Revision:   $
 * Original Author: Young Ki Kim(ykkim3@humaxdigital.com)
 * Current Author:  $Author:  $
 * Date:            $Date:
 * File Description:	     MPD(ABR Manifest) Parser
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2011 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#include "xmlmemory.h"
#include "parserInternals.h"
#include "parser.h"
#include "tree.h"

#include "vkernel.h"
#include "abr_mpdparser.h"
#include <stdlib.h>
#include "di_iptuner_config.h"
#include "di_uart.h"

/*----------------------------------------------------------------------------------------------
 *	define
 *---------------------------------------------------------------------------------------------*/
//#define ABR_MPDPARSER_DEBUG

#ifdef IPTUNER_ALL_DEBUG	/* DEBUG Define */

#ifdef ABR_MPDPARSER_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* The END of DEBUG DEFINE */

/*----------------------------------------------------------------------------------------------
 *	MACRO
 *---------------------------------------------------------------------------------------------*/

#if ABR_MPDPARSER_CONFIG_NOT_USE_VK
#define VK_MEM_Alloc malloc
#define VK_MEM_Memset memset
#define VK_strncpy strncpy
#define VK_MEM_Free free
#define VK_StrDup strdup
#endif

/*----------------------------------------------------------------------------------------------
 *	TAG and ATTR
 *---------------------------------------------------------------------------------------------*/

/* I only define real tag/attr name because the XML syntax distinguishes between upper and lower letters.
   shall i define lower/upper name? */
#ifdef ABR_MPDPARSER_CONFIG_UPPERCASE
#define MPD_TAG_MPD_NAME			"MPD"
#define MPD_TAG_PROGINFO_NAME		"PROGRAMINFORMATION"
#define MPD_TAG_PERIOD_NAME			"PERIOD"
#define MPD_TAG_SEGINFODEFAULT_NAME		"SEGMENTINFODEFAULT"
#define MPD_TAG_REPRESENTATION_NAME		"REPRESENTATION"
#define MPD_TAG_CONTENTPROTECTION_NAME		"CONTENTPROTECTION"
#define MPD_TAG_TRICKMODE_NAME		"TRICKMODE"
#define MPD_TAG_SEGMENTINFO_NAME		"SEGMENTINFO"
#define MPD_TAG_INITSEGURL_NAME		"INITIALISATIONSEGMENTURL"
#define MPD_TAG_URLTEMPLATE_NAME		"URLTEMPLATE"
#define MPD_TAG_SEGMENTURL_NAME		"URL"

#define MPD_ATTR_TYPE_NAME "TYPE"
#define MPD_ATTR_AVAILSTARTTIME_NAME "AVAILABILITYSTARTTIME"
#define MPD_ATTR_AVAILENDTIME_NAME "AVAILABILITYENDTIME"
#define MPD_ATTR_MEDIAPRESENTDURATION_NAME "MEDIAPRESENTATIONDURATION"
#define MPD_ATTR_MINBUFFERTIME_NAME "MINBUFFERTIME"
#define MPD_ATTR_TIMESHIFTBUFFDEPTH_NAME "TIMESHIFTBUFFERDEPTH"
#define MPD_ATTR_BASEURL_NAME "BASEURL"
#define MPD_ATTR_PERIODSTARTTIME_NAME "START"
#define MPD_ATTR_ID_NAME "ID"
#define MPD_ATTR_SEGALIGNFLAG_NAME "SEGMENTALIGNMENTFLAG"
#define MPD_ATTR_BITSTREAMSWITCHFLAG_NAME "BITSTREAMSWITCHINGFLAG"
#define MPD_ATTR_DURATION_NAME "DURATION"
#define MPD_ATTR_STARTINDEX_NAME "STARTINDEX"
#define MPD_ATTR_SRCURLTEMPLATEPERIOD_NAME "SOURCEURLTEMPLATEPERIOD"
#define MPD_ATTR_BANDWIDTH_NAME "BANDWIDTH"
#define MPD_ATTR_WIDTH_NAME "WIDTH"
#define MPD_ATTR_HEIGHT_NAME "HEIGHT"
#define MPD_ATTR_LANG_NAME "LANG"
#define MPD_ATTR_MIMETYPE_NAME "MIMETYPE"
#define MPD_ATTR_GROUP_NAME "GROUP"
#define MPD_ATTR_STARTWITHRAP_NAME "STARTWITHRAP"
#define MPD_ATTR_QUALITYRANK_NAME "QUALITYRANKING"
#define MPD_ATTR_SOURCEURL_NAME "SOURCEURL"
#define MPD_ATTR_RANGE_NAME "RANGE"
#define MPD_ATTR_ENDINDEX_NAME "ENDINDEX"

#define MPD_VAL_TYPE_ONDEMAND "ONDEMAND"
#define MPD_VAL_TYPE_LIVE "LIVE"
#define MPD_VAL_TRUE "TRUE"
#define MPD_VAL_FALSE "FALSE"

#else /* !ABR_MPDPARSER_CONFIG_UPPERCASE */

#define MPD_TAG_MPD_NAME			"MPD"
#define MPD_TAG_PROGINFO_NAME		"ProgramInformation"
#define MPD_TAG_PERIOD_NAME			"Period"
#define MPD_TAG_SEGINFODEFAULT_NAME		"SegmentInfoDefault"
#define MPD_TAG_REPRESENTATION_NAME		"Representation"
#define MPD_TAG_CONTENTPROTECTION_NAME		"ContentProtection"
#define MPD_TAG_TRICKMODE_NAME		"TrickMode"
#define MPD_TAG_SEGMENTINFO_NAME		"SegmentInfo"
#define MPD_TAG_INITSEGURL_NAME		"InitialisationSegmentURL"
#define MPD_TAG_URLTEMPLATE_NAME		"UrlTemplate"
#define MPD_TAG_SEGMENTURL_NAME		"Url"

#define MPD_ATTR_TYPE_NAME "type"
#define MPD_ATTR_AVAILSTARTTIME_NAME "availabilityStartTime"
#define MPD_ATTR_AVAILENDTIME_NAME "availabilityEndTime"
#define MPD_ATTR_MEDIAPRESENTDURATION_NAME "mediaPresentationDuration"
#define MPD_ATTR_MINBUFFERTIME_NAME "minBufferTime"
#define MPD_ATTR_TIMESHIFTBUFFDEPTH_NAME "timeShiftBufferDepth"
#define MPD_ATTR_BASEURL_NAME "baseURL"
#define MPD_ATTR_PERIODSTARTTIME_NAME "start"
#define MPD_ATTR_ID_NAME "id"
#define MPD_ATTR_SEGALIGNFLAG_NAME "segmentAlignmentFlag"
#define MPD_ATTR_BITSTREAMSWITCHFLAG_NAME "bitstreamSwitchingFlag"
#define MPD_ATTR_DURATION_NAME "duration"
#define MPD_ATTR_STARTINDEX_NAME "startIndex"
#define MPD_ATTR_SRCURLTEMPLATEPERIOD_NAME "sourceUrlTemplatePeriod"
#define MPD_ATTR_BANDWIDTH_NAME "bandwidth"
#define MPD_ATTR_WIDTH_NAME "width"
#define MPD_ATTR_HEIGHT_NAME "height"
#define MPD_ATTR_LANG_NAME "lang"
#define MPD_ATTR_MIMETYPE_NAME "mimeType"
#define MPD_ATTR_GROUP_NAME "group"
#define MPD_ATTR_STARTWITHRAP_NAME "startWithRAP"
#define MPD_ATTR_QUALITYRANK_NAME "qualityRanking"
#define MPD_ATTR_SOURCEURL_NAME "sourceURL"
#define MPD_ATTR_RANGE_NAME "range"
#define MPD_ATTR_ENDINDEX_NAME "endIndex"

#define MPD_VAL_TYPE_ONDEMAND "OnDemand"
#define MPD_VAL_TYPE_LIVE "Live"
#define MPD_VAL_TRUE "true"
#define MPD_VAL_FALSE "false"
#endif /* ABR_MPDPARSER_CONFIG_UPPERCASE */

/*----------------------------------------------------------------------------------------------
 *	static functions
 *---------------------------------------------------------------------------------------------*/
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
static inline xmlDoc *__xmldoc(void *document) { return (xmlDoc *)document; }

static inline xmlNode *__xmlnode(void *node) { return (xmlNode *)node; }

static inline mpdData_t *__mpdData(void *mpdData) { return (mpdData_t*)mpdData; }
#else
static inline void *__xmldoc(void *document) { return (void *)document; }

static inline void *__xmlnode(void *node) { return (void *)node; }

static inline void *__mpdData(void *mpdData) { return (void*)mpdData; }
#endif


#ifdef ABR_MPDPARSER_CONFIG_UPPERCASE
static int
mpdUpperStrcmp(const xmlChar *str1, const xmlChar *str2) {
	char upc1, upc2;
    register int tmp;

    if ((str1 == NULL) && (str2 == NULL)) return(0);
    if (str1 == NULL) return(-1);
    if (str2 == NULL) return(1);
    do {
    	upc1 = VK_toupper(*str1++);
    	upc2 = VK_toupper(*str2++);
  		tmp = upc1 - upc2;
	if (tmp != 0) return(tmp);
    } while ((*str1 != 0) && (*str2 != 0));
    return (*str1 - *str2);
}
#endif

static long long _atoi64(const char *nptr)
{
  char *s = (char *)nptr;
  long long acc = 0;
  int neg = 0;

  while(isspace((int)*s))
    s++;
  if (*s == '-')
    {
      neg = 1;
      s++;
    }
  else if (*s == '+')
    s++;

  while (VK_isdigit((int)*s))
    {
      acc = 10 * acc + ((int)*s - '0');
      s++;
    }

  if (neg)
    acc *= -1;
  return acc;
}

static unsigned long long _atoui64(const char *nptr)
{
  char *s = (char *)nptr;
  unsigned long long acc = 0;
  int neg = 0;

  while(isspace((int)*s))
    s++;
  if (*s == '-')
    {
      neg = 1;
      s++;
    }
  else if (*s == '+')
    s++;

  while (VK_isdigit((int)*s))
    {
      acc = 10 * acc + ((int)*s - '0');
      s++;
    }

  if (neg)
    acc *= -1;
  return acc;
}

static unsigned int mpdXsDurationToMs(char *pStr, int nSize)
{
	char buff[5] = {0};
	unsigned int res = 0;
	int nPos = 0;
	int nStart = 0;
	int nEnd = 0;
	char *p = pStr;
	while(p != NULL && nSize > nPos)
	{
		VK_memset(buff,0x0,sizeof(buff));
		switch(*p)
		{
			case 'P' :
				nStart = nPos+1;
				break;
			case 'T' :
				nStart = nPos+1;
				break;
			case 'Y' :
				/* not supported */
				break;
			case 'M' :
				/* not supported for "Month"*/
				nEnd = nPos;
				VK_strncpy(buff,pStr+nStart,nEnd-nStart);
//				PrintDebug ("mpdXsDurationToMs M : %s\n",buff);
				nStart = nPos+1;
				res += VK_atoi(buff) * 60 * 1000;
				break;
			case 'D' :
				/* not supported */
				break;
			case 'H' :
				nEnd = nPos;
				VK_strncpy(buff,pStr+nStart,nEnd-nStart);
//				PrintDebug ("mpdXsDurationToMs H : %s\n",buff);
				nStart = nPos+1;
				res += VK_atoi(buff) * 60 * 60 * 1000;
				break;
			case 'S' :
				nEnd = nPos;
				VK_strncpy(buff,pStr+nStart,nEnd-nStart);
//				PrintDebug ("mpdXsDurationToMs S : %s\n",buff);
				nStart = nPos+1;
				res += atof(buff) * 1000;

				break;
		}
		p++;
		nPos++;
	}
//	PrintDebug ("mpdXsDurationToMs Result %u\n",res);
	return res;
}

/**
 * @brief 	releases mpd data
 * @param 	mpdData : the structure of parsed mpd
 * @return 	mpd Error Code
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see 	util_mpd_close
 */
static mpdErr_e *local_mpd_free(mpdData_t *mpdData)
{
	int i = 0;
	int j = 0;
	if(mpdData->pBaseUrl != NULL)
	{
		VK_MEM_Free(mpdData->pBaseUrl);
	}
	for(i = 0; i < mpdData->nTotalPeriod ;i++)
	{
		if((mpdData->periodSet+i)->segInfoDef.pBaseUrl != NULL)
		{
			VK_MEM_Free((mpdData->periodSet+i)->segInfoDef.pBaseUrl);
		}
		if((mpdData->periodSet+i)->segInfoDef.pSrcUrlTemplate != NULL)
		{
			VK_MEM_Free((mpdData->periodSet+i)->segInfoDef.pSrcUrlTemplate);
		}
		for(j = 0 ; j < (mpdData->periodSet+i)->nTotalRepresent ; j++)
		{
			if((mpdData->periodSet+i)->representSet[j].segInfo.pBaseUrl != NULL)
			{
				VK_MEM_Free((mpdData->periodSet+i)->representSet[j].segInfo.pBaseUrl);
			}
			if((mpdData->periodSet+i)->representSet[j].segInfo.initSegUrl.pSrcUrl != NULL)
			{
				VK_MEM_Free((mpdData->periodSet+i)->representSet[j].segInfo.initSegUrl.pSrcUrl);
			}
			if((mpdData->periodSet+i)->representSet[j].segInfo.urlTemplate.pSrcUrl != NULL)
			{
				VK_MEM_Free((mpdData->periodSet+i)->representSet[j].segInfo.urlTemplate.pSrcUrl);
			}
		}
		if((mpdData->periodSet+i)->representSet != NULL)
		{
			VK_MEM_Free((mpdData->periodSet+i)->representSet);
		}
	}
	if(mpdData->periodSet != NULL)
	{
		VK_MEM_Free(mpdData->periodSet);
	}
	if(mpdData != NULL)
	{
		VK_MEM_Free(mpdData);
	}
	return MPD_ERR_OK;
}

/**
 * @brief 	finding a specific node
 * @param 	startNode : starting node for finding
 * @param 	szName : node name
 * @return 	xmlNode
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark      this function travels all nodes to find the one matched to @szName
 * @see
 **/
/* This function is operated recursively.
 * XML has tree-structure and this function travels all nodes to find the one matched to @szName
 */
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
static xmlNode *local_find_node(xmlNode *startNode, const xmlChar *szName)
{
	xmlNode *retNode = NULL;

	if (startNode == NULL) { return NULL; }
	// PrintDebug ("local_find_node(%s) : %s\n",szName,startNode->name); // for DEBUG, must remove it

	#ifdef ABR_MPDPARSER_CONFIG_UPPERCASE
	if (!mpdUpperStrcmp(startNode->name, szName))
	{
		return startNode;
	}
	#else
	if (!xmlStrcmp(startNode->name, szName))
	{
		return startNode;
	}
	#endif
	else
	{
		retNode = local_find_node(startNode->children, szName);
	}

	if (retNode == NULL)
	{
		retNode = local_find_node(startNode->next, szName);
	}

	return retNode;
}
#else

#if	0
static void *local_find_node(xmlNode *startNode, const char *szName)
{
	return NULL;
}
#endif

#endif

/**
 * @brief 	finding a specific node from the starting node to all the next node
 * @param 	startNode : starting node for finding
 * @param 	szName : node name
 * @return 	xmlNode
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark      this function travels all nodes in same level to find the one matched to @szName
 * @see 	local_find_node
 **/
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
static xmlNode *local_find_next_node(xmlNode *startNode, const xmlChar *szName)
{
//	int i = 0;
	xmlNode *retNode = NULL;
#if 0 /* recursive */
	if (startNode == NULL) { return NULL; }
	PrintDebug ("local_find_next_node(%s) : %s\n",szName,startNode->name); // for DEBUG, must remove it
	#ifdef ABR_MPDPARSER_CONFIG_UPPERCASE
	if (!sstrUpperStrcmp(startNode->name, szName))
	{
		return startNode;
	}
	#else
	if (!xmlStrcmp(startNode->name, szName))
	{
		return startNode;
	}
	#endif
	else
	{
		retNode = local_find_node(startNode->next, szName);
	}

	return retNode;
#else
	if (startNode == NULL) { return NULL; }
	while(startNode != NULL)
	{
//		PrintDebug ("local_find_next_node(%s:%d) : %s\n",szName,i++,startNode->name); // for DEBUG, must remove it
		if (!xmlStrcmp(startNode->name, szName))
		{
//			PrintDebug ("Correct (%s) : %s\n",szName,startNode->name);
			return startNode;
		}
		else
		{
//			PrintDebug ("startNode->next(%s) : %s\n",szName,startNode->name);
			startNode = startNode->next;
		}
	}
	return NULL;
#endif

}
#else

#if	0
static void *local_find_node(xmlNode *startNode, const char *szName)
{
	return NULL;
}
#endif

#endif

/**
 * @brief 	finding a specific node from the starting node to all the previous node
 * @param 	startNode : starting node for finding
 * @param 	szName : node name
 * @return 	xmlNode
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark      this function travels all nodes in same level to find the one matched to @szName
 * @see 	local_find_next_node
 **/
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
static xmlNode *local_find_prev_node(xmlNode *startNode, const xmlChar *szName)
{
	xmlNode *retNode = NULL;
#if 0 /* recursive */
		if (startNode == NULL) { return NULL; }
		PrintDebug ("local_find_next_node(%s) : %s\n",szName,startNode->name); // for DEBUG, must remove it
#ifdef ABR_MPDPARSER_CONFIG_UPPERCASE
		if (!sstrUpperStrcmp(startNode->name, szName))
		{
			return startNode;
		}
#else
		if (!xmlStrcmp(startNode->name, szName))
		{
			return startNode;
		}
#endif
		else
		{
			retNode = local_find_node(startNode->prev, szName);
		}

		return retNode;
#else
		if (startNode == NULL) { return NULL; }
		while(startNode != NULL)
		{
	//		PrintDebug ("local_find_next_node(%s:%d) : %s\n",szName,i++,startNode->name); // for DEBUG, must remove it
			if (!xmlStrcmp(startNode->name, szName))
			{
	//			PrintDebug ("Correct (%s) : %s\n",szName,startNode->name);
				return startNode;
			}
			else
			{
	//			PrintDebug ("startNode->next(%s) : %s\n",szName,startNode->name);
				startNode = startNode->prev;
			}
		}
		return NULL;
#endif
		return retNode;
}
#else

#if	0
static void *local_find_node(xmlNode *startNode, const char *szName)
{
	return NULL;
}
#endif

#endif

/*
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
static xmlNode *local_find_child_node(xmlNode *startNode, const char *szName)
{
	xmlNode *retNode = NULL;

	if (startNode == NULL) { return NULL; }

	if (!xmlStrcmp(startNode->name, szName))
	{
		return startNode;
	}
	else
	{
		retNode = local_find_child_node(startNode->childs, szName);
	}

	return retNode;
}
#else

#if	0
static void *local_find_child_node(xmlNode *startNode, const char *szName)
{
	return NULL;
}
#endif

#endif
*/

/**
 * @brief 	get the "ROOT" of MPD from parser handle
 * @param 	parser : parser handle
 * @return 	element handle of the "ROOT"
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see 	local_find_next_node
 **/
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
static mpdElement_Root_t *local_mpdparser_get_mpdRoot(mpdParser_t *parser)
{
	xmlNode *pNode = NULL;

	if (__xmlnode(parser->root)->children != NULL )
	{
		pNode = local_find_node(__xmlnode(parser->root), (xmlChar*)MPD_TAG_MPD_NAME);
		if ( pNode != NULL )
		{
			return (mpdElement_Root_t *)pNode;
		}
	}
	return NULL;
}
#else
static mpdElement_Root_t *local_mpdparser_get_mpdRoot(mpdParser_t *parser)
{
	return NULL;
}
#endif

/**
 * @brief 	finding a specific element from the child node of starting element to all the next element
 * @param 	element : starting element for finding
 * @param 	szName : element name
 * @return 	element handle
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see 	local_find_node
 **/
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
static mpdElement_t *local_mpdparser_get_element(mpdElement_t *element, const xmlChar *szName)
{
	xmlNode *pNode = NULL;

	if (__xmlnode(element)->children != NULL )
	{
		pNode = local_find_node(__xmlnode(element)->children, szName);
		if ( pNode != NULL )
		{
			return (mpdElement_t *)pNode;
		}
	}

	if (__xmlnode(element)->next == NULL )
	{
		return NULL;
	}

	return local_find_node(__xmlnode(element)->next, szName);
}
#else
static mpdElement_t *local_mpdparser_get_element(mpdElement_t *element, const char *szName)
{
	return NULL;
}
#endif

/**
 * @brief 	finding a specific element from the node of starting element to all the next element
 * @param 	element : starting element for finding
 * @param 	szName : element name
 * @return 	element handle
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see 	local_find_node
 **/
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
static mpdElement_t *local_mpdparser_get_next_element(mpdElement_t *element, const xmlChar *szName)
{
	// xmlNode *pNode = NULL;

	if (__xmlnode(element)->next == NULL )
	{
		return NULL;
	}

	return local_find_next_node(__xmlnode(element)->next, szName);
}
#else
static mpdElement_t *local_mpdparser_get_next_element(mpdElement_t *element, const char *szName)
{
	return NULL;
}
#endif

/**
 * @brief 	finding a specific element from the node of starting element to all the previous element
 * @param 	element : starting element for finding
 * @param 	szName : element name
 * @return 	element handle
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see 	local_find_node
 **/
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
static mpdElement_t *local_mpdparser_get_prev_element(mpdElement_t *element, const xmlChar *szName)
{
	// xmlNode *pNode = NULL;

	if (__xmlnode(element)->prev == NULL )
	{
		return NULL;
	}

	return local_find_prev_node(__xmlnode(element)->prev, szName);
}
#else
static mpdElement_t *local_mpdparser_get_prev_element(mpdElement_t *element, const char *szName)
{
	return NULL;
}
#endif

/**
 * @brief 	get a child element of starting element
 * @param 	element : starting element for finding
 * @param 	szName : element name
 * @return 	element handle
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see 	local_find_node
 **/
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
static mpdElement_t *local_mpdparser_get_child_element(mpdElement_t *element, const xmlChar *szName)
{
	xmlNode *pNode = NULL;

	// PrintDebug ("local_mpdparser_get_child_element(%s)\n",szName);
	if (__xmlnode(element)->children != NULL )
	{
		pNode = local_find_next_node(__xmlnode(element)->children, szName);
		if ( pNode != NULL )
		{
			return (mpdElement_t *)pNode;
		}
		else
			return NULL;
	}
	else
		return NULL;
}
#else
static mpdElement_t *local_mpdparser_get_child_element(mpdElement_t *element, const char *szName)
{
	return NULL;
}
#endif

/*
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
static mpdSegment_t *local_mpdparser_get_segment(mpdSegment_t *segment, const char *szName)
{
	xmlNode *pNode = NULL;

	if (__xmlnode(segment)->childs != NULL )
	{
		pNode = local_find_node(__xmlnode(segment)->childs, szName);
		if ( pNode != NULL )
		{
			return (asxentry_t *)pNode;
		}
	}

	if (__xmlnode(segment)->next == NULL )
	{
		return NULL;
	}

	return local_find_node(__xmlnode(segment)->next, szName);
}
#else
static mpdSegment_t *local_mpdparser_get_segment(mpdSegment_t *segment, const char *szName)
{
	return NULL;
}
#endif
*/

/**
 * @brief 	get a attribute value
 * @param 	entry : entry point for finding attribute
 * @param 	szAttrName : attribute name
 * @param 	szBuf : the memory buffer for storing attribute value
 * @param	nBufsize : size of the memory buffer
 * @return 	mpd error code
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
static mpdErr_e local_mpdparser_get_attr_value(void *entry, const xmlChar *szAttrName,
									char *szBuf, int nBufsize)
{
	//unsigned int	nAttrLen = 0;
	int	nAttrLen = 0;
	xmlChar 		*xmlAttr = NULL;

	xmlAttr = xmlGetProp(__xmlnode(entry), szAttrName);
	if ( xmlAttr == NULL )
	{
		// PrintDebug ("MPD_ERR_FAIL_ATTR_NOTFOUND : %s\n",szAttrName);
		return MPD_ERR_FAIL_ATTR_NOTFOUND;
	}

	nAttrLen = xmlStrlen(xmlAttr);
	if ( nBufsize <= nAttrLen )
	{
		xmlFree(xmlAttr);
		return MPD_ERR_FAIL_BUFFER_SIZE;
	}
	VK_strncpy(szBuf, (char *)xmlAttr, nAttrLen);
	szBuf[nAttrLen] = '\0';
	xmlFree(xmlAttr);
	return MPD_ERR_OK;
}
#else
static mpdErr_e local_mpdparser_get_attr_value(void *entry, const char *szAttrName,
									char *szBuf, int nBufsize)
{
	return MPD_ERR_FAIL;
}
#endif

/**
 * @brief 	get the number of elements
 * @param 	element : starting element for finding
 * @param 	szName : element name
 * @return 	the number of elements
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

int local_mpdparser_get_element_countByfirstElement(mpdElement_t *firstElement, const xmlChar *szName)
{
	int nCnt = 0;
	mpdElement_t *element = firstElement;

	while(element != NULL)


	{
		nCnt++;
		element = local_mpdparser_get_next_element(element, szName);
	}
	// PrintDebug ("local_mpdparser_get_element_countByfirstElement(%s) : %d\n",szName,nCnt);
	return nCnt;
}

/**
 * @brief 	get the 'n'th element
 * @param 	element : starting element for finding
 * @param 	szName : element name
 * @param 	n : 'n' th
 * @return 	element handle
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
mpdElement_t* local_mpdparser_get_n_element(mpdElement_t *firstElement, const xmlChar *szName, int n)
{
	int nCnt = 0;
	mpdElement_t *element = firstElement;
	if(firstElement == NULL)
	{
		return NULL;
	}
	while(nCnt < n && element != NULL)
	{
		element = local_mpdparser_get_next_element(element, szName);
		nCnt++;
	}
	return element;
}

/**
 * @brief 	parse the "ROOT" of MPD
 * @param 	mpdData : structure of mpd data for storing parsed data
 * @param 	mpdRoot : root handle
 * @return 	mpd error code
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
mpdErr_e local_mpdparser_parse_mpdRoot(mpdData_t *mpdData, mpdElement_Root_t *mpdRoot)
{
	mpdErr_e errCode = MPD_ERR_OK;
	char szBuf[ABR_MPDPARSER_CONST_ATTR_BUFSIZE]={0}; // check buffer size

	/* Parse MPD root tag "MPD" */

	/* Parse "type" */
	VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
  	errCode = local_mpdparser_get_attr_value(mpdRoot,(xmlChar*)MPD_ATTR_TYPE_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
	{
		mpdData->etype = MPD_TYPE_ONDEMAND;
	}
	else if(errCode == MPD_ERR_OK)
	{
		if(NULL != VK_strstr(szBuf, MPD_VAL_TYPE_LIVE))
		{
			mpdData->etype = MPD_TYPE_LIVE;
		}
		else
		{
			mpdData->etype = MPD_TYPE_ONDEMAND;
		}
	}
	else
	{
		return errCode;
	}

	/* attr : mediaPresentationDuration (must write code(or method) to convert "xs:duration" to "milliseconds") */
	errCode = local_mpdparser_get_attr_value(mpdRoot,(xmlChar*)MPD_ATTR_MEDIAPRESENTDURATION_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
	{
		mpdData->ulDuration = 0;
	}
	else if(errCode == MPD_ERR_OK)
	{
		/* attr : duration (must write code(or method) to convert "xs:duration" to "milliseconds") */
		mpdData->ulDuration = mpdXsDurationToMs(szBuf,VK_strlen(szBuf));
	}
	else
	{
		return errCode;
	}

	/* attr : ulMinBufferTime (must write code(or method) to convert "xs:duration" to "milliseconds") */
	errCode = local_mpdparser_get_attr_value(mpdRoot,(xmlChar*)MPD_ATTR_MINBUFFERTIME_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
	{
		mpdData->ulMinBufferTime= 0;
	}
	else if(errCode == MPD_ERR_OK)
	{
		/* attr : ulMinBufferTime (must write code(or method) to convert "xs:duration" to "milliseconds") */
		mpdData->ulMinBufferTime = mpdXsDurationToMs(szBuf,VK_strlen(szBuf));
	}
	else
	{
		return errCode;
	}
	/* Parse "baseURL" */
	VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_mpdparser_get_attr_value(mpdRoot,(xmlChar*)MPD_ATTR_BASEURL_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
	{
		mpdData->pBaseUrl = NULL;
	}
	else if(errCode == MPD_ERR_OK)
	{
		mpdData->pBaseUrl = VK_StrDup(szBuf);
	}
	else
	{
		return errCode;
	}

	return MPD_ERR_OK;
}

/**
 * @brief 	parse the "ProgramInformation" of MPD
 * @param 	mpdData : structure of mpd data for storing parsed data
 * @param 	mpdRoot : root handle
 * @return 	mpd error code
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
mpdErr_e local_mpdparser_parse_progInfo(mpdData_t *mpdData, mpdElement_Root_t *mpdRoot)
{
	mpdErr_e errCode = MPD_ERR_OK;
	char szBuf[ABR_MPDPARSER_CONST_ATTR_BUFSIZE]={0}; // check buffer size
	// mpdElement_t *element  = NULL;
	/* Parse "ProgramInfo" */

	VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
	mpdData->progInfo = local_mpdparser_get_child_element(mpdRoot,(xmlChar*)MPD_TAG_PROGINFO_NAME);

	/* do not parse attributes in ProgramInfo but if any attributes needed, write code(or method) for parsing */

	return errCode;
}

/**
 * @brief 	parse the "SegmentInformationDefault" of MPD
 * @param 	mpdPeriod : structure of period for storing parsed data
 * @param 	periodElement : period handle
 * @return 	mpdErr_e
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
mpdErr_e local_mpdparser_parse_SegInfoDef(mpdPeriod_t *mpdPeriod, mpdElement_Period_t *periodElement)
{
	mpdErr_e errCode = MPD_ERR_OK;
	mpdElement_t *element  = NULL;
	char szBuf[ABR_MPDPARSER_CONST_ATTR_BUFSIZE]={0}; // check buffer size

	/* Parse "SegmentInfoDefault" */
	VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
	element = local_mpdparser_get_child_element(periodElement,(xmlChar*)MPD_TAG_SEGINFODEFAULT_NAME);
	if(element != NULL)
	{
		mpdPeriod->bExistSegInfoDef = TRUE;
		/* Parse "duration" */
		VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_mpdparser_get_attr_value(element,(xmlChar*)MPD_ATTR_DURATION_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
		{
			mpdPeriod->segInfoDef.ulDuration = 0;
		}
		else if(errCode == MPD_ERR_OK)
		{
			/* attr : duration (must write code(or method) to convert "xs:duration" to "milliseconds") */
			mpdPeriod->segInfoDef.ulDuration = mpdXsDurationToMs(szBuf,VK_strlen(szBuf));
		}
		else
		{
			return errCode;
		}

		/* Parse "startIndex" */
		VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_mpdparser_get_attr_value(element,(xmlChar*)MPD_ATTR_STARTINDEX_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
		{
			mpdPeriod->segInfoDef.startIndex = 1;
		}
		else if(errCode == MPD_ERR_OK)
		{
			mpdPeriod->segInfoDef.startIndex = VK_atoi(szBuf);
		}
		else
		{
			return errCode;
		}

		/* Parse "baseURL" */
		VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_mpdparser_get_attr_value(element,(xmlChar*)MPD_ATTR_BASEURL_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
		{
			mpdPeriod->segInfoDef.pBaseUrl = NULL;
		}
		else if(errCode == MPD_ERR_OK)
		{
			mpdPeriod->segInfoDef.pBaseUrl = VK_StrDup(szBuf);
		}
		else
		{
			return errCode;
		}

		/* Parse "sourceUrlTemplatePeriod" */
		VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_mpdparser_get_attr_value(element,(xmlChar*)MPD_ATTR_SRCURLTEMPLATEPERIOD_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
		{
			mpdPeriod->segInfoDef.pSrcUrlTemplate = NULL;
		}
		else if(errCode == MPD_ERR_OK)
		{
			mpdPeriod->segInfoDef.pSrcUrlTemplate = VK_StrDup(szBuf);
		}
		else
		{
			return errCode;
		}
	}
	else
	{
		mpdPeriod->bExistSegInfoDef = FALSE;
		mpdPeriod->segInfoDef.startIndex = 1;
		mpdPeriod->segInfoDef.ulDuration = 0;
		mpdPeriod->segInfoDef.pBaseUrl = NULL;
		mpdPeriod->segInfoDef.pSrcUrlTemplate = NULL;
	}
	return MPD_ERR_OK;
}

/**
 * @brief 	parse the "Period" of MPD
 * @param 	mpdPeriod : structure of mpd for storing parsed data
 * @param 	periodElement : root handle
 * @return 	mpdErr_e
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
mpdErr_e local_mpdparser_parse_period(mpdData_t *mpdData, mpdElement_Root_t *mpdRoot)
{
	int i = 0;
	mpdErr_e errCode = MPD_ERR_OK;
	mpdElement_Period_t *period = NULL;
	// mpdElement_t *element  = NULL;
	char szBuf[ABR_MPDPARSER_CONST_ATTR_BUFSIZE]={0}; // check buffer size
	mpdData->nTotalPeriod = 0;
	mpdData->firstPeriod = local_mpdparser_get_child_element(mpdRoot,(xmlChar*)MPD_TAG_PERIOD_NAME);

	if(mpdData->firstPeriod == NULL)
	{

		return MPD_ERR_FAIL_PERIOD_NOTFOUND;
	}
	period = mpdData->firstPeriod;
	mpdData->nTotalPeriod = local_mpdparser_get_element_countByfirstElement(period,(xmlChar*)MPD_TAG_PERIOD_NAME);
	mpdData->periodSet = (mpdPeriod_t*) VK_MEM_Alloc (sizeof(mpdPeriod_t) * mpdData->nTotalPeriod);
	if(mpdData->periodSet == NULL)
	{
		return 	MPD_ERR_FAIL_MEMORY_ALLOC;
	}
	for(i = 0 ; i < mpdData->nTotalPeriod && period != NULL ; i++)
	{

		/* Parse "start" */
		VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_mpdparser_get_attr_value(period,(xmlChar*)MPD_ATTR_PERIODSTARTTIME_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
		{
			(mpdData->periodSet+i)->ulStart = 0;
		}
		else if(errCode == MPD_ERR_OK)
		{
			/* attr : start (must write code(or method) to convert "xs:duration" to "milliseconds") */
			(mpdData->periodSet+i)->ulStart = mpdXsDurationToMs(szBuf,VK_strlen(szBuf));
		}
		else
		{
			return errCode;
		}

		/* Parse "id" */
		VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_mpdparser_get_attr_value(period,(xmlChar*)MPD_ATTR_ID_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
		{
			(mpdData->periodSet+i)->nId = 0;
		}
		else if(errCode == MPD_ERR_OK)
		{
			(mpdData->periodSet+i)->nId = VK_atoi(szBuf);
		}
		else
		{
			return errCode;
		}

		/* Parse "segmentAlignmentFlag"*/
		VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_mpdparser_get_attr_value(period,(xmlChar*)MPD_ATTR_SEGALIGNFLAG_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
		{
			(mpdData->periodSet+i)->bSegAlignFlag = FALSE;
		}
		else if(errCode == MPD_ERR_OK)
		{
			if(NULL != VK_strstr(szBuf, MPD_VAL_TRUE))
			{
				(mpdData->periodSet+i)->bSegAlignFlag= TRUE;
			}
			else
			{
				(mpdData->periodSet+i)->bSegAlignFlag= FALSE;
			}
		}
		else
		{
			return errCode;
		}
		/* Parse "bitstreamSwitchingFlag" */
		VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_mpdparser_get_attr_value(period,(xmlChar*)MPD_ATTR_BITSTREAMSWITCHFLAG_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
		{
			(mpdData->periodSet+i)->bBitstreamSwitchFlag= FALSE;
		}
		else if(errCode == MPD_ERR_OK)
		{
			if(NULL != VK_strstr(szBuf, MPD_VAL_TRUE))
			{
				(mpdData->periodSet+i)->bBitstreamSwitchFlag= TRUE;
			}
			else
			{
				(mpdData->periodSet+i)->bBitstreamSwitchFlag= FALSE;
			}
		}
		else
		{
			return errCode;
		}
		errCode = local_mpdparser_parse_SegInfoDef((mpdData->periodSet+i), period);
		if(errCode != MPD_ERR_OK)
		{
			return errCode;
		}

		period = local_mpdparser_get_next_element(period, (xmlChar*)MPD_TAG_PERIOD_NAME);
	}
	return MPD_ERR_OK;
}

/**
 * @brief 	parse the "Presentation" of MPD
 * @param 	mpdPeriod : structure of period for storing parsed data
 * @param 	periodElement : period handle
 * @return 	mpdErr_e
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
mpdErr_e local_mpdparser_parse_represent(mpdPeriod_t *mpdPeriod, mpdElement_Period_t *periodElement)
{
	int i = 0;
	mpdErr_e errCode = MPD_ERR_OK;
	mpdElement_Represent_t *represent = NULL;
	mpdElement_t *segInfo = NULL;
	mpdElement_t *element = NULL;
	mpdElement_UrlTemplete_t *urlTemplate = NULL;
	char szBuf[ABR_MPDPARSER_CONST_ATTR_BUFSIZE]={0}; // check buffer size

	mpdPeriod->nTotalRepresent = 0;
	mpdPeriod->firstRepresent = local_mpdparser_get_child_element(periodElement,(xmlChar*)MPD_TAG_REPRESENTATION_NAME);

	if(mpdPeriod->firstRepresent == NULL)
	{

		return MPD_ERR_FAIL_REPRESENT_NOTFOUND;
	}

	represent = mpdPeriod->firstRepresent ;
	mpdPeriod->nTotalRepresent = local_mpdparser_get_element_countByfirstElement(represent,(xmlChar*)MPD_TAG_REPRESENTATION_NAME);
	mpdPeriod->representSet = (mpdRepresent_t*)VK_MEM_Alloc(sizeof(mpdRepresent_t) * mpdPeriod->nTotalRepresent);
	if(mpdPeriod->representSet == NULL)
	{
		return MPD_ERR_FAIL_MEMORY_ALLOC;
	}
	for(i = 0 ; i < mpdPeriod->nTotalRepresent && represent != NULL ; i++)
	{
		/* do not parse other attributes in Representation but if other attributes needed, write code(or method) for parsing */

		/* Parse "id" */
		VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_mpdparser_get_attr_value(represent,(xmlChar*)MPD_ATTR_ID_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
		{
			(mpdPeriod->representSet+i)->nId = 0;
		}
		else if(errCode == MPD_ERR_OK)
		{
			(mpdPeriod->representSet+i)->nId = VK_atoi(szBuf);
		}
		else
		{
			return errCode;
		}

		/* Parse "bandwidth" */
		VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_mpdparser_get_attr_value(represent,(xmlChar*)MPD_ATTR_BANDWIDTH_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == MPD_ERR_OK)
		{
			// (mpdPeriod->representSet+i)->ulBandwidth = strtoul(szBuf,NULL,10);
			(mpdPeriod->representSet+i)->ullBandwidth = _atoi64(szBuf);
		}
		else
		{
			return errCode;
		}

		/* Parse "qualityRanking" */
		VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_mpdparser_get_attr_value(represent,(xmlChar*)MPD_ATTR_QUALITYRANK_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
		{
			(mpdPeriod->representSet+i)->nQualityRanking= 0;
		}
		else if(errCode == MPD_ERR_OK)
		{
			(mpdPeriod->representSet+i)->nQualityRanking= VK_atoi(szBuf);
		}
		else
		{
			return errCode;
		}

		/* Parse "ContentProtection"*/
		(mpdPeriod->representSet+i)->firstProtection = local_mpdparser_get_child_element(represent,(xmlChar*)MPD_TAG_CONTENTPROTECTION_NAME);
		(mpdPeriod->representSet+i)->nTotalProtection = local_mpdparser_get_element_countByfirstElement((mpdPeriod->representSet+i)->firstProtection,(xmlChar*)MPD_TAG_CONTENTPROTECTION_NAME);
		/* do not parse any attributes in ContentProtection but if attributes needed, write code(or method) for parsing */

		/* Parse "TrickMode" */
		(mpdPeriod->representSet+i)->trickMode = local_mpdparser_get_child_element(represent,(xmlChar*)MPD_TAG_TRICKMODE_NAME);
		/* do not parse any attributes in TrickMode but if attributes needed, write code(or method) for parsing */

		/* Parse "SegmentInfo" */
		segInfo = local_mpdparser_get_child_element(represent,(xmlChar*)MPD_TAG_SEGMENTINFO_NAME);

		/* Parse "duration" */
		VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_mpdparser_get_attr_value(segInfo,(xmlChar*)MPD_ATTR_DURATION_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
		{
			(mpdPeriod->representSet+i)->segInfo.ulDuration = mpdPeriod->segInfoDef.ulDuration;
		}
		else if(errCode == MPD_ERR_OK)
		{
			/* attr : duration (must write code(or method) to convert "xs:duration" to "milliseconds") */
			(mpdPeriod->representSet+i)->segInfo.ulDuration = mpdXsDurationToMs(szBuf,VK_strlen(szBuf));
		}
		else
		{
			return errCode;
		}

		/* Parse "startIndex" */
		VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_mpdparser_get_attr_value(segInfo,(xmlChar*)MPD_ATTR_STARTINDEX_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
		{
			(mpdPeriod->representSet+i)->segInfo.startIndex = mpdPeriod->segInfoDef.startIndex;
		}
		else if(errCode == MPD_ERR_OK)
		{
			(mpdPeriod->representSet+i)->segInfo.startIndex = VK_atoi(szBuf);
		}
		else
		{
			return errCode;
		}

		/* Parse "baseURL" */
		VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_mpdparser_get_attr_value(segInfo,(xmlChar*)MPD_ATTR_BASEURL_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
		{
			(mpdPeriod->representSet+i)->segInfo.pBaseUrl = mpdPeriod->segInfoDef.pBaseUrl;
		}
		else if(errCode == MPD_ERR_OK)
		{
			(mpdPeriod->representSet+i)->segInfo.pBaseUrl = VK_StrDup(szBuf);
		}
		else
		{
			return errCode;
		}

		/* Parse "InitialisationSegmentURL" */
		// PrintDebug ("+Parse InitialisationSegmentURL\n");
		element = local_mpdparser_get_child_element(segInfo,(xmlChar*)MPD_TAG_INITSEGURL_NAME);
		if( element == NULL )
		{
			(mpdPeriod->representSet+i)->segInfo.bExistInitSegUrl = FALSE;
		}
		else
		{
			(mpdPeriod->representSet+i)->segInfo.bExistInitSegUrl = TRUE;
			/* Parse "sourceURL" */
			VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
			errCode = local_mpdparser_get_attr_value(element,(xmlChar*)MPD_ATTR_SOURCEURL_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
			if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
			{
				(mpdPeriod->representSet+i)->segInfo.initSegUrl.pSrcUrl = NULL;
			}
			else if(errCode == MPD_ERR_OK)
			{
				(mpdPeriod->representSet+i)->segInfo.initSegUrl.pSrcUrl = VK_StrDup(szBuf);
			}
			else
			{
				return errCode;
			}

			/* Parse "range" */
			VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
			errCode = local_mpdparser_get_attr_value(element,(xmlChar*)MPD_ATTR_RANGE_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
			if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
			{
				(mpdPeriod->representSet+i)->segInfo.initSegUrl.ulRange= 0;
			}
			else if(errCode == MPD_ERR_OK)
			{
				(mpdPeriod->representSet+i)->segInfo.initSegUrl.ulRange = strtoul(szBuf,NULL,10);
			}
			else
			{
				return errCode;
			}
		}

		/* Parse "UrlTemplate" */
		urlTemplate = local_mpdparser_get_child_element(represent,(xmlChar*)MPD_TAG_URLTEMPLATE_NAME);
		if( urlTemplate == NULL )
		{
			(mpdPeriod->representSet+i)->segInfo.segType = MPD_SEGMENT_TYPE_URL;
			(mpdPeriod->representSet+i)->segInfo.firstSegment = local_mpdparser_get_child_element(segInfo,(xmlChar*)MPD_TAG_SEGMENTURL_NAME);
			(mpdPeriod->representSet+i)->segInfo.nTotalSegment = local_mpdparser_get_element_countByfirstElement((mpdPeriod->representSet+i)->segInfo.firstSegment,(xmlChar*)MPD_TAG_SEGMENTURL_NAME);
			(mpdPeriod->representSet+i)->segInfo.urlTemplate.pSrcUrl = NULL;
			(mpdPeriod->representSet+i)->segInfo.urlTemplate.endIndex = 0;
		}
		else
		{
			(mpdPeriod->representSet+i)->segInfo.segType = MPD_SEGMENT_TYPE_TEMPLATE;

			/* Parse "sourceURL" */
			VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
			errCode = local_mpdparser_get_attr_value(urlTemplate,(xmlChar*)MPD_ATTR_SOURCEURL_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
			if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
			{
				(mpdPeriod->representSet+i)->segInfo.urlTemplate.pSrcUrl = NULL;
			}
			else if(errCode == MPD_ERR_OK)
			{
				(mpdPeriod->representSet+i)->segInfo.urlTemplate.pSrcUrl = VK_StrDup(szBuf);
			}
			else
			{
				return errCode;
			}

			/* Parse "endIndex" */
			VK_MEM_Memset(szBuf, 0x00, ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
			errCode = local_mpdparser_get_attr_value(urlTemplate,(xmlChar*)MPD_ATTR_ENDINDEX_NAME,szBuf,ABR_MPDPARSER_CONST_ATTR_BUFSIZE);
			if(errCode == MPD_ERR_FAIL_ATTR_NOTFOUND)
			{
				(mpdPeriod->representSet+i)->segInfo.urlTemplate.endIndex = 0;
			}
			else if(errCode == MPD_ERR_OK)
			{
				(mpdPeriod->representSet+i)->segInfo.urlTemplate.endIndex = VK_atoi(szBuf);
			}
			else
			{
				return errCode;
			}

			(mpdPeriod->representSet+i)->segInfo.nTotalSegment = ((mpdPeriod->representSet+i)->segInfo.urlTemplate.endIndex - (mpdPeriod->representSet+i)->segInfo.startIndex) + 1;
		}
		/* next representation */
		represent = local_mpdparser_get_next_element(represent, (xmlChar*)MPD_TAG_REPRESENTATION_NAME);
	}
	return MPD_ERR_OK;
}

/**
 * @brief 	open the manifest file to make the parser handle
 * @param 	szPath : file path
 * @return 	parser handle
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
mpdParser_t* ABR_MPDParser_FileOpen(const char *szPath)
{
	mpdData_t *pMpdData = NULL;
	mpdParser_t *parser = (mpdParser_t *)VK_MEM_Alloc(sizeof(mpdParser_t));
	if ( parser == NULL )
	{
		return NULL;
	}
	pMpdData = (mpdData_t *)VK_MEM_Alloc(sizeof(mpdData_t));
	VK_MEM_Memset(pMpdData,0x0,sizeof(mpdData_t));

	if (pMpdData == NULL)
	{
		ABR_MPDParser_Close(parser);
		return NULL;
	}
	parser->mpdData = pMpdData;
	parser->document = (void *)xmlParseFile(szPath);
	if ( parser->document == NULL )
	{
		ABR_MPDParser_Close(parser);
		return NULL;
	}
	parser->root = (void *)xmlDocGetRootElement(__xmldoc(parser->document));
	if (parser->root == NULL)
	{
		ABR_MPDParser_Close(parser);
		return NULL;
	}

	return parser;
}
#else
mpdParser_t* ABR_MPDParser_FileOpen(const char *szPath)
{
	return NULL;
}
#endif

/**
 * @brief 	make the parser handle from memory
 * @param 	pBuffer : stored manifest in the memory buffer
 * @param	nBufsize : size of the memory buffer
 * @return 	mpdParser_t*
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
mpdParser_t* ABR_MPDParser_MemOpen(const char *pBuffer,int nBufsize)
{
	mpdData_t *pMpdData = NULL;
	mpdParser_t *parser = (mpdParser_t *)VK_MEM_Alloc(sizeof(mpdParser_t));
	if ( parser == NULL )
	{
		return NULL;
	}
	pMpdData = (mpdData_t *)VK_MEM_Alloc(sizeof(mpdData_t));
	VK_MEM_Memset(pMpdData,0x0,sizeof(mpdData_t));
	if (pMpdData == NULL)
	{
		ABR_MPDParser_Close(parser);
		return NULL;
	}
	VK_MEM_Memset(pMpdData,0x00,sizeof(mpdData_t));

	parser->mpdData = (void*)pMpdData;
	parser->document = (void *)xmlParseMemory(pBuffer,nBufsize);
	if ( parser->document == NULL )
	{
		ABR_MPDParser_Close(parser);
		return NULL;
	}
	parser->root = (void *)xmlDocGetRootElement(__xmldoc(parser->document));
	if (parser->root == NULL)
	{
		ABR_MPDParser_Close(parser);
		return NULL;
	}
	return parser;
}
#else
mpdParser_t* ABR_MPDParser_MemOpen(const char *pBuffer,int nBufsize)
{
	return NULL;
}
#endif


/**
 * @brief 	close the parser handle
 * @param 	parser : parser handle
 * @return 	void
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
#ifdef ABR_MPDPARSER_CONFIG_3RD_XML
void ABR_MPDParser_Close(mpdParser_t *parser)
{
	if ( parser )
	{
		local_mpd_free(__mpdData(parser->mpdData));
		if ( parser->document )
		{
			xmlFreeDoc(__xmldoc(parser->document));
			parser->document = NULL;
		}

		parser->root = NULL;
		VK_MEM_Free(parser);
		parser = NULL;
	}
}
#else
void ABR_MPDParser_Close(mpdParser_t *parser)
{
	return;
}
#endif

/**
 * @brief 	parse the MPD manifest data
 * @param 	parser : parser handle
 * @return 	mpdErr_e
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
mpdErr_e ABR_MPDParser_Parse(mpdParser_t *parser)
{
	int i = 0;
	mpdErr_e errCode = MPD_ERR_OK;
 	// char szBuf[ABR_MPDPARSER_CONST_ATTR_BUFSIZE]; // check buffer size
	mpdData_t *mpdData = NULL;
	mpdElement_Root_t *mpdRoot = NULL;
	mpdElement_Period_t *period = NULL;

	mpdData = __mpdData(parser->mpdData);
	mpdRoot = local_mpdparser_get_mpdRoot(parser);
	if(mpdRoot == NULL)
	{
		return MPD_ERR_FAIL_MPD_NOTFOUND;
	}
	errCode = local_mpdparser_parse_mpdRoot(mpdData, mpdRoot);
	if(MPD_ERR_OK != errCode)
	{
		return errCode;
	}
	errCode = local_mpdparser_parse_progInfo(mpdData, mpdRoot);
	if(MPD_ERR_OK != errCode)
	{
		return errCode;
	}
	errCode = local_mpdparser_parse_period(mpdData, mpdRoot);
	if(MPD_ERR_OK != errCode)
	{
		return errCode;
	}
	if(mpdData->firstPeriod == NULL)
	{
		return MPD_ERR_FAIL_PERIOD_NOTFOUND;
	}
	else
	{
		period = mpdData->firstPeriod;
	}

	for(i = 0 ; i < mpdData->nTotalPeriod && period != NULL ; i++)
	{
		errCode = local_mpdparser_parse_represent((mpdData->periodSet+i), period);
		if(MPD_ERR_OK != errCode)
		{
			return errCode;
		}
		period = local_mpdparser_get_next_element(period, (xmlChar*)MPD_TAG_PERIOD_NAME);
	}
	return MPD_ERR_OK;
}

/**
 * @brief 	get the next segment from the current segment
 * @param 	curSegment : current segment handle
 * @return 	next segment handle
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see 	local_mpdparser_get_next_element
 **/
mpdElement_Segment_t* ABR_MPDParser_NextSegment(mpdElement_Segment_t* curSegment)
{
	return local_mpdparser_get_next_element(curSegment, (xmlChar*)MPD_TAG_SEGMENTURL_NAME);
}

/**
 * @brief 	get the previous segment from the current segment
 * @param 	curSegment : current segment handle
 * @return 	previous segment handle
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see 	local_mpdparser_get_prev_element
 **/
mpdElement_Segment_t* ABR_MPDParser_PrevSegment(mpdElement_Segment_t* curSegment)
{
	return local_mpdparser_get_prev_element(curSegment, (xmlChar*)MPD_TAG_SEGMENTURL_NAME);
}

/**
 * @brief 	get the "n"th segment
 * @param 	segment : segment handle
 * @param 	n : "n" th
 * @return 	mpdElement_Segment_t*
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see 	local_mpdparser_get_n_element
 **/
mpdElement_Segment_t* ABR_MPDParser_n_Segment(mpdElement_Segment_t* segment, int n)
{
	return local_mpdparser_get_n_element(segment, (xmlChar*)MPD_TAG_SEGMENTURL_NAME, n);
}

/**
 * @brief 	get the url of segment from the current segment
 * @param 	curSegment : current segment handle
 * @param	szBuf : buffer for storing url of the segment
 * @param	nBufsize : size of the buffer
 * @return 	mpd error code
 * @date 	2011/02/19
 * @author 	ykkim3@humaxdigital.com
 * @remark
 * @see 	local_mpdparser_get_attr_value
 **/
mpdErr_e ABR_MPDParser_GetSegmentUrl(mpdElement_Segment_t* curSegment, char *szBuf, int nBufsize)
{
	mpdErr_e errCode = MPD_ERR_OK;
	if(curSegment == NULL)
	{
		return MPD_ERR_FAIL_SEGMENT_NOTFOUND;
	}
	errCode = local_mpdparser_get_attr_value(curSegment,(xmlChar*)MPD_ATTR_SOURCEURL_NAME,szBuf,nBufsize);
	return errCode;
}

#if 0
void ABR_MPDParser_print(mpdParser_t *parser)
{
	int i = 0;
	int j = 0;
//	mpdErr_e errCode = MPD_ERR_OK;
// 	char szBuf[ABR_MPDPARSER_CONST_ATTR_BUFSIZE]; // check buffer size
	mpdData_t *mpdData = NULL;
//	mpdElement_Root_t *mpdRoot = NULL;
//	mpdElement_Period_t *period = NULL;

	mpdData = __mpdData(parser->mpdData);
	PrintDebug ("-----------------------------------------------\n");
	PrintDebug ("| MPD\n");
	PrintDebug ("-----------------------------------------------\n");
	PrintDebug ("Type : %d\n",mpdData->etype);
	PrintDebug ("Duration : %u\n",mpdData->ulDuration);
	PrintDebug ("Base URL : %s\n",mpdData->pBaseUrl);
	PrintDebug ("Total Period : %d\n",mpdData->nTotalPeriod);
	for(i = 0; i < mpdData->nTotalPeriod ;i++)
	{
		PrintDebug ("-----------------------------------------------\n");
		PrintDebug ("| Period[%d]\n",i);
		PrintDebug ("-----------------------------------------------\n");
		PrintDebug ("Id : %d\n",(mpdData->periodSet+i)->nId);
		PrintDebug ("Start : %u\n",(mpdData->periodSet+i)->ulStart);
		PrintDebug ("SegmentAlignment : %d\n",(mpdData->periodSet+i)->bSegAlignFlag);
		PrintDebug ("BitstreamSwitching : %d\n",(mpdData->periodSet+i)->bBitstreamSwitchFlag);
		PrintDebug ("ExistSegmentInfoDefault : %d\n",(mpdData->periodSet+i)->bExistSegInfoDef);
		PrintDebug ("Total Representation : %d\n",(mpdData->periodSet+i)->nTotalRepresent);
		PrintDebug ("-----------------------------------------------\n");
		PrintDebug ("| SegmentInfoDefault\n");
		PrintDebug ("-----------------------------------------------\n");
		PrintDebug ("startIndex : %d\n",(mpdData->periodSet+i)->segInfoDef.startIndex);
		PrintDebug ("duration : %u\n",(mpdData->periodSet+i)->segInfoDef.ulDuration);
		PrintDebug ("Base URL : %s\n",(mpdData->periodSet+i)->segInfoDef.pBaseUrl);
		PrintDebug ("URL Template : %s\n",(mpdData->periodSet+i)->segInfoDef.pSrcUrlTemplate);
		for(j = 0 ; j < (mpdData->periodSet+i)->nTotalRepresent ; j++)
		{
			PrintDebug ("-----------------------------------------------\n");
			PrintDebug ("| Represent[%d]\n",j);
			PrintDebug ("-----------------------------------------------\n");
			PrintDebug ("Id : %d\n",(mpdData->periodSet+i)->representSet[j].nId);
			PrintDebug ("Bandwidth : %lld\n",(mpdData->periodSet+i)->representSet[j].ullBandwidth);
			PrintDebug ("Quality Ranking : %d\n",(mpdData->periodSet+i)->representSet[j].nQualityRanking);
			PrintDebug ("Segment Type : %d\n",(mpdData->periodSet+i)->representSet[j].segInfo.segType);
			PrintDebug ("Segment Start Index : %d\n",(mpdData->periodSet+i)->representSet[j].segInfo.startIndex);
			PrintDebug ("Segment Duration : %u\n",(mpdData->periodSet+i)->representSet[j].segInfo.ulDuration);
			PrintDebug ("Segment BaseUrl : %s\n",(mpdData->periodSet+i)->representSet[j].segInfo.pBaseUrl);
			PrintDebug ("Segment bExistInitSegmentURL : %d\n",(mpdData->periodSet+i)->representSet[j].segInfo.bExistInitSegUrl);
			PrintDebug ("Segment InitSegmentURL : %s\n",(mpdData->periodSet+i)->representSet[j].segInfo.initSegUrl.pSrcUrl);
			PrintDebug ("Segment InitSegmentURL Range : %u\n",(mpdData->periodSet+i)->representSet[j].segInfo.initSegUrl.ulRange);
			PrintDebug ("Segment urlTemplate URL : %s\n",(mpdData->periodSet+i)->representSet[j].segInfo.urlTemplate.pSrcUrl);
			PrintDebug ("Segment urlTemplate endIndex : %d\n",(mpdData->periodSet+i)->representSet[j].segInfo.urlTemplate.endIndex);
			PrintDebug ("Segment nTotalSegment : %d\n",(mpdData->periodSet+i)->representSet[j].segInfo.nTotalSegment);
		}
	}
}
#endif

/*int local_mpdparser_get_period_count(mpdElement_Period_t *firstPeriod)
{
	int nCnt = 0;
	mpdElement_Period_t *period = firstPeriod;

	while(period != NULL)


	{
		nCnt++;
		period = local_mpdparser_get_next_element(period, MPD_TAG_PERIOD_NAME);
	}
	return nCnt;
}

int local_mpdparser_get_represent_count(mpdElement_Represent_t *firstRepresent)
{
	int nCnt = 0;
	mpdElement_Represent_t *represent = firstRepresent;

	while(represent != NULL)


	{
		nCnt++;
		represent = local_mpdparser_get_next_element(represent, MPD_TAG_REPRESENTATION_NAME);
	}
	return nCnt;
}
*/
