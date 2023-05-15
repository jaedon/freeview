@echo off

if "%1"=="" (
	SET ERR_CODE=4
	goto error
)
if "%2"=="" (
	SET ERR_CODE=4
	goto error
)
if "%3"=="" (
	SET ERR_CODE=4
	goto error
)

if NOT DEFINED SCRIPT_DIR (
	SET ERR_CODE=6
	goto error
)

@echo Image Type : [%1]
@echo Primary Directory : [%2]
@echo Secondary Directory : [%3]

SET ERR_CODE=
SET RETRY_CNT=0
SET IMAGE_DIR=%2
:copy_retry
call %SCRIPT_DIR%\cp.bat %1 %IMAGE_DIR%
@echo Copy Retry Count : [%RETRY_CNT%]
if /i NOT %ERR_CODE%==0 (
	if /i %RETRY_CNT%==1 (
		goto end
	)
	SET IMAGE_DIR=%3
	SET RETRY_CNT=1
	goto copy_retry
) else (
	goto finish
)

:error
@echo ERROR_CODE : [%ERR_CODE%]
goto end

:finish
@echo %1 Image Copy Success!!

:end
@echo off
