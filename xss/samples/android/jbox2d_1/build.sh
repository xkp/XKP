#!/bin/bash
#
#

ABS_PATH=$(dirname $0)
ORI_PATH=`pwd`
PARAM1=${1}
CONSOLE="--console"

test -z "$ABS_PATH" && ABS_PATH=.
cd $ABS_PATH ;

source ../../../variables.sh

PROJECTS_LIST=`ls | grep "\.project.xml"`
for proj in $PROJECTS_LIST ; do

	#TODO: filter for execute only one project
	#if [ $XSS_FILTER -nt null && $proj -is $XSS_FILTER] ; then
	#	continue ;
	#fi
	
	echo "Executing project: " $proj ;
	
	if [ $PARAM1 == $CONSOLE ] ; then
		echo -e "\n===[" $proj "]================\n" ;
		$XSS_FILENAME $proj ;
	else
		echo -e "\n===[" $proj "]================\n" >> $LOG_FILENAME ;
		$XSS_FILENAME $proj >> $LOG_FILENAME ;
	fi
	
	result=$? ;
	
	if [ "$result" -gt 0 ] ; then
		echo "ERROR!!" $proj ;
		echo "ERROR: Project: " $proj >> $LOG_FILENAME ;
	fi
done

cd $ORI_PATH ;

echo ;
