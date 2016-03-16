#!/bin/bash

OIFS="$IFS"
IFS=""

while read line
do
	[ -z "$indentsav" ] && indentsav=`expr "$line" : '\(^[ ]*\)'`
	indentcur=`expr "$line" : '\(^[ ]*\)'`
	(( "${#indentsav}" > "${#indentcur}" )) && break
	echo "${line#$indentsav}"
done

