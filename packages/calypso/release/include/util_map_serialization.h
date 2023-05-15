/* This is not generic header. Refer to devtest */


/* Example :
	*) from source that include this header, define map structure as below >

	Use Macro >
		#define MAP_STRUCT_NAME TestStructure
		#define MAP_STRUCT_MEMBERS \
					DECLARE_MAP_INT32(testInt) \
					DECLARE_MAP_UINT64(testUint64) \
					DECLARE_MAP_BOOL(testBool) \
					DECLARE_MAP_STRING(testString) \
					DECLARE_MAP_DATA(testData) \



	*) this will be expanded during preprocessing as below >

	Result >
		typedef struct {
			int32 		testInt;
			uint64_t 	testUint64;
			bool_t		testBool;
			int8_t		*testString;
			void		*testData;
			int32_t		testData_size;
		} TestStructure;

		static inline UTIL_MAP_t * do_generate_map_from_TestStructure (TestStructure *obj)
		{
			UTIL_MAP_t *pMap = NULL;

			UTIL_MAP_SetValueInt32(&pMap, "testInt", obj->testInt);
			UTIL_MAP_SetValueUint64(&pMap, "testUint64", obj->testUint64);
			UTIL_MAP_SetValueBool(&pMap, "testBool", obj->testBool);
			UTIL_MAP_SetValueString(&pMap, "testString", obj->testString);
			UTIL_MAP_SetValueData(&pMap, "testData", obj->testData_size);

			return pMap;
		}

		static inline void * do_serialize_from_TestStructure (TestStructure *obj)
		{
			UTIL_MAP_t *pMap = NULL;
			void *pData = NULL;
			int32_t nRetSize = 0;
			
			UTIL_MAP_SetValueInt32(&pMap, "testInt", obj->testInt);
			UTIL_MAP_SetValueUint64(&pMap, "testUint64", obj->testUint64);
			UTIL_MAP_SetValueBool(&pMap, "testBool", obj->testBool);
			UTIL_MAP_SetValueString(&pMap, "testString", obj->testString);
			UTIL_MAP_SetValueData(&pMap, "testData", obj->testData_size);

			if(pMap != NULL)
			{
				UTIL_MAP_Serialize(pMap, &pData, &nRetSize);
				if(pRetSize != NULL)
				{
					*pRetSize = nRetSize;
				}
				UTIL_MAP_Free(&pMap);
			}
			return pData;
		}
	
*/

#define DECLARE_MAP_BOOL(identifier) \
	bool_t identifier;

#define DECLARE_MAP_INT32(identifier) \
	int32_t identifier;

#define DECLARE_MAP_UINT32(identifier) \
	uint32_t identifier;

#define DECLARE_MAP_INT64(identifier) \
	int64_t identifier;

#define DECLARE_MAP_UINT64(identifier) \
	uint64_t identifier;

#define DECLARE_MAP_DOUBLE(identifier) \
	double identifier;

#define DECLARE_MAP_STRING(identifier) \
	int8_t * identifier;

#define DECLARE_MAP_DATA(identifier) \
	void * identifier; \
	int32_t identifier ## _ ## size;


typedef struct {
    MAP_STRUCT_MEMBERS
} MAP_STRUCT_NAME;

#define MAP_STRUCT_MAKENAME(functype, structname) \
    functype ## _ ## structname

#define MAP_STRUCT_DECLAREFUNC_MAPPING2(functype, structname) \
    static inline UTIL_MAP_t* MAP_STRUCT_MAKENAME(functype, structname) \
      (structname *obj)

#define MAP_STRUCT_DECLAREFUNC_SERIALIZATION2(functype, structname) \
    static inline void* MAP_STRUCT_MAKENAME(functype, structname) \
      (structname *obj, uint32_t *pRetSize)

#define MAP_STRUCT_DECLAREFUNC_MAPPING(functype) \
    MAP_STRUCT_DECLAREFUNC_MAPPING2(functype, MAP_STRUCT_NAME)

#define MAP_STRUCT_DECLAREFUNC_SERIALIZATION(functype) \
	MAP_STRUCT_DECLAREFUNC_SERIALIZATION2(functype, MAP_STRUCT_NAME)
	
#undef DECLARE_MAP_BOOL
#undef DECLARE_MAP_INT32
#undef DECLARE_MAP_UINT32
#undef DECLARE_MAP_INT64
#undef DECLARE_MAP_UINT64
#undef DECLARE_MAP_DOUBLE
#undef DECLARE_MAP_STRING
#undef DECLARE_MAP_DATA

#define DECLARE_MAP_BOOL(identifier) \
	UTIL_MAP_SetValueBool(&pMap, #identifier, obj->identifier);
#define DECLARE_MAP_INT32(identifier) \
	UTIL_MAP_SetValueInt32(&pMap, #identifier, obj->identifier);
#define DECLARE_MAP_UINT32(identifier) \
	UTIL_MAP_SetValueUint32(&pMap, #identifier, obj->identifier);
#define DECLARE_MAP_INT64(identifier) \
	UTIL_MAP_SetValueInt64(&pMap, #identifier, obj->identifier);
#define DECLARE_MAP_UINT64(identifier) \
	UTIL_MAP_SetValueUint64(&pMap, #identifier, obj->identifier);
#define DECLARE_MAP_DOUBLE(identifier) \
	UTIL_MAP_SetValueDouble(&pMap, #identifier, obj->identifier);
#define DECLARE_MAP_STRING(identifier) \
	UTIL_MAP_SetValueString(&pMap, #identifier, obj->identifier);
#define DECLARE_MAP_DATA(identifier) \
	UTIL_MAP_SetValueData(&pMap, #identifier, obj->identifier, obj->identifier ## _ ## size);
    
MAP_STRUCT_DECLAREFUNC_MAPPING(do_generate_map_from) {
	UTIL_MAP_t *pMap = NULL;
    MAP_STRUCT_MEMBERS
	return pMap;
}
   
MAP_STRUCT_DECLAREFUNC_SERIALIZATION(do_serialize_from) {
	UTIL_MAP_t *pMap = NULL;
	void *pData = NULL;
	int32_t nRetSize = 0;
	
	MAP_STRUCT_MEMBERS
	if(pMap != NULL)
	{
		UTIL_MAP_Serialize(pMap, &pData, &nRetSize);
		if(pRetSize != NULL)
		{
			*pRetSize = nRetSize;
		}
		UTIL_MAP_Free(&pMap);
	}
	return pData;
}

#undef DECLARE_MAP_BOOL
#undef DECLARE_MAP_INT32
#undef DECLARE_MAP_UINT32
#undef DECLARE_MAP_INT64
#undef DECLARE_MAP_UINT64
#undef DECLARE_MAP_DOUBLE
#undef DECLARE_MAP_STRING
#undef DECLARE_MAP_DATA
#undef MAP_STRUCT_DECLAREFUNC_MAPPING2
#undef MAP_STRUCT_DECLAREFUNC_SERIALIZATION2
#undef MAP_STRUCT_DECLAREFUNC_MAPPING
#undef MAP_STRUCT_DECLAREFUNC_SERIALIZATION
#undef MAP_STRUCT_MAKENAME
#undef MAP_STRUCT_MEMBERS
#undef MAP_STRUCT_NAME
