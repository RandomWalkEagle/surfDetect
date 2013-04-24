#include "corner.h"

//----------------------------------------------------------------------------------------------------------------------------------
/*Corner*/

Corner::Corner( const IplImage* image, const CvPoint& pos )
{
	m_pos = pos;
	setCornerPixel(image, pos);
	m_isMatched = Matched_No;
}

Corner::~Corner()
{
	m_adjacentPixels.clear();
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
//	int (*direction)[2] = new int[sizeRegion][2];

	//int regionX = 0;
	//for (int i = 0; i < sizeRegion; ++i)
	//{
	//	direction[i][0] = regionX/Region - Region/2;
	//	direction[i][1] = regionX%Region - Region/2;

	//	regionX++;
	//}

	//simulate FAST squares
	sizeRegion = 16;
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
			m_adjacentPixels.push_back(cvGet2D(image, m_pos.y + direction[i][1], m_pos.x + direction[i][0]));
		}
	}

	//delete []direction;
	return m_adjacentPixels;
}

//----------------------------------------------------------------------------------------------------------------------------------
/*Corners*/

Corners::Corners( IplImage* image )
{
	m_imagePtr = image;
	m_num = 0;
	m_rect = cvRect(0,0,0,0);
}

Corners::~Corners()
{
	m_corners.clear();
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
	 std::vector<Corner>::iterator iter;
	for (iter = m_corners.begin(); iter != m_corners.end();)
	{
		if( !iter->isMatched() )
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

void Corners::setSimilarRect( const CvRect& rect )
{
	m_rect.x = rect.x;
	m_rect.y = rect.y;
	m_rect.height = rect.height;
	m_rect.width = rect.width;
}

CvRect Corners::getSimilarRect()
{
	return m_rect;
}
