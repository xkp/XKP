#!/bin/bash
#
# Declare here all variables that are called from another
# bash scripts

if [ ! -z $FLAG ] ; then
	return ;
fi

FLAG=1 ;
export FLAG=$FLAG ;

ABSOLUTE_PATH="$(cd "$(dirname "$BASH_SOURCE")"; pwd)"
test -z "$ABSOLUTE_PATH" && ABSOLUTE_PATH=.

ORIGIN_PATH=`pwd`
FILTER=$1

#for windows
XSS_FILENAME=$ABSOLUTE_PATH/../bin/Debug/xss.exe

#TODO: linux
#XSS_FILENAME=$ORIGIN_PATH/bin/cb/release/xss

LOG_FILENAME=$ABSOLUTE_PATH/production/result.log
#date +%c >> $LOG_FILENAME ;

# export variables
export ABSOLUTE_PATH=$ABSOLUTE_PATH ;
export ORIGIN_PATH=$ORIGIN_PATH ;
export XSS_FILENAME=$XSS_FILENAME ;
export XSS_FILTER=$FILTER ;
export LOG_FILENAME=$LOG_FILENAME ;
