#ifndef __IR_UTIL_H__
#define	__IR_UTIL_H__

 
/********************************************************************
*   type definition
********************************************************************/
#define MAKE_IR_STATUSCODE(ch, status, source)					((ch <<24) | (status <<8) | source)


HBOOL Is_D29_4 (ia_status_st stStatus);
HBOOL Is_D100_8 (ia_status_st stStatus);
HBOOL Is_I34_9 (ia_status_st stStatus);
HBOOL Is_I07_4 (ia_status_st stStatus);
HBOOL Is_E04_4 (ia_status_st stStatus);
HBOOL Is_E05_4 (ia_status_st stStatus);
HBOOL Is_E06_4 (ia_status_st stStatus);
HBOOL Is_D00_4 (ia_status_st stStatus);
HBOOL Is_E136orE137_4 (ia_status_st stStatus);
char SEVERITY_TO_CHAR (ia_status_severity_e eSeverity);

#endif

/*	end of file	*/


