#include <cvaux.h>
#include <highgui.h>
#include <iostream>
#include <vector>

#include "cvfast.h"
#include "match.h"
#include "corner.h"
#include "drawCorner.h"

const float KSCALE = 0.125*2;//change
const int KFASTTHRESHHOLD = 180;//范围[0-250]
const int KNPIXELS = 11;
const char fileOne[] = "../imgs/YY100.png";
const char fileTwo[] = "../imgs/YY101.png";

IplImage *scaleImage(IplImage* image, float scale = KSCALE)
{
	//calTime cal(std::string(__FUNCTION__));
	CvSize size = cvGetSize( image );
	size.width = (int)(size.width * scale);
	size.height = (int)(size.height * scale);
	IplImage *imageAfterScale= cvCreateImage(size, image->depth, image->nChannels);
	cvResize(image, imageAfterScale, CV_INTER_LINEAR);
	return imageAfterScale;
}

void showImage(IplImage* image)
{
	cvNamedWindow("FAST", CV_WINDOW_AUTOSIZE); 
	cvShowImage("FAST", image);
}

void geneCorners(CvPoint* cornersPos, int numCorners, Corners& corners)
{
	calTime cal(std::string(__FUNCTION__));
	for (int i =0; i < numCorners; ++i)
	{
		cornersPos[i].x *= (int)(1.0/KSCALE);
		cornersPos[i].y *= (int)(1.0/KSCALE);
		corners.pushCorner(cornersPos[i]);
	}
}

void processImage(IplImage* I_inOrigin,Corners& corners)
{
	int inNpixels = 11;
	int inNonMaxSuppression  = 0;
	CvPoint* cornersPos;
	 int numCorners;

	IplImage *I_inScale = scaleImage(I_inOrigin);

	IplImage* IGray = cvCreateImage( cvGetSize( I_inScale ), IPL_DEPTH_8U, 1);
	cvCvtColor( I_inScale, IGray, CV_RGB2GRAY );

	cvCornerFast(IGray, KFASTTHRESHHOLD, KNPIXELS, inNonMaxSuppression, &numCorners, &cornersPos);
	std::cout << "------->number of corners found " << " are " << numCorners << "\n\n";

	geneCorners(cornersPos, numCorners, corners);

	cvReleaseImage(&IGray);
	cvReleaseImage(&I_inScale);
}

void mergeImage(IplImage* image1, IplImage* image2)
{
	cv::Mat leftImage(image1);
	cv::Mat rightImage(image2);
	int bias = 20;

	int row = leftImage.rows > rightImage.rows ? leftImage.rows : rightImage.rows;
	int col = leftImage.cols + rightImage.cols;
	col += bias;

	cv::Mat doubleImage(row, col, leftImage.type());

	leftImage.copyTo(doubleImage(cv::Rect(0,0, leftImage.cols, leftImage.rows)));
	rightImage.copyTo(doubleImage(cv::Rect(leftImage.cols + bias, 0, rightImage.cols, rightImage.rows)));

	cv::imshow("Result",doubleImage);
}
/*
		1、缩小分辨率，计算出缩小图像的特征点;
		2、放大分辨率，特征点位置映射回源图像;
		3、在源图像做如下匹配算法：
				(1)根据X、Y值做粗略匹配;
				(2)对粗略匹配结果进行聚类分析;
				(3)对粗略匹配结果进行分类：
				可能有用的方法：（根据X[最小值---->最大值],Y[最小值---->最大值]算出来粗略矩阵表示该物体）
					a、X、Y相同，表示该特征物体没有变动;
					b、X、Y不相同，表示该特征物体变动了;
*/
int main()
{
	IplImage* I_inOriginOne = cvLoadImage( fileOne);
	Corners cornersOne(I_inOriginOne);
	processImage(I_inOriginOne, cornersOne);

	IplImage* I_inOriginTwo = cvLoadImage( fileTwo);
	Corners cornersTwo(I_inOriginTwo);
	processImage(I_inOriginTwo, cornersTwo);

	matchCorners(I_inOriginOne, cornersOne, I_inOriginTwo, cornersTwo);

	//cornersOne.reduceUselessCorners();
	//cornersTwo.reduceUselessCorners();

	drawCorners(I_inOriginOne, cornersOne);
	drawCorners(I_inOriginTwo, cornersTwo);

	drawRect(I_inOriginOne, cornersOne);
	drawRect(I_inOriginTwo, cornersTwo);

	//matchRect(I_inOriginOne, cornersOne, I_inOriginTwo, cornersTwo);

	mergeImage(I_inOriginOne, I_inOriginTwo);

	cvWaitKey(0);

	cvDestroyAllWindows();
	cvReleaseImage(&I_inOriginOne);
	cvReleaseImage(&I_inOriginTwo);

	return 0;
}
