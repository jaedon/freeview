/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_drmagent.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_DRMAGENT_H___
#define	___MGR_DRMAGENT_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <mgr_common.h>


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define	DRMAGENT_DRMMESSAGE_TYPE_MAX_LEN			256
#define	DRMAGENT_DRMMESSAGE_MAX_LEN					1024
#define	DRMAGENT_DRMMESSAGE_SYSTEMID_MAX_LEN		256


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eMGR_DRMAGENT_MSGTYPE_PRDY = 0,
	eMGR_DRMAGENT_MSGTYPE_NAPRM_CSPG_HEXBIN,
	eMGR_DRMAGENT_MSGTYPE_NAPRM_HDPLUSCA_SC,
	eMGR_DRMAGENT_MSGTYPE_VIEWRIGHT_WEB,
	eMGR_DRMAGENT_MSGTYPE_MAX
}e_MgrDrmAgent_SendMsgType_t;

typedef enum
{
	eMGR_DRMAGENT_SYSTEMID_PRDY = 0,
	eMGR_DRMAGENT_SYSTEMID_NAPRM_6304,
	eMGR_DRMAGENT_SYSTEMID_NAPRM_6211,
	eMGR_DRMAGENT_SYSTEMID_MAX
}e_MgrDrmAgent_SendDRMSystemID_t;

typedef enum
{
	eMGR_DRMAGENT_ELEMENT_DEPTH_ROOT	= 0,
	eMGR_DRMAGENT_ELEMENT_DEPTH_1,
	eMGR_DRMAGENT_ELEMENT_DEPTH_2,
	eMGR_DRMAGENT_ELEMENT_DEPTH_3,
	eMGR_DRMAGENT_ELEMENT_DEPTH_4,
	eMGR_DRMAGENT_ELEMENT_DEPTH_5,
	eMGR_DRMAGENT_ELEMENT_DEPTH_6,
	eMGR_DRMAGENT_ELEMENT_DEPTH_7,
	eMGR_DRMAGENT_ELEMENT_DEPTH_8,
	eMGR_DRMAGENT_ELEMENT_DEPTH_9,
	eMGR_DRMAGENT_ELEMENT_DEPTH_10
}DRMAGENT_ELEMENT_DEPTH_e;

typedef enum
{
	eMGR_DRMAGENT_XML_TAG_NONE=0,
	eMGR_DRMAGENT_XML_TAG_PLAYREADYRESPONSE_START,
	eMGR_DRMAGENT_XML_TAG_PLAYREADYRESPONSE_END,
	eMGR_DRMAGENT_XML_TAG_VIEWRIGHTWEBRESPONSE_START,
	eMGR_DRMAGENT_XML_TAG_VIEWRIGHTWEBRESPONSE_END,
	eMGR_DRMAGENT_XML_TAG_DRM_RESULT_START,
	eMGR_DRMAGENT_XML_TAG_DRM_RESULT_END,
	eMGR_DRMAGENT_XML_TAG_CUSTOMDATA_START,
	eMGR_DRMAGENT_XML_TAG_CUSTOMDATA_END,
	eMGR_DRMAGENT_XML_TAG_LICENSEQUERYSTATE_START,
	eMGR_DRMAGENT_XML_TAG_LICENSEQUERYSTATE_END,
	eMGR_DRMAGENT_XML_TAG_DRM_LICENSE_STATE_START,
	eMGR_DRMAGENT_XML_TAG_DRM_LICENSE_STATE_END,
	eMGR_DRMAGENT_XML_TAG_DATETIME_START,
	eMGR_DRMAGENT_XML_TAG_DATETIME_END,
	eMGR_DRMAGENT_XML_TAG_VAGUE_START,
	eMGR_DRMAGENT_XML_TAG_VAGUE_END,
	eMGR_DRMAGENT_XML_TAG_MAX,
	eMGR_DRMAGENT_XML_ATTRIBUTE_NONE,
	eMGR_DRMAGENT_XML_ATTRIBUTE_XMLNS,
	eMGR_DRMAGENT_XML_ATTRIBUTE_MAX,
	eMGR_DRMAGENT_XML_ELEMENT_NONE,
	eMGR_DRMAGENT_XML_ELEMENT_DRM_RESULT,
	eMGR_DRMAGENT_XML_ELEMENT_MAX
}e_MgrDrmAgent_XmlStructure_t;

typedef enum
{
	eMGR_DRMAGENT_RESULTCODE_SUCCESSFUL = 0,
	eMGR_DRMAGENT_RESULTCODE_UKNOWN_ERROR,
	eMGR_DRMAGENT_RESULTCODE_CANNOT_PROCESS_REQUEST,
	eMGR_DRMAGENT_RESULTCODE_UNKNOWN_MIME_TYPE,
	eMGR_DRMAGENT_RESULTCODE_USER_CONSENT_NEEDED,
	eMGR_DRMAGENT_RESULTCODE_UNKNOWN_DRM_SYSTEM,
	eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT
}e_MgrDrmAgent_ResultCodeType_t;

