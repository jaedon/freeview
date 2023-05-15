#include <stdlib.h>
#include <string.h>
#include "dsmcc-descriptor.h"
#include "libdsmcc.h"

/*******************************************************************/
/********************	   Static function Prototype	 ********************/
/*******************************************************************/
static void dsmcc_desc_process_type(unsigned char *Data, DsmccDes_t *desc);
static void dsmcc_desc_process_name(unsigned char *Data, DsmccDes_t *desc);
static void dsmcc_desc_process_info(unsigned char *Data,  DsmccDes_t *desc);
static void dsmcc_desc_process_modlink(unsigned char *Data, DsmccDes_t *desc);
static void dsmcc_desc_process_crc32(unsigned char *Data, DsmccDes_t *desc);
static void dsmcc_desc_process_location(unsigned char *Data, DsmccDes_t *desc);
static void dsmcc_desc_process_dltime(unsigned char *Data, DsmccDes_t *desc);
static void dsmcc_desc_process_grouplink(unsigned char *Data, DsmccDes_t *desc);
static void dsmcc_desc_process_compressed(unsigned char *Data, DsmccDes_t *desc);
static DsmccDes_t *dsmcc_desc_process_one(unsigned char *Data, int *offset);

/*******************************************************************/
/********************      Static Functions   ********************/
/*******************************************************************/

#define _____STATIC_FUCTIONS_____

static void dsmcc_desc_process_type(unsigned char *Data, DsmccDes_t *desc)
{
	TypeDes_t	*pTypeDes = &desc->data.type;

	pTypeDes->text = (char *)malloc(desc->len);

	memcpy(pTypeDes->text, Data, desc->len);
}

static void dsmcc_desc_process_name(unsigned char *Data, DsmccDes_t *desc)
{
	NameDes_t	*pNameDes = &desc->data.name;

	pNameDes->text = (char *)malloc(desc->len);

	memcpy(pNameDes->text, Data, desc->len);
}

static void dsmcc_desc_process_info(unsigned char *Data,  DsmccDes_t *desc)
{
	InfoDes_t *pInfoDes = &desc->data.info;

	memcpy(pInfoDes->lang_code, Data, 3);

	pInfoDes->text = (char *)malloc(desc->len - 3);

	memcpy(pInfoDes->text, Data+3, desc->len - 3);
}

static void dsmcc_desc_process_modlink(unsigned char *Data, DsmccDes_t *desc)
{
	ModuleLinkDes_t *pModuleLinkDes = &desc->data.modlink;

	pModuleLinkDes->position = Data[0];

	pModuleLinkDes->module_id = (Data[1] << 8) | Data[2];
}

static void dsmcc_desc_process_crc32(unsigned char *Data, DsmccDes_t *desc)
{
	Crc32Des_t *pCrc32Des = &desc->data.crc32;

	pCrc32Des->crc = (Data[0] << 24) | (Data[1] << 16) | (Data[2] << 8) | Data[3];
}

static void dsmcc_desc_process_location(unsigned char *Data, DsmccDes_t *desc)
{
	LocationDes_t *pLocationDes = &desc->data.location;

	pLocationDes->location_tag = Data[0];
}

static void dsmcc_desc_process_dltime(unsigned char *Data, DsmccDes_t *desc)
{
	EstDownloadTimeDes_t	*pEstimatedDownloadTimeDes = &desc->data.dltime;

	pEstimatedDownloadTimeDes->download_time = (Data[0] << 24) | (Data[1] << 16) | (Data[2] << 8 ) | Data[3];
}

static void dsmcc_desc_process_grouplink(unsigned char *Data, DsmccDes_t *desc)
{
	GroupLinkDes_t	*pGroupLinkDes = &desc->data.grouplink;

	pGroupLinkDes->position = Data[0];
	pGroupLinkDes->group_id = (Data[1] << 24) | (Data[2] << 16) | (Data[3] << 8 ) | Data[4];
}

static void dsmcc_desc_process_compressed(unsigned char *Data, DsmccDes_t *desc)
{
	CompressedModuleDes_t	*pCompressedModuleDes = &desc->data.compressed;

	pCompressedModuleDes->method = Data[0];
	pCompressedModuleDes->original_size = (Data[1] << 24) | (Data[2] << 16) | (Data[3] << 8)  | Data[4];
}

static DsmccDes_t *dsmcc_desc_process_one(unsigned char *Data, int *offset)
{
	DsmccDes_t *desc;
	int off=0;

	desc = malloc(sizeof(DsmccDes_t));
	desc->tag = Data[0];
	desc->len = Data[1];

	off += 2;

	switch(desc->tag) 
	{
		case TAG_TYPE:
			dsmcc_desc_process_type(Data+2, desc);
			break;
		case TAG_NAME:
			dsmcc_desc_process_name(Data+2, desc);
			break;
		case TAG_INFO:
			dsmcc_desc_process_info(Data+2, desc);
			break;
		case TAG_MODULE_LINK:
			dsmcc_desc_process_modlink(Data+2, desc);
			break;
		case TAG_CRC32:
			dsmcc_desc_process_crc32(Data+2, desc);
			break;
		case TAG_LOCATION:
			dsmcc_desc_process_location(Data+2, desc);
			break;
		case TAG_EST_DOWNLOAD_TIME:
			dsmcc_desc_process_dltime(Data+2, desc);
			break;
		case TAG_GROUP_LINK:
			dsmcc_desc_process_grouplink(Data+2, desc);
			break;
		case TAG_COMPRESSED_MODULE:
			dsmcc_desc_process_compressed(Data+2, desc);
			break;
		default:
			break;
	}

	off+= desc->len;

	*offset += off;

	return desc;

}

#define ______GLOBAL_FUNCTIONS_____

DsmccDes_t *dsmcc_desc_process(unsigned char *Data, int data_len, int *offset)
{
	int index = 0;
	DsmccDes_t	*desc, *list, *l;

	desc = list = NULL;

	while(data_len > index) 
	{
		desc = dsmcc_desc_process_one(Data+index, &index);
		if(list == NULL) 
		{
			list = desc;
			list->next = NULL;
		} 
		else
		{
			for(l=list;l->next!=NULL;l=l->next) { ; }
			l->next = desc;
			desc->next = NULL;
		}
	}

	*offset += index;

	return list;
}

void dsmcc_desc_free(DsmccDes_t *desc)
{
	free(desc);
}

