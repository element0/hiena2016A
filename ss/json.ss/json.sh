#!/bin/bash

# this takes a directory hierachy and outputs it as json
# what it doesn't do is output file contents

indent="";

function output_json() {
    COMMA=""	# improper commas can mess up simple validators such as 'json_pp'
    printf "{"
    for each in *;
    do
	printf "$COMMA\n$indent\"$each\":"
	if [ -d "$each" ]
	then
	    indent=$indent"    "
	    pushd $each &>/dev/null
	    output_json
	    popd &>/dev/null
	    indent=${indent%    }
	elif [ -f "$each" ]
	then
	    printf $(stat -c "%i" "$each")
	fi
	COMMA=","
    done
    printf "\n$indent}"
}

output_json
printf "\n"
