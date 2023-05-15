#include  <string.h>

#include "htype.h"

#include "vkernel.h"
#include "di_err.h"

#include "di_nvram.h"
#include "drv_nvram.h"
#include "di_uart.h"

static HUINT32 NVRAM_SWAP32(HUINT32 A)
{
	return ((A << 24) | ((A << 8) &  0xFF0000) | ((A >> 8) &  0xFF00) |(A >> 24));
}

DRV_Error DRV_NVRAM_ConvertSystemId(HUINT32 *data)
{
	*((HUINT32*)data) = NVRAM_SWAP32((HUINT32)*(HUINT32*)data);

	return DI_ERR_OK;
}


