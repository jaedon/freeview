#!/bin/bash

awk '/# << Operator Spec/,/# Operator specific sub configurations/' .config > new_config
awk '/PRODUCT_NAME/' .config >> new_config
awk '/CONFIG_DEBUG/' .config >> new_config
awk '/CONFIG_USES_NFSROOT/' .config >> new_config
awk '/CONFIG_ENHANCED_SECURITY/' .config >> new_config
awk '/CONFIG_PRIVILEGE_CONTROL/' .config >> new_config
awk '/CONFIG_ENABLE_LXC/' .config >> new_config
awk '/CONFIG_USE_HAMA/' .config >> new_config
awk '/CONFIG_SUPPORT_MHEG_IB/' .config >> new_config
awk '/CONFIG_PROD_HMS1000S/' .config >> new_config
awk '/CONFIG_PROD_IR4000HD/' .config >> new_config
awk '/CONFIG_PROD_HDR1000S/' .config >> new_config
awk '/CONFIG_TARGET_ROOTFS_SQUASH/' .config >> new_config
awk '/CONFIG_IPEPG_SERVER_URL/' .config >> new_config
awk '/CONFIG_RP_SERVER_URL/' .config >> new_config
awk '/CONFIG_APPPORTAL_SERVER_URL/' .config >> new_config
awk '/CONFIG_IPOTA_SERVER_URL/' .config >> new_config
awk '/CONFIG_FUNC_EPG_USE_SVCUID/' .config >> new_config
awk '/CONFIG_/' new_config > new_config2

awk '$1 !~ /^[#]/' new_config2 >  latest_config

( 
	echo \##########################################
	echo \#    OCTO 2.0 global configuration	
	echo \#    generation time : $(date --utc)
	echo \##########################################
	echo \
)	> ./.globalconfig
cat ./latest_config >> ./.globalconfig
{
	echo \ 
	echo \##########################################
} >> ./.globalconfig

awk -v date_string="$(date --utc )" '
	BEGIN {
		printf "/***********************************************\n";
		printf " * This is OCTO 2.0 global configuration header\n";
		printf " *  - Generation Time :%s\n",date_string;	
		printf " ***********************************************/\n\n";
		printf "#ifndef __GLOBAL_CONFIG_H__\n";
		printf "#define __GLOBAL_CONFIG_H__\n\n";
		printf "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n";
	}
	{
		name_of_define = substr($1,1,(index($1,"=")-1));
		name_of_value = substr($1,(index($1,"=")+1),length($1));
		if ( "y" == name_of_value)
		{	
			printf "#define %s %s\n",name_of_define,1;
		}
		else
		{
			printf "#define %s %s\n",name_of_define,name_of_value;
		}
	}
	END {
		printf "\n#ifdef __cplusplus\n}\n#endif\n\n";
		printf "#endif // __GLOBAL_CONFIG_H__\n";
	}' latest_config > ./include/globalconfig.h
rm new_config
rm new_config2
rm latest_config

