/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 08:14:18 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/21 21:57:24 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <map>
#include <string>
#include <stdint.h>

// ============================================================================
// Static Helpers
// ============================================================================

static std::string	_extract(const std::string &src, int (*until)(int))
{
	size_t start = 0;
	while (start < src.size() && until(src[start]))
		start++;

	size_t end = src.size();
	while (end > start && until(src[end - 1]))
		end--;

	return src.substr(start, end - start);
}

static inline std::string	_trim(const std::string& s)
{
	return _extract(s, std::isspace);
}

static inline void _openFile(std::ifstream &file, const std::string &path)
{
	file.open(path.c_str());
	if (!file.is_open())
		throw BitcoinExchange::FileOpenException();
}

// ============================================================================
// Construction / Destruction
// ============================================================================

BitcoinExchange::BitcoinExchange(): _db()
{}

BitcoinExchange::BitcoinExchange(const std::string &dbPath): _db()
{
	loadDatabase(dbPath);
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other): _db(other._db)
{}

BitcoinExchange::~BitcoinExchange()
{}

// ============================================================================
// Operators
// ============================================================================

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange &other)
{
	if (this != &other)
	{
		_db = other._db;
	}
	return *this;
}

// ============================================================================
// Public methods
// ============================================================================

void	BitcoinExchange::loadDatabase(const std::string &dbPath)
{
	std::ifstream					dbFile;
	std::string						line;

	_openFile(dbFile, dbPath);
	std::getline(dbFile, line);

	while (std::getline(dbFile, line))
	{
		std::pair<std::string, double> entry = parseSingleLine(line, ",");

		_db[entry.first] = entry.second;
	}
}

void	BitcoinExchange::processInput(const std::string &path, std::ostream &os) const
{
	std::ifstream	file;
	std::string		line;

	_openFile(file, path);
	std::getline(file, line);

	while (std::getline(file, line))
	{
		try
		{
			std::pair<std::string, double> entry =
				parseSingleLine(line, " | ");

			double rate = getRateForDate(entry.first);

			os << entry.first << " => " << entry.second
				<< " = " << entry.second * rate
				<< std::endl;
		}
		catch (std::exception &e)
		{
			os << "Error: " << e.what() << std::endl;
		}
	}
}

// ============================================================================
// Private methods
// ============================================================================

double	BitcoinExchange::getRateForDate(const std::string &date) const
{
	if (_db.empty())
		throw NoRateFoundException();

	std::map<std::string, double>::const_iterator it = _db.lower_bound(date);

	if (it != _db.end() && it->first == date)
		return it->second;
	if (it == _db.begin())
		throw NoRateFoundException();

	--it;
	return it->second;
}

// ---- Validators ----

bool	BitcoinExchange::isValidDate(const std::string &date)
{
	if (date.length() != 10)
		return false;

	if (date[4] != '-' || date[7] != '-')
		return false;
	for (size_t i = 0; i < date.length(); ++i)
	{
		if (i == 4 || i == 7)
			continue;
		if (!std::isdigit(date[i]))
			return false;
	}

	int32_t	month = std::atoi(date.substr(5,2).c_str());
	if (month < 1 || month > 12)
		return false;

	int32_t	day = std::atoi(date.substr(8,2).c_str());
	if (day < 1 || day > 31)
		return false;

	return true;
}

bool	BitcoinExchange::isValidValue(const std::string &value, double &out)
{
	char	*endptr = NULL;
	double	val = std::strtod(value.c_str(), &endptr);

	if (*endptr != '\0')
		return false;

	if (val < BE_MIN_RATE_VALUE)
		throw NegativeValueException();
	if (val > BE_MAX_RATE_VALUE)
		throw TooLargeValueException();

	out = val;
	return true;
}

// ---- Parsing ----

std::pair<std::string, double>
BitcoinExchange::parseSingleLine(const std::string &line, const std::string &sep)
{
	size_t	sepPos = line.find(sep);
	if (sepPos == std::string::npos)
		throw BadInputException(line);

	std::string	date = _trim(line.substr(0, sepPos));
	std::string	valueStr = _trim(line.substr(sepPos + sep.length()));

	double value;

	if (!isValidDate(date))
		throw BadInputException(line);

	if (!isValidValue(valueStr, value))
		throw BadInputException(line);

	return std::make_pair(date, value);
}

// ========================================================================
// Exceptions
// ========================================================================

// File open
const char* BitcoinExchange::FileOpenException::what() const throw()
{
	return "could not open file.";
}

// Bad input
BitcoinExchange::BadInputException::BadInputException(const std::string &line)
	: std::runtime_error("bad input => " + line)
{}

// Negative value
const char* BitcoinExchange::NegativeValueException::what() const throw()
{
	return "not a positive number.";
}

// Too large
const char* BitcoinExchange::TooLargeValueException::what() const throw()
{
	return "too large a number.";
}

// No rate
const char* BitcoinExchange::NoRateFoundException::what() const throw()
{
	return "no rate found for date.";
}
