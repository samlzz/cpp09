/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 15:26:00 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/23 18:22:42 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __RPN_HPP__
# define __RPN_HPP__

# include <cstdint>
# include <list>
# include <stack>
# include <stdexcept>
# include <stdint.h>
# include <string>

# define RPN_OP_COUNT	4

class RPN {

private:
	typedef int32_t	(*OpFunc)(int32_t, int32_t);
	struct Operation
	{
		char	symbol;
		OpFunc	func;

	};

	std::stack<int32_t, std::list<int32_t> >	_lifo;
	static const Operation						_ops[RPN_OP_COUNT];

public:

	// ========================================================================
	// Construction / Destruction
	// ========================================================================
	RPN();
	RPN(const RPN &other);
	~RPN();

	// ========================================================================
	// Operators
	// ========================================================================
	RPN& operator=(const RPN &other);

	// ========================================================================
	// Public Method
	// ========================================================================
	int32_t			evaluate(const std::string &expr);
	void			clear(void);

	// ========================================================================
	// Exception
	// ========================================================================
	class InvalidOperator : public std::runtime_error {
	public:
		InvalidOperator(char op);
	};

	class InvalidExpression : public std::runtime_error {
	public:
		InvalidExpression(const std::string &msg);
	};

private:
	// ========================================================================
	// Static helpers
	// ========================================================================
	static OpFunc	getOp(char symbol);

	// --- Operations ----
	static int32_t	add(int32_t a, int32_t b);
	static int32_t	sub(int32_t a, int32_t b);
	static int32_t	mul(int32_t a, int32_t b);
	static int32_t	div(int32_t a, int32_t b);
};


#endif /* __RPN_HPP__ */
