
#ifndef __NOCS3_CSD_HARNESS_TEST_CN_PRIVATE_H__
#define __NOCS3_CSD_HARNESS_TEST_CN_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define CSD_HELP_M_CN_INST		"-cn = This computes the checknumbers."
#define CSD_HELP_M_CN_OPTIONS	"options = This defines what type of checknumber needs to be computed. \
The various types could be NUID, STBCASN, CSDD and DINTEG."
#define CSD_HELP_M_CN_IFIC		"[-if InFile|-ic Value] = Optional parameter. This is the input data for the \
checknumber CSD APIs.  The data can come from file.  If not a file then a value in the range of 1 to 16 bytes \
must be used as an input.  If the data is not present then this command shall ignore the value while \
computing the checknumber"
#define CSD_HELP_M_CN_OC		"-oc = If this option is present, then the Checknumber is printed on the \
standard output."

#define CSD_HARNESS_TEST_PARSE_CN_TYPE \
	if ( bCnTypeParsered == false )\
	{\
		if ( cmdline_offset<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset], CSD_HARNESS_TEST_CN_TYPE_NUID_STR)==0 )\
			{\
				pParams->cnType = CSD_HARNESS_TEST_CN_TYPE_NUID;\
			}\
			else if ( strcmp (argv[cmdline_offset], CSD_HARNESS_TEST_CN_TYPE_STBCASN_STR)==0 )\
			{\
				pParams->cnType = CSD_HARNESS_TEST_CN_TYPE_STBCASN;\
			}\
			else if ( strcmp (argv[cmdline_offset], CSD_HARNESS_TEST_CN_TYPE_CSCD_STR)==0 )\
			{\
				pParams->cnType = CSD_HARNESS_TEST_CN_TYPE_CSCD;\
			}\
			else if ( strcmp (argv[cmdline_offset], CSD_HARNESS_TEST_CN_TYPE_DINTEG_STR)==0 )\
			{\
				pParams->cnType = CSD_HARNESS_TEST_CN_TYPE_DINTEG;\
			}\
			if ( pParams->cnType != CSD_HARNESS_TEST_CN_TYPE_MAX )\
			{\
				bCnTypeParsered = true;\
				cmdline_offset ++;\
				bCommandProcessed = true;\
			}\
		}\
		else \
		{\
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
		}\
	}\

#define CSD_HARNESS_TEST_CN_TYPE_NUID_STR		"NUID"
#define CSD_HARNESS_TEST_CN_TYPE_STBCASN_STR	"STBCASN"
#define CSD_HARNESS_TEST_CN_TYPE_CSCD_STR		"CSCD"
#define CSD_HARNESS_TEST_CN_TYPE_DINTEG_STR		"DINTEG"


typedef enum CSD_HARNESS_TEST_CN_TYPE
{
	CSD_HARNESS_TEST_CN_TYPE_NUID,
	CSD_HARNESS_TEST_CN_TYPE_STBCASN,
	CSD_HARNESS_TEST_CN_TYPE_CSCD,
	CSD_HARNESS_TEST_CN_TYPE_DINTEG,

	/* Add new enum before this line */
	CSD_HARNESS_TEST_CN_TYPE_MAX
}	CSD_HARNESS_TEST_CN_TYPE;


typedef struct CSD_Harness_Test_CN_Parameters
{
	CSD_HARNESS_TEST_CN_TYPE			cnType;
	bool								bOutPut;
	unsigned int						sizeData;
	unsigned char						data[16];

	FILE *								pTvFile;

}  CSD_Harness_Test_CN_Parameters;

#define CSD_HARNESS_TEST_CN_SIZE	4

typedef struct NOCS3_Harness_Test_CheckNumber_Test_Vector
{
	unsigned int						sizeNuid;
	T8BytesVector						nuid;

	unsigned int						sizeNuidCn;
	unsigned char						nuidCn[CSD_HARNESS_TEST_CN_SIZE];

	unsigned int						sizeStbSn;
	unsigned char						stbSn[4];

	unsigned int						sizeStbSnCn;
	unsigned char						stbSnCn[CSD_HARNESS_TEST_CN_SIZE];

	unsigned int						sizeCscData;
	unsigned char						cscData[16];

	unsigned int						sizeCscDataCn;
	unsigned char						cscDataCn[CSD_HARNESS_TEST_CN_SIZE];

	unsigned int						sizeDateIntegrity;
	unsigned char						dateIntegrity[4];

	unsigned int						sizeDateIntegrityCn;
	unsigned char						dateIntegrityCn[CSD_HARNESS_TEST_CN_SIZE];


}  NOCS3_Harness_Test_CheckNumber_Test_Vector;

NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_get_cn_test_vector_4_nuid (T8BytesVector nuid, \
															  NOCS3_Harness_Test_CheckNumber_Test_Vector * pVector,
															  FILE * pTvFile);
NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_cn_dump_test_vector (NOCS3_Harness_Test_CheckNumber_Test_Vector * pVector);


#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_CSD_HARNESS_TEST_CN_PRIVATE_H__ */