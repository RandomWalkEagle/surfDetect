#include <cvaux.h>
#include <highgui.h>
#include <iostream>
#include <vector>

#include "../calTime.h"
#include "cvfast.h"

const float KSCALE = 0.03;//change

enum MatchType{
	XMatch = 0,
	YMatch = 1,
};

IplImage *scaleImage(IplImage* image)
{
	CvSize size = cvGetSize( image );
	size.width = size.width * KSCALE;
	size.height = size.height * KSCALE;
	IplImage *imageAfterScale= cvCreateImage(size, image->depth, image->nChannels);
	cvResize(image, imageAfterScale, CV_INTER_LINEAR);
	return imageAfterScale;
}

void showImage(IplImage* image)
{
	cvNamedWindow("FAST", CV_WINDOW_AUTOSIZE); 
	cvShowImage("FAST", image);
}

IplImage* processImage(const char* fileName, CvPoint** corners, int* numCorners)
{
	int inFASTThreshhold = 190;//·¶Î§[0-250]//change
	int inNpixels = 9;
	int inNonMaxSuppression  = 0;

	IplImage* I_inOrigin = cvLoadImage( fileName, -1 );
	IplImage *I_inScale = scaleImage(I_inOrigin);
	cvReleaseImage(&I_inOrigin);

	IplImage* IGray = cvCreateImage( cvGetSize( I_inScale ), IPL_DEPTH_8U, 1);
	cvCvtColor( I_inScale, IGray, CV_RGB2GRAY );

	cvCornerFast(IGray, inFASTThreshhold, inNpixels, inNonMaxSuppression, numCorners, corners);
	std::cout << " number of corners found in " << fileName << " are " << *numCorners << "\n\n";

	//cvCvtColor( IGray, I_inScale, CV_GRAY2RGB );
	cvReleaseImage(&IGray);

	return I_inScale;
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

void drawCorners(IplImage* image, CvPoint* corners,const int numCorners, MatchType type = YMatch)
{
	for(int i = 0; i < numCorners; i++ ) 
	{
		cvLine( image, 
			cvPoint( corners[i].x-1, corners[i].y ), 
			cvPoint( corners[i].x+1, corners[i].y ), 
			type == YMatch ? CV_RGB(255,0,0) : CV_RGB(255,0,255) );
		cvLine( image, 
			cvPoint( corners[i].x, corners[i].y-1 ), 
			cvPoint( corners[i].x, corners[i].y+1 ), 
			type == YMatch ? CV_RGB(255,0,0) : CV_RGB(255,0,255) );
	}
}

void drawCorners(IplImage* image, std::vector<CvPoint> corners, MatchType type)
{
	for(int i = 0; i < corners.size(); ++i )
	{
		cvCircle(image, corners[i], 20.0,
			type == YMatch ? CV_RGB(255,0,0) : CV_RGB(0,0,255) );
	}
}

void getAreaPixel(std::vector<CvScalar>& total, IplImage* image, CvPoint* corners, const CvPoint center)
{
	int region = 3;////change
	int sizeRegion = region * region;
	//int (*direction)[2] = new int[sizeRegion][2];

	//int regionX = 0;
	//for (int i = 0; i < sizeRegion; ++i)
	//{
	//	direction[i][0] = regionX/region - region/2;
	//	direction[i][1] = regionX%region - region/2;

	//	regionX++;
	//}
	int direction[25][2] = {
		{-2,-2},{-2,-1},{-2,0},{-2,1},{-2,2},
		{-1,-2},{-1,-1},{-1,0},{-1,1},{-1,2},
		{0,-2},{0,-1},{0,0},{0,1},{0,2},
		{1,-2},{1,-1},{1,0},{1,1},{1,2},
		{2,-2},{2,-1},{2,0},{2,1},{2,2},
	};

	for(int i=0; i < sizeRegion; i++)
	{
		if ( center.y - direction[i][1] >= 0 && center.y - direction[i][1] < image->height
			&&  center.x - direction[i][0] >= 0 && center.x - direction[i][0] < image->width)
		{
			total.push_back(cvGet2D(image, center.y - direction[i][0], center.x - direction[i][1]));
		}
	}

	//delete []direction;
}

bool matchPixel(const CvScalar one, const CvScalar two)
{
	//double sum = 0.0;
	//for (int i = 0; i < 4; ++i)
	//{
	//	sum += (one.val[i] - two.val[i])*(one.val[i] - two.val[i]);
	//}
	//return sum;
	return one.val[0] == two.val[0] && one.val[1] == two.val[1]
	&& one.val[2] == two.val[2] && one.val[3] == two.val[3];
}

bool matchCorner(const std::vector<CvScalar> one,const std::vector<CvScalar> two)
{
	int size = one.size();
	if (size != two.size() || size == 0)
	{
		return false;
	}

	for (int i = 0; i < size; i++)
	{
		if (!matchPixel(one[i], two[i]))
		{
			return false;
		}
	}

	return true;
}

void matchCorners(IplImage* imageOne, CvPoint* cornersOne, const int numCornersOne
				  ,IplImage* imageTwo, CvPoint* cornersTwo, const int numCornersTwo)
{
	calTime cal(std::string(__FUNCTION__));
	std::vector<CvPoint> needDrawOnImageOneX;
	std::vector<CvPoint> needDrawOnImageTwoX;

	std::vector<CvPoint> needDrawOnImageOneY;
	std::vector<CvPoint> needDrawOnImageTwoY;

	for (int i = 0; i < numCornersOne; ++i)
	{
		CvScalar cornerOne = cvGet2D(imageOne, cornersOne[i].y, cornersOne[i].x);
		for (int j = 0; j < numCornersTwo; ++j)
		{
			CvScalar cornerTwo = cvGet2D(imageTwo, cornersTwo[j].y, cornersTwo[j].x);
			if (cornersOne[i].x == cornersTwo[j].x) // both on the same x axis
			{
				std::vector<CvScalar> one;
				getAreaPixel(one, imageOne, cornersOne, cornersOne[i]);

				std::vector<CvScalar> two;
				getAreaPixel(two, imageTwo, cornersTwo, cornersTwo[j]);

				if (matchCorner(one, two))
				{
					needDrawOnImageOneX.push_back(cornersOne[i]);
					needDrawOnImageTwoX.push_back(cornersTwo[j]);
					cornersTwo[j].x = 0;
					cornersTwo[j].y = 0;
				}
			}else if(cornersOne[i].y == cornersTwo[j].y) // both on the same y axis
			{
				std::vector<CvScalar> one;
				getAreaPixel(one, imageOne, cornersOne, cornersOne[i]);

				std::vector<CvScalar> two;
				getAreaPixel(two, imageTwo, cornersTwo, cornersTwo[j]);

				if (matchCorner(one, two))
				{
					needDrawOnImageOneY.push_back(cornersOne[i]);
					needDrawOnImageTwoY.push_back(cornersTwo[j]);
					cornersTwo[j].x = 0;
					cornersTwo[j].y = 0;
				}
			}
		}
	}

	drawCorners(imageOne, needDrawOnImageOneX, XMatch);
	drawCorners(imageTwo, needDrawOnImageTwoX, XMatch);

	drawCorners(imageOne, needDrawOnImageOneY, YMatch);
	drawCorners(imageTwo, needDrawOnImageTwoY, YMatch);
}

int main(int argc, char* argv[])
{
	const char fileNameOne[] = "../imgs/YY66.png";
	CvPoint* cornersOne;
	int numCornersOne;
	IplImage* imageOne = processImage(fileNameOne, &cornersOne, &numCornersOne);
	drawCorners(imageOne, cornersOne, numCornersOne);

	const char fileNameTwo[] = "../imgs/YY77.png";
	CvPoint* cornersTwo;
	int numCornersTwo;
	IplImage* imageTwo = processImage(fileNameTwo, &cornersTwo, &numCornersTwo);
	drawCorners(imageTwo, cornersTwo, numCornersTwo);

	matchCorners(imageOne, cornersOne, numCornersOne
								, imageTwo, cornersTwo, numCornersTwo);

	mergeImage(imageOne, imageTwo);

	cvWaitKey(0);

	cvDestroyAllWindows();
	cvReleaseImage(&imageOne);
	cvReleaseImage(&imageTwo);
	return 0;
}
