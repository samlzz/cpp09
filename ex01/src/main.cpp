/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 18:26:04 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/23 18:30:05 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <expression>" << std::endl;
		return 1;
	}

	try
	{
		RPN rpn;
		int32_t result = rpn.evaluate(argv[1]);
		std::cout << result << std::endl;
	}
	catch (const std::exception &e)
	{
#ifdef STRICT_SUBJECT_OUTPUT
		std::cerr << "Error" << std::endl;
#else
		std::cerr << "Error: " << e.what() << std::endl;
#endif
		return 1;
	}

	return 0;
}
