/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   dash_mpdparser.c  $
 * Version:			$Revision:   1.0  $
 * Original Author: $
 * Current Author:	$Author: parkjh4@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	Dash mpd parser module
 * Module:
 * Remarks:
 */

/**
 * @defgroup DASH_MPDPARSER
 * @brief DASH의 MPD를 Parsing 및 구조화
 */

/**
 * @author
 * @date 30 Jul 2012
 */

/**
 * @brief DASH의 MPD를 Parsing 및 구조화
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file dash_mpdparser.c
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "di_iptuner_config.h"
#include "di_uart.h"
#include "dash_mpdparser.h"
#include "xmlmemory.h"
#include "parserInternals.h"
#include "parser.h"
#include "tree.h"

#include "htype.h"

#include "vkernel.h"
#include "taskdef.h"
#include "util_url.h"

//#define DASH_MEMORY_DEBUG

#ifdef DASH_MEMORY_DEBUG
#include "util_memtool.h"
#define VK_MEM_Alloc(X)		EIKI_MEM_Alloc((HUINT32)X,(HINT8*)__FUNCTION__,(HUINT32)__LINE__)
#define VK_MEM_Free(X)		EIKI_MEM_Free((void*)X,(HINT8*)__FUNCTION__,(HUINT32)__LINE__)
#define VK_MEM_Realloc(X,Y)	EIKI_MEM_Realloc((void*)X, (HUINT32) Y, (HINT8*)__FUNCTION__,(HUINT32)__LINE__)
#endif


/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define IPTUNER_ALL_DEBUG
//#define DASH_MPDPARSER_DEBUG
//#define DI_IPTUNER_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef DASH_MPDPARSER_DEBUG
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
#endif

#ifdef DI_IPTUNER_CRITICAL_DEBUG
#define PrintCriticalDebug		DI_UART_Print
#else
#define PrintCriticalDebug		while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)

/*----------------------------------------------------------------------------------------------
 *	MACRO
 *---------------------------------------------------------------------------------------------*/
#define DASH_TAG_MPD	"MPD"
#define DASH_TAG_PERIOD	"Period"
#define DASH_TAG_ADAPTATIONSET	"AdaptationSet"
#define DASH_TAG_REPRESENTATION	"Representation"
#define DASH_TAG_SUBREPRESENTATION	"SubRepresentation"

/* in MPD element */
#define DASH_ATTR_ID	"id"
#define DASH_ATTR_PROFILE	"profiles"
#define DASH_ATTR_TYPE	"type"
#define DASH_ATTR_AVAIL_START_TIME	"availabilityStartTime"
#define DASH_ATTR_AVAIL_END_TIME	"availabilityEndTime"
#define DASH_ATTR_MEDIA_PRESENTATION_DURATION	"mediaPresentationDuration"
#define DASH_ATTR_MINIMUM_UPDATE_PERIOD			"minimumUpdatePeriod"
#define DASH_ATTR_MIN_BUFFER_TIME	"minBufferTime"
#define DASH_ATTR_TIMESHIFT_BUFFER_DEPTH	"timeShiftBufferDepth"
#define DASH_ATTR_SUGGESTED_PRESENTATION_DELAY	"suggestedPresentationDelay"
#define DASH_ATTR_MAX_SEGMENT_DURATION		"maxSegmentDuration"
#define DASH_ATTR_MAX_SUBSEGMENT_DURATION	"maxSubsegmentDuration"
#define DASH_TAG_PROGRAMINFORMATION	"ProgramInformation"
#define DASH_TAG_LOCATION			"Location"
#define DASH_TAG_METRICS			"Metrics"

/* Common */
#define DASH_TAG_BASEURL			"BaseURL"
#define DASH_TAG_SEGMENTBASE		"SegmentBase"
#define DASH_ATTR_TIMESCALE			"timescale"
#define DASH_ATTR_PRESENTATIONTIMEOFFSET "presentationTimeOffset"
#define DASH_ATTR_INDEXRANGE		"indexRange"
#define DASH_ATTR_INDEXRANGEEXACT	"indexRangeExact"
#define DASH_TAG_INITIALISATION		"Initialisation"
#define DASH_TAG_INITIALIZATION		"Initialization"
#define DASH_TAG_REPRESENTATIONINDEX	"RepresentationIndex"
#define DASH_TAG_SEGMENTLIST		"SegmentList"
#define DASH_TAG_SEGMENTTEMPLATE	"SegmentTemplate"
#define DASH_TAG_COMMONATTRIBUTE	"CommonAttributesElements"
#define DASH_ATTR_WIDTH				"width"
#define DASH_ATTR_HEIGHT			"height"
#define DASH_ATTR_SAR				"sar"
#define DASH_ATTR_FRAMERATE			"frameRate"
#define DASH_ATTR_AUDIOSAMPLINGRATE	"audioSamplingRate"
#define DASH_ATTR_MIMETYPE			"mimeType"
#define DASH_ATTR_SEGMENTPROFILES	"segmentProfiles"
#define DASH_ATTR_CODECS			"codecs"
#define DASH_ATTR_MAXIMUMSAPPERIOD	"maximumSAPPeriod"
#define DASH_ATTR_STARTWITHSAP		"startWithSAP"
#define DASH_ATTR_MAXPLAYOUTRATE	"maxPlayoutRate"
#define DASH_ATTR_CODINGDEPENDENCY	"codingDependency"
#define DASH_ATTR_SCANTYPE			"scanType"
#define DASH_TAG_FRAMEPACKING		"FramePacking"
#define DASH_TAG_AUDIOCHANNELCONFIGURATION "AudioChannelConfiguration"
#define DASH_TAG_CONTENTPROTECTION	"ContentProtection"
#define DASH_ATTR_SCHEMEIDURI		"schemeIdUri"
#define DASH_ATTR_VALUE				"value"
#define DASH_ATTR_SOURCEURL			"sourceURL"
#define DASH_ATTR_RANGE				"range"

/* in Period element */
#define DASH_ATTR_START				"start"
#define DASH_ATTR_DURATION			"duration"
#define DASH_ATTR_BITSTREAMSWITCHING "bitstreamSwitching"
#define DASH_TAG_SUBSET				"Subset"

/* in Adaptationset element */
#define DASH_TAG_ACCESSIBILITY		"Accessibility"
#define DASH_TAG_ROLE				"Role"
#define DASH_TAG_RATING				"Rating"
#define DASH_TAG_VIEWPOINT			"ViewPoint"
#define DASH_TAG_CONTENTCOMPONENT	"ContentComponent"
#define DASH_ATTR_GROUP				"group"
#define DASH_ATTR_PAR				"par"
#define DASH_ATTR_CONTENTTYPE		"contentType"
#define DASH_ATTR_MINBANDWIDTH		"minBandwidth"
#define DASH_ATTR_MAXBANDWIDTH		"maxBandwidth"
#define DASH_ATTR_MINWIDTH			"minWidth"
#define DASH_ATTR_MAXWIDTH			"maxWidth"
#define DASH_ATTR_MINHEIGHT			"minHeight"
#define DASH_ATTR_MAXHEIGHT			"maxHeight"
#define DASH_ATTR_MINFRAMERATE		"minFrameRate"
#define DASH_ATTR_MAXFRAMERATE		"maxFrameRate"
#define DASH_ATTR_SEGMENTALIGNMENT	"segmentAlignment"
#define DASH_ATTR_SUBSEGMENTALIGNMENT "subsegmentAlignment"
#define DASH_ATTR_SUBSEGMENTSTARTSWITHSAP "subsegmentStartsWithSAP"

/* Representation element */
#define DASH_ATTR_BANDWIDTH			"bandwidth"
#define DASH_ATTR_QUALITYRANKING	"qualityRanking"
#define DASH_ATTR_DEPENDENCYID		"dependencyId"
#define DASH_ATTR_MEDIASTREAMSTRUCTUREID	"mediaStreamStructureId"

/* Subrepresentation element */
#define DASH_ATTR_LEVEL				"level"
#define DASH_ATTR_DEPENDENCYLEVEL	"dependencyLevel"
#define DASH_ATTR_CONTENTCOMPONENT  "contentComponent"

/* in Program information element */
#define DASH_ATTR_LANG				"lang"
#define DASH_ATTR_MOREINFORMATIONURL "moreInformationURL"
#define DASH_TAG_TITLE				"Title"
#define DASH_TAG_SOURCE				"Source"
#define DASH_TAG_COPYRIGHT			"Copyright"

/* in Location element */
#define DASH_ATTR_URI				"uri"

/* in metrics element */
#define DASH_ATTR_METRICS			"metrics"
#define DASH_TAG_RANGE				"range"
#define DASH_TAG_REPORTING			"Reporting"

/* in BaseURL information element */
#define DASH_TAG_SERVICELOCATION	"serviceLocation"
#define DASH_TAG_BYTERANGE			"byteRange"

/* xlink information */
#define DASH_ATTR_XLINK_HREF		"href"
#define DASH_ATTR_XLINK_ACTUATE		"actuate"

/* in SegmentBase information*/
#define DASH_TAG_INITIALISATION			"Initialisation"
#define DASH_TAG_REPRESENTATIONINDEX	"RepresentationIndex"

/* MultipleSegmentBase */
//#deinfe DASH_SEGMENTBASEINFO		"SegmentBase"
#define DASH_TAG_SEGMENTTIMELINE		"SegmentTimeline"
#define DASH_TAG_BITSTREAMSWITCHING		"BitStreamSwitching"
#define DASH_ATTR_STARTNUMBER			"startNumber"
#define DASH_ATTR_TIMELINE_T			"t"
#define DASH_ATTR_TIMELINE_D			"d"
#define DASH_ATTR_TIMELINE_R			"r"
#define DASH_ATTR_MEDIA					"media"
#define DASH_ATTR_MEDIARANGE			"mediaRange"
#define DASH_ATTR_INDEX					"index"
#define DASH_ATTR_INDEXRANGE			"indexRange"
#define DASH_ATTR_INITIALISATION		"initialisation"

/* Segment List */
#define DASH_TAG_SEGMENTURL				"SegmentURL"

/* Codec string */
#define DASH_CODECSTR_MP4A				"mp4a"
#define DASH_CODECSTR_MP4V				"mp4v"
#define DASH_CODECSTR_AVC1				"avc1"

static unsigned long long _atoui64(char *nptr)
{
	char *s = (char *)nptr;
	unsigned long long acc = 0;
	// int neg = 0;

  while(isspace((int)*s))
    s++;
/*  if (*s == '-')
    {
      neg = 1;
      s++;
    }
  else if (*s == '+')
    s++;
*/
  while (VK_isdigit((int)*s))
    {
      acc = 10 * acc + ((int)*s - '0');
      s++;
    }
/*
  if (neg)
    acc *= -1;
*/
  return acc;
}

static HBOOL __dash_strdup(DASH_STRING_t* dest, HINT8* src)
{
	size_t szStr;
	szStr = (size_t)VK_strlen(src);
	dest->length = szStr;
	dest->str = VK_MEM_Alloc(szStr+1);
	if(dest->str == NULL)
	{
		PrintDebug("%s(%d) Error : Not enough memory!!\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	VK_MEM_Memset((void*)dest->str, 0x0, (HUINT32)szStr+1);
	VK_strncpy((void*)dest->str, (void*)src, (HUINT32)szStr);

	return TRUE;
}

static int __dash_split(HINT8* msg, HINT8* split, HINT8*** result)
{
    HUINT32 i=0;
    HUINT32 charCount=0;
	HUINT32 totalCount=0;
    HINT8 *prevPoint=msg;
    HINT8 *currPoint=NULL;
    HINT8 **array2d=NULL;

	do
	{
        currPoint=(HINT8*)VK_strstr((const char*)prevPoint, (const char*)split);
        if(currPoint!=NULL){
            totalCount=currPoint-msg;					/*total length of msg so far.*/
            if(prevPoint==msg) charCount=totalCount;	/*total length of msg so far(1st)*/
            else charCount=currPoint-prevPoint;			/*difference between currPoint - prevPoint*/

			if(i==0)
			{
	            array2d=(HINT8**)VK_MEM_Alloc(sizeof(char*));
			}
			else
			{
				array2d=(HINT8**)VK_MEM_Realloc(array2d, sizeof(char*)*(i+1));
			}
            array2d[i]=(HINT8*)VK_MEM_Alloc(charCount);
			if(array2d[i] == NULL)
			{
				PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
				return 0;
			}
            VK_strncpy((char*)array2d[i], (const char*)prevPoint, charCount);
            array2d[i][charCount]='\0';
            prevPoint=currPoint+VK_strlen(split);
        }
    }
	while(currPoint!=NULL && ++i);
    if( i > 0 )
	{
        array2d=(HINT8**)VK_MEM_Realloc( array2d, sizeof(char*)*(i+1));
        charCount=VK_strlen(msg)-totalCount;
        array2d[i]=(HINT8*)VK_MEM_Alloc(charCount);
        VK_strncpy((char*)array2d[i], (const char*)prevPoint, charCount);
        array2d[i][charCount]='\0';
        ++i;
        *result=array2d;
    }
    return i;
}

void __dash_freeSplit(HINT8** result, HINT32 count)
{
	while (--count>-1)
	{
		VK_MEM_Free(result[count]);
		result[count] = NULL;
	}
	VK_MEM_Free(result);
	result = NULL;
}

void _traverse_document(xmlNode* a_node, int depth)
{
        xmlNode *cur_node = NULL;
        int i=0;
        for (cur_node = a_node; cur_node; cur_node = cur_node->next)
		{
                if (cur_node->type == XML_ELEMENT_NODE)
				{
                        for (i=0;i<depth;i++)
                                PrintDebug("----");
                        PrintDebug("<%s ", cur_node->name);
                        if ( cur_node->properties != NULL)
						{
                                xmlAttr* pAttr=cur_node->properties;
                                while (pAttr != NULL)
								{
                                        PrintDebug(" %s=\"%s\"", pAttr->name, pAttr->children->content);
                                        pAttr=pAttr->next;
                                }
                        }
                        PrintDebug("/>");
                        if (cur_node->children != NULL )
                                PrintDebug("%s", cur_node->children->content);
                        PrintDebug("\n");
                }
                _traverse_document(cur_node->children, depth + 1);
        }
}

static HINT64 __convert_ms_to_nonscaled(HINT64 ms, HINT64 timescale)
{
	return ms * timescale;
}

static HINT64 __convert_nonscaled_to_ms(HUINT64 nonscaled, HINT64 timescale)
{
	return nonscaled / timescale * 1000;
}

void DASH_MPD_Print_Representation(DASH_AdaptationSet_t* Adaptation)
{
	HUINT32 idx;
	HUINT32 idx_sub;
	HUINT32 cnt;
	UTIL_DLLNode_t* dllnode = NULL;
	DASH_Representation_t *representation = NULL;
	cnt = UTIL_DLLIST_Size(&Adaptation->Representation);
	PrintDebug("\tTotal Representation set : %d\n",cnt);

	for(idx = 0; idx < cnt; idx++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&Adaptation->Representation, idx);
		representation = dllnode->pData;

		PrintDebug("\t\t-------------------------\n");
		PrintDebug("\t\tRepresentation parsing [%d] result\n", idx);
		PrintDebug("\t\t-------------------------\n");

		PrintDebug("\t\tid : %s\n", representation->id.str);
		PrintDebug("\t\tbandwidth : %d\n", representation->bandwidth);
		PrintDebug("\t\twidth : %d\n", representation->CommonAttributesElements.width);
		PrintDebug("\t\theight : %d\n", representation->CommonAttributesElements.height);
		if(representation->isSegmentList == TRUE)
		{
			PrintDebug("\tSegmentlist size : %d\n", UTIL_DLLIST_Size(&representation->SegmentList.SegmentUrl));
			for(idx_sub = 0; idx_sub < UTIL_DLLIST_Size(&representation->SegmentList.SegmentUrl); idx_sub++)
			{
				UTIL_DLLNode_t* node = UTIL_DLLIST_Get_n_Node(&representation->SegmentList.SegmentUrl, idx_sub);
				DASH_URLTYPE_t* url = node->pData;
				PrintDebug("\t\tSegmentUrl[%d] : %s\n", idx_sub, url->sourceURL.str);
				PrintDebug("\t\t	Start: %d, End : %d\n", url->byteRange.start, url->byteRange.end);
			}
		}

		for (idx_sub = 0; idx_sub < UTIL_DLLIST_Size(&representation->BaseURL); idx_sub++)
		{
			UTIL_DLLNode_t* node = UTIL_DLLIST_Get_n_Node(&representation->BaseURL, idx_sub);
			PrintDebug("\t\tBaseURL [%d] : %s\n", idx_sub, ((DASH_BaseURL_t*)node->pData)->serviceLocation.str);
		}

		for (idx_sub = 0; idx_sub < UTIL_DLLIST_Size(&representation->mediaStreamStructureId); idx_sub++)
		{
			UTIL_DLLNode_t* node = UTIL_DLLIST_Get_n_Node(&representation->mediaStreamStructureId, idx_sub);
			PrintDebug("\t\tmediaStreamStructureId [%d] : %s\n", idx_sub, node->pData);
		}

		for (idx_sub = 0; idx_sub < UTIL_DLLIST_Size(&representation->dependencyId); idx_sub++)
		{
			UTIL_DLLNode_t* node = UTIL_DLLIST_Get_n_Node(&representation->dependencyId, idx_sub);
			PrintDebug("\t\tdependencyId [%d] : %s\n", idx_sub, node->pData);
		}

		PrintDebug("\t\tSetmentBase-indexRange start : %d\n", representation->SegmentBase.indexRange.start);
		PrintDebug("\t\tSetmentBase-indexRange end : %d\n", representation->SegmentBase.indexRange.end);
	}
}

void DASH_MPD_Print_Adaptation(DASH_Period_t* period)
{
	HUINT32 idx;
	HUINT32 idx_sub;
	HUINT32 cnt;
	UTIL_DLLNode_t* dllnode = NULL;
	DASH_AdaptationSet_t *adaptation= NULL;
	cnt = UTIL_DLLIST_Size(&period->AdaptationSet);
	PrintDebug("Total Adaptation set : %d\n",cnt);

	for(idx = 0; idx < cnt; idx++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&period->AdaptationSet, idx);
		adaptation = dllnode->pData;


		PrintDebug("\t-------------------------\n");
		PrintDebug("\tAdaptation parsing [%d] result\n", idx);
		PrintDebug("\t-------------------------\n");

		PrintDebug("\tid : %s\n", adaptation->id.str);
		PrintDebug("\tgroup : %s\n", adaptation->group.str);
		PrintDebug("\tcontentType : %s\n", adaptation->contentType.str);
		PrintDebug("\tmimeType : %s\n", adaptation->CommonAttributesElements.mimeType.str);
		PrintDebug("\tcodecs : %s\n", adaptation->CommonAttributesElements.codecs.str);
		PrintDebug("\tlangs : %s\n", adaptation->lang.str);
		PrintDebug("\tsubsegmentAlignment : %d\n", adaptation->subsegmentAlignment);
		if(adaptation->isSegmentList == TRUE)
		{
			PrintDebug("\tSegmentlist size : %d\n", UTIL_DLLIST_Size(&adaptation->SegmentList.SegmentUrl));
			for(idx_sub = 0; idx_sub < UTIL_DLLIST_Size(&adaptation->SegmentList.SegmentUrl); idx_sub++)
			{
				UTIL_DLLNode_t* node = UTIL_DLLIST_Get_n_Node(&adaptation->SegmentList.SegmentUrl, idx_sub);
				DASH_URLTYPE_t* url = node->pData;
				PrintDebug("\t\tSegmentUrl[%d] : %s\n", idx_sub, url->sourceURL.str);
			}
		}
		for (idx_sub = 0; idx_sub < UTIL_DLLIST_Size(&adaptation->Role); idx_sub++)
		{
			UTIL_DLLNode_t* node = UTIL_DLLIST_Get_n_Node(&adaptation->Role, idx_sub);
			PrintDebug("\tRole->schemeIdUrl : %s\n", ((DASH_DESCRIPTOR_t*)node->pData)->schemeIdUri.str);
			PrintDebug("\tRole->value : %s\n", ((DASH_DESCRIPTOR_t*)node->pData)->value.str);
		}

		for (idx_sub = 0; idx_sub < UTIL_DLLIST_Size(&adaptation->CommonAttributesElements.ContentProtection); idx_sub++)
		{
			UTIL_DLLNode_t* node = UTIL_DLLIST_Get_n_Node(&adaptation->CommonAttributesElements.ContentProtection, idx_sub);
			PrintDebug("\tContentProtection->schemeIdUrl : %s\n", ((DASH_DESCRIPTOR_t*)node->pData)->schemeIdUri.str);
			PrintDebug("\tContentProtection->value : %s\n", ((DASH_DESCRIPTOR_t*)node->pData)->value.str);
		}

		DASH_MPD_Print_Representation(adaptation);
	}
}

