/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_reg_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 2/2/12 5:42p $
 *
 * Module Description: APE Register Routines
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_reg_priv.h $
 * 
 * Hydra_Software_Devel/3   2/2/12 5:42p gskerl
 * SW7429-18: Added BAPE_Reg_P_ReadField() and BAPE_Reg_P_ReadField_isr().
 * 
 * Hydra_Software_Devel/2   1/13/12 3:31p gskerl
 * SW7429-18: Added BAPE_ wrappers for BCHP_ field macros in order to
 * provide macro expansion for arguments.
 * 
 * Hydra_Software_Devel/1   11/14/11 3:14p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/3   11/7/11 2:03p gskerl
 * SW7429-18: Fixed parentheses in BAPE_Reg_P_Update() and
 * BAPE_Reg_P_Update_isr()
 * 
 * Hydra_Software_Devel/SW7429-18/2   10/25/11 5:33p jgarrett
 * SW7429-18: Adding _isr version of InitFieldList
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:40p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 ***************************************************************************/
#ifndef BAPE_REG_PRIV_H_
#define BAPE_REG_PRIV_H_

#include "breg_mem.h"

/* Compute Array Register Address */
#define BAPE_Reg_P_GetArrayAddress(ArrayName, ArrayIndex) (BDBG_ASSERT((ArrayIndex)<=BCHP_##ArrayName##_ARRAY_END),(BCHP_##ArrayName##_ARRAY_BASE)+((ArrayIndex)*((BCHP_##ArrayName##_ARRAY_ELEMENT_SIZE)/8)))

/* Read/Write registers */
#define BAPE_Reg_P_Read(hApe, regAddr) (BREG_Read32((hApe)->regHandle, (regAddr)))
#define BAPE_Reg_P_Write(hApe, regAddr, regVal) (BREG_Write32((hApe)->regHandle, (regAddr), (regVal)))

#define BAPE_Reg_P_Read_isr(hApe, regAddr) (BREG_Read32_isr((hApe)->regHandle, (regAddr)))
#define BAPE_Reg_P_Write_isr(hApe, regAddr, regVal) (BREG_Write32_isr((hApe)->regHandle, (regAddr), (regVal)))

#define BAPE_Reg_P_ReadField(hApe, regAddr, regName, fieldName)        BCHP_GET_FIELD_DATA(BAPE_Reg_P_Read    (hApe, regAddr),regName,fieldName)
#define BAPE_Reg_P_ReadField_isr(hApe, regAddr, regName, fieldName)    BCHP_GET_FIELD_DATA(BAPE_Reg_P_Read_isr(hApe, regAddr),regName,fieldName)

/* Update registers based on address, mask, value */
#define BAPE_Reg_P_Update(hApe, regAddr, regMask, regVal) (BREG_Write32((hApe)->regHandle, (regAddr), (BREG_Read32((hApe)->regHandle, (regAddr)) & ~(regMask)) | (regVal)))
#define BAPE_Reg_P_UpdateAtomic(hApe, regAddr, regMask, regVal) (BREG_AtomicUpdate32((hApe)->regHandle, regAddr, regMask, regVal))

#define BAPE_Reg_P_Update_isr(hApe, regAddr, regMask, regVal) (BREG_Write32_isr((hApe)->regHandle, (regAddr), (BREG_Read32((hApe)->regHandle, (regAddr)) & ~(regMask)) | (regVal)))
#define BAPE_Reg_P_UpdateAtomic_isr(hApe, regAddr, regMask, regVal) (BREG_AtomicUpdate32_isr((hApe)->regHandle, regAddr, regMask, regVal))

/* Update single field of a register */
#define BAPE_Reg_P_UpdateField(hApe, regAddr, regName, fieldName, fieldData) (BAPE_Reg_P_Update(hApe, regAddr, BCHP_MASK(regName, fieldName), BCHP_FIELD_DATA(regName, fieldName, fieldData)))
#define BAPE_Reg_P_UpdateFieldAtomic(hApe, regAddr, regName, fieldName, fieldData) (BAPE_Reg_P_AtomicUpdate(hApe, regAddr, BCHP_MASK(regName, fieldName), BCHP_FIELD_DATA(regName, fieldName, fieldData)))

#define BAPE_Reg_P_UpdateField_isr(hApe, regAddr, regName, fieldName, fieldData) (BAPE_Reg_P_Update_isr(hApe, regAddr, BCHP_MASK(regName, fieldName), BCHP_FIELD_DATA(regName, fieldName, fieldData)))
#define BAPE_Reg_P_UpdateFieldAtomic_isr(hApe, regAddr, regName, fieldName, fieldData) (BAPE_Reg_P_AtomicUpdate_isr(hApe, regAddr, BCHP_MASK(regName, fieldName), BCHP_FIELD_DATA(regName, fieldName, fieldData)))

