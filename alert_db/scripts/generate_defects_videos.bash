#!/bin/bash
if [ $# -ne 1 ]; then
  echo "This script uses the alert_db generate jpg files over a set of bag files"
  echo "Usage: $0 <number of defects> "
  exit 1
fi

CONTADOR=0
until [ $CONTADOR -gt $1 ]; do
  rosrun alert_db generate_video.bash defect_${CONTADOR}_front_left_rgb_image_raw_compressed_
  rosrun alert_db generate_video.bash defect_${CONTADOR}_front_rgb_image_raw_compressed_
  rosrun alert_db generate_video.bash defect_${CONTADOR}_front_right_rgb_image_raw_compressed_
  rosrun alert_db generate_video.bash defect_${CONTADOR}_back_left_rgb_image_raw_compressed_
  rosrun alert_db generate_video.bash defect_${CONTADOR}_back_rgb_image_raw_compressed_
  rosrun alert_db generate_video.bash defect_${CONTADOR}_back_right_rgb_image_raw_compressed_
  let CONTADOR+=1
done