#pragma once


class CvFindCenter
{

public:
	void FindCenter();
	IplImage* img;
	IplImage* gray;
	CvMemStorage* storage;
	CvSeq* circles;
	int x_center;
	int y_center;


};
