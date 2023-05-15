#! /bin/sh
if [ $# -eq 0 ];
then
	echo "usage : project_rm.sh target_project"
fi

if [ $# -eq 1 ];
then
	TAR_MODEL=`echo $1`
	
	echo $TAR_MODEL

	svn rm --force		./$TAR_MODEL
	
	rm -rf	./$TAR_MODEL
fi

