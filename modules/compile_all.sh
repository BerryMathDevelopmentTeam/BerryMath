#! /bin/bash

non_cxx=("bmlang")
dir_list=( )

for file in `ls`
do
# echo ${file}
if [ -d $file ];then
dir_list[${#dir_list[@]}]=${file}
fi
done

echo ${dir_list[*]}

for dir in ${dir_list[*]}
do
cd ${dir}
cmake .
make
cp lib${dir}.dylib /usr/local/BM/libraries/
cd ..
done

