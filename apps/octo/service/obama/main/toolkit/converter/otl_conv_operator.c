
#include	<otl.h>
#include	<octo_common.h>
#include	<db_param.h>

typedef struct
{
	DxOPTYPE_e 		eSvcOpType;
	DxOperator_e	eSysOpType;
} OTL_CONV_OperatorTable_t;

const STATIC OTL_CONV_OperatorTable_t s_stOperatorTable[] =
{
	{eDxOPTYPE_APSHD,		eDxOPERATOR_Ses},
	{eDxOPTYPE_FREESAT,		eDxOPERATOR_Freesat},
	{eDxOPTYPE_CANALSAT,		eDxOPERATOR_CanalReady},
	{eDxOPTYPE_NTVPLUS,		eDxOPERATOR_NtvPlus},
	{eDxOPTYPE_UPC,			eDxOPERATOR_UPC},
	{eDxOPTYPE_ADTV,			eDxOPERATOR_ADTV},
	{eDxOPTYPE_ALJAZEERA,	eDxOPERATOR_Aljazeera},
	{eDxOPTYPE_CNE,			eDxOPERATOR_CNE},
	{eDxOPTYPE_DIGITURK,		eDxOPERATOR_Digiturk},
	{eDxOPTYPE_TIVUSAT,		eDxOPERATOR_TIVU},
	{eDxOPTYPE_MOSTELECOM,	eDxOPERATOR_Mostelecom}
};

#define	NUM_OF_OPERATOR_TABLE		(sizeof(s_stOperatorTable) / sizeof(OTL_CONV_OperatorTable_t))

DxOperator_e	OTL_CONV_SvcOperator2SysOperator(DxOPTYPE_e eSvcOpType)
{
	HINT32	i;

	for (i = 0; i < NUM_OF_OPERATOR_TABLE; i++)
	{
		if (s_stOperatorTable[i].eSvcOpType == eSvcOpType)
			return s_stOperatorTable[i].eSysOpType;
	}

	return eDxOPERATOR_NOT_DEFINED;
}

DxOPTYPE_e		OTL_CONV_SysOperator2SvcOperator(DxOperator_e eSysOperator)
{
	HINT32	i;

	for (i =0; i < NUM_OF_OPERATOR_TABLE; i++)
	{
		if (s_stOperatorTable[i].eSysOpType == eSysOperator)
			return s_stOperatorTable[i].eSvcOpType;
	}

	return eDxOPTYPE_NONE;
}

const HCHAR*	OTL_CONV_Operator2String(DxOperator_e eOpType)
{
	switch (eOpType)
	{
		case eDxOPERATOR_Ziggo: return "Ziggo";
		case eDxOPERATOR_Comhem:return "Comhem";
		case eDxOPERATOR_KDG:return "KDG";
		case eDxOPERATOR_SkyD:return "SkyD";
		case eDxOPERATOR_Ses:return "Ses";
		case eDxOPERATOR_FiCom :return "FiCom";
		case eDxOPERATOR_Freesat:return "Freesat";
		case eDxOPERATOR_CanalReady:return "CanalReady";
		case eDxOPERATOR_Rcs:return "Rcs";
		case eDxOPERATOR_NtvPlus:return "NtvPlus";
		case eDxOPERATOR_BBC:return "BBC";
		case eDxOPERATOR_UPC:return "UPC";
		case eDxOPERATOR_Digiturk:return "Digiturk";
		case eDxOPERATOR_ADTV:return "ADTV";
		case eDxOPERATOR_Aljazeera:return "Aljazeera";
		case eDxOPERATOR_CNE:return "CNE";
		case eDxOPERATOR_SpainTdt:return "SpainTdt";
		case eDxOPERATOR_Truevisions:return "Truevisions";
		case eDxOPERATOR_Boxer:return "Boxer";
		case eDxOPERATOR_TIVU:return "TIVU";
		case eDxOPERATOR_JCOM:return "JCOM";
		case eDxOPERATOR_SKAPA:return "SKAPA";
		case eDxOPERATOR_Mostelecom:return "Mostelecom";
		case eDxOPERATOR_SwissCanal:return "SwissCanal";
		case eDxOPERATOR_RussiaTvt :return "RussiaTvt";
		case eDxOPERATOR_Almajd:return "Almajd";
		case eDxOPERATOR_NOVA:return "NOVA";
		case eDxOPERATOR_SkyCable:return "SkyCable";
		case eDxOPERATOR_GermanCab :return "GermanCab";
		case eDxOPERATOR_Primacom:return "Primacom";
		case eDxOPERATOR_VTC:return "VTC";
		case eDxOPERATOR_ERtelecom :return "ERtelecom";
		case eDxOPERATOR_Tdc :return "Tdc";
		case eDxOPERATOR_Telenor :return "Telenor";
		case eDxOPERATOR_AMX:return "AMX";
		default:
			break;
	}

	return NULL;
}

