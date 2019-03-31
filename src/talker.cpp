#include "ros/ros.h"
#include "tasks/board_pose.h"
#include "tasks/danger_region.h"
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include <bits/stdc++.h>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{	
	ros::init(argc, argv, "talker");
	ros::NodeHandle n;
	ros::Publisher chatter_pub = n.advertise<tasks::board_pose>("check_pose", 1000);
	ros::Rate loop_rate(10);
	tasks::board_pose msg;

	namedWindow("video",0);
	VideoCapture Video("/home/raghwendra/Desktop/ARK-Tasks/Ros_tasks/new.avi");
	Mat frame,gray;
	
	while(ros::ok())
	{
		Video >> frame;
		if(frame.empty())
			break;
		cvtColor(frame, gray, COLOR_BGR2GRAY);
		vector<Point2f> corners;
		Size board_size = Size(5,8);
		bool patternfound = findChessboardCorners( frame, board_size, corners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE+ CALIB_CB_FAST_CHECK);
		if(patternfound)
			cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		else
			break;
		drawChessboardCorners(frame, board_size, Mat(corners), patternfound);
		imshow("video",frame);
		waitKey(10);

		msg.x = (corners[22].x + corners[17].x)/2;
		msg.y = (corners[22].y + corners[17].y)/2;
		ROS_INFO("%d %d", msg.x,msg.y);
		chatter_pub.publish(msg);

		ros::ServiceClient client = n.serviceClient<tasks::danger_region>("danger_region");
		tasks::danger_region srv;

		if ((abs(msg.x-500) <= 200)&&(abs(msg.y-500) <= 200))
		{
			srv.request.inp = true;
			ROS_INFO("sent request: input=%ld", (int)srv.request.inp);
		}
		else
		{
			srv.request.inp = false;
			ROS_INFO("sent request: input=%ld", (int)srv.request.inp);
		}

		if (client.call(srv))
		{
			ROS_INFO("Output: %s", srv.response.out.c_str());
		}
		else
		{
			ROS_ERROR("Failed to call service danger_region");
			return 1;
		}

			ros::spinOnce();

		loop_rate.sleep();
	}
}

