@echo off

call clean.bat

SET BIN_DIR=..\Util
SET SCRIPT_DIR=..\Script
SET INPUT_DIR=..\Image
SET OUTPUT_DIR=.\output
if NOT EXIST %OUTPUT_DIR% (
	mkdir %OUTPUT_DIR%
) else (
	REM del %OUTPUT_DIR%\*
)

SET ERR_CODE=0
call %SCRIPT_DIR%\setenv_fvp4000t.bat
if /i NOT %ERR_CODE%==0 goto error

@echo Output Directory [%OUTPUT_DIR%]
SET OUTPUT_FILE_NAME=%PROD_NAME%_compare_fvp4000t.hdf
SET CFG_NAME=compare_fvp4000t.cfg


@echo STEP 0. Copy Configuration Files from binary directory
@echo off
SET ERR_CODE=0
call %SCRIPT_DIR%\cp.bat gang %INPUT_DIR%
if /i NOT %ERR_CODE%==0 goto error
call %SCRIPT_DIR%\cp.bat loader %INPUT_DIR%
if /i NOT %ERR_CODE%==0 goto error


@echo STEP 1. Unzip DB and NVRAM files
@echo off
%BIN_DIR%\unzip.exe -o db.bin.zip
%BIN_DIR%\unzip.exe -o dbbackup.bin.zip
%BIN_DIR%\unzip.exe -o dbuser.bin.zip
%BIN_DIR%\unzip.exe -o nvram_fvp4000t.bin.zip
if /i NOT %ERR_CODE%==0 goto error

@echo STEP 2. Create Singed/Encrypted Kernel/Rootfs Binary.
%BIN_DIR%\makehdf.exe enc_sign_kernel_N_rootfs.cfg fvp4000t_enc_N_sign_tmp.hdf
if NOT EXIST fvp4000t_enc_N_sign_tmp.hdf (
	SET ERR_CODE=5
	goto error
)

@echo STEP 3. Make UBI Binary
@echo off
%BIN_DIR%\mg.exe ubi.lst UBI_CODE ubi_code.bin
%BIN_DIR%\mg.exe ubi.lst UBI_RAWFS ubi_rawfs.bin


@echo STEP 4. Make CRC of UBI Code Area
@echo off
%BIN_DIR%\crc32.exe ubi_code.bin ubi_code.crc


@echo STEP 5. Make NVRAM Binary
@echo off
@if exist ronvram.bin del ronvram.bin
@if exist rwnvram.bin del rwnvram.bin
%BIN_DIR%\makenvram ronvram_fvp4000t.cfg ronvram.bin
%BIN_DIR%\makenvram rwnvram.cfg rwnvram.bin
%BIN_DIR%\mg.exe ubi.lst MG_NVRAM ubi_nvram_mg.bin


@echo STEP 6. Make GANG HDF
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

