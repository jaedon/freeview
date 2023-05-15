#!/bin/bash

# dowh.sh
#
# 2016-06-01: Ver 0.11, by khwoo
# - add command branch_info for branch source codes.
#
# 2016-06-01: Ver 0.10, by khwoo
# - add command cp for branch source codes.
#
# 2015-10-22: Ver 0.9, by khwoo
# - add "include" command to seperate download script for each components.
#
# 2009-11-20: Ver 0.6, by kimkb
# - add command, cleanup, revert, local_info and server_info for local directory.
#
# 2008-12-01: Ver 0.5, by dssonds
# - down: drr (distributed repository reference) initital script version for distributed repository reference
#

SCMTOOL=svn
SVNCLEAN=./svn-clean
BASE_URL=http://svn.humaxdigital.com

svn --help | grep "^usage:" > chk_file
read HELP_MSG < chk_file 
rm -f chk_file

if [ -n "${HELP_MSG}" ]; then
  SVN_REVISION=Revision:
else
	SVN_REVISION="Revision"
fi

[ -f $1 ] || echo file not found $1 >> .error_occured_tag

# if second paramter is not exist, default action is subversion update.
# you can set co/ci/cleanup/etc.....
case "$2" in
	?*) ACTION=$2;;
	'') ACTION=co
esac 

# update model/down folder firstly..
svn up > /dev/null
VAR_ACTION_CP_PASS=FALSE

if [ ${ACTION} = "branch_info" ] ; then
	if [ -z $3 ]; then
		echo file not found $3 >> .error_occured_tag;
		echo help:./down.sh [branch_txt] branch_info [full_head] [full_branch] [BRANCH_NAME]
		exit
	fi
	if [ -z $4 ]; then
		echo no file $4 >> .error_occured_tag;
		echo help:./down.sh [branch_txt] branch_info [full_head] [full_branch] [BRANCH_NAME]
		exit
	fi
	if [ -z $5 ]; then
		echo no file $4 >> .error_occured_tag;
		echo help:./down.sh [branch_txt] branch_info [full_head] [full_branch] [BRANCH_NAME]
		exit
	fi
	[ -f $3 ] || echo file not found $3 >> .error_occured_tag
	cp $3 $4
fi

if [ ${ACTION} = "cp" ] ; then
	if [ -z $3 ]; then
		echo help:./down.sh [branch_path] cp [BRANCH_NAME]
		echo file not found $3 >> .error_occured_tag
		exit
	fi
fi

