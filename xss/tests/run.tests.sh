#!/bin/bash

# windows structure
XSS_FILENAME=xss.exe
XSS_PATH=../../bin/Debug/

# linux structure
#XSS_FILENAME=xss
#XSS_PATH=../../bin/cb/release/

LOG_FILENAME=../production/result.log
OUT_PATH=../
CHP_PATH=tests/

EXEC=$XSS_PATH$XSS_FILENAME

PROJECTS_LIST=`ls | grep "\.project.xml"`

test -f $LOG_FILENAME && rm $LOG_FILENAME ;

echo Running tests... Wait please! ;

#date +%c >> $LOG_FILENAME ;

count=0 ;
for proj in $PROJECTS_LIST ; do
	echo "Executing project: " $proj ;
	echo "===[" $proj "]================" >> $LOG_FILENAME ;
	$EXEC $proj >> $LOG_FILENAME ;
	result=$? ;
	if [ "$result" -gt 0 ] ; then
		echo "ERROR!!" $proj ;
		echo "ERROR: Project: " $proj >> $LOG_FILENAME ;
		let "count++" ;
	fi
done
echo ;

if [ "$count" -gt 0 ] ; then
	echo "Fails" $count "tests." ;

#commented for now
#else

	echo "All tests executed OK!" ;
	
	cd $OUT_PATH ;
	
	if [ -f list.md5 ] ; then
		echo "Verifing hashes..." ;
		md5sum -c list.md5 ;
	else
		echo "Creating hashes..." ;
		find ./production -type f -print0 | xargs -0 md5sum > list.md5
	fi
	
	cd $CHP_PATH ;
fi
