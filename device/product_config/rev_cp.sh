#! /bin/sh
if [ $# -eq 0 ];
then
	echo "usage : rev_cp.sh project source_rev target_rev"
fi

if [ $# -eq 1 ];
then
	echo "usage : rev_cp.sh project source_rev target_rev"
fi

if [ $# -eq 2 ];
then
	echo "usage : rev_cp.sh project source_rev target_rev"
fi

if [ $# -eq 3 ];
then
	echo "copy revision config from $2 to $3 in $1"
	
	svn export	./defconfigs/$1/rev$2					./defconfigs/$1/rev$3
	svn export	./flashmap/$1/rev$2					./flashmap/$1/rev$3
	svn export	./platform/driver/$1/rev$2			./platform/driver/$1/rev$3
	svn export	./platform/sdk/driver/$1/rev$2		./platform/sdk/driver/$1/rev$3
	svn export	./platform/sdk/os/kernel/$1/rev$2		./platform/sdk/os/kernel/$1/rev$3
	
svn add		./defconfigs/$1/rev$3
svn add		./flashmap/$1/rev$3
svn add		./platform/driver/$1/rev$3
svn add		./platform/sdk/driver/$1/rev$3
svn add		./platform/sdk/os/kernel/$1/rev$3
fi

