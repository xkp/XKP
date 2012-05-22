#!/bin/bash
#
#

ABS_PATH=$(dirname $0)
ORI_PATH=`pwd`

test -z "$ABS_PATH" && ABS_PATH=.
cd $ABS_PATH ;

source ../variables.sh

count=0 ;
echo Running tests... Please wait! ;

PROJECTS_LIST=`ls | grep "\.project.xml"`
for proj in $PROJECTS_LIST ; do
	
	echo "Executing project: " $proj ;
	echo -e "\n===[" $proj "]================(Tests)\n" >> $LOG_FILENAME ;
	
	$XSS_FILENAME $proj >> $LOG_FILENAME ;
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

cd $ORI_PATH ;

echo ;
