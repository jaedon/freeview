#define PTS_FLAG_SET					0x02
#define PTS_DTS_FLAG_SET				0x03

#define DVB_SBTL_DATA_ID				0x20
#define DVB_SBTL_STREAM_ID				0x00
#define DVB_SBTL_SYNC_BYTE				0x0f
#define DVB_SBTL_END_MARKER				0xff
#define DVB_SBTL_LEAST_SEGMENT_SIZE		6

#define DVB_SBTL_SEG_PCS				0x10
#define DVB_SBTL_SEG_RCS				0x11
#define DVB_SBTL_SEG_CDS				0x12
#define DVB_SBTL_SEG_ODS				0x13
#define DVB_SBTL_SEG_DDS				0x14
#define DVB_SBTL_SEG_EDS				0x80
#define DVB_SBTL_STUFFING				0xFF

#define SBTL_OBJECT_TYPE_BITMAP			0x00
#define SBTL_OBJECT_TYPE_CHARACTER		0x01
#define SBTL_OBJECT_TYPE_STRING_OF_CHAR	0x02

#define REGION_DEPTH_2BIT				0x01
#define REGION_DEPTH_4BIT				0x02
#define REGION_DEPTH_8BIT				0x03

#define SBTL_DEC_DEFAULT_COLOR			0

#define SBTL_CODING_METHOD_PIXEL		0x00
#define SBTL_CODING_METHOD_STRING		0x01

#define SBTL_2BIT_PIXEL_STRING			0x10
#define SBTL_4BIT_PIXEL_STRING			0x11
#define SBTL_8BIT_PIXEL_STRING			0x12

#define SBTL_MAP_2_TO_4_BIT				0x20
#define SBTL_MAP_2_TO_8_BIT				0x21
#define SBTL_MAP_4_TO_8_BIT				0x22
#define SBTL_END_OF_OBJECT_LINE			0xF0

#define	MAX_SBTL_PIXEL_BIT_SIZE			(1920 * 8)
#define DEFAULT_FONT_SIZE 36
#define DEFAULT_TEXT_POSITON 600


typedef struct  tagOBJECT_SEGMENT
{
    HBIT		u16PageId			: 16;
    HBIT		u16ObjectId			: 16;
    HBIT		u4ObjectVersion		: 4;
    HBIT		u2ObjectCoding		: 2;
    HBIT		u1NonModifyColor	: 1;
    HBIT		u1Reserved			: 1;
    HUINT32		ulObjectSize;
    HUINT8		*pObjectData;
} ObjectSegment_t;


typedef struct  tagOBJECTLIST_t
{
	HBIT		u16ObjectId			: 16;
	HBIT		u2ObjectType		: 2;
	HBIT		u2ObjectPrvFlag		: 2;
	HBIT		u12ObjectHorPos		: 12;
	HBIT		u4Reserved			: 4;
	HBIT		u12ObjectVerPos		: 12;
	HBIT		u8ObjectForeCode	: 8;
	HBIT		u8ObjectBackCode	: 8;
} ObjectList_t;


typedef struct  tagCLUT_ENTRY
{
	HBIT		u8EntryId			: 8;
	HBIT		u1Bit2Clut			: 1;
	HBIT		u1Bit4Clut			: 1;
	HBIT		u1Bit8Clut			: 1;
	HBIT		u4Reserved			: 4;
	HBIT		u1RangeFlag			: 1;
	HBIT		u8Yvalue			: 8;
	HBIT		u8Crvalue			: 8;
	HBIT		u8Cbvalue			: 8;
	HBIT		u8Tvalue			: 8;
} CLUTentry_t;


typedef struct  tagCLUT_SEGMENT
{
	HBIT		u16PageId			: 16;
	HBIT		u8ClutId			: 8;
	HBIT		u4ClutVersion		: 4;
	HBIT		u4Reserved			: 4;
	HUINT32		ulClutNum;
	CLUTentry_t	*pCLUT;
} CLUTSegment_t;


typedef struct  tagREGION_SEGMENT
{
    HBIT		u16PageId			: 16;
    HBIT		u8RegionId			: 8;
    HBIT		u4RegionVersion		: 4;
    HBIT		u1RegionFillFlag	: 1;
    HBIT		u16RegionWidth		: 16;
	HBIT		u16RegionHeight		: 16;
	HBIT		u3RegionLevelComp	: 3;
	HBIT		u3RegionDepth		: 3;
	HBIT		u2Reserved1			: 2;
	HBIT		u8ClutId			: 8;
	HBIT		u8Bit8Code			: 8;
	HBIT		u4Bit4Code			: 4;
	HBIT		u2Bit2Code			: 2;
	HBIT		u2Reserved2			: 2;
    HUINT32		ulObjectNum;
    ObjectList_t	*pObjectList;

	//	Region Frame Buffer
	ONDKSurfaceHandle			surface;
	HBOOL						bSurfaceLock;
	ONDK_Color_t				*color_table;
}RegionSegment_t;


typedef struct  tagREGION_LIST
{
	HBIT		u8RegionId			: 8;
	HBIT		u16RegionHorAddr	: 16;
	HBIT		u16RegionVerAddr	: 16;
} RegionList_t;


typedef struct  tagPAGE_SEGMENT
{
	HBIT		u16PageId			: 16;
	HBIT		u8PageTimeOut		: 8;
	HBIT		u4PageVersion		: 4;
	HBIT		u2PageState			: 2;
	HBIT		u2Rserved			: 2;
	HUINT32		ulRegionNum;
	RegionList_t	*pRgList;
} PageSegment_t;

typedef struct tagDISPLAY_SEGMENT
{
	HBIT		u16pageId 			: 16;
	HBIT		u4DdsVersion		: 4;
	HBIT		u1WindowFlag		: 1;
	HBIT		u3Reserved			: 3;
	HBIT		u16DisplayWidth		: 16;
	HBIT		u16DisplayHeight	: 16;
	HBIT		u16DWHorPosMin		: 16;
	HBIT		u16DWHorPosMax		: 16;
	HBIT		u16DWVerPosMin		: 16;
	HBIT		u16DWVerPosMax		: 16;
} DisplaySegment_t;

#define PAGE_STATE_NORMAL_CASE			0x00
#define PAGE_STATE_ACQUISITION_POINT	0x01
#define PAGE_STATE_MODE_CHANGE			0x10


static	HBOOL		_func_find_page_segment_id(void *_page_id, void *_data)
{
	HUINT32			page_id = (HUINT32)_page_id;
	PageSegment_t	*data = (PageSegment_t*)_data;

	if (data->u16PageId == page_id)
		return TRUE;

	return FALSE;
}

static	PageSegment_t	*local_subtitle_find_page_segment(DVBData_t *dvbInfo, HUINT16 page_id)
{
	HxList_t	*list;

	list = HLIB_LIST_FindEx(dvbInfo->page_segment, (void*)((HUINT32)page_id), _func_find_page_segment_id);
	if (list == NULL)
		return NULL;

	return (PageSegment_t*)HLIB_LIST_Data(list);
}

typedef	struct
{
	HUINT16		page_id;
	HUINT16		region_id;
} _find_region_info_t;

static	HBOOL		_func_find_region_segment_id(void *_findInfo, void *_data)
{
	_find_region_info_t *findInfo = (_find_region_info_t*)_findInfo;
	RegionSegment_t		*data = (RegionSegment_t*)_data;

	if (data->u16PageId == findInfo->page_id)
	{
		if (findInfo->region_id >= 0x100)
			return TRUE;

		if (data->u8RegionId == findInfo->region_id)
			return TRUE;
	}

	return FALSE;
}

static	RegionSegment_t	*local_subtitle_find_region_segment(DVBData_t *dvbInfo, HUINT16 page_id, HUINT16 region_id)
{
	HxList_t		*list;

	_find_region_info_t	stFindInfo;

	stFindInfo.page_id = page_id;
	stFindInfo.region_id = region_id;

	list = HLIB_LIST_FindEx(dvbInfo->region_segment, (void*)&stFindInfo, _func_find_region_segment_id);
	if (list == NULL)
		return NULL;

	return (RegionSegment_t*)HLIB_LIST_Data(list);
}

typedef	struct
{
	HUINT16		page_id;
	HUINT16		clut_id;
} _find_clut_info_t;

static	HBOOL		_func_find_clut_segment_id(void *_findInfo, void *_data)
{
	_find_clut_info_t	*findInfo = (_find_clut_info_t*)_findInfo;
	CLUTSegment_t		*data = (CLUTSegment_t*)_data;

	if (data->u16PageId == findInfo->page_id)
	{
		if (findInfo->clut_id >= 0x100)
			return TRUE;

		if (data->u8ClutId == findInfo->clut_id)
			return TRUE;
	}

	return FALSE;
}
static	CLUTSegment_t	*local_subtitle_find_clut_segment(DVBData_t *dvbInfo, HUINT16 page_id, HUINT16 clut_id)
{
	HxList_t		*list;

	_find_clut_info_t	stFindInfo;

	stFindInfo.page_id = page_id;
	stFindInfo.clut_id = clut_id;
	list = HLIB_LIST_FindEx(dvbInfo->clut_segment, &stFindInfo, _func_find_clut_segment_id);
	if (list == NULL)
		return NULL;

	return (CLUTSegment_t*)HLIB_LIST_Data(list);
}

typedef	struct
{
	HUINT16		page_id;
	HUINT32		object_id;
} _find_object_info_t;

static	HBOOL		_func_find_object_segment_id(void *_findInfo, void *_data)
{
	_find_object_info_t	*findInfo = (_find_object_info_t*)_findInfo;
	ObjectSegment_t		*data = (ObjectSegment_t*)_data;

	if (data->u16PageId == findInfo->page_id)
	{
		if (findInfo->object_id >= 0x10000)
			return TRUE;

		if (data->u16ObjectId == findInfo->object_id)
			return TRUE;
	}

	return FALSE;
}
static	ObjectSegment_t	*local_subtitle_find_object_segment(DVBData_t *dvbInfo, HUINT16 page_id, HUINT32 object_id)
{
	HxList_t		*list;

	_find_object_info_t	stFindInfo;

	stFindInfo.page_id = page_id;
	stFindInfo.object_id = object_id;
	list = HLIB_LIST_FindEx(dvbInfo->object_segment, &stFindInfo, _func_find_object_segment_id);
	if (list == NULL)
		return NULL;

	return (ObjectSegment_t*)HLIB_LIST_Data(list);
}

