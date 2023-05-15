#!/bin/bash

if [ $0 != "./netflix_manuss.sh" ]
then
    echo "You must run this script in the same directory as the netflix executable."
    exit
fi

idfile=""
args=("$@")

for ((i=0; i<$#; i++))
do
    if [ ${args[$i]} == "-I" ]
    then
        idfile=${args[$i+1]}
        break
    fi 
done 

if [ "$idfile" == "" ]
then
    if [[ -n $NF_IDFILE ]]
    then
        idfile=$NF_IDFILE
    fi
fi

if [[ -n $NF_DATA_DIR ]]
then
    nfDataPlayready=$NF_DATA_DIR"/dpi/playready"
else
    nfDataPlayready="./data/dpi/playready"
fi 

./manufss ./ $idfile $nfDataPlayready
./netflix  $@ -I ./manuf_ss.bin
