/**************************************************************
 *	@file		mgr_decode_common.h
 *	This module for converting message from OAPI to OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2011/09/25
 *	@author			humax
 *	@brief			Converting System Control table for OCTO 2.0 and OAPI
 **************************************************************/

/**************************************************************
	thie function just for codec folder's internal usage.
		\octo\main\tvmanager\ap_core\action\app_manager_new\codec\
 **************************************************************/

typedef	enum
{
	eTAG_Pair,
	eTAG_Integer,
	eTAG_Float,
	eTAG_String,
	eTAG_Bool,
	eTAG_Binary
} TAGTYPE_e;

typedef	struct
{
	HCHAR		*tag;
	HUINT32		value;
} _TAG2ENUM_t;

typedef	struct
{
	TAGTYPE_e	tag_type;
	_TAG2ENUM_t	taglist[];
} TAG2ENUM_t;

typedef	struct	_PROP2ENUM
{
	HCHAR			*property;
	TAG2ENUM_t		*enumtable;
	HUINT32			offset;

	HINT32				sizeofstructure;
	struct	_PROP2ENUM	*propTable;

	HINT32				ObjectCount;
} PROP2ENUM_t;

typedef	struct	_PROP2DATA
{
	HCHAR			*property;
	TAG2ENUM_t		*enumtable;
	HINT32			offset;
	HINT32			size;
} PROP2DATA_t;

extern	TAG2ENUM_t		GTAG_ONOFF_TABLE;
extern	TAG2ENUM_t		GTAG_BOOL_TABLE;
extern	TAG2ENUM_t		GTAG_INT_TABLE;
extern	TAG2ENUM_t		GTAG_STRING_TABLE;
extern	TAG2ENUM_t		GTAG_FLOAT_TABLE;
extern	TAG2ENUM_t		GTAG_BINARY_TABLE;

//extern TAG2ENUM_t		GTAG_VIDEO_Codec_Table;
//extern TAG2ENUM_t		GTAG_AUDIO_Codec_Table;



extern	void			mgr_appmgr_dec_value_to_enum_adv(TAG2ENUM_t *table, HxJSON_t object, HUINT32 write_addr, HUINT32 sizeofstructure);
extern	HUINT32			mgr_appmgr_dec_value_to_enum(_TAG2ENUM_t *table, const HCHAR *value);
extern	HCHAR*			mgr_appmgr_dec_enum_to_value(_TAG2ENUM_t *table, HUINT32 data);
extern	HCHAR*			mgr_appmgr_dec_enum_to_value_safe(_TAG2ENUM_t *table, HUINT32 data);
extern	HUINT32			mgr_appmgr_dec_default_enum(_TAG2ENUM_t *table);
extern	void			mgr_appmgr_decode_enum (TAG2ENUM_t *table, HxJSON_t object, HUINT32 write_addr, HUINT32 size);
extern	HERROR			mgr_appmgr_decode_object (void *pInfo, HxJSON_t root, PROP2DATA_t *table);
extern	HCHAR*			mgr_appmgr_encode_object(void *pInfo, HCHAR *object_name, PROP2DATA_t *table, ...);
extern	HCHAR*			mgr_appmgr_encode_object_array(HCHAR *object_name, HINT32 max_cnt, HINT32 size, HINT32 offset_of_flag, void *pInfo, PROP2DATA_t *table);
extern	HERROR			mgr_appmgr_dec_parse_property(void *pInfo, HxJSON_t root, PROP2ENUM_t *table);
extern	HERROR			mgr_appmgr_dec_encode_property(HINT32 stream, void *pInfo, PROP2ENUM_t *table);
extern	void			mgr_appmgr_printout_data(HCHAR *data);

extern	HCHAR			*hom_ipc_decodebase64(const HCHAR *string);
extern	HCHAR			*hom_ipc_encodebase64(const HCHAR *string);
