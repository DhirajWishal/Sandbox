#pragma once
#include <chrono>
#include <fstream>

/**
 * Timer class.
 * This calculates the total time taken from creation till the termination of the object.
 */
class Timer {
public:
	/**
	 * Default contructor.
	 */
	Timer(std::ofstream* pFile = nullptr) : pFile(pFile)
	{
		// Set the time point at the creation of the object.
		startPoint = std::chrono::high_resolution_clock::now();
	}

	/**
	 * Default destructor.
	 */
	~Timer()
	{
		// Get the time point of the time of the object's termination.
		auto endPoint = std::chrono::high_resolution_clock::now();

		// Convert time points.
		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(startPoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endPoint).time_since_epoch().count();

		// Print the time to the console.
		if (pFile)
			(*pFile) << static_cast<__int64>(end - start) << "\n";
		else
			printf("Time taken: %15i64\n", static_cast<__int64>(end - start));
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> startPoint;	// The start time point.
	std::ofstream* pFile = nullptr;
};

struct Consts {
	static const int x;
};