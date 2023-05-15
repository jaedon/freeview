@echo off

SET BIN_DIR=..\..\Util
SET OUTPUT_DIR=.\
SET SCRIPT_DIR=..\..\Script
SET INPUT_DIR=..\..\Image


if "%1"=="" (
	SET CFG_NAME=sysid_fvp4000t.cfg
) else (
	SET CFG_NAME=%1
)

if /i %CFG_NAME%==help (
	@echo Usage1 : %0 help : Print This Message
	@echo Usage2 : %0 [config_file_name] [output_file_name]
	@echo config_file_name : default name sysid_fvp4000t.cfg
	@echo output_file_name : default name sysid_fvp4000t.hdf
	@echo e.g. %0 : output sysid_fvp4000t.hdf
	@echo e.g. %0 sysid_00001111.cfg : output sysid_fvp4000t.hdf
	@echo e.g. %0 sysid_00001111.cfg sysid_00001111.hdf : output sysid_00001111.hdf
	goto finish
)

if NOT EXIST %CFG_NAME% (
	@echo config file [%CFG_NAME%] not found
	SET ERR_CODE=2
	goto error
)

if "%2"=="" (
	SET OUTPUT_FILE_NAME=fvp4000t_upgrade_systemid.hdf
) else (
	SET OUTPUT_FILE_NAME=%2
)

@echo STEP 0. Copy Configuration Files from binary directory
@echo off
SET ERR_CODE=0
call %SCRIPT_DIR%\cp.bat sysid %INPUT_DIR%
if /i NOT %ERR_CODE%==0 goto error

%BIN_DIR%\makehdf.exe %CFG_NAME% %OUTPUT_DIR%\%OUTPUT_FILE_NAME%
if NOT EXIST %OUTPUT_DIR%\%OUTPUT_FILE_NAME% (
	SET ERR_CODE=5
	goto error
) 
explorer %OUTPUT_DIR%
goto finish

:error
@echo Fail to make application image : error code [%ERR_CODE%]

:finish
@echo off
