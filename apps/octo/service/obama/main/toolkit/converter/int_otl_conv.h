
#ifndef	___INT_CONV_H___
#define	___INT_CONV_H___

#include <htype.h>

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct
{
	HINT32			 nObamaEnum;
	HINT32			 nDxEnum;
} OTL_EnumObama2Dx_t;

extern HINT32 OTL_CONV_ConvertObamaEnum2DxEnum (OTL_EnumObama2Dx_t *pstEnumTable, HUINT32 ulTableSize, HINT32 nObamaEnum);
extern HINT32 OTL_CONV_ConvertDxEnum2ObamaEnum (OTL_EnumObama2Dx_t *pstEnumTable, HUINT32 ulTableSize, HINT32 nDxEnum);

#ifdef  __cplusplus
}
#endif

#endif /* ___INT_CONV_H___ */