void DASH_MPD_Print_Period(DASH_MPD_t* mpd)
{
	HUINT32 idx;
	HUINT32 idx_sub;
	HUINT32 cnt;
	cnt = UTIL_DLLIST_Size(&mpd->Period);
	UTIL_DLLNode_t* dllnode;
	PrintDebug("Total Period : %d\n",cnt);
	DASH_Period_t *period;

	for(idx = 0; idx < cnt; idx++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&mpd->Period, idx);
		period = dllnode->pData;

		PrintDebug("-------------------------\n");
		PrintDebug("Period parsing [%d] result\n", idx);
		PrintDebug("-------------------------\n");
		if(period->xlink.href.str != NULL)
		{
			PrintDebug("xlink:href : %s\n", period->xlink.href.str);
		}
		PrintDebug("xlink:actuate : %d\n", period->xlink.eActuate);
		if(period->id.str != NULL)
		{
			PrintDebug("id : %s\n", period->id.str);
		}
		PrintDebug("start : %"PRId64"\n", period->start);
		PrintDebug("duration : %"PRId64"\n", period->duration);
		PrintDebug("bitstream switching : %d\n", period->bitstreamSwitching);
		for(idx_sub = 0; idx_sub < UTIL_DLLIST_Size(&period->BaseURL); idx_sub++)
		{
			UTIL_DLLNode_t* temp = UTIL_DLLIST_Get_n_Node(&period->BaseURL, idx_sub);
			if(((DASH_BaseURL_t*)temp->pData)->serviceLocation.str != NULL)
			{
				PrintDebug("Base URL [%d]: %s\n", idx_sub,((DASH_BaseURL_t*)temp->pData)->serviceLocation.str);
			}
		}

		DASH_MultipleSegmentBase_t* multi;

		if(period->isSegmentList)
		{
			PrintDebug("SegmentList used\n");
			multi = &period->SegmentList.MultipleSegmentBaseInformation;
		}
		else if(period->isSegmentTemplate)
		{
			PrintDebug("SegmentTemplate used\n");
			multi = &period->SegmentTemplate.MultipleSegmentBaseInformation;
		}

		if(period->isSegmentList || period->isSegmentTemplate)
		{
			PrintDebug("SegmentBase - indexRange(start) : %d\n", multi->SegmentBaseInformation.indexRange.start);
			PrintDebug("SegmentBase - indexRange(end) : %d\n", multi->SegmentBaseInformation.indexRange.end);
			PrintDebug("SegmentBase - indexRangeExact : %d\n", multi->SegmentBaseInformation.indexRangeExact);

			if(multi->SegmentBaseInformation.Initialisation.sourceURL.str != NULL)
			{
				PrintDebug("SegmentBase - Initialisation : %s\n", multi->SegmentBaseInformation.Initialisation.sourceURL.str);
			}
			if(multi->SegmentBaseInformation.RepresentationIndex.sourceURL.str != NULL)
			{
				PrintDebug("SegmentBase - Representation : %s\n", multi->SegmentBaseInformation.RepresentationIndex.sourceURL.str);
			}

			PrintDebug("MultipleInfo - duration : %d\n", multi->duration);
			PrintDebug("MultipleInfo - startNumber : %d\n", multi->startNumber);

			for(idx_sub = 0; idx_sub < UTIL_DLLIST_Size(&multi->SegmentTimeline); idx_sub++)
			{
				UTIL_DLLNode_t* temp = UTIL_DLLIST_Get_n_Node(&multi->SegmentTimeline, idx_sub);
				PrintDebug("MultipleInfo - Timeline[%d] : t(%d) d(%d) r(%d)\n", idx_sub, ((DASH_SegmentTimeline_t*)temp->pData)->t, ((DASH_SegmentTimeline_t*)temp->pData)->d, ((DASH_SegmentTimeline_t*)temp->pData)->r);
			}
			if( multi->BitstreamSwitching.sourceURL.str != NULL)
			{
				PrintDebug("MultipleInfo - BitstreamSwitching : %s\n", multi->BitstreamSwitching.sourceURL.str);
			}

			if(period->isSegmentList)
			{
				if(period->SegmentList.index.sourceURL.str != NULL)
				{
					PrintDebug("SegmentList - Index : %s\n", period->SegmentList.index.sourceURL.str);
				}
				PrintDebug("SegmentList - indexRange(start) : %d\n", period->SegmentList.indexRange.start);
				PrintDebug("SegmentList - indexRange(end) : %d\n", period->SegmentList.indexRange.end);
				PrintDebug("SegmentList - media : %d\n", period->SegmentList.media.sourceURL);
				PrintDebug("SegmentList - mediaRange(start) : %d\n", period->SegmentList.mediaRange.start);
				PrintDebug("SegmentList - mediaRange(end) : %d\n", period->SegmentList.mediaRange.end);
				for(idx_sub = 0; idx_sub < UTIL_DLLIST_Size(&period->SegmentList.SegmentUrl); idx_sub++)
				{
					UTIL_DLLNode_t* temp = UTIL_DLLIST_Get_n_Node(&period->SegmentList.SegmentUrl, idx_sub);
					PrintDebug("SegmentList - SegmentUrl[%d] : %d\n", idx_sub, ((DASH_URLTYPE_t*)temp->pData)->sourceURL);
				}
			}
			else if (period->isSegmentTemplate)
			{
				if(period->SegmentTemplate.bitstreamSwitching.str != NULL)
				{
					PrintDebug("SegmentTemplate - BitstreamSwitching : %s\n", period->SegmentTemplate.bitstreamSwitching.str);
				}
				if(period->SegmentTemplate.index.str != NULL)
				{
					PrintDebug("SegmentTemplate - index : %s\n", period->SegmentTemplate.index.str);
				}
				if( period->SegmentTemplate.initialisation.str != NULL)
				{
					PrintDebug("SegmentTemplate - initialisation : %s\n", period->SegmentTemplate.initialisation.str);
				}
				if(period->SegmentTemplate.media.str != NULL)
				{
					PrintDebug("SegmentTemplate - media : %s\n", period->SegmentTemplate.media.str);
				}
			}
		}
		DASH_MPD_Print_Adaptation(period);
	}
}
void DASH_MPD_Print_MPD(DASH_MPD_t* mpd)
{
	HUINT32 idx;

	PrintDebug("-------------------------\n");
	PrintDebug("MPD parsing result 		 \n");
	PrintDebug("-------------------------\n");
	PrintDebug("ID : %s\n", mpd->id.str);
	PrintDebug("Type : %d\n", mpd->type);
	PrintDebug("Profiles : %s\n", mpd->profiles);
	PrintDebug("Availability Start time : %"PRId64"\n", mpd->availibilityStartTime);
	PrintDebug("Availability End time : %"PRId64"\n", mpd->availibilityEndTime);
	PrintDebug("Media Presentation Duration : %"PRId64"\n", mpd->mediaPresentationDuration);
	PrintDebug("Minimum Update Period : %u\n", mpd->minimumUpdatePeriod);
	PrintDebug("Min Buffer time : %u\n", mpd->minBufferTime);
	PrintDebug("Time-shift buffer depth : %u\n", mpd->timeShiftBufferDepth);
	PrintDebug("Suggested Presentation delay : %u\n", mpd->suggestedPresentationDelay);
	PrintDebug("Max segment duration : %u\n", mpd->maxSegmentDuration);
	PrintDebug("Max subsegment duration : %u\n", mpd->maxSubsegmentDuration);

	for(idx = 0; idx < UTIL_DLLIST_Size(&mpd->BaseURL); idx++)
	{
		UTIL_DLLNode_t* temp = UTIL_DLLIST_Get_n_Node(&mpd->BaseURL, idx);
		DASH_BaseURL_t* burl = temp->pData;
		PrintDebug("Base URL [%d]: %s\n", idx, burl->serviceLocation.str);
	}

	for(idx = 0; idx < UTIL_DLLIST_Size(&mpd->ProgramInformation); idx++)
	{
		UTIL_DLLNode_t* temp = UTIL_DLLIST_Get_n_Node(&mpd->ProgramInformation, idx);
		PrintDebug("ProgramInformation[%d] lang : %s\n",idx, ((DASH_ProgramInformation_t*)temp->pData)->lang.str);
		PrintDebug("ProgramInformation[%d] more info url : %s\n",idx, ((DASH_ProgramInformation_t*)temp->pData)->moreInformationURL.str);
		PrintDebug("ProgramInformation[%d] Title : %s\n",idx, ((DASH_ProgramInformation_t*)temp->pData)->title.str);
		PrintDebug("ProgramInformation[%d] Source : %s\n",idx, ((DASH_ProgramInformation_t*)temp->pData)->source.str);
		PrintDebug("ProgramInformation[%d] Copyright : %s\n",idx, ((DASH_ProgramInformation_t*)temp->pData)->copyright.str);
	}

	for(idx = 0; idx < UTIL_DLLIST_Size(&mpd->Location); idx++)
	{
		UTIL_DLLNode_t* temp = UTIL_DLLIST_Get_n_Node(&mpd->Location, idx);
		PrintDebug("Location[%d] uri : %s\n",idx, ((DASH_Location_t*)temp->pData)->uri.str);
	}

	DASH_MPD_Print_Period(mpd);
}

static HINT64 P_MPD_ConvertTime(char *pStr, int nSize, HINT64 scale)
{
	char buff[5] = {0};
	HINT64 res = 0;
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
				res += VK_atoi(buff) * 60 * scale;
				break;
			case 'D' :
				/* not supported */
				break;
			case 'H' :
				nEnd = nPos;
				VK_strncpy(buff,pStr+nStart,nEnd-nStart);
//				PrintDebug ("mpdXsDurationToMs H : %s\n",buff);
				nStart = nPos+1;
				res += VK_atoi(buff) * 60 * 60 * scale;
				break;
			case 'S' :
				nEnd = nPos;
				VK_strncpy(buff,pStr+nStart,nEnd-nStart);
//				PrintDebug ("mpdXsDurationToMs S : %s\n",buff);
				nStart = nPos+1;
				res += atof(buff) * scale;

				break;
		}
		p++;
		nPos++;
	}
//	PrintDebug ("mpdXsDurationToMs Result %u\n",res);
	return res;
}

static HINT64 P_MPD_ConvertTimeToMS(char *pStr, int nSize)
{
	return P_MPD_ConvertTime(pStr,nSize,1000);
}

static xmlNode* P_MPD_FindNode(xmlNode* startNode, const xmlChar *szName)
{
	xmlNode* retNode = NULL;
	if(startNode == NULL) { return NULL; }
	if(!xmlStrcmp(startNode->name, szName))
	{
		return startNode;
	}
	else
	{
		retNode = P_MPD_FindNode(startNode->children, szName);
	}

	if( retNode == NULL)
	{
		retNode = P_MPD_FindNode(startNode->next, szName);
	}

	return retNode;
}

static DASH_SEGMENT_FORMAT_e P_MPD_FindSegmentFormat(xmlNode* root)
{
	xmlNode *foundNode = NULL;
	xmlChar *pXmlAttr = NULL;
	DASH_SEGMENT_FORMAT_e nRet = DASH_SEGMENT_UNKNOWN;

	foundNode = P_MPD_FindNode(root, (xmlChar*)DASH_TAG_ADAPTATIONSET);
	for( ; foundNode; foundNode = foundNode->next)
	{
		pXmlAttr = xmlGetProp( (xmlNode *)foundNode, (xmlChar*)DASH_ATTR_MIMETYPE);
		if(pXmlAttr != NULL)
		{
			if(!VK_strncmp((char*)pXmlAttr, "video/mp4", 9))
			{
				nRet = DASH_SEGMENT_MP4;
				xmlFree(pXmlAttr);
				return nRet;
			}
			else if(!VK_strncmp((char*)pXmlAttr, "video/mp2t", 10))
			{
				nRet = DASH_SEGMENT_TS;
				xmlFree(pXmlAttr);
				return nRet;
			}
			else
			{
				xmlFree(pXmlAttr);
			}
		}
	}

	foundNode = P_MPD_FindNode(root, (xmlChar*)DASH_TAG_REPRESENTATION);
	for( ; foundNode; foundNode = foundNode->next)
	{
		pXmlAttr = xmlGetProp( (xmlNode *)foundNode, (xmlChar*)DASH_ATTR_MIMETYPE);
		if(pXmlAttr != NULL)
		{
			if(!VK_strncmp((char*)pXmlAttr, "video/mp4", 9))
			{
				nRet = DASH_SEGMENT_MP4;
				xmlFree(pXmlAttr);
				return nRet;
			}
			else if(!VK_strncmp((char*)pXmlAttr, "video/mp2t", 10))
			{
				nRet = DASH_SEGMENT_TS;
				xmlFree(pXmlAttr);
				return nRet;
			}
			else
			{
				xmlFree(pXmlAttr);
			}
		}
	}

	foundNode = P_MPD_FindNode(root, (xmlChar*)DASH_TAG_SUBREPRESENTATION);
	for( ; foundNode; foundNode = foundNode->next)
	{
		pXmlAttr = xmlGetProp( (xmlNode *)foundNode, (xmlChar*)DASH_ATTR_MIMETYPE);
		if(pXmlAttr != NULL)
		{
			if(!VK_strncmp((char*)pXmlAttr, "video/mp4", 9))
			{
				nRet = DASH_SEGMENT_MP4;
				xmlFree(pXmlAttr);
				return nRet;
			}
			else if(!VK_strncmp((char*)pXmlAttr, "video/mp2t", 10))
			{
				nRet = DASH_SEGMENT_TS;
				xmlFree(pXmlAttr);
				return nRet;
			}
			else
			{
				xmlFree(pXmlAttr);
			}
		}
	}

	return DASH_SEGMENT_UNKNOWN;
}

static void *P_MPD_Get_MPD(DASH_MPDPARSER_HANDLE_t mpd)
{
	xmlNode *pNode = NULL;

	if(((xmlNode *)mpd->root)->children != NULL)
	{
		pNode = P_MPD_FindNode((xmlNode *)mpd->root, (xmlChar*)DASH_TAG_MPD);
		if(pNode != NULL)
		{
			return pNode;
		}
	}
	return NULL;
}

static void *P_MPD_Get_Period(DASH_MPDPARSER_HANDLE_t mpd)
{
	xmlNode *pNode = NULL;

	if(((xmlNode *)mpd->root)->children != NULL)
	{
		pNode = P_MPD_FindNode((xmlNode *)mpd->root, (xmlChar*)DASH_TAG_PERIOD);
		if(pNode != NULL)
		{
			return pNode;
		}
	}
	return NULL;
}

static xmlNode *P_MPD_Get_Element(void *entry, const xmlChar *szName)
{
	xmlNode *pNode = NULL;

	if (((xmlNode *)entry)->children != NULL )
	{
		pNode = P_MPD_FindNode(((xmlNode*)entry)->children, szName);
		if ( pNode != NULL )
		{
			return pNode;
		}
	}

	if (((xmlNode*)entry)->next == NULL )
	{
		return NULL;
	}

	return P_MPD_FindNode(((xmlNode*)entry)->next, szName);
}

static DASH_MPD_ERROR_e P_MPD_Get_Attr(void* entry, const xmlChar *szAttrName, HINT8 *szBuf, int nBufsize)
{
	int	nAttrLen = 0;
	xmlChar 		*pXmlAttr = NULL;

	pXmlAttr = xmlGetProp( (xmlNode *)entry, szAttrName);
	if ( pXmlAttr == NULL )
	{
		return DASH_MPD_ERR_ATTR_NOTFOUND;
	}

	nAttrLen = xmlStrlen(pXmlAttr);
	if ( nBufsize <= nAttrLen )
	{
		xmlFree(pXmlAttr);
		return DASH_MPD_ERR_ERROR_BUFFERSIZE;
	}
	VK_strncpy(szBuf, (char *)pXmlAttr, nAttrLen);
	szBuf[nAttrLen] = '\0';
	xmlFree(pXmlAttr);
	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Get_Attribute(xmlNode* element, HINT8* pBuf, xmlChar* pAttr)
{
	VK_MEM_Memset(pBuf, 0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);
	return P_MPD_Get_Attr(element, (xmlChar*)pAttr, pBuf, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);
}

static DASH_TIME_t P_MPD_FindTimeScale(void* root)
{
	xmlNode *foundNode = NULL;
	xmlChar *pXmlAttr = NULL;
	DASH_TIME_t scale = 0ULL;

	foundNode = P_MPD_FindNode((xmlNode*)root, (xmlChar*)DASH_TAG_SEGMENTTEMPLATE);
	if(foundNode == NULL)
	{
		foundNode = P_MPD_FindNode((xmlNode*)root, (xmlChar*)DASH_TAG_SEGMENTLIST);
		if(foundNode == NULL)
		{
			return 1;		//default time scale
		}
	}
	for( ; foundNode; foundNode = foundNode->next)
	{
		pXmlAttr = xmlGetProp( (xmlNode *)foundNode, (xmlChar*)DASH_ATTR_TIMESCALE);
		if(pXmlAttr != NULL)
		{
			scale = (DASH_TIME_t)_atoui64((char*)pXmlAttr);
			xmlFree(pXmlAttr);
			return scale;
		}
	}
	return 1;
}

static void P_MPD_ConvertStrToCodec(HUINT8* str, DASH_CODEC_e* codec)
{
	HUINT32 split_cnt = 0;
	HUINT8 **split = NULL;

	split_cnt = __dash_split((HINT8*)str, (HINT8*)".", (HINT8***)&split);
	if(split_cnt == 0)
	{
		if(!VK_strcmp(str, DASH_CODECSTR_AVC1))
		{
			*codec |= DASH_VIDEO_CODEC_AVC1;
		}
		else if(!VK_strcmp(str, DASH_CODECSTR_MP4V))
		{
			*codec |= DASH_VIDEO_CODEC_MP4V;
		}
	}
	else if(split_cnt > 0)
	{
		/* prefix : mp4a */
		if(split[0] != NULL && !VK_strcmp(split[0], DASH_CODECSTR_MP4A))
		{
			/* OTI for mp4a */
			if(split[1] != NULL && (!VK_strcmp(split[1], "40") || !VK_strcmp(split[1], "0x40")))
			{
				*codec |= DASH_AUDIO_CODEC_AAC;
				/* OTI for audio : 0x40 */
				if(split[2] != NULL && !VK_strcmp(split[2], "2"))
				{
					*codec |= DASH_AUDIO_CODEC_AAC; // Low complexible
				}
			}
		}
		/* prefix : mp4v */
		else if(split[0] != NULL && !VK_strcmp(split[0], DASH_CODECSTR_AVC1))
		{
			*codec |= DASH_VIDEO_CODEC_AVC1;
		}
		else
		{
			PrintError("%s(%d) Not supported codec by DASH \n", __FUNCTION__, __LINE__);
			*codec = DASH_CODEC_NONE;
		}
		__dash_freeSplit((HINT8**)split, split_cnt);
		split = NULL;
	}
}

static DASH_CODEC_e P_MPD_ParseCodec(HINT8* codecstr)
{
	HUINT32 nrcodecs = 0;
	HUINT8 **codecs = NULL;
	HUINT32 loopcnt = 0;
	DASH_CODEC_e codec = DASH_CODEC_NONE;

	nrcodecs = __dash_split(codecstr, (HINT8*)",", (HINT8***)&codecs);
	if(nrcodecs == 0)
	{
		P_MPD_ConvertStrToCodec(codecstr, &codec);
	}
	else if(nrcodecs > 0)
	{
		for(loopcnt = 0; loopcnt < nrcodecs; loopcnt++)
		{
			P_MPD_ConvertStrToCodec(codecs[loopcnt], &codec);
		}
		__dash_freeSplit((HINT8**)codecs, nrcodecs);
		codecs = NULL;
	}
	return codec;
}

DASH_MPD_ERROR_e P_MPD_Parse_BaseURL(xmlNode* element, DASH_BaseURL_t **url)
{
	DASH_BaseURL_t* pUrl = NULL;

	if(element->children != NULL)
	{
		*url = VK_MEM_Alloc(sizeof(DASH_BaseURL_t));
		if(*url == NULL)
		{
			PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
			return DASH_MPD_ERR_ERROR;
		}
		pUrl = *url;
		__dash_strdup(&pUrl->serviceLocation, (HINT8*)element->children->content);
	}
	return DASH_MPD_ERR_OK;
}

DASH_MPD_ERROR_e P_MPD_Parse_Location(xmlNode* element, DASH_Location_t** loc)
{
	HINT8 szBuf[DASH_MPDPARSER_CONST_ATTR_BUFSIZE];
	DASH_Location_t* ploc = NULL;
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;

	VK_MEM_Memset(szBuf, 0x0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);

	*loc = VK_MEM_Alloc(sizeof(DASH_Location_t));
	if(*loc == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	VK_MEM_Memset(*loc, 0, sizeof(DASH_Location_t));
	ploc = *loc;

	/* uri */
	merr = P_MPD_Get_Attribute(element, szBuf, (xmlChar*)DASH_ATTR_URI);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&ploc->uri, szBuf);
	}

	return merr;
}

DASH_MPD_ERROR_e P_MPD_Parse_Metrics(xmlNode * element, DASH_Metrics_t **metrics)
{
	HINT8 szBuf[DASH_MPDPARSER_CONST_ATTR_BUFSIZE];
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	DASH_Metrics_t* pMetrics = NULL;
	xmlNode* traverseNode = NULL;
	DASH_RANGE_t* range = NULL;
	DASH_DESCRIPTOR_t* reporting = NULL;

	VK_MEM_Memset(szBuf, 0x0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);

	*metrics = VK_MEM_Alloc(sizeof(DASH_Metrics_t));
	if(*metrics == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	VK_MEM_Memset(*metrics, 0, sizeof(DASH_Metrics_t));
	VK_MEM_Memset(szBuf, 0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);
	pMetrics = *metrics;

	/* metrics */
	merr = P_MPD_Get_Attr(traverseNode, (xmlChar*)DASH_ATTR_METRICS, szBuf, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		// TODO: metrics insert
		//__dash_strdup(&(metrics->metrics), szBuf);
	}

	for (traverseNode = element->children; traverseNode; traverseNode = traverseNode->next)
	{
		if(traverseNode->children != NULL)
		{
			/* title element */
			if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_RANGE))
			{
				// TODO: range setting
				/* __dash_strdup(&range->, traverseSubNode->children->content);
				split_cnt = __dash_split(pBuf, "-", &split);
				pBase->Initialisation.byteRange.start = (HUINT32)VK_atoi(split[0]);
				pBase->Initialisation.byteRange.end = (HUINT32)VK_atoi(split[1]);
				__dash_freeSplit(split, split_cnt);
				*/
				range = VK_MEM_Alloc(sizeof(DASH_RANGE_t));
				if(range == NULL)
				{
					PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
					return DASH_MPD_ERR_ERROR;
				}
				VK_MEM_Memset(range, 0, sizeof(DASH_RANGE_t));
				UTIL_DLLIST_Append(&pMetrics->Range, range);
			}
			/* Source element */
			if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_REPORTING))
			{
				// TODO: reporting setting
				/* __dash_strdup(&pi->source, traverseSubNode->children->content);*/
				reporting = VK_MEM_Alloc(sizeof(DASH_DESCRIPTOR_t));
				if(reporting == NULL)
				{
					PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
					return DASH_MPD_ERR_ERROR;
				}
				VK_MEM_Memset(reporting, 0, sizeof(DASH_DESCRIPTOR_t));
				UTIL_DLLIST_Append(&pMetrics->Reporting, reporting);
			}
		}
	}

	return DASH_MPD_ERR_OK;
}

DASH_MPD_ERROR_e P_MPD_Parse_ContentComponent(xmlNode* element, DASH_ContentComponent_t **component)
{
	HINT8 szBuf[DASH_MPDPARSER_CONST_ATTR_BUFSIZE];
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	DASH_ContentComponent_t *pComponent = NULL;

	VK_MEM_Memset(szBuf, 0x0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);
	*component = VK_MEM_Alloc(sizeof(DASH_ContentComponent_t));
	if(*component == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	VK_MEM_Memset(*component, 0, sizeof(DASH_ContentComponent_t));
	pComponent = *component;

	/* id */
	merr = P_MPD_Get_Attribute(element, szBuf, (xmlChar*)DASH_ATTR_ID);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&pComponent->id, szBuf);
	}

	/* lang */
	merr = P_MPD_Get_Attribute(element, szBuf, (xmlChar*)DASH_ATTR_LANG);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&pComponent->lang, szBuf);
	}
	/* contentType */
	merr = P_MPD_Get_Attribute(element, szBuf, (xmlChar*)DASH_ATTR_CONTENTTYPE);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&pComponent->contentType, szBuf);
	}

	/* par */
	merr = P_MPD_Get_Attribute(element, szBuf, (xmlChar*)DASH_ATTR_PAR);
	if(merr == DASH_MPD_ERR_OK)
	{
		if(!VK_strncmp("16:9",(char*)szBuf,4))
		{
			pComponent->par = DASH_PAR_16_9;
		}
		else if(!VK_strncmp("4:3",(char*)szBuf,4))
		{
			pComponent->par = DASH_PAR_4_3;
		}
	}

#if 0
	/* Accessibility, Role, Rating, Viewpoint */
	if( !xmlStrcmp(traverseNode->name, DASH_TAG_ACCESSIBILITY) ||
	    !xmlStrcmp(traverseNode->name, DASH_TAG_ROLE) ||
		!xmlStrcmp(traverseNode->name, DASH_TAG_RATING)  ||
		!xmlStrcmp(traverseNode->name, DASH_TAG_VIEWPOINT))
	{
		desc = VK_MEM_Alloc(sizeof(DASH_DESCRIPTOR_t));
		if(desc == NULL)
		{
			PrintError("Not enough memory\n");
		}

		merr = P_MPD_Get_Attribute(traverseNode, szBuf, (xmlChar*)DASH_ATTR_SCHEMEIDURI);
		if(merr == DASH_MPD_ERR_OK)
		{
			__dash_strdup(&desc->schemeIdUri, szBuf);
		}
		else
		{
			VK_MEM_Free(desc);
			desc = NULL;
			return DASH_MPD_ERR_PARSE_ERROR;
		}

		merr = P_MPD_Get_Attribute(traverseNode, szBuf, (xmlChar*)DASH_ATTR_VALUE);
		if(merr == DASH_MPD_ERR_OK)
		{
			__dash_strdup(&desc->value, szBuf);
		}

		if( !xmlStrcmp(traverseNode->name, DASH_TAG_ACCESSIBILITY) ||
		    !xmlStrcmp(traverseNode->name, DASH_TAG_ROLE) ||
			!xmlStrcmp(traverseNode->name, DASH_TAG_RATING)  ||
			!xmlStrcmp(traverseNode->name, DASH_TAG_VIEWPOINT))
		{
			if(!xmlStrcmp(traverseNode->name, DASH_TAG_ACCESSIBILITY))
			{
				UTIL_DLLIST_Append(&pComponent->Accessibility, desc);
			}
			else if(!xmlStrcmp(traverseNode->name, DASH_TAG_ROLE))
			{
				UTIL_DLLIST_Append(&pComponent->Role, desc);
			}
			else if(!xmlStrcmp(traverseNode->name, DASH_TAG_RATING))
			{
				UTIL_DLLIST_Append(&pComponent->Rating, desc);
			}
			else if(!xmlStrcmp(traverseNode->name, DASH_TAG_VIEWPOINT))
			{
				UTIL_DLLIST_Append(&pComponent->ViewPoint, desc);
			}
		}
	}
#endif
	return DASH_MPD_ERR_OK;
}

