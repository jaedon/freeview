/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#include "octo_common.h"
#include "vd_humax.h"
#include "ir_platform.h"
#include "ir_fta_block.h"
#include "ir_pvr_dlna_tvportal_block.h"
#include "ir_svc.h"
#include "ir_evt.h"
#include <svc_si.h>

#include <db_param.h>

/*
 *  Smartcard Product를 보고 PVR, TV Portal, 등 기능을 제한 하는 모듈 ...
 *  FTA Block을 기반으로 디자인 됨.
*/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

#if defined(CONFIG_DEBUG)
HUINT32 g_IrPRODUCTBlock_Level = (DBG_ASSERT);
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define ________Debugging_Macro________
#if defined(CONFIG_DEBUG)
#define IrPRODUCTBlockPrint(level, message)	(((level) & g_IrPRODUCTBlock_Level) ? (VK_Print message) : 0)
#else
#define IrPRODUCTBlockPrint(level, message)
#endif

#define IR_UPC_PRODUCT_BLOCK_ID_PVR			0x03F1 //0x21	// PT2 Test를 위해 임시 변경.
#define IR_UPC_PRODUCT_BLOCK_ID_PORTAL			0x03F2 //0x22	// PT2 Test를 위해 임시 변경.
#define IR_UPC_PRODUCT_BLOCK_ID_DLNA			0x23

/*******************************************************************/
/********************      Extern Function         *************************/
/*******************************************************************/
extern HVD_Result  HUMAX_VD_SC_GetCardPhysicalStatus(HUINT8 ucSlotID, HBOOL *pSCInserted);


/*******************************************************************/
/********************      Static Function         *************************/
/*******************************************************************/
static HBOOL is_smartcard_in(void)
{
	HBOOL 				bSCInserted;
	HVD_Result			hVdResult;
	HINT32				i;

	for (i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++)	// multi card 도 신경 써주는 척 하자
	{
		hVdResult = HUMAX_VD_SC_GetCardPhysicalStatus (i, &bSCInserted);
		IrPRODUCTBlockPrint(DBG_TRACE, ("bSCInserted: %d\n", bSCInserted));
		if (hVdResult == eHVD_ERR)
		{
			bSCInserted = FALSE;
		}

		if (bSCInserted == TRUE)
		{	// smart card가 삽입되면, 삽입된 상태임 (추가 작업 필요 없음 )
			return TRUE;
		}
	}
	return FALSE;
}

#if defined(CONFIG_OP_UPC)
static HBOOL is_upc_disable_spec(void)
{
	HERROR		hErr = ERR_FAIL;
	HINT32		nDisableUpc = FALSE;

	hErr = DB_PARAM_GetItem(MENUCFG_ITEMNAME_DISABLE_UPC_SPEC, (HUINT32 *)&nDisableUpc, 0);
	if( (hErr == ERR_OK) && (nDisableUpc == TRUE))
	{
		return TRUE;
	}
	return FALSE;
}
#endif


/*****************************************************************************
  Function    : local_ir_product_has_this_product
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
HUINT32 local_ir_product_has_this_product(HUINT32 queryProductID)
{
	IR_API_SC_INFO	stIrApiScInfo;
	HINT32			i=0, j=0;

	if (is_smartcard_in() == FALSE)
		return e_product_block_state_fail_no_card;

	/* 1. smartcard에서 info를 읽는다. */
	if(xsvc_cas_IrScInfo(0, &stIrApiScInfo) == ERR_OK)
	{
		for(i=0; i<stIrApiScInfo.nNumberOfSecotrs; i++)
		{
			if((stIrApiScInfo.stExtProductListData[i].stExtProductListSector.fSectorValid == 1) &&
			   (stIrApiScInfo.stExtProductListData[i].stExtProductListSector.bProductCount>0))
			{
				/* 2. pvr product를 확인한다. */
				for(j=0; j<stIrApiScInfo.stExtProductListData[i].stExtProductListSector.bProductCount; j++){
					if (queryProductID == stIrApiScInfo.stExtProductListData[i].p_ExtProductListPorduct[j].wProductId){
						IrPRODUCTBlockPrint(DBG_TRACE, ("[%s]:%d This card has Product[0x%x] \n", __FUNCTION__, __LINE__, (HUINT16)queryProductID));
						return e_product_block_state_ok;
					}
				}
			}
		}
	}
	return e_product_block_state_fail_no_product;
}