/* Update single enum field of a register */
#define BAPE_Reg_P_UpdateEnum(hApe, regAddr, regName, fieldName, fieldEnumVal) (BAPE_Reg_P_Update(hApe, regAddr, BCHP_MASK(regName, fieldName), BCHP_FIELD_ENUM(regName, fieldName, fieldEnumVal)))
#define BAPE_Reg_P_UpdateEnumAtomic(hApe, regAddr, regName, fieldName, fieldEnumVal) (BAPE_Reg_P_AtomicUpdate(hApe, regAddr, BCHP_MASK(regName, fieldName), BCHP_FIELD_ENUM(regName, fieldName, fieldEnumVal)))

#define BAPE_Reg_P_UpdateEnum_isr(hApe, regAddr, regName, fieldName, fieldEnumVal) (BAPE_Reg_P_Update_isr(hApe, regAddr, BCHP_MASK(regName, fieldName), BCHP_FIELD_ENUM(regName, fieldName, fieldEnumVal)))
#define BAPE_Reg_P_UpdateEnumAtomic_isr(hApe, regAddr, regName, fieldName, fieldEnumVal) (BAPE_Reg_P_AtomicUpdate_isr(hApe, regAddr, BCHP_MASK(regName, fieldName), BCHP_FIELD_ENUM(regName, fieldName, fieldEnumVal)))

/* Create some wrapper macros for the BCHP_ field macros.
   This is needed to allow for macro-expansion of arguments.
   Currently, BCHP_FIELD_ENUM, BCHP_MASK, and BCHP_SHIFT do NOT
   macro-expand their arguments.  */
#define BAPE_P_FIELD_ENUM(Register,Field,Name) \
        BCHP_FIELD_ENUM(Register,Field,Name)

#define BAPE_P_BCHP_MASK(Register,Field) \
        BCHP_MASK(Register,Field)

#define BAPE_P_BCHP_SHIFT(Register,Field) \
        BCHP_SHIFT(Register,Field)

#define BAPE_P_BCHP_ENUM_private(Register,Field,Name) \
        BCHP_##Register##_##Field##_##Name

#define BAPE_P_BCHP_ENUM(Register,Field,Name)  \
        BAPE_P_BCHP_ENUM_private(Register,Field,Name)      /* Using wrapper macro to provide macro-expansion of args */


/* Field List */
BDBG_OBJECT_ID_DECLARE(BAPE_Reg_P_FieldList);
typedef struct BAPE_Reg_P_FieldList
{
    BDBG_OBJECT(BAPE_Reg_P_FieldList)
    uint32_t mask, value;
    BAPE_Handle hApe;
}BAPE_Reg_P_FieldList;

/* Initialize Field List */
#define BAPE_Reg_P_InitFieldList(ApeHandle, FieldList) do { BDBG_OBJECT_SET(FieldList, BAPE_Reg_P_FieldList); (FieldList)->mask=0; (FieldList)->value=0; (FieldList)->hApe=(ApeHandle); } while(0)
#define BAPE_Reg_P_InitFieldList_isr BAPE_Reg_P_InitFieldList

/* Add Value to field list */
#define BAPE_Reg_P_AddMaskValueToFieldList_isr BAPE_Reg_P_AddMaskValueToFieldList
void BAPE_Reg_P_AddMaskValueToFieldList(BAPE_Reg_P_FieldList *pFieldList, uint32_t mask, uint32_t value);

#define BAPE_Reg_P_AddToFieldList(FieldList, RegName, FieldName, FieldData) BAPE_Reg_P_AddMaskValueToFieldList(FieldList, BCHP_MASK(RegName, FieldName), BCHP_FIELD_DATA(RegName, FieldName, FieldData))
#define BAPE_Reg_P_AddEnumToFieldList(FieldList, RegName, FieldName, FieldEnum) BAPE_Reg_P_AddMaskValueToFieldList(FieldList, BCHP_MASK(RegName, FieldName), BCHP_FIELD_ENUM(RegName, FieldName, FieldEnum))

#define BAPE_Reg_P_AddToFieldList_isr(FieldList, RegName, FieldName, FieldData) BAPE_Reg_P_AddMaskValueToFieldList_isr(FieldList, BCHP_MASK(RegName, FieldName), BCHP_FIELD_DATA(RegName, FieldName, FieldData))
#define BAPE_Reg_P_AddEnumToFieldList_isr(FieldList, RegName, FieldName, FieldEnum) BAPE_Reg_P_AddMaskValueToFieldList_isr(FieldList, BCHP_MASK(RegName, FieldName), BCHP_FIELD_ENUM(RegName, FieldName, FieldEnum))

/* Apply field list to a register */
void BAPE_Reg_P_ApplyFieldList(BAPE_Reg_P_FieldList *pFieldList, uint32_t address);
void BAPE_Reg_P_ApplyFieldListAtomic(BAPE_Reg_P_FieldList *pFieldList, uint32_t address);

void BAPE_Reg_P_ApplyFieldList_isr(BAPE_Reg_P_FieldList *pFieldList, uint32_t address);
void BAPE_Reg_P_ApplyFieldListAtomic_isr(BAPE_Reg_P_FieldList *pFieldList, uint32_t address);

#endif /* !defined BAPE_REG_PRIV_H_ */
