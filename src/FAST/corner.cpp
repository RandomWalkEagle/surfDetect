#include "corner.h"
#include "../calTime.h"
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------------------
/*Corner*/

Corner::Corner( const IplImage* image, const CvPoint& pos )
{
	m_pos = pos;
	setCornerPixel(image, pos);
	m_isMatched = Matched_No;
	m_shift = 0;
	m_description= 0;
}

Corner::~Corner()
{
	m_adjacentPixels.clear();
	m_adjacentCirclePixels.clear();
}

void Corner::setCornerPixel( const IplImage* image, const CvPoint& pos )
{
	m_pixel = cvGet2D(image, pos.y, pos.x);
}

CvPoint Corner::getCornerPos()
{
	return m_pos;
}

CvScalar Corner::getCornerPixel()
{
	return m_pixel;
}

bool Corner::isMatched()
{
	return m_isMatched != Matched_No;
}

void Corner::setMatchingStatus( matchStatus status )
{
	m_isMatched = status;
}

Corner::matchStatus Corner::getMatchingStatus()
{
	return m_isMatched;
}

std::vector<CvScalar>& Corner::getAdjacentPixels(const IplImage* image,const int& Region )
{
	if (!m_adjacentPixels.empty())
	{
		return m_adjacentPixels;
	}

	m_adjacentPixels.clear();

	int sizeRegion = Region * Region;
	int (*direction)[2] = new int[sizeRegion][2];

	int regionX = 0;
	for (int i = 0; i < sizeRegion; ++i)
	{
		direction[i][0] = regionX/Region - Region/2;
		direction[i][1] = regionX%Region - Region/2;

		regionX++;
	}

	for(int i=0; i < sizeRegion; i++)
	{
		if ( m_pos.y + direction[i][1] >= 0 && m_pos.y + direction[i][1] < image->height
			&&  m_pos.x + direction[i][0] >= 0 && m_pos.x + direction[i][0] < image->width)
		{
			m_adjacentPixels.push_back(cvGet2D(image, m_pos.y + direction[i][1], m_pos.x + direction[i][0]));
		}
	}

	delete []direction;
	return m_adjacentPixels;
}

void Corner::setShift( int shift )
{
	m_shift = shift;
}

int Corner::getShift()
{
	return m_shift;
}

void Corner::setDescription( int description )
{
	m_description = description;
}

int Corner::getDescription()
{
	return m_description;
}

std::vector<CvScalar>& Corner::getAdjacentCirclePixels( const IplImage* image)
{
	if (!m_adjacentCirclePixels.empty())
	{
		return m_adjacentCirclePixels;
	}

	m_adjacentCirclePixels.clear();

	int sizeRegion = 16;
	int direction[16][2] = 
	{
		(-2,-2),(-1,-3),(0,-3),(1,-3),
		(2,-2),(3,-1),(3,0),(3,1),
		(2,2),(1,3),(0,3),(-1,3),
		(-2,2),(-3,1),(-3,0),(-3,-1)
	};

	for(int i=0; i < sizeRegion; i++)
	{
		if ( m_pos.y + direction[i][1] >= 0 && m_pos.y + direction[i][1] < image->height
			&&  m_pos.x + direction[i][0] >= 0 && m_pos.x + direction[i][0] < image->width)
		{
			m_adjacentCirclePixels.push_back(cvGet2D(image, m_pos.y + direction[i][1], m_pos.x + direction[i][0]));
		}
	}

	return m_adjacentCirclePixels;
}


//----------------------------------------------------------------------------------------------------------------------------------
/*Corners*/

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

Corners::Corners( IplImage* image )
{
	m_imagePtr = image;
	m_num = 0;
	m_rect = cvRect(0,0,0,0);
}

Corners::~Corners()
{
	m_corners.clear();
	m_shiftX.clear();
	m_shiftY.clear();
}

void Corners::pushCorner( const CvPoint& cornerPos )
{
	if (m_imagePtr == NULL)
	{
		return;
	}

	m_corners.push_back(Corner(m_imagePtr, cornerPos));
	++m_num;
}