static	void	local_subtitle_delete_page_segment(DVBData_t *dvbInfo, HUINT16 page_id)
{
	HxList_t	*list = NULL;
	PageSegment_t	*ps = NULL;

	list = HLIB_LIST_FindEx(dvbInfo->page_segment, (void*)((HUINT32)page_id), _func_find_page_segment_id);
	if (list)
	{
		ps = (PageSegment_t*)HLIB_LIST_Data(list);
		HxLOG_Debug("free : pRgList [%X]\n", ps->pRgList);
		if (ps->pRgList)
		{
			AP_Free(ps->pRgList);
			ps->pRgList = NULL;
		}

		dvbInfo->page_segment = HLIB_LIST_Remove(dvbInfo->page_segment, ps);
		AP_Free(ps);
		ps = NULL;
	}
}

static	void	local_subtitle_delete_region_segment(DVBData_t *dvbInfo, HUINT16 page_id, HUINT32 region_id)
{
	RegionSegment_t	*segment = NULL;

	while ( (segment = local_subtitle_find_region_segment(dvbInfo, page_id, region_id)) )
	{
		HxLOG_Debug("free : pObjectList [%X]\n", segment->pObjectList);
		if (segment->pObjectList)
		{
			AP_Free(segment->pObjectList);
			segment->pObjectList = NULL;
		}
		HxLOG_Debug("free : FB [%X]\n", segment->surface);
		if (segment->surface)
		{
			if (segment->bSurfaceLock == TRUE) {
				HxLOG_Critical("bSurfaceLock is LOCK\n");
				ONDK_SURFACE_Unlock(segment->surface);
			}
			if (segment->surface)
			    ONDK_SURFACE_Destroy(segment->surface);
			segment->surface = NULL;
			segment->bSurfaceLock = FALSE;
		}
		HxLOG_Debug("free : color_table [%X]\n", segment->color_table);
		if (segment->color_table)
		{
			AP_Free(segment->color_table);
			segment->color_table = NULL;
		}
		dvbInfo->region_segment = HLIB_LIST_Remove(dvbInfo->region_segment, segment);
		AP_Free(segment);
		segment = NULL;
	}
}

static	void	local_subtitle_delete_clut_segment(DVBData_t *dvbInfo, HUINT16 page_id, HUINT16 clut_id)
{
	CLUTSegment_t	*segment = NULL;

	while ( (segment = local_subtitle_find_clut_segment(dvbInfo, page_id, clut_id)) )
	{
		HxLOG_Debug("free : CLUT [%X]\n", segment->pCLUT);
		if (segment->pCLUT)
		{
			AP_Free(segment->pCLUT);
			segment->pCLUT = NULL;
		}
		dvbInfo->clut_segment = HLIB_LIST_Remove(dvbInfo->clut_segment, segment);
		AP_Free(segment);
		segment = NULL;
	}
}

static	void	local_subtitle_delete_object_segment(DVBData_t *dvbInfo, HUINT16 page_id, HUINT32 object_id)
{
	ObjectSegment_t	*segment = NULL;

	while ( (segment = local_subtitle_find_object_segment(dvbInfo, page_id, object_id)) )
	{
		HxLOG_Debug("free : pObjectData [%X]\n", segment->pObjectData);
		if (segment->pObjectData)
		{
			AP_Free(segment->pObjectData);
			segment->pObjectData = NULL;
		}

		dvbInfo->object_segment = HLIB_LIST_Remove(dvbInfo->object_segment, segment);
		AP_Free(segment);
		segment = NULL;
	}
}

static	void	local_subtitle_delete_all_segment_by_pageid(DVBData_t *dvbInfo, HUINT16 page_id)
{
	HxLOG_Print("(+)local_subtitle_delete_all_segment_by_page_id (%d)\n", page_id);
	local_subtitle_delete_page_segment(dvbInfo, page_id);
	local_subtitle_delete_region_segment(dvbInfo, page_id, 0xFFFF);
	local_subtitle_delete_clut_segment(dvbInfo, page_id, 0xFFFF);
	local_subtitle_delete_object_segment(dvbInfo, page_id, 0xFFFFFFFF);
	HxLOG_Print("(-)local_subtitle_delete_all_segment_by_page_id\n");
}

static	void	local_subtitle_delete_all_segment(DVBData_t *dvbInfo)
{
	HxList_t		*page_list = NULL;
	PageSegment_t	*page_segment = NULL;

	HxLOG_Print("(+)local_subtitle_delete_all_segment\n");
	while ((page_list = dvbInfo->page_segment))
	{
		page_segment = HLIB_LIST_Data(page_list);
		local_subtitle_delete_all_segment_by_pageid(dvbInfo, page_segment->u16PageId);
	}
	HxLOG_Print("(-)local_subtitle_delete_all_segment\n");
}

static	void	local_subtitle_parse_page_segment(DVBData_t *dvbInfo, HUINT8 *p)
{
	HINT32			cnt = 0;
	HUINT16			page_id = 0;
	HUINT32			page_state = 0, segment_len = 0;
	PageSegment_t	*page_segment = NULL;

	page_id = HxMACRO_Get16Bit(p + 2);
	segment_len = HxMACRO_Get16Bit(p + 4) + 6;
	page_state = (HxMACRO_Get8Bit(p + 7) & 0x0C) >> 2;

	if (page_state != PAGE_STATE_NORMAL_CASE)
	{
		local_subtitle_delete_all_segment_by_pageid(dvbInfo, page_id);
	}

	page_segment = local_subtitle_find_page_segment(dvbInfo, page_id);
	if (page_segment != NULL)
	{
		if (page_segment->u4PageVersion == ((HxMACRO_Get8Bit(p + 7) & 0xF0) >> 4))
		{
			HxLOG_Print("Same Page Segment received..\n");
			return;
		}
		else
		{
			local_subtitle_delete_page_segment(dvbInfo, page_id);
		}
	}

	page_segment = (PageSegment_t*)AP_Malloc(sizeof(PageSegment_t));
	if (page_segment == NULL)
	{
		HxLOG_Assert(page_segment);
		return;
	}
	HxLOG_Debug("alloc page_segment [%X]\n", page_segment);

	page_segment->u16PageId		= page_id;
	page_segment->u8PageTimeOut	= HxMACRO_Get8Bit(p + 6);
	page_segment->u4PageVersion	= (HxMACRO_Get8Bit(p + 7) & 0xF0) >> 4;
	page_segment->u2PageState	= page_state;
	page_segment->ulRegionNum	= (HUINT8)(segment_len - 8) / 6;
	page_segment->pRgList		= NULL;

	HxLOG_Debug("\tPage Id : (%d)\n", (int)page_segment->u16PageId);
	HxLOG_Debug("\tPage Time Out : (%d)\n", (int)page_segment->u8PageTimeOut);
	HxLOG_Debug("\tPage Version : (%d)\n", (int)page_segment->u4PageVersion);
	HxLOG_Debug("\tPage State : (%d)\n", (int)page_segment->u2PageState);
	HxLOG_Debug("\tPage Region Num : (%d)\n", (int)page_segment->ulRegionNum);

	if (page_segment->u8PageTimeOut > 10)
	{
		HxLOG_Debug("Page timeout is too long --> [%d sec] --> Forced change to 10 sec\n", page_segment->u8PageTimeOut);
		page_segment->u8PageTimeOut = 10;
	}

	if (page_segment->ulRegionNum != 0)
	{
		page_segment->pRgList = (RegionList_t*)AP_Malloc(page_segment->ulRegionNum * sizeof(RegionList_t));
		if (page_segment->pRgList == NULL)
		{
			page_segment->ulRegionNum = 0;
			AP_Free(page_segment);
			page_segment = NULL;

			HxLOG_Error("page_segment->pRgList AP_Malloc error..\n");
			return;
		}
		HxLOG_Debug("alloc page_segment->pRgList [%X]\n", page_segment->pRgList);
	}

	for (cnt = 0; cnt < page_segment->ulRegionNum; cnt++)
	{
		page_segment->pRgList[cnt].u8RegionId		= HxMACRO_Get8Bit(8 + p + (cnt * 6));
		page_segment->pRgList[cnt].u16RegionHorAddr	= HxMACRO_Get16Bit(8 + p + 2 + (cnt * 6));
		page_segment->pRgList[cnt].u16RegionVerAddr	= HxMACRO_Get16Bit(8 + p + 4 + (cnt * 6));
		HxLOG_Debug("\tPage Region Id (%d), HorAddr (%d), VerAddr (%d)\n",
		(int)page_segment->pRgList[cnt].u8RegionId, (int)page_segment->pRgList[cnt].u16RegionHorAddr, (int)page_segment->pRgList[cnt].u16RegionVerAddr);
	}

	dvbInfo->page_segment = HLIB_LIST_Append(dvbInfo->page_segment, page_segment);
}

