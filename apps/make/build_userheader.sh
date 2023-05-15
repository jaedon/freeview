#!/bin/bash

(echo
 echo Generating User And Group Header for Enhanced Security Model)

sudo awk -F: '{print "#define UID_" $1 "	" $3 }' ./_gen_rootfs/root/etc/passwd > ./tmp_header.h
sudo awk -F: '{print "#define GID_" $1 "	" $3 }' ./_gen_rootfs/root/etc/group >> ./tmp_header.h

sudo awk -F: -v date_string="$(date --utc )" '
	BEGIN {
		printf "/*****************************************\n";
		printf " * OCTO 2.0 Secure Header \n";
		printf " * Generated %s\n",date_string;
		printf " *****************************************/\n";
		printf "#ifndef __SECURE_HEADER_H__\n";
		printf "#define __SECURE_HEADER_H__\n";
		printf "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n";
	}
	{
		sub("-","",$1);
		printf "%s\n",$1;
	}
	END {
		printf "\n#ifdef __cplusplus\n}\n#endif\n\n";
		printf "#endif // __SECURE_HEADER_H__\n";
	}' tmp_header.h > ./include/secure_header.h

sudo rm -f tmp_header.h

(echo ./include/secure_header.h is generated...)
(cat ./include/secure_header.h)
(echo
 echo )
