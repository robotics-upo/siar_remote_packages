#!/bin/bash
if [ $# -ne 2 ]; then
  echo "This script uses the alert_db generate jpg files over a set of bag files"
  echo "Usage: $0 <folder> <alert_times file> "
  exit 1
fi

for filename in $1/*.bag
do
  echo Filename $filename
  rosrun alert_db get_images_from_bag $filename $2
done;