static	void	local_subtitle_parse_region_segment(DVBData_t *dvbInfo, HUINT8 *p)
{
	HBOOL		reuse_segment = FALSE;
	HUINT16		page_id;
	HUINT32		segment_len, idx, i;
	HUINT8		region_id, object_type;

	RegionSegment_t		*region_segment = NULL;

	page_id = HxMACRO_Get16Bit(p + 2);
	segment_len = HxMACRO_Get16Bit(p + 4) + 6;

	region_id = HxMACRO_Get8Bit(p + 6);
	region_segment = local_subtitle_find_region_segment(dvbInfo, page_id, region_id);
	if (region_segment != NULL)
	{
		if (region_segment->u4RegionVersion == ((HxMACRO_Get8Bit(p + 7) & 0xF0) >> 4))
		{
			HxLOG_Print("Same Region Segment received..\n");
			return;
		}
//		else
//			local_subtitle_delete_region_segment(dvbInfo, page_id, region_id);
		reuse_segment = TRUE;
	} else
	{
		region_segment = (RegionSegment_t*)AP_Malloc(sizeof(RegionSegment_t));
		if (region_segment == NULL)
		{
			HxLOG_Assert(region_segment);
			return;
		}
		region_segment->surface = NULL;
		region_segment->color_table = NULL;
		region_segment->pObjectList = NULL;
		HxLOG_Debug("alloc region_segment [%X]\n", region_segment);
	}

//	region_segment = (RegionSegment_t*)AP_Malloc(sizeof(RegionSegment_t));
	HxLOG_Assert(region_segment);

	region_segment->u16PageId = page_id;
	region_segment->u8RegionId = region_id;
	region_segment->u4RegionVersion = (HxMACRO_Get8Bit(p + 7) & 0xf0) >> 4;
	region_segment->u1RegionFillFlag = (HxMACRO_Get8Bit(p + 7) & 0x08) >> 3;
	region_segment->u16RegionWidth = (HUINT16) HxMACRO_Get16Bit(p + 8);
	region_segment->u16RegionHeight = (HUINT16) HxMACRO_Get16Bit(p + 10);
	region_segment->u3RegionLevelComp = (HxMACRO_Get8Bit(p + 12) & 0xe0) >> 5;
	region_segment->u3RegionDepth = (HxMACRO_Get8Bit(p + 12) & 0x1c) >> 2;
	region_segment->u8ClutId = HxMACRO_Get8Bit(p + 13);
	region_segment->u8Bit8Code = HxMACRO_Get8Bit(p + 14);
	region_segment->u4Bit4Code = (HxMACRO_Get8Bit(p + 15) & 0xf0) >> 4;
	region_segment->u2Bit2Code = (HxMACRO_Get8Bit(p + 15) & 0x0c) >> 2;
	region_segment->ulObjectNum = 0;
	if (region_segment->pObjectList)
	{
		AP_Free(region_segment->pObjectList);
	}
	region_segment->pObjectList = NULL;

	if (region_segment->u1RegionFillFlag)
	{
		if (region_segment->surface)
			ONDK_SURFACE_Destroy(region_segment->surface);
		if (region_segment->color_table)
		{
			AP_Free(region_segment->color_table);
		}

		region_segment->surface = NULL;
		region_segment->color_table = NULL;
	}

	HxLOG_Debug("\tRegion Id : (%d)\n", (int)region_segment->u8RegionId);
	HxLOG_Debug("\tRegion version : (%d)\n", (int)region_segment->u4RegionVersion);
	HxLOG_Debug("\tRegion Fill flag : (%d)\n", (int)region_segment->u1RegionFillFlag);
	HxLOG_Debug("\tRegion width, height : (%d,%d)\n", (int)region_segment->u16RegionWidth, (int)region_segment->u16RegionHeight);
	HxLOG_Debug("\tRegion level of compatibillity : (%d)\n", (int)region_segment->u3RegionLevelComp);
	HxLOG_Debug("\tRegion Depth : (%d)\n", (int)region_segment->u3RegionDepth);
	HxLOG_Debug("\tRegion CLUT ID : (%d)\n", (int)region_segment->u8ClutId);
	HxLOG_Debug("\tRegion Bit8(%d), Bit4(%d), Bit2(%d)\n", (int)region_segment->u8Bit8Code, (int)region_segment->u4Bit4Code, (int)region_segment->u2Bit2Code);

	for (idx = 16; idx < segment_len; )
	{
	    object_type = (HxMACRO_Get8Bit(p + idx + 2) & 0xc0) >> 6;
	    if ( (object_type == SBTL_OBJECT_TYPE_CHARACTER)
	        || (object_type == SBTL_OBJECT_TYPE_STRING_OF_CHAR) )
	    {
	        idx += 8;
	    }
	    else
	    {
	        idx += 6;
	    }

	    region_segment->ulObjectNum++;
	}

	HxLOG_Debug("\tRegion Number of Object : (%d)\n", (int)region_segment->ulObjectNum);

	if ( region_segment->ulObjectNum == 0 )
	{
		// 새로 Allocate된 것은 List 에 없으므로 아무 영향을 주지 않는다.
		// List 안에 있는 것이라면 반드시 List에서 제거하고 free해야 한다.
		dvbInfo->region_segment = HLIB_LIST_Remove(dvbInfo->region_segment, (const void *)region_segment);

		AP_Free(region_segment);
		return;
	}

	region_segment->pObjectList = (ObjectList_t *)AP_Malloc(region_segment->ulObjectNum * sizeof(ObjectList_t));
	if (region_segment->pObjectList == NULL)
	{
		HxLOG_Assert(region_segment);
		region_segment->pObjectList = NULL;
		return;
	}
	HxLOG_Debug("alloc region_segment object list [%X]\n", region_segment->pObjectList);

	for ( i = 0, idx = 16/* Header Length */; i < region_segment->ulObjectNum; i++ )
	{
	    region_segment->pObjectList[i].u16ObjectId = HxMACRO_Get16Bit(p + idx);
	    region_segment->pObjectList[i].u2ObjectType = (HxMACRO_Get8Bit(p + idx + 2) & 0xc0) >> 6;
	    region_segment->pObjectList[i].u2ObjectPrvFlag = (HxMACRO_Get8Bit(p + idx + 2) & 0x30) >> 4;
	    region_segment->pObjectList[i].u12ObjectHorPos = HxMACRO_Get16Bit(p + idx + 2) & 0x0fff;
	    region_segment->pObjectList[i].u12ObjectVerPos = HxMACRO_Get16Bit(p + idx + 4) & 0x0fff;

		HxLOG_Debug("\tRegion Object ID : (%d)\n", (int)region_segment->pObjectList[i].u16ObjectId);
		HxLOG_Debug("\tRegion Object Type : (%d)\n", (int)region_segment->pObjectList[i].u2ObjectType);

		HxLOG_Debug("\tRegion Object Prv Flag : (%d)\n", (int)region_segment->pObjectList[i].u2ObjectPrvFlag);
		HxLOG_Debug("\tRegion Object HorPos, VerPos (%d,%d)\n", (int)region_segment->pObjectList[i].u12ObjectHorPos,region_segment->pObjectList[i].u12ObjectVerPos);

	    if ( (region_segment->pObjectList[i].u2ObjectType == SBTL_OBJECT_TYPE_CHARACTER)
	        || (region_segment->pObjectList[i].u2ObjectType == SBTL_OBJECT_TYPE_STRING_OF_CHAR) )
	    {
	        region_segment->pObjectList[i].u8ObjectForeCode = HxMACRO_Get8Bit(p + idx + 6);
	        region_segment->pObjectList[i].u8ObjectBackCode = HxMACRO_Get8Bit(p + idx + 7);

	        HxLOG_Debug("\tRegion Object Foreground code : (%d)\n", (int)region_segment->pObjectList[i].u8ObjectForeCode);
	        HxLOG_Debug("\tRegion Object Backgroud code : (%d)\n", (int)region_segment->pObjectList[i].u8ObjectBackCode);

	        idx += 8;
	    }
	    else
	    {
	        idx += 6;
	    }
	}

	/* new region segment를 넣는다. */
	if (reuse_segment == FALSE)
		dvbInfo->region_segment = HLIB_LIST_Append(dvbInfo->region_segment, region_segment);
}

