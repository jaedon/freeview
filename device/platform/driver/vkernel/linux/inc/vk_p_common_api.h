/*************************************************************
 * @file		vk_p_common_api.h
 * @date		
 * @author		
 * @brief		

 * http://www.humaxdigital.com
 *************************************************************/

#define ___HEADER________________________________________________________________________________________

#define ___DEFINE________________________________________________________________________________________

#define ___TYPEDEF_______________________________________________________________________________________

#define ___STATIC_VARIABLES______________________________________________________________________________

#define ___PRIVATE_FUNCTION_PROTOTYPE____________________________________________________________________

#define	___PRIVATE_API___________________________________________________________________________________

#define	___PUBLIC_API____________________________________________________________________________________

int vk_p_common_GetPid(void);
int vk_p_common_GetTid(void);
int vk_p_common_GetProcessName(int pid, char *buffer, int size);
int vk_p_common_GetThreadName(int pid, int tid, char *buffer, int size);
int vk_p_common_PrintReturnFunction(int pid, void *ret_addr, const char *file, unsigned long line);
void vk_p_common_ShowBackTrace(char* pucProcessName, void *oldcontext);

