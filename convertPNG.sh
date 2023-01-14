#!/bin/bash

for f in TPAGE0*.png
do
# FAILSAFE #
# Check if "$f" FILE exists and is a regular file and then only copy it #
  if [ -f "$f" ]
  then
    echo "Processing $f file..."
    ls "$f" 
	convert $f -depth 4 $f.4.png
#	identify -verbose $f.temp.bmp
#	read

#	convert -verbose $f.temp.bmp -depth 4 -define bmp:subtype=ARGB4444 $f.bmp


	identify $f.4.png	
#	identify -verbose $f.4.png
	read
  fi
done

#convert -verbose TPAGE01.bmp -depth 4 -define bmp:subtype=ARGB4444 BMP:TPAGE01ARGB4444.bmpfor f in file1 file2 file3 file5