static	void	local_subtitle_parse_clut_segment(DVBData_t *dvbInfo, HUINT8 *p)
{
	HUINT32		segment_len, idx, i;
	HUINT16		page_id;
	HUINT8		clut_id;
	HUINT8		range_flag;

	CLUTSegment_t	*clut_segment = NULL;

	page_id = HxMACRO_Get16Bit(p + 2);
	segment_len = HxMACRO_Get16Bit(p + 4) + 6;
	clut_id = HxMACRO_Get8Bit(p + 6);

	clut_segment = local_subtitle_find_clut_segment(dvbInfo, page_id, clut_id);
	if (clut_segment)
	{
		if (clut_segment->u4ClutVersion == ((HxMACRO_Get8Bit(p + 7) & 0xF0) >> 4))
		{
			HxLOG_Print("Same CLUT Segment received..\n");
			return;
		} else
			local_subtitle_delete_clut_segment(dvbInfo, page_id, clut_id);
	}

	clut_segment = (CLUTSegment_t*)AP_Malloc(sizeof(CLUTSegment_t));
	if (clut_segment == NULL)
	{
		HxLOG_Assert(clut_segment);
		return;
	}
	HxLOG_Debug("alloc clut_segment [%X]\n", clut_segment);

	clut_segment->u16PageId = page_id;
	clut_segment->u8ClutId = HxMACRO_Get8Bit(p + 6);
	clut_segment->u4ClutVersion = (HxMACRO_Get8Bit(p + 7) & 0xf0) >> 4;
	clut_segment->ulClutNum = 0;
	clut_segment->pCLUT = NULL;

	HxLOG_Debug("\tCLUT Id : (%d)\n", (int)clut_segment->u8ClutId);
	HxLOG_Debug("\tCLUT version : (%d)\n", (int)clut_segment->u4ClutVersion);

	for (idx = 8; idx < segment_len; )
	{
	    range_flag = HxMACRO_Get8Bit(p + idx + 1) & 0x01;
	    if (range_flag == 1)
			idx += 6;
		else
			idx += 4;

	    clut_segment->ulClutNum++;
	}

	if (clut_segment->ulClutNum == 0 )
	{
		HxLOG_Debug("Free clut_segment [%X]\n", clut_segment);
		AP_Free(clut_segment);
		return;
	}

	HxLOG_Debug("\tCLUT Number of Entry : (%d)\n", (int)clut_segment->ulClutNum);

	clut_segment->pCLUT = (CLUTentry_t *)AP_Malloc(clut_segment->ulClutNum * sizeof(CLUTentry_t));
	if (clut_segment->pCLUT == NULL)
	{
		HxLOG_Assert(clut_segment->pCLUT);
		clut_segment->ulClutNum = 0;
		clut_segment->pCLUT = NULL;
	}
	HxLOG_Debug("alloc clut_segment->pCLUT [%X]\n", clut_segment->pCLUT);

	for (i = 0, idx = 8; i < clut_segment->ulClutNum; i++ )
	{
	    clut_segment->pCLUT[i].u8EntryId = HxMACRO_Get8Bit(p + idx);
	    clut_segment->pCLUT[i].u1Bit2Clut = (HxMACRO_Get8Bit(p + idx + 1) & 0x80) >> 7;
	    clut_segment->pCLUT[i].u1Bit4Clut = (HxMACRO_Get8Bit(p + idx + 1) & 0x40) >> 6;
	    clut_segment->pCLUT[i].u1Bit8Clut = (HxMACRO_Get8Bit(p + idx + 1) & 0x20) >> 5;
	    clut_segment->pCLUT[i].u1RangeFlag	= HxMACRO_Get8Bit(p + idx + 1) & 0x01;

		HxLOG_Debug("\tCLUT Entry Id : (%d)", (int)clut_segment->pCLUT[i].u8EntryId);
		HxLOG_Debug("\tCLUT Range Flag : (%d)", (int)clut_segment->pCLUT[i].u1RangeFlag);
	    if (clut_segment->pCLUT[i].u1RangeFlag)
	    {
	        clut_segment->pCLUT[i].u8Yvalue	= HxMACRO_Get8Bit(p + idx + 2);
	        clut_segment->pCLUT[i].u8Crvalue = HxMACRO_Get8Bit(p + idx + 3);
	        clut_segment->pCLUT[i].u8Cbvalue = HxMACRO_Get8Bit(p + idx + 4);
	        clut_segment->pCLUT[i].u8Tvalue = HxMACRO_Get8Bit(p + idx + 5);

			HxLOG_Debug("\tY, Cr, Cb, T : (%d,%d,%d,%d)\n",
				(int)clut_segment->pCLUT[i].u8Yvalue, (int)clut_segment->pCLUT[i].u8Crvalue, (int)clut_segment->pCLUT[i].u8Cbvalue, (int)clut_segment->pCLUT[i].u8Tvalue);

	        idx += 6;
	    }
	    else
	    {
	        clut_segment->pCLUT[i].u8Yvalue	= (HxMACRO_Get8Bit(p + idx + 2) & 0xfc) >> 2;
	        clut_segment->pCLUT[i].u8Crvalue = ((HxMACRO_Get8Bit(p + idx + 2) & 0x03) << 2) + ((HxMACRO_Get8Bit(p + idx + 3) & 0xc0) >> 6);
	        clut_segment->pCLUT[i].u8Cbvalue = (HxMACRO_Get8Bit(p + idx + 3) & 0x3c) >> 2;
	        clut_segment->pCLUT[i].u8Tvalue	= HxMACRO_Get8Bit(p + idx + 3) & 0x03;

			HxLOG_Debug("\t\tY, Cr, Cb, T : (%d,%d,%d,%d)\n",
				(int)clut_segment->pCLUT[i].u8Yvalue, (int)clut_segment->pCLUT[i].u8Crvalue, (int)clut_segment->pCLUT[i].u8Cbvalue, (int)clut_segment->pCLUT[i].u8Tvalue);

	        idx += 4;
	    }
	}

	dvbInfo->clut_segment = HLIB_LIST_Append(dvbInfo->clut_segment, clut_segment);
}

static	void	local_subtitle_parse_object_segment(DVBData_t *dvbInfo, HUINT8 *p)
{
	HUINT32		segment_len;
	HUINT16		page_id;
	HUINT16		object_id;
	ObjectSegment_t		*object_segment = NULL;

	page_id = HxMACRO_Get16Bit(p + 2);
	segment_len = HxMACRO_Get16Bit(p + 4) + 6;
	object_id = HxMACRO_Get16Bit(p + 6);

	object_segment = local_subtitle_find_object_segment(dvbInfo, page_id, object_id);
	if (object_segment)
	{
		if (object_segment->u4ObjectVersion == ((HxMACRO_Get8Bit(p + 8) & 0xF0) >> 4))
		{
			HxLOG_Print("Same CLUT Segment received..\n");
			return;
		} else
			local_subtitle_delete_object_segment(dvbInfo, page_id, object_id);
	}

	object_segment = (ObjectSegment_t *)AP_Malloc(sizeof(ObjectSegment_t));
	if (object_segment == NULL)
	{
		HxLOG_Assert(object_segment);
		return;
	}
	HxLOG_Debug("alloc object_segment [%X]\n", object_segment);

	object_segment->u16PageId = page_id;
	object_segment->u16ObjectId = HxMACRO_Get16Bit(p + 6);
	object_segment->u4ObjectVersion = (HxMACRO_Get8Bit(p + 8) & 0xf0)>> 4;
	object_segment->u2ObjectCoding = (HxMACRO_Get8Bit(p + 8) & 0x0c) >> 2;
	object_segment->u1NonModifyColor = (HxMACRO_Get8Bit(p + 8) & 0x02) >> 1;
	object_segment->ulObjectSize = segment_len - 9;
	object_segment->pObjectData = NULL;

	HxLOG_Debug("\tObject Id : (%d)\n", (int)object_segment->u16ObjectId);
	HxLOG_Debug("\tObject version : (%d)\n", (int)object_segment->u4ObjectVersion);
	HxLOG_Debug("\tObject coding method : (%d)\n", (int)object_segment->u2ObjectCoding);
	HxLOG_Debug("\tObject non modfying color flag : (%d)\n", (int)object_segment->u1NonModifyColor);
	HxLOG_Debug("\tObject size : (%d)\n", (int)object_segment->ulObjectSize);

	if (object_segment->ulObjectSize == 0 )
	{
	    AP_Free(object_segment);
		object_segment = NULL;
		return;
	}

	object_segment->pObjectData = (HUINT8 *)AP_Malloc(object_segment->ulObjectSize);
	if (object_segment->pObjectData == NULL)
	{
		HxLOG_Assert(object_segment->pObjectData);
		object_segment->pObjectData = NULL;
	}

	HxLOG_Debug("alloc object_segment->pObjectData [%X]\n", object_segment->pObjectData);

	if (object_segment->pObjectData)
	{
		/* Object data copy */
		HxSTD_MemCopy(object_segment->pObjectData, p + 9, object_segment->ulObjectSize);
	}
	else
	{
		object_segment->pObjectData = NULL;
	}

	/* Object segment 등록 */
	dvbInfo->object_segment = HLIB_LIST_Append(dvbInfo->object_segment, object_segment);
}

static	void	local_subtitle_parse_display_segment(DVBData_t *dvbInfo, HUINT8 *p)
{
//	HUINT32		segment_len;
	HUINT16		page_id;

	DisplaySegment_t	*display_segment;

	page_id = HxMACRO_Get16Bit(p + 2);
//	segment_len = HxMACRO_Get16Bit(p + 4) + 6;

	display_segment = dvbInfo->display_segment;
	if (display_segment == NULL)
	{
		display_segment = (DisplaySegment_t*)AP_Malloc(sizeof(DisplaySegment_t));
		if (display_segment == NULL)
		{
			HxLOG_Assert(display_segment);
			display_segment = NULL;
			return;
		}
		HxSTD_MemSet(display_segment, 0, sizeof(DisplaySegment_t));
	}

	HxLOG_Debug("alloc display_segment [%X]\n", display_segment);

	display_segment->u16pageId	= page_id;
	display_segment->u4DdsVersion = (HxMACRO_Get8Bit(p + 6) & 0xF0) >> 4;
	display_segment->u1WindowFlag = (HxMACRO_Get8Bit(p + 6) & 0x08) >> 3;
	display_segment->u16DisplayWidth = HxMACRO_Get16Bit(p + 7) + 1;
	display_segment->u16DisplayHeight = HxMACRO_Get16Bit(p + 9) + 1;

	HxLOG_Debug("\tDisplay version : (%d)\n", (int)display_segment->u4DdsVersion);
	HxLOG_Debug("\tDisplay window flag : (%d)\n", (int)display_segment->u1WindowFlag);
	HxLOG_Debug("\tDisplay width, height : (%d,%d)\n", (int)display_segment->u16DisplayWidth, (int)display_segment->u16DisplayHeight);
	if (display_segment->u1WindowFlag)
	{
		display_segment->u16DWHorPosMin = HxMACRO_Get16Bit(p + 11);
	    display_segment->u16DWHorPosMax = HxMACRO_Get16Bit(p + 13);
	    display_segment->u16DWVerPosMin = HxMACRO_Get16Bit(p + 15);
	    display_segment->u16DWVerPosMax = HxMACRO_Get16Bit(p + 17);

	    HxLOG_Debug("\tDisplay HorPos Min & Max (%d:%d) VerPos Min & Max(%d:%d)\n",
	    	(int)display_segment->u16DWHorPosMin, (int)display_segment->u16DWHorPosMax, (int)display_segment->u16DWVerPosMin, (int)display_segment->u16DWVerPosMax);

	    HxLOG_Debug("\tDisplay x, y, w, h (%d:%d:%d:%d)\n", 	(int)display_segment->u16DWHorPosMin,
	    														(int)display_segment->u16DWVerPosMin,
	    														(int)display_segment->u16DWHorPosMax - (int)display_segment->u16DWHorPosMin + 1,
	    														(int)display_segment->u16DWVerPosMax - display_segment->u16DWVerPosMin + 1);
	}

	dvbInfo->display_segment = display_segment;
}

static	void	local_subtitle_parse_end_segment(DVBData_t *dvbInfo, HUINT8 *p)
{
//	HUINT16		page_id;

//	page_id = HxMACRO_Get16Bit(p + 2);

	dvbInfo->end_of_segment = TRUE;
}

