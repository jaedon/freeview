@echo off

call clean.bat

SET BIN_DIR=..\..\Util
SET SCRIPT_DIR=..\..\Script
SET INPUT_DIR=..\..\Image
SET OUTPUT_DIR=.\output
if NOT EXIST %OUTPUT_DIR% (
	mkdir %OUTPUT_DIR%
) else (
	REM del %OUTPUT_DIR%\*
)

SET ERR_CODE=0
call %SCRIPT_DIR%\setenv.bat
if /i NOT %ERR_CODE%==0 goto error

@echo Output Directory [%OUTPUT_DIR%]
SET OUTPUT_FILE_NAME=%PROD_NAME%_upgrade_nvram_for_gang.hdf
SET CFG_NAME=nvram.cfg

@echo STEP 0. Copy Configuration Files from binary directory
@echo off

SET ERR_CODE=0
call %SCRIPT_DIR%\cp.bat keys %INPUT_DIR%
if /i NOT %ERR_CODE%==0 goto error

call %SCRIPT_DIR%\cp.bat dtcp %INPUT_DIR%
if /i NOT %ERR_CODE%==0 goto error

@echo Unzip DTCP files
%BIN_DIR%\unzip.exe -o dtcp.zip

call %SCRIPT_DIR%\cp.bat hdcp %INPUT_DIR%
if /i NOT %ERR_CODE%==0 goto error

call %SCRIPT_DIR%\cp.bat loader_img %INPUT_DIR%
if /i NOT %ERR_CODE%==0 goto error

call %SCRIPT_DIR%\cp.bat version %INPUT_DIR%
if /i NOT %ERR_CODE%==0 goto error

call %SCRIPT_DIR%\cp.bat sysid %INPUT_DIR%
if /i NOT %ERR_CODE%==0 goto error

@echo STEP 1. Make NVRAM Packfile
@echo off
%BIN_DIR%\packfile.exe packfile_gang.cfg packfile.out.bin

@echo STEP 3. Make HDF
@echo off

:make_images
if EXIST %OUTPUT_DIR%\%OUTPUT_FILE_NAME% del %OUTPUT_DIR%\%OUTPUT_FILE_NAME%
%BIN_DIR%\makehdf.exe %CFG_NAME% %OUTPUT_DIR%\%OUTPUT_FILE_NAME%
if NOT EXIST %OUTPUT_DIR%\%OUTPUT_FILE_NAME% (
	SET ERR_CODE=5
	goto error
)
call clean.bat

:display_msg
@echo [%OUTPUT_DIR%\%OUTPUT_FILE_NAME%] Created!!
explorer %OUTPUT_DIR%
goto finish

:error
@echo Fail to make hdf file : error code [%ERR_CODE%]

:finish
@echo off

