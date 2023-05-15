#ifndef ADDRESSMANAGER_C_H
#define ADDRESSMANAGER_C_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ADDRMGR_Init(const char *path);
bool ADDRMGR_Deinit(void);
void ADDRMGR_Reset(void);
bool ADDRMGR_Parse(void);
int ADDRMGR_GetItemCount(void);
bool ADDRMGR_AddAddress(const char* mac);
bool ADDRMGR_RemoveAddress(const char* mac);
bool ADDRMGR_SaveAsFile(void);

#ifdef __cplusplus
}
#endif

#endif // ADDRESSMANAGER_C_H
