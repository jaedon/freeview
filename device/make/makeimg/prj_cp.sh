#! /bin/sh
if [ $# -eq 0 ];
then
	echo "usage : project_cp.sh source_project target_project"
fi

if [ $# -eq 1 ];
then
	echo "usage : project_cp.sh source_project target_project"
fi

if [ $# -eq 2 ];
then
	echo "copy project config from $1 to $2"
	SRC_MODEL=`echo $1`
	TAR_MODEL=`echo $2`

	echo $SRC_MODEL
	echo $TAR_MODEL
	
	svn export	./$SRC_MODEL		./$TAR_MODEL

	svn add		./$TAR_MODEL
fi

