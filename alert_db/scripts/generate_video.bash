#!/bin/bash
if [ $# -ne 1 ]; then
  echo "This script uses the alert_db generate jpg files over a set of bag files"
  echo "Usage: $0 <base_name> "
  exit 1
fi

ffmpeg -y -framerate 10 -i $1%04d.jpg -c:v libx264 -profile:v high -crf 20 -pix_fmt yuv420p $1.mp4
