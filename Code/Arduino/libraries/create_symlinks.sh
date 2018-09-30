#!/bin/bash 
function createIfNotExists {
	source="$(pwd)/$1"
	target=$2/$1
	if [[ -d "$source" ]] ; then
		if [[ -d "$target" || -h "$target" ]] ; then
        		echo "exists: $target"
		else
        		echo "create: $target"
			ln -s "$source" "$target"
		fi
	else
		echo "missing: $source"
	fi
}  
# -h: symbolic link 

if [[ "$1" == "" ]] ; then
	target_path=~/Documents/Arduino/libraries
	echo "No target path specified, fallback to standard: $target_path"
else
	target_path=$1
fi

if [[ -d "$target_path" ]] ; then
	createIfNotExists FlipTheDot_FP2800a "$target_path"
	createIfNotExists FlipTheDot_ColumnRowController "$target_path"
	createIfNotExists FlipTheDot_Matrix "$target_path"
	createIfNotExists FlipTheDot_Lawo28x24 "$target_path"
else
	echo "Target path doesn't exists: $target_path"
fi
