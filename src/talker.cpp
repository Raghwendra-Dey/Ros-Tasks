#include "ros/ros.h"
#include <cv_bridge/cv_bridge.h>
#include "tasks/board_pose.h"
#include "tasks/danger_region.h"
#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"
#include <bits/stdc++.h>

using namespace std;
using namespace cv;

int main()
{
	ros::init(argc, argv, "talker");
	ros::NodeHandle n;
	ros::Publisher chatter_pub = n.advertise<tasks::board_pose>("check_pose", 1000);
	ros::Rate loop_rate(10);
	tasks::board_pose msg;

	namedWindow("video",0);
	VideoCapture Video("new.avi");
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

		msg.x = (corners[22].x + corners[17].x)/2;
		msg.y = (corners[22].y + corners[17].y)/2;
		ROS_INFO("%d %d", msg.x,msg.y);
    	chatter_pub.publish(msg);

    	ros::ServiceClient client = n.serviceClient<tasks::danger_region.srv>("add_two_ints");
  		tasks::danger_region srv;

  		if ((abs(corners[22].x-500) < 100)&&(abs(corners[22].y-500) < 100))
		{
			srv.request.inp = true;
		  	ROS_INFO("sent request: input=%d", (int)request.inp);
		}
  		else
		{
			srv.request.inp = false;
		  	ROS_INFO("sent request: input=%d", (int)request.inp);
		}

  		if (client.call(srv))
  		{
    		ROS_INFO("Output: %s", srv.response.out);
 		}
  		else
  		{
    		ROS_ERROR("Failed to call service add_two_ints");
    		return 1;
  		}

    		ros::spinOnce();

    	loop_rate.sleep();
	}
}