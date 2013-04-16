#pragma once
#include <ctime>
#include <iostream>

class calTime
{
public:
	calTime(const std::string name){
		start = clock();
		fucName = name;
	}
	~calTime(){
		std::cout << fucName << " excute time: " 
		<<  float(clock() - start) / CLOCKS_PER_SEC  << " seconds" << std::endl;
	}

private:
	clock_t start;
	std::string fucName;
};