Corner& Corners::getCorner( int pos )
{
	assert(pos >= 0 && pos < m_num);

	return m_corners[pos];
}

void Corners::reduceUselessCorners()
{
	calTime cal(std::string(__FUNCTION__));

	 std::vector<Corner>::iterator iter;
	for (iter = m_corners.begin(); iter != m_corners.end();)
	{
		if( /*!iter->isMatched()*/iter->getDescription() == 0 )
		{
			iter = m_corners.erase(iter);
			--m_num;
		}
		else
		{
			++iter;
		}
	}
}

void Corners::calculateSimilarRectBasedOnDescription(const int& description)
{
	std::cout << description << "\n";
	int minX = 0xFFFF;
	int minY = 0xFFFF;
	int maxX = 0;
	int maxY = 0;
	std::vector<Corner>::iterator iter;
	for (iter = m_corners.begin(); iter != m_corners.end(); ++iter)
	{
		if( iter->getDescription() == description )
		{
			getMinAndMax(*iter, minX, minY, maxX, maxY);
		}
	}

	m_rect = cvRect(minX, minY, maxX - minX, maxY - minY);
}

void Corners::calculateSimilarRect()
{
	calTime cal(std::string(__FUNCTION__));

	int bestDescription = 0;
	int counts = 0;
	std::vector<std::pair<Corner, int>>::iterator iter;
	for (iter = m_shiftX.begin(); iter != m_shiftX.end(); ++iter)
	{
		if ( iter->second > counts)
		{
			bestDescription = iter->first.getDescription();
			counts = iter->second;
		}
	}
	calculateSimilarRectBasedOnDescription(bestDescription);

	bestDescription = 0;
	counts = 0;
	for (iter = m_shiftY.begin(); iter != m_shiftY.end(); ++iter)
	{
		if ( iter->second > counts)
		{
			bestDescription = iter->first.getDescription();
			counts = iter->second;
		}
	}
	calculateSimilarRectBasedOnDescription(bestDescription);
}

CvRect Corners::getSimilarRect()
{
	return m_rect;
}

void Corners::pushShift( const shiftType& type , const Corner& shiftCorner )
{
	switch (type)
	{
	case Shift_X:
		m_shiftX.push_back(std::make_pair(shiftCorner, 1));
		break;
	case Shift_Y:
		m_shiftY.push_back(std::make_pair(shiftCorner, 1));
		break;
	default:
		break;
	}
}

int Corners::findDescription( const shiftType& type , const int& shift )
{
	switch (type)
	{
	case Shift_X:
		{
			std::vector<std::pair<Corner, int>>::iterator iter;
			for (iter = m_shiftX.begin(); iter != m_shiftX.end(); ++iter)
			if (iter->first.getShift() == shift)
			{
				return iter->first.getDescription();
			}
			break;
		}
	case Shift_Y:
		{
			std::vector<std::pair<Corner, int>>::iterator iter;
			for (iter = m_shiftY.begin(); iter != m_shiftY.end(); ++iter)
				if (iter->first.getShift() == shift)
				{
					return iter->first.getDescription();
				}
			break;
		}
	default:
		break;
	}

	return 0;
}

void Corners::addShiftCounts( const shiftType& type , const int& shift )
{
	switch (type)
	{
	case Shift_X:
		{
			std::vector<std::pair<Corner, int>>::iterator iter;
			for (iter = m_shiftX.begin(); iter != m_shiftX.end(); ++iter)
				if (iter->first.getShift() == shift)
				{
					iter->second++;
				}
				break;
		}
	case Shift_Y:
		{
			std::vector<std::pair<Corner, int>>::iterator iter;
			for (iter = m_shiftY.begin(); iter != m_shiftY.end(); ++iter)
				if (iter->first.getShift() == shift)
				{
					iter->second++;
				}
				break;
		}
	default:
		break;
	}
}
