#include "PmergeMe.hpp"

#include <iostream>
#include <vector>
#include <deque>
#include <ctime>
#include <stdint.h>

// ============================================================================
// Helpers
// ============================================================================

static inline void	_parseArgs(
						int ac, char **av,
						std::vector<uint32_t> &vec,
						std::deque<uint32_t> &deq
)
{
	for (int32_t i = 1; i < ac; ++i)
	{
		uint32_t value = parsePositiveInt(av[i]);
		vec.push_back(value);
		deq.push_back(value);
	}
}

static void	_printVec(const std::string &label, const std::vector<uint32_t> &vec)
{
	std::cout << label;

	for (size_t i = 0; i < vec.size(); ++i)
		std::cout << " " << vec[i];

	std::cout << std::endl;
}

static void	_printTime(const std::string &name, size_t elemCount, double elapsed)
{
	std::cout << "Time to process a range of "
				<< elemCount
				<< " elements with " << name << " : "
				<< elapsed << " us" << std::endl;
}

// ============================================================================
// Main function
// ============================================================================

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Error" << std::endl;
		return 1;
	}

	try
	{
		std::vector<uint32_t>	vec;
		std::deque<uint32_t>	deq;

		_parseArgs(argc, argv, vec, deq);
		_printVec("Before:", vec);

		std::vector<uint32_t>	sortedVec;
		double					timeVec = timedFJSort(vec, sortedVec);

		std::deque<uint32_t>	sortedDeq;
		double					timeDeq = timedFJSort(deq, sortedDeq);

		_printVec("After: ", sortedVec);
		_printTime("std::vector", vec.size(), timeVec);
		_printTime("std::deque ", deq.size(), timeDeq);
	}
	catch (const std::exception&)
	{
		std::cerr << "Error" << std::endl;
		return 1;
	}

	return 0;
}
