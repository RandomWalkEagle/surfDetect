#pragma once
#include <ctime>
#include <iostream>
#include <windows.h>

class calTime
{
public:
	calTime(const std::string name){
		LARGE_INTEGER llPerfCount = {0};
		QueryPerformanceCounter(&llPerfCount);
		m_llBeginPerfCount = llPerfCount.QuadPart;
	}
	~calTime(){
		LARGE_INTEGER llPerfCount = {0};
		QueryPerformanceCounter(&llPerfCount);
		m_llEndPerfCount = llPerfCount.QuadPart;

		LARGE_INTEGER liPerfFreq={0};
		QueryPerformanceFrequency(&liPerfFreq);

		std::cout << fucName << " excute time: " 
		<<  float(m_llEndPerfCount - m_llBeginPerfCount) * 1000 / liPerfFreq.QuadPart  << " millisecond(ºÁÃë)" << std::endl;
	}

private:
	__int64 m_llBeginPerfCount;
	__int64 m_llEndPerfCount;
	std::string fucName;
};