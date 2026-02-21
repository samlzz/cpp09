/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 07:13:21 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/21 22:08:35 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __BITCOIN_EXCHANGE_HPP__
# define __BITCOIN_EXCHANGE_HPP__

# include <exception>
# include <utility>
# include <iostream>
# include <map>
# include <ostream>
# include <stdexcept>
# include <string>

# ifndef BE_MIN_RATE_VALUE
#  define BE_MIN_RATE_VALUE		0
# endif
# ifndef BE_MAX_RATE_VALUE
#  define BE_MAX_RATE_VALUE		1000
# endif

class BitcoinExchange {

private:
	std::map<std::string, double>	_db;

public:

	// ========================================================================
	// Construction / Destruction
	// ========================================================================
	BitcoinExchange();
	BitcoinExchange(const std::string &dbPath);
	~BitcoinExchange();

	// ========================================================================
	// Methods
	// ========================================================================

	void		loadDatabase(const std::string &dbPath);
	void		processInput(const std::string &path,
								std::ostream &os = std::cout) const;

private:
	double		getRateForDate(const std::string &date) const;

	static std::pair<std::string, double>
				parseSingleLine(const std::string &line,
								const std::string &sep,
								bool checkMax = true);

	static bool	isValidDate(const std::string &date);
	static bool	isValidValue(const std::string &value,
								double &out, bool checkMax);

	// ? Forbidden
	BitcoinExchange(const BitcoinExchange &other);
	BitcoinExchange& operator=(const BitcoinExchange &other);

public:
	// ========================================================================
	// Exceptions
	// ========================================================================

	class FileOpenException : public std::exception {
	public:
		virtual const char* what() const throw();
	};

	class EntryException : public std::exception {
	public:
		virtual const char* what() const throw();
	};

	class BadInputException : public std::runtime_error {
	public:
		BadInputException(const std::string &line);
	};

	class NegativeValueException : public std::exception {
	public:
		virtual const char* what() const throw();
	};

	class TooLargeValueException : public std::exception {
	public:
		virtual const char* what() const throw();
	};

	class NoRateFoundException : public std::exception {
	public:
		virtual const char* what() const throw();
	};

};


#endif /* __BITCOINEXCHANGE_HPP__ */
