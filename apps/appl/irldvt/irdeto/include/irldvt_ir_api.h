
#include <htype.h>
#include <ir_loader.h>


typedef enum{
	IRNVR_PART_IRDETO = 1,
	IRNVR_PART_CA,
	IRNVR_PART_LOADER,

	IRNVR_WHOLE_AREA

}IRNVR_PART_Type;

static Boot_Block_Control_Block s_stBBCB;



IHL_S16 IRLDVT_I_HLSUP_GetStatusInfo (IHL_PSTATUS pstStatus);


HUINT16	IRLDVT_DRV_Get16Bits(HUINT8 *base);
HUINT32	IRLDVT_DRV_Get32Bits(HUINT8 *base);
HINT32 irldvt_Read_BBCB(Boot_Block_Control_Block *pBBCB);
HINT32 irldvt_Read_CPCB(HUINT8 * pCPCB);
HINT32 irldvt_ReadLoaderStatus(IHL_STATUS * pLoaderStatus );
HINT32 irldvt_cas_IrLoaderInit(void);
