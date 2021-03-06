#!/bin/bash

ABS_PATH=$(dirname $0)
cd $ABS_PATH

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

# remove logfile if exists
test -f $LOG_FILENAME && rm $LOG_FILENAME ;

#date +%c >> $LOG_FILENAME ;

# execute all tests projects and put result in logfile
count=0 ;
echo Running tests... Wait please! ;
for proj in $PROJECTS_LIST ; do
	echo "Executing project: " $proj ;
	echo -e "\n===[" $proj "]================\n" >> $LOG_FILENAME ;
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
else
	echo "All tests executed OK!" ;
fi
	
#CURRENT_PATH = $(pwd) ;
#cd $ABS_PATH ;
cd $OUT_PATH ;

if [ -f list.md5 ] ; then
	echo "Verifing hashes..." ;
	md5sum -c list.md5 ;
else
	echo "Creating hashes..." ;
	find ./production -type f -print0 | xargs -0 md5sum > list.md5
fi

cd $CHP_PATH ;
#cd $CURRENT_PATH
