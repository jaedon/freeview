@echo off

call clean.bat

SET BIN_DIR=..\Util
SET SCRIPT_DIR=..\Script
if "%1"=="" (
	SET INPUT_DIR=..\Image
) else (
	SET INPUT_DIR=..\Image\%1
)
if NOT EXIST %INPUT_DIR% (
	@echo        Please copy image files into [%INPUT_DIR%] directory
	@echo        and execute this bat file once again!
	@echo off
	goto error
)

SET OUTPUT_DIR=.\output
if NOT EXIST %OUTPUT_DIR% (
	mkdir %OUTPUT_DIR%
) else (
	REM del %OUTPUT_DIR%\*
)

SET ERR_CODE=0
call %SCRIPT_DIR%\setenv.bat
if /i NOT %ERR_CODE%==0 goto error

@echo Copy Application Version
call %SCRIPT_DIR%\cp.bat version %INPUT_DIR%
if /i NOT %ERR_CODE%==0 goto error

@echo Output Directory [%OUTPUT_DIR%]
SET OUTPUT_FILE_NAME=%PROD_NAME%_upgrade_loader.hdf
SET CFG_NAME=app_loader.cfg

@echo STEP 0. Copy Configuration Files from binary directory
@echo off

SET ERR_CODE=0
@echo Copy Application Images
call %SCRIPT_DIR%\cp.bat app_onerootfs %INPUT_DIR%
if /i NOT %ERR_CODE%==0 goto error

@echo Copy Loader Images
call %SCRIPT_DIR%\cp.bat loader %INPUT_DIR%
if /i NOT %ERR_CODE%==0 goto error

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
@echo Fail to make application image : error code [%ERR_CODE%]

:finish
@echo off

