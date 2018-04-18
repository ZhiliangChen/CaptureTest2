#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <iostream> 
#include "FindCenter.h"

void CvFindCenter::FindCenter()
{
	
	img = cvLoadImage("RawToRGBData.bmp", 1);
	gray = cvCreateImage(cvGetSize(img), 8, 1);
	storage = cvCreateMemStorage(0);
	cvCvtColor(img, gray, CV_BGR2GRAY);
	cvSmooth(gray, gray, CV_GAUSSIAN, 9, 9); // smooth it, otherwise a lot of false circles may be detected
	/*cvHoughCircles(CvArr* image, void* circle_storage,int method, double dp, double min_dist,double param1 CV_DEFAULT(100),
		double param2 CV_DEFAULT(100),int min_radius CV_DEFAULT(0),int max_radius CV_DEFAULT(0));*/
	//检测接箍内圆，最大最小半径设置为325可以检测到；累加器阈值50会导致程序反应过慢，改成30有所缓解
	circles = cvHoughCircles(gray, storage, CV_HOUGH_GRADIENT, 1, gray->height / 4, 200, 30, 324, 326);
	//cv::seqToMat(seq, _circles);
	int i;
	for (i = 0; i < circles->total; i++)
	{
		float* p = (float*)cvGetSeqElem(circles, i);
		cvCircle(img, cvPoint(cvRound(p[0]), cvRound(p[1])), 3, CV_RGB(0, 255, 0), -1, 8, 0);
		cvCircle(img, cvPoint(cvRound(p[0]), cvRound(p[1])), cvRound(p[2]), CV_RGB(255, 0, 0), 3, 8, 0);
		x_center = cvRound(p[0]);
		y_center = cvRound(p[1]);
		//如何输出小数位？？？
		//x_center = p[0];
		//y_center = p[1];
	}
	//cout << "圆数量=" << circles->total << endl;
	cvNamedWindow("circles", 1);
	cvShowImage("circles", img);
	//cvWaitKey(1000);


}
