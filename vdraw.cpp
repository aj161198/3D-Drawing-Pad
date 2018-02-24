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
	namedWindow("Drawing",CV_WINDOW_AUTOSIZE);
	
	int low_r = 0, low_b = 168, low_g = 0, high_r = 255, high_b = 255, high_g = 152;	//Initializes the track-bar values 	
	
	/*
	createTrackbar("Low_Red", "Original", &low_r, 255);			// Create Track-Bar
	createTrackbar("Low_Blue", "Original", &low_b, 255);		
	createTrackbar("Low_Green", "Original", &low_g, 255);
	createTrackbar("High_Red", "Original", &high_r, 255);
	createTrackbar("High_Blue", "Original", &high_b, 255);
	createTrackbar("High_Green", "Original", &high_g, 255);*/
	
	int R = 0, G = 0, B = 0, thick = 3;
	createTrackbar("RED", "Drawing", &R, 255);			// Create Track-Bar
	createTrackbar("BLUE", "Drawing", &B, 255);		
	createTrackbar("GREEN", "Drawing", &G, 255);
	createTrackbar("Thickness", "Drawing", &thick, 10);

	Mat canvas = Mat::zeros(480, 640, CV_8UC3);
	while(1)
    	{
		Mat frame;
		bool cap_status = cap.read(frame); // get a new frame from camera
		if (cap_status == 0)			//If status is false returns 1 to console
		{
			cout << "Error in capturing Frames" << endl;			
			return 1;
		}

		
			
		/*setTrackbarPos("Low_Red","Original", low_r);			// Set the values of Trackbar in runtime to threshold.
		setTrackbarPos("Low_Blue","Original", low_b);
		setTrackbarPos("Low_Green","Original", low_g);
		setTrackbarPos("High_Red","Original", high_r);
		setTrackbarPos("High_Blue","Original", high_b);
		setTrackbarPos("High_Green","Original", high_g);*/	
//<--------------------------------------------------------------------------------------------------------------------------->//
		Mat thresh;			
		inRange(frame,Scalar(low_b,low_g,low_r), Scalar(high_b,high_g,high_r),thresh);	//Threshold using inRange Function
	
		Mat element = getStructuringElement( MORPH_ELLIPSE, Size(5, 5));		//TO remove holes and other noises in thresholded-image
		dilate(thresh, thresh, element);
		erode(thresh, thresh, element);
		erode(thresh, thresh, element);	
		dilate(thresh, thresh, element);                                       
		
		
		//imshow("Thresholding", thresh);		//Shows the thresholded frames
//<----------------------------------------------------------------------------------------------------------------------------------->//

		setTrackbarPos("RED","Drawing", R);			// Set the values of Trackbar in runtime to threshold.
		setTrackbarPos("BLUE","Drawing", B);
		setTrackbarPos("GREEN","Drawing", G);
		
		Mat thresh_c = thresh.clone(); 		//Create a clone of thresholded image-thresh
		
		vector<vector<Point> > contours;	
		vector<Vec4i> hierarchy;

		findContours(thresh_c, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);		//Find contours and store it in contours
		Mat img(frame.size(), CV_8UC3, Scalar(0, 0, 0));						//Single channel black image is created
		
		// Filtering of Contours
				
		Point2f corners[4];
		
		double area = -1;
		size_t index = -1;
		for (size_t idx = 0; idx < contours.size(); idx++) 					//Locating all the corners for every contour and drawing the rectangles above some value
        	{
			if (contourArea(contours[idx]) > 2000)							// To remove noises sets threshold value as 5000
			{
				double epsilon = arcLength(contours[idx], true) * 0.1; 				// calculate error value based on arclength			
				approxPolyDP(contours[idx], contours[idx], epsilon, true);			// approximates any contour to a polygon				
				if (contours[idx].size() == 4) 							// If polygon has 4 sides
				{
					if (area < contourArea(contours[idx]))
					{	
						area = contourArea(contours[idx]);
						index = idx;
					}
				}
			}
		}

		if (index >= 0 && area > 2000)
		{
			drawContours(frame, contours, index, Scalar(255, 0, 255), 1, 8, hierarchy);		// Draws the contours
			corners[0] = contours[index][0];						// And Stores all the corner values
			corners[1] = contours[index][1];
			corners[2] = contours[index][2];
			corners[3] = contours[index][3];		
		}
		
		int cx = int((corners[0].x + corners[1].x + corners[2].x + corners[3].x) / 4);
    		int cy = int((corners[0].y + corners[1].y + corners[2].y + corners[3].y) / 4);	 
		
		Vec3b color;
		color[0] = B;
		color[1] = G;
		color[2] = R;
		
		for (int i = 0; i < thick; i++)
		{
			for (int j = 0; j < thick; j++)
			{
				canvas.at<Vec3b>(Point(cx + j, cy + i)) = color;
			}
		}
	
		//cout << color << endl;
		imshow("Drawing", canvas);
		imshow("Original", frame);
		if (waitKey(1) == 27)
			break;
	}
	return 0;
}
