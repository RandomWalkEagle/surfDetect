#pragma once

#include <iostream>
#include <vector>
#include "corner.h"

//void drawCorners(IplImage* image, CvPoint* corners,const int numCorners, MatchType type = YMatch)
//{
//	for(int i = 0; i < numCorners; i++ ) 
//	{
//		cvLine( image, 
//			cvPoint( corners[i].x-1, corners[i].y ), 
//			cvPoint( corners[i].x+1, corners[i].y ), 
//			type == YMatch ? CV_RGB(255,0,0) : CV_RGB(255,0,255) );
//		cvLine( image, 
//			cvPoint( corners[i].x, corners[i].y-1 ), 
//			cvPoint( corners[i].x, corners[i].y+1 ), 
//			type == YMatch ? CV_RGB(255,0,0) : CV_RGB(255,0,255) );
//	}
//}
//
//void drawCorners(IplImage* image, std::vector<CvPoint> corners, MatchType type)
//{
//	if (corners.empty())
//	{
//		std::cout << "No match happened in Corners. " << (type == YMatch ? "YMatch" : "XMatch") << "\n\n";
//	}else
//	{
//		std::cout << corners.size() <<" points matched in Corners. " << (type == YMatch ? "YMatch" : "XMatch") << "\n\n";
//	}
//
//
//	for(int i = 0; i < corners.size(); ++i )
//	{
//		cvLine( image, 
//			cvPoint( corners[i].x-1, corners[i].y ), 
//			cvPoint( corners[i].x+1, corners[i].y ), 
//			type == YMatch ? CV_RGB(255,0,0) : CV_RGB(0,0,255) );
//		cvLine( image, 
//			cvPoint( corners[i].x, corners[i].y-1 ), 
//			cvPoint( corners[i].x, corners[i].y+1 ), 
//			type == YMatch ? CV_RGB(255,0,0) : CV_RGB(0,0,255) );
//
//		if (type == YMatch)
//		{
//			cvCircle(image, corners[i], 15.0,
//				CV_RGB(255,0,0));
//		}else
//		{
//			cvCircle(image, corners[i], 20.0,
//				CV_RGB(0,0,255));
//		}
//	}
//}

void drawCorners(IplImage* image, Corners& corners)
{
	int countX = 0;
	int countY = 0;
	int countXY = 0;

	int size = corners.getCornerSize();
	for(int i = 0; i < size; ++i )
	{
		Corner corner = corners.getCorner(i);
		CvPoint pos = corner.getCornerPos();
		Corner::matchStatus status = corner.getMatchingStatus();
		cvLine( image,
			cvPoint( pos.x-1, pos.y ),
			cvPoint( pos.x+1, pos.y ),
			status == Corner::Matched_No ? CV_RGB(0,255,255) : 
			status == Corner::Matched_X ? CV_RGB(10*(corner.getDescription()%10+10),15*(corner.getDescription()%10+4),23*(corner.getDescription()%10+2)) : 
			status == Corner::Matched_Y ? CV_RGB(0,255,0) : CV_RGB(255,0,0),
			6);
		cvLine( image,
			cvPoint( pos.x, pos.y-1 ),
			cvPoint( pos.x, pos.y+1 ),
			status == Corner::Matched_No ? CV_RGB(0,255,255) : 
			status == Corner::Matched_X ? CV_RGB(10*(corner.getDescription()%10+10),15*(corner.getDescription()%10+4),23*(corner.getDescription()%10+2)) : 
			status == Corner::Matched_Y ? CV_RGB(0,255,0) : CV_RGB(255,0,0),
			6);

		if (status == Corner::Matched_Y)
		{
			cvCircle(image, pos, 15.0,
				CV_RGB(0,30*(corner.getDescription()%5+5),0));
			++countY;
		}else if (status == Corner::Matched_X)
		{
			cvCircle(image, pos, 20.0,
				CV_RGB(0,0,30*(corner.getDescription()%10+5)));
			++countX;
		}else if (status == Corner::Matched_XY)
		{
			cvCircle(image, pos, 25.0,
				CV_RGB(255,0,0));
			++countXY;
		}
		

	}

	if (countX == 0)
	{
		std::cout << "No match happened in Corners. " << "XMatch" << "\n\n";
	}else
	{
		std::cout << countX <<" points matched in Corners. " << "XMatch" << "\n\n";
	}

	if (countY == 0)
	{
		std::cout << "No match happened in Corners. " << "YMatch" << "\n\n";
	}else
	{
		std::cout << countY <<" points matched in Corners. " << "YMatch" << "\n\n";
	}

	if (countXY == 0)
	{
		std::cout << "No match happened in Corners. " << "XYMatch" << "\n\n";
	}else
	{
		std::cout << countXY <<" points matched in Corners. " << "XYMatch" << "\n\n";
	}
}

void drawRect(IplImage* image, Corners& corners)
{
	cvRectangleR(image, corners.getSimilarRect(), CV_RGB(255,0,0));
}