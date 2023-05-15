#! /bin/sh
if [ $# -eq 0 ];
then
	echo "usage : rev_rm.sh target_project target_revision"
fi

if [ $# -eq 1 ];
then
	echo "usage : rev_rm.sh target_project target_revision"
fi

if [ $# -eq 2 ];
then
	svn rm --force 		./defconfigs/$1/rev$2
	svn rm --force		./flashmap/$1/rev$2
	svn rm --force		./platform/driver/$1/rev$2
	svn rm --force		./platform/sdk/driver/$1/rev$2
	svn rm --force		./platform/sdk/os/kernel/$1/rev$2

	rm -rf	./defconfigs/$1/rev$2
	rm -rf	./flashmap/$1/rev$2
	rm -rf	./platform/driver/$1/rev$2
	rm -rf	./platform/sdk/driver/$1/rev$2
	rm -rf	./platform/sdk/os/kernel/$1/rev$2
fi