static	void	m_subtitle_decode_dvb(DVBData_t *pstDVBInfo, HUINT8 *pucBuffer, HINT32 size)
{
	HUINT32		packet_size = 0;
	HUINT8		packet_header_len = 0;
	HUINT8		pts_dts_flags = 0;
	HUINT32		pts_size = 0;
	HUINT8		data_identifier = 0, subtitle_stream_id = 0;
//	HUINT8		end_of_marker = 0;
	HUINT8		*p = NULL;

	packet_size = size;
	packet_header_len = HxMACRO_Get8Bit(pucBuffer + 8);
	if (packet_size < ((HUINT32)packet_header_len + 7 + 3))
	{
		HxLOG_Error("Packet size error..\n");
		return;
	}
	pts_dts_flags = (HUINT8)((HxMACRO_Get8Bit(pucBuffer + 7) & 0xC0) >> 6);
	if (pts_dts_flags == PTS_FLAG_SET)
	{
		pts_size = 5;
	}
	else if (pts_dts_flags == PTS_DTS_FLAG_SET)
	{
		HxLOG_Critical("Invalid PTS/DTS Flag..\n");
		return;
	}
	else
	{
		pts_size = 0;
	}

	data_identifier = HxMACRO_Get8Bit(pucBuffer + 9 + pts_size);
	subtitle_stream_id = HxMACRO_Get8Bit(pucBuffer + 9 + pts_size + 1);
//	end_of_marker = HxMACRO_Get8Bit(pucBuffer + packet_size - 1);

	/*
	*  End of Marker Value가 간혹 0xff가 아닌 다른 값으로 들어와 간혹 일부 글자가 안 나오는 경우가
	*  있음. ETSI EN 300 743 V1.3.1에는 '111 111'로 약간 모호하게 기술되어 있음. 그래서인지 이 값을
	*  0xbb or 0xe or 0x0이란 값으로 보낸는 PES가 존재하여 체크 하는 부분을 Skip함.
	*/
	if ((data_identifier != DVB_SBTL_DATA_ID) || (subtitle_stream_id != DVB_SBTL_STREAM_ID))
	{
		HxLOG_Error("Check PES Packet...\n");
		return;
	}

	/*  header length + header length position */
	packet_size -= (9 + pts_size + 2);
	p = pucBuffer + 9 + pts_size + 2;

	{
		HINT32	dummy_cnt = 0;
		HINT32	segment_len = 0;
		HUINT16	page_id = 0;

		while (packet_size >= DVB_SBTL_LEAST_SEGMENT_SIZE)
		{
			if (*p == DVB_SBTL_SYNC_BYTE)
			{
				page_id = HxMACRO_Get16Bit(p + 2);
				segment_len = HxMACRO_Get16Bit(p + 4) + 6;
				if ((page_id == pstDVBInfo->usCompPageId) || (page_id == pstDVBInfo->usAncPageId))
				{
					switch (*(p + 1))
					{
						case	DVB_SBTL_SEG_PCS:			//	0x10
							HxLOG_Print("(+)DVB_SBTL_SEG_PCS\n");
							local_subtitle_parse_page_segment(pstDVBInfo, p);
							HxLOG_Print("(-)DVB_SBTL_SEG_PCS\n");
							break;
						case	DVB_SBTL_SEG_RCS:			//	0x11
							HxLOG_Print("(+)DVB_SBTL_SEG_RCS\n");
							local_subtitle_parse_region_segment(pstDVBInfo, p);
							HxLOG_Print("(-)DVB_SBTL_SEG_RCS\n");
							break;
						case	DVB_SBTL_SEG_CDS:			//	0x12
							HxLOG_Print("(+)DVB_SBTL_SEG_CDS\n");
							local_subtitle_parse_clut_segment(pstDVBInfo, p);
							HxLOG_Print("(-)DVB_SBTL_SEG_CDS\n");
							break;
						case	DVB_SBTL_SEG_ODS:			//	0x13
							HxLOG_Print("(+)DVB_SBTL_SEG_ODS\n");
							local_subtitle_parse_object_segment(pstDVBInfo, p);
							HxLOG_Print("(-)DVB_SBTL_SEG_ODS\n");
							break;
						case	DVB_SBTL_SEG_DDS:			//	0x14
							HxLOG_Print("(+)DVB_SBTL_SEG_DDS\n");
							local_subtitle_parse_display_segment(pstDVBInfo, p);
							HxLOG_Print("(-)DVB_SBTL_SEG_DDS\n");
							break;
						case	DVB_SBTL_SEG_EDS:			//	0x80
							HxLOG_Print("(+)DVB_SBTL_SEG_EDS\n");
							local_subtitle_parse_end_segment(pstDVBInfo, p);
							HxLOG_Print("(-)DVB_SBTL_SEG_EDS\n");
							break;
						case	DVB_SBTL_STUFFING:			//	0xFF
							HxLOG_Print("(+)DVB_SBTL_STUFFING\n");
							local_subtitle_delete_all_segment(pstDVBInfo);
							HxLOG_Print("(-)DVB_SBTL_STUFFING\n");
							break;
						default:
							segment_len = 1;
							break;
					}
				}

				if (packet_size < segment_len)
				{
					HxLOG_Critical("Parsing Fail : Bad Signal???\n");
					return;
				}

				p += segment_len;
				packet_size -= segment_len;
			} else
			{
				if (*p != DVB_SBTL_STUFFING)
				{
					/* Stuffing이 아닌 데이터가 DVB_SBTL_STUFFING 이상 온다면 잘 못 된 data로 간주하여
					더이상 parsing을 하지 않는다. */
					dummy_cnt++;
					if (dummy_cnt > DVB_SBTL_LEAST_SEGMENT_SIZE)
						break;
				}

				p++;
				packet_size--;
			}
		}
	}
}

static HUINT8	_YCrCbT2RGB_R(HUINT8 Y, HUINT8 Cr)
{
	HINT32	res;

	res = (1164 * (Y - 16) + 1596 * (Cr - 128)) / 1000;

	if ((res < 0) || (Y == 0))
	{
		return 0;
	}

	if (res > 255)
	{
		return 255;
	}

	return (HUINT8)res;
}


static HUINT8	_YCrCbT2RGB_G(HUINT8 Y, HUINT8 Cr, HUINT8 Cb)
{
	HINT32	res;

	res = (1164 * (Y - 16) - 813 * (Cr - 128) - 391 * (Cb - 128)) / 1000;

	if ((res < 0) || (Y == 0))
	{
		return 0;
	}

	if (res > 255)
	{
		return 255;
	}

	return (HUINT8)res;
}


static HUINT8	_YCrCbT2RGB_B(HUINT8 Y, HUINT8 Cb)
{
	HINT32	res;

	res = (1164 * (Y - 16) + 2018 * (Cb - 128)) / 1000;

	if ((res < 0) || (Y == 0))
	{
		return 0;
	}

	if (res > 255)
	{
		return 255;
	}

	return (HUINT8)res;
}


static HUINT8	_YCrCbT2RGB_A(HUINT8 Y, HUINT8 T)
{
	HINT32	res;

	/*
	 *  A value of zero in the Y_value field signals full transparency.
	 *  T_value : A value of zero identifies no transparency. The maximum
	 *  value plus one would correspond to full transparency. Full transparency
	 *  is acquired through a value of zero in the Y_value field.
	 *  - [ETSI EN 300 743 V1.3.1]
	 */
	res = (255 - T);
	if (Y == 0)
	{
		return 0;
	}

	return (HUINT8)res;
}

static	ONDK_Color_t	_default_clut_2[] =
{
	0x00000000,	0xFF000000, 0xFFFFFFFF,	0xFF808080,
};

static	ONDK_Color_t	_default_clut_4[] =
{
	0x00000000,	0xFF0000FF,	0xFF00FF00,	0xFF00FFFF,
	0xFFFF0000,	0xFFFF00FF,	0xFFFFFF00,	0xFFFFFFFF,
	0x00000000,	0xFF000080,	0xFF008000,	0xFF008080,
	0xFF800000,	0xFF800080,	0xFF808000,	0xFF808080,
};

