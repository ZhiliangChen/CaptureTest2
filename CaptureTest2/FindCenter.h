#pragma once


class CvFindCenter
{

public:
	void FindCenter();
	IplImage* img;
	IplImage* gray;
	CvMemStorage* storage;
	CvSeq* circles;
	double x_center;
	double y_center;


};
