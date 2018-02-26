#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main()
{
	VideoCapture cap(0); // open the default camera
	if(!cap.isOpened())  // check if we succeeded
        {
		cout << "Error in using VideoCapture function." << endl;	
		return -1;
	}
	namedWindow("Original",CV_WINDOW_AUTOSIZE);
	namedWindow("Drawing",CV_WINDOW_AUTOSIZE);

	Point2i initial, final_;
	int low_r = 0, low_b = 54, low_g = 0, high_r = 67, high_b = 173, high_g = 59;	//Initializes the track-bar values 	
	int count = 0, flag = 0;
	
	createTrackbar("Low_Red", "Original", &low_r, 255);			// Create Track-Bar
	createTrackbar("Low_Blue", "Original", &low_b, 255);		
	createTrackbar("Low_Green", "Original", &low_g, 255);
	createTrackbar("High_Red", "Original", &high_r, 255);
	createTrackbar("High_Blue", "Original", &high_b, 255);
	createTrackbar("High_Green", "Original", &high_g, 255);
	
	int R = 0, G = 0, B = 0, thick = 3;
	createTrackbar("RED", "Drawing", &R, 255);			// Create Track-Bar
	createTrackbar("BLUE", "Drawing", &B, 255);		
	createTrackbar("GREEN", "Drawing", &G, 255);
	createTrackbar("Thickness", "Drawing", &thick, 10);
	Mat canvas = Mat::zeros(480, 640, CV_8UC3);
	while(1)
    	{
		Mat frame;
		count++;
		bool cap_status = cap.read(frame); // get a new frame from camera
		if (cap_status == 0)			//If status is false returns 1 to console
		{
			cout << "Error in capturing Frames" << endl;			
			return 1;
		}
			
		setTrackbarPos("Low_Red","Original", low_r);			// Set the values of Trackbar in runtime to threshold.
		setTrackbarPos("Low_Blue","Original", low_b);
		setTrackbarPos("Low_Green","Original", low_g);
		setTrackbarPos("High_Red","Original", high_r);
		setTrackbarPos("High_Blue","Original", high_b);
		setTrackbarPos("High_Green","Original", high_g);	
//<--------------------------------------------------------------------------------------------------------------------------->//
		Mat thresh;			
		inRange(frame,Scalar(low_b,low_g,low_r), Scalar(high_b,high_g,high_r),thresh);	//Threshold using inRange Function
	
		Mat element = getStructuringElement( MORPH_ELLIPSE, Size(5, 5));		//TO remove holes and other noises in thresholded-image
		dilate(thresh, thresh, element);
		erode(thresh, thresh, element);
		erode(thresh, thresh, element);	
		dilate(thresh, thresh, element);                                       
		
		
		imshow("Thresholding", thresh);		//Shows the thresholded frames
//<----------------------------------------------------------------------------------------------------------------------------------->//

		setTrackbarPos("RED","Drawing", R);			// Set the values of Trackbar in runtime to threshold.
		setTrackbarPos("BLUE","Drawing", B);
		setTrackbarPos("GREEN","Drawing", G);
		
		Mat thresh_c = thresh.clone(); 		//Create a clone of thresholded image-thresh
		
		vector<vector<Point> > contours;	
		vector<Vec4i> hierarchy;

		findContours(thresh_c, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);		//Find contours and store it in contours
		Mat img(frame.size(), CV_8UC3, Scalar(0, 0, 0));						//Single channel black image is created
		
		// Filtering of Contours
				
		
		double area = -1;
		int index = -1;
		for (int idx = 0; idx < contours.size(); idx++) 					
        	{
			if (contourArea(contours[idx]) > 600)							// To remove noises sets threshold value as 5000
			{
				if (area < contourArea(contours[idx]))
				{	
					area = contourArea(contours[idx]);
					index = idx;
				}
			}
		}
		cout << (int)index << endl;
		if (index >= 0 )
		{
			
			drawContours(frame, contours, index, Scalar(255,0,0), CV_FILLED, 8);			
			Moments M = moments(contours[index]); 	
			int cx = int(M.m10 / M.m00);
    			int cy = int(M.m01 / M.m00);
			circle(frame, Point(cx, cy), 3, Scalar(0, 255, 0), CV_FILLED, 8);
			if (count == 1 || flag == 1)
			{
				initial = Point(cx, cy);
				final_ = Point(cx, cy);
				flag = 0;
			}
			else
			{
				initial = final_;
				final_ = Point(cx, cy);
			}
			line(canvas, initial, final_, Scalar(B, G, R), thick, 8, 0);		
			//count = count % 10000;
		}
		else if (index == -1)
		{
			flag = 1;
		}
		imshow("Drawing", canvas);
		imshow("Original", frame);
		if (waitKey(1) == 27)
			break;
	}
	return 0;
}