DASH_MPD_ERROR_e P_MPD_Parse_ProgramInformation(xmlNode* element, DASH_ProgramInformation_t **pi)
{
	HINT8 szBuf[DASH_MPDPARSER_CONST_ATTR_BUFSIZE];
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	DASH_ProgramInformation_t *pProgram = NULL;
	xmlNode* traverseNode = NULL;

	VK_MEM_Memset(szBuf, 0x0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);

	*pi = VK_MEM_Alloc(sizeof(DASH_ProgramInformation_t));
	if(*pi == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	VK_MEM_Memset(*pi, 0, sizeof(DASH_ProgramInformation_t));
	pProgram = *pi;

	/* lang */
	merr = P_MPD_Get_Attribute(element, szBuf, (xmlChar*)DASH_ATTR_LANG);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&pProgram->lang, szBuf);
	}

	/* more information */
	merr = P_MPD_Get_Attribute(element, szBuf, (xmlChar*)DASH_ATTR_MOREINFORMATIONURL);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&pProgram->moreInformationURL, szBuf);
	}

	/* elements */
	for (traverseNode = element->children; traverseNode; traverseNode = traverseNode->next)
	{
		if(traverseNode->children != NULL)
		{
			/* title element */
			if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_TITLE))
			{
				__dash_strdup(&pProgram->title, (HINT8*)traverseNode->children->content);
			}
			/* Source element */
			if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_SOURCE))
			{
				__dash_strdup(&pProgram->source, (HINT8*)traverseNode->children->content);
			}
			/* Copyright element */
			if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_COPYRIGHT))
			{
				__dash_strdup(&pProgram->copyright, (HINT8*)traverseNode->children->content);
			}
		}
	}
	return DASH_MPD_ERR_OK;
}
DASH_MPD_ERROR_e P_MPD_Parse_SegmentBase(xmlNode* element, DASH_SegmentBase_t** segmentbase)
{
	HINT8 pBuf[DASH_MPDPARSER_CONST_ATTR_BUFSIZE];
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	DASH_SegmentBase_t* pBase = NULL;
	xmlNode* foundNode = NULL;
	HUINT8 **split = NULL;
	HUINT32 split_cnt = 0;

	VK_MEM_Memset(pBuf, 0x0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);

	*segmentbase = VK_MEM_Alloc(sizeof(DASH_SegmentBase_t));
	if(*segmentbase == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	VK_MEM_Memset(*segmentbase, 0, sizeof(DASH_SegmentBase_t));
	pBase = *segmentbase;

	/* timescale */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_TIMESCALE);
	if(merr == DASH_MPD_ERR_OK)
	{
		pBase->timescale = (DASH_TIME_t)_atoui64((char*)pBuf);
	}
	else
	{
		pBase->timescale = 1;
	}

	/* presentationTimeOffset */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_PRESENTATIONTIMEOFFSET);
	if(merr == DASH_MPD_ERR_OK)
	{
		pBase->presentationTimeOffset = (DASH_TIME_t)_atoui64((char*)pBuf);
	}

	/* indexRange */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_INDEXRANGE);
	if(merr == DASH_MPD_ERR_OK)
	{
		split_cnt = __dash_split(pBuf, (HINT8*)"-", (HINT8***)&split);
		pBase->indexRange.start = (HUINT32)VK_atoi((char*)split[0]);
		pBase->indexRange.end = (HUINT32)VK_atoi((char*)split[1]);
		__dash_freeSplit((HINT8**)split, split_cnt);

		/* indexRangeExact : depend on a existence of indexRange */
		VK_MEM_Memset(pBuf, 0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);
		merr = P_MPD_Get_Attr(element, (xmlChar*)DASH_ATTR_INDEXRANGEEXACT, pBuf, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);
		if(merr == DASH_MPD_ERR_OK)
		{
			if(!VK_strncmp((char*)pBuf, "true", 4))
			{
				pBase->indexRangeExact = TRUE;
			}
			else
			{
				pBase->indexRangeExact = FALSE;
			}
		}
	}

	/* Initialisation */
	foundNode = P_MPD_Get_Element(element, (xmlChar*)DASH_TAG_INITIALISATION);
	if(foundNode != NULL)
	{
		merr = P_MPD_Get_Attribute(foundNode, pBuf, (xmlChar*)DASH_ATTR_SOURCEURL);
		if(merr == DASH_MPD_ERR_OK)
		{
			__dash_strdup(&(pBase->Initialisation.sourceURL), pBuf);
		}

		merr = P_MPD_Get_Attribute(foundNode, pBuf, (xmlChar*)DASH_ATTR_RANGE);
		if(merr == DASH_MPD_ERR_OK)
		{
			split_cnt = __dash_split(pBuf, (HINT8*)"-", (HINT8***)&split);
			pBase->Initialisation.byteRange.start = (HUINT32)VK_atoi((char*)split[0]);
			pBase->Initialisation.byteRange.end = (HUINT32)VK_atoi((char*)split[1]);
			__dash_freeSplit((HINT8**)split, split_cnt);
		}
	}

	foundNode = P_MPD_Get_Element(element, (xmlChar*)DASH_TAG_INITIALIZATION);
	if(foundNode != NULL)
	{
		merr = P_MPD_Get_Attribute(foundNode, pBuf, (xmlChar*)DASH_ATTR_SOURCEURL);
		if(merr == DASH_MPD_ERR_OK)
		{
			__dash_strdup(&(pBase->Initialisation.sourceURL), pBuf);
		}

		merr = P_MPD_Get_Attribute(foundNode, pBuf, (xmlChar*)DASH_ATTR_RANGE);
		if(merr == DASH_MPD_ERR_OK)
		{
			split_cnt = __dash_split(pBuf, (HINT8*)"-", (HINT8***)&split);
			pBase->Initialisation.byteRange.start = (HUINT32)VK_atoi((char*)split[0]);
			pBase->Initialisation.byteRange.end = (HUINT32)VK_atoi((char*)split[1]);
			__dash_freeSplit((HINT8**)split, split_cnt);
		}
	}

	/* Representation Index */
	foundNode = P_MPD_Get_Element(element, (xmlChar*)DASH_TAG_REPRESENTATIONINDEX);
	if(foundNode != NULL)
	{
		__dash_strdup(&(pBase->RepresentationIndex.sourceURL), (HINT8*)foundNode->children->content);
	}

	return DASH_MPD_ERR_OK;
}

DASH_MPD_ERROR_e P_MPD_Parse_MultipleSegmentBaseInformation(xmlNode *element, DASH_MultipleSegmentBase_t **seginfo)
{
	HINT8 pBuf[DASH_MPDPARSER_CONST_ATTR_BUFSIZE];
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	DASH_MultipleSegmentBase_t* pInfo = NULL;
	DASH_SegmentBase_t* pBaseInfo = NULL;
	DASH_SegmentTimeline_t* timeline = NULL;
	xmlNode* traverseNode = NULL;
	xmlNode* foundNode = NULL;

	VK_MEM_Memset(pBuf, 0x0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);

	*seginfo = VK_MEM_Alloc(sizeof(DASH_MultipleSegmentBase_t));
	if(*seginfo == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	VK_MEM_Memset(*seginfo, 0, sizeof(DASH_MultipleSegmentBase_t));
	pInfo = *seginfo;

	merr = P_MPD_Parse_SegmentBase(element, &pBaseInfo);
	if(merr == DASH_MPD_ERR_OK)
	{
		VK_MEM_Memcpy(&pInfo->SegmentBaseInformation, pBaseInfo, sizeof(DASH_SegmentBase_t));
		VK_MEM_Free(pBaseInfo);
		pBaseInfo = NULL;
	}
	else
	{
		return DASH_MPD_ERR_PARSE_ERROR;
	}

	/* duration */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_DURATION);
	if(merr == DASH_MPD_ERR_OK)
	{
		pInfo->duration = (HUINT32)VK_atoi((char*)pBuf);
	}

	/* startnumber */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_STARTNUMBER);
	if(merr == DASH_MPD_ERR_OK)
	{
		pInfo->startNumber = (HUINT32)VK_atoi((char*)pBuf);
	}

	/* Element : this elements have the value directle. ex) <Initialisation>http://</Initialisation> */

	/* Initialisation */
	foundNode = P_MPD_Get_Element(element, (xmlChar*)DASH_TAG_BITSTREAMSWITCHING);
	if(foundNode != NULL)
	{
		__dash_strdup(&(pInfo->BitstreamSwitching.sourceURL), (HINT8*)foundNode->children->content);
	}

	/* SegmentTimeline */
	foundNode = P_MPD_Get_Element(element, (xmlChar*)DASH_TAG_SEGMENTTIMELINE);
	if(foundNode != NULL)
	{
		foundNode = P_MPD_Get_Element(element, (xmlChar*)"S");
		for(traverseNode = (xmlNode*)foundNode; traverseNode; traverseNode = traverseNode->next)
		{
			if (traverseNode->type != XML_ELEMENT_NODE)
			{
				continue;
			}
			timeline = VK_MEM_Alloc(sizeof(DASH_SegmentTimeline_t));
			if(timeline == NULL)
			{
				PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
				return DASH_MPD_ERR_ERROR;
			}
			if(timeline == NULL)
			{
				PrintError("%s(%d) Not enough memory\n", __FUNCTION__, __LINE__);
				VK_MEM_Free(timeline);
				timeline = NULL;
				return DASH_MPD_ERR_PARSE_ERROR;
			}

			/* t */
			merr = P_MPD_Get_Attribute(traverseNode, pBuf, (xmlChar*)DASH_ATTR_TIMELINE_T);
			if(merr == DASH_MPD_ERR_OK)
			{
				timeline->t = (DASH_TIME_t)_atoui64((char*)pBuf);
			}
			else
			{
				timeline->t = -1;
			}

			/* d */
			merr = P_MPD_Get_Attribute(traverseNode, pBuf, (xmlChar*)DASH_ATTR_TIMELINE_D);
			if(merr == DASH_MPD_ERR_OK)
			{
				timeline->d = (DASH_TIME_t)_atoui64((char*)pBuf);
			}
			else
			{
				return DASH_MPD_ERR_PARSE_ERROR;
			}

			/* r */
			merr = P_MPD_Get_Attribute(traverseNode, pBuf, (xmlChar*)DASH_ATTR_TIMELINE_R);
			if(merr == DASH_MPD_ERR_OK)
			{
				timeline->r = (HUINT32)VK_atoi((char*)pBuf);
			}
			else
			{
				timeline->r = 0;
			}

			pInfo->isSegmentTimeline = TRUE;
			UTIL_DLLIST_Append(&pInfo->SegmentTimeline, timeline);
		}
	}
	return DASH_MPD_ERR_OK;
}

DASH_MPD_ERROR_e P_MPD_Parse_SegmentList(xmlNode* element, DASH_SegmentList_t **segmentlist)
{
	HINT8 pBuf[DASH_MPDPARSER_CONST_ATTR_BUFSIZE];
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	xmlNode* traverseNode = NULL;
	DASH_SegmentList_t* pList = NULL;
	DASH_MultipleSegmentBase_t* pInfo = NULL;
	DASH_URLTYPE_t *pTempURL = NULL;
	HUINT32 split_cnt = 0;
	HINT8** split = NULL;

	VK_MEM_Memset(pBuf, 0x0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);

	*segmentlist = VK_MEM_Alloc(sizeof(DASH_SegmentList_t));
	if(*segmentlist == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	VK_MEM_Memset(*segmentlist, 0, sizeof(DASH_SegmentList_t));
	pList = *segmentlist;

	merr = 	P_MPD_Parse_MultipleSegmentBaseInformation(element, &pInfo);
	if(merr == DASH_MPD_ERR_OK)
	{
		VK_MEM_Memcpy(&pList->MultipleSegmentBaseInformation, pInfo, sizeof(DASH_MultipleSegmentBase_t));
		VK_MEM_Free(pInfo);
		pInfo = NULL;
	}
	else
	{
		return DASH_MPD_ERR_PARSE_ERROR;
	}

	/* xlink:href */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_XLINK_HREF);
	if(merr == DASH_MPD_ERR_OK)
	{
		PrintError("currently, xlink is not supported\n");
		return DASH_MPD_ERR_ERROR;
#if 0
		__dash_strdup(&(pList->xlink.href), pBuf);
		pList->isXlink = TRUE;
#endif
	}

	/* xlink:actuate */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_XLINK_ACTUATE);
	if(merr == DASH_MPD_ERR_OK)
	{
		if(!VK_strncmp((char*)pBuf, "OnLoad", 6))
		{
			pList->xlink.eActuate = DASH_XLINK_ACTUATE_ONLOAD;
		}
		else if(!VK_strncmp((char*)pBuf, "OnRequest", 9))
		{
			pList->xlink.eActuate = DASH_XLINK_ACTUATE_ONREQUEST;
		}
	}
	else if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		pList->xlink.eActuate = DASH_XLINK_ACTUATE_ONREQUEST;
	}


	for (traverseNode = ((xmlNode*)element)->children; traverseNode; traverseNode = traverseNode->next)
	{
		if( !xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_SEGMENTURL))
		{
			pTempURL = VK_MEM_Alloc(sizeof(DASH_URLTYPE_t));
			if(pTempURL == NULL)
			{
				PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
				return DASH_MPD_ERR_ERROR;
			}
			merr = P_MPD_Get_Attribute(traverseNode, pBuf, (xmlChar*)DASH_ATTR_MEDIA);
			if(merr == DASH_MPD_ERR_OK)
			{
				__dash_strdup(&pTempURL->sourceURL, pBuf);
			}

			merr = P_MPD_Get_Attribute(traverseNode, pBuf, (xmlChar*)DASH_ATTR_MEDIARANGE);
			if(merr == DASH_MPD_ERR_OK)
			{
				split_cnt = __dash_split(pBuf, (HINT8*)"-", (HINT8***)&split);
				pTempURL->byteRange.start = (HUINT32)VK_atoi((char*)split[0]);
				pTempURL->byteRange.end = (HUINT32)VK_atoi((char*)split[1]);
				__dash_freeSplit((HINT8**)split, split_cnt);
			}
			else
			{
				pTempURL->byteRange.start = 0;
				pTempURL->byteRange.end = 0;
			}

			UTIL_DLLIST_Append(&pList->SegmentUrl, pTempURL);
		}
	}

	return DASH_MPD_ERR_OK;

}
DASH_MPD_ERROR_e P_MPD_Parse_SegmentTemplate(xmlNode* element, DASH_SegmentTemplate_t **segmenttemplate)
{
	HINT8 pBuf[DASH_MPDPARSER_CONST_ATTR_BUFSIZE];
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	DASH_SegmentTemplate_t* pTemplate = NULL;
	DASH_MultipleSegmentBase_t* pInfo = NULL;

	VK_MEM_Memset(pBuf, 0x0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);

	*segmenttemplate = VK_MEM_Alloc(sizeof(DASH_SegmentTemplate_t));
	if(*segmenttemplate == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	VK_MEM_Memset(*segmenttemplate, 0, sizeof(DASH_SegmentTemplate_t));
	pTemplate = *segmenttemplate;

	merr = 	P_MPD_Parse_MultipleSegmentBaseInformation(element, &pInfo);
	if(merr == DASH_MPD_ERR_OK)
	{
		VK_MEM_Memcpy(&pTemplate->MultipleSegmentBaseInformation, pInfo, sizeof(DASH_MultipleSegmentBase_t));
		VK_MEM_Free(pInfo);
		pInfo = NULL;
	}
	else
	{
		return DASH_MPD_ERR_PARSE_ERROR;
	}

	/* media */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_MEDIA);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&(pTemplate->media), pBuf);
	}

	/* index */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_INDEX);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&(pTemplate->index), pBuf);
	}

	/* initialisation */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_INITIALISATION);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&(pTemplate->initialisation), pBuf);
	}

	/* bitstreamSwitching */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_BITSTREAMSWITCHING);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&(pTemplate->bitstreamSwitching), pBuf);
	}

	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Parse_CommonAttributes(xmlNode* element, DASH_CommonAttributesElements_t **comm)
{
	HINT8 pBuf[DASH_MPDPARSER_CONST_ATTR_BUFSIZE];
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	xmlNode* traverseNode = NULL;
	DASH_DESCRIPTOR_t *desc = NULL;
	DASH_CommonAttributesElements_t* pCommon = NULL;

	VK_MEM_Memset(pBuf, 0x0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);

	*comm = VK_MEM_Alloc(sizeof(DASH_CommonAttributesElements_t));
	if(*comm == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	VK_MEM_Memset(*comm, 0, sizeof(DASH_CommonAttributesElements_t));
	pCommon = *comm;

	/* profile */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_PROFILE);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		__dash_strdup(&(pCommon->profile), pBuf);
	}

	/* width */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_WIDTH);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		pCommon->width = (HUINT32)VK_atoi((char*)pBuf);
	}

	/* height */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_HEIGHT);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		pCommon->height = (HUINT32)VK_atoi((char*)pBuf);
	}

	/* sar */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_SAR);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		if(!VK_strncmp("16:9",(char*)pBuf,4))
		{
			pCommon->sar = DASH_SAR_16_9;
		}
		else if(!VK_strncmp("4:3",(char*)pBuf,4))
		{
			pCommon->sar = DASH_SAR_4_3;
		}
	}

	/* frameRate */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_FRAMERATE);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		__dash_strdup(&(pCommon->frameRate), pBuf);
	}

	/* audioSamplingRate */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_AUDIOSAMPLINGRATE);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		__dash_strdup(&(pCommon->audioSamplingRate), pBuf);
	}

	/* mimetype */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_MIMETYPE);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		__dash_strdup(&(pCommon->mimeType), pBuf);
	}

	/* segmentProfiles */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_SEGMENTPROFILES);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		__dash_strdup(&(pCommon->segmentProfiles), pBuf);
	}

	/* codecs */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_CODECS);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		__dash_strdup(&(pCommon->codecs), pBuf);
		pCommon->eCodec = P_MPD_ParseCodec(pCommon->codecs.str);
	}

	/* maximumSAPPeriod */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_MAXIMUMSAPPERIOD);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		pCommon->maximumSAPPeriod = (double)atof((char*)pBuf);
	}

	/* startWithSAP */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_STARTWITHSAP);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		pCommon->startWithSAP = (HUINT32)VK_atoi((char*)pBuf);
	}

	/* maxPlayoutRate */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_MAXPLAYOUTRATE);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		pCommon->maxPlayRate = (double)atof((char*)pBuf);
	}

	/* codingDependency */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_CODINGDEPENDENCY);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		if(!VK_strncmp("true",(char*)pBuf,4))
		{
			pCommon->codingDependency = TRUE;
		}
		else
		{
			pCommon->codingDependency = FALSE;
		}
	}

	/* scanType */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_SCANTYPE);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		if(!VK_strncmp("interlaced",(char*)pBuf,4))
		{
			pCommon->scantype = DASH_VIDEOSCAN_INTERACE;
		}
		else if(!VK_strncmp("progressive",(char*)pBuf,4))
		{
			pCommon->scantype = DASH_VIDEOSCAN_PROGRESSIVE;
		}
		else
		{
			pCommon->scantype = DASH_VIDEOSCAN_UNKNOWN;
		}
	}

	/* element parsing */
	for (traverseNode = ((xmlNode*)element)->children; traverseNode; traverseNode = traverseNode->next)
	{
		if( !xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_FRAMEPACKING) ||
		    !xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_AUDIOCHANNELCONFIGURATION) ||
			!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_CONTENTPROTECTION ))
		{
			desc = VK_MEM_Alloc(sizeof(DASH_DESCRIPTOR_t));
			if(desc == NULL)
			{
				PrintError("Not enough memory\n");
				return DASH_MPD_ERR_ERROR;
			}
			VK_MEM_Memset(desc, 0, sizeof(DASH_DESCRIPTOR_t));

			merr = P_MPD_Get_Attribute(traverseNode, pBuf, (xmlChar*)DASH_ATTR_SCHEMEIDURI);
			if(merr == DASH_MPD_ERR_OK)
			{
				__dash_strdup(&desc->schemeIdUri, pBuf);
			}
			else
			{
				return DASH_MPD_ERR_PARSE_ERROR;
			}

			merr = P_MPD_Get_Attribute(traverseNode, pBuf, (xmlChar*)DASH_ATTR_VALUE);
			if(merr == DASH_MPD_ERR_OK)
			{
				__dash_strdup(&desc->value, pBuf);
			}


			if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_FRAMEPACKING))
			{
				UTIL_DLLIST_Append(&pCommon->FramePacking, desc);
			}
			else if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_AUDIOCHANNELCONFIGURATION))
			{
				UTIL_DLLIST_Append(&pCommon->AudioChannelConfiguration, desc);
			}
			else if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_CONTENTPROTECTION))
			{
				UTIL_DLLIST_Append(&pCommon->ContentProtection, desc);
			}
		}
	}
	return DASH_MPD_ERR_OK;
}


