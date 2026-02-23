/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 17:15:35 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/23 18:24:26 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"
#include <cctype>
#include <cstdint>
#include <list>
#include <stack>
#include <stdexcept>

// ============================================================================
// Construction / Destruction
// ============================================================================

const RPN::Operation RPN::_ops[RPN_OP_COUNT] =
{
	{'+', &RPN::add},
	{'-', &RPN::sub},
	{'*', &RPN::mul},
	{'/', &RPN::div}
};

RPN::RPN(): _lifo()
{}

RPN::RPN(const RPN &other): _lifo(other._lifo)
{}

RPN::~RPN()
{}

// ============================================================================
// Operators
// ============================================================================

RPN& RPN::operator=(const RPN &other)
{
	if (this != &other)
	{
		_lifo = other._lifo;
	}
	return *this;
}

// ============================================================================
// Methods
// ============================================================================

static inline int32_t	next(std::stack<int32_t, std::list<int32_t> > &st)
{
	if (st.empty())
		throw RPN::InvalidExpression("not enough operands");
	int32_t	x = st.top();
	st.pop();
	return x;
}

int32_t	RPN::evaluate(const std::string &expr)
{
	OpFunc	op;
	int32_t	a;
	int32_t	b;
	uint8_t	c;

	clear();
	for (size_t i = 0; i < expr.size(); ++i)
	{
		c = static_cast<uint8_t>(expr[i]);
		if (std::isdigit(c))
		{
			if (i + 1 < expr.size()
				&& std::isdigit(static_cast<uint8_t>(expr[i + 1]))
			)
				throw InvalidExpression("numbers must be single digits (0-9)");

			_lifo.push(expr[i] - '0');
			continue;
		}
		if (std::isspace(c))
			continue;

		op = getOp(expr[i]);
		if (!op)
			throw InvalidOperator(expr[i]);

		b = next(_lifo);
		a = next(_lifo);

		_lifo.push(op(a, b));
	}

	if (_lifo.size() != 1)
		throw InvalidExpression("invalid expression format");
	return next(_lifo);
}

void	RPN::clear(void)
{
	while (!_lifo.empty())
		_lifo.pop();
}

// ========================================================================
// Static helpers
// ========================================================================

RPN::OpFunc	RPN::getOp(char symbol)
{
	for (size_t i = 0; i < RPN_OP_COUNT; ++i)
	{
		if (_ops[i].symbol == symbol)
			return _ops[i].func;
	}
	return NULL;
}

// ---- Operations ----
int32_t	RPN::add(int32_t a, int32_t b) { return a + b; }
int32_t	RPN::sub(int32_t a, int32_t b) { return a - b; }
int32_t	RPN::mul(int32_t a, int32_t b) { return a * b; }
int32_t	RPN::div(int32_t a, int32_t b)
{
	if (b == 0)
		throw std::runtime_error("division by zero");
	return a / b;
}

// ========================================================================
// Exception
// ========================================================================

RPN::InvalidOperator::InvalidOperator(char op)
	: std::runtime_error(
		"invalid operator: '" + std::string(1, op) + '\''
	)
{}

RPN::InvalidExpression::InvalidExpression(const std::string &msg)
	: std::runtime_error(msg)
{}