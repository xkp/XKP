#!/bin/bash 

LOG_FILENAME=result.log
XSS_FILENAME=xss.exe
MD5_CREATE=`find ./ -type f -print0 | xargs -0 md5sum`
MD5_VERIFY=md5sum -c ./ list.md5
XSS_PATH=../../bin/Debug/
OUT_PATH=../production/
CHP_PATH=../tests/
EXEC=$XSS_PATH$XSS_FILENAME
#PROJECTS_LIST=`ls | grep "\.project.xml"`

echo Running tests... Wait please! ;

cc=0 ;
for i in 1 2 3 4 5 ; do
	cc=`expr $cc + 1` ;
done
echo $cc ;

count=0 ;
date +%c >> $LOG_FILENAME ;
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
else
	echo "All tests executed OK!" ;
	echo "Verifing hashes..." ;
	cd $OUT_PATH ;
	$MD5_VERIFY ;
	cd $CHP_PATH ;
fi