typedef enum
{
	eMGR_DRMAGENT_PLAYREADY_LICENSE_ACQUISITION = 0,
	eMGR_DRMAGENT_PLAYREADY_DOMAIN_JOIN,
	eMGR_DRMAGENT_PLAYREADY_DOMAIN_LEAVE,
	eMGR_DRMAGENT_PLAYREADY_METERING,
	eMGR_DRMAGENT_PLAYREADY_LAURL_OVERRIDE,
	eMGR_DRMAGENT_PLAYREADY_SET_CUSTOMDATA,
	eMGR_DRMAGENT_PLAYREADY_SET_SOAPHEADER,
	eMGR_DRMAGENT_PLAYREADY_SET_HTTPHEADER,
	eMGR_DRMAGENT_PLAYREADY_REQUEST_EXISTING_RIGHTS,
	eMGR_DRMAGENT_PLAYREADY_NOTSUPPORT
}e_MgrDrmAgent_PlayReady_MsgType_t;

typedef struct
{
	HUINT8				szDrmMsgType[DRMAGENT_DRMMESSAGE_TYPE_MAX_LEN];
	HUINT8				szDrmMsg[DRMAGENT_DRMMESSAGE_MAX_LEN];
	HUINT8				szDrmSystemId[DRMAGENT_DRMMESSAGE_SYSTEMID_MAX_LEN];
} MgrDrmAgentSendDrmMessage_t;

typedef struct
{
	e_MgrDrmAgent_SendMsgType_t eMsgType;
	HUINT8 ucStr[50];
}MgrDrmAgent_SendMsgType_t;

typedef struct
{
	e_MgrDrmAgent_SendDRMSystemID_t eDrmSystemID;
	HUINT8 ucStr[50];
}MgrDrmAgent_SendDrmSystemId_t;

typedef struct DrmagentElement
{
	HUINT8	*pucTagName;
	HUINT32	ulDepth;
	HBOOL	bFlagEnd;
	struct DrmagentElement *ElementList;
}DRMAGENT_ELEMENT_t;

typedef struct
{
	e_MgrDrmAgent_XmlStructure_t eField;
	HUINT8 ucStr[128];
}MgrDrmAgent_ResultFieldItem_t;

const static DRMAGENT_ELEMENT_t s_Element_Depth2_LAURL[] =
{
	{(HUINT8 *)"LA_URL", eMGR_DRMAGENT_ELEMENT_DEPTH_2, TRUE, NULL},
};

const static DRMAGENT_ELEMENT_t s_Element_Depth1[] =
{
	{(HUINT8 *)"LicenseAcquisition", eMGR_DRMAGENT_ELEMENT_DEPTH_1, FALSE, NULL},
	{(HUINT8 *)"JoinDomain", eMGR_DRMAGENT_ELEMENT_DEPTH_1, FALSE, NULL},
	{(HUINT8 *)"LeaveDomain", eMGR_DRMAGENT_ELEMENT_DEPTH_1, FALSE, NULL},
	{(HUINT8 *)"Metering", eMGR_DRMAGENT_ELEMENT_DEPTH_1, FALSE, NULL},
	{(HUINT8 *)"LicenseServerUriOverride", eMGR_DRMAGENT_ELEMENT_DEPTH_1, FALSE, (DRMAGENT_ELEMENT_t *)s_Element_Depth2_LAURL},
	{(HUINT8 *)"SetCustomData", eMGR_DRMAGENT_ELEMENT_DEPTH_1, FALSE, NULL},
	{(HUINT8 *)"SetSoapHeader", eMGR_DRMAGENT_ELEMENT_DEPTH_1, FALSE, NULL},
	{(HUINT8 *)"LicenseQuery", eMGR_DRMAGENT_ELEMENT_DEPTH_1, TRUE, NULL},
};

const static DRMAGENT_ELEMENT_t s_Element_PlayReadyInitiator[] =
{
	{(HUINT8 *)"PlayReadyInitiator", eMGR_DRMAGENT_ELEMENT_DEPTH_ROOT, TRUE, (DRMAGENT_ELEMENT_t *)s_Element_Depth1}
};

const static MgrDrmAgent_SendMsgType_t stDrmType[eMGR_DRMAGENT_MSGTYPE_MAX] =
{
	{eMGR_DRMAGENT_MSGTYPE_PRDY,					"application/vnd.ms-playready.initiator+xml"	},
	{eMGR_DRMAGENT_MSGTYPE_NAPRM_CSPG_HEXBIN,	"application/vnd.oipf.cspg-hexbinary"			},
	{eMGR_DRMAGENT_MSGTYPE_NAPRM_HDPLUSCA_SC,	"hdplusCA_SC"									},
	{eMGR_DRMAGENT_MSGTYPE_VIEWRIGHT_WEB,		"application/viewrightweb.initiator+xml"		},
	/* Add : MsgType */
};