static DASH_MPD_ERROR_e P_MPD_Parse_Subrepresentation(xmlNode* element, DASH_Subrepresentation_t **srep)
{
	HINT8 pBuf[DASH_MPDPARSER_CONST_ATTR_BUFSIZE];
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	DASH_Subrepresentation_t* pSubrep = NULL;
	HUINT8 **split = NULL;
	HUINT32 split_cnt = 0;
	HUINT32 loopcnt = 0;

	VK_MEM_Memset(pBuf, 0x0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);

	*srep = VK_MEM_Alloc(sizeof(DASH_Subrepresentation_t));
	if(*srep == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	VK_MEM_Memset(*srep, 0, sizeof(DASH_Subrepresentation_t));
	pSubrep = *srep;

	DASH_CommonAttributesElements_t* common;

	/* level */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_LEVEL);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		pSubrep->level = (HUINT32)VK_atoi((char*)pBuf);
	}
	else
	{
		pSubrep->level = DASH_INT32_MAX;
	}

	/* dependency level */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_DEPENDENCYLEVEL);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		split_cnt = __dash_split(pBuf, (HINT8*)" ", (HINT8***)&split);
		for(loopcnt = 0; loopcnt < split_cnt; loopcnt++)
		{
			UTIL_DLLIST_Append(&pSubrep->dependencyLevel, split[loopcnt]);
		}
		VK_MEM_Free(split);
		split = NULL;
	}

	/* bandwidth */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_BANDWIDTH);
	if(merr != DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		pSubrep->bandwidth = (HUINT32)VK_atoi((char*)pBuf);
	}
	else
	{
		if(pSubrep->level != (HUINT32)DASH_INT32_MAX)
		{
			return DASH_MPD_ERR_PARSE_ERROR;
		}
	}

	/* contentComponent(attribute) */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_CONTENTCOMPONENT);
	if(merr == DASH_MPD_ERR_OK)
	{
		split_cnt = __dash_split(pBuf, (HINT8*)" ", (HINT8***)&split);
		for(loopcnt = 0; loopcnt < split_cnt; loopcnt++)
		{
			UTIL_DLLIST_Append(&pSubrep->contentComponent, split[loopcnt]);
		}
		VK_MEM_Free(split);
		split = NULL;
	}

	/* commonAttribute */
	merr = P_MPD_Parse_CommonAttributes(element, &common);
	if(merr == DASH_MPD_ERR_OK)
	{
		VK_MEM_Memcpy(&pSubrep->CommonAttributesElements, common, sizeof(DASH_CommonAttributesElements_t));
		VK_MEM_Free(common);
		common = NULL;
	}
	else if(merr == DASH_MPD_ERR_PARSE_ERROR)
	{
		return merr;
	}

	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Parse_Representation(xmlNode* element, DASH_Representation_t **rep)
{
	HINT8 pBuf[DASH_MPDPARSER_CONST_ATTR_BUFSIZE];
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	xmlNode* traverseNode = NULL;
	DASH_Representation_t* pRepresentation = NULL;
	DASH_BaseURL_t* b_url = NULL;
	DASH_SegmentBase_t* segmentbase = NULL;
	DASH_SegmentList_t* segmentlist = NULL;
	DASH_SegmentTemplate_t* segmenttemplate = NULL;
	DASH_Subrepresentation_t* subrep = NULL;
	DASH_CommonAttributesElements_t* common = NULL;
	HUINT8 **split = NULL;
	HUINT32 split_cnt = 0;
	HUINT32 loopcnt = 0;

	VK_MEM_Memset(pBuf, 0x0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);

	*rep = VK_MEM_Alloc(sizeof(DASH_Representation_t));
	if(*rep == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	VK_MEM_Memset(*rep, 0, sizeof(DASH_Representation_t));
	pRepresentation = *rep;

	/* id */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_ID);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&(pRepresentation->id), pBuf);
	}
	else if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		return DASH_MPD_ERR_PARSE_ERROR;
	}

	/* bandwidth */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_BANDWIDTH);
	if(merr == DASH_MPD_ERR_OK)
	{
		pRepresentation->bandwidth = (HUINT32)VK_atoi((char*)pBuf);
	}
	else if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		return DASH_MPD_ERR_PARSE_ERROR;
	}

	/* qualityRanking */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_QUALITYRANKING);
	if(merr == DASH_MPD_ERR_OK)
	{
		pRepresentation->qualityRanking = (HUINT32)VK_atoi((char*)pBuf);
	}

	/* dependencyId */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_DEPENDENCYID);
	if(merr == DASH_MPD_ERR_OK)
	{
		split_cnt = __dash_split(pBuf, (HINT8*)" ", (HINT8***)&split);
		for(loopcnt = 0; loopcnt < split_cnt; loopcnt++)
		{
			UTIL_DLLIST_Append(&pRepresentation->dependencyId,split[loopcnt]);
		}
		VK_MEM_Free(split);
		split = NULL;
	}

	/* mediaStreamStructureId */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_MEDIASTREAMSTRUCTUREID);
	if(merr == DASH_MPD_ERR_OK)
	{
		split_cnt = __dash_split(pBuf, (HINT8*)" ", (HINT8***)&split);
		for(loopcnt = 0; loopcnt < split_cnt; loopcnt++)
		{
			UTIL_DLLIST_Append(&pRepresentation->mediaStreamStructureId, split[loopcnt]);
		}
		VK_MEM_Free(split);
		split = NULL;
	}

	/* commonAttribute */
	merr = P_MPD_Parse_CommonAttributes(element, &common);
	if(merr == DASH_MPD_ERR_OK)
	{
		VK_MEM_Memcpy(&pRepresentation->CommonAttributesElements, common, sizeof(DASH_CommonAttributesElements_t));
		VK_MEM_Free(common);
		common = NULL;
	}
	else if(merr == DASH_MPD_ERR_PARSE_ERROR)
	{
		return merr;
	}

	/* element parsing */
	for (traverseNode = ((xmlNode*)element)->children; traverseNode; traverseNode = traverseNode->next)
	{
		/* baseURL : Optional */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_BASEURL))
		{
			merr = P_MPD_Parse_BaseURL(traverseNode, &b_url);
			if(merr == DASH_MPD_ERR_OK)
			{
				UTIL_DLLIST_Append(&pRepresentation->BaseURL, b_url);
			}
		}

		/* SegmentBase */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_SEGMENTBASE))
		{
			merr = P_MPD_Parse_SegmentBase(traverseNode, &segmentbase);
			if(merr == DASH_MPD_ERR_OK)
			{
				pRepresentation->isSegmentBase = TRUE;
				VK_MEM_Memcpy(&pRepresentation->SegmentBase, segmentbase, sizeof(DASH_SegmentBase_t));
				VK_MEM_Free(segmentbase);
				segmentbase = NULL;
			}
		}

		/* SegmentList */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_SEGMENTLIST))
		{
			merr = P_MPD_Parse_SegmentList(traverseNode, &segmentlist);
			if(merr == DASH_MPD_ERR_OK)
			{
				pRepresentation->isSegmentList = TRUE;
				VK_MEM_Memcpy(&pRepresentation->SegmentList, segmentlist, sizeof(DASH_SegmentList_t));
				VK_MEM_Free(segmentlist);
				segmentlist = NULL;
			}
		}

		/* SegmentTemplate */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_SEGMENTTEMPLATE))
		{
			merr = P_MPD_Parse_SegmentTemplate(traverseNode, &segmenttemplate);
			if(merr == DASH_MPD_ERR_OK)
			{
				pRepresentation->isSegmentTemplate = TRUE;
				VK_MEM_Memcpy(&pRepresentation->SegmentTemplate, segmenttemplate, sizeof(DASH_SegmentTemplate_t));
				VK_MEM_Free(segmenttemplate);
				segmenttemplate = NULL;
			}
		}

		/* Subrepresentation */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_SUBREPRESENTATION))
		{
			merr = P_MPD_Parse_Subrepresentation(traverseNode, &subrep);
			if(merr == DASH_MPD_ERR_OK)
			{
				UTIL_DLLIST_Append(&pRepresentation->SubRepresentation, subrep);
			}
		}

	}
	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Parse_AdaptationSet(xmlNode* element, DASH_AdaptationSet_t **adap)
{
	HINT8 pBuf[DASH_MPDPARSER_CONST_ATTR_BUFSIZE];
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	xmlNode* traverseNode = NULL;
	DASH_AdaptationSet_t* pAdaptation = NULL;
	DASH_BaseURL_t* b_url = NULL;
	DASH_SegmentBase_t* segmentbase = NULL;
	DASH_SegmentList_t* segmentlist = NULL;
	DASH_SegmentTemplate_t* segmenttemplate = NULL;
	DASH_ContentComponent_t* component = NULL;
	DASH_Representation_t* representation = NULL;
	DASH_DESCRIPTOR_t* desc = NULL;
	DASH_CommonAttributesElements_t* common = NULL;

	VK_MEM_Memset(pBuf, 0x0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);

	*adap = VK_MEM_Alloc(sizeof(DASH_AdaptationSet_t));
	if(*adap == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	VK_MEM_Memset(*adap, 0, sizeof(DASH_AdaptationSet_t));
	pAdaptation = *adap;

	/* xlink:href */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_XLINK_HREF);
	if(merr == DASH_MPD_ERR_OK)
	{
		PrintError("currently, xlink is not supported\n");
		return DASH_MPD_ERR_ERROR;
#if 0
		__dash_strdup(&(pAdaptation->xlink.href), pBuf);
		pAdaptation->isXlink = TRUE;
#endif
	}

	/* xlink:actuate */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_XLINK_ACTUATE);
	if(merr == DASH_MPD_ERR_OK)
	{
		if(!VK_strncmp((char*)pBuf, "OnLoad", 6))
		{
			pAdaptation->xlink.eActuate = DASH_XLINK_ACTUATE_ONLOAD;
		}
		else if(!VK_strncmp((char*)pBuf, "OnRequest", 9))
		{
			pAdaptation->xlink.eActuate = DASH_XLINK_ACTUATE_ONREQUEST;
		}
	}
	else if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		pAdaptation->xlink.eActuate = DASH_XLINK_ACTUATE_ONREQUEST;
	}

	/* id */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_ID);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&pAdaptation->id, pBuf);
	}

	/* group */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_GROUP);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&pAdaptation->group, pBuf);
	}

	/* lang */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_LANG);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&pAdaptation->lang, pBuf);

		if(!VK_strncmp(pAdaptation->lang.str, "en", 2))
		{
			pAdaptation->localeType = DASH_LOCALE_TYPE_EN;
		}
		else if(!VK_strncmp(pAdaptation->lang.str, "kr", 2))
		{
			pAdaptation->localeType = DASH_LOCALE_TYPE_KR;
		}
		else if(!VK_strncmp(pAdaptation->lang.str, "fr", 2))
		{
			pAdaptation->localeType = DASH_LOCALE_TYPE_FR;
		}
		else
		{
			pAdaptation->localeType = DASH_LOCALE_TYPE_NONE;
		}
	}

	/* contentType */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_CONTENTTYPE);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&pAdaptation->contentType, pBuf);
	}

	/* par */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_PAR);
	if(merr == DASH_MPD_ERR_OK)
	{
		if(!VK_strncmp("16:9",(char*)pBuf,4))
		{
			pAdaptation->par = DASH_PAR_16_9;
		}
		else if(!VK_strncmp("4:3",(char*)pBuf,4))
		{
			pAdaptation->par = DASH_PAR_4_3;
		}
	}

	/* minBandwidth */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_MINBANDWIDTH);
	if(merr == DASH_MPD_ERR_OK)
	{
		pAdaptation->minBandwidth = (HUINT32)VK_atoi((char*)pBuf);
	}

	/* maxBandwidth */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_MAXBANDWIDTH);
	if(merr == DASH_MPD_ERR_OK)
	{
		pAdaptation->maxBandwidth = (HUINT32)VK_atoi((char*)pBuf);
	}

	/* minWidth */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_MINWIDTH);
	if(merr == DASH_MPD_ERR_OK)
	{
		pAdaptation->minWidth = (HUINT32)VK_atoi((char*)pBuf);
	}

	/* maxWidth */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_MAXWIDTH);
	if(merr == DASH_MPD_ERR_OK)
	{
		pAdaptation->maxWidth = (HUINT32)VK_atoi((char*)pBuf);
	}

	/* minHeight */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_MINHEIGHT);
	if(merr == DASH_MPD_ERR_OK)
	{
		pAdaptation->minHeight = (HUINT32)VK_atoi((char*)pBuf);
	}

	/* maxHeight */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_MAXHEIGHT);
	if(merr == DASH_MPD_ERR_OK)
	{
		pAdaptation->maxHeight = (HUINT32)VK_atoi((char*)pBuf);
	}

	/* minFrameRate */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_MINFRAMERATE);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&pAdaptation->minFrameRate, pBuf);
	}

	/* maxFrameRate */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_MAXFRAMERATE);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&pAdaptation->maxFrameRate, pBuf);
	}

	/* segmentAlignment */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_SEGMENTALIGNMENT);
	if(merr == DASH_MPD_ERR_OK)
	{
		if(!VK_strncmp("true", (char*)pBuf, 4))
		{
			pAdaptation->segmentAlignment = TRUE;
		}
		else
		{
			pAdaptation->segmentAlignment = FALSE;
		}
	}
	else if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		pAdaptation->segmentAlignment = FALSE;
	}

	/* bitstreamSwitching */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_BITSTREAMSWITCHING);
	if(merr == DASH_MPD_ERR_OK)
	{
		if(!VK_strncmp("true", (char*)pBuf, 4))
		{
			pAdaptation->bitstreamSwitching = TRUE;
		}
		else
		{
			pAdaptation->bitstreamSwitching = FALSE;
		}
	}
	else if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		pAdaptation->bitstreamSwitching = FALSE;
	}

	/* subsegmentAlignment */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_SUBSEGMENTALIGNMENT);
	if(merr == DASH_MPD_ERR_OK)
	{
		if(!VK_strncmp("true", (char*)pBuf, 4))
		{
			pAdaptation->subsegmentAlignment = TRUE;
		}
		else
		{
			pAdaptation->subsegmentAlignment = FALSE;
		}
	}
	else if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		pAdaptation->subsegmentAlignment = FALSE;
	}

	/* subsegmentStartsWithSAP */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_SUBSEGMENTSTARTSWITHSAP);
	if(merr == DASH_MPD_ERR_OK)
	{
		pAdaptation->subsegmentStartsWithSAP = (HUINT32)VK_atoi((char*)pBuf);
	}
	else if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		pAdaptation->subsegmentStartsWithSAP = 0;
	}

	/* commonAttribute */
	merr = P_MPD_Parse_CommonAttributes(element, &common);
	if(merr == DASH_MPD_ERR_OK)
	{
		VK_MEM_Memcpy(&pAdaptation->CommonAttributesElements, common, sizeof(DASH_CommonAttributesElements_t));
		if(common->mimeType.str != NULL)
		{
			if(!VK_strncmp((char*)common->mimeType.str, "video", 5))
			{
				pAdaptation->trackType = DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY;
			}
			else if(!VK_strncmp((char*)common->mimeType.str, "audio", 5))
			{
				pAdaptation->trackType = DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY;
			}
			else if(!VK_strncmp((char*)common->mimeType.str, "application/ttml+xml",20))
			{
				pAdaptation->trackType = DASH_ADAPTATIONSET_TYPE_TEXT;
			}
		}
		else
		{
			//return DASH_MPD_ERR_PARSE_ERROR;
			//pAdaptation->trackType = DASH_ADAPTATIONSET_TYPE_AV;
		}
		VK_MEM_Free(common);
		common = NULL;
	}
	else if(merr == DASH_MPD_ERR_PARSE_ERROR)
	{
		return merr;
	}

	/* element parsing */
	for (traverseNode = ((xmlNode*)element)->children; traverseNode; traverseNode = traverseNode->next)
	{
		/* baseURL : Optional */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_BASEURL))
		{
			merr = P_MPD_Parse_BaseURL(traverseNode, &b_url);
			if(merr == DASH_MPD_ERR_OK)
			{
				UTIL_DLLIST_Append(&pAdaptation->BaseURL, b_url);
			}
		}

		/* SegmentBase */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_SEGMENTBASE))
		{
			merr = P_MPD_Parse_SegmentBase(traverseNode, &segmentbase);
			if(merr == DASH_MPD_ERR_OK)
			{
				pAdaptation->isSegmentBase = TRUE;
				VK_MEM_Memcpy(&pAdaptation->SegmentBase, segmentbase, sizeof(DASH_SegmentBase_t));
				VK_MEM_Free(segmentbase);
				segmentbase = NULL;
			}
		}

		/* SegmentList */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_SEGMENTLIST))
		{
			merr = P_MPD_Parse_SegmentList(traverseNode, &segmentlist);
			if(merr == DASH_MPD_ERR_OK)
			{
				pAdaptation->isSegmentList = TRUE;
				VK_MEM_Memcpy(&pAdaptation->SegmentList, segmentlist, sizeof(DASH_SegmentList_t));
				VK_MEM_Free(segmentlist);
				segmentlist = NULL;
			}
		}

		/* SegmentTemplate */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_SEGMENTTEMPLATE))
		{
			merr = P_MPD_Parse_SegmentTemplate(traverseNode, &segmenttemplate);
			if(merr == DASH_MPD_ERR_OK)
			{
				pAdaptation->isSegmentTemplate = TRUE;
				VK_MEM_Memcpy(&pAdaptation->SegmentTemplate, segmenttemplate, sizeof(DASH_SegmentTemplate_t));
				VK_MEM_Free(segmenttemplate);
				segmenttemplate = NULL;
			}
		}
		/* ContentComponent */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_CONTENTCOMPONENT))
		{
			merr = P_MPD_Parse_ContentComponent(traverseNode, &component);
			if(merr == DASH_MPD_ERR_OK)
			{
				UTIL_DLLIST_Append(&pAdaptation->ContentComponent, component);
			}
		}

		if(UTIL_DLLIST_Size(&pAdaptation->ContentComponent) > 0)
		{
			pAdaptation->trackType = DASH_ADAPTATIONSET_TYPE_AV;
		}

		/* Accessibility, Role, Rating, Viewpoint */
		if( !xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_ACCESSIBILITY) ||
		    !xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_ROLE) ||
			!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_RATING)  ||
			!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_VIEWPOINT))
		{
			desc = VK_MEM_Alloc(sizeof(DASH_DESCRIPTOR_t));
			if(desc == NULL)
			{
				PrintError("Not enough memory\n");
				return DASH_MPD_ERR_ERROR;
			}

			merr = P_MPD_Get_Attribute(traverseNode, pBuf, (xmlChar*)DASH_ATTR_SCHEMEIDURI);
			if(merr == DASH_MPD_ERR_OK)
			{
				__dash_strdup(&desc->schemeIdUri, pBuf);
			}
			else
			{
				return DASH_MPD_ERR_PARSE_ERROR;
			}

			merr = P_MPD_Get_Attribute(traverseNode, pBuf, (xmlChar*)DASH_ATTR_VALUE);
			if(merr == DASH_MPD_ERR_OK)
			{
				__dash_strdup(&desc->value, pBuf);
			}

			if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_ACCESSIBILITY))
			{
				UTIL_DLLIST_Append(&pAdaptation->Accessibility, desc);
			}
			else if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_ROLE))
			{
				UTIL_DLLIST_Append(&pAdaptation->Role, desc);
			}
			else if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_RATING))
			{
				UTIL_DLLIST_Append(&pAdaptation->Rating, desc);
			}
			else if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_VIEWPOINT))
			{
				UTIL_DLLIST_Append(&pAdaptation->ViewPoint, desc);
			}
		}

		/* Representation */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_REPRESENTATION))
		{
			merr = P_MPD_Parse_Representation(traverseNode, &representation);
			if(merr == DASH_MPD_ERR_OK)
			{
				UTIL_DLLIST_Append(&pAdaptation->Representation, representation);
			}
		}
	}

	// content component is to describe a multi stream in one file
	if(UTIL_DLLIST_Size(&pAdaptation->ContentComponent) > 1)
	{
		pAdaptation->trackType = DASH_ADAPTATIONSET_TYPE_AV;
	}
	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Parse_Period(xmlNode* element, DASH_Period_t** per)
{
	xmlNode* traverseNode = NULL;
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	HINT8 pBuf[DASH_MPDPARSER_CONST_ATTR_BUFSIZE];
	DASH_Period_t* period = NULL;
	DASH_BaseURL_t* b_url = NULL;
	DASH_SegmentBase_t* segmentbase = NULL;
	DASH_SegmentList_t* segmentlist = NULL;
	DASH_SegmentTemplate_t* segmenttemplate = NULL;
	DASH_AdaptationSet_t* adaptation = NULL;

	VK_MEM_Memset(pBuf, 0x0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);

	*per = VK_MEM_Alloc(sizeof(DASH_Period_t));
	if(*per == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	VK_MEM_Memset(*per, 0, sizeof(DASH_Period_t));
	period = *per;

	/* xlink:href */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_XLINK_HREF);
	if(merr == DASH_MPD_ERR_OK)
	{
		PrintError("currently, xlink is not supported\n");
		return DASH_MPD_ERR_ERROR;
#if 0
		__dash_strdup(&(period->xlink.href), pBuf);
		period->isXlink = TRUE;
#endif
	}

	/* xlink:actuate */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_XLINK_ACTUATE);
	if(merr == DASH_MPD_ERR_OK)
	{
		if(!VK_strncmp((char*)pBuf, "OnLoad", 6))
		{
			period->xlink.eActuate = DASH_XLINK_ACTUATE_ONLOAD;
		}
		else if(!VK_strncmp((char*)pBuf, "onRequest", 9))
		{
			period->xlink.eActuate = DASH_XLINK_ACTUATE_ONREQUEST;
		}
	}
	else if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		period->xlink.eActuate = DASH_XLINK_ACTUATE_ONREQUEST;
	}

	/* id */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_ID);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&period->id, pBuf);
	}
	/* start */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_START);
	if(merr == DASH_MPD_ERR_OK)
	{
		period->start = (DASH_WALLCLOCK_t)P_MPD_ConvertTimeToMS((char*)pBuf, VK_strlen((char*)pBuf));
	}
	else if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		period->start = DASH_WALLCLK_NOT_EXIST;
	}

	/* duration */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_DURATION);
	if(merr == DASH_MPD_ERR_OK)
	{
		period->duration = (DASH_WALLCLOCK_t)P_MPD_ConvertTimeToMS((char*)pBuf, VK_strlen((char*)pBuf));
	}
	else if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		period->duration = DASH_WALLCLK_NOT_EXIST;
	}

	/* bitstreamswitching */
	merr = P_MPD_Get_Attribute(element, pBuf, (xmlChar*)DASH_ATTR_BITSTREAMSWITCHING);
	if(merr == DASH_MPD_ERR_OK)
	{
		if( !VK_strncmp("true",(char*)pBuf, 4) )
		{
			period->bitstreamSwitching = TRUE;
		}
		else
		{
			period->bitstreamSwitching = FALSE;
		}
	}
	else if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		period->bitstreamSwitching = FALSE;
	}

	/* element parsing */
	for (traverseNode = ((xmlNode*)element)->children; traverseNode; traverseNode = traverseNode->next)
	{
		/* baseURL : Optional */
		// TODO: baseURL에 attribute가 존재함. 이 부분에 대한 추가 구현 필요할지도
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_BASEURL))
		{
			period->isBaseURL = TRUE;
			merr = P_MPD_Parse_BaseURL(traverseNode, &b_url);
			if(merr == DASH_MPD_ERR_OK)
			{
				UTIL_DLLIST_Append(&period->BaseURL, b_url);
			}
		}

		/* SegmentBase */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_SEGMENTBASE))
		{
			merr = P_MPD_Parse_SegmentBase(traverseNode, &segmentbase);
			if(merr == DASH_MPD_ERR_OK)
			{
				period->isSegmentBase = TRUE;
				VK_MEM_Memcpy(&period->SegmentBase, segmentbase, sizeof(DASH_SegmentBase_t));
				VK_MEM_Free(segmentbase);
				segmentbase = NULL;
			}
		}

		/* SegmentList */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_SEGMENTLIST))
		{
			merr = P_MPD_Parse_SegmentList(traverseNode, &segmentlist);
			if(merr == DASH_MPD_ERR_OK)
			{
				period->isSegmentList = TRUE;
				VK_MEM_Memcpy(&period->SegmentList, segmentlist, sizeof(DASH_SegmentList_t));
				VK_MEM_Free(segmentlist);
				segmentlist = NULL;
			}
		}

		/* SegmentTemplate */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_SEGMENTTEMPLATE))
		{
			merr = P_MPD_Parse_SegmentTemplate(traverseNode, &segmenttemplate);
			if(merr == DASH_MPD_ERR_OK)
			{
				period->isSegmentTemplate = TRUE;
				VK_MEM_Memcpy(&period->SegmentTemplate, segmenttemplate, sizeof(DASH_SegmentTemplate_t));
				VK_MEM_Free(segmenttemplate);
				segmenttemplate = NULL;
			}
		}

		/* AdaptationSet */
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_ADAPTATIONSET))
		{
			merr = P_MPD_Parse_AdaptationSet(traverseNode, &adaptation);
			if(merr == DASH_MPD_ERR_OK)
			{
				UTIL_DLLIST_Append(&period->AdaptationSet, adaptation);
			}
			else if(merr == DASH_MPD_ERR_PARSE_ERROR)
			{
				return merr;
			}
		}
	}
	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Parse_MPD(DASH_MPD_t* mpd, void* root)
{
	xmlNode *traverseNode = NULL;
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	HINT8 szBuf[DASH_MPDPARSER_CONST_ATTR_BUFSIZE];
	DASH_ProgramInformation_t *pi = NULL;
	DASH_Location_t*loc = NULL;
	DASH_BaseURL_t *b_url = NULL;
	DASH_Metrics_t *metrics = NULL;
	DASH_Period_t *period = NULL;
	HUINT32 nY = 0;
	HUINT32 nM = 0;
	HUINT32 nD = 0;
	HUINT32 nHour = 0;
	HUINT32 nMin = 0;
	HUINT32 nSec = 0;
	struct tm tmDate;

	/* get timescale */
	mpd->timeScale = P_MPD_FindTimeScale(root);

	VK_MEM_Memset(szBuf, 0x0, DASH_MPDPARSER_CONST_ATTR_BUFSIZE);
	/* print xml tree */
	//_traverse_document(root, 0);

	/* id : optional */
	merr = P_MPD_Get_Attribute(root, szBuf, (xmlChar*)DASH_ATTR_ID);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&mpd->id, szBuf);
	}

	/* profiles : optional */
	merr = P_MPD_Get_Attribute(root, szBuf, (xmlChar*)DASH_ATTR_PROFILE);
	if(merr == DASH_MPD_ERR_OK)
	{
		__dash_strdup(&mpd->profiles, szBuf);
	}

	/* type : default : static */
	merr = P_MPD_Get_Attribute(root, szBuf, (xmlChar*)DASH_ATTR_TYPE);
	if(merr == DASH_MPD_ERR_OK)
	{
		if(NULL != VK_strstr((char *)szBuf,"dynamic"))
		{
			mpd->type = DASH_MPD_TYPE_DYNAMIC;
		}
		else if(NULL != VK_strstr((char *)szBuf,"static") || merr == DASH_MPD_ERR_ATTR_NOTFOUND)
		{
			mpd->type = DASH_MPD_TYPE_STATIC;
		}
		else
		{
			PrintError("%s(%d) Error : Attribute [type] in MPD is invalid\n", __FUNCTION__, __LINE__);
			return DASH_MPD_ERR_ERROR;
		}
	}

	/* availabilityStartTime : Conditional mandatory */
	merr = P_MPD_Get_Attribute(root, szBuf, (xmlChar*)DASH_ATTR_AVAIL_START_TIME);
	if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		if(mpd->type == DASH_MPD_TYPE_DYNAMIC)
		{
			PrintError("%s(%d) Error : Attribute [type] is Dynamic and [availabilityStartTime] is absent\n", __FUNCTION__, __LINE__);
			return DASH_MPD_ERR_PARSE_ERROR;
		}
		else if(mpd->type == DASH_MPD_TYPE_STATIC)
		{
			mpd->availibilityStartTime = 0ULL;
		}
	}
	else if(merr == DASH_MPD_ERR_OK)
	{
		//mpd->availibilityStartTime = _atoui64(szBuf);
		VK_sscanf((char*)szBuf, "%d-%d-%dT%d:%d:%d", (int*)&nY, (int*)&nM, (int*)&nD, (int*)&nHour, (int*)&nMin, (int*)&nSec);
		VK_MEM_Memset(&tmDate, 0x0, sizeof(tmDate));
		tmDate.tm_year = nY - 1900;
		tmDate.tm_mon = nM - 1;
		tmDate.tm_mday = nD;
		tmDate.tm_hour = nHour;
		tmDate.tm_min = nMin;
		tmDate.tm_sec = nSec;
		mpd->availibilityStartTime = mktime(&tmDate);
	}

	/* availabilityEndTime : Optional */
	merr = P_MPD_Get_Attribute(root, szBuf, (xmlChar*)DASH_ATTR_AVAIL_END_TIME);
	if(merr == DASH_MPD_ERR_OK)
	{
		mpd->availibilityEndTime = _atoui64((char*)szBuf);
	}
	else if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		/* [note] in this case, 0ULL means "unknown".
		   this could be wrong behavior, focus on it */
		mpd->availibilityEndTime = 0ULL;
	}

	/* mediaPresentationDuration : Conditional mendatory */
	merr = P_MPD_Get_Attribute(root, szBuf, (xmlChar*)DASH_ATTR_MEDIA_PRESENTATION_DURATION);
	if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		if(mpd->type == DASH_MPD_TYPE_STATIC)
		{
			PrintError("%s(%d) Error : Attribute [type] is Static and [mediaPresentationDuration] is absent\n", __FUNCTION__, __LINE__);
			return DASH_MPD_ERR_PARSE_ERROR;
		}
		else if(mpd->type == DASH_MPD_TYPE_DYNAMIC)
		{
			/* [note] in this case, 0ULL means "unknown".
		   this could be wrong behavior, focus on it */
			mpd->mediaPresentationDuration = 0ULL;
		}
	}
	else if(merr == DASH_MPD_ERR_OK)
	{
		mpd->mediaPresentationDuration =
			(DASH_WALLCLOCK_t)P_MPD_ConvertTime((char*)szBuf, VK_strlen((char*)szBuf), mpd->timeScale);
	}

	/* minimumUpdatePeriod : Optional */
	merr = P_MPD_Get_Attribute(root, szBuf, (xmlChar*)DASH_ATTR_MINIMUM_UPDATE_PERIOD);
	if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		if(mpd->type == DASH_MPD_TYPE_DYNAMIC)
		{
			/* if "minimumUpdatePeriod" is absent and the attribute "type" is dynamic,
			   mpd is not updated although the mpd is dynamic. so we can regard it as static type */
			mpd->type = DASH_MPD_TYPE_STATIC;
		}
		mpd->minimumUpdatePeriod = 0U;
	}
	else if(merr == DASH_MPD_ERR_OK)
	{
		if(mpd->type == DASH_MPD_TYPE_STATIC)
		{
			PrintError("%s(%d) Error : Attribute [type] is Static and [minimumUpdatePeriod] should be absent\n", __FUNCTION__, __LINE__);
			return DASH_MPD_ERR_PARSE_ERROR;
		}
		mpd->minimumUpdatePeriod = P_MPD_ConvertTime((char*)szBuf, VK_strlen((char*)szBuf), mpd->timeScale);
	}

	/* minBufferTime : Mendatory */
	merr = P_MPD_Get_Attribute(root, szBuf, (xmlChar*)DASH_ATTR_MIN_BUFFER_TIME);
	if(merr == DASH_MPD_ERR_OK)
	{
		mpd->minBufferTime = (DASH_TIME_t)P_MPD_ConvertTime((char*)szBuf, VK_strlen((char*)szBuf), mpd->timeScale);
	}
	else if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		PrintError("%s(%d) Error : Attribute [minBufferTime] must be present\n", __FUNCTION__, __LINE__);
		return DASH_MPD_ERR_PARSE_ERROR;
	}

	/* timeShiftBufferDepth : Optional */
	merr = P_MPD_Get_Attribute(root, szBuf, (xmlChar*)DASH_ATTR_TIMESHIFT_BUFFER_DEPTH);
	if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		if(mpd->type == DASH_MPD_TYPE_DYNAMIC)
		{
			/* if [type] is dynamic and [timeShiftBufferDepth] is abasent, it means timeShiftBufferDepth is infinite */
			mpd->timeShiftBufferDepth = DASH_INT64_MAX;
		}
		else if(mpd->type == DASH_MPD_TYPE_STATIC)
		{
			/* if [type] is static, [timeShiftBufferDepth] is undefined */
			mpd->timeShiftBufferDepth = DASH_INT64_MIN;
		}
	}
	else
	{
		if(mpd->type == DASH_MPD_TYPE_DYNAMIC)
		{
			mpd->timeShiftBufferDepth = (DASH_TIME_t)VK_atoi((char*)szBuf);
		}
		else if(mpd->type == DASH_MPD_TYPE_STATIC)
		{
			/* if [type] is static, [timeShiftBufferDepth] is undefined */
			mpd->timeShiftBufferDepth = 0U;
		}
	}

	/* suggestedPresentationDelay : Optional */
	merr = P_MPD_Get_Attribute(root, szBuf, (xmlChar*)DASH_ATTR_SUGGESTED_PRESENTATION_DELAY);
	if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		/* if [type] is static, [suggestedPresentationDelay] is undefined and may be ignored */
		/* if [type] is dynamic, not specified in spec */
		mpd->suggestedPresentationDelay = DASH_INT64_MIN;
	}
	else if(merr == DASH_MPD_ERR_OK)
	{
		if(mpd->type == DASH_MPD_TYPE_DYNAMIC)
		{
			mpd->suggestedPresentationDelay = (DASH_TIME_t)VK_atoi((char*)szBuf);
		}
		else if(mpd->type == DASH_MPD_TYPE_STATIC)
		{
			mpd->suggestedPresentationDelay = DASH_INT64_MIN;
		}
	}

	/* maxSegmentDuration : Optional */
	merr = P_MPD_Get_Attribute(root, szBuf, (xmlChar*)DASH_ATTR_MAX_SEGMENT_DURATION);
	if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		/* if attribute [maxSegmentDuration] is absent, should be the maximum duration of any segment in this document */
		/* so this value must fill even though not this time */
		mpd->maxSegmentDuration = DASH_INT64_MIN;
	}
	else if(merr == DASH_MPD_ERR_OK)
	{
		mpd->maxSegmentDuration = (DASH_TIME_t)VK_atoi((char*)szBuf);
	}

	/* maxSubsegmentDuration : Optional */
	merr = P_MPD_Get_Attribute(root, szBuf, (xmlChar*)DASH_ATTR_MAX_SUBSEGMENT_DURATION);
	if(merr == DASH_MPD_ERR_ATTR_NOTFOUND)
	{
		/* if attribute [maxSubegmentDuration] is absent, it imply that this value is the same as [maxSegmentDuration] */
		/* so this value must fill even though not this time */
		mpd->maxSubsegmentDuration = mpd->maxSegmentDuration;
	}
	else if(merr == DASH_MPD_ERR_OK)
	{
		mpd->maxSubsegmentDuration = (DASH_TIME_t)VK_atoi((char*)szBuf);
	}

	/* Element parsing */
	for (traverseNode = ((xmlNode*)root)->children; traverseNode; traverseNode = traverseNode->next)
	{
		/* baseURL : Optional */
		// TODO: baseURL에 attribute가 존재함. 이 부분에 대한 추가 구현 필요할지도
		if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_BASEURL))
		{
			merr = P_MPD_Parse_BaseURL(traverseNode, &b_url);
			if(merr == DASH_MPD_ERR_OK)
			{
				UTIL_DLLIST_Append(&mpd->BaseURL, b_url);
			}
		}
		/* Location : Optional(0~N) */
		else if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_LOCATION))
		{
			merr = P_MPD_Parse_Location(traverseNode, &loc);
			if(merr == DASH_MPD_ERR_OK)
			{
				UTIL_DLLIST_Append(&mpd->Location, loc);
			}
		}
		/* Metrics : Optional(0~N) */
		else if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_METRICS))
		{
			merr = P_MPD_Parse_Metrics(traverseNode, &metrics);
			if(merr == DASH_MPD_ERR_OK)
			{
				UTIL_DLLIST_Append(&mpd->Metrics, (xmlChar*)metrics);
			}
		}
		/* Program information : Optional(0~N) */
		else if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_PROGRAMINFORMATION))
		{
			merr = P_MPD_Parse_ProgramInformation(traverseNode, &pi);
			if(merr == DASH_MPD_ERR_OK)
			{
				UTIL_DLLIST_Append(&mpd->ProgramInformation, pi);
			}
		}
		else if(!xmlStrcmp(traverseNode->name, (xmlChar*)DASH_TAG_PERIOD))
		{
			merr = P_MPD_Parse_Period(traverseNode, &period);
			if(merr == DASH_MPD_ERR_OK)
			{
				UTIL_DLLIST_Append(&mpd->Period, period);
			}
		}
	}

	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Merge_SegmentList(DASH_SegmentList_t* pOrg, DASH_SegmentList_t* pSrc)
{
	UTIL_DLLNode_t* pOrgList = NULL;
	UTIL_DLLNode_t* pSrcList = NULL;
	DASH_URLTYPE_t* pOrgUrl = NULL;
	DASH_URLTYPE_t* pSrcUrl = NULL;
	HUINT32 loopcnt = 0;
	HUINT32 nrOrgSegmentUrl = 0;
	HUINT32 nrSrcSegmentUrl = 0;

	nrOrgSegmentUrl = UTIL_DLLIST_Size(&pOrg->SegmentUrl);
	nrSrcSegmentUrl = UTIL_DLLIST_Size(&pSrc->SegmentUrl);
	pOrgList = UTIL_DLLIST_GetTailNode(&pOrg->SegmentUrl);
	pOrgUrl = pOrgList->pData;

	/* find last node of original MPD segment in Source */
	for(loopcnt = 0; loopcnt < nrSrcSegmentUrl; loopcnt++)
	{
		pSrcList = UTIL_DLLIST_Get_n_Node(&pSrc->SegmentUrl, loopcnt);
		pSrcUrl = pSrcList->pData;

		if(pOrgUrl->sourceURL.str != NULL, pSrcUrl->sourceURL.str != NULL)
		{
			if(!VK_strcmp(pOrgUrl->sourceURL.str, pSrcUrl->sourceURL.str))
			{
				break;
			}
		}
	}
	/* this means there aren't anything that need to be merged */
	if(loopcnt == nrSrcSegmentUrl)
	{
		return DASH_MPD_ERR_MERGE_NONE;
	}
	for(loopcnt = loopcnt+1; loopcnt < nrSrcSegmentUrl; loopcnt++)
	{
		pSrcList = UTIL_DLLIST_Get_n_Node(&pSrc->SegmentUrl, loopcnt);
		pSrcUrl = pSrcList->pData;
		pSrcList->pData = NULL;
		UTIL_DLLIST_Append(&pOrg->SegmentUrl, pSrcUrl);
	}

	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Merge_Representation(DASH_Representation_t* pOrg, DASH_Representation_t* pSrc)
{
	DASH_MPD_ERROR_e merr;
	UTIL_DLLNode_t* pSrcList = NULL;
	HUINT32 loopcnt = 0;
	HUINT32 nrSrcSubRep = 0;

	/* Segment list merge */
	if(pOrg->isSegmentList == TRUE && pSrc->isSegmentList == TRUE)
	{
		merr = P_MPD_Merge_SegmentList(&pOrg->SegmentList, &pSrc->SegmentList);
		if(merr == DASH_MPD_ERR_MERGE_ERROR)
		{
			PrintError("%s(%d) Error (P_MPD_Merge_SegmentList)\n", __FUNCTION__, __LINE__);
			return merr;
		}
	}
	/* remove source data */
	for(loopcnt = 0; loopcnt < nrSrcSubRep; loopcnt++)
	{
		pSrcList = UTIL_DLLIST_Get_n_Node(&pSrc->SubRepresentation, loopcnt);
		pSrcList->pData = NULL;
	}
	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Merge_AdaptationSet(DASH_AdaptationSet_t* pOrg, DASH_AdaptationSet_t* pSrc)
{
	DASH_MPD_ERROR_e merr;
	UTIL_DLLNode_t* pSrcList = NULL;
	UTIL_DLLNode_t* pOrgList = NULL;
	DASH_Representation_t* pSrcRep= NULL;
	DASH_Representation_t* pOrgRep = NULL;
	HUINT32 loopcnt = 0;
	HUINT32 subloopcnt = 0;
	HUINT32 nrOrgRep = 0;
	HUINT32 nrSrcRep = 0;

	/* Segment list merge */
	if(pOrg->isSegmentList == TRUE && pSrc->isSegmentList == TRUE)
	{
		merr = P_MPD_Merge_SegmentList(&pOrg->SegmentList, &pSrc->SegmentList);
		if(merr == DASH_MPD_ERR_MERGE_ERROR)
		{
			PrintError("%s(%d) Error (P_MPD_Merge_SegmentList)\n", __FUNCTION__, __LINE__);
			return merr;
		}
	}
	nrOrgRep = UTIL_DLLIST_Size(&pOrg->Representation);
	nrSrcRep = UTIL_DLLIST_Size(&pSrc->Representation);
	/* Present adaptation set merge */
	for(loopcnt = 0; loopcnt < nrOrgRep; loopcnt++)
	{
		pOrgList = UTIL_DLLIST_Get_n_Node(&pOrg->Representation, loopcnt);
		pOrgRep = pOrgList->pData;

		for(subloopcnt = 0; subloopcnt < nrSrcRep; subloopcnt++)
		{
			pSrcList = UTIL_DLLIST_Get_n_Node(&pSrc->Representation, loopcnt);
			pSrcRep = pSrcList->pData;
			/* default equivalent check method */
			if(pOrgRep->id.str != NULL && pSrcRep->id.str != NULL)
			{
				if(!VK_strcmp(pOrgRep->id.str, pSrcRep->id.str))
				{
					/* try to merge representation */
					merr = P_MPD_Merge_Representation(pOrgRep, pSrcRep);
					if(merr == DASH_MPD_ERR_MERGE_ERROR)
					{
						PrintError("%s(%d) Error (P_MPD_Merge_Representation)\n", __FUNCTION__, __LINE__);
						return merr;
					}
					else if(merr == DASH_MPD_ERR_MERGE_NONE)
					{
						continue;
					}
				}
			}
		}
	}
	/* append new period */
	pOrgList = UTIL_DLLIST_GetTailNode(&pOrg->Representation);
	pOrgRep = pOrgList->pData;
	for(loopcnt = 0; loopcnt < UTIL_DLLIST_Size(&pSrc->Representation); loopcnt++)
	{
		pSrcList = UTIL_DLLIST_Get_n_Node(&pSrc->Representation, loopcnt);
		pSrcRep = pSrcList->pData;
		if(pOrgRep->id.str != NULL && pSrcRep->id.str != NULL)
		{
			if(!VK_strcmp(pOrgRep->id.str, pSrcRep->id.str))
			{
				break;
			}
		}
	}
	if(loopcnt == nrSrcRep)
	{
		PrintDebug("%s(%d) Cannot decision that two mpd is the same mpd\n", __FUNCTION__, __LINE__);
		return DASH_MPD_ERR_MERGE_ERROR;
	}
	for(loopcnt = loopcnt+1; loopcnt < nrSrcRep; loopcnt++)
	{
		pSrcList = UTIL_DLLIST_Get_n_Node(&pSrc->Representation, loopcnt);
		pSrcRep = pSrcList->pData;
		pSrcList->pData = NULL;
		UTIL_DLLIST_Append(&pOrg->Representation, pSrcRep);
	}

	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Merge_Period(DASH_Period_t* pOrg, DASH_Period_t* pSrc)
{
	DASH_MPD_ERROR_e merr;
	UTIL_DLLNode_t* pSrcList = NULL;
	UTIL_DLLNode_t* pOrgList = NULL;
	DASH_AdaptationSet_t* pSrcAdaptation= NULL;
	DASH_AdaptationSet_t* pOrgAdaptation = NULL;
	HUINT32 loopcnt = 0;
	HUINT32 subloopcnt = 0;
	HUINT32 nrOrgAdaptation = 0;
	HUINT32 nrSrcAdaptation = 0;

	/* Segment list merge */
	if(pOrg->isSegmentList == TRUE && pSrc->isSegmentList == TRUE)
	{
		merr = P_MPD_Merge_SegmentList(&pOrg->SegmentList, &pSrc->SegmentList);
		if(merr == DASH_MPD_ERR_MERGE_ERROR)
		{
			PrintError("%s(%d) Error (ret of P_MPD_Merge_SegmentList)\n", __FUNCTION__, __LINE__);
			return merr;
		}
	}
	nrOrgAdaptation = UTIL_DLLIST_Size(&pOrg->AdaptationSet);
	nrSrcAdaptation = UTIL_DLLIST_Size(&pSrc->AdaptationSet);
	/* Present adaptation set merge */
	for(loopcnt = 0; loopcnt < nrOrgAdaptation; loopcnt++)
	{
		pOrgList = UTIL_DLLIST_Get_n_Node(&pOrg->AdaptationSet, loopcnt);
		pOrgAdaptation = pOrgList->pData;

		for(subloopcnt = 0; subloopcnt < nrSrcAdaptation; subloopcnt++)
		{
			pSrcList = UTIL_DLLIST_Get_n_Node(&pSrc->AdaptationSet, loopcnt);
			pSrcAdaptation = pSrcList->pData;
			/* default equivalent check method */
			if(pOrgAdaptation->id.str != NULL && pOrgAdaptation->id.str != NULL)
			{
				if(!VK_strcmp(pOrgAdaptation->id.str, pOrgAdaptation->id.str))
				{
					merr = P_MPD_Merge_AdaptationSet(pOrgAdaptation, pSrcAdaptation);
					if(merr == DASH_MPD_ERR_MERGE_ERROR)
					{
						PrintError("%s(%d) Error (P_MPD_Merge_Adaptationset)\n", __FUNCTION__, __LINE__);
						return merr;
					}
					else if(merr == DASH_MPD_ERR_MERGE_NONE)
					{
						continue;
					}
				}
			}
		}
	}
	/* append new period */
	pOrgList = UTIL_DLLIST_GetTailNode(&pOrg->AdaptationSet);
	pOrgAdaptation = pOrgList->pData;

	for(loopcnt = 0; loopcnt < nrSrcAdaptation; loopcnt++)
	{
		pSrcList = UTIL_DLLIST_Get_n_Node(&pSrc->AdaptationSet, loopcnt);
		pSrcAdaptation = pSrcList->pData;
		if(pOrgAdaptation->id.str != NULL && pSrcAdaptation->id.str != NULL)
		{
			if(!VK_strcmp(pOrgAdaptation->id.str, pSrcAdaptation->id.str))
			{
				break;
			}
		}
	}
	if(loopcnt == nrSrcAdaptation)
	{
		PrintDebug("%s(%d) Cannot decision that two mpd is the same mpd\n", __FUNCTION__, __LINE__);
		return DASH_MPD_ERR_MERGE_ERROR;
	}
	for(loopcnt = loopcnt+1; loopcnt < nrSrcAdaptation; loopcnt++)
	{
		pSrcList = UTIL_DLLIST_Get_n_Node(&pSrc->AdaptationSet, loopcnt);
		pSrcAdaptation = pSrcList->pData;
		pSrcList->pData = NULL;
		UTIL_DLLIST_Append(&pOrg->AdaptationSet, pSrcAdaptation);
	}
	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Merge_MPD(DASH_MPD_t* pOrg, DASH_MPD_t* pSrc)
{
	UTIL_DLLNode_t* pSrcList = NULL;
	UTIL_DLLNode_t* pOrgList = NULL;
	DASH_Period_t* pSrcPeriod = NULL;
	DASH_Period_t* pOrgPeriod = NULL;
	DASH_MPD_ERROR_e merr;
	HUINT32 loopcnt = 0;
	HUINT32 subloopcnt = 0;

	HUINT32 nrOrgPeriod = 0;
	HUINT32 nrSrcPeriod = 0;

	nrOrgPeriod = UTIL_DLLIST_Size(&pOrg->Period);
	nrSrcPeriod = UTIL_DLLIST_Size(&pSrc->Period);

	/* present period merge */
	for(loopcnt = 0; loopcnt < nrOrgPeriod; loopcnt++)
	{
		pOrgList = UTIL_DLLIST_Get_n_Node(&pOrg->Period, loopcnt);
		pOrgPeriod = pOrgList->pData;

		for(subloopcnt = 0; subloopcnt < nrSrcPeriod; subloopcnt++)
		{
			pSrcList = UTIL_DLLIST_Get_n_Node(&pSrc->Period, loopcnt);
			pSrcPeriod = pSrcList->pData;
			/* default equivalent check method */
			if(pOrgPeriod->id.str != NULL && pSrcPeriod->id.str != NULL)
			{
				if(!VK_strcmp(pOrgPeriod->id.str, pSrcPeriod->id.str))
				{
					merr = P_MPD_Merge_Period(pOrgPeriod, pSrcPeriod);
					if(merr == DASH_MPD_ERR_MERGE_ERROR)
					{
						PrintError("%s(%d) Error(ret of P_MPD_Merge_Period)\n", __FUNCTION__,__LINE__);
						return merr;
					}
					else if(merr == DASH_MPD_ERR_MERGE_NONE)
					{
						continue;
					}
				}
			}
			/* if [id] is not present, alternative method */
			else if(pOrgPeriod->start == pSrcPeriod->start)
			{
				merr = P_MPD_Merge_Period(pOrgPeriod, pSrcPeriod);
				if(merr == DASH_MPD_ERR_MERGE_ERROR)
				{
					PrintError("%s(%d) Error(ret of P_MPD_Merge_Period)\n", __FUNCTION__,__LINE__);
					return merr;
				}
				else if(merr == DASH_MPD_ERR_MERGE_NONE)
				{
					continue;
				}
			}
		}
	}

	/* append new period */
	pOrgList = UTIL_DLLIST_GetTailNode(&pOrg->Period);
	pOrgPeriod = pOrgList->pData;

	for(loopcnt = 0; loopcnt < nrSrcPeriod; loopcnt++)
	{
		pSrcList = UTIL_DLLIST_Get_n_Node(&pSrc->Period, loopcnt);
		pSrcPeriod = pSrcList->pData;
		if(pOrgPeriod->id.str != NULL && pSrcPeriod->id.str != NULL)
		{
			if(!VK_strcmp(pOrgPeriod->id.str, pSrcPeriod->id.str))
			{
				break;
			}
		}
		else if(pOrgPeriod->start == pSrcPeriod->start)
		{
			break;
		}
	}

	if(loopcnt == UTIL_DLLIST_Size(&pSrc->Period))
	{
		PrintDebug("%s(%d) Cannot decision that two mpd is the same mpd\n", __FUNCTION__, __LINE__);
		return DASH_MPD_ERR_MERGE_ERROR;
	}

	for(loopcnt = loopcnt+1; loopcnt < nrSrcPeriod; loopcnt++)
	{
		pSrcList = UTIL_DLLIST_Get_n_Node(&pSrc->Period, loopcnt);
		pSrcPeriod = pSrcList->pData;
		pSrcList->pData = NULL;
		UTIL_DLLIST_Append(&pOrg->Period, pSrcPeriod);
	}
	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_BaseURL(DASH_BaseURL_t* pBaseURL)
{
	if(pBaseURL == NULL)
	{
		return DASH_MPD_ERR_ERROR;
	}
	if(pBaseURL->serviceLocation.str != NULL)
	{
		VK_MEM_Free(pBaseURL->serviceLocation.str);
		pBaseURL->serviceLocation.str = NULL;
	}
	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_Location(DASH_Location_t* pLocation)
{
	if(pLocation == NULL)
	{
		return DASH_MPD_ERR_ERROR;
	}
	if(pLocation->uri.str != NULL)
	{
		VK_MEM_Free(pLocation->uri.str);
		pLocation->uri.str = NULL;
	}
	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_Metrics(DASH_Metrics_t* pMetrics)
{
	HUINT32 listcnt = 0;
	HUINT32 nrListElement = 0;
	UTIL_DLLNode_t* dllnode = NULL;
	DASH_DESCRIPTOR_t* desc = NULL;

	if(pMetrics == NULL)
	{
		return DASH_MPD_ERR_ERROR;
	}
	/* metrics release */
	nrListElement = UTIL_DLLIST_Size(&pMetrics->metrics);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pMetrics->metrics, listcnt);
		if(dllnode != NULL)
		{
			desc = dllnode->pData;
			if(desc->schemeIdUri.str != NULL)
			{
				VK_MEM_Free(desc->schemeIdUri.str);
				desc->schemeIdUri.str = NULL;
			}
			if(desc->value.str != NULL)
			{
				VK_MEM_Free(desc->value.str);
				desc->value.str = NULL;
			}
			VK_MEM_Free(dllnode->pData);
			dllnode->pData = NULL;
		}
	}
	UTIL_DLLIST_Reset(&pMetrics->metrics);
	/* Range release */
	nrListElement = UTIL_DLLIST_Size(&pMetrics->Range);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pMetrics->Range, listcnt);
		if(dllnode != NULL)
		{
			desc = dllnode->pData;
			if(desc->schemeIdUri.str != NULL)
			{
				VK_MEM_Free(desc->schemeIdUri.str);
				desc->schemeIdUri.str = NULL;
			}
			if(desc->value.str != NULL)
			{
				VK_MEM_Free(desc->value.str);
				desc->value.str = NULL;
			}
			VK_MEM_Free(dllnode->pData);
			dllnode->pData = NULL;
		}

	}
	UTIL_DLLIST_Reset(&pMetrics->Range);
	/* Reporting release */
	nrListElement = UTIL_DLLIST_Size(&pMetrics->Reporting);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pMetrics->Reporting, listcnt);
		if(dllnode != NULL)
		{
			desc = dllnode->pData;
			if(desc != NULL)
			{
				if(desc->schemeIdUri.str != NULL)
				{
					VK_MEM_Free(desc->schemeIdUri.str);
					desc->schemeIdUri.str = NULL;
				}
				if(desc->value.str != NULL)
				{
					VK_MEM_Free(desc->value.str);
					desc->value.str = NULL;
				}
				VK_MEM_Free(desc);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pMetrics->Reporting);

	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_ContentComponent(DASH_ContentComponent_t* pComponent)
{
	DASH_DESCRIPTOR_t* desc = NULL;
	UTIL_DLLNode_t* dllnode = NULL;
	HUINT32 listcnt = 0;
	HUINT32 nrListElement = 0;

	if(pComponent->id.str != NULL)
	{
		VK_MEM_Free(pComponent->id.str);
		pComponent->id.str = NULL;
	}
	if(pComponent->lang.str != NULL)
	{
		VK_MEM_Free(pComponent->lang.str);
		pComponent->lang.str = NULL;
	}
	if(pComponent->contentType.str != NULL)
	{
		VK_MEM_Free(pComponent->contentType.str);
		pComponent->contentType.str = NULL;
	}
	/* Accessibility release */
	nrListElement = UTIL_DLLIST_Size(&pComponent->Accessibility);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pComponent->Accessibility, listcnt);
		if(dllnode != NULL)
		{
			desc = dllnode->pData;
			if(desc != NULL)
			{
				if(desc->schemeIdUri.str != NULL)
				{
					VK_MEM_Free(desc->schemeIdUri.str);
					desc->schemeIdUri.str = NULL;
				}
				if(desc->value.str != NULL)
				{
					VK_MEM_Free(desc->value.str);
					desc->value.str = NULL;
				}
				VK_MEM_Free(desc);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pComponent->Accessibility);
	/* Role release */
	nrListElement = UTIL_DLLIST_Size(&pComponent->Role);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pComponent->Role, listcnt);
		if(dllnode != NULL)
		{
			desc = dllnode->pData;
			if(desc != NULL)
			{
				if(desc->schemeIdUri.str != NULL)
				{
					VK_MEM_Free(desc->schemeIdUri.str);
					desc->schemeIdUri.str = NULL;
				}
				if(desc->value.str != NULL)
				{
					VK_MEM_Free(desc->value.str);
					desc->value.str = NULL;
				}
				VK_MEM_Free(desc);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pComponent->Role);
	/* Rating release */
	nrListElement = UTIL_DLLIST_Size(&pComponent->Rating);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pComponent->Rating, listcnt);
		if(dllnode != NULL)
		{
			desc = dllnode->pData;
			if(desc != NULL)
			{
				if(desc->schemeIdUri.str != NULL)
				{
					VK_MEM_Free(desc->schemeIdUri.str);
					desc->schemeIdUri.str = NULL;
				}
				if(desc->value.str != NULL)
				{
					VK_MEM_Free(desc->value.str);
					desc->value.str = NULL;
				}
				VK_MEM_Free(desc);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pComponent->Role);
	/* ViewPoint release */
	nrListElement = UTIL_DLLIST_Size(&pComponent->ViewPoint);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pComponent->ViewPoint, listcnt);
		if(dllnode != NULL)
		{
			desc = dllnode->pData;
			if(desc != NULL)
			{
				if(desc->schemeIdUri.str != NULL)
				{
					VK_MEM_Free(desc->schemeIdUri.str);
					desc->schemeIdUri.str = NULL;
				}
				if(desc->value.str != NULL)
				{
					VK_MEM_Free(desc->value.str);
					desc->value.str = NULL;
				}
				VK_MEM_Free(desc);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pComponent->ViewPoint);

	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_ProgramInformation(DASH_ProgramInformation_t* pInfo)
{
	if(pInfo == NULL)
	{
		return DASH_MPD_ERR_ERROR;
	}
	if(pInfo->copyright.str != NULL)
	{
		VK_MEM_Free(pInfo->copyright.str);
		pInfo->copyright.str = NULL;
	}
	if(pInfo->lang.str != NULL)
	{
		VK_MEM_Free(pInfo->lang.str);
		pInfo->lang.str = NULL;
	}
	if(pInfo->moreInformationURL.str != NULL)
	{
		VK_MEM_Free(pInfo->moreInformationURL.str);
		pInfo->moreInformationURL.str = NULL;
	}
	if(pInfo->source.str != NULL)
	{
		VK_MEM_Free(pInfo->source.str);
		pInfo->source.str = NULL;
	}
	if(pInfo->title.str != NULL)
	{
		VK_MEM_Free(pInfo->title.str);
		pInfo->title.str = NULL;
	}
	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_SegmentBase(DASH_SegmentBase_t* pBase)
{
	/* usually pBASE is not dynamically allocated */
	if(pBase->Initialisation.sourceURL.str != NULL)
	{
		VK_MEM_Free(pBase->Initialisation.sourceURL.str);
		pBase->Initialisation.sourceURL.str = NULL;
	}
	if(pBase->RepresentationIndex.sourceURL.str != NULL)
	{
		VK_MEM_Free(pBase->RepresentationIndex.sourceURL.str);
		pBase->RepresentationIndex.sourceURL.str = NULL;
	}

	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_MultipleSegmentBaseInformation(DASH_MultipleSegmentBase_t* pMultiSegBase)
{
	DASH_MPD_ERROR_e merr;
	UTIL_DLLNode_t* dllnode = NULL;
	HUINT32 listcnt = 0;
	HUINT32 nrListElement = 0;

	if(pMultiSegBase->BitstreamSwitching.sourceURL.str != NULL)
	{
		VK_MEM_Free(pMultiSegBase->BitstreamSwitching.sourceURL.str);
		pMultiSegBase->BitstreamSwitching.sourceURL.str = NULL;
	}

	merr = P_MPD_Release_SegmentBase(&pMultiSegBase->SegmentBaseInformation);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}

	/* Base URL Release */
	nrListElement = UTIL_DLLIST_Size(&pMultiSegBase->SegmentTimeline);
	for(listcnt = 0; listcnt < nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pMultiSegBase->SegmentTimeline, listcnt);
		if(dllnode != NULL)
		{
			if(dllnode->pData != NULL)
			{
				VK_MEM_Free(dllnode->pData);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pMultiSegBase->SegmentTimeline);

	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_SegmentList(DASH_SegmentList_t* pList)
{
	DASH_MPD_ERROR_e merr;
	DASH_URLTYPE_t* url = NULL;
	UTIL_DLLNode_t* dllnode = NULL;
	HUINT32 listcnt = 0;
	HUINT32 nrListElement = 0;

	if(pList->index.sourceURL.str != NULL)
	{
		VK_MEM_Free(pList->index.sourceURL.str);
		pList->index.sourceURL.str = NULL;
	}
	if(pList->media.sourceURL.str != NULL)
	{
		VK_MEM_Free(pList->media.sourceURL.str);
		pList->media.sourceURL.str = NULL;
	}

	/* Base URL Release */
	nrListElement = UTIL_DLLIST_Size(&pList->SegmentUrl);
	for(listcnt = 0; listcnt < nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pList->SegmentUrl, listcnt);
		if(dllnode != NULL)
		{
			url = dllnode->pData;
			if(url != NULL)
			{
				if(url->sourceURL.str != NULL)
				{
					VK_MEM_Free(url->sourceURL.str);
					url->sourceURL.str = NULL;
				}
				VK_MEM_Free(url);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pList->SegmentUrl);

	merr = P_MPD_Release_MultipleSegmentBaseInformation(&pList->MultipleSegmentBaseInformation);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}
	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_SegmentTemplate(DASH_SegmentTemplate_t* pTemplate)
{
	DASH_MPD_ERROR_e merr;
	if(pTemplate->bitstreamSwitching.str != NULL)
	{
		VK_MEM_Free(pTemplate->bitstreamSwitching.str);
		pTemplate->bitstreamSwitching.str = NULL;
	}
	if(pTemplate->media.str != NULL)
	{
		VK_MEM_Free(pTemplate->media.str);
		pTemplate->media.str = NULL;
	}
	if(pTemplate->initialisation.str != NULL)
	{
		VK_MEM_Free(pTemplate->initialisation.str);
		pTemplate->initialisation.str = NULL;
	}
	if(pTemplate->index.str != NULL)
	{
		VK_MEM_Free(pTemplate->index.str);
		pTemplate->index.str = NULL;
	}
	merr = P_MPD_Release_MultipleSegmentBaseInformation(&pTemplate->MultipleSegmentBaseInformation);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}

	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_CommonAttributes(DASH_CommonAttributesElements_t* pAttributes)
{
	UTIL_DLLNode_t* dllnode = NULL;
	DASH_DESCRIPTOR_t* desc = NULL;
	HUINT32 listcnt = 0;
	HUINT32 nrListElement = 0;

	if(pAttributes->profile.str != NULL)
	{
		VK_MEM_Free(pAttributes->profile.str);
		pAttributes->profile.str = NULL;
	}
	if(pAttributes->frameRate.str != NULL)
	{
		VK_MEM_Free(pAttributes->frameRate.str);
		pAttributes->frameRate.str = NULL;
	}
	if(pAttributes->audioSamplingRate.str != NULL)
	{
		VK_MEM_Free(pAttributes->audioSamplingRate.str);
		pAttributes->audioSamplingRate.str = NULL;
	}
	if(pAttributes->mimeType.str != NULL)
	{
		VK_MEM_Free(pAttributes->mimeType.str);
		pAttributes->mimeType.str = NULL;
	}
	if(pAttributes->segmentProfiles.str != NULL)
	{
		VK_MEM_Free(pAttributes->segmentProfiles.str);
		pAttributes->segmentProfiles.str = NULL;
	}
	if(pAttributes->codecs.str != NULL)
	{
		VK_MEM_Free(pAttributes->codecs.str);
		pAttributes->codecs.str = NULL;
	}
	/* AudioChannelConfiguration release */
	nrListElement = UTIL_DLLIST_Size(&pAttributes->AudioChannelConfiguration);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAttributes->AudioChannelConfiguration, listcnt);
		if(dllnode != NULL)
		{
			desc = dllnode->pData;
			if(desc != NULL)
			{
				if(desc->schemeIdUri.str != NULL)
				{
					VK_MEM_Free(desc->schemeIdUri.str);
					desc->schemeIdUri.str = NULL;
				}
				if(desc->value.str != NULL)
				{
					VK_MEM_Free(desc->value.str);
					desc->value.str = NULL;
				}
				VK_MEM_Free(desc);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pAttributes->AudioChannelConfiguration);
	/* ContentProtection release */
	nrListElement = UTIL_DLLIST_Size(&pAttributes->ContentProtection);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAttributes->ContentProtection, listcnt);
		if(dllnode != NULL)
		{
			desc = dllnode->pData;
			if(desc != NULL)
			{
				if(desc->schemeIdUri.str != NULL)
				{
					VK_MEM_Free(desc->schemeIdUri.str);
					desc->schemeIdUri.str = NULL;
				}
				if(desc->value.str != NULL)
				{
					VK_MEM_Free(desc->value.str);
					desc->value.str = NULL;
				}
				VK_MEM_Free(desc);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pAttributes->ContentProtection);
	/* FramePacking release */
	nrListElement = UTIL_DLLIST_Size(&pAttributes->FramePacking);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAttributes->FramePacking, listcnt);
		if(dllnode != NULL)
		{
			desc = dllnode->pData;
			if(desc != NULL)
			{
				if(desc->schemeIdUri.str != NULL)
				{
					VK_MEM_Free(desc->schemeIdUri.str);
					desc->schemeIdUri.str = NULL;
				}
				if(desc->value.str != NULL)
				{
					VK_MEM_Free(desc->value.str);
					desc->value.str = NULL;
				}
				VK_MEM_Free(desc);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pAttributes->FramePacking);

	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_Subrepresentation(DASH_Subrepresentation_t* pSubRep)
{
	DASH_MPD_ERROR_e merr;
	UTIL_DLLNode_t* dllnode = NULL;
	DASH_STRING_t* str = NULL;
	HUINT32 listcnt = 0;
	HUINT32 nrListElement = 0;

	merr = P_MPD_Release_CommonAttributes(&pSubRep->CommonAttributesElements);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}
	/* dependencyLevel release */
	nrListElement = UTIL_DLLIST_Size(&pSubRep->dependencyLevel);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pSubRep->dependencyLevel, listcnt);
		if(dllnode != NULL)
		{
			str = dllnode->pData;
			if(str != NULL)
			{
				if(str->str != NULL)
				{
					VK_MEM_Free(str->str);
					str->str = NULL;
				}
				VK_MEM_Free(str);
				str = NULL;
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pSubRep->dependencyLevel);
	/* contentComponent release */
	nrListElement = UTIL_DLLIST_Size(&pSubRep->dependencyLevel);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pSubRep->contentComponent, listcnt);
		if(dllnode != NULL)
		{
			str = dllnode->pData;
			if(str != NULL)
			{
				if(str->str != NULL)
				{
					VK_MEM_Free(str->str);
					str->str = NULL;
				}
				VK_MEM_Free(str);
				str = NULL;
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pSubRep->contentComponent);

	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_Representation(DASH_Representation_t* pRepresentation)
{
	DASH_MPD_ERROR_e merr;
	UTIL_DLLNode_t* dllnode = NULL;
	DASH_STRING_t* str = NULL;
	DASH_BaseURL_t* baseurl = NULL;
	DASH_Subrepresentation_t* subrep  = NULL;
	HUINT32 listcnt = 0;
	HUINT32 nrListElement = 0;

	if(pRepresentation->id.str != NULL)
	{
		VK_MEM_Free(pRepresentation->id.str);
		pRepresentation->id.str = NULL;
	}
	merr = P_MPD_Release_CommonAttributes(&pRepresentation->CommonAttributesElements);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}
	merr = P_MPD_Release_SegmentBase(&pRepresentation->SegmentBase);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}
	merr = P_MPD_Release_SegmentList(&pRepresentation->SegmentList);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}
	merr = P_MPD_Release_SegmentTemplate(&pRepresentation->SegmentTemplate);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}
	/* dependencyId release */
	nrListElement = UTIL_DLLIST_Size(&pRepresentation->dependencyId);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pRepresentation->dependencyId, listcnt);
		if(dllnode != NULL)
		{
			str = dllnode->pData;
			if(str != NULL)
			{
				if(str->str != NULL)
				{
					VK_MEM_Free(str->str);
					str->str = NULL;
				}
				VK_MEM_Free(str);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pRepresentation->dependencyId);
	/* mediaStreamStructureId release */
	nrListElement = UTIL_DLLIST_Size(&pRepresentation->mediaStreamStructureId);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pRepresentation->mediaStreamStructureId, listcnt);
		if(dllnode != NULL)
		{
			str = dllnode->pData;
			if(str != NULL)
			{
				if(str->str != NULL)
				{
					VK_MEM_Free(str->str);
					str->str = NULL;
				}
				VK_MEM_Free(str);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pRepresentation->mediaStreamStructureId);
	/* Base URL Release */
	nrListElement = UTIL_DLLIST_Size(&pRepresentation->BaseURL);
	for(listcnt = 0; listcnt < nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pRepresentation->BaseURL, listcnt);
		if(dllnode != NULL)
		{
			baseurl = dllnode->pData;
			if(baseurl != NULL)
			{
				merr = P_MPD_Release_BaseURL(baseurl);
				if(merr != DASH_MPD_ERR_OK)
				{
					PrintError("%s(%d) Error> Release failed(BASE URL)\n", __FUNCTION__,__LINE__);
					return merr;
				}
				VK_MEM_Free(dllnode->pData);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pRepresentation->BaseURL);
	/* SubRepresentation release */
	nrListElement = UTIL_DLLIST_Size(&pRepresentation->SubRepresentation);
	for(listcnt = 0; listcnt < nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pRepresentation->SubRepresentation, listcnt);
		if(dllnode != NULL)
		{
			subrep = dllnode->pData;
			if(subrep != NULL)
			{
				merr = P_MPD_Release_Subrepresentation(subrep);
				if(merr != DASH_MPD_ERR_OK)
				{
					PrintError("%s(%d) Error> Release failed(BASE URL)\n", __FUNCTION__,__LINE__);
					return merr;
				}
				VK_MEM_Free(dllnode->pData);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pRepresentation->SubRepresentation);
	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_AdaptationSet(DASH_AdaptationSet_t* pAdaptation)
{
	DASH_MPD_ERROR_e merr;
	HUINT32 listcnt = 0;
	UTIL_DLLNode_t* dllnode = NULL;
	DASH_BaseURL_t* baseurl = NULL;
	DASH_Representation_t* representation = NULL;
	DASH_DESCRIPTOR_t* desc = NULL;
	DASH_ContentComponent_t* contentcomponent = NULL;
	HUINT32 nrListElement = 0;

	if(pAdaptation == NULL)
	{
		return DASH_MPD_ERR_ERROR;
	}
	if(pAdaptation->id.str != NULL)
	{
		VK_MEM_Free(pAdaptation->id.str);
		pAdaptation->id.str = NULL;
	}
	if(pAdaptation->group.str != NULL)
	{
		VK_MEM_Free(pAdaptation->group.str);
		pAdaptation->group.str = NULL;
	}
	if(pAdaptation->lang.str != NULL)
	{
		VK_MEM_Free(pAdaptation->lang.str);
		pAdaptation->lang.str = NULL;
	}
	if(pAdaptation->contentType.str != NULL)
	{
		VK_MEM_Free(pAdaptation->contentType.str);
		pAdaptation->contentType.str = NULL;
	}
	if(pAdaptation->minFrameRate.str != NULL)
	{
		VK_MEM_Free(pAdaptation->minFrameRate.str);
		pAdaptation->minFrameRate.str = NULL;
	}
	if(pAdaptation->maxFrameRate.str != NULL)
	{
		VK_MEM_Free(pAdaptation->maxFrameRate.str);
		pAdaptation->maxFrameRate.str = NULL;
	}
	merr = P_MPD_Release_CommonAttributes(&pAdaptation->CommonAttributesElements);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}
	merr = P_MPD_Release_SegmentBase(&pAdaptation->SegmentBase);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}
	merr = P_MPD_Release_SegmentList(&pAdaptation->SegmentList);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}
	merr = P_MPD_Release_SegmentTemplate(&pAdaptation->SegmentTemplate);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}
	/* Content component Release */
	nrListElement = UTIL_DLLIST_Size(&pAdaptation->ContentComponent);
	for(listcnt = 0; listcnt < nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAdaptation->ContentComponent, listcnt);
		if(dllnode != NULL)
		{
			contentcomponent = dllnode->pData;
			if(contentcomponent != NULL)
			{
				merr = P_MPD_Release_ContentComponent(contentcomponent);
				if(merr != DASH_MPD_ERR_OK)
				{
					PrintError("%s(%d) Error> Release failed(ContentComponent)\n", __FUNCTION__,__LINE__);
					return merr;
				}
				VK_MEM_Free(dllnode->pData);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pAdaptation->BaseURL);
	/* Accessibility release */
	nrListElement = UTIL_DLLIST_Size(&pAdaptation->Accessibility);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAdaptation->Accessibility, listcnt);
		if(dllnode != NULL)
		{
			desc = dllnode->pData;
			if(desc != NULL)
			{
				if(desc->schemeIdUri.str != NULL)
				{
					VK_MEM_Free(desc->schemeIdUri.str);
					desc->schemeIdUri.str = NULL;
				}
				if(desc->value.str != NULL)
				{
					VK_MEM_Free(desc->value.str);
					desc->value.str = NULL;
				}
				VK_MEM_Free(desc);
				desc = NULL;
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pAdaptation->Accessibility);
	/* Role release */
	nrListElement = UTIL_DLLIST_Size(&pAdaptation->Role);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAdaptation->Role, listcnt);
		if(dllnode != NULL)
		{
			desc = dllnode->pData;
			if(desc != NULL)
			{
				if(desc->schemeIdUri.str != NULL)
				{
					VK_MEM_Free(desc->schemeIdUri.str);
					desc->schemeIdUri.str = NULL;
				}
				if(desc->value.str != NULL)
				{
					VK_MEM_Free(desc->value.str);
					desc->value.str = NULL;
				}
				VK_MEM_Free(desc);
				desc = NULL;
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pAdaptation->Role);
	/* Rating release */
	nrListElement = UTIL_DLLIST_Size(&pAdaptation->Rating);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAdaptation->Rating, listcnt);
		if(dllnode != NULL)
		{
			desc = dllnode->pData;
			if(desc != NULL)
			{
				if(desc->schemeIdUri.str != NULL)
				{
					VK_MEM_Free(desc->schemeIdUri.str);
					desc->schemeIdUri.str = NULL;
				}
				if(desc->value.str != NULL)
				{
					VK_MEM_Free(desc->value.str);
					desc->value.str = NULL;
				}
				VK_MEM_Free(desc);
				desc = NULL;
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pAdaptation->Rating);
	/* ViewPoint release */
	nrListElement = UTIL_DLLIST_Size(&pAdaptation->ViewPoint);
	for(listcnt = 0; listcnt< nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAdaptation->ViewPoint, listcnt);
		if(dllnode != NULL)
		{
			desc = dllnode->pData;
			if(desc != NULL)
			{
				if(desc->schemeIdUri.str != NULL)
				{
					VK_MEM_Free(desc->schemeIdUri.str);
					desc->schemeIdUri.str = NULL;
				}
				if(desc->value.str != NULL)
				{
					VK_MEM_Free(desc->value.str);
					desc->value.str = NULL;
				}
				VK_MEM_Free(desc);
				desc = NULL;
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pAdaptation->ViewPoint);
	/* Base URL Release */
	nrListElement = UTIL_DLLIST_Size(&pAdaptation->BaseURL);
	for(listcnt = 0; listcnt < nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAdaptation->BaseURL, listcnt);
		if(dllnode != NULL)
		{
			baseurl = dllnode->pData;
			if(baseurl != NULL)
			{
				merr = P_MPD_Release_BaseURL(baseurl);
				if(merr != DASH_MPD_ERR_OK)
				{
					PrintError("%s(%d) Error> Release failed(BASE URL)\n", __FUNCTION__,__LINE__);
					return merr;
				}
				VK_MEM_Free(dllnode->pData);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pAdaptation->BaseURL);
	/* Representation Release */
	nrListElement = UTIL_DLLIST_Size(&pAdaptation->Representation);
	for(listcnt = 0; listcnt < nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAdaptation->Representation, listcnt);
		if(dllnode != NULL)
		{
			representation = dllnode->pData;
			if(representation != NULL)
			{
				merr = P_MPD_Release_Representation(representation);
				if(merr != DASH_MPD_ERR_OK)
				{
					PrintError("%s(%d) Error> Release failed(BASE URL)\n", __FUNCTION__,__LINE__);
					return merr;
				}
				VK_MEM_Free(dllnode->pData);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pAdaptation->Representation);

	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_Period(DASH_Period_t* pPeriod)
{
	DASH_MPD_ERROR_e merr;
	HUINT32 listcnt = 0;
	HUINT32 nrListElement = 0;
	UTIL_DLLNode_t* dllnode = NULL;
	DASH_BaseURL_t* baseurl = NULL;
	DASH_AdaptationSet_t* adaptationset = NULL;

	if(pPeriod == NULL)
	{
		return DASH_MPD_ERR_ERROR;
	}
	if(pPeriod->id.str != NULL)
	{
		VK_MEM_Free(pPeriod->id.str);
		pPeriod->id.str = NULL;
	}
	merr = P_MPD_Release_SegmentBase(&pPeriod->SegmentBase);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}
	merr = P_MPD_Release_SegmentList(&pPeriod->SegmentList);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}
	merr = P_MPD_Release_SegmentTemplate(&pPeriod->SegmentTemplate);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}
	/* Base URL Release */
	nrListElement = UTIL_DLLIST_Size(&pPeriod->BaseURL);
	for(listcnt = 0; listcnt < nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pPeriod->BaseURL, listcnt);
		if(dllnode != NULL)
		{
			baseurl = dllnode->pData;
			if(baseurl != NULL)
			{
				merr = P_MPD_Release_BaseURL(baseurl);
				if(merr != DASH_MPD_ERR_OK)
				{
					PrintError("%s(%d) Error> Release failed(BASE URL)\n", __FUNCTION__,__LINE__);
					return merr;
				}
				VK_MEM_Free(dllnode->pData);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pPeriod->BaseURL);


#if 0
				VK_printf("mpd test\n");
//				DASH_MPD_t* mpdt = newmpd->parsedMPD;
//				DASH_Period_t* ped = ((UTIL_DLLNode_t*)UTIL_DLLIST_Get_n_Node(&mpdt->Period,0))->pData;
				DASH_AdaptationSet_t* ada = ((UTIL_DLLNode_t*)UTIL_DLLIST_Get_n_Node(&pPeriod->AdaptationSet,0))->pData;
				VK_MEM_Free(ada->id.str);
				ada->id.str = NULL;
#endif
	/* Adaptation set Release */
	nrListElement = UTIL_DLLIST_Size(&pPeriod->AdaptationSet);
	for(listcnt = 0; listcnt < UTIL_DLLIST_Size(&pPeriod->AdaptationSet); listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pPeriod->AdaptationSet, listcnt);
		if(dllnode != NULL)
		{
			adaptationset = dllnode->pData;
			if(adaptationset != NULL)
			{
				merr = P_MPD_Release_AdaptationSet(adaptationset);
				if(merr != DASH_MPD_ERR_OK)
				{
					PrintError("%s(%d) Error> Release failed(Adaptationset)\n", __FUNCTION__,__LINE__);
					return merr;
				}
				VK_MEM_Free(dllnode->pData);
				dllnode->pData = NULL;
			}
		}
	}
	UTIL_DLLIST_Reset(&pPeriod->AdaptationSet);

	return DASH_MPD_ERR_OK;
}

static DASH_MPD_ERROR_e P_MPD_Release_MPD(DASH_MPD_t* pMPD)
{
	UTIL_DLLNode_t* dllnode = NULL;
	DASH_BaseURL_t* baseurl = NULL;
	DASH_ProgramInformation_t* progInfo = NULL;
	DASH_Location_t* location = NULL;
	DASH_Metrics_t* metrics = NULL;
	DASH_Period_t* period = NULL;
	HUINT32 listcnt = 0;
	HUINT32 nrListElement = 0;
	DASH_MPD_ERROR_e merr;

	if(pMPD == NULL)
	{
		PrintError("%s(%d) Error> Release failed(MPD)\n", __FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	if(pMPD->id.str != NULL)
	{
		VK_MEM_Free(pMPD->id.str);
		pMPD->id.str = NULL;
	}
	if(pMPD->profiles.str != NULL)
	{
		VK_MEM_Free(pMPD->profiles.str);
		pMPD->profiles.str = NULL;
	}
	/* program information release */
	nrListElement = UTIL_DLLIST_Size(&pMPD->ProgramInformation);
	for(listcnt = 0; listcnt < nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pMPD->ProgramInformation, listcnt);
		if(dllnode != NULL)
		{
			progInfo = dllnode->pData;
			if(progInfo != NULL)
			{
				merr = P_MPD_Release_ProgramInformation(progInfo);
				if(merr != DASH_MPD_ERR_OK)
				{
					PrintError("%s(%d) Error> Release failed(program information)\n", __FUNCTION__,__LINE__);
					return merr;
				}
				VK_MEM_Free(dllnode->pData);
				dllnode->pData = NULL;
			}
		}
		else
		{
			PrintError("%s(%d) Error> Corrupted list found\n", __FUNCTION__,__LINE__);
			return merr;
		}
	}
	UTIL_DLLIST_Reset(&pMPD->ProgramInformation);
	/* Base URL Release */
	nrListElement = UTIL_DLLIST_Size(&pMPD->BaseURL);
	for(listcnt = 0; listcnt < nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pMPD->BaseURL, listcnt);
		if(dllnode != NULL)
		{
			baseurl = dllnode->pData;
			if(baseurl != NULL)
			{
				merr = P_MPD_Release_BaseURL(baseurl);
				if(merr != DASH_MPD_ERR_OK)
				{
					PrintError("%s(%d) Error> Release failed(BASE URL)\n", __FUNCTION__,__LINE__);
					return merr;
				}
				VK_MEM_Free(dllnode->pData);
				dllnode->pData = NULL;
			}
		}
		else
		{
			PrintError("%s(%d) Error> Corrupted list found\n", __FUNCTION__,__LINE__);
			return merr;
		}
	}
	UTIL_DLLIST_Reset(&pMPD->BaseURL);
	/* Location Release */
	nrListElement = UTIL_DLLIST_Size(&pMPD->Location);
	for(listcnt = 0; listcnt < nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pMPD->Location, listcnt);
		if(dllnode != NULL)
		{
			location = dllnode->pData;
			if(location != NULL)
			{
				merr = P_MPD_Release_Location(location);
				if(merr != DASH_MPD_ERR_OK)
				{
					PrintError("%s(%d) Error> Release failed(Location)\n", __FUNCTION__, __LINE__);
					return merr;
				}
				VK_MEM_Free(dllnode->pData);
				dllnode->pData = NULL;
			}
		}
		else
		{
			PrintError("%s(%d) Error> Corrupted list found\n", __FUNCTION__,__LINE__);
			return merr;
		}
	}
	UTIL_DLLIST_Reset(&pMPD->Location);
	/* Metrics Release */
	nrListElement = UTIL_DLLIST_Size(&pMPD->Metrics);
	for(listcnt = 0; listcnt < nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pMPD->Metrics, listcnt);
		if(dllnode != NULL)
		{
			metrics = dllnode->pData;
			if(metrics != NULL)
			{
				merr = P_MPD_Release_Metrics(metrics);
				if(merr != DASH_MPD_ERR_OK)
				{
					PrintError("%s(%d) Error> Release failed(Metrics)\n", __FUNCTION__, __LINE__);
					return merr;
				}
				VK_MEM_Free(dllnode->pData);
				dllnode->pData = NULL;
			}
		}
		else
		{
			PrintError("%s(%d) Error> Corrupted list found\n", __FUNCTION__,__LINE__);
			return merr;
		}
	}
	UTIL_DLLIST_Reset(&pMPD->Metrics);
	/* Period */
	nrListElement = UTIL_DLLIST_Size(&pMPD->Period);
	for(listcnt = 0; listcnt < nrListElement; listcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pMPD->Period, listcnt);
		if(dllnode != NULL)
		{
			period = dllnode->pData;
			if(period != NULL)
			{
				merr = P_MPD_Release_Period(period);
				if(merr != DASH_MPD_ERR_OK)
				{
					PrintError("%s(%d) Error> Release failed(Metrics)\n", __FUNCTION__, __LINE__);
					return merr;
				}
				VK_MEM_Free(dllnode->pData);
				dllnode->pData = NULL;
			}
		}
		else
		{
			PrintError("%s(%d) Error> Corrupted list found\n", __FUNCTION__,__LINE__);
			return merr;
		}
	}
	UTIL_DLLIST_Reset(&pMPD->Period);

	return DASH_MPD_ERR_OK;
}

HBOOL P_MPD_IsRepresentationAsTrack(DASH_MPD_t* mpd)
{
	DASH_Period_t* period = NULL;
	DASH_AdaptationSet_t* adaptation = NULL;
	DASH_Representation_t* representation = NULL;
	UTIL_DLLNode_t* dllnode = NULL;
	HUINT32 loopcnt = 0;

	dllnode = UTIL_DLLIST_GetHeadNode(&mpd->Period);
	period = dllnode->pData;

	dllnode = UTIL_DLLIST_GetHeadNode(&period->AdaptationSet);
	adaptation = dllnode->pData;

	/* if adaptationset has no codec infomation, regard representation as track */
	if(adaptation->CommonAttributesElements.codecs.str == NULL)
	{
		return TRUE;
	}

	for(loopcnt = 0; loopcnt < UTIL_DLLIST_Size(&adaptation->Representation); loopcnt ++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&adaptation->Representation, loopcnt);
		representation = dllnode->pData;

		if(representation->CommonAttributesElements.codecs.str != NULL)
		{
			if(!VK_strcmp((char*)adaptation->CommonAttributesElements.codecs.str,
				(char*)representation->CommonAttributesElements.codecs.str))
			{
				continue;
			}
			else
			{
				return TRUE;
			}
		}
		else
		{
			/* if representation has no codec information, regard adaptation as track */
			return FALSE;
		}
	}

	return FALSE;
}

DASH_MPD_ERROR_e P_MPD_DetermineTrack(DASH_MPD_t* mpd)
{
	DASH_Period_t* period = NULL;
	DASH_AdaptationSet_t* adaptation = NULL;
	DASH_Representation_t* representation = NULL;
	DASH_Representation_t* minRep = NULL;
	UTIL_DLLNode_t* dllnode = NULL;
	HUINT32 loopcnt = 0;


	dllnode = UTIL_DLLIST_GetHeadNode(&mpd->Period);
	period = dllnode->pData;

	dllnode = UTIL_DLLIST_GetHeadNode(&period->AdaptationSet);
	adaptation = dllnode->pData;

	//workaround(select minimum bandwidth!)

	dllnode = UTIL_DLLIST_GetHeadNode(&adaptation->Representation);
	minRep = dllnode->pData;
	/* determin proper track in representation */
	for(loopcnt = 0; loopcnt < UTIL_DLLIST_Size(&adaptation->Representation); loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&adaptation->Representation, loopcnt);
		representation = dllnode->pData;

		if(minRep->bandwidth > representation->bandwidth)
		{
			minRep = representation;
		}
	}
	/* fill codec in adaptation based on codec info in representation */
	if(adaptation->CommonAttributesElements.codecs.str != NULL)
	{
		VK_MEM_Free(adaptation->CommonAttributesElements.codecs.str);
		adaptation->CommonAttributesElements.codecs.str = NULL;
	}
	adaptation->CommonAttributesElements.codecs.str = VK_MEM_Alloc(VK_strlen(minRep->CommonAttributesElements.codecs.str)+1);
	if(adaptation->CommonAttributesElements.codecs.str == NULL)
	{
		PrintError("%s(%d) Error : Not enough memory\n", __FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	adaptation->CommonAttributesElements.codecs.length = VK_strlen(minRep->CommonAttributesElements.codecs.str);
	adaptation->CommonAttributesElements.codecs.length = VK_strlen(minRep->CommonAttributesElements.codecs.str);
	VK_strcpy(adaptation->CommonAttributesElements.codecs.str, minRep->CommonAttributesElements.codecs.str);

	/* remove other representations */

	return DASH_MPD_ERR_OK;
}

DASH_MPD_ERROR_e P_DASH_MPD_Setup(DASH_MPDPARSER_HANDLE_t mpd, const char *pBuf, HUINT32 szBuf)
{
	DASH_MPD_t *pDescriptor = NULL;
	pDescriptor = (DASH_MPD_t*)VK_MEM_Alloc(sizeof(DASH_MPD_t));
	if(pDescriptor == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		goto __init_error;
	}
	VK_MEM_Memset(pDescriptor, 0x0, sizeof(DASH_MPD_t));

	mpd->parsedMPD = pDescriptor;
	mpd->document = (void *)xmlParseMemory(pBuf, szBuf);
	if( mpd->document == NULL)
	{
		goto __init_error;
	}
	mpd->root = (void*)xmlDocGetRootElement((xmlDoc*)mpd->document);
	if( mpd->root == NULL)
	{
		goto __init_error;
	}

	return DASH_MPD_ERR_OK;

__init_error:
	if(pDescriptor != NULL)
	{
		VK_MEM_Free(pDescriptor);
		pDescriptor = NULL;
	}
	if(mpd->document != NULL)
	{
		VK_MEM_Free(mpd->document);
		mpd->document = NULL;
	}
	return DASH_MPD_ERR_ERROR;
}

DASH_MPD_ERROR_e DASH_MPD_PostProcess(DASH_MPDPARSER_HANDLE_t mpdparser)
{
	HUINT32 listSize = 0;
	HUINT32 loopcnt = 0;
	UTIL_DLLNode_t* dllnode = NULL;
	UTIL_DLLNode_t* prevDllnode = NULL;
	DASH_Period_t *period = NULL;
	DASH_Period_t *prevPeriod = NULL;
	DASH_MPD_t *mpd = mpdparser->parsedMPD;
	HBOOL isRepAsTrack = FALSE;
	//DASH_TIME_t	timeScale = 1;

	/* start time and duration insertion in period */
	listSize = UTIL_DLLIST_Size(&mpd->Period);
	for(loopcnt = 0; loopcnt < listSize; loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&mpd->Period, loopcnt);
		period = (DASH_Period_t*)dllnode->pData;

		/* first period */
		if(loopcnt == 0)
		{
			if(period->start == DASH_WALLCLK_NOT_EXIST)
			{
				if(mpd->type == DASH_MPD_TYPE_STATIC)
				{
					period->start = 0;
					period->duration = mpd->mediaPresentationDuration;
					period->type = DASH_PERIOD_TYPE_REGULAR;
				}
				else if(mpd->type == DASH_MPD_TYPE_DYNAMIC)
				{
					period->start = 0;
					period->type = DASH_PERIOD_TYPE_EARLY_ACCESS;
					period->duration = mpd->mediaPresentationDuration;
					if(period->duration == 0)
					{
						period->duration = 0x7FFFFFFFFFFFFFFF;
					}
				}
			}
			else
			{
				period->start = __convert_ms_to_nonscaled(period->start, mpd->timeScale);
				period->duration = __convert_ms_to_nonscaled(period->duration, mpd->timeScale);
				period->type = DASH_PERIOD_TYPE_REGULAR;
			}
		}
		/* intermediate period */
		else if(loopcnt > 0)
		{
			prevDllnode = UTIL_DLLIST_Get_n_Node(&mpd->Period, loopcnt-1);
			prevPeriod = (DASH_Period_t*)prevDllnode->pData;

			if(period->start == DASH_WALLCLK_NOT_EXIST)
			{
				/* if previous period was early access period, make this period as beginning period */
				if(prevPeriod->type == DASH_PERIOD_TYPE_EARLY_ACCESS)
				{
					period->start = 0LL;
					period->type = DASH_PERIOD_TYPE_REGULAR;
				}
				else if(prevPeriod->duration != DASH_WALLCLK_NOT_EXIST)
				{
					period->start = prevPeriod->start + prevPeriod->duration;
				}
				else
				{
					PrintError("%s(%d) Error : Time information in period is invalid\n", __FUNCTION__, __LINE__);
					return DASH_MPD_ERR_ERROR;
				}
			}
		}
	}

	/* find media segment type */
	mpdparser->format = P_MPD_FindSegmentFormat(mpdparser->root);\
	PrintDebug("%s(%d) *** MPD segment format : %d\n", __FUNCTION__, __LINE__, mpdparser->format);

	/* find is representation as track */
	isRepAsTrack = P_MPD_IsRepresentationAsTrack(mpdparser->parsedMPD);
	if(isRepAsTrack == TRUE)
	{
		/* determin proper representation as track */
		//P_MPD_DetermineTrack(mpdparser->parsedMPD);
	}

	return DASH_MPD_ERR_OK;
}

DASH_MPD_ERROR_e DASH_MPD_Parse_Period(void* element, DASH_Period_t* per)
{
	DASH_Period_t *period;
	DASH_MPD_ERROR_e merr;
	merr = P_MPD_Parse_Period(element,&period);

	VK_MEM_Memcpy(per, period, sizeof(DASH_Period_t));
	VK_MEM_Free(period);
	period = NULL;
	return merr;
}

DASH_MPD_ERROR_e DASH_MPD_Parse_AdaptationSet(void* element, DASH_AdaptationSet_t *adap)
{
	DASH_AdaptationSet_t *adaptation;
	DASH_MPD_ERROR_e merr;
	merr =  P_MPD_Parse_AdaptationSet(element, &adaptation);

	VK_MEM_Memcpy(adap, adaptation, sizeof(DASH_AdaptationSet_t));
	VK_MEM_Free(adaptation);
	adaptation = NULL;
	return merr;
}

DASH_MPD_ERROR_e DASH_MPD_Parse_SegmentList(void* element, DASH_SegmentList_t *segmentlist)
{
	DASH_SegmentList_t *list;
	DASH_MPD_ERROR_e merr;
	merr = P_MPD_Parse_SegmentList(element, &list);

	VK_MEM_Memcpy(segmentlist, list, sizeof(DASH_SegmentList_t));
	VK_MEM_Free(list);
	list = NULL;
	return merr;
}

DASH_MPD_ERROR_e DASH_MPD_GetXlink(DASH_MPDPARSER_HANDLE_t parser, DASH_XLINK_ACTUATE_e eActuate ,DASH_XLINK_LIST_t** list)
{
	DASH_MPD_t* mpd = parser->parsedMPD;
	DASH_Period_t* period;
	DASH_AdaptationSet_t* adaptation;
	DASH_Representation_t* representation;
	UTIL_DLLNode_t* dllperiod;
	UTIL_DLLNode_t* dlladaptation;
	UTIL_DLLNode_t* dllrepresentation;
	HUINT32 period_cnt;
	HUINT32 adaptation_cnt;
	HUINT32 representation_cnt;
	DASH_XLINK_LIST_t* pList;

	*list = VK_MEM_Alloc(sizeof(DASH_XLINK_LIST_t));
	if(*list == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	VK_MEM_Memset(*list, 0x0, sizeof(DASH_XLINK_LIST_t));
	pList = *list;

	for(period_cnt = 0; period_cnt < UTIL_DLLIST_Size(&mpd->Period); period_cnt++)
	{
		dllperiod = UTIL_DLLIST_Get_n_Node(&mpd->Period, period_cnt);
		period = dllperiod->pData;
		if(period->isXlink == TRUE)
		{
			if(period->xlink.eActuate == eActuate)
			{
				UTIL_DLLIST_Append(&pList->periods, period);
			}
		}
		if(period->SegmentList.isXlink == TRUE)
		{
			if(period->SegmentList.xlink.eActuate == eActuate)
			{
				UTIL_DLLIST_Append(&pList->segmentlist, &period->SegmentList);
			}
		}

		for(adaptation_cnt = 0; adaptation_cnt < UTIL_DLLIST_Size(&period->AdaptationSet); adaptation_cnt++)
		{
			dlladaptation = UTIL_DLLIST_Get_n_Node(&period->AdaptationSet, adaptation_cnt);
			adaptation = dlladaptation->pData;
			if(adaptation->isXlink == TRUE)
			{
				if(adaptation->xlink.eActuate == eActuate)
				{
					UTIL_DLLIST_Append(&pList->adaptations, adaptation);
				}
			}
			if(adaptation->SegmentList.isXlink == TRUE)
			{
				if(adaptation->SegmentList.xlink.eActuate == eActuate)
				{
					UTIL_DLLIST_Append(&pList->segmentlist, &adaptation->SegmentList);
				}
			}

			for(representation_cnt = 0; representation_cnt < UTIL_DLLIST_Size(&adaptation->Representation); representation_cnt++)
			{
				dllrepresentation = UTIL_DLLIST_Get_n_Node(&adaptation->Representation, representation_cnt);
				representation = dllrepresentation->pData;
				if(representation->SegmentList.isXlink == TRUE)
				{
					if(representation->SegmentList.xlink.eActuate == eActuate)
					{
						UTIL_DLLIST_Append(&pList->segmentlist, &representation->SegmentList);
					}
				}
			}
		}
	}
	return DASH_MPD_ERR_OK;
}

DASH_MPDPARSER_HANDLE_t DASH_MPD_Create(void)
{
	DASH_MPDParser_t *pInstance;
	PrintEnter;

	pInstance = VK_MEM_Alloc(sizeof(DASH_MPDParser_t));
	if(pInstance == NULL)
	{
		PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
		return NULL;
	}
	VK_MEM_Memset(pInstance, 0, sizeof(DASH_MPDParser_t));

	PrintDebug("%s(%d) MPDParser has been allocated\n", __FUNCTION__, __LINE__);
	PrintExit;
	return (DASH_MPDPARSER_HANDLE_t)pInstance;
}

DASH_MPD_ERROR_e DASH_MPD_Destroy(DASH_MPDPARSER_HANDLE_t mpd)
{
	DASH_MPD_ERROR_e merr;

	PrintEnter;
	if(mpd == NULL)
	{
		PrintError("%s(%d) Error : MPD handle is NULL\n", __FUNCTION__, __LINE__);
		return DASH_MPD_ERR_ERROR;
	}

	if(mpd->parsedMPD != NULL)
	{
		merr = P_MPD_Release_MPD(mpd->parsedMPD);
		if(merr != DASH_MPD_ERR_OK)
		{
			return DASH_MPD_ERR_ERROR;
		}
		else if(merr == DASH_MPD_ERR_OK)
		{
			PrintDebug("%s(%d) Release complete\n", __FUNCTION__,__LINE__);
		}
		VK_MEM_Free(mpd->parsedMPD);
		mpd->parsedMPD = NULL;
	}
	else
	{
		PrintError("%s(%d) Error : MPD is NULL\n", __FUNCTION__, __LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	if(mpd->document != NULL)
	{
		xmlFreeDoc(mpd->document);
		mpd->document = NULL;
	}

	VK_MEM_Free(mpd);
	mpd = NULL;
	PrintExit;
	return DASH_MPD_ERR_OK;
}

DASH_MPD_ERROR_e DASH_MPD_REGISTER_XLINKDOWN_CB(DASH_MPDPARSER_HANDLE_t mpd, DASH_XLINK_DOWN_CB_t cb)
{
	mpd->XlinkDown = cb;
	return DASH_MPD_ERR_OK;
}

/* APIs for MPD Task */
DASH_MPD_ERROR_e DASH_MPD_ParseXML(DASH_MPDPARSER_HANDLE_t mpd, void* buf, HUINT32 szBuf)
{
	DASH_MPD_ERROR_e merr;
	PrintEnter;

	if(mpd == NULL)
	{
		PrintError("%s(%d) Error : MPD handle is NULL\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_MPD_ERR_ERROR;
	}
	if(buf == NULL)
	{
		PrintError("%s(%d) Error : MPD data buffer is NULL\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_MPD_ERR_ERROR;
	}

	/* Allocation of heap for XML data and parsing MPD */
	merr = P_DASH_MPD_Setup(mpd, buf, szBuf);
	if(merr != DASH_MPD_ERR_OK)
	{
		PrintError("%s(%d) Error : MPD Setup is failed\n", __FUNCTION__, __LINE__);
	}

	PrintExit;
	return merr;
}
DASH_MPD_ERROR_e DASH_MPD_ParseMPD(DASH_MPDPARSER_HANDLE_t mpd)
{
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	void* xmlroot;

	PrintEnter;

	if(mpd == NULL)
	{
		PrintError("%s(%d) Error : new MPD handle is NULL\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_MPD_ERR_ERROR;
	}

	xmlroot = P_MPD_Get_MPD(mpd);
	if(xmlroot == NULL)
	{
		return DASH_MPD_ERR_ERROR;
	}

	/* start to parse MPD element */
	merr = P_MPD_Parse_MPD(mpd->parsedMPD, xmlroot);
	if(merr != DASH_MPD_ERR_OK)
	{
		switch(merr)
		{
			case DASH_MPD_ERR_ERROR:
			case DASH_MPD_ERR_PARSE_ERROR:
				PrintError("%s(%d) Error : Parsing failed \n", __FUNCTION__, __LINE__);
				return merr;
				break;
			default:
				break;
		}
	}
	PrintExit;
	return DASH_MPD_ERR_OK;
}

DASH_MPD_ERROR_e DASH_MPD_MergeMPD(DASH_MPDPARSER_HANDLE_t current, DASH_MPDPARSER_HANDLE_t newer)
{
	DASH_MPD_ERROR_e merr;

	if(newer->revision != current->revision + 1)
	{
		PrintError("%s(%d) Revision of new MPD is invalid\n", __FUNCTION__, __LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	/* Initial MPD update, just replace current with newer */
	if(current->revision == 0)
	{
		newer->ulSema = current->ulSema;
		newer->state = current->state;
		newer->XlinkDown = current->XlinkDown;
		/************************************************************
		   No need to free parsedMPD and document in Rev.0 MPD(current)
		************************************************************/
		VK_MEM_Memcpy(current, newer, sizeof(DASH_MPDParser_t));
		VK_MEM_Free(newer);
		newer = NULL;
		return DASH_MPD_ERR_OK;
	}

	merr = P_MPD_Merge_MPD(current->parsedMPD, newer->parsedMPD);
	if(merr != DASH_MPD_ERR_OK)
	{
		return DASH_MPD_ERR_MERGE_ERROR;
	}
	else if(merr == DASH_MPD_ERR_OK)
	{
		PrintDebug("%s(%d) Merge complete\n", __FUNCTION__, __LINE__);
	}
	current->revision = newer->revision;

	merr = DASH_MPD_Destroy(newer);
	if(merr != DASH_MPD_ERR_OK)
	{
		return DASH_MPD_ERR_ERROR;
	}
	else if(merr == DASH_MPD_ERR_OK)
	{
		PrintDebug("%s(%d) Destroy complete\n", __FUNCTION__, __LINE__);
	}
	return DASH_MPD_ERR_OK;
}

HINT32 DASH_MPD_GetMPDUpdatePeriod(DASH_MPDPARSER_HANDLE_t mpd)
{
	DASH_MPD_t* parsedMPD = (DASH_MPD_t*)mpd->parsedMPD;
	return parsedMPD->minimumUpdatePeriod;
}

HBOOL DASH_MPD_Get_IsLiveUpdate(DASH_MPDPARSER_HANDLE_t mpd)
{
	DASH_MPD_t* parsedMPD = (DASH_MPD_t*)mpd->parsedMPD;
	return (parsedMPD->minimumUpdatePeriod != 0) && (parsedMPD->type != DASH_MPD_TYPE_STATIC);
}

HINT64 DASH_MPD_GetAvailableStartTime(DASH_MPDPARSER_HANDLE_t mpd)
{
	DASH_MPD_t* parsedMPD = (DASH_MPD_t*)mpd->parsedMPD;
	return parsedMPD->availibilityStartTime;
}

HINT64 DASH_MPD_GetAvailableEndTime(DASH_MPDPARSER_HANDLE_t mpd)
{
	DASH_MPD_t* parsedMPD = (DASH_MPD_t*)mpd->parsedMPD;
	return parsedMPD->availibilityEndTime;
}

DASH_MPD_ERROR_e DASH_MPD_SetState(DASH_MPDPARSER_HANDLE_t mpd, DASH_MPD_STATE_e state)
{
	if(mpd == NULL)
	{
		return DASH_MPD_ERR_ERROR;
	}
	mpd->state = state;
	return DASH_MPD_ERR_OK;
}

DASH_MPD_STATE_e DASH_MPD_GetState(DASH_MPDPARSER_HANDLE_t mpd)
{
	if(mpd == NULL)
	{
		return DASH_MPD_ERR_ERROR;
	}
	return mpd->state;
}

DASH_MPD_ERROR_e DASH_MPD_WaitState(DASH_MPDPARSER_HANDLE_t e, DASH_MPD_STATE_e state)
{
	DASH_MPD_STATE_e d_state;

	if(e == NULL)
	{
		PrintError("%s(%d) Invalid essential handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_MPD_ERR_ERROR;
	}

	while(DASH_MPD_GetState(e) != state)
	{
		d_state = DASH_MPD_GetState(e);
		if( d_state == DASH_MPD_STATE_ERROR)
		{
			return DASH_MPD_ERR_ERROR;
		}
		VK_TASK_Sleep(DASH_MINIMUM_TASK_SLEEP);
	}
	return DASH_MPD_ERR_OK;
}

/* APIs for Controller */
DASH_MPD_ERROR_e DASH_MPD_GetPeriodInfo(DASH_MPDPARSER_HANDLE_t mpd, DASH_Period_t **period, DASH_TIME_t time)
{
	DASH_MPD_t* parsedMPD = (DASH_MPD_t*)mpd->parsedMPD;
	DASH_Period_t* pPeriod = NULL;
	UTIL_DLLNode_t* dllnode = NULL;
	DASH_XLINK_LIST_t *xlink_list;
	HUINT32 nrPeriod;
	HUINT32 loopcnt;

	/* if MPD type is static, we need to check the requested time is over the media duration */
	if(parsedMPD->type == DASH_MPD_TYPE_STATIC)
	{
		if(parsedMPD->mediaPresentationDuration < time)
		{
			PrintError("%s(%d) Not found requested period, requested time could be over the duration of mpd ( time : %"PRId64")\n",__FUNCTION__,__LINE__, time);
			return DASH_MPD_ERR_EOF;
		}
		else if(parsedMPD->mediaPresentationDuration == time)
		{
			PrintError("%s(%d) Attempt to seek the end of media ( time : %"PRId64")\n",__FUNCTION__,__LINE__, time);
			dllnode = UTIL_DLLIST_GetTailNode(&parsedMPD->Period);
			pPeriod = (DASH_Period_t*)dllnode->pData;
			*period = pPeriod;
			return DASH_MPD_ERR_OK;
			//return DASH_MPD_ERR_EOF;
		}
	}

	nrPeriod = UTIL_DLLIST_Size(&parsedMPD->Period);
	if(nrPeriod > 1)
	{
		for(loopcnt = 0; loopcnt < nrPeriod; loopcnt++)
		{
			dllnode = UTIL_DLLIST_Get_n_Node(&parsedMPD->Period, loopcnt);
			pPeriod = (DASH_Period_t*)dllnode->pData;

			/* if pPeriod is not downloaded (that means there exists xlink) */
			if( pPeriod->isXlink == TRUE)
			{
				xlink_list = VK_MEM_Alloc(sizeof(DASH_XLINK_LIST_t));
				if(xlink_list == NULL)
				{
					PrintError("%s(%d) Not enough memory\n",__FUNCTION__,__LINE__);
					return DASH_MPD_ERR_ERROR;
				}
				VK_MEM_Memset(xlink_list, 0x0, sizeof(DASH_XLINK_LIST_t));

				UTIL_DLLIST_Append(&xlink_list->periods, pPeriod);
				/* xlink download callback */
				if(mpd->XlinkDown != NULL)
				{
					mpd->XlinkDown(xlink_list);
				}
				else
				{
					PrintError("%s(%d) Xlink download callback is NULL\n",__FUNCTION__,__LINE__);
				}

				if( pPeriod->isSegmentList == TRUE)
				{
					if(pPeriod->SegmentList.isXlink == TRUE)
					{
						UTIL_DLLIST_Append(&xlink_list->segmentlist, &pPeriod->SegmentList);
						/* xlink download callback */
						if(mpd->XlinkDown != NULL)
						{
							mpd->XlinkDown(xlink_list);
						}
						else
						{
							PrintError("%s(%d) Xlink download callback is NULL\n",__FUNCTION__,__LINE__);
						}
					}
				}

				DASH_MPD_PostProcess(mpd);
				VK_MEM_Free(xlink_list);
				xlink_list = NULL;
			}

			if( pPeriod->start <= time && pPeriod->start + pPeriod->duration > time)
			{
				*period = pPeriod;
				return DASH_MPD_ERR_OK;
			}
		}
		PrintError("%s(%d) Not found requested period\n",__FUNCTION__,__LINE__);
		return DASH_MPD_ERR_ERROR;
	}
	else if (nrPeriod == 1)
	{
		dllnode = UTIL_DLLIST_GetHeadNode(&parsedMPD->Period);
		pPeriod = (DASH_Period_t*)dllnode->pData;

		/* if pPeriod is not downloaded (that means there exists xlink) */
		if( pPeriod->isXlink == TRUE)
		{
			xlink_list = VK_MEM_Alloc(sizeof(DASH_XLINK_LIST_t));
			if(xlink_list == NULL)
			{
				PrintError("%s(%d) Not enough memory\n",__FUNCTION__,__LINE__);
				return DASH_MPD_ERR_ERROR;
			}
			VK_MEM_Memset(xlink_list, 0x0, sizeof(DASH_XLINK_LIST_t));
			UTIL_DLLIST_Append(&xlink_list->periods, pPeriod);

			/* xlink download callback */
			if(mpd->XlinkDown != NULL)
			{
				mpd->XlinkDown(xlink_list);
			}
			else
			{
				PrintError("%s(%d) Xlink download callback is NULL\n",__FUNCTION__,__LINE__);
			}

			if( pPeriod->isSegmentList == TRUE)
			{
				if(pPeriod->SegmentList.isXlink == TRUE)
				{
					UTIL_DLLIST_Append(&xlink_list->segmentlist, &pPeriod->SegmentList);
					/* xlink download callback */
					if(mpd->XlinkDown != NULL)
					{
						mpd->XlinkDown(xlink_list);
					}
					else
					{
						PrintError("%s(%d) Xlink download callback is NULL\n",__FUNCTION__,__LINE__);
					}
				}
			}
			DASH_MPD_PostProcess(mpd);
			VK_MEM_Free(xlink_list);
			xlink_list = NULL;
		}

		*period = pPeriod;
		return DASH_MPD_ERR_OK;
	}
	else
	{
		PrintError("%s(%d) Error : Invalid time or MPD\n", __FUNCTION__, __LINE__);
		return DASH_MPD_ERR_PERIOD_NOTFOUND;
	}

	/* this could be reached by end of live MPD */
	return DASH_MPD_ERR_PERIOD_NOTFOUND;
}

DASH_MPD_ERROR_e DASH_MPD_GetAdaptationSetInfo(DASH_MPDPARSER_HANDLE_t mpd, DASH_AdaptationSet_t **adaptationset, DASH_ADAPTATIONSET_TYPE_e type, DASH_LOCALE_TYPE_e locale, DASH_TIME_t time)
{
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	DASH_AdaptationSet_t* pAdaptation;
	DASH_Period_t* pPeriod;
	UTIL_DLLNode_t* dllnode = NULL;
	DASH_XLINK_LIST_t *xlink_list;
	HUINT32 loopcnt;

	merr = DASH_MPD_GetPeriodInfo(mpd,&pPeriod,time);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}

	//find selected audio, video, subtitle, and so on
	for(loopcnt = 0; loopcnt < UTIL_DLLIST_Size(&pPeriod->AdaptationSet); loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pPeriod->AdaptationSet, loopcnt);
		pAdaptation = (DASH_AdaptationSet_t*)dllnode->pData;

		/* if pAdaptation is not downloaded (that means there exists xlink) */
		if( pAdaptation->isXlink == TRUE)
		{
			xlink_list = VK_MEM_Alloc(sizeof(DASH_XLINK_LIST_t));
			if(xlink_list == NULL)
			{
				PrintError("%s(%d) Not enough memory\n",__FUNCTION__,__LINE__);
				return DASH_MPD_ERR_ERROR;
			}
			VK_MEM_Memset(xlink_list, 0x0, sizeof(DASH_XLINK_LIST_t));
			UTIL_DLLIST_Append(&xlink_list->adaptations, pAdaptation);
			/* xlink download callback */
			if(mpd->XlinkDown != NULL)
			{
				mpd->XlinkDown(xlink_list);
			}
			else
			{
				PrintError("%s(%d) Xlink download callback is NULL\n",__FUNCTION__,__LINE__);
			}

			if( pAdaptation->isSegmentList == TRUE)
			{
				if(pAdaptation->SegmentList.isXlink == TRUE)
				{
					UTIL_DLLIST_Append(&xlink_list->segmentlist, &pAdaptation->SegmentList);
					/* xlink download callback */
					if(mpd->XlinkDown != NULL)
					{
						mpd->XlinkDown(xlink_list);
					}
					else
					{
						PrintError("%s(%d) Xlink download callback is NULL\n",__FUNCTION__,__LINE__);
					}
				}
			}
			VK_MEM_Free(xlink_list);
			xlink_list = NULL;
		}

		if(pAdaptation->trackType == type)
		{
			if(locale != DASH_LOCALE_TYPE_NONE)
			{
				if(pAdaptation->localeType == locale)
				{
					*adaptationset = pAdaptation;
					return DASH_MPD_ERR_OK;
				}
				else
				{
					continue;
				}
			}
			else
			{
				*adaptationset = pAdaptation;
				return DASH_MPD_ERR_OK;
			}
		}
	}
	return DASH_MPD_ERR_ADAPTATION_NOTFOUND;

}

DASH_MPD_ERROR_e DASH_MPD_GetRepresentationSetInfo(DASH_MPDPARSER_HANDLE_t mpd, DASH_Representation_t **representation, DASH_ADAPTATIONSET_TYPE_e type, DASH_LOCALE_TYPE_e locale, DASH_TIME_t time, DASH_BANDWIDTH_t bandwidth)
{
	DASH_AdaptationSet_t* pAdaptation;
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	DASH_Representation_t* pRepresentation = NULL;
	DASH_Representation_t* candidate = NULL;
	UTIL_DLLNode_t* dllnode = NULL;
	DASH_XLINK_LIST_t *xlink_list = NULL;
	HUINT32 loopcnt;
	HINT32 differential = 0x7FFFFFFF;

	merr = DASH_MPD_GetAdaptationSetInfo(mpd, &pAdaptation,type, locale, time);
	if(merr != DASH_MPD_ERR_OK)
	{
		return merr;
	}
	if(pAdaptation == NULL)
	{
		return DASH_MPD_ERR_ERROR;
	}
	dllnode = UTIL_DLLIST_GetHeadNode(&pAdaptation->Representation);
	for(loopcnt = 0; loopcnt < UTIL_DLLIST_Size(&pAdaptation->Representation); loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAdaptation->Representation, loopcnt);
		pRepresentation = (DASH_Representation_t*)dllnode->pData;

		/* if pAdaptation is not downloaded (that means there exists xlink) */
		if( pRepresentation->isSegmentList == TRUE)
		{
			if(pRepresentation->SegmentList.isXlink == TRUE)
			{
				xlink_list = VK_MEM_Alloc(sizeof(DASH_XLINK_LIST_t));
				if(xlink_list == NULL)
				{
					PrintError("%s(%d) Not enough memory\n",__FUNCTION__,__LINE__);
				}
				VK_MEM_Memset(xlink_list, 0x0, sizeof(DASH_XLINK_LIST_t));
				UTIL_DLLIST_Append(&xlink_list->segmentlist, &pRepresentation->SegmentList);
				/* xlink download callback */
				if(mpd->XlinkDown != NULL)
				{
					mpd->XlinkDown(xlink_list);
				}
				else
				{
					PrintError("%s(%d) Xlink download callback is NULL\n",__FUNCTION__,__LINE__);
				}
				VK_MEM_Free(xlink_list);
				xlink_list = NULL;
			}
		}

		/* select representation current */
		if(bandwidth - pRepresentation->bandwidth < 0)
		{
			continue;
		}
		else if(bandwidth - pRepresentation->bandwidth >= 0)
		{
			if( bandwidth - pRepresentation->bandwidth < differential )
			{
				candidate = pRepresentation;
				differential = bandwidth - pRepresentation->bandwidth;
			}
		}
	}

	/* if bandwidth is lower than the lowerst of bandwidth in MPD */
	differential = 0x7FFFFFFF;
	if(candidate == NULL)
	{
		for(loopcnt = 0; loopcnt < UTIL_DLLIST_Size(&pAdaptation->Representation); loopcnt++)
		{
			dllnode = UTIL_DLLIST_Get_n_Node(&pAdaptation->Representation, loopcnt);
			pRepresentation = (DASH_Representation_t*)dllnode->pData;
			if(pRepresentation->bandwidth < differential)
			{
				differential = pRepresentation->bandwidth;
				candidate = pRepresentation;
			}
		}
	}
	*representation = candidate;
	return DASH_MPD_ERR_OK;
}

DASH_BANDWIDTH_t DASH_MPD_GetNextBandwidth(DASH_MPDPARSER_HANDLE_t mpd, DASH_ADAPTATIONSET_TYPE_e type, DASH_LOCALE_TYPE_e locale, DASH_TIME_t time, DASH_BANDWIDTH_t bandwidth)
{
	DASH_AdaptationSet_t* pAdaptation;
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	DASH_MPD_ERROR_e submerr = DASH_MPD_ERR_OK;
	DASH_Representation_t* pRepresentation = NULL;
	DASH_Representation_t* candidate = NULL;
	DASH_Representation_t* current = NULL;
	UTIL_DLLNode_t* dllnode = NULL;
	HINT32 differential = DASH_INT32_MAX;
	HUINT32 loopcnt;

	merr = DASH_MPD_GetAdaptationSetInfo(mpd, &pAdaptation,type, locale, time);
	if(merr != DASH_MPD_ERR_OK)
	{
		//if requested adaptation type is not found, change alternative then try to find again
		if(merr == 	DASH_MPD_ERR_ADAPTATION_NOTFOUND)
		{
			if(type != DASH_ADAPTATIONSET_TYPE_AV)
			{
				type = DASH_ADAPTATIONSET_TYPE_AV;
			}
			else if(type == DASH_ADAPTATIONSET_TYPE_AV)
			{
				type = DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY;
			}
			submerr = DASH_MPD_GetAdaptationSetInfo(mpd, &pAdaptation, type, locale, time);
			if(submerr != DASH_MPD_ERR_OK)
			{
				return submerr;
			}
		}
		else
		{
			return merr;
		}
	}

	dllnode = UTIL_DLLIST_GetHeadNode(&pAdaptation->Representation);
	current = dllnode->pData;

	for(loopcnt = 0; loopcnt < UTIL_DLLIST_Size(&pAdaptation->Representation); loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAdaptation->Representation, loopcnt);
		pRepresentation = (DASH_Representation_t*)dllnode->pData;

		/* select representation current */
		if(bandwidth - pRepresentation->bandwidth < 0)
		{
			continue;
		}
		else if(bandwidth - pRepresentation->bandwidth >= 0)
		{
			if( bandwidth - pRepresentation->bandwidth < differential )
			{
				current = pRepresentation;
				differential = pRepresentation->bandwidth - bandwidth;
			}
		}
	}

	differential = DASH_INT32_MAX;
	for(loopcnt = 0; loopcnt < UTIL_DLLIST_Size(&pAdaptation->Representation); loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAdaptation->Representation, loopcnt);
		pRepresentation = (DASH_Representation_t*)dllnode->pData;

		/* select representation candidate */
		if(pRepresentation->bandwidth - current->bandwidth <= 0)
		{
			continue;
		}
		else if(pRepresentation->bandwidth - current->bandwidth > 0 )
		{
			if( pRepresentation->bandwidth - current->bandwidth < (HINT32)differential )
			{
				candidate = pRepresentation;
				differential = pRepresentation->bandwidth - current->bandwidth;
			}
		}
	}

	if(candidate != NULL)
	{
		PrintDebug("%s(%d) Found next bandwidth(%d) : %d => %d\n", __FUNCTION__, __LINE__, bandwidth, current->bandwidth, candidate->bandwidth);
		return candidate->bandwidth;
	}
	else
	{
		PrintDebug("%s(%d) Not found next bandwidth : %d => DASH_INT32_MAX\n", __FUNCTION__, __LINE__, current->bandwidth);
		return DASH_INT32_MAX;
	}
}

DASH_BANDWIDTH_t DASH_MPD_GetPrevBandwidth(DASH_MPDPARSER_HANDLE_t mpd, DASH_ADAPTATIONSET_TYPE_e type, DASH_LOCALE_TYPE_e locale, DASH_TIME_t time, DASH_BANDWIDTH_t bandwidth)
{
	DASH_AdaptationSet_t* pAdaptation;
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	DASH_MPD_ERROR_e submerr = DASH_MPD_ERR_OK;
	DASH_Representation_t* pRepresentation = NULL;
	DASH_Representation_t* candidate = NULL;
	DASH_Representation_t* current = NULL;
	UTIL_DLLNode_t* dllnode = NULL;
	HINT32 differential = DASH_INT32_MAX;
	HUINT32 loopcnt;

	merr = DASH_MPD_GetAdaptationSetInfo(mpd, &pAdaptation, type, locale, time);
	if(merr != DASH_MPD_ERR_OK)
	{
		//if requested adaptation type is not found, change alternative then try to find again
		if(merr == 	DASH_MPD_ERR_ADAPTATION_NOTFOUND)
		{
			if(type != DASH_ADAPTATIONSET_TYPE_AV)
			{
				type = DASH_ADAPTATIONSET_TYPE_AV;

			}
			else if(type == DASH_ADAPTATIONSET_TYPE_AV)
			{
				type = DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY;
			}
			submerr = DASH_MPD_GetAdaptationSetInfo(mpd, &pAdaptation, type, locale, time);
			if(submerr != DASH_MPD_ERR_OK)
			{
				return submerr;
			}
		}
		else
		{
			return merr;
		}
	}

	dllnode = UTIL_DLLIST_GetHeadNode(&pAdaptation->Representation);
	current = dllnode->pData;

	for(loopcnt = 0; loopcnt < UTIL_DLLIST_Size(&pAdaptation->Representation); loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAdaptation->Representation, loopcnt);
		pRepresentation = (DASH_Representation_t*)dllnode->pData;

		/* select representation current */
		if(bandwidth - pRepresentation->bandwidth < 0)
		{
			continue;
		}
		else if(bandwidth - pRepresentation->bandwidth >= 0)
		{
			if( bandwidth - pRepresentation->bandwidth < differential )
			{
				current = pRepresentation;
				differential = pRepresentation->bandwidth - bandwidth;
			}
		}
	}

	differential = DASH_INT32_MAX;
	for(loopcnt = 0; loopcnt < UTIL_DLLIST_Size(&pAdaptation->Representation); loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&pAdaptation->Representation, loopcnt);
		pRepresentation = (DASH_Representation_t*)dllnode->pData;

		/* select representation candidate */
		if(current->bandwidth - pRepresentation->bandwidth <= 0)
		{
			continue;
		}
		else if(current->bandwidth - pRepresentation->bandwidth > 0 )
		{
			if(current->bandwidth - pRepresentation->bandwidth < differential )
			{
				candidate = pRepresentation;
				differential = current->bandwidth - pRepresentation->bandwidth;
			}
		}
	}
	if(candidate != NULL)
	{
		PrintDebug("%s(%d) Found prev bandwidth(%d) : %d => %d\n", __FUNCTION__, __LINE__, bandwidth, current->bandwidth, candidate->bandwidth);
		return candidate->bandwidth;
	}
	else
	{
		PrintDebug("%s(%d) Not found previous bandwidth : %d => DASH_INT32_MIN\n", __FUNCTION__, __LINE__, current->bandwidth);
		return DASH_INT32_MIN;
	}
}

DASH_SEGMENT_FORMAT_e DASH_MPD_GetSegmentFormat(DASH_MPDPARSER_HANDLE_t mpd)
{
	if(mpd == NULL)
	{
		PrintError("%s(%d) Error : MPD handle is NULL\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_SEGMENT_UNKNOWN;
	}
	return mpd->format;
}

HINT64 DASH_MPD_GetMediaDuration(DASH_MPDPARSER_HANDLE_t mpd)
{
	if(mpd == NULL)
	{
		PrintError("%s(%d) Error : MPD handle is NULL\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_MPD_ERR_ERROR;
	}

	return ((DASH_MPD_t*)mpd->parsedMPD)->mediaPresentationDuration;
}

HINT32 DASH_MPD_GetTotalTracks(DASH_MPDPARSER_HANDLE_t mpd)
{
	UTIL_DLLNode_t* node;
	DASH_Period_t* period;
	if(mpd == NULL)
	{
		PrintExit;
		return DASH_MPD_ERR_ERROR;
	}
	if(UTIL_DLLIST_Size(&((DASH_MPD_t*)mpd->parsedMPD)->Period) != 0)
	{
		node = UTIL_DLLIST_GetHeadNode(&((DASH_MPD_t*)mpd->parsedMPD)->Period);
		period = node->pData;
		return UTIL_DLLIST_Size(&period->AdaptationSet);
	}
	else
	{
		return 0;
	}
}

DASH_CODEC_e DASH_MPD_GetTrackCodec(DASH_MPDPARSER_HANDLE_t mpd, HUINT32 nTrack)
{
	UTIL_DLLNode_t *node = NULL;
	DASH_Period_t *period = NULL;
	DASH_AdaptationSet_t *adaptation = NULL;

	if(mpd == NULL)
	{
		PrintError("%s(%d) Error : MPD handle is NULL\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_CODEC_NONE;
	}
	if(UTIL_DLLIST_Size(&((DASH_MPD_t*)mpd->parsedMPD)->Period) != 0)
	{
		node = UTIL_DLLIST_GetHeadNode(&((DASH_MPD_t*)mpd->parsedMPD)->Period);
		period = node->pData;
		if(nTrack > UTIL_DLLIST_Size(&period->AdaptationSet))
		{
			return DASH_CODEC_NONE;
		}
		node = UTIL_DLLIST_Get_n_Node(&period->AdaptationSet, nTrack);
		adaptation = node->pData;
		return adaptation->CommonAttributesElements.eCodec;
	}
	else
	{
		return DASH_CODEC_NONE;
	}

}

DASH_ADAPTATIONSET_TYPE_e DASH_MPD_GetTrackType(DASH_MPDPARSER_HANDLE_t mpd, HUINT32 nTrack)
{
	UTIL_DLLNode_t *node = NULL;
	DASH_Period_t *period = NULL;
	DASH_AdaptationSet_t *adaptation = NULL;

	if(mpd == NULL)
	{
		PrintError("%s(%d) Error : MPD handle is NULL\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ADAPTATIONSET_TYPE_NONE;
	}
	if(UTIL_DLLIST_Size(&((DASH_MPD_t*)mpd->parsedMPD)->Period) != 0)
	{
		node = UTIL_DLLIST_GetHeadNode(&((DASH_MPD_t*)mpd->parsedMPD)->Period);
		period = node->pData;
		if(nTrack > UTIL_DLLIST_Size(&period->AdaptationSet))
		{
			return DASH_ADAPTATIONSET_TYPE_NONE;
		}
		node = UTIL_DLLIST_Get_n_Node(&period->AdaptationSet, nTrack);
		adaptation = node->pData;
		return adaptation->trackType;
	}
	else
	{
		return DASH_ADAPTATIONSET_TYPE_NONE;
	}
}

DASH_STRING_t DASH_MPD_GetTrackLang(DASH_MPDPARSER_HANDLE_t mpd, HUINT32 nTrack)
{
	UTIL_DLLNode_t *node = NULL;
	DASH_Period_t *period = NULL;
	DASH_AdaptationSet_t *adaptation = NULL;
	DASH_STRING_t str;
	str.str = NULL;
	str.length = 0;

	if(mpd == NULL)
	{
		PrintError("%s(%d) Error : MPD handle is NULL\n", __FUNCTION__, __LINE__);
		PrintExit;
		return str;
	}
	if(UTIL_DLLIST_Size(&((DASH_MPD_t*)mpd->parsedMPD)->Period) != 0)
	{
		node = UTIL_DLLIST_GetHeadNode(&((DASH_MPD_t*)mpd->parsedMPD)->Period);
		period = node->pData;
		if(nTrack > UTIL_DLLIST_Size(&period->AdaptationSet))
		{
			return str;
		}
		node = UTIL_DLLIST_Get_n_Node(&period->AdaptationSet, nTrack);
		adaptation = node->pData;
		return adaptation->lang;
	}
	else
	{
		return str;
	}
}

DASH_TIME_t DASH_MPD_GetTimeScale(DASH_MPDPARSER_HANDLE_t mpd)
{
	return ((DASH_MPD_t*)mpd->parsedMPD)->timeScale;
}

