#!/bin/bash
#
#

ABS_PATH=$(dirname $0)
ORI_PATH=`pwd`

test -z "$ABS_PATH" && ABS_PATH=.
cd $ABS_PATH ;

source variables.sh

# remove logfile if exists
test -f $LOG_FILENAME && rm $LOG_FILENAME ;

DIRECTORY_LIST=`ls -F | grep /`
for dir in $DIRECTORY_LIST ; do
	echo "Processing directory \""$dir"\"";
	
	EXEC=$dir/build.sh ;
	
	if [ -f $EXEC ] ; then
	
		# executing shell inside directory
		$EXEC ;
	fi
done

#TODO: next section
#
#if [ -f list.md5 ] ; then
#	echo "Verifing hashes..." ;
#	md5sum -c list.md5 ;
#else
#	echo "Creating hashes..." ;
#	find ./production -type f -print0 | xargs -0 md5sum > list.md5
#fi

cd $ORI_PATH ;
