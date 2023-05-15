@echo off

if "%1"=="" (
	set ERR_CODE=1
	goto error
)
SET IMG_TYPE=%1
@echo IMG_TYPE : [%IMG_TYPE%]

if "%2"=="" (
	@echo Image Directory : [%IMAGE_DIR%]
) else (
	SET IMAGE_DIR=%2
)
@echo Image Directory : [%IMAGE_DIR%]

@echo off
SET ERR_FILE_NAME=
SET ERR_CODE=0
		
if %IMG_TYPE%==app (
	@echo Copying Application images
	SET FILE_LIST=lib.squa bin.squa root.squa share.squa vmlinuz ccb.bin eep_ir.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==app_only (
	@echo Copying Application images only
	SET FILE_LIST=lib.squa bin.squa root.squa share.squa vmlinuz aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==app_onerootfs (
	@echo Copying Application images only
	SET FILE_LIST=root.squa vmlinuz aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==loader (
	@echo Copying Loader Images
	SET FILE_LIST=loader.bin loader_d.bin loader_f_fvp4000t.bin loader_f_fvp5000t.bin user_bg.jpg LOADER_VERSION.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==loader_img (
	@echo Copying Loader Images
	SET FILE_LIST=user_bg.jpg LOADER_VERSION.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==loader_ir (
	@echo Copying Loader Images
	SET FILE_LIST=loader.bin loader_d.bin user_bg.jpg eep_ir.bin LOADER_VERSION.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==config (
	@echo Copying config images
	SET FILE_LIST=def_mac.bin dummy.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
	REM	bbcb.bin
) else if %IMG_TYPE%==mac (
	@echo Copying Default Macaddress image
	SET FILE_LIST=def_mac.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==db (
	@echo Copying DB images
	SET FILE_LIST=db.bin.zip dbbackup.bin.zip dbuser.bin.zip aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==ccb (
	@echo Copying CCB image
	SET FILE_LIST=ccb.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==eeprom (
	@echo Copying EEPROM images
	SET FILE_LIST=eeprom.bin eep_sys.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==eeprom_ir (
	@echo Copying EEPROM images
	SET FILE_LIST=eeprom.bin eep_sys.bin eep_ir.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==eep_ir (
	@echo Copying EEPROM Irdeto Partition image
	SET FILE_LIST=eep_ir.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==eep_clean (
	@echo Copying EEPROM clean image
	SET FILE_LIST=eeprom_clean.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==hdcp (
	@echo Copying HDCP images
	SET FILE_LIST=hdcp.bin hdcp_erase.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==empty_4m (
	@echo Copying Empty image
	SET FILE_LIST=empty4m.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==ciplus (
	@echo Copying CI+ Key
	SET FILE_LIST=ciplus_prod.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==dtcp (
	@echo Copying DTCP Key
	SET FILE_LIST=dtcp.zip aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==gang (
	@echo Copying Gang Images
	SET FILE_LIST=nvram.bin.zip db.bin.zip dbbackup.bin.zip dbuser.bin.zip crcprefx.bin ff.bin root.squa vmlinuz aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==version (
	@echo Copying version binaries
	SET FILE_LIST=APP_VERSION.bin aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==sysid (
	@echo Copying version binaries
	SET FILE_LIST=sysid_fvp4000t.bin sysid_fvp5000t.bin
) else if %IMG_TYPE%==keys (
	@echo Copying version binaries
	SET FILE_LIST=aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==micom (
	@echo Copying version binaries
	SET FILE_LIST=micom.hex aes_key.bin rsa_private_key_2048.bin rsa_public_key_2048.bin
) else if %IMG_TYPE%==packfile (
	@echo Copying packfile binaries
	SET FILE_LIST=packfile.out.bin
) else (
	@echo Invalid Image Type : [%IMG_TYPE%]
	SET ERR_CODE=1
	goto error
)

@echo [%IMG_TYPE%] Image List
FOR %%I IN (%FILE_LIST%) DO (
	@echo Check image [%%I]
	if NOT EXIST %IMAGE_DIR%\%%I (
		SET ERR_CODE=2
		SET ERR_FILE_NAME=%IMAGE_DIR%\%%I
		goto error_file_not_found
	) 
)

@echo Copy [%IMG_TYPE%] Image List
FOR %%I IN (%FILE_LIST%) DO (
	@echo Copy image [%%I]
	copy %IMAGE_DIR%\%%I 
)

goto finish

:error_file_not_found
@echo on
@echo File Not Found!
@echo ERROR_CODE : [%ERR_CODE%]
@echo %ERR_FILE_NAME% Not Found!
goto end

:error
@echo on
@echo An Error Occured!
@echo ERROR_CODE : [%ERR_CODE%]
if %ERR_CODE%==1 ( 
	@echo Invalid Image Type [%IMG_TYPE%]
) else (
	@echo Unknown Error
)
goto end

:finish
@echo [%IMG_TYPE%] Image Copy Finished!

:end
@echo off
