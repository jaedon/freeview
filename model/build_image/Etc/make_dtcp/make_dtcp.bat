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
SET OUTPUT_FILE_NAME=%PROD_NAME%_upgrade_dtcp

@echo STEP 0. Copy Configuration Files from binary directory
@echo off

SET ERR_CODE=0
call %SCRIPT_DIR%\cp.bat dtcp %INPUT_DIR%
if /i NOT %ERR_CODE%==0 goto error

@echo Unzip DTCP files
%BIN_DIR%\unzip.exe -o dtcp.zip

:make_images
FOR /L %%Z IN (1,1,8) DO (
	if EXIST %OUTPUT_DIR%\%OUTPUT_FILE_NAME% del %OUTPUT_DIR%\%OUTPUT_FILE_NAME%%%Z.hdf
	%BIN_DIR%\makehdf.exe dtcp%%Z.cfg %OUTPUT_DIR%\%OUTPUT_FILE_NAME%%%Z.hdf
	if NOT EXIST %OUTPUT_DIR%\%OUTPUT_FILE_NAME%%%Z.hdf (
	SET ERR_CODE=5
	goto error
)
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

