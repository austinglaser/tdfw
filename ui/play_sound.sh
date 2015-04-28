#!/bin/bash

if [ $# -eq 1 ]
then
    if [ -f $1 ]
    then
        cvlc $1 --play-and-exit 2> /dev/null > /dev/null &
    else
        echo "$1 doesn't exist"
    fi    
else
    echo "usage: $0 <wav_file>"
fi
