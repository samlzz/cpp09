/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 14:27:25 by sliziard          #+#    #+#             */
/*   Updated: 2026/03/01 16:13:21 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PMergeMe.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>
#include <deque>
#include <ctime>
#include <stdint.h>

// ============================================================================
// Helpers
// ============================================================================

// ---- Parsing ----

static uint32_t	_parsePositiveInt(const char* s)
{
	if (!s || !*s)
		throw std::runtime_error("empty or null input");

	for (size_t i = 0; s[i]; ++i)
	{
		if (!std::isdigit(static_cast<uint8_t>(s[i])))
			throw std::runtime_error("invalid character in input");
	}

	long value = std::strtol(s, NULL, 10);
	if (value < 0 || value > std::numeric_limits<uint32_t>::max())
		throw std::runtime_error("value out of range");

	return static_cast<uint32_t>(value);
}

static inline void	_parseArgs(
						int ac, char **av,
						std::vector<uint32_t> &vec,
						std::deque<uint32_t> &deq
)
{
	vec.resize(ac - 1);
	deq.resize(ac - 1);
	for (int32_t i = 1; i < ac; ++i)
	{
		uint32_t value = _parsePositiveInt(av[i]);
		vec[i - 1] = value;
		deq[i - 1] = value;
	}
}

// ---- Printing ----

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
