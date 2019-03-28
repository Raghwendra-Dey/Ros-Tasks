#include "ros/ros.h"
#include "tasks/board_pose.h"
#include "tasks/danger_region.h"
#include <bits/stdc++.h>

using namespace std;

void chatterCallback(const tasks::board_pose::ConstPtr& msg)
{
  ROS_INFO("I heard: [%d,%d]", msg->x,msg->y);
}

bool danger(tasks::danger_region::Request  &req,
         tasks::danger_region::Response &res)
{
	stringstream ss;
	if(req.inp)
		ss << "Inside!!";
	else
		ss << "outside!!";
	res.out = ss.str();
	ROS_INFO("request: input=%d", (int)req.inp);
	ROS_INFO("sending back response: %s", res.out);
	return true;
}

int main(int argc,char **argv,)
{
	ros::init(argc, argv, "listener");

	ros::NodeHandle n;

	ros::Subscriber sub = n.subscribe("check_pose", 1000, chatterCallback);

	ros::ServiceServer service = n.advertiseService("danger_region", danger);

	ros::spin();
}