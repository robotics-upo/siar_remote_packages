/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2014  sinosuke <email>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "Comms.h"

#include <fstream>
#include "functions/functions.h"
#include <std_msgs/Bool.h>
#include <std_msgs/Float32.h>
#include <siar_driver/SiarLightCommand.h>
#include <rssi_get/Nvip_status.h>

using namespace functions;
using namespace std;

Comms::Comms(int argc, char** argv):spinner(NULL),emergency(false),slow(false),automatic(0)
{
  ros::init(argc,argv,"comms");
  
  ros::NodeHandle nh;
  ros::NodeHandle pnh("~");
  
  // Subscribers:
  status_sub = nh.subscribe<const siar_driver::SiarStatus::ConstPtr&>("siar_status", 2, &Comms::siarStatusCallback, this);
  rssi_sub = nh.subscribe<const rssi_get::Nvip_status::ConstPtr&>("/rssi_nvip_2400", 2, &Comms::nvipCallback, this);
  text_sub = nh.subscribe<const std_msgs::String::ConstPtr&>("/alert_text", 2, &Comms::alertTextCallback, this);
  
  // Publisher
  emergency_pub = nh.advertise<std_msgs::Bool>("/emergency_stop",2);
  elec_x_pub = nh.advertise<std_msgs::Float32>("/width_pos",2);
  
  
  // Get parameters
  if (pnh.hasParam("n_inspection_cams")) {
    pnh.getParam("n_inspection_cams", n_inspection_cams);
  } else {
    n_inspection_cams = 1; // One or two
  }
  if (pnh.hasParam("thermal_cam")) {
    pnh.getParam("thermal_cam", thermal_cam);
  } else {
    thermal_cam = false; // One or two
  }
  
}
 

void Comms::setEmergencyStop()
{
  emergency = !emergency;
  std_msgs::Bool msg;
  msg.data = emergency;
  
  emergency_pub.publish(msg);
}

void Comms::setElecX(int new_x)
{
  std_msgs::Float32 msg;
  msg.data = new_x/14.0;
  
  elec_x_pub.publish(msg);
}


Comms::~Comms()
{
  shutdownComms();
//   clearLogs();
}

void Comms::siarStatusCallback(const siar_driver::SiarStatus::ConstPtr& s)
{
  emit siarStatusChanged(*s);
}

void Comms::nvipCallback(const rssi_get::Nvip_status::ConstPtr& msg)
{
  emit newRSSI(*msg);
}

void Comms::alertTextCallback(const std_msgs::String::ConstPtr& msg)
{
  emit alertDBReceived(msg->data);
}

void Comms::armModeCallback(const std_msgs::Bool::ConstPtr &msg) {
  
  emit armModeReceived(msg->data);
}

void Comms::armTorqueCallback(const std_msgs::UInt8::ConstPtr &msg) {
  emit armTorqueReceived(msg->data);
}

void Comms::startComms()
{
  spinner = new ros::AsyncSpinner(2);
  spinner->start();
  
  init_log_time.getTime();
  
  ros::NodeHandle n;
}

void Comms::shutdownComms()
{
  delete spinner;
  spinner = NULL;
}

// void Comms::siarStatusChanged(const siar_driver::SiarStatus new_status)
// {
// 
// }