static	ONDK_Color_t	_default_clut_8[] =
{
	0x00000000,	0x200000ff,	0x2000ff00,	0x2000ffff,
	0x20ff0000,	0x20ff00ff,	0x20ffff00,	0x20ffffff,
	0x40000000,	0x40000055,	0x40005500,	0x40005555,
	0x40550000,	0x40550055,	0x40555500,	0x40555555,
	0xFF0000aa,	0xFF0000ff,	0xFF0055aa,	0xFF0055ff,
	0xFF5500aa,	0xFF5500ff,	0xFF5555aa,	0xFF5555ff,
	0x400000aa,	0x400000ff,	0x400055aa,	0x400055ff,
	0x405500aa,	0x405500ff,	0x405555aa,	0x405555ff,
	0xFF00aa00,	0xFF00aa55,	0xFF00ff00,	0xFF00ff55,
	0xFF55aa00,	0xFF55aa55,	0xFF55ff00,	0xFF55ff55,
	0x4000aa00,	0x4000aa55,	0x4000ff00,	0x4000ff55,
	0x4055aa00,	0x4055aa55,	0x4055ff00,	0x4055ff55,
	0xFF00aaaa,	0xFF00aaff,	0xFF00ffaa,	0xFF00ffff,
	0xFF55aaaa,	0xFF55aaff,	0xFF55ffaa,	0xFF55ffff,
	0x4000aaaa,	0x4000aaff,	0x4000ffaa,	0x4000ffff,
	0x4055aaaa,	0x4055aaff,	0x4055ffaa,	0x4055ffff,
	0xFFaa0000,	0xFFaa0055,	0xFFaa5500,	0xFFaa5555,
	0xFFff0000,	0xFFff0055,	0xFFff5500,	0xFFff5555,
	0x40aa0000,	0x40aa0055,	0x40aa5500,	0x40aa5555,
	0x40ff0000,	0x40ff0055,	0x40ff5500,	0x40ff5555,
	0xFFaa00aa,	0xFFaa00ff,	0xFFaa55aa,	0xFFaa55ff,
	0xFFff00aa,	0xFFff00ff,	0xFFff55aa,	0xFFff55ff,
	0x40aa00aa,	0x40aa00ff,	0x40aa55aa,	0x40aa55ff,
	0x40ff00aa,	0x40ff00ff,	0x40ff55aa,	0x40ff55ff,
	0xFFaaaa00,	0xFFaaaa55,	0xFFaaff00,	0xFFaaff55,
	0xFFffaa00,	0xFFffaa55,	0xFFffff00,	0xFFffff55,
	0x40aaaa00,	0x40aaaa55,	0x40aaff00,	0x40aaff55,
	0x40ffaa00,	0x40ffaa55,	0x40ffff00,	0x40ffff55,
	0xFFaaaaaa,	0xFFaaaaff,	0xFFaaffaa,	0xFFaaffff,
	0xFFffaaaa,	0xFFffaaff,	0xFFffffaa,	0xFFffffff,
	0x40aaaaaa,	0x40aaaaff,	0x40aaffaa,	0x40aaffff,
	0x40ffaaaa,	0x40ffaaff,	0x40ffffaa,	0x40ffffff,
	0xFF808080,	0xFF8080aa,	0xFF80aa80,	0xFF80aaaa,
	0xFFaa8080,	0xFFaa80aa,	0xFFaaaa80,	0xFFaaaaaa,
	0xFF000000,	0xFF00002a,	0xFF002a00,	0xFF002a2a,
	0xFF2a0000,	0xFF2a002a,	0xFF2a2a00,	0xFF2a2a2a,
	0xFF8080d5,	0xFF8080ff,	0xFF80aad5,	0xFF80aaff,
	0xFFaa80d5,	0xFFaa80ff,	0xFFaaaad5,	0xFFaaaaff,
	0xFF000055,	0xFF00007f,	0xFF002a55,	0xFF002a7f,
	0xFF2a0055,	0xFF2a007f,	0xFF2a2a55,	0xFF2a2a7f,
	0xFF80d580,	0xFF80d5aa,	0xFF80ff80,	0xFF80ffaa,
	0xFFaad580,	0xFFaad5aa,	0xFFaaff80,	0xFFaaffaa,
	0xFF005500,	0xFF00552a,	0xFF007f00,	0xFF007f2a,
	0xFF2a5500,	0xFF2a552a,	0xFF2a7f00,	0xFF2a7f2a,
	0xFF80d5d5,	0xFF80d5ff,	0xFF80ffd5,	0xFF80ffff,
	0xFFaad5d5,	0xFFaad5ff,	0xFFaaffd5,	0xFFaaffff,
	0xFF005555,	0xFF00557f,	0xFF007f55,	0xFF007f7f,
	0xFF2a5555,	0xFF2a557f,	0xFF2a7f55,	0xFF2a7f7f,
	0xFFd58080,	0xFFd580aa,	0xFFd5aa80,	0xFFd5aaaa,
	0xFFff8080,	0xFFff80aa,	0xFFffaa80,	0xFFffaaaa,
	0xFF550000,	0xFF55002a,	0xFF552a00,	0xFF552a2a,
	0xFF7f0000,	0xFF7f002a,	0xFF7f2a00,	0xFF7f2a2a,
	0xFFd580d5,	0xFFd580ff,	0xFFd5aad5,	0xFFd5aaff,
	0xFFff80d5,	0xFFff80ff,	0xFFffaad5,	0xFFffaaff,
	0xFF550055,	0xFF55007f,	0xFF552a55,	0xFF552a7f,
	0xFF7f0055,	0xFF7f007f,	0xFF7f2a55,	0xFF7f2a7f,
	0xFFd5d580,	0xFFd5d5aa,	0xFFd5ff80,	0xFFd5ffaa,
	0xFFffd580,	0xFFffd5aa,	0xFFffff80,	0xFFffffaa,
	0xFF555500,	0xFF55552a,	0xFF557f00,	0xFF557f2a,
	0xFF7f5500,	0xFF7f552a,	0xFF7f7f00,	0xFF7f7f2a,
	0xFFd5d5d5,	0xFFd5d5ff,	0xFFd5ffd5,	0xFFd5ffff,
	0xFFffd5d5,	0xFFffd5ff,	0xFFffffd5,	0xFFffffff,
	0xFF555555,	0xFF55557f,	0xFF557f55,	0xFF557f7f,
	0xFF7f5555,	0xFF7f557f,	0xFF7f7f55,	0xFF7f7f7f,
};

static HERROR local_subtitle_GetBitFlagSub(HUINT8 *ptr, HUINT32 size, HUINT32 pos, HUINT32 *val)
{
	HUINT8	ch;
	HUINT32	bytePos;

	if ( (size > 0) && (pos <= size) )
	{
		bytePos = pos >> 3;			/* / 8 */
		ch = 0x80 >> (pos & 0x07);	/* % 8 */

		if (ptr[bytePos] & ch)
		{
			*val = 1;
		}
		else
		{
			*val = 0;
		}

		return ERR_OK;
	}

	HxLOG_Error("Get Bit error!. pos(%d), size(%d)", pos, size);

	return ERR_FAIL;
}


static HINT32 local_subtitle_GetBitData(HUINT8 *ptr, HUINT32 *pos, HINT32 cnt)
{
	HERROR 	hErr;
	HINT32	i, res, val;

	res = 0;

	for (i = 0; i < cnt; i++)
	{
		hErr = local_subtitle_GetBitFlagSub(ptr, MAX_SBTL_PIXEL_BIT_SIZE, *pos + i, &val);
		if ( hErr != ERR_OK )
		{
			return 0;
		}

		res = (res << 1) + val;
	}

	*pos = *pos + cnt;

	return res;
}

static	void	local_subtitle_put_pixel(HUINT32 **ppulAddr, ONDK_Color_t color, HINT32 cnt)
{
	HINT32	i;
	HUINT32	*p;

	p = *ppulAddr;
	for (i = 0; i < cnt; i++)
	{
		*p++ = color;
	}

	*ppulAddr = p;
}

static	void	local_subtitle_decode2bpCode(HUINT32 *target_addr, HUINT8 *pData, HINT32 *cur_index, HUINT8 *map_table, ONDK_Color_t *color_table)
{
	HUINT32		nBitPos = 0, *p = NULL;
	HINT32		val = 0, runLen = 0;

	p = target_addr;
	nBitPos = 0;

	if (color_table == NULL)
	{
		HxLOG_Critical("Fail color_table is NULL.\n");
		return;
	}

	while (1)
	{
		val = local_subtitle_GetBitData(pData, &nBitPos, 2);
		if (val != 0)
		{
			val = (map_table == NULL) ? val : map_table[val];
			local_subtitle_put_pixel(&p, color_table[val], 1);
		}
		else
		{
			val = local_subtitle_GetBitData(pData, &nBitPos, 1);
			if (val == 0x01)
			{
				runLen = local_subtitle_GetBitData(pData, &nBitPos, 3);
				val = local_subtitle_GetBitData(pData, &nBitPos, 2);
				val = (map_table == NULL) ? val : map_table[val];

				local_subtitle_put_pixel(&p, color_table[val], runLen + 3);
			}
			else
			{
				val = local_subtitle_GetBitData(pData, &nBitPos, 1);
				if (val == 0x00)
				{
					val = local_subtitle_GetBitData(pData, &nBitPos, 2);
					if (val == 0x02)
					{
						runLen = local_subtitle_GetBitData(pData, &nBitPos, 4);
						val = local_subtitle_GetBitData(pData, &nBitPos, 2);
						val = (map_table == NULL) ? val : map_table[val];

						local_subtitle_put_pixel(&p, color_table[val], runLen + 12);
					}
					else if (val == 0x03)
					{
						runLen = local_subtitle_GetBitData(pData, &nBitPos, 8);
						val = local_subtitle_GetBitData(pData, &nBitPos, 2);
						val = (map_table == NULL) ? val : map_table[val];

						local_subtitle_put_pixel(&p, color_table[val], runLen + 29);
					}
					else if (val == 0x00)
					{
						// WBH - make byte alignment only in case of byte alignement fail. 2005. 11. 24 at SkyD (ZDF channel)
						if ( nBitPos & 0x07	)
						{
							*cur_index = *cur_index + ((nBitPos >> 3) + 1) + 1;
						}
						else
						{
							*cur_index = *cur_index + (nBitPos >> 3) + 1;
						}

						break;
					}
					else if (val == 0x01)
					{
						val = (map_table == NULL) ? 0 : map_table[0];

						local_subtitle_put_pixel(&p, color_table[val], 2);
					}
				}
				else
				{
					val = (map_table == NULL) ? 0 : map_table[0];
					local_subtitle_put_pixel(&p, color_table[val], 1);
				}
			}
		}
	}
}

static	void	local_subtitle_decode4bpCode(HUINT32 *target_addr, HUINT8 *pData, HINT32 *cur_index, HUINT8 *map_table, ONDK_Color_t *color_table)
{
	HUINT32		nBitPos = 0, *p = NULL;
	HINT32		val = 0, runLen = 0;

	p = target_addr;
	nBitPos = 0;

	if (color_table == NULL)
	{
		HxLOG_Critical("Fail color_table is NULL.\n");
		return;
	}

	while (1)
	{
		val = local_subtitle_GetBitData(pData, &nBitPos, 4);
		if (val != 0)
		{
			val = (map_table == NULL) ? val : map_table[val];

			/* 1byte write */
			local_subtitle_put_pixel(&p, color_table[val], 1);
		}
		else
		{
			val = local_subtitle_GetBitData(pData, &nBitPos, 1);
			if (val == 0x00)
			{
				runLen = local_subtitle_GetBitData(pData, &nBitPos, 3);
				if (runLen != 0x00)
				{
					val = (map_table == NULL) ? 0 : map_table[0];

					/* [OK] 0000 0LLL L pixels (3-9) in colour 0 (L>0)	*/
					runLen += 2;
					local_subtitle_put_pixel(&p, color_table[val], runLen);
				}
				else
				{
					/* [OK] 0000 0000 end of 4-bit/pixel_code_string*/
					// WBH - make byte alignment only in case of byte alignement fail. 2005. 11. 24 at SkyD (ZDF channel)
					if ( nBitPos & 0x07 )
					{
						*cur_index = *cur_index + ((nBitPos >> 3) + 1) + 1;
					}
					else
					{
						*cur_index = *cur_index + (nBitPos >> 3) + 1;
					}

					break;
				}
			}
			else
			{
				val = local_subtitle_GetBitData(pData, &nBitPos, 1);
				if (val == 0x00)
				{
					/* [OK] 0000 10LL CCCC L pixels (4-7) in colour C */
					runLen = local_subtitle_GetBitData(pData, &nBitPos, 2) + 4;
					val = local_subtitle_GetBitData(pData, &nBitPos, 4);
					val = (map_table == NULL) ? val : map_table[val];
					local_subtitle_put_pixel(&p, color_table[val], runLen);
				}
				else
				{
					val = local_subtitle_GetBitData(pData, &nBitPos, 2);
					if (val == 0x02)
					{
						/* [OK] 0000 1110 LLLL CCCC L pixels (9-24) in colour C */
						runLen = local_subtitle_GetBitData(pData, &nBitPos, 4) + 9;
						val = local_subtitle_GetBitData(pData, &nBitPos, 4);
						val = (map_table == NULL) ? val : map_table[val];

						local_subtitle_put_pixel(&p, color_table[val], runLen);
					}
					else if (val == 0x03)
					{
						/* [OK] 0000 1111 LLLL LLLL CCCC L pixels (25-280) in colour C */
						runLen = local_subtitle_GetBitData(pData, &nBitPos, 8) + 25;
						val = local_subtitle_GetBitData(pData, &nBitPos, 4);
						val = (map_table == NULL) ? val : map_table[val];

						local_subtitle_put_pixel(&p, color_table[val], runLen);
					}
					else if (val == 0x00)
					{
						val = (map_table == NULL) ? 0 : map_table[0];

						/* [OK] 0000 1100 one pixel in colour 0 */
						local_subtitle_put_pixel(&p, color_table[val], 1);
					}
					else if (val == 0x01)
					{
						val = (map_table == NULL) ? 0 : map_table[0];

						/* [OK] 0000 1101 two pixels in colour 0 */
						local_subtitle_put_pixel(&p, color_table[val], 2);
					}
				}
			}
		}
	}
}

