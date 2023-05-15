
#include <hlib.h>

#include "int_otl_conv.h"

HINT32 OTL_CONV_ConvertObamaEnum2DxEnum (OTL_EnumObama2Dx_t *pstEnumTable, HUINT32 ulTableSize, HINT32 nObamaEnum)
{
	HUINT32					 ulIndex;
	HUINT32					 ulTableLen = ulTableSize / sizeof(OTL_EnumObama2Dx_t);

	for (ulIndex = 0; ulIndex < ulTableLen; ulIndex++)
	{
		if (pstEnumTable[ulIndex].nObamaEnum == nObamaEnum)
		{
			return pstEnumTable[ulIndex].nDxEnum;
		}
	}

	return 0;
}

HINT32 OTL_CONV_ConvertDxEnum2ObamaEnum (OTL_EnumObama2Dx_t *pstEnumTable, HUINT32 ulTableSize, HINT32 nDxEnum)
{
	HUINT32					 ulIndex;
	HUINT32					 ulTableLen = ulTableSize / sizeof(OTL_EnumObama2Dx_t);

	for (ulIndex = 0; ulIndex < ulTableLen; ulIndex++)
	{
		if (pstEnumTable[ulIndex].nDxEnum == nDxEnum)
		{
			return pstEnumTable[ulIndex].nObamaEnum;
		}
	}

	return 0;
}

