/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PMergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 13:48:15 by sliziard          #+#    #+#             */
/*   Updated: 2026/03/01 13:48:32 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cmath>
#include <cstddef>

// size_t	jacobsthal(size_t n)
// {
// 	if (n <= 1)
// 		return n;

// 	size_t	prev2 = 0;
// 	size_t	prev1 = 1;
// 	size_t	current = 0;

// 	for (size_t i = 2; i <= n; ++i)
// 	{
// 		current = prev1 + 2 * prev2;
// 		prev2 = prev1;
// 		prev1 = current;
// 	}
// 	return current;
// }

size_t	jacobsthal(size_t n)
{
	if (n <= 1)
		return n;
	return static_cast<size_t>(
		round((1ULL << (n + 1)) + (n % 2 ? -1 : 1)) / 3
	);
}
