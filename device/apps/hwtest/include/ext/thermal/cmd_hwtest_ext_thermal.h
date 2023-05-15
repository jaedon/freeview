/**
 * cmd_hwtest.h
*/

/**
 * @defgroup		CMD_HWTEST
 * @author			Chung Dong Chul
 * @note			
 * @brief			
 * @file 			cmd_hwtest.h
*/

#ifndef __CMD_HWTEST_EXT_THERMAL_H__
#define __CMD_HWTEST_EXT_THERMAL_H__


void P_CMDHWTEST_EXT_ThermalTest_BeginThermalTest(unsigned int isStart, unsigned int mode);
void P_CMDHWTEST_EXT_InitThermalTest(void);
void P_CMDHWTEST_StartHDDTempMonitor(void);
void P_CMDHWTEST_StopHDDTempMonitor(void);
void P_CMDHWTEST_InitFanMonitor(void);
void P_CMDHWTEST_StartFanAutoMonitor(void);
void P_CMDHWTEST_StopFanAutoMonitor(void);

#endif