static	void	local_subtitle_decode8bpCode(HUINT32 *target_addr, HUINT8 *pData, HINT32 *cur_index, HUINT8 *map_table, ONDK_Color_t *color_table)
{
	HUINT32		nBitPos = 0, *p = NULL;
	HINT32		val = 0, runLen = 0;

	p = target_addr;
	nBitPos = 0;

	if (color_table == NULL)
	{
		HxLOG_Critical("Fail color_table is NULL.\n");
		return;
	}

	while (1)
	{
		val = local_subtitle_GetBitData(pData, &nBitPos, 8);
		if (val != 0)
		{
			val = (map_table == NULL) ? val : map_table[val];
			local_subtitle_put_pixel(&p, color_table[val], 1);
		}
		else
		{
			val = local_subtitle_GetBitData(pData, &nBitPos, 1);
			if (val == 0x00)
			{
				val = local_subtitle_GetBitData(pData, &nBitPos, 7);
				if (val != 0x00)
				{
					val = (map_table == NULL) ? 0 : map_table[0];

					local_subtitle_put_pixel(&p, color_table[val], val);
				}
				else
				{
					*cur_index = *cur_index + ((nBitPos >> 3) + 1) + 1;
					break;
				}
			}
			else
			{
				runLen = local_subtitle_GetBitData(pData, &nBitPos, 7);
				val = local_subtitle_GetBitData(pData, &nBitPos, 8);
				val = (map_table == NULL) ? val : map_table[val];

				local_subtitle_put_pixel(&p, color_table[val], runLen);
			}
		}
	}
}

static	void	local_subtitle_draw_string_to_region(ONDKWindowHandle _window, HINT32 obj_x, HINT32 obj_y, ObjectSegment_t *object_segment)
{
	HxLOG_Critical("[%s;%d] function is not implemented!\n", __FUNCTION__, __LINE__);
#if 0		//	function is not working perfectly, just comments it.
	HUINT8			*pData;
	HUINT8			*pchar;
	HUINT32 		 numberOfCodes , i;
	pData = object_segment->pObjectData;
	pchar = NULL;
	HxLOG_Assert(pData);

	HxLOG_Debug("DVB Subtitle u2ObjectCoding[%x]",object_segment->u2ObjectCoding);
	if(object_segment->u2ObjectCoding == 0x01){
		numberOfCodes = HxMACRO_Get8Bit(pData);
		for(i = 0 ; i < numberOfCodes ; i++){
			pchar =	HxMACRO_Get16Bit(pData + (i*2));
//			ONDK_FONT_DrawStringMediaSubtitle_ToWindow(_window,pchar,DEFAULT_FONT_SIZE,DEFAULT_TEXT_POSITON + (i * (DEFAULT_FONT_SIZE +2)));
		}
	}
#endif
}

