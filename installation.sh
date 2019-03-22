#!/bin/bash

if [ ! -f "BerryMath" ];then
	echo "Cannot open realese file: BerryMath"
	echo "'cmake . & make' to realese"
else
	echo "Copying release file..."
	cp BerryMath /usr/local/bin
fi
if [ ! -f "libs/libjson.dylib" ];then
	echo "Cannot open josncpp lib file: libs/libjson.dylib"
	echo "Check that the folder is complete"
else
	if [ ! -f "/usr/local/lib/libjsoncpp.dylib" ];then
		echo "Copying dylib file..."
		cp libs/libjson.dylib /usr/local/lib/libjsoncpp.dylib
	else
		echo "The dylib file already had"
	fi
fi

dirs_arr=("include" "src" "system_files")

if [ ! -f "/usr/local/BerryMath" ];then
	echo "Creating system folder..."
	mkdir /usr/local/BerryMath
fi

echo "Copying system files and source files..."
for d in ${dirs_arr[@]}; do
	echo $d
	cp -r $d /usr/local/BerryMath/$d
	# echo each: $d
	# for file in ./$d/*
	# do
	# if [ -f $file ];then
		echo $file
		# cp $file 
	# fi
	# done
done