/*****************************************************************************
  Function    : CAS_IR_PRODUCT_BLOCK_OP_ProductID
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
HUINT32 CAS_IR_PRODUCT_BLOCK_OP_ProductID(HUINT16 iProductID)
{
	HUINT16		usFoundID;

	IrPRODUCTBlockPrint(DBG_TRACE, ("[%s] ++\n", __FUNCTION__));

	switch(iProductID)
	{
		case IR_UPC_PRODUCT_BLOCK_ID_PORTAL:
		case IR_UPC_PRODUCT_BLOCK_ID_PVR:
		case IR_UPC_PRODUCT_BLOCK_ID_DLNA:
			usFoundID = TRUE;
			IrPRODUCTBlockPrint(DBG_TRACE, ("iProductID is same as UPC_PRODUCT_BLOCK_ID:[0x%x] \n", (HUINT16)iProductID));
			break;
		default:
			usFoundID = FALSE;
			IrPRODUCTBlockPrint(DBG_TRACE, ("iProductID:[0x%x] is not as UPC_PRODUCT_BLOCK_ID:[0x%x, 0x%x, 0x%x] \n",
				iProductID, (HUINT16)IR_UPC_PRODUCT_BLOCK_ID_PORTAL, (HUINT16)IR_UPC_PRODUCT_BLOCK_ID_PVR, (HUINT16)IR_UPC_PRODUCT_BLOCK_ID_DLNA));
			break;
	}
	IrPRODUCTBlockPrint(DBG_TRACE, ("[%s] --\n", __FUNCTION__));

	return usFoundID;

}
//CAS_IR_DRV_SetSCExtendedProductList 참고 ...


/*****************************************************************************
  Function    : CAS_IR_PRODUCT_BLOCK_has_pvr_product
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
HUINT32 CAS_IR_PRODUCT_BLOCK_has_pvr_product(void)
{
#if defined(CONFIG_OP_UPC)
	if ( is_upc_disable_spec() == TRUE) return e_product_block_state_ok;
#endif
	return local_ir_product_has_this_product(IR_UPC_PRODUCT_BLOCK_ID_PVR);
}


/*****************************************************************************
  Function    : CAS_IR_PRODUCT_BLOCK_has_tvportal_product
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
HUINT32 CAS_IR_PRODUCT_BLOCK_has_tvportal_product(void)
{
#if defined(CONFIG_OP_UPC)
		if ( is_upc_disable_spec() == TRUE) return e_product_block_state_ok;
#endif
	return local_ir_product_has_this_product(IR_UPC_PRODUCT_BLOCK_ID_PORTAL);
}


/*****************************************************************************
  Function    : CAS_IR_PRODUCT_BLOCK_has_tvportal_product
  Parameters  :
  Output      :
  Return      :
  Description : pvr 및 portal product가 동시에 존재하면 DLNA 권한이 가진 것이다.
*****************************************************************************/
HUINT32 CAS_IR_PRODUCT_BLOCK_has_dlna_right(void)
{
	HUINT32 uiDlnaResult = e_product_block_state_fail_no_product;

#if defined(CONFIG_OP_UPC)
		if ( is_upc_disable_spec() == TRUE) return e_product_block_state_ok;
#endif

	uiDlnaResult = local_ir_product_has_this_product(IR_UPC_PRODUCT_BLOCK_ID_PVR);
	if (uiDlnaResult != e_product_block_state_ok)
		return uiDlnaResult;

	uiDlnaResult = local_ir_product_has_this_product(IR_UPC_PRODUCT_BLOCK_ID_PORTAL);
	if (uiDlnaResult != e_product_block_state_ok)
		return uiDlnaResult;

	/* pvr 및 portal product가 존재한다. */
	return e_product_block_state_ok;
}