const static MgrDrmAgent_SendDrmSystemId_t stDrmSystemId[eMGR_DRMAGENT_SYSTEMID_MAX] =
{
	{eMGR_DRMAGENT_SYSTEMID_PRDY,			"urn:dvb:casystemid:19219"			},
	{eMGR_DRMAGENT_SYSTEMID_NAPRM_6304,		"urn:dvb:casystemid:6304"			},
	{eMGR_DRMAGENT_SYSTEMID_NAPRM_6211,		"urn:dvb:casystemid:6211"			},
	/* Add : DrmSystemID */
};

const static char s_XmlDeclaration[] = {"<?xml version=\"1.0\" encoding=\"utf-8\"?>"};

const static MgrDrmAgent_ResultFieldItem_t st_DrmResultMsgField[] =
{
	{eMGR_DRMAGENT_XML_TAG_NONE, ""},
	{eMGR_DRMAGENT_XML_TAG_PLAYREADYRESPONSE_START, "<PlayReadyResponse>"},
	{eMGR_DRMAGENT_XML_TAG_PLAYREADYRESPONSE_END, "</PlayReadyResponse>"},
	{eMGR_DRMAGENT_XML_TAG_VIEWRIGHTWEBRESPONSE_START, "<ViewRightWebResponse>"},
	{eMGR_DRMAGENT_XML_TAG_VIEWRIGHTWEBRESPONSE_END, "</ViewRightWebResponse>"},
	{eMGR_DRMAGENT_XML_TAG_DRM_RESULT_START, "<DRM_RESULT>"},
	{eMGR_DRMAGENT_XML_TAG_DRM_RESULT_END, "</DRM_RESULT>"},
	{eMGR_DRMAGENT_XML_TAG_CUSTOMDATA_START, "<CustomData>"},
	{eMGR_DRMAGENT_XML_TAG_CUSTOMDATA_END, "</CustomData>"},
	{eMGR_DRMAGENT_XML_TAG_LICENSEQUERYSTATE_START, "<LicenseQueryState>"},
	{eMGR_DRMAGENT_XML_TAG_LICENSEQUERYSTATE_END, "</LicenseQueryState>"},
	{eMGR_DRMAGENT_XML_TAG_DRM_LICENSE_STATE_START, "<DRM_LICENSE_STATE>"},
	{eMGR_DRMAGENT_XML_TAG_DRM_LICENSE_STATE_END, "</DRM_LICENSE_STATE>"},
	{eMGR_DRMAGENT_XML_TAG_DATETIME_START, "<DATETIME>"},
	{eMGR_DRMAGENT_XML_TAG_DATETIME_END, "</DATETIME>"},
	{eMGR_DRMAGENT_XML_TAG_VAGUE_START, "<VAGUE>"},
	{eMGR_DRMAGENT_XML_TAG_VAGUE_END, "</VAGUE>"},
	{eMGR_DRMAGENT_XML_TAG_MAX, ""},
	{eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, ""},
	{eMGR_DRMAGENT_XML_ATTRIBUTE_XMLNS, "xmlns=\"http://schemas.microsoft.com/DRM/2007/03/protocols/\""},
	{eMGR_DRMAGENT_XML_ATTRIBUTE_MAX, ""},
	{eMGR_DRMAGENT_XML_ELEMENT_NONE, ""},
	{eMGR_DRMAGENT_XML_ELEMENT_DRM_RESULT, "0"},
	{eMGR_DRMAGENT_XML_ELEMENT_MAX, ""},
};



typedef enum
{
	eMEVT_DRMAGENT		= eMEVT_DRMAGENT_START,


	// TODO: eOEVT_ 로 분리되어야 함.
	/* - Description :
			   - Parameters Usage :
				   handle :
				   p1 :
				   p2 :
				   p3 :  */

	/* - Description :
			   - Parameters Usage :
				   handle :
				   p1 :
				   p2 :
				   p3 :  */
	eOEVT_DRMAGENT_REQUESTDRMSYSTEMSTAUTS,


	eMEVT_DRMAGENT_COMMON_RESPONSEDRMMESSAGERESULT,
	eMEVT_DRMAGENT_COMMON_RESPONSEDRMSYSTEMSTATUS,
	eOEVT_DRMAGENT_SENDDRMMESSAGE_NAGRA_PRM,
	eOEVT_DRMAGENT_SENDDRMMESSAGE_PLAYREADY_LAURL,
	eOEVT_DRMAGENT_SENDDRMMESSAGE_PLAYREADY_CUSTOMDATA,
	eOEVT_DRMAGENT_SENDDRMMESSAGE_VIEWRIGHT_WEB,
}DRMAGENT_EVENT_e;

#endif /* !___MGR_DRMAGENT_H___ */

