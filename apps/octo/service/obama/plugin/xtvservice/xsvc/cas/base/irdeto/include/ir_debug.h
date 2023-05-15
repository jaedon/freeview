#ifndef __IR_DEBUG_H__
#define	__IR_DEBUG_H__

 
/********************************************************************
*   type definition
********************************************************************/


void CAS_IR_DBG_ParseMessage(HMSG* msg );
void CAS_IR_DBG_STATUS(HUINT32 eSvcType, ia_word16 wServiceHandle, HUINT32	Source, ia_status_st status);

#endif

