#!/bin/bash
#
#

ABS_PATH=$(dirname $0)
ORI_PATH=`pwd`

test -z "$ABS_PATH" && ABS_PATH=.
cd $ABS_PATH ;

DIRECTORY_LIST=`ls -F | grep /`
for dir in $DIRECTORY_LIST ; do
	echo "Processing Android Sample \""$dir"\"";
	
	EXEC=$dir/build.sh ;
	
	if [ -f $EXEC ] ; then
	
		# executing shell inside directory
		$EXEC ;
	fi
done

cd $ORI_PATH ;

echo ;
