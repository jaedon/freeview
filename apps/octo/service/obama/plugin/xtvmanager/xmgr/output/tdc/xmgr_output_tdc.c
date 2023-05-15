#include <octo_common.h>

#include <db_param.h>

#include <svc_output.h>


HERROR xmgr_output_GetSupportTvScartFormat_Tdc (DxScartFormat_b *pulScartFormatCapability)
{
	
    if (NULL == pulScartFormatCapability)
            return ERR_FAIL;
	
    *pulScartFormatCapability = (eDxSCART_FORMAT_CVBS | eDxSCART_FORMAT_RGB);
     return ERR_OK;
}


