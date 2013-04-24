#pragma once

#include "drawCorner.h"
#include "corner.h"
#include "../calTime.h"

const int KREGION = 4;//change

bool matchPixel(const CvScalar& one, const CvScalar& two)
{
	return one.val[0] == two.val[0] && one.val[1] == two.val[1]
	&& one.val[2] == two.val[2] && one.val[3] == two.val[3];
}

bool matchCorner(const std::vector<CvScalar>& one,const std::vector<CvScalar>& two)
{
	int size = one.size() < two.size() ? one.size() : two.size();
	if (size == 0 )
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

void getMinAndMax(Corner& corner, int& minX, int& minY, int& maxX, int& maxY)
{
	CvPoint pos = corner.getCornerPos();
	if (pos.x < minX)
	{
		minX = pos.x;
	}

	if (pos.y < minY)
	{
		minY = pos.y;
	}

	if (pos.x > maxX)
	{
		maxX = pos.x;
	}

	if (pos.y > maxY)
	{
		maxY = pos.y;
	}
}

void matchCorners(const IplImage* imageOne, Corners& cornersOne,
									const IplImage* imageTwo, Corners& cornersTwo)
{
	calTime cal(std::string(__FUNCTION__));

	int minOneX = imageOne->width;
	int minOneY = imageOne->height;
	int maxOneX = 0, maxOneY = 0;

	int minTwoX = imageTwo->width;
	int minTwoY = imageTwo->height;
	int maxTwoX = 0, maxTwoY = 0;

	int numCornersOne = cornersOne.getCornerSize();
	int numCornersTwo = cornersTwo.getCornerSize();
	for (int i = 0; i < numCornersOne; ++i)
	{
		Corner& cornerOne = cornersOne.getCorner(i);
		for (int j = 0; j < numCornersTwo; ++j)
		{
			Corner& cornerTwo = cornersTwo.getCorner(j);

			if ( !cornerOne.isMatched() && !cornerTwo.isMatched()
				&& cornerOne.getCornerPos().x == cornerTwo.getCornerPos().x
				&& matchPixel(cornerOne.getCornerPixel(), cornerTwo.getCornerPixel())) // both on the same x axis
			{
				std::vector<CvScalar>& one = cornerOne.getAdjacentPixels(imageOne, KREGION);
				std::vector<CvScalar>& two = cornerTwo.getAdjacentPixels(imageTwo, KREGION);

				if (matchCorner(one, two))
				{
					if (cornerOne.getCornerPos().y == cornerTwo.getCornerPos().y)
					{
						cornerOne.setMatchingStatus(Corner::Matched_XY);
						cornerTwo.setMatchingStatus(Corner::Matched_XY);
					}
					else
					{
						cornerOne.setMatchingStatus(Corner::Matched_X);
						cornerTwo.setMatchingStatus(Corner::Matched_X);
						getMinAndMax(cornerOne, minOneX, minOneY, maxOneX, maxOneY);
						getMinAndMax(cornerTwo, minTwoX, minTwoY, maxTwoX, maxTwoY);
					}
				}
			}

			if ( !cornerOne.isMatched() && !cornerTwo.isMatched()
				&& cornerOne.getCornerPos().y == cornerTwo.getCornerPos().y
				&& matchPixel(cornerOne.getCornerPixel(), cornerTwo.getCornerPixel())) // both on the same y axis
			{
				std::vector<CvScalar>& one = cornerOne.getAdjacentPixels(imageOne, KREGION);
				std::vector<CvScalar>& two = cornerTwo.getAdjacentPixels(imageTwo, KREGION);

				if (matchCorner(one, two))
				{
					cornerOne.setMatchingStatus(Corner::Matched_Y);
					cornerTwo.setMatchingStatus(Corner::Matched_Y);

					getMinAndMax(cornerOne, minOneX, minOneY, maxOneX, maxOneY);
					getMinAndMax(cornerTwo, minTwoX, minTwoY, maxTwoX, maxTwoY);
				}
			}
		}
	}

	CvRect rect;
	//minOneY -= 20;
	rect.x = minOneX;
	rect.y = minOneY;
	rect.width = maxOneX - minOneX;
	rect.height= maxOneY - minOneY;
	cornersOne.setSimilarRect(rect);

	//maxTwoY += 20;
	rect.x = minTwoX;
	rect.y = minTwoY;
	rect.width = maxTwoX - minTwoX;
	rect.height= maxTwoY - minTwoY;
	cornersTwo.setSimilarRect(rect);
}

void matchRect(IplImage* imageOne, Corners& cornersOne,
			   IplImage* imageTwo, Corners& cornersTwo)
{
	CvRect rect = cornersOne.getSimilarRect();
	if (rect.width <= 0 )
	{
		return;
	}
	
	cvSetImageROI(imageOne, rect);
	IplImage* tempImageOne = cvCreateImage(cvSize(rect.width, rect.height),imageOne->depth,imageOne->nChannels);
	cvCopy(imageOne,tempImageOne);
	cvResetImageROI(imageOne);

	rect = cornersTwo.getSimilarRect();
	cvSetImageROI(imageTwo, rect);
	IplImage* tempImageTwo = cvCreateImage(cvSize(rect.width, rect.height),imageTwo->depth,imageTwo->nChannels);
	cvCopy(imageTwo,tempImageTwo);
	cvResetImageROI(imageTwo);

	IplImage* dstImage = cvCreateImage(cvSize(rect.width, rect.height),imageOne->depth,imageOne->nChannels);;
	cvSub(tempImageOne, tempImageTwo, dstImage);

	cvNamedWindow("Result", CV_WINDOW_AUTOSIZE); 
	cvShowImage("Result", dstImage);
}