static	void	local_subtitle_draw_object_to_region(RegionSegment_t *region_segment, HINT32 obj_x, HINT32 obj_y, ObjectSegment_t *object_segment)
{
	HUINT8			map2to4[] = { 0x0,  0x7,  0x8,  0xf};
	HUINT8			map2to8[] = {0x00, 0x77, 0x88, 0xff};
	HUINT8			map4to8[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

	HINT32			top_field_len, bottom_field_len;
	HINT32			i, buf_index = 0;
	HUINT8			*pData, *map_table;
	HUINT32			posX, posY;

	HUINT32			*pulRegionBaseAddr, *pulTargetAddr;

// prevent #228543
	if(object_segment == NULL)
	{
		HxLOG_Critical("Fail to object_segment is NULL!!!\n");
		return;
	}

	pData = object_segment->pObjectData;
	HxLOG_Assert(pData);

	if (region_segment == NULL )
	{
		HxLOG_Critical("Fail to region_segment  is NULL!!!\n");
		return;
	}

	if (object_segment->u2ObjectCoding != SBTL_CODING_METHOD_PIXEL)
		return;

	if (region_segment->surface == NULL)
	{
		HxLOG_Critical("Fail to Surface is NULL!!!\n");
		return;
	}

	pulRegionBaseAddr = ONDK_SURFACE_Lock(region_segment->surface);
	if (pulRegionBaseAddr == NULL)
	{
		HxLOG_Critical("Fail to Surface LOCK!!!\n");
		return;
	}
	region_segment->bSurfaceLock = TRUE;
	top_field_len = HxMACRO_Get16Bit(pData);
	bottom_field_len = HxMACRO_Get16Bit(pData + 2);
	pData += 4;


	posX = obj_x;
	posY = obj_y;

//	HxLOG_Critical("Object Size : [%d] top_field : [%d] bottom_field : [%d]\n", object_segment->ulObjectSize, top_field_len, bottom_field_len);
//	HLIB_LOG_Dump(pData, object_segment->ulObjectSize, 0, FALSE);

	/* Top field를 그린다. */
	while (buf_index < top_field_len)
	{
		pulTargetAddr = (pulRegionBaseAddr + posX + posY * region_segment->u16RegionWidth);

		switch (pData[buf_index++])
		{
			case	SBTL_2BIT_PIXEL_STRING:
				if (region_segment->u3RegionDepth == REGION_DEPTH_8BIT)
					map_table = map2to8;
				else if (region_segment->u3RegionDepth == REGION_DEPTH_4BIT)
					map_table = map2to4;
				else
					map_table = NULL;

				local_subtitle_decode2bpCode(pulTargetAddr, (HUINT8*)&pData[buf_index], &buf_index, map_table, region_segment->color_table);
				break;

			case	SBTL_4BIT_PIXEL_STRING:
				if (region_segment->u3RegionDepth == REGION_DEPTH_8BIT)
					map_table = map4to8;
				else
					map_table = NULL;

				local_subtitle_decode4bpCode(pulTargetAddr, (HUINT8*)&pData[buf_index], &buf_index, map_table, region_segment->color_table);
				break;

			case	SBTL_8BIT_PIXEL_STRING:
				local_subtitle_decode8bpCode(pulTargetAddr, (HUINT8*)&pData[buf_index], &buf_index, NULL, region_segment->color_table);
				break;

			case SBTL_MAP_2_TO_4_BIT :
				map2to4[0] = (pData[buf_index]) >> 4;
				map2to4[1] = (pData[buf_index++]) & 0xf;
				map2to4[2] = (pData[buf_index]) >> 4;
				map2to4[3] = (pData[buf_index++]) & 0xf;
				break;

			case SBTL_MAP_2_TO_8_BIT :
				for ( i = 0; i < 4; i++ )
				{
					map4to8[i] = (pData[buf_index++]);
				}
				break;

			case SBTL_MAP_4_TO_8_BIT :
				for ( i = 0; i < 16; i++ )
				{
					map4to8[i] = (pData[buf_index++]);
				}
				break;

			case SBTL_END_OF_OBJECT_LINE :
				/*
				 *  "end of object line code" shall be included after every series of code strings that
				 *  together represent one line of the object.
				 *  [ETSI EN 300 743 V1.3.1]
				 */
				 break;

			default :
				if (region_segment->surface == NULL)
				{
					HxLOG_Critical("Fail to Surface is NULL!!!\n");
					return;
				}
				if (region_segment->bSurfaceLock == TRUE)
				{
					ONDK_SURFACE_Unlock(region_segment->surface);
					region_segment->bSurfaceLock = FALSE;
				}
				HxLOG_Error("Check Data...\n");
				return;
		}

		posY++;

		if (bottom_field_len == 0)
		{
			HUINT32		*pulTopFieldBuffer, *pulBottomFieldBuffer;

			pulTopFieldBuffer = (pulRegionBaseAddr + (posY - 1) * region_segment->u16RegionWidth);
			pulBottomFieldBuffer = (pulRegionBaseAddr + (posY ) * region_segment->u16RegionWidth);

			HxSTD_MemCopy(pulBottomFieldBuffer, pulTopFieldBuffer, region_segment->u16RegionWidth * sizeof(ONDK_Color_t));

			posY++;
		} else
		{
			posY++;
		}
	}

	/* Top field보다 하나 아래 부터.. */
	posY = obj_y + 1;

	/* bottom field를 그린다. */
	while (buf_index < bottom_field_len + top_field_len)
	{
		pulTargetAddr = (pulRegionBaseAddr + posX + posY * region_segment->u16RegionWidth);

		switch ( pData[buf_index++] )
		{
			case SBTL_2BIT_PIXEL_STRING :
				if (region_segment->u3RegionDepth == REGION_DEPTH_8BIT)
				{
					map_table = map2to8;
				}
				else if (region_segment->u3RegionDepth == REGION_DEPTH_4BIT)
				{
					map_table = map2to4;
				}
				else
				{
					map_table = NULL;
				}

				local_subtitle_decode2bpCode(pulTargetAddr, (HUINT8*)&pData[buf_index], &buf_index, map_table, region_segment->color_table);
				break;

			case SBTL_4BIT_PIXEL_STRING :
				if (region_segment->u3RegionDepth == REGION_DEPTH_8BIT)
					map_table = map4to8;
				else
					map_table = NULL;

				local_subtitle_decode4bpCode(pulTargetAddr, (HUINT8*)&pData[buf_index], &buf_index, map_table, region_segment->color_table);
				break;

			case SBTL_8BIT_PIXEL_STRING :
				local_subtitle_decode8bpCode(pulTargetAddr, (HUINT8*)&pData[buf_index], &buf_index, NULL, region_segment->color_table);
				break;

			case SBTL_MAP_2_TO_4_BIT :
				map2to4[0] = (pData[buf_index]) >> 4;
				map2to4[1] = (pData[buf_index++]) & 0xf;
				map2to4[2] = (pData[buf_index]) >> 4;
				map2to4[3] = (pData[buf_index++]) & 0xf;
				break;

			case SBTL_MAP_2_TO_8_BIT :
				for ( i = 0; i < 4; i++ )
				{
					map4to8[i] = (pData[buf_index++]);
				}
				break;

			case SBTL_MAP_4_TO_8_BIT :
				for ( i = 0; i < 16; i++ )
				{
					map4to8[i] = (pData[buf_index++]);
				}
				break;

			case SBTL_END_OF_OBJECT_LINE :
				/*
				 *  "end of object line code" shall be included after every series of code strings that
				 *  together represent one line of the object.
				 *  [ETSI EN 300 743 V1.3.1]
				 */
				 break;

			default :
				HxLOG_Print("Check data!!\n");
				if (region_segment->surface == NULL)
				{
					HxLOG_Critical("Fail to Surface is NULL!!!\n");
					return;
				}
				if (region_segment->bSurfaceLock == TRUE)
				{
					ONDK_SURFACE_Unlock(region_segment->surface);
					region_segment->bSurfaceLock = FALSE;
				}
				return;
		}

		posY += 2;
	}

	if (region_segment->surface)
	{
		if (region_segment->bSurfaceLock == TRUE)
		{
			ONDK_SURFACE_Unlock(region_segment->surface);
			region_segment->bSurfaceLock = FALSE;
		}
	}
}

static	void	m_subtitle_process_dvb(ONDKWindowHandle _window,HUINT16 page_id, DVBData_t *pstDVBInfo)
{
	HUINT32			i, j;

	PageSegment_t	*page_segment;
	RegionSegment_t	*region_segment;
	CLUTSegment_t	*color_segment;

	page_segment = local_subtitle_find_page_segment(pstDVBInfo, page_id);
	if (page_segment == NULL)
	{
		HxLOG_Error("No Page Segment\n");
		return;
	}

	for (i = 0; i < page_segment->ulRegionNum; i++)
	{
		HUINT8		region_color_depth = 0;
		HUINT32		clut_no = 0;

		if (page_segment->pRgList == NULL)
		{
			HxLOG_Critical("page_segment->pRgList..\n");
			return;
		}
		region_segment = local_subtitle_find_region_segment(pstDVBInfo, page_id, page_segment->pRgList[i].u8RegionId);
		if (region_segment == NULL)
		{
			HxLOG_Error("Can't find region : id [%X]\n", page_segment->pRgList[i].u8RegionId);
			continue;
		}

		color_segment = local_subtitle_find_clut_segment(pstDVBInfo, page_id, region_segment->u8ClutId);
		if (color_segment == NULL)
		{
			HxLOG_Critical("No CLUT Segment...\n");
			continue;
		}

		if (region_segment->surface == NULL)
		{
			region_segment->surface = ONDK_SURFACE_Create(region_segment->u16RegionWidth, region_segment->u16RegionHeight, "ARGB8888");
			//ONDK_DRAW_FillRect(region_segment->surface, ONDK_Rect(0, 0, region_segment->u16RegionWidth, region_segment->u16RegionHeight), ONDK_Color(0, 0, 0, 0));
		}
		if (region_segment->surface == NULL)
		{
			HxLOG_Critical("Fail ONDK_SURFACE_Create().\n");
			continue;
		}

		region_color_depth = region_segment->u3RegionDepth;
		switch (region_color_depth)
		{
			case	REGION_DEPTH_2BIT:
				clut_no = 4;
				break;
			case	REGION_DEPTH_4BIT:
				clut_no = 16;
				break;
			case	REGION_DEPTH_8BIT:
				clut_no = 256;
				break;
			default:
				clut_no = 256;
				break;
		}
		if (region_segment->color_table)
		{
			AP_Free(region_segment->color_table);
			region_segment->color_table = NULL;
		}

		region_segment->color_table = (ONDK_Color_t*)AP_Malloc(sizeof(ONDK_Color_t) * clut_no);
		if (region_segment->color_table == NULL)		{

			HxLOG_Critical("Fail AP_Malloc().\n");
			region_segment->color_table = NULL;
			continue;
		}

		if (color_segment && color_segment->pCLUT)
		{
			HUINT8		y, cr, cb, t;
			HUINT32		cnt;

			for (cnt = 0; cnt < color_segment->ulClutNum; cnt++)
			{
				if ( !((region_color_depth == REGION_DEPTH_2BIT) && color_segment->pCLUT[cnt].u1Bit2Clut)
					&& !((region_color_depth == REGION_DEPTH_4BIT) && color_segment->pCLUT[cnt].u1Bit4Clut)
					&& !((region_color_depth == REGION_DEPTH_8BIT) && color_segment->pCLUT[cnt].u1Bit8Clut))
					continue;

				y  = color_segment->pCLUT[cnt].u8Yvalue;
				cr = color_segment->pCLUT[cnt].u8Crvalue;
				cb = color_segment->pCLUT[cnt].u8Cbvalue;
				t  = color_segment->pCLUT[cnt].u8Tvalue;

				region_segment->color_table[color_segment->pCLUT[cnt].u8EntryId] = ONDK_Color(_YCrCbT2RGB_A(y, t), _YCrCbT2RGB_R(y, cr), _YCrCbT2RGB_G(y, cr, cb), _YCrCbT2RGB_B(y, cb));
			}
		} else
		{
			switch (region_color_depth)
			{
				case	REGION_DEPTH_2BIT:
					HxSTD_MemCopy(region_segment->color_table, _default_clut_2, clut_no * sizeof(ONDK_Color_t));
					break;
				case	REGION_DEPTH_4BIT:
					HxSTD_MemCopy(region_segment->color_table, _default_clut_4, clut_no * sizeof(ONDK_Color_t));
					break;
				case	REGION_DEPTH_8BIT:
					HxSTD_MemCopy(region_segment->color_table, _default_clut_8, clut_no * sizeof(ONDK_Color_t));
					break;
				default:
					HxLOG_Error("Region Color Depth is not valid!!\n");
					return;
			}
		}

		if (region_segment->u1RegionFillFlag)
		{
			ONDK_Color_t		bg_color;

			switch (region_color_depth)
			{
				case	REGION_DEPTH_2BIT:
					bg_color = region_segment->color_table[region_segment->u2Bit2Code];
					break;
				case	REGION_DEPTH_4BIT:
					bg_color = region_segment->color_table[region_segment->u4Bit4Code];
					break;
				case	REGION_DEPTH_8BIT:
					bg_color = region_segment->color_table[region_segment->u8Bit8Code];
					break;
				default:
					bg_color = SBTL_DEC_DEFAULT_COLOR;
			}
			if (region_segment->surface == NULL)
			{
				HxLOG_Critical("Fail region_segment->surface .\n");
				continue;
			}

			if(region_segment->u16RegionWidth !=0 && region_segment->u16RegionHeight !=0)
				ONDK_DRAW_FillCopyRect(region_segment->surface, ONDK_Rect(0, 0, region_segment->u16RegionWidth, region_segment->u16RegionHeight), bg_color);

		}


		for (j = 0; j < region_segment->ulObjectNum; j++)
		{
			HUINT32		obj_x = 0, obj_y = 0;
			ObjectSegment_t	*object_segment = NULL;

			if (region_segment->pObjectList == NULL)
			{
				HxLOG_Critical("Fail region_segment->pObjectList is NULL.\n");
				continue;
			}

			obj_x = region_segment->pObjectList[j].u12ObjectHorPos;
			obj_y = region_segment->pObjectList[j].u12ObjectVerPos;

			switch (region_segment->pObjectList[j].u2ObjectType)
			{
				case	SBTL_OBJECT_TYPE_BITMAP:
					object_segment = local_subtitle_find_object_segment(pstDVBInfo, page_id, region_segment->pObjectList[j].u16ObjectId);
					if (object_segment)
						local_subtitle_draw_object_to_region(region_segment, obj_x, obj_y, object_segment);
					break;
				case	SBTL_OBJECT_TYPE_CHARACTER:
				case	SBTL_OBJECT_TYPE_STRING_OF_CHAR:
					object_segment = local_subtitle_find_object_segment(pstDVBInfo, page_id, region_segment->pObjectList[j].u16ObjectId);
					if (object_segment)
						local_subtitle_draw_string_to_region(_window, obj_x, obj_y, object_segment);
					break;
				default:
#if defined(CONFIG_OP_UK_DTT)
					HxLOG_Error("Object Type Error!! Check (%d) ObjectType = %d \n", j, region_segment->pObjectList[j].u2ObjectType);
					break; // UK 지상파 506Mhz에서 Reserved인 3으로 들어오는 경우가 있어  일단 break 처리함.
#else
					HxLOG_Assert(0);
#endif
			}
		}
	}

	HxLOG_Debug("page timeout : [%d]\n", page_segment->u8PageTimeOut);
}

