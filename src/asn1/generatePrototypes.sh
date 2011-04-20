#!/bin/bash

structs=`cat $1 | grep " struct" | cut -d" " -f3`

for line in $structs
do
	echo void decode_${line,,} \(unsigned char *buffer, $line *pointer \)\;
done
