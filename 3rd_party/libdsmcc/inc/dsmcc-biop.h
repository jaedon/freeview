#ifndef DSMCC_BIOP_H
#define DSMCC_BIOP_H

#include "dsmcc-descriptor.h"

typedef struct biop_tap {
	unsigned short id;
	unsigned short use;
	unsigned short assoc_tag;
	unsigned short selector_len;
	char *selector_data;
}BIOP_Tap_t;

typedef struct biop_module_info {
	unsigned long mod_timeout;
	unsigned long block_timeout;
	unsigned long min_blocktime;
	unsigned char taps_count;
	BIOP_Tap_t 	stBiopTap;
	unsigned char user_info_len;
	DsmccDes_t *	descriptors;
}BIOP_ModuleInfo_t;

typedef struct biop_dsm_connbinder {
	unsigned long component_tag;
	unsigned char component_data_len;
	unsigned char taps_count;
	BIOP_Tap_t 	stBiopTap;
}BIOP_DsmConnbinder_t;

typedef struct biop_obj_location {
	unsigned long component_tag;
	char component_data_len;
	unsigned long carousel_id;
	unsigned short module_id;
	char version_major;
	char version_minor;
	unsigned char objkey_len;
	char *objkey;
}BIOP_ObjLocation_t;

typedef struct biop_profile_body {
	unsigned long data_len;
	char byte_order;
	char lite_components_count;
	BIOP_ObjLocation_t stBiopObjLocation;
	BIOP_DsmConnbinder_t stBiopDsmConnbiner;
	/* ignore the rest  */
}BIOP_ProfileBody_t;

typedef struct biop_ior {
	unsigned long type_id_length;
	char *type_id;
	unsigned long tagged_profiles_count;
	unsigned long profile_id_tag;
	union {
		BIOP_ProfileBody_t stBiopProfileBody;
	//	struct biop_profile_lite lite;
	} profile;
	/* UKProfile - ignore other profiles */
}BIOP_Ior_t;

typedef struct biop_name_component {
	unsigned char id_len;
	char *id;
	unsigned char kind_len;
	char *kind;
}BIOP_NameComponent_t;

typedef struct biop_name {
	unsigned char component_count;
	BIOP_NameComponent_t *comps;
}BIOP_Name_t;


typedef struct biop_binding {
	BIOP_Name_t stBiopName;
	char binding_type;
	BIOP_Ior_t 	stBiopIor;
	unsigned int objinfo_len;
	char *objinfo;
}BIOP_Binding_t;

typedef struct biop_msg_header {
	unsigned char version_major;
	unsigned char version_minor;
	unsigned int message_size;
	unsigned char objkey_len;
	char *objkey;
	unsigned long objkind_len;
	char *objkind;
	unsigned int objinfo_len;
	char *objinfo;
}BIOP_Message_Header_t;

typedef struct biop_body_file {
	unsigned long msgbody_len;
	unsigned long content_len;
}BIOP_BodyFile_t;

typedef struct biop_body_directory {
	unsigned long msgbody_len;
	unsigned int bindings_count;
	BIOP_Binding_t stBiopBinding;
}BIOP_BodyDriectory_t;

typedef struct biop_body_servergateway {
	unsigned long msgbody_len;
	unsigned int bindings_count;
	BIOP_Binding_t stBiopBinding;
}BIOP_BodyServerGateWay_t;

typedef struct biop_body_stream {
	unsigned long msgbody_len;
	unsigned int taps_count;
	BIOP_Binding_t stBiopBinding;
}BIOP_BodyStream_t;

typedef struct biop_body_streamevent {
	unsigned long msgbody_len;
	unsigned int taps_count;
	BIOP_Binding_t stBiopBinding;
}BIOP_BodyStreamEvent_t;

typedef struct biop_message {
	BIOP_Message_Header_t  stBiopMsgHeader;

	union
	{
		BIOP_BodyFile_t 			stBiopBodyFile;
		BIOP_BodyDriectory_t 		stBiopBodyDirectory;
		BIOP_BodyServerGateWay_t 	stBiopBodyServerGateWay;
		BIOP_BodyStream_t 		stBiopBodyStream;
		BIOP_BodyStreamEvent_t 	stBiopBodyStreamEvent;
	} body;
}BIOP_Message_t;

int dsmcc_biop_process_module_info(BIOP_ModuleInfo_t *pstBiopModuleInfo, unsigned char *Data);
int dsmcc_biop_process_Ior(BIOP_Ior_t *pstIor, unsigned char *pucRawData);
#endif
