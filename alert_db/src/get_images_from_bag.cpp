#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CompressedImage.h>
#include <sensor_msgs/image_encodings.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv/cvwimage.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <cv.h>
#include <boost/foreach.hpp>
#ifndef foreach
#define foreach BOOST_FOREACH
#endif
// #include <compressed_image_transport/codec.h>
#include <compressed_depth_image_transport/codec.h>
#include <compressed_depth_image_transport/compression_common.h>
#include "ros/ros.h"
#include <limits>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>      // std::setfill, std::setwidth

#include <functions/functions.h>

using namespace std;
namespace enc = sensor_msgs::image_encodings;
using namespace cv;

double max_range = 10.0;

int main(int argc, char **argv) 
{
  
  rosbag::Bag bag;
  std::string camera("/up");
  if (argc < 3) {
    cerr << "Usage: " << argv[0] << " <bag file> <input_file> [<skip first n images = 0>] \n";
    return -1;
  }
  int ignore = -1;
  if (argc > 3) {
    ignore = atoi(argv[3]);
    cout << "Ignoring ids with less than: " << ignore << endl;
  }
  std::string filename(argv[2]);
  std::string bag_file(argv[1]);
  
  std::vector<std::vector <double> > M;
  functions::getMatrixFromFile(filename, M);
  
  map<string, int> count_map;
  
  try {
    bag.open(string(argv[1]), rosbag::bagmode::Read);

    std::vector<std::string> topics;
    topics.push_back("/front/rgb/image_raw/compressed");
    topics.push_back("/front_right/rgb/image_raw/compressed");
    topics.push_back("/front_left/rgb/image_raw/compressed");
    topics.push_back("/back/rgb/image_raw/compressed");
    topics.push_back("/back_right/rgb/image_raw/compressed");
    topics.push_back("/back_left/rgb/image_raw/compressed");
    

    rosbag::View view(bag, rosbag::TopicQuery(topics));
    bool initialized = false;
    bool _positive = false;
    bool ignoring = true;
    unsigned int last_defect = 0;
    unsigned long curr_msg = 0;
    unsigned long view_size = view.size();
    foreach(rosbag::MessageInstance const m, view)
    {
      sensor_msgs::CompressedImage::Ptr im = m.instantiate<sensor_msgs::CompressedImage>();
      
      ROS_INFO("%d%%", (int) ((float)curr_msg/view_size*100.0));
      printf("\033[1A"); // Move up X lines;
      curr_msg++;
      
      if (im != NULL) {
	ros::Time t = im->header.stamp;
	
	
	for (unsigned int i = last_defect; i < M.size(); i++) {
	  ros::Time t1(M[i][0], M[i][1]);
	  ros::Time t2(M[i][2], M[i][3]);  
	  
// 	  if (t1 < t) {
// 	    ROS_INFO("T1 < T");
// 	    
// 	  }
// 	  if (t < t2) {
// 	    ROS_INFO("T < T2");
// 	  }
// 	  if (t1.sec < t2.sec) {
// 	    ROS_INFO("T1 < T2");
// 	  }
	  
	  if (t1 <= t && t2 >= t) {
	    ROS_INFO("Time: %f", t.toSec());
	    if (i != last_defect) {
	      count_map.clear();
	      last_defect = i;
	      ROS_INFO("Detected new defect. Number: %u", last_defect);
	    }
	    
	    if ( count_map.find(m.getTopic()) == count_map.end() ) {
	      count_map[m.getTopic()] = 1;
	    } else {
	      count_map[m.getTopic()]++;
	    }
	    ostringstream filename_;
// 	    filename_ 
	    filename_ << "defect_" << i << m.getTopic() << "_" << std::setfill ('0') << std::setw (4) << count_map[m.getTopic()] <<".jpg";
	    
	    string s = filename_.str();
	    replace(s.begin(), s.end(), '/', '_');
	    
	    
	    Mat decompressed;
            try
            {
              // Decode image data
              decompressed = cv::imdecode(im->data, CV_LOAD_IMAGE_UNCHANGED);
	      
	      if (m.getTopic() == topics[2] || m.getTopic() == topics[4]) {
		// Front left and Back right should be flip
		flip(decompressed, decompressed, -1);
	      }
	      if (imwrite (s, decompressed)) {
		ROS_INFO("Exported file: %s", s.c_str());
	      } else {
		ROS_ERROR("Could not export file: %s", s.c_str());
	      }
            } catch (exception &e) {
              
	    }
	    
	  }
	}
      }
    }

    bag.close();
  } catch (exception &e) {
    cerr << "Exception while manipulating the bag  " << argv[1] << endl;
    cerr << "Content " << e.what() << endl;
    return -2;
  }
  printf("\n");
  
  return 0;
}

