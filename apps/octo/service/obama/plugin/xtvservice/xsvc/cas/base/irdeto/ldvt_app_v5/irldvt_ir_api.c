
#include <octo_common.h>
//#include "ir_loader_v5.h"
#include <irldvt_ir_api.h>
#include <tir_nvr.h>
#include <tir_sup.h>
#include <sysdefs.h>
#include <flash.h>

#include <tir_platform.h>
#include <tir_nvr.h>

#include <_hlib_log.h>

#include "irv5_dev.h"
#include "util.h"

#define _____CA_NVRAM_DEFINE_______________________________________________________________


Boot_Block_Control_Block s_stBBCB;




HINT32 irldvt_Read_BBCB(Boot_Block_Control_Block *pBBCB)
{
	HUINT32	result = ERR_OK;
//	HUINT8 aucBBCB[IR_BBCB_SIZE];
//	printf("\n[%s],[%d] irldvt_Read_BBCB IR_BBCB_SIZE = %d ++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__, IR_BBCB_SIZE);
//	HxLOG_Print("%s\n", __FUNCTION__ );
//	lc_bbcb_st	bbcb;
	lc_bool 	flag_crc_correct;
	dbgprt0("+++\n");

	BBCB_GetBBCB(pBBCB, &flag_crc_correct);

#if 0
	/* When you tried to get real value from nvram, you must not use a structure copy method due to fragmentation, jichoi. */
	pBBCB->usCRC16 			= IRLDVT_DRV_Get16Bits(aucBBCB + BBCB_CRC16_OFFSET);
	pBBCB->ucManufacturerCode	= aucBBCB[BBCB_MANCODE_OFFSET] ;
	pBBCB->ucHardwareCode		= aucBBCB[BBCB_HWCODE_OFFSET] ;
	pBBCB->ucLoaderMajor		= aucBBCB[BBCB_LOADERMAJOR_OFFSET] ;
	pBBCB->ucLoaderMinor		= aucBBCB[BBCB_LOADERMINOR_OFFSET] ;
	pBBCB->ucLoaderType		= aucBBCB[BBCB_LOADERTYPE_OFFSET] ;
	pBBCB->ulSerialNumber		= IRLDVT_DRV_Get32Bits(aucBBCB + BBCB_SERIALNUM_OFFSET);
	pBBCB->ulRandomNumber 	= IRLDVT_DRV_Get32Bits(aucBBCB + BBCB_RANDOMNUM_OFFSET);
#endif

#if 1
	dbgprt0("\n***** v5 BBCB *****\n");
	dbgprt3("OUI[3] : [%02x][%02x][%02x]\n", pBBCB->OUI[0], pBBCB->OUI[1], pBBCB->OUI[2] );
	dbgprt1("Manufacture Id : [%04x]\n", pBBCB->manufacturer_id );
	dbgprt1("HW_Version : [%04x]\n", pBBCB->hardware_version);
	dbgprt1("LDR_Version_Major : %d\n", pBBCB->loader_major_version);
	dbgprt1("LDR_Version_Minor : %d\n", pBBCB->loader_minor_version);
	dbgprt1("LDR_version_build : %d\n", pBBCB->loader_version_buildnumber );
	dbgprt1("Boot Check Alg. : %d\n", pBBCB->boot_check_algorithm);
	dbgprt4("SerialNumber : [%02x %02x %02x %02x]\n", pBBCB->serial_number[0], pBBCB->serial_number[1], pBBCB->serial_number[2], pBBCB->serial_number[3] );
	dbgprt4("RandomNumber : [%02x %02x %02x %02x]\n", pBBCB->random_number[0], pBBCB->random_number[1], pBBCB->random_number[2], pBBCB->random_number[3] );
	dbgprt1("CRC32 : [%08x]\n", pBBCB->crc32 );
	dbgprt0("*********************\n");
#endif

	return ERR_OK;

}

HINT32 irldvt_cas_IrLoaderInit(void)
{
	HINT32	result = ERR_OK;

	HxLOG_Info("\n");

	if(  ERR_OK != irldvt_Read_BBCB(&s_stBBCB) )
	{
		HxLOG_Print("in local_cas_ir_Read_BBCB !\n");
		result |= ERR_FAIL;
	}

	return result;
}


