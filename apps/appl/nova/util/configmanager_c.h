#ifndef CONFIGMANAGER_C_H
#define CONFIGMANAGER_C_H

#define		CONFIG_USER_TARA_PATH	"/usr/tara"
#define		CONFIG_HUMAXTV_PATH		"/var/lib/humaxtv"


#ifdef __cplusplus
extern "C" {
#endif

typedef void* HANDLE_CONIFG;

HANDLE_CONIFG	CONFIG_Create			(char* configVersion);
bool			CONFIG_Read				(HANDLE_CONIFG hConf ,char* path);
bool			CONFIG_Save				(HANDLE_CONIFG hConf);
bool			CONFIG_Close			(HANDLE_CONIFG hConf);
bool			CONFIG_SetConfigVersion	(HANDLE_CONIFG hConf, char* configVersion);
const char*		CONFIG_GetString		(HANDLE_CONIFG hConf, char* name);
int				CONFIG_GetValue			(HANDLE_CONIFG hConf, char* name);
bool			CONFIG_SetString		(HANDLE_CONIFG hConf, char* name, char* value);
bool			CONFIG_SetValue			(HANDLE_CONIFG hConf, char* name, int value);
bool			CONFIG_DeleteConfig		(HANDLE_CONIFG hConf, char* name);
void			CONFIG_PrintAll			(HANDLE_CONIFG hConf);

bool			CONFIG_CopyFilesToFlash	(void);
#ifdef __cplusplus
}
#endif

#endif // CONFIGMANAGER_C_H