while read SRC_NAME SRC_PATH SRC_REV
do
  if [ ${ACTION} = "cp" ] && [ -n "${SRC_NAME}" ] && [ ${VAR_ACTION_CP_PASS} = "FALSE" ] ; then
		if [ ${SRC_NAME:0:3} = "#cp" ]; then
		VAR_ACTION_CP_PASS=TRUE
		else
			echo syntax error in $1 file for cp command.
			exit
		fi
	fi

  case "${SRC_NAME:0:1}" in '#')
			continue;
	esac

  case "${SRC_NAME}" in 'include')
		./_down.sh ${SRC_PATH} ${ACTION}
		continue;
	esac

  case "${SRC_NAME}" in 'BASE_URL')
		BASE_URL=${SRC_PATH}
		continue;
	esac

	if [ -z ${SRC_NAME} ]; then
		continue;
	fi

	if [ -z ${SRC_PATH} ]; then
		continue;
	fi
	
	if [ ${SRC_PATH:0:7} = "http://" ]; then
		SRC_FULL_PATH=${SRC_PATH}
	POS=`expr index "${SRC_PATH:7}" / + 7`
	BASE_URL=${SRC_PATH:0:${POS}}
	echo BASE_URL=${BASE_URL}
	SRC_PATH=${SRC_PATH/${BASE_URL}/}
	echo RELATIVE PATH=${SRC_PATH}
	echo FULL_PATH=${BASE_URL}${SRC_PATH}
	else
		SRC_FULL_PATH=${BASE_URL}/${SRC_PATH}
	fi

	if [ ${SRC_NAME:0:7} = "file:///" ]; then
	echo ============================${POS}
	POS=`expr index "${SRC_NAME:7}" / + 7`
	echo ============================${POS}
	echo ============================${BASE_URL}
	echo ============================${SRC_NAME/${BASE_URL}/}
	SRC_NAME=${SRC_NAME/${BASE_URL}/}
	echo ============================${SRC_NAME}
	fi


	if [ ${ACTION} = "update" ] ; then
		echo "== update ${SRC_NAME}"
		echo ${SCMTOOL} ${ACTION} --revision ${SRC_REV} ../../${SRC_NAME}
		${SCMTOOL} ${ACTION} --revision ${SRC_REV} ../../${SRC_NAME}
		continue;
	fi

	if [ ${ACTION} = "up" ] ; then
		echo "== update ${SRC_NAME}"
		echo ${SCMTOOL} ${ACTION} --revision ${SRC_REV} ../../${SRC_NAME}
		${SCMTOOL} ${ACTION} --revision ${SRC_REV} ../../${SRC_NAME}
		continue;
	fi

	if [ ${ACTION} = "st" ] ; then
		echo "== status ${SRC_NAME}"
		echo ${SCMTOOL} ${ACTION} ../../${SRC_NAME}
		${SCMTOOL} ${ACTION} ../../${SRC_NAME}
		continue;
	fi

	if [ ${ACTION} = "status" ] ; then
		echo "== status ${SRC_NAME}"
		echo ${SCMTOOL} ${ACTION} ../../${SRC_NAME} | grep -v "^?"
		${SCMTOOL} ${ACTION} ../../${SRC_NAME} | grep -v "^?"
		continue;
	fi

	if [ ${ACTION} = "revert" ] ; then
		echo "== revert ${SRC_NAME} -R"
		${SCMTOOL} ${ACTION} ../../${SRC_NAME} -R
		continue;
	fi

	if [ ${ACTION} = "cleanup" ] ; then
		echo "== cleanup ${SRC_NAME}"
		${SCMTOOL} ${ACTION} ../../${SRC_NAME}
		continue;
	fi

	if [ ${ACTION} = "local_info" ] ; then					
		svn info ../../${SRC_NAME} | egrep "^${SVN_REVISION}" | \
			while read TEXT_REV LOCAL_REV; do
				echo -e ${SRC_NAME} \\t\\t${SRC_PATH} \\t\\t${LOCAL_REV}
			done
		continue;
	fi

	if [ ${ACTION} = "server_info" ] ; then
		svn info ${SRC_FULL_PATH} | egrep "^${SVN_REVISION}" | while read TEXT_REV SERVER_REV; do
		echo ${SRC_NAME}	${SRC_PATH}	${SERVER_REV}
		done
		continue;
	fi

	if [ ${ACTION} = "branch_info" ] ; then
		BRANCH_PATH=${SRC_NAME}
		sed -i "s^${SRC_PATH}^${BRANCH_PATH}/$5^g" $4
		continue;
	fi

	if [ ${ACTION} = "svn-clean" ] ; then
		echo "== svn-clean ${SRC_NAME}"
		echo ${SVNCLEAN} ../../${SRC_NAME}
		${SVNCLEAN} ../../${SRC_NAME}
		continue;
	fi

	if [ ${ACTION} = "export" ] ; then
		echo "== export ${SRC_NAME}"
		echo ${SCMTOOL} ${ACTION} ${SRC_FULL_PATH} ../../${SRC_NAME}
		${SCMTOOL} ${ACTION} ${SRC_FULL_PATH} ../../${SRC_NAME}
		continue;
	fi

	if [ ${ACTION} = "cp" ] ; then
		BRANCH_PATH=${SRC_NAME}
		${SCMTOOL} info ${BASE_URL}/${BRANCH_PATH}/$3 > /dev/null
		if [ $? -ne 0 ]; then
			echo -e "\n	  $Check Direcoty Success. Ignore avobe warning message.\n"
		else
			echo TRUE >> .error_occured_tag
			echo -e "\n	  ${RED}ERROR!!${BLACK} Please check error messages above.\n"
			continue;
		fi
		${SCMTOOL} ${ACTION} --parents ${BASE_URL}/${SRC_PATH}/@${SRC_REV} ${BASE_URL}/${BRANCH_PATH}/$3 -m "branch to ${BRANCH_PATH} from ${SRC_PATH}/@${SRC_REV}"
		# Check svn error 
		if [ $? -ne 0 ]; then
			echo TRUE >> .error_occured_tag
			echo -e "\n	  ${RED}ERROR!!${BLACK} Please check error messages above.\n"
		fi
		continue;
	fi

	case "${SRC_REV}" in
		HEAD)
			echo "== ${SRC_NAME} --revision ${SRC_REV}"
			if [ -n "$3" ]; then
				${SCMTOOL} ${ACTION} --username $3 ${SRC_FULL_PATH} ../../${SRC_NAME}
			else
				${SCMTOOL} ${ACTION} ${SRC_FULL_PATH} ../../${SRC_NAME}
			fi
			# Check svn error 
			if [ $? -ne 0 ]; then
				echo TRUE >> .error_occured_tag
				echo -e "\n   ${RED}ERROR!!${BLACK} Please check error messages above.\n"
			fi
			;;
		BS_REPO)
			echo "== ${SRC_NAME}"
            cp -a ${BS_PATH}/${SRC_PATH} ../../${SRC_NAME}
			;;
		WGET)
			;;
		'')
			;;
		*)
			echo "== ${SRC_NAME} --revision ${SRC_REV}"
			if [ -n "$3" ]; then
				${SCMTOOL} ${ACTION} --username $3 --revision ${SRC_REV} ${SRC_FULL_PATH} ../../${SRC_NAME}
			else
				${SCMTOOL} ${ACTION} --revision ${SRC_REV} ${SRC_FULL_PATH} ../../${SRC_NAME}
			fi
			;;
	esac
done < $1  

