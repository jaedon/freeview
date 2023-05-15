#ifndef DSMCC_DESCRIPTOR_H
#define DSMCC_DESCRIPTOR_H

#include <stdio.h>

typedef enum dsmccDescriptorTag
{
	// 여기에는 ETSI EN 301 192 V1.4.1에서 정의해 놓은 Descriptor만 등록 함.
	
	TAG_TYPE			= 0x01,
	TAG_NAME			= 0x02,
	TAG_INFO			= 0x03,
	TAG_MODULE_LINK	= 0x04,
	TAG_CRC32					= 0x05,
	TAG_LOCATION				= 0x06,
	TAG_EST_DOWNLOAD_TIME	= 0x07,
	TAG_GROUP_LINK				= 0x08,
	TAG_COMPRESSED_MODULE	= 0x09,
} DsmccDescriptorTag;

/** @brief TAG_TYPE (0x01) */
typedef struct tagType_DES{
	char *text;
}TypeDes_t;

/** @brief TAG_NAME (0x02) */
typedef struct tagName_DES {
	char *text;
}NameDes_t;

/** @brief TAG_INFO (0x03) */
typedef struct tagInfo_DES{
	char lang_code[3];
	char *text;
}InfoDes_t;

/** @brief TAG_MODULE_LINK (0x04) */
typedef struct tagModuleLink_DES{
	char position;
	unsigned short module_id;
}ModuleLinkDes_t;

/** @brief TAG_CRC32 (0x05) */
typedef struct tagCrc32_DES{
	unsigned long crc;
}Crc32Des_t;

/** @brief TAG_LOCATION (0x06) */
typedef struct tagLocation_DES{
	char location_tag;
}LocationDes_t;

/** @brief TAG_EST_DOWNLOAD_TIME (0x07) */
typedef struct tagEstDownloadTime_DES{
	unsigned long download_time;
}EstDownloadTimeDes_t;

/** @brief TAG_GROUP_LINK (0x08) */
typedef struct tagGroupLink_DES {
	char position;
	unsigned long group_id;
}GroupLinkDes_t;

/** @brief TAG_COMPRESSED_MODULE (0x09) */
typedef struct tagCompressedModule_DES { 
	char method;
	unsigned long original_size;
}CompressedModuleDes_t;

typedef struct dsmcc_descriptor {
	unsigned char tag;
	unsigned char len;
	union {
		TypeDes_t 		type;
		NameDes_t 		name;
		InfoDes_t 		info;
		ModuleLinkDes_t	modlink;
		Crc32Des_t		crc32;
		LocationDes_t		location;
		EstDownloadTimeDes_t	dltime;
		GroupLinkDes_t			grouplink;
		CompressedModuleDes_t	compressed;
		
		//PrivateDes_t				private;
		/*struct descriptor_subgroup subgroup  #ref. DVB SSU */
	} data;

	struct dsmcc_descriptor *next;
}DsmccDes_t;

DsmccDes_t *dsmcc_desc_process(unsigned char *Data, int data_len, int *offset);
 void dsmcc_desc_free(DsmccDes_t *desc);
#endif
