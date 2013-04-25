#pragma once

#include "drawCorner.h"
#include "corner.h"
#include "../calTime.h"

const int KREGION = 5;//change

bool matchPixel(const CvScalar& one, const CvScalar& two)
{
	return one.val[0] == two.val[0] && one.val[1] == two.val[1]
	&& one.val[2] == two.val[2] && one.val[3] == two.val[3];
}

bool matchAdjacentPixels(const std::vector<CvScalar>& one,const std::vector<CvScalar>& two)
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

bool matchPreciseCorner(const IplImage* imageOne, Corner& cornerOne,
						const IplImage* imageTwo, Corner& cornerTwo)
{
	std::vector<CvScalar>& one = cornerOne.getAdjacentPixels(imageOne, KREGION);
	std::vector<CvScalar>& two = cornerTwo.getAdjacentPixels(imageTwo, KREGION);

	if (matchAdjacentPixels(one, two))
	{
		return true;
	}

	return false;
}

bool matchCorner(const IplImage* imageOne, Corner& cornerOne, Corners& cornersOne,
									const IplImage* imageTwo, Corner& cornerTwo, Corners& cornersTwo,
									int& description)
{
	if ( cornerOne.isMatched() || cornerTwo.isMatched() )
	{
		return false;
	}

	CvPoint posOne = cornerOne.getCornerPos();
	CvPoint posTwo = cornerTwo.getCornerPos();

	Corner::matchStatus status = (posOne.x == posTwo.x) ? Corner::Matched_X:
														(posOne.y == posTwo.y) ? Corner::Matched_Y : 
														Corner::Matched_No;

	if (status == Corner::Matched_No)
	{
		return false;
	}

	if ( status == Corner::Matched_X && posOne.y == posTwo.y )
	{
		cornerOne.setMatchingStatus(Corner::Matched_XY);
		cornerTwo.setMatchingStatus(Corner::Matched_XY);
		return false;
	}

	if ( !matchPixel(cornerOne.getCornerPixel(), cornerTwo.getCornerPixel()) )
	{
		return false;
	}

	std::vector<CvScalar>& one = cornerOne.getAdjacentCirclePixels(imageOne);
	std::vector<CvScalar>& two = cornerTwo.getAdjacentCirclePixels(imageTwo);

	if ( !matchAdjacentPixels(one, two) )
	{
		return false;
	}

	switch (status)
	{
	case Corner::Matched_X:
		{
			int shift = posOne.y - posTwo.y;
			int searchedDesc = cornersTwo.findDescription(Corners::Shift_Y,shift);
			if ( searchedDesc )
			{
				cornerOne.setDescription(searchedDesc);
				cornerTwo.setDescription(searchedDesc);
				cornersOne.addShiftCounts(Corners::Shift_X,shift);// can delete
				cornersTwo.addShiftCounts(Corners::Shift_Y,shift);
			}else if ( matchPreciseCorner(imageOne, cornerOne, imageTwo, cornerTwo) )
			{
				++description;
				//cornerOne.setShift(shift);
				cornerTwo.setShift(shift);

				cornerOne.setDescription(description);
				cornerTwo.setDescription(description);
				//std::cout << "match type: "<< description << "\n";
				cornersOne.pushShift(Corners::Shift_Y,cornerOne);// can delete
				cornersTwo.pushShift(Corners::Shift_Y,cornerTwo);
			}else// No match
			{
				return false;
			}
			cornerOne.setMatchingStatus(Corner::Matched_X);//information of One maybe can be destroyed.
			cornerTwo.setMatchingStatus(Corner::Matched_X);
			break;
		}
	case Corner::Matched_Y:
		{
			int shift = posOne.x - posTwo.x;
			int searchedDesc = cornersTwo.findDescription(Corners::Shift_X,shift);
			if ( searchedDesc )
			{
				cornerOne.setDescription(searchedDesc);
				cornerTwo.setDescription(searchedDesc);
				cornersOne.addShiftCounts(Corners::Shift_X,shift);// can delete
				cornersTwo.addShiftCounts(Corners::Shift_X,shift);
			}else if ( matchPreciseCorner(imageOne, cornerOne, imageTwo, cornerTwo) )
			{
				++description;
				//cornerOne.setShift(shift);
				cornerTwo.setShift(shift);

				cornerOne.setDescription(description);
				cornerTwo.setDescription(description);

				cornersOne.pushShift(Corners::Shift_X,cornerOne);// can delete
				cornersTwo.pushShift(Corners::Shift_X,cornerTwo);
			}else// No match
			{
				return false;
			}
			cornerOne.setMatchingStatus(Corner::Matched_X);//information of One maybe can be destroyed.
			cornerTwo.setMatchingStatus(Corner::Matched_X);
			break;
		}
	default:
		break;
	}

	return false;
}

void matchCorners(const IplImage* imageOne, Corners& cornersOne,
												const IplImage* imageTwo, Corners& cornersTwo)
{
	calTime cal(std::string(__FUNCTION__));

	int description = 0;

	int numCornersOne = cornersOne.getCornerSize();
	int numCornersTwo = cornersTwo.getCornerSize();
	for (int i = 0; i < numCornersOne; ++i)
	{
		Corner& cornerOne = cornersOne.getCorner(i);
		for (int j = 0; j < numCornersTwo; ++j)
		{
			Corner& cornerTwo = cornersTwo.getCorner(j);
			matchCorner(imageOne, cornerOne, cornersOne, 
									imageTwo, cornerTwo, cornersTwo,
									description );
		}
	}
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