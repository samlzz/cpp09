/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 15:50:04 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/24 16:10:23 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <stdint.h>

uint32_t	parsePositiveInt(const char* s)
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


