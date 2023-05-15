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
call %SCRIPT_DIR%\setenv.bat
if /i NOT %ERR_CODE%==0 goto error

@echo Output Directory [%OUTPUT_DIR%]
SET OUTPUT_FILE_NAME=%PROD_NAME%_gang.hdf
SET OUTPUT_GANG_BIN_NAME=%PROD_NAME%_gang.bin
SET OUTPUT_GANG_LOADER_BIN_NAME=%PROD_NAME%_gang_factory_loader.bin
SET CFG_NAME=gang.cfg


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
if /i NOT %ERR_CODE%==0 goto error

@echo STEP 2. Create Singed/Encrypted Kernel/Rootfs Binary.
%BIN_DIR%\makehdf.exe enc_sign_kernel_N_rootfs.cfg fvp4000t_enc_N_sign_tmp.hdf
if NOT EXIST fvp4000t_enc_N_sign_tmp.hdf (
	SET ERR_CODE=5
	goto error
)

@echo STEP 3. Make UBI Binary
@echo off
%BIN_DIR%\makehdf app.cfg UBI_CODE.hdf
%BIN_DIR%\mg.exe ubi.lst UBI_CODE ubi_code.bin
%BIN_DIR%\mg.exe ubi.lst UBI_RAWFS ubi_rawfs.bin


@echo STEP 4. Make CRC of UBI Code Area
@echo off
%BIN_DIR%\crc32.exe ubi_code.bin ubi_code.crc


@echo STEP 5. Make NVRAM Binary
@echo off
@if exist ronvram.bin del ronvram.bin
@if exist rwnvram.bin del rwnvram.bin
%BIN_DIR%\makenvram ronvram.cfg ronvram.bin
%BIN_DIR%\makenvram rwnvram.cfg rwnvram.bin
%BIN_DIR%\mg.exe ubi.lst MG_NVRAM ubi_nvram_mg.bin


@echo STEP 6. Merge UBI Area
@echo off
%BIN_DIR%\mg.exe ubi.lst RF_K_NVR %OUTPUT_DIR%\%OUTPUT_GANG_BIN_NAME%

@echo STEP 7. Make GANG HDF
:make_images
if EXIST %OUTPUT_DIR%\%OUTPUT_FILE_NAME% del %OUTPUT_DIR%\%OUTPUT_FILE_NAME%
%BIN_DIR%\makehdf.exe %CFG_NAME% %OUTPUT_DIR%\%OUTPUT_FILE_NAME%
if NOT EXIST %OUTPUT_DIR%\%OUTPUT_FILE_NAME% (
	SET ERR_CODE=5
	goto error
)
copy loader_f.bin .\%OUTPUT_DIR%\%OUTPUT_GANG_LOADER_BIN_NAME%
@echo call clean.bat

:display_msg
@echo [%OUTPUT_DIR%\%OUTPUT_FILE_NAME%] Created!!
explorer %OUTPUT_DIR%
goto finish

:error
@echo Fail to make hdf file : error code [%ERR_CODE%]

:finish
@echo off
clean.bat
