#pragma once
#include <ctime>
#include <cstdlib>
#include <iostream>

struct record
{
	float i;
	float u;

	void randomValues (void)
	{
		u = static_cast <float> (rand ()) / (static_cast <float> (RAND_MAX / 100));
		i = static_cast <float> (rand ()) / (static_cast <float> (RAND_MAX / 100));
	}

	void typeValues (void)
	{
		std::cout << "Type in u value: ";
		std::cin >> u;
		std::cout << "Type in i value: ";
		std::cin >> i;
	}

	void printRecord (void)
	{
		std::cout << u / i << "\n";
	}

	bool compare (const record& another)
	{
		if (this->u / this->i > another.u / another.i)
			return true;
		return false;
	}